/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/rtp_rtcp/source/rtp_pt_manipulator_impl.h"

#include <cstring>

#include "modules/rtp_rtcp/source/rtp_packet.h"
#include "rtc_base/checks.h"

namespace webrtc {

RtpPtManipulatorImpl::RtpPtManipulatorImpl() {}

RtpPtManipulatorImpl::~RtpPtManipulatorImpl() {}

void RtpPtManipulatorImpl::ConfigureSdp(const SdpMediaDescription& sdp) {
  sdp_ = sdp;
}

RtpPayloadTypes RtpPtManipulatorImpl::ParsePtValues(const RtpPacket& packet) const {
  RtpPayloadTypes pt_values;

  uint8_t outer_pt = packet.PayloadType();

  if (IsRedPacket(outer_pt)) {
    pt_values.red_pt = outer_pt;
    // Parse the RED payload (skipping the 1-byte RED header).
    if (packet.payload_size() >= kRedHeaderSize) {
      ParseRedPayload(packet.payload().data(), packet.payload_size(), pt_values);
    }
  } else if (IsUlpfecPacket(outer_pt)) {
    pt_values.ulpfec_pt = outer_pt;
    ParseUlpfecPayload(packet.payload().data(), packet.payload_size(), pt_values);
  } else if (IsVp9Packet(outer_pt)) {
    pt_values.vp9_pt = outer_pt;
  } else if (IsRtxPacket(outer_pt)) {
    pt_values.rtx_pt = outer_pt;
  }

  return pt_values;
}

bool RtpPtManipulatorImpl::ModifyPtValues(RtpPacket* packet,
                                          const RtpPayloadTypes& new_pt_values) {
  if (!packet) {
    return false;
  }

  uint8_t current_pt = packet->PayloadType();

  // Modify outer PT using official API.
  if (IsRedPacket(current_pt) && new_pt_values.red_pt.has_value()) {
    packet->SetPayloadType(new_pt_values.red_pt.value());
    // Modify inner RED payload.
    if (packet->payload_size() >= kRedHeaderSize) {
      rtc::CopyOnWriteBuffer buffer = packet->Buffer();
      uint8_t* data = buffer.data();  // 使用 data() 获得可写指针
      size_t headers_size = packet->headers_size();
      ModifyRedPayload(data + headers_size, packet->payload_size(), new_pt_values);
      // Re-parse the modified buffer into the packet.
      if (!packet->Parse(buffer)) {
        return false;
      }
    }
  } else if (IsUlpfecPacket(current_pt) && new_pt_values.ulpfec_pt.has_value()) {
    packet->SetPayloadType(new_pt_values.ulpfec_pt.value());
    // Modify ULPFEC payload.
    if (packet->payload_size() >= kUlpfecLevel0HeaderSize) {
      rtc::CopyOnWriteBuffer buffer = packet->Buffer();
      uint8_t* data = buffer.data();  // 使用 data() 获得可写指针
      size_t headers_size = packet->headers_size();
      ModifyUlpfecPayload(data + headers_size, packet->payload_size(), new_pt_values);
      if (!packet->Parse(buffer)) {
        return false;
      }
    }
  } else if (IsVp9Packet(current_pt) && new_pt_values.vp9_pt.has_value()) {
    packet->SetPayloadType(new_pt_values.vp9_pt.value());
  } else if (IsRtxPacket(current_pt) && new_pt_values.rtx_pt.has_value()) {
    packet->SetPayloadType(new_pt_values.rtx_pt.value());
  } else {
    // No PT to modify for this packet type.
    return true;
  }

  return true;
}

bool RtpPtManipulatorImpl::VerifyModification(const RtpPacket& modified_packet,
                                              const RtpPayloadTypes& expected_pt_values) const {
  RtpPayloadTypes actual_pt_values = ParsePtValues(modified_packet);

  if (expected_pt_values.red_pt.has_value()) {
    if (!actual_pt_values.red_pt.has_value() ||
        *actual_pt_values.red_pt != *expected_pt_values.red_pt) {
      return false;
    }
  }
  if (expected_pt_values.ulpfec_pt.has_value()) {
    if (!actual_pt_values.ulpfec_pt.has_value() ||
        *actual_pt_values.ulpfec_pt != *expected_pt_values.ulpfec_pt) {
      return false;
    }
  }
  if (expected_pt_values.vp9_pt.has_value()) {
    if (!actual_pt_values.vp9_pt.has_value() ||
        *actual_pt_values.vp9_pt != *expected_pt_values.vp9_pt) {
      return false;
    }
  }
  if (expected_pt_values.rtx_pt.has_value()) {
    if (!actual_pt_values.rtx_pt.has_value() ||
        *actual_pt_values.rtx_pt != *expected_pt_values.rtx_pt) {
      return false;
    }
  }
  return true;
}

// RED payload format (WebRTC style):
//  0                   1
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |F|   block PT  |    data...    |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// F bit is always 0 in WebRTC (only one block).
bool RtpPtManipulatorImpl::ParseRedPayload(const uint8_t* payload,
                                           size_t payload_size,
                                           RtpPayloadTypes& pt_values) const {
  if (!payload || payload_size < kRedHeaderSize) {
    return false;
  }

  uint8_t first_byte = payload[0];
  bool more_blocks = (first_byte & 0x80) != 0;
  uint8_t inner_pt = first_byte & 0x7F;

  // Skip RED header.
  const uint8_t* inner_payload = payload + kRedHeaderSize;
  size_t inner_payload_size = payload_size - kRedHeaderSize;

  // If more blocks, WebRTC does not use them; we could recursively parse,
  // but for simplicity just stop.
  if (more_blocks) {
    // Not supported in WebRTC production; fall through.
    return false;
  }

  if (IsUlpfecPacket(inner_pt)) {
    pt_values.ulpfec_pt = inner_pt;
    return ParseUlpfecPayload(inner_payload, inner_payload_size, pt_values);
  } else if (IsVp9Packet(inner_pt)) {
    pt_values.vp9_pt = inner_pt;
    return true;
  } else if (IsRtxPacket(inner_pt)) {
    pt_values.rtx_pt = inner_pt;
    return true;
  } else if (IsRedPacket(inner_pt)) {
    // Nested RED (theoretically possible) – recurse.
    return ParseRedPayload(inner_payload, inner_payload_size, pt_values);
  }

  return false;
}

// ULPFEC Level 0 Header (10 bytes):
//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |E|L|P|X|  CC   |M| PT recovery |            SN base            |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                          TS recovery                          |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |        length recovery        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// PT recovery is bits 0-6 of byte 1 (i.e., 7 bits).
bool RtpPtManipulatorImpl::ParseUlpfecPayload(const uint8_t* payload,
                                              size_t payload_size,
                                              RtpPayloadTypes& pt_values) const {
  if (!payload || payload_size < kUlpfecLevel0HeaderSize) {
    return false;
  }

  uint8_t pt_recovery = payload[1] & 0x7F;  // 7-bit mask

  if (IsVp9Packet(pt_recovery)) {
    pt_values.vp9_pt = pt_recovery;
  } else if (IsRedPacket(pt_recovery)) {
    pt_values.red_pt = pt_recovery;
  } else if (IsRtxPacket(pt_recovery)) {
    pt_values.rtx_pt = pt_recovery;
  }
  // Note: ULPFEC normally protects a media packet (e.g., VP9).
  // It never protects another RED packet in WebRTC.

  return true;
}

bool RtpPtManipulatorImpl::ModifyRedPayload(uint8_t* payload,
                                            size_t payload_size,
                                            const RtpPayloadTypes& new_pt_values) const {
  if (!payload || payload_size < kRedHeaderSize) {
    return false;
  }

  uint8_t first_byte = payload[0];
  bool more_blocks = (first_byte & 0x80) != 0;
  uint8_t inner_pt = first_byte & 0x7F;

  const uint8_t* inner_payload = payload + kRedHeaderSize;
  size_t inner_payload_size = payload_size - kRedHeaderSize;

  if (more_blocks) {
    // Not expected in WebRTC; ignore.
    return false;
  }

  // Modify inner PT if needed.
  if (IsUlpfecPacket(inner_pt) && new_pt_values.ulpfec_pt.has_value()) {
    payload[0] = (first_byte & 0x80) | (new_pt_values.ulpfec_pt.value() & 0x7F);
    // Recurse into ULPFEC payload.
    if (inner_payload_size >= kUlpfecLevel0HeaderSize) {
      // Need mutable copy of inner payload – but we are already inside a mutable buffer.
      // Just call ModifyUlpfecPayload on the inner payload pointer.
      ModifyUlpfecPayload(const_cast<uint8_t*>(inner_payload), inner_payload_size, new_pt_values);
    }
  } else if (IsVp9Packet(inner_pt) && new_pt_values.vp9_pt.has_value()) {
    payload[0] = (first_byte & 0x80) | (new_pt_values.vp9_pt.value() & 0x7F);
  } else if (IsRtxPacket(inner_pt) && new_pt_values.rtx_pt.has_value()) {
    payload[0] = (first_byte & 0x80) | (new_pt_values.rtx_pt.value() & 0x7F);
  } else if (IsRedPacket(inner_pt) && new_pt_values.red_pt.has_value()) {
    payload[0] = (first_byte & 0x80) | (new_pt_values.red_pt.value() & 0x7F);
    // Recursively modify nested RED.
    ModifyRedPayload(const_cast<uint8_t*>(inner_payload), inner_payload_size, new_pt_values);
  }

  return true;
}

bool RtpPtManipulatorImpl::ModifyUlpfecPayload(uint8_t* payload,
                                               size_t payload_size,
                                               const RtpPayloadTypes& new_pt_values) const {
  if (!payload || payload_size < kUlpfecLevel0HeaderSize) {
    return false;
  }

  uint8_t current_pt_recovery = payload[1] & 0x7F;

  if (IsVp9Packet(current_pt_recovery) && new_pt_values.vp9_pt.has_value()) {
    payload[1] = (payload[1] & 0x80) | (new_pt_values.vp9_pt.value() & 0x7F);
  } else if (IsRedPacket(current_pt_recovery) && new_pt_values.red_pt.has_value()) {
    payload[1] = (payload[1] & 0x80) | (new_pt_values.red_pt.value() & 0x7F);
  } else if (IsRtxPacket(current_pt_recovery) && new_pt_values.rtx_pt.has_value()) {
    payload[1] = (payload[1] & 0x80) | (new_pt_values.rtx_pt.value() & 0x7F);
  }

  return true;
}

bool RtpPtManipulatorImpl::IsRedPacket(uint8_t pt) const {
  return sdp_.IsCodec(pt, "red");
}

bool RtpPtManipulatorImpl::IsUlpfecPacket(uint8_t pt) const {
  return sdp_.IsCodec(pt, "ulpfec");
}

bool RtpPtManipulatorImpl::IsVp9Packet(uint8_t pt) const {
  return sdp_.IsCodec(pt, "VP9");
}

bool RtpPtManipulatorImpl::IsRtxPacket(uint8_t pt) const {
  return sdp_.IsCodec(pt, "rtx");
}

}  // namespace webrtc