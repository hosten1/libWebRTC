#include "modules/rtp_rtcp/source/rtp_pt_manipulator_impl.h"

#include <cstring>
#include <algorithm>
#include <map>
#include <set>

#include "rtc_base/checks.h"

namespace webrtc {

// RED 头大小（WebRTC 实现：1 字节）
static constexpr size_t kRedHeaderSize = 1;

// === SdpMediaDescription 实现 ===
std::string SdpMediaDescription::GetCodecName(uint8_t pt) const {
  auto it = rtpmap.find(pt);
  if (it == rtpmap.end()) return "";
  const std::string& full = it->second;
  size_t slash = full.find('/');
  if (slash != std::string::npos) {
    return full.substr(0, slash);
  }
  return full;
}

bool SdpMediaDescription::IsCodec(uint8_t pt, const std::string& codec_name) const {
  std::string name = GetCodecName(pt);
  return !name.empty() && name == codec_name;
}

// === RtpPayloadTypes 实现 ===
void RtpPayloadTypes::Clear() {
  red_pts.clear();
  ulpfec_pts.clear();
  vp9_pts.clear();
  rtx_pts.clear();
}

bool RtpPayloadTypes::HasAny() const {
  return !red_pts.empty() || !ulpfec_pts.empty() || !vp9_pts.empty() || !rtx_pts.empty();
}

std::string RtpPayloadTypes::ToString() const {
  std::string out;
  auto append = [&](const char* name, const std::vector<uint8_t>& pts) {
    if (!pts.empty()) {
      out += name;
      out += "=";
      for (size_t i = 0; i < pts.size(); ++i) {
        out += std::to_string(pts[i]);
        if (i + 1 < pts.size()) out += ",";
      }
      out += " ";
    }
  };
  append("RED", red_pts);
  append("ULPFEC", ulpfec_pts);
  append("VP9", vp9_pts);
  append("RTX", rtx_pts);
  if (!out.empty()) out.pop_back();
  return out;
}

// === RtpPtManipulatorImpl 实现 ===
RtpPtManipulatorImpl::RtpPtManipulatorImpl() = default;
RtpPtManipulatorImpl::~RtpPtManipulatorImpl() = default;

void RtpPtManipulatorImpl::ConfigureSdp(const SdpMediaDescription& sdp) {
  red_pt_.reset();
  ulpfec_pt_.reset();
  vp9_pt_.reset();
  rtx_pt_.reset();
  for (const auto& kv : sdp.rtpmap) {
    std::string name = sdp.GetCodecName(kv.first);
    if (name == "red") red_pt_ = kv.first;
    else if (name == "ulpfec") ulpfec_pt_ = kv.first;
    else if (name == "VP9") vp9_pt_ = kv.first;
    else if (name == "rtx") rtx_pt_ = kv.first;
  }
}

RtpPtManipulatorImpl::CodecType RtpPtManipulatorImpl::GetCodecType(uint8_t pt) const {
  if (red_pt_.has_value() && pt == *red_pt_) return kCodecRed;
  if (ulpfec_pt_.has_value() && pt == *ulpfec_pt_) return kCodecUlpfec;
  if (vp9_pt_.has_value() && pt == *vp9_pt_) return kCodecVp9;
  if (rtx_pt_.has_value() && pt == *rtx_pt_) return kCodecRtx;
  return kCodecUnknown;
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

RtpPayloadTypes RtpPtManipulatorImpl::ParsePtValues(const RtpPacket& packet) const {
  RtpPayloadTypes result;
  uint8_t outer_pt = packet.PayloadType();
  CodecType outer_type = GetCodecType(outer_pt);

  if (outer_type == kCodecRed) {
    result.red_pts.push_back(outer_pt);
    // RED 包：负载的第一个字节是内层 PT
    if (packet.payload_size() >= kRedHeaderSize) {
      uint8_t inner_pt = packet.payload()[0] & 0x7F;
      CodecType inner_type = GetCodecType(inner_pt);
      if (inner_type == kCodecUlpfec) {
        result.ulpfec_pts.push_back(inner_pt);
      } else if (inner_type == kCodecVp9) {
        result.vp9_pts.push_back(inner_pt);
      } else if (inner_type == kCodecRtx) {
        result.rtx_pts.push_back(inner_pt);
      }
    }
  } else if (outer_type == kCodecUlpfec) {
    result.ulpfec_pts.push_back(outer_pt);
  } else if (outer_type == kCodecVp9) {
    result.vp9_pts.push_back(outer_pt);
  } else if (outer_type == kCodecRtx) {
    result.rtx_pts.push_back(outer_pt);
  }
  return result;
}

bool RtpPtManipulatorImpl::ModifyPtValues(RtpPacket* packet,
                                          const std::vector<PtMappingRule>& mappings) {
  if (!packet) return false;
  uint8_t current_pt = packet->PayloadType();
  CodecType outer_type = GetCodecType(current_pt);

  // 构建映射表
  std::map<uint8_t, uint8_t> pt_map;
  for (const auto& m : mappings) {
    pt_map[m.old_pt] = m.new_pt;
  }

  if (outer_type == kCodecRed) {
    // 修改外层 RTP PT
    auto it = pt_map.find(current_pt);
    if (it != pt_map.end()) {
      packet->SetPayloadType(it->second);
    }
    // 修改 RED 负载中的内层 PT（第一个字节）
    if (packet->payload_size() >= kRedHeaderSize) {
      rtc::CopyOnWriteBuffer buffer = packet->Buffer();
      uint8_t* payload_ptr = buffer.data() + packet->headers_size();
      uint8_t inner_pt = payload_ptr[0] & 0x7F;
      auto it2 = pt_map.find(inner_pt);
      if (it2 != pt_map.end()) {
        payload_ptr[0] = (payload_ptr[0] & 0x80) | (it2->second & 0x7F);
      }
      // 重新解析修改后的 buffer
      if (!packet->Parse(buffer)) {
        return false;
      }
    }
    return true;
  } else if (outer_type != kCodecUnknown) {
    auto it = pt_map.find(current_pt);
    if (it != pt_map.end()) {
      packet->SetPayloadType(it->second);
    }
    return true;
  }
  return true;
}

bool RtpPtManipulatorImpl::ModifyPtValuesSimple(RtpPacket* packet,
                                                const RtpPayloadTypes& new_pt_values) {
  if (!packet) return false;
  uint8_t current_pt = packet->PayloadType();
  CodecType outer_type = GetCodecType(current_pt);

  std::vector<PtMappingRule> mappings;

  auto map_outer = [&](uint8_t old_pt, const std::vector<uint8_t>& new_pts) {
    if (!new_pts.empty()) {
      mappings.push_back({old_pt, new_pts[0]});
    }
  };
  if (outer_type == kCodecRed && !new_pt_values.red_pts.empty()) {
    map_outer(current_pt, new_pt_values.red_pts);
  } else if (outer_type == kCodecUlpfec && !new_pt_values.ulpfec_pts.empty()) {
    map_outer(current_pt, new_pt_values.ulpfec_pts);
  } else if (outer_type == kCodecVp9 && !new_pt_values.vp9_pts.empty()) {
    map_outer(current_pt, new_pt_values.vp9_pts);
  } else if (outer_type == kCodecRtx && !new_pt_values.rtx_pts.empty()) {
    map_outer(current_pt, new_pt_values.rtx_pts);
  }

  // 如果是 RED 包，还需要添加内层 PT 的映射
  if (outer_type == kCodecRed && packet->payload_size() >= kRedHeaderSize) {
    uint8_t inner_pt = packet->payload()[0] & 0x7F;
    CodecType inner_type = GetCodecType(inner_pt);
    if (inner_type == kCodecUlpfec && !new_pt_values.ulpfec_pts.empty()) {
      mappings.push_back({inner_pt, new_pt_values.ulpfec_pts[0]});
    } else if (inner_type == kCodecVp9 && !new_pt_values.vp9_pts.empty()) {
      mappings.push_back({inner_pt, new_pt_values.vp9_pts[0]});
    } else if (inner_type == kCodecRtx && !new_pt_values.rtx_pts.empty()) {
      mappings.push_back({inner_pt, new_pt_values.rtx_pts[0]});
    }
  }

  return ModifyPtValues(packet, mappings);
}

bool RtpPtManipulatorImpl::VerifyModification(const RtpPacket& packet,
                                              const std::vector<PtMappingRule>& expected_mappings) const {
  RtpPayloadTypes parsed = ParsePtValues(packet);
  std::set<uint8_t> expected_new_pts;
  for (const auto& m : expected_mappings) {
    expected_new_pts.insert(m.new_pt);
  }
  std::vector<uint8_t> actual_pts;
  auto add = [&](const std::vector<uint8_t>& pts) {
    actual_pts.insert(actual_pts.end(), pts.begin(), pts.end());
  };
  add(parsed.red_pts);
  add(parsed.ulpfec_pts);
  add(parsed.vp9_pts);
  add(parsed.rtx_pts);

  for (uint8_t pt : actual_pts) {
    if (expected_new_pts.find(pt) == expected_new_pts.end()) {
      return false;
    }
  }
  return true;
}

}  // namespace webrtc