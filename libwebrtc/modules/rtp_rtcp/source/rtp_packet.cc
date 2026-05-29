/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/rtp_rtcp/source/rtp_packet.h"

#include <cstdint>
#include <cstring>
#include <utility>

#include "modules/rtp_rtcp/source/byte_io.h"
#include "rtc_base/checks.h"
//#include "rtc_base/logging.h"
#include "rtc_base/numerics/safe_conversions.h"

namespace webrtc {
// lym
static constexpr int kMinId = 1;
static constexpr int kMaxId = 255;
static constexpr int kMaxValueSize = 255;
static constexpr int kOneByteHeaderExtensionMaxId = 14;
static constexpr int kOneByteHeaderExtensionMaxValueSize = 16;
// lym
namespace {
constexpr size_t kFixedHeaderSize = 12;
constexpr uint8_t kRtpVersion = 2;
constexpr uint16_t kOneByteExtensionProfileId = 0xBEDE;
constexpr uint16_t kTwoByteExtensionProfileId = 0x1000;
constexpr size_t kOneByteExtensionHeaderLength = 1;
constexpr size_t kTwoByteExtensionHeaderLength = 2;
constexpr size_t kDefaultPacketSize = 1500;
}  // namespace

//  0                   1                   2                   3
//  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |V=2|P|X|  CC   |M|     PT      |       sequence number         |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                           timestamp                           |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |           synchronization source (SSRC) identifier            |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |            Contributing source (CSRC) identifiers             |
// |                             ....                              |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |  header eXtension profile id  |       length in 32bits        |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                          Extensions                           |
// |                             ....                              |
// +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
// |                           Payload                             |
// |             ....              :  padding...                   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |               padding         | Padding size  |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
RtpPacket::RtpPacket() : RtpPacket(nullptr, kDefaultPacketSize) {}

RtpPacket::RtpPacket(const ExtensionManager* extensions)
    : RtpPacket(extensions, kDefaultPacketSize) {}

RtpPacket::RtpPacket(const RtpPacket&) = default;

RtpPacket::RtpPacket(const ExtensionManager* extensions, size_t capacity)
    : extensions_(extensions ? *extensions : ExtensionManager()),
      buffer_(capacity) {
  RTC_DCHECK_GE(capacity, kFixedHeaderSize);
  Clear();
}

RtpPacket::~RtpPacket() {}

void RtpPacket::IdentifyExtensions(const ExtensionManager& extensions) {
  extensions_ = extensions;
}

bool RtpPacket::Parse(const uint8_t* buffer, size_t buffer_size) {
  if (!ParseBuffer(buffer, buffer_size)) {
    Clear();
    return false;
  }
  buffer_.SetData(buffer, buffer_size);
  RTC_DCHECK_EQ(size(), buffer_size);
  return true;
}

bool RtpPacket::Parse(rtc::ArrayView<const uint8_t> packet) {
  return Parse(packet.data(), packet.size());
}

bool RtpPacket::Parse(rtc::CopyOnWriteBuffer buffer) {
  if (!ParseBuffer(buffer.cdata(), buffer.size())) {
    Clear();
    return false;
  }
  size_t buffer_size = buffer.size();
  buffer_ = std::move(buffer);
  RTC_DCHECK_EQ(size(), buffer_size);
  return true;
}

std::vector<uint32_t> RtpPacket::Csrcs() const {
  size_t num_csrc = data()[0] & 0x0F;
  RTC_DCHECK_GE(capacity(), kFixedHeaderSize + num_csrc * 4);
  std::vector<uint32_t> csrcs(num_csrc);
  for (size_t i = 0; i < num_csrc; ++i) {
    csrcs[i] =
        ByteReader<uint32_t>::ReadBigEndian(&data()[kFixedHeaderSize + i * 4]);
  }
  return csrcs;
}

void RtpPacket::CopyHeaderFrom(const RtpPacket& packet) {
  RTC_DCHECK_GE(capacity(), packet.headers_size());

  marker_ = packet.marker_;
  payload_type_ = packet.payload_type_;
  sequence_number_ = packet.sequence_number_;
  timestamp_ = packet.timestamp_;
  ssrc_ = packet.ssrc_;
  payload_offset_ = packet.payload_offset_;
  extensions_ = packet.extensions_;
  extension_entries_ = packet.extension_entries_;
  extensions_size_ = packet.extensions_size_;
  buffer_.SetData(packet.data(), packet.headers_size());
  // Reset payload and padding.
  payload_size_ = 0;
  padding_size_ = 0;
}

void RtpPacket::SetMarker(bool marker_bit) {
  marker_ = marker_bit;
  if (marker_) {
    WriteAt(1, data()[1] | 0x80);
  } else {
    WriteAt(1, data()[1] & 0x7F);
  }
}

void RtpPacket::SetPayloadType(uint8_t payload_type) {
  RTC_DCHECK_LE(payload_type, 0x7Fu);
  payload_type_ = payload_type;
  WriteAt(1, (data()[1] & 0x80) | payload_type);
}

void RtpPacket::SetSequenceNumber(uint16_t seq_no) {
  sequence_number_ = seq_no;
  ByteWriter<uint16_t>::WriteBigEndian(WriteAt(2), seq_no);
}

void RtpPacket::SetTimestamp(uint32_t timestamp) {
  timestamp_ = timestamp;
  ByteWriter<uint32_t>::WriteBigEndian(WriteAt(4), timestamp);
}

void RtpPacket::SetSsrc(uint32_t ssrc) {
  ssrc_ = ssrc;
  ByteWriter<uint32_t>::WriteBigEndian(WriteAt(8), ssrc);
}

void RtpPacket::SetCsrcs(rtc::ArrayView<const uint32_t> csrcs) {
  RTC_DCHECK_EQ(extensions_size_, 0);
  RTC_DCHECK_EQ(payload_size_, 0);
  RTC_DCHECK_EQ(padding_size_, 0);
  RTC_DCHECK_LE(csrcs.size(), 0x0fu);
  RTC_DCHECK_LE(kFixedHeaderSize + 4 * csrcs.size(), capacity());
  payload_offset_ = kFixedHeaderSize + 4 * csrcs.size();
  WriteAt(0, (data()[0] & 0xF0) | rtc::dchecked_cast<uint8_t>(csrcs.size()));
  size_t offset = kFixedHeaderSize;
  for (uint32_t csrc : csrcs) {
    ByteWriter<uint32_t>::WriteBigEndian(WriteAt(offset), csrc);
    offset += 4;
  }
  buffer_.SetSize(payload_offset_);
}

rtc::ArrayView<uint8_t> RtpPacket::AllocateRawExtension(int id, size_t length) {
  RTC_DCHECK_GE(id, kMinId);
  RTC_DCHECK_LE(id, kMaxId);
  RTC_DCHECK_GE(length, 1);
  RTC_DCHECK_LE(length, kMaxValueSize);
  const ExtensionInfo* extension_entry = FindExtensionInfo(id);
  if (extension_entry != nullptr) {
    // Extension already reserved. Check if same length is used.
    if (extension_entry->length == length)
      return rtc::MakeArrayView(WriteAt(extension_entry->offset), length);
//
//    RTC_LOG(LS_ERROR) << "Length mismatch for extension id " << id
//                      << ": expected "
//                      << static_cast<int>(extension_entry->length)
//                      << ". received " << length;
    return nullptr;
  }
  if (payload_size_ > 0) {
//    RTC_LOG(LS_ERROR) << "Can't add new extension id " << id
//                      << " after payload was set.";
    return nullptr;
  }
  if (padding_size_ > 0) {
//    RTC_LOG(LS_ERROR) << "Can't add new extension id " << id
//                      << " after padding was set.";
    return nullptr;
  }

  const size_t num_csrc = data()[0] & 0x0F;
  const size_t extensions_offset = kFixedHeaderSize + (num_csrc * 4) + 4;
  // Determine if two-byte header is required for the extension based on id and
  // length. Please note that a length of 0 also requires two-byte header
  // extension. See RFC8285 Section 4.2-4.3.
  const bool two_byte_header_required =
      id > kOneByteHeaderExtensionMaxId ||
      length > kOneByteHeaderExtensionMaxValueSize || length == 0;
  RTC_CHECK(!two_byte_header_required || extensions_.ExtmapAllowMixed());

  uint16_t profile_id;
  if (extensions_size_ > 0) {
    profile_id =
        ByteReader<uint16_t>::ReadBigEndian(data() + extensions_offset - 4);
    if (profile_id == kOneByteExtensionProfileId && two_byte_header_required) {
      // Is buffer size big enough to fit promotion and new data field?
      // The header extension will grow with one byte per already allocated
      // extension + the size of the extension that is about to be allocated.
      size_t expected_new_extensions_size =
          extensions_size_ + extension_entries_.size() +
          kTwoByteExtensionHeaderLength + length;
      if (extensions_offset + expected_new_extensions_size > capacity()) {
//        RTC_LOG(LS_ERROR)
//            << "Extension cannot be registered: Not enough space left in "
//               "buffer to change to two-byte header extension and add new "
//               "extension.";
        return nullptr;
      }
      // Promote already written data to two-byte header format.
      PromoteToTwoByteHeaderExtension();
      profile_id = kTwoByteExtensionProfileId;
    }
  } else {
    // Profile specific ID, set to OneByteExtensionHeader unless
    // TwoByteExtensionHeader is required.
    profile_id = two_byte_header_required ? kTwoByteExtensionProfileId
                                          : kOneByteExtensionProfileId;
  }

  const size_t extension_header_size = profile_id == kOneByteExtensionProfileId
                                           ? kOneByteExtensionHeaderLength
                                           : kTwoByteExtensionHeaderLength;
  size_t new_extensions_size =
      extensions_size_ + extension_header_size + length;
  if (extensions_offset + new_extensions_size > capacity()) {
//    RTC_LOG(LS_ERROR)
//        << "Extension cannot be registered: Not enough space left in buffer.";
    return nullptr;
  }

  // All checks passed, write down the extension headers.
  if (extensions_size_ == 0) {
    RTC_DCHECK_EQ(payload_offset_, kFixedHeaderSize + (num_csrc * 4));
    WriteAt(0, data()[0] | 0x10);  // Set extension bit.
    ByteWriter<uint16_t>::WriteBigEndian(WriteAt(extensions_offset - 4),
                                         profile_id);
  }

  if (profile_id == kOneByteExtensionProfileId) {
    uint8_t one_byte_header = rtc::dchecked_cast<uint8_t>(id) << 4;
    one_byte_header |= rtc::dchecked_cast<uint8_t>(length - 1);
    WriteAt(extensions_offset + extensions_size_, one_byte_header);
  } else {
    // TwoByteHeaderExtension.
    uint8_t extension_id = rtc::dchecked_cast<uint8_t>(id);
    WriteAt(extensions_offset + extensions_size_, extension_id);
    uint8_t extension_length = rtc::dchecked_cast<uint8_t>(length);
    WriteAt(extensions_offset + extensions_size_ + 1, extension_length);
  }

  const uint16_t extension_info_offset = rtc::dchecked_cast<uint16_t>(
      extensions_offset + extensions_size_ + extension_header_size);
  const uint8_t extension_info_length = rtc::dchecked_cast<uint8_t>(length);
  extension_entries_.emplace_back(id, extension_info_length,
                                  extension_info_offset);

  extensions_size_ = new_extensions_size;

  uint16_t extensions_size_padded =
      SetExtensionLengthMaybeAddZeroPadding(extensions_offset);
  payload_offset_ = extensions_offset + extensions_size_padded;
  buffer_.SetSize(payload_offset_);
  return rtc::MakeArrayView(WriteAt(extension_info_offset),
                            extension_info_length);
}

void RtpPacket::PromoteToTwoByteHeaderExtension() {
  size_t num_csrc = data()[0] & 0x0F;
  size_t extensions_offset = kFixedHeaderSize + (num_csrc * 4) + 4;

  RTC_CHECK_GT(extension_entries_.size(), 0);
  RTC_CHECK_EQ(payload_size_, 0);
  RTC_CHECK_EQ(kOneByteExtensionProfileId, ByteReader<uint16_t>::ReadBigEndian(
                                               data() + extensions_offset - 4));
  // Rewrite data.
  // Each extension adds one to the offset. The write-read delta for the last
  // extension is therefore the same as the number of extension entries.
  size_t write_read_delta = extension_entries_.size();
  for (auto extension_entry = extension_entries_.rbegin();
       extension_entry != extension_entries_.rend(); ++extension_entry) {
    size_t read_index = extension_entry->offset;
    size_t write_index = read_index + write_read_delta;
    // Update offset.
    extension_entry->offset = rtc::dchecked_cast<uint16_t>(write_index);
    // Copy data. Use memmove since read/write regions may overlap.
    memmove(WriteAt(write_index), data() + read_index, extension_entry->length);
    // Rewrite id and length.
    WriteAt(--write_index, extension_entry->length);
    WriteAt(--write_index, extension_entry->id);
    --write_read_delta;
  }

  // Update profile header, extensions length, and zero padding.
  ByteWriter<uint16_t>::WriteBigEndian(WriteAt(extensions_offset - 4),
                                       kTwoByteExtensionProfileId);
  extensions_size_ += extension_entries_.size();
  uint16_t extensions_size_padded =
      SetExtensionLengthMaybeAddZeroPadding(extensions_offset);
  payload_offset_ = extensions_offset + extensions_size_padded;
  buffer_.SetSize(payload_offset_);
}

uint16_t RtpPacket::SetExtensionLengthMaybeAddZeroPadding(
    size_t extensions_offset) {
  // Update header length field.
  uint16_t extensions_words = rtc::dchecked_cast<uint16_t>(
      (extensions_size_ + 3) / 4);  // Wrap up to 32bit.
  ByteWriter<uint16_t>::WriteBigEndian(WriteAt(extensions_offset - 2),
                                       extensions_words);
  // Fill extension padding place with zeroes.
  size_t extension_padding_size = 4 * extensions_words - extensions_size_;
  memset(WriteAt(extensions_offset + extensions_size_), 0,
         extension_padding_size);
  return 4 * extensions_words;
}

uint8_t* RtpPacket::AllocatePayload(size_t size_bytes) {
  // Reset payload size to 0. If CopyOnWrite buffer_ was shared, this will cause
  // reallocation and memcpy. Keeping just header reduces memcpy size.
  SetPayloadSize(0);
  return SetPayloadSize(size_bytes);
}

uint8_t* RtpPacket::SetPayloadSize(size_t size_bytes) {
  RTC_DCHECK_EQ(padding_size_, 0);
  if (payload_offset_ + size_bytes > capacity()) {
//    RTC_LOG(LS_WARNING) << "Cannot set payload, not enough space in buffer.";
    return nullptr;
  }
  payload_size_ = size_bytes;
  buffer_.SetSize(payload_offset_ + payload_size_);
  return WriteAt(payload_offset_);
}

bool RtpPacket::SetPadding(size_t padding_bytes) {
  if (payload_offset_ + payload_size_ + padding_bytes > capacity()) {
//    RTC_LOG(LS_WARNING) << "Cannot set padding size " << padding_bytes
//                        << ", only "
//                        << (capacity() - payload_offset_ - payload_size_)
//                        << " bytes left in buffer.";
    return false;
  }
  padding_size_ = rtc::dchecked_cast<uint8_t>(padding_bytes);
  buffer_.SetSize(payload_offset_ + payload_size_ + padding_size_);
  if (padding_size_ > 0) {
    size_t padding_offset = payload_offset_ + payload_size_;
    size_t padding_end = padding_offset + padding_size_;
    memset(WriteAt(padding_offset), 0, padding_size_ - 1);
    WriteAt(padding_end - 1, padding_size_);
    WriteAt(0, data()[0] | 0x20);  // Set padding bit.
  } else {
    WriteAt(0, data()[0] & ~0x20);  // Clear padding bit.
  }
  return true;
}

void RtpPacket::Clear() {
  marker_ = false;
  payload_type_ = 0;
  sequence_number_ = 0;
  timestamp_ = 0;
  ssrc_ = 0;
  payload_offset_ = kFixedHeaderSize;
  payload_size_ = 0;
  padding_size_ = 0;
  extensions_size_ = 0;
  extension_entries_.clear();

  memset(WriteAt(0), 0, kFixedHeaderSize);
  buffer_.SetSize(kFixedHeaderSize);
  WriteAt(0, kRtpVersion << 6);
}

bool RtpPacket::ParseBuffer(const uint8_t* buffer, size_t size) {
  if (size < kFixedHeaderSize) {
    return false;
  }
// 第1个字节解析
//  版本v占2bit
  const uint8_t version = buffer[0] >> 6;
  if (version != kRtpVersion) {//kRtpVersion=2
    return false;
  }
    // 解析填充标识位，占1bit；
  const bool has_padding = (buffer[0] & 0x20) != 0;
    // 解析扩展标识位，占1bit；表示当前这个包是不是有RTP扩展头，1表示有，后面一定有extension
  const bool has_extension = (buffer[0] & 0x10) != 0;
    // 解析CSRC计数（CC），占4位；当前的数据是由那些源共同生成；
  const uint8_t number_of_crcs = buffer[0] & 0x0f;
//     第2个字节解析
//    包结束标志位，占1bit，一般用来表示一个数据分包后的最后一个包
  marker_ = (buffer[1] & 0x80) != 0;
//    有效负载类型 (PT)，占7bit;
  payload_type_ = buffer[1] & 0x7f;
    // 第3 4个字节解析,解析当前包的序号，占2个字节；
  sequence_number_ = ByteReader<uint16_t>::ReadBigEndian(&buffer[2]);
    // 第5 6 7 8个字节解析,解析当前包的时间戳，占4个字节；
  timestamp_ = ByteReader<uint32_t>::ReadBigEndian(&buffer[4]);
    // 第9 10 11 12个字节解析,解析RTP包流的来源SSRC，占4个字节；
  ssrc_ = ByteReader<uint32_t>::ReadBigEndian(&buffer[8]);
    // 如果 已有的包头信息 和CSRC占用的已经超过包大小，那就返回失败；
  if (size < kFixedHeaderSize + number_of_crcs * 4) {
    return false;
  }
 // 记录解析玩的位置，也就是指针偏移量
  payload_offset_ = kFixedHeaderSize + number_of_crcs * 4;//kFixedHeaderSize=12
// 如果有填充信息，这里会计算填充字段的大小；
  if (has_padding) {
    padding_size_ = buffer[size - 1];
    if (padding_size_ == 0) {
//      RTC_LOG(LS_WARNING) << "Padding was set, but padding size is zero";
      return false;
    }
  } else {
    padding_size_ = 0;
  }

  extensions_size_ = 0;
  extension_entries_.clear();
  if (has_extension) {
    /* RTP header extension, RFC 3550.
     0                   1                   2                   3
     0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |      defined by profile       |           length              |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                        header extension                       |
    |                             ....                              |
    */
//      记录扩展内容的位置
    size_t extension_offset = payload_offset_ + 4;
    if (extension_offset > size) {
      return false;
    }
//      解析 profile ,占2个字节,这里固定是 0XBEDE 48862
    uint16_t profile =
        ByteReader<uint16_t>::ReadBigEndian(&buffer[payload_offset_]);
//      解析 extension的个数也就是lenght,占2个字节
    size_t extensions_capacity =
        ByteReader<uint16_t>::ReadBigEndian(&buffer[payload_offset_ + 2]);
//     WebRTC这里按照： 扩展的每个item占用4个字节，也就是以32bit对齐；
    extensions_capacity *= 4;
    // 如果解析到这里已经超过包 大小就返回错误；
    if (extension_offset + extensions_capacity > size) {
      return false;
    }
//      判断类型是不是正确的值
    if (profile != kOneByteExtensionProfileId &&
        profile != kTwoByteExtensionProfileId) {
//      RTC_LOG(LS_WARNING) << "Unsupported rtp extension " << profile;
    } else {
      size_t extension_header_length = profile == kOneByteExtensionProfileId
                                           ? kOneByteExtensionHeaderLength//1
                                           : kTwoByteExtensionHeaderLength;//2
      constexpr uint8_t kPaddingByte = 0;
      constexpr uint8_t kPaddingId = 0;
      constexpr uint8_t kOneByteHeaderExtensionReservedId = 15;
        // 解析每一个item；
      while (extensions_size_ + extension_header_length < extensions_capacity) {
          // 如果解析的id值是0，就累加一次
        if (buffer[extension_offset + extensions_size_] == kPaddingByte) {
          extensions_size_++;
          continue;
        }
        int id;
        uint8_t length;
//          如果是OneByte的就按照 onej解析
        if (profile == kOneByteExtensionProfileId) {
//        占用4bits，
          id = buffer[extension_offset + extensions_size_] >> 4;
//        这个item占用的字节数，ont byte是要+1；占4bit；
          length = 1 + (buffer[extension_offset + extensions_size_] & 0xf);
          if (id == kOneByteHeaderExtensionReservedId ||
              (id == kPaddingId && length != 1)) {
            break;
          }
        } else {
        // two  byte的每一项都占用1字节，初始extensions_size_是0，每解析一个item就累加一次；
          id = buffer[extension_offset + extensions_size_];
          length = buffer[extension_offset + extensions_size_ + 1];
        }

        if (extensions_size_ + extension_header_length + length >
            extensions_capacity) {
//          RTC_LOG(LS_WARNING) << "Oversized rtp header extension.";
          break;
        }
       // 根据 extesion id 查找对应的值；
        ExtensionInfo& extension_info = FindOrCreateExtensionInfo(id);
        if (extension_info.length != 0) {
//          RTC_LOG(LS_VERBOSE)
//              << "Duplicate rtp header extension id " << id << ". Overwriting.";
        }
        // 计算 exteninfo的位置
        size_t offset =
            extension_offset + extensions_size_ + extension_header_length;
        if (!rtc::IsValueInRangeForNumericType<uint16_t>(offset)) {
//          RTC_DLOG(LS_WARNING) << "Oversized rtp header extension.";
          break;
        }
        extension_info.offset = static_cast<uint16_t>(offset);
        extension_info.length = length;
        /*累加长度  也就是:
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |  ID   | L=0   |     data      |  ID   |  L=1  |   data...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      */
        extensions_size_ += extension_header_length + length;
      }
    }
      // extension 解析完成后 ，记录指针偏移量；
    payload_offset_ = extension_offset + extensions_capacity;
  }

  if (payload_offset_ + padding_size_ > size) {
    return false;
  }
 // 剩下的数据大小
  payload_size_ = size - payload_offset_ - padding_size_;
  return true;
}

const RtpPacket::ExtensionInfo* RtpPacket::FindExtensionInfo(int id) const {
  for (const ExtensionInfo& extension : extension_entries_) {
    if (extension.id == id) {
      return &extension;
    }
  }
  return nullptr;
}

RtpPacket::ExtensionInfo& RtpPacket::FindOrCreateExtensionInfo(int id) {
  for (ExtensionInfo& extension : extension_entries_) {
    if (extension.id == id) {
      return extension;
    }
  }
  extension_entries_.emplace_back(id);
  return extension_entries_.back();
}

rtc::ArrayView<const uint8_t> RtpPacket::FindExtension(
    ExtensionType type) const {
  uint8_t id = extensions_.GetId(type);
  if (id == ExtensionManager::kInvalidId) {
    // Extension not registered.
    return nullptr;
  }
  ExtensionInfo const* extension_info = FindExtensionInfo(id);
  if (extension_info == nullptr) {
    return nullptr;
  }
  return rtc::MakeArrayView(data() + extension_info->offset,
                            extension_info->length);
}

rtc::ArrayView<uint8_t> RtpPacket::AllocateExtension(ExtensionType type,
                                                     size_t length) {
  // TODO(webrtc:7990): Add support for empty extensions (length==0).
  if (length == 0 || length > kMaxValueSize ||
      (!extensions_.ExtmapAllowMixed() &&
       length > kOneByteHeaderExtensionMaxValueSize)) {
    return nullptr;
  }

  uint8_t id = extensions_.GetId(type);
  if (id == ExtensionManager::kInvalidId) {
    // Extension not registered.
    return nullptr;
  }
  if (!extensions_.ExtmapAllowMixed() &&
      id > kOneByteHeaderExtensionMaxId) {
    return nullptr;
  }
  return AllocateRawExtension(id, length);
}

bool RtpPacket::HasExtension(ExtensionType type) const {
  // TODO(webrtc:7990): Add support for empty extensions (length==0).
  return !FindExtension(type).empty();
}

}  // namespace webrtc
