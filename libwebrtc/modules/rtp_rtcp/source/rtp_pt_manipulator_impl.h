/*
 *  Copyright (c) 2024 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_PT_MANIPULATOR_IMPL_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_PT_MANIPULATOR_IMPL_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "modules/rtp_rtcp/source/rtp_packet.h"

namespace webrtc {

struct SdpMediaDescription {
  std::string media_type;
  uint16_t port;
  std::string protocol;
  std::vector<uint8_t> payload_types;
  std::map<uint8_t, std::string> rtpmap;
  std::string address;

  std::string GetCodecName(uint8_t pt) const {
    auto it = rtpmap.find(pt);
    if (it != rtpmap.end()) {
      const std::string& full = it->second;
      // rtpmap 格式通常为 "VP9/90000" 或 "red/90000"
      size_t slash = full.find('/');
      if (slash != std::string::npos) {
        return full.substr(0, slash);
      }
      return full;
    }
    return "";
  }

  bool IsCodec(uint8_t pt, const std::string& codec_name) const {
    std::string name = GetCodecName(pt);
    return !name.empty() && name == codec_name;
  }
};

struct RtpPayloadTypes {
  std::optional<uint8_t> red_pt;
  std::optional<uint8_t> ulpfec_pt;
  std::optional<uint8_t> vp9_pt;
  std::optional<uint8_t> rtx_pt;

  void Reset() {
    red_pt.reset();
    ulpfec_pt.reset();
    vp9_pt.reset();
    rtx_pt.reset();
  }

  bool HasAnyValue() const {
    return red_pt.has_value() || ulpfec_pt.has_value() ||
           vp9_pt.has_value() || rtx_pt.has_value();
  }

  bool HasAllValues() const {
    return red_pt.has_value() && ulpfec_pt.has_value() &&
           vp9_pt.has_value() && rtx_pt.has_value();
  }

  std::string ToString() const {
    std::string result = "RtpPayloadTypes: ";
    if (red_pt) result += "RED=" + std::to_string(*red_pt) + " ";
    if (ulpfec_pt) result += "ULPFEC=" + std::to_string(*ulpfec_pt) + " ";
    if (vp9_pt) result += "VP9=" + std::to_string(*vp9_pt) + " ";
    if (rtx_pt) result += "RTX=" + std::to_string(*rtx_pt);
    return result;
  }
};

class RtpPtManipulatorImpl {
 public:
  RtpPtManipulatorImpl();
  ~RtpPtManipulatorImpl();

  void ConfigureSdp(const SdpMediaDescription& sdp);

  RtpPayloadTypes ParsePtValues(const RtpPacket& packet) const;

  bool ModifyPtValues(RtpPacket* packet, const RtpPayloadTypes& new_pt_values);

  bool VerifyModification(const RtpPacket& modified_packet,
                         const RtpPayloadTypes& expected_pt_values) const;

 private:
  bool ParseRedPayload(const uint8_t* payload, size_t payload_size,
                       RtpPayloadTypes& pt_values) const;

  bool ParseUlpfecPayload(const uint8_t* payload, size_t payload_size,
                          RtpPayloadTypes& pt_values) const;

  bool ModifyRedPayload(uint8_t* payload, size_t payload_size,
                        const RtpPayloadTypes& new_pt_values) const;

  bool ModifyUlpfecPayload(uint8_t* payload, size_t payload_size,
                           const RtpPayloadTypes& new_pt_values) const;

  bool IsRedPacket(uint8_t pt) const;
  bool IsUlpfecPacket(uint8_t pt) const;
  bool IsVp9Packet(uint8_t pt) const;
  bool IsRtxPacket(uint8_t pt) const;

  SdpMediaDescription sdp_;
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_RTP_PT_MANIPULATOR_IMPL_H_