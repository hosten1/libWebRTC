#ifndef MODULES_RTP_RTCP_SOURCE_RTP_PT_MANIPULATOR_IMPL_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_PT_MANIPULATOR_IMPL_H_

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "absl/types/optional.h"
#include "modules/rtp_rtcp/source/rtp_packet.h"

namespace webrtc {

struct SdpMediaDescription {
  std::string media_type;
  uint16_t port;
  std::string protocol;
  std::vector<uint8_t> payload_types;
  std::map<uint8_t, std::string> rtpmap;
  std::string address;

  std::string GetCodecName(uint8_t pt) const;
  bool IsCodec(uint8_t pt, const std::string& codec_name) const;
};

struct RtpPayloadTypes {
  std::vector<uint8_t> red_pts;
  std::vector<uint8_t> ulpfec_pts;
  std::vector<uint8_t> vp9_pts;
  std::vector<uint8_t> rtx_pts;

  void Clear();
  bool HasAny() const;
  std::string ToString() const;
};

struct PtMappingRule {
  uint8_t old_pt;
  uint8_t new_pt;
};

class RtpPtManipulatorImpl {
 public:
  RtpPtManipulatorImpl();
  ~RtpPtManipulatorImpl();

  void ConfigureSdp(const SdpMediaDescription& sdp);

  RtpPayloadTypes ParsePtValues(const RtpPacket& packet) const;

  bool ModifyPtValues(RtpPacket* packet, const std::vector<PtMappingRule>& mappings);

  bool ModifyPtValuesSimple(RtpPacket* packet, const RtpPayloadTypes& new_pt_values);

  bool VerifyModification(const RtpPacket& packet,
                          const std::vector<PtMappingRule>& expected_mappings) const;

 private:
  enum CodecType { kCodecRed, kCodecUlpfec, kCodecVp9, kCodecRtx, kCodecUnknown };
  CodecType GetCodecType(uint8_t pt) const;

  bool IsRedPacket(uint8_t pt) const;
  bool IsUlpfecPacket(uint8_t pt) const;
  bool IsVp9Packet(uint8_t pt) const;
  bool IsRtxPacket(uint8_t pt) const;

  // 缓存的 PT 值（单值，简化）
  absl::optional<uint8_t> red_pt_;
  absl::optional<uint8_t> ulpfec_pt_;
  absl::optional<uint8_t> vp9_pt_;
  absl::optional<uint8_t> rtx_pt_;
};

}  // namespace webrtc

#endif