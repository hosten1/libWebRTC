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

#include "modules/rtp_rtcp/source/rtp_packet.h"

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
    ParseRedPayload(packet.payload().data(), packet.payload().size(), pt_values);
  } else if (IsUlpfecPacket(outer_pt)) {
    pt_values.ulpfec_pt = outer_pt;
    ParseUlpfecPayload(packet.payload().data(), packet.payload().size(), pt_values);
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
  
  rtc::CopyOnWriteBuffer buffer = packet->Buffer();
  uint8_t* data = const_cast<uint8_t*>(buffer.data());
  
  uint8_t current_pt = packet->PayloadType();
  const size_t kRtpHeaderPayloadTypeOffset = 1;
  
  // 修改外层 PT，保留 M 位（bit 7）
  if (IsRedPacket(current_pt) && new_pt_values.red_pt.has_value()) {
    data[kRtpHeaderPayloadTypeOffset] = 
        (data[kRtpHeaderPayloadTypeOffset] & 0x80) | (new_pt_values.red_pt.value() & 0x7F);
    ModifyRedPayload(data + packet->headers_size(), 
                     packet->payload_size(), new_pt_values);
  } else if (IsUlpfecPacket(current_pt) && new_pt_values.ulpfec_pt.has_value()) {
    data[kRtpHeaderPayloadTypeOffset] = 
        (data[kRtpHeaderPayloadTypeOffset] & 0x80) | (new_pt_values.ulpfec_pt.value() & 0x7F);
    ModifyUlpfecPayload(data + packet->headers_size(), 
                        packet->payload_size(), new_pt_values);
  } else if (IsVp9Packet(current_pt) && new_pt_values.vp9_pt.has_value()) {
    data[kRtpHeaderPayloadTypeOffset] = 
        (data[kRtpHeaderPayloadTypeOffset] & 0x80) | (new_pt_values.vp9_pt.value() & 0x7F);
  } else if (IsRtxPacket(current_pt) && new_pt_values.rtx_pt.has_value()) {
    data[kRtpHeaderPayloadTypeOffset] = 
        (data[kRtpHeaderPayloadTypeOffset] & 0x80) | (new_pt_values.rtx_pt.value() & 0x7F);
  }
  
  // 重新解析修改后的包
  *packet = RtpPacket();
  return packet->Parse(buffer);
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

bool RtpPtManipulatorImpl::ParseRedPayload(const uint8_t* payload, 
                                           size_t payload_size,
                                           RtpPayloadTypes& pt_values) const {
  if (!payload || payload_size == 0) {
    return false;
  }
  
  size_t offset = 0;
  
  while (offset < payload_size) {
    if (offset + 3 > payload_size) {
      break;
    }
    
    // RED 块第一个字节：bit7 表示最后一个块，bits 0-6 为负载类型
    uint8_t embedded_pt = payload[offset] & 0x7F;
    uint16_t block_length = (static_cast<uint16_t>(payload[offset + 1]) << 8) | 
                           payload[offset + 2];
    
    if (IsUlpfecPacket(embedded_pt)) {
      pt_values.ulpfec_pt = embedded_pt;
      ParseUlpfecPayload(payload + offset + 3, 
                         std::min(static_cast<size_t>(block_length), 
                                  payload_size - offset - 3), 
                         pt_values);
    } else if (IsVp9Packet(embedded_pt)) {
      pt_values.vp9_pt = embedded_pt;
    } else if (IsRtxPacket(embedded_pt)) {
      pt_values.rtx_pt = embedded_pt;
    }
    
    offset += 3 + block_length;
  }
  
  return true;
}

bool RtpPtManipulatorImpl::ParseUlpfecPayload(const uint8_t* payload,
                                              size_t payload_size,
                                              RtpPayloadTypes& pt_values) const {
  if (!payload || payload_size < 10) {
    return false;
  }
  
  // ULPFEC Level 0 Header (10 bytes):
  // Byte 1 (offset 1): 高3位保留，低5位为 PT recovery
  uint8_t pt_recovery = payload[1] & 0x1F;
  
  if (IsVp9Packet(pt_recovery)) {
    pt_values.vp9_pt = pt_recovery;
  } else if (IsRedPacket(pt_recovery)) {
    pt_values.red_pt = pt_recovery;
  }
  
  return true;
}

bool RtpPtManipulatorImpl::ModifyRedPayload(uint8_t* payload,
                                            size_t payload_size,
                                            const RtpPayloadTypes& new_pt_values) const {
  if (!payload || payload_size == 0) {
    return false;
  }
  
  size_t offset = 0;
  
  while (offset < payload_size) {
    if (offset + 3 > payload_size) {
      break;
    }
    
    uint8_t& embedded_pt_byte = payload[offset];
    uint8_t original_pt = embedded_pt_byte & 0x7F;
    uint16_t block_length = (static_cast<uint16_t>(payload[offset + 1]) << 8) | 
                           payload[offset + 2];
    
    if (IsUlpfecPacket(original_pt) && new_pt_values.ulpfec_pt.has_value()) {
      // 保留最高位的块结束标志，修改低7位
      embedded_pt_byte = (embedded_pt_byte & 0x80) | (new_pt_values.ulpfec_pt.value() & 0x7F);
      ModifyUlpfecPayload(payload + offset + 3, 
                          std::min(static_cast<size_t>(block_length), 
                                   payload_size - offset - 3), 
                          new_pt_values);
    } else if (IsVp9Packet(original_pt) && new_pt_values.vp9_pt.has_value()) {
      embedded_pt_byte = (embedded_pt_byte & 0x80) | (new_pt_values.vp9_pt.value() & 0x7F);
    } else if (IsRtxPacket(original_pt) && new_pt_values.rtx_pt.has_value()) {
      embedded_pt_byte = (embedded_pt_byte & 0x80) | (new_pt_values.rtx_pt.value() & 0x7F);
    }
    
    offset += 3 + block_length;
  }
  
  return true;
}

bool RtpPtManipulatorImpl::ModifyUlpfecPayload(uint8_t* payload,
                                               size_t payload_size,
                                               const RtpPayloadTypes& new_pt_values) const {
  if (!payload || payload_size < 10) {
    return false;
  }
  
  uint8_t current_pt_recovery = payload[1] & 0x1F;
  
  if (IsVp9Packet(current_pt_recovery) && new_pt_values.vp9_pt.has_value()) {
    payload[1] = (payload[1] & 0xE0) | (new_pt_values.vp9_pt.value() & 0x1F);
  } else if (IsRedPacket(current_pt_recovery) && new_pt_values.red_pt.has_value()) {
    payload[1] = (payload[1] & 0xE0) | (new_pt_values.red_pt.value() & 0x1F);
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