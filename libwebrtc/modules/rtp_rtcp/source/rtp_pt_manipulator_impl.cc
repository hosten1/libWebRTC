/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 */

#include "modules/rtp_rtcp/source/rtp_pt_manipulator_impl.h"

#include <cstring>

#include "modules/rtp_rtcp/source/rtp_packet.h"
#include "rtc_base/checks.h"

namespace webrtc {

static constexpr size_t kRedBlockHeaderSize = 4;

RtpPtManipulatorImpl::RtpPtManipulatorImpl() {}

RtpPtManipulatorImpl::~RtpPtManipulatorImpl() {}

void RtpPtManipulatorImpl::ConfigureSdp(const SdpMediaDescription& sdp) {
  sdp_ = sdp;
  red_pt_.reset();
  ulpfec_pt_.reset();
  vp9_pt_.reset();
  rtx_pt_.reset();
  for (const auto& kv : sdp.rtpmap) {
    const std::string& name = kv.second;
    if (name.find("red") == 0) red_pt_ = kv.first;
    else if (name.find("ulpfec") == 0) ulpfec_pt_ = kv.first;
    else if (name.find("VP9") == 0) vp9_pt_ = kv.first;
    else if (name.find("rtx") == 0) rtx_pt_ = kv.first;
  }
}

bool RtpPtManipulatorImpl::ParseRedBlocks(const uint8_t* payload,
                                          size_t payload_size,
                                          std::vector<RedBlock>& blocks) const {
  size_t offset = 0;
  while (offset + kRedBlockHeaderSize <= payload_size) {
    const uint8_t* header = payload + offset;
    bool is_last = (header[0] & 0x80) != 0;
    uint8_t pt = header[0] & 0x7F;
    uint16_t timestamp_offset = ((header[1] & 0x3F) << 8) | header[2];
    uint16_t block_length = ((header[1] & 0xC0) << 2) | header[3];
    size_t data_offset = offset + kRedBlockHeaderSize;
    blocks.push_back({is_last, pt, timestamp_offset, block_length, data_offset});
    offset += kRedBlockHeaderSize;
    if (is_last) break;
  }
  size_t total_end = 0;
  for (const auto& blk : blocks) {
    total_end = std::max(total_end, blk.offset + blk.block_length);
  }
  return total_end <= payload_size;
}

bool RtpPtManipulatorImpl::ModifyRedBlocks(uint8_t* payload, size_t payload_size,
                                           const RtpPayloadTypes& new_pt_values) const {
  std::vector<RedBlock> blocks;
  if (!ParseRedBlocks(payload, payload_size, blocks)) return false;
  size_t offset = 0;
  for (const auto& blk : blocks) {
    uint8_t* header = payload + offset;
    uint8_t old_pt = header[0] & 0x7F;
    uint8_t new_pt = old_pt;
    if (IsUlpfecPacket(old_pt) && new_pt_values.ulpfec_pt.has_value())
      new_pt = *new_pt_values.ulpfec_pt;
    else if (IsVp9Packet(old_pt) && new_pt_values.vp9_pt.has_value())
      new_pt = *new_pt_values.vp9_pt;
    else if (IsRtxPacket(old_pt) && new_pt_values.rtx_pt.has_value())
      new_pt = *new_pt_values.rtx_pt;
    header[0] = (header[0] & 0x80) | (new_pt & 0x7F);
    offset += kRedBlockHeaderSize;
  }
  return true;
}

RtpPayloadTypes RtpPtManipulatorImpl::ParsePtValues(const RtpPacket& packet) const {
  RtpPayloadTypes pt_values;
  uint8_t outer_pt = packet.PayloadType();
  if (IsRedPacket(outer_pt)) {
    pt_values.red_pt = outer_pt;
    std::vector<RedBlock> blocks;
    if (ParseRedBlocks(packet.payload().data(), packet.payload().size(), blocks)) {
      for (const auto& blk : blocks) {
        if (IsUlpfecPacket(blk.payload_type))
          pt_values.ulpfec_pt = blk.payload_type;
        else if (IsVp9Packet(blk.payload_type))
          pt_values.vp9_pt = blk.payload_type;
        else if (IsRtxPacket(blk.payload_type))
          pt_values.rtx_pt = blk.payload_type;
      }
    }
  } else if (IsUlpfecPacket(outer_pt)) {
    pt_values.ulpfec_pt = outer_pt;
  } else if (IsVp9Packet(outer_pt)) {
    pt_values.vp9_pt = outer_pt;
  } else if (IsRtxPacket(outer_pt)) {
    pt_values.rtx_pt = outer_pt;
  }
  return pt_values;
}

bool RtpPtManipulatorImpl::ModifyPtValues(RtpPacket* packet,
                                          const RtpPayloadTypes& new_pt_values) {
  if (!packet) return false;

  uint8_t current_pt = packet->PayloadType();

  // RED packet (may contain multiple blocks)
  if (IsRedPacket(current_pt) && new_pt_values.red_pt.has_value()) {
    // Step 1: modify outer RTP PT
    packet->SetPayloadType(*new_pt_values.red_pt);
    // Step 2: get a modifiable copy of the entire packet buffer
    rtc::CopyOnWriteBuffer buffer = packet->Buffer(); // copy
    // Step 3: modify RED blocks inside the payload
    if (packet->payload_size() > 0) {
      uint8_t* payload_ptr = buffer.data() + packet->headers_size();
      if (!ModifyRedBlocks(payload_ptr, packet->payload_size(), new_pt_values)) {
        return false;
      }
    }
    // Step 4: re-parse the modified buffer back into the packet
    if (!packet->Parse(buffer)) {
      return false;
    }
    return true;
  }

  // Non-RED packets: simple PT change
  if (IsVp9Packet(current_pt) && new_pt_values.vp9_pt.has_value())
    packet->SetPayloadType(*new_pt_values.vp9_pt);
  else if (IsUlpfecPacket(current_pt) && new_pt_values.ulpfec_pt.has_value())
    packet->SetPayloadType(*new_pt_values.ulpfec_pt);
  else if (IsRtxPacket(current_pt) && new_pt_values.rtx_pt.has_value())
    packet->SetPayloadType(*new_pt_values.rtx_pt);

  return true;
}

bool RtpPtManipulatorImpl::VerifyModification(const RtpPacket& modified_packet,
                                              const RtpPayloadTypes& expected) const {
  RtpPayloadTypes actual = ParsePtValues(modified_packet);
  if (expected.red_pt && (!actual.red_pt || *actual.red_pt != *expected.red_pt)) return false;
  if (expected.ulpfec_pt && (!actual.ulpfec_pt || *actual.ulpfec_pt != *expected.ulpfec_pt)) return false;
  if (expected.vp9_pt && (!actual.vp9_pt || *actual.vp9_pt != *expected.vp9_pt)) return false;
  if (expected.rtx_pt && (!actual.rtx_pt || *actual.rtx_pt != *expected.rtx_pt)) return false;
  return true;
}

bool RtpPtManipulatorImpl::IsRedPacket(uint8_t pt) const {
  return red_pt_.has_value() && pt == *red_pt_;
}
bool RtpPtManipulatorImpl::IsUlpfecPacket(uint8_t pt) const {
  return ulpfec_pt_.has_value() && pt == *ulpfec_pt_;
}
bool RtpPtManipulatorImpl::IsVp9Packet(uint8_t pt) const {
  return vp9_pt_.has_value() && pt == *vp9_pt_;
}
bool RtpPtManipulatorImpl::IsRtxPacket(uint8_t pt) const {
  return rtx_pt_.has_value() && pt == *rtx_pt_;
}

}  // namespace webrtc