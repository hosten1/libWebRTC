/**
 * libwebrtc 全量接口测试 Demo
 * 覆盖：STUN、线程、RTP、音频、视频、FEC、GCC、TaskQueue、信号处理、日志、时钟等
 */

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <atomic>
#include <chrono>
#include <thread>
#include <cmath>

// ---- STUN ----
#include "api/transport/stun.h"
#include "rtc_base/byte_buffer.h"
#include "rtc_base/ip_address.h"
#include "rtc_base/socket_address.h"

// ---- 线程 ----
#include "rtc_base/thread.h"
#include "rtc_base/event.h"
#include "rtc_base/critical_section.h"

// ---- TaskQueue ----
#include "api/task_queue/default_task_queue_factory.h"
#include "api/task_queue/task_queue_factory.h"
#include "rtc_base/task_utils/to_queued_task.h"

// ---- RTP ----
#include "modules/rtp_rtcp/source/rtp_packet.h"
#include "modules/rtp_rtcp/include/rtp_header_extension_map.h"

// ---- 音频 ----
#include "api/audio/audio_frame.h"
#include "api/audio/channel_layout.h"
#include "common_audio/include/audio_util.h"
#include "common_audio/signal_processing/include/signal_processing_library.h"

// ---- 视频 ----
#include "api/video/encoded_image.h"
#include "api/video/video_codec_type.h"
#include "api/video/video_rotation.h"
#include "api/video/video_content_type.h"
#include "api/video/video_bitrate_allocation.h"
#include "common_video/h264/sps_parser.h"

// ---- FEC ----
#include "modules/rtp_rtcp/source/forward_error_correction.h"
#include "modules/rtp_rtcp/source/ulpfec_header_reader_writer.h"
#include "modules/rtp_rtcp/source/flexfec_header_reader_writer.h"
#include "modules/rtp_rtcp/include/flexfec_sender.h"
#include "modules/rtp_rtcp/source/ulpfec_generator.h"
#include "modules/rtp_rtcp/source/rtp_format_h264.h"
#include "modules/include/module_fec_types.h"
#include "modules/include/module_common_types.h"
#include "common_video/h264/h264_common.h"
#include "rtc_base/copy_on_write_buffer.h"
#include "rtc_base/ref_counted_object.h"
#include "rtc_base/buffer.h"

// ---- GCC ----
#include "api/transport/goog_cc_factory.h"
#include "api/transport/network_control.h"
#include "api/units/time_delta.h"
#include "api/units/data_rate.h"
#include "api/units/data_size.h"
#include "api/units/timestamp.h"

// ---- 工具 ----
#include "rtc_base/logging.h"
#include "rtc_base/message_digest.h"
#include "system_wrappers/include/clock.h"

#define TEST_PASS(name) std::cout << "  [PASS] " << name << std::endl
#define TEST_FAIL(name, msg) do { \
    std::cerr << "  [FAIL] " << name << ": " << msg << std::endl; \
    return false; \
} while(0)
#define ASSERT_TRUE(cond, name) \
    if (!(cond)) TEST_FAIL(name, "assertion failed: " #cond)
#define ASSERT_FALSE(cond, name) \
    if ((cond)) TEST_FAIL(name, "assertion failed: not " #cond)
#define ASSERT_EQ(a, b, name) \
    if ((a) != (b)) TEST_FAIL(name, "expected " #a " == " #b)
#define ASSERT_NE(a, b, name) \
    if ((a) == (b)) TEST_FAIL(name, "expected " #a " != " #b)
#define ASSERT_GT(a, b, name) \
    if (!((a) > (b))) TEST_FAIL(name, "expected " #a " > " #b)
#define ASSERT_GE(a, b, name) \
    if (!((a) >= (b))) TEST_FAIL(name, "expected " #a " >= " #b)

static int g_pass = 0;
static int g_fail = 0;

#define RUN_TEST(fn) do { \
    std::cout << "Test: " #fn << std::endl; \
    if (fn()) { g_pass++; } else { g_fail++; } \
} while(0)

// ============================================================================
// 1. STUN 模块
// ============================================================================

bool test_stun_basic() {
    cricket::StunMessage msg;
    msg.SetType(cricket::STUN_BINDING_REQUEST);
    std::string tid(12, 'a');
    ASSERT_TRUE(msg.SetTransactionID(tid), "set tid");
    ASSERT_EQ(static_cast<int>(msg.type()), cricket::STUN_BINDING_REQUEST, "type");
    ASSERT_EQ(msg.transaction_id(), tid, "tid");
    ASSERT_FALSE(msg.IsLegacy(), "not legacy");
    TEST_PASS("test_stun_basic");
    return true;
}

bool test_stun_xor_address() {
    cricket::StunMessage msg;
    msg.SetType(cricket::STUN_BINDING_RESPONSE);
    rtc::SocketAddress addr("192.168.1.100", 3478);
    auto attr = std::unique_ptr<cricket::StunXorAddressAttribute>(
        new cricket::StunXorAddressAttribute(
            cricket::STUN_ATTR_XOR_MAPPED_ADDRESS, addr));
    msg.AddAttribute(std::move(attr));
    const cricket::StunAddressAttribute* got = msg.GetAddress(cricket::STUN_ATTR_XOR_MAPPED_ADDRESS);
    ASSERT_TRUE(got != nullptr, "get attr");
    ASSERT_EQ(got->GetAddress().ipaddr().ToString(), "192.168.1.100", "ip");
    ASSERT_EQ(got->GetAddress().port(), 3478, "port");
    TEST_PASS("test_stun_xor_address");
    return true;
}

bool test_stun_error_code() {
    cricket::StunMessage msg;
    msg.SetType(cricket::STUN_BINDING_ERROR_RESPONSE);
    auto attr = std::unique_ptr<cricket::StunErrorCodeAttribute>(
        new cricket::StunErrorCodeAttribute(
            cricket::STUN_ATTR_ERROR_CODE, 401, "Unauthorized"));
    msg.AddAttribute(std::move(attr));
    const cricket::StunErrorCodeAttribute* ec = msg.GetErrorCode();
    ASSERT_TRUE(ec != nullptr, "get error code");
    ASSERT_EQ(ec->code(), 401, "code");
    ASSERT_EQ(ec->reason(), "Unauthorized", "reason");
    ASSERT_EQ(msg.GetErrorCodeValue(), 401, "error code value");
    TEST_PASS("test_stun_error_code");
    return true;
}

bool test_stun_software() {
    cricket::StunMessage msg;
    msg.SetType(cricket::STUN_BINDING_RESPONSE);
    std::string sw_str = "test-agent 1.0";
    auto attr = std::unique_ptr<cricket::StunByteStringAttribute>(
        new cricket::StunByteStringAttribute(
            cricket::STUN_ATTR_SOFTWARE, sw_str));
    msg.AddAttribute(std::move(attr));
    const cricket::StunByteStringAttribute* sw = msg.GetByteString(cricket::STUN_ATTR_SOFTWARE);
    ASSERT_TRUE(sw != nullptr, "get software");
    ASSERT_EQ(sw->GetString(), "test-agent 1.0", "software string");
    TEST_PASS("test_stun_software");
    return true;
}

bool test_stun_fingerprint() {
    cricket::StunMessage msg;
    msg.SetType(cricket::STUN_BINDING_REQUEST);
    std::string tid(12, 'b');
    msg.SetTransactionID(tid);
    auto fp = std::unique_ptr<cricket::StunUInt32Attribute>(
        new cricket::StunUInt32Attribute(cricket::STUN_ATTR_FINGERPRINT));
    fp->SetValue(0x12345678);
    msg.AddAttribute(std::move(fp));
    const cricket::StunUInt32Attribute* got = msg.GetUInt32(cricket::STUN_ATTR_FINGERPRINT);
    ASSERT_TRUE(got != nullptr, "get fp");
    ASSERT_EQ(got->value(), 0x12345678u, "fp value");
    ASSERT_GT(msg.length(), 0u, "msg length");
    TEST_PASS("test_stun_fingerprint");
    return true;
}

bool test_stun_write_read() {
    cricket::StunMessage msg;
    msg.SetType(cricket::STUN_BINDING_REQUEST);
    std::string tid(12, 'c');
    msg.SetTransactionID(tid);
    auto attr = std::unique_ptr<cricket::StunXorAddressAttribute>(
        new cricket::StunXorAddressAttribute(
            cricket::STUN_ATTR_XOR_MAPPED_ADDRESS,
            rtc::SocketAddress("10.0.0.1", 5000)));
    msg.AddAttribute(std::move(attr));
    rtc::ByteBufferWriter buf;
    ASSERT_TRUE(msg.Write(&buf), "write");
    cricket::StunMessage parsed;
    rtc::ByteBufferReader reader(buf.Data(), buf.Length());
    ASSERT_TRUE(parsed.Read(&reader), "read");
    ASSERT_EQ(static_cast<int>(parsed.type()), cricket::STUN_BINDING_REQUEST, "parsed type");
    ASSERT_EQ(parsed.transaction_id(), tid, "parsed tid");
    TEST_PASS("test_stun_write_read");
    return true;
}

// ============================================================================
// 2. MessageDigest / 哈希
// ============================================================================

bool test_message_digest_md5() {
    std::string result = rtc::MD5("hello");
    ASSERT_FALSE(result.empty(), "md5 result not empty");
    ASSERT_EQ(result.size(), 32u, "md5 hex length");
    TEST_PASS("test_message_digest_md5");
    return true;
}

bool test_message_digest_sha1() {
    std::string result = rtc::ComputeDigest(rtc::DIGEST_SHA_1, "hello");
    ASSERT_FALSE(result.empty(), "sha1 result");
    ASSERT_EQ(result.size(), 40u, "sha1 hex length");
    TEST_PASS("test_message_digest_sha1");
    return true;
}

bool test_message_digest_factory() {
    std::unique_ptr<rtc::MessageDigest> d(rtc::MessageDigestFactory::Create(rtc::DIGEST_SHA_256));
    ASSERT_TRUE(d != nullptr, "create sha256");
    ASSERT_EQ(d->Size(), 32u, "sha256 size");
    TEST_PASS("test_message_digest_factory");
    return true;
}

bool test_hmac() {
    std::string result = rtc::ComputeHmac(rtc::DIGEST_SHA_1, "key", "data");
    ASSERT_FALSE(result.empty(), "hmac result");
    ASSERT_EQ(result.size(), 40u, "hmac hex length");
    TEST_PASS("test_hmac");
    return true;
}

// ============================================================================
// 3. 线程模块
// ============================================================================

bool test_thread_basic() {
    std::unique_ptr<rtc::Thread> thread = rtc::Thread::Create();
    thread->SetName("TestThread", nullptr);
    ASSERT_TRUE(thread->Start(), "start");
    rtc::Event done;
    thread->PostTask(RTC_FROM_HERE, [&done]() { done.Set(); });
    ASSERT_TRUE(done.Wait(1000), "task executed");
    thread->Stop();
    TEST_PASS("test_thread_basic");
    return true;
}

bool test_thread_invoke() {
    std::unique_ptr<rtc::Thread> thread = rtc::Thread::Create();
    thread->Start();
    int result = thread->Invoke<int>(RTC_FROM_HERE, []() { return 42; });
    ASSERT_EQ(result, 42, "invoke result");
    thread->Stop();
    TEST_PASS("test_thread_invoke");
    return true;
}

bool test_event() {
    rtc::Event event;
    auto start = std::chrono::steady_clock::now();
    std::thread t([&event]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        event.Set();
    });
    ASSERT_TRUE(event.Wait(1000), "event signaled");
    auto end = std::chrono::steady_clock::now();
    ASSERT_TRUE(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() >= 30, "timing");
    t.join();
    TEST_PASS("test_event");
    return true;
}

bool test_critical_section() {
    rtc::CriticalSection cs;
    int counter = 0;
    const int N = 1000;
    auto worker = [&]() {
        for (int i = 0; i < N; i++) {
            rtc::CritScope lock(&cs);
            counter++;
        }
    };
    std::thread t1(worker), t2(worker);
    t1.join(); t2.join();
    ASSERT_EQ(counter, N * 2, "counter");
    TEST_PASS("test_critical_section");
    return true;
}

// ============================================================================
// 4. TaskQueue
// ============================================================================

bool test_task_queue_factory() {
    std::unique_ptr<webrtc::TaskQueueFactory> factory = webrtc::CreateDefaultTaskQueueFactory();
    ASSERT_TRUE(factory != nullptr, "factory");
    auto tq = factory->CreateTaskQueue("test-tq", webrtc::TaskQueueFactory::Priority::NORMAL);
    ASSERT_TRUE(tq != nullptr, "tq");
    rtc::Event done;
    tq->PostTask(webrtc::ToQueuedTask([&done]() { done.Set(); }));
    ASSERT_TRUE(done.Wait(1000), "task ran");
    TEST_PASS("test_task_queue_factory");
    return true;
}

// ============================================================================
// 5. RTP 包
// ============================================================================

bool test_rtp_packet_basic() {
    webrtc::RtpPacket packet;
    packet.SetMarker(true);
    packet.SetPayloadType(96);
    packet.SetSequenceNumber(1234);
    packet.SetTimestamp(5678);
    packet.SetSsrc(0x11223344);
    ASSERT_TRUE(packet.Marker(), "marker");
    ASSERT_EQ(packet.PayloadType(), 96, "pt");
    ASSERT_EQ(packet.SequenceNumber(), 1234, "seq");
    ASSERT_EQ(packet.Timestamp(), 5678u, "ts");
    ASSERT_EQ(packet.Ssrc(), 0x11223344u, "ssrc");
    ASSERT_GT(packet.size(), 0u, "size");
    TEST_PASS("test_rtp_packet_basic");
    return true;
}

bool test_rtp_packet_payload() {
    webrtc::RtpPacket packet;
    packet.SetPayloadType(96);
    packet.SetSequenceNumber(1);
    packet.SetTimestamp(1000);
    packet.SetSsrc(1);
    uint8_t payload[100];
    memset(payload, 0xab, sizeof(payload));
    uint8_t* ptr = packet.SetPayloadSize(sizeof(payload));
    ASSERT_NE(ptr, nullptr, "set payload ptr");
    memcpy(ptr, payload, sizeof(payload));
    ASSERT_EQ(packet.payload_size(), sizeof(payload), "payload size");
    ASSERT_EQ(packet.payload()[0], 0xab, "payload[0]");
    TEST_PASS("test_rtp_packet_payload");
    return true;
}

bool test_rtp_header_extension_map() {
    webrtc::RtpHeaderExtensionMap map;
    ASSERT_TRUE(map.RegisterByType(1, webrtc::kRtpExtensionTransportSequenceNumber), "register");
    ASSERT_EQ(map.GetType(1), webrtc::kRtpExtensionTransportSequenceNumber, "get type");
    ASSERT_TRUE(map.IsRegistered(webrtc::kRtpExtensionTransportSequenceNumber), "is reg");
    ASSERT_EQ(map.Deregister(webrtc::kRtpExtensionTransportSequenceNumber), 0, "dereg");
    ASSERT_FALSE(map.IsRegistered(webrtc::kRtpExtensionTransportSequenceNumber), "not reg");
    TEST_PASS("test_rtp_header_extension_map");
    return true;
}

// ============================================================================
// 6. 音频
// ============================================================================

bool test_audio_frame() {
    webrtc::AudioFrame frame;
    frame.sample_rate_hz_ = 48000;
    frame.num_channels_ = 2;
    frame.samples_per_channel_ = 480;
    ASSERT_EQ(frame.sample_rate_hz(), 48000, "sr");
    ASSERT_EQ(frame.num_channels(), 2u, "ch");
    ASSERT_EQ(frame.samples_per_channel(), 480u, "spc");
    frame.mutable_data();
    ASSERT_TRUE(frame.data() != nullptr, "data");
    TEST_PASS("test_audio_frame");
    return true;
}

bool test_channel_layout() {
    ASSERT_EQ(webrtc::ChannelLayoutToChannelCount(webrtc::CHANNEL_LAYOUT_MONO), 1, "mono");
    ASSERT_EQ(webrtc::ChannelLayoutToChannelCount(webrtc::CHANNEL_LAYOUT_STEREO), 2, "stereo");
    ASSERT_EQ(webrtc::GuessChannelLayout(1), webrtc::CHANNEL_LAYOUT_MONO, "guess mono");
    ASSERT_EQ(webrtc::GuessChannelLayout(2), webrtc::CHANNEL_LAYOUT_STEREO, "guess stereo");
    TEST_PASS("test_channel_layout");
    return true;
}

bool test_audio_util_float_s16() {
    int16_t in[8] = {0, 100, -200, 300, 500, -1000, 2000, -3000};
    float f[8];
    int16_t out[8];
    webrtc::S16ToFloat(in, 8, f);
    webrtc::FloatToS16(f, 8, out);
    for (int i = 0; i < 8; i++)
        ASSERT_EQ(out[i], in[i], "sample " + std::to_string(i));
    TEST_PASS("test_audio_util_float_s16");
    return true;
}

bool test_signal_processing() {
    int16_t data[8] = {1, -2, 3, -4, 500, -500, 100, -100};
    int16_t max_abs = WebRtcSpl_MaxAbsValueW16(data, 8);
    ASSERT_EQ(max_abs, 500, "max abs");
    int16_t max_val = WebRtcSpl_MaxValueW16(data, 8);
    ASSERT_EQ(max_val, 500, "max val");
    int16_t min_val = WebRtcSpl_MinValueW16(data, 8);
    ASSERT_EQ(min_val, -500, "min val");
    int32_t dot = WebRtcSpl_DotProductWithScale(data, data, 8, 0);
    ASSERT_GT(dot, 0, "dot product > 0");
    int32_t energy = WebRtcSpl_DotProductWithScale(data, data, 8, 0);
    ASSERT_GT(energy, 0, "energy > 0");
    TEST_PASS("test_signal_processing");
    return true;
}

// ============================================================================
// 7. 视频
// ============================================================================

bool test_encoded_image() {
    webrtc::EncodedImage img;
    uint8_t data[64];
    memset(data, 0x42, sizeof(data));
    auto buf = webrtc::EncodedImageBuffer::Create(data, sizeof(data));
    img.SetEncodedData(buf);
    ASSERT_EQ(img.size(), sizeof(data), "size");
    ASSERT_NE(img.data(), nullptr, "data");
    ASSERT_EQ(img.data()[0], 0x42, "data[0]");
    TEST_PASS("test_encoded_image");
    return true;
}

bool test_video_rotation() {
    ASSERT_EQ(static_cast<int>(webrtc::kVideoRotation_0), 0, "rot0");
    ASSERT_EQ(static_cast<int>(webrtc::kVideoRotation_90), 90, "rot90");
    ASSERT_EQ(static_cast<int>(webrtc::kVideoRotation_180), 180, "rot180");
    ASSERT_EQ(static_cast<int>(webrtc::kVideoRotation_270), 270, "rot270");
    TEST_PASS("test_video_rotation");
    return true;
}

bool test_video_codec_type() {
    ASSERT_EQ(webrtc::kVideoCodecVP8, 1, "vp8");
    ASSERT_EQ(webrtc::kVideoCodecVP9, 2, "vp9");
    ASSERT_EQ(webrtc::kVideoCodecH264, 3, "h264");
    TEST_PASS("test_video_codec_type");
    return true;
}

bool test_h264_sps_parser() {
    uint8_t sps[] = {
        0x42, 0xc0, 0x29, 0xd9, 0x00, 0x78, 0x02,
        0x27, 0xe5, 0x84, 0x00, 0x00, 0x03, 0x00,
        0x04, 0x00, 0x00, 0x03, 0x00, 0xc2, 0x3c,
        0x3c, 0x60
    };
    auto result = webrtc::SpsParser::ParseSps(sps, sizeof(sps));
    ASSERT_TRUE(result.has_value(), "parse success");
    ASSERT_GT(result->width, 0, "width > 0");
    ASSERT_GT(result->height, 0, "height > 0");
    TEST_PASS("test_h264_sps_parser");
    return true;
}

bool test_video_bitrate_allocation() {
    webrtc::VideoBitrateAllocation alloc;
    alloc.SetBitrate(0, 0, 100000);
    alloc.SetBitrate(0, 1, 200000);
    ASSERT_EQ(alloc.GetBitrate(0, 0), 100000u, "br 0,0");
    ASSERT_EQ(alloc.GetBitrate(0, 1), 200000u, "br 0,1");
    ASSERT_EQ(alloc.get_sum_bps(), 300000u, "sum");
    TEST_PASS("test_video_bitrate_allocation");
    return true;
}

// ============================================================================
// 8. FEC
// ============================================================================

bool test_forward_error_correction() {
    std::unique_ptr<webrtc::ForwardErrorCorrection> fec =
        webrtc::ForwardErrorCorrection::CreateUlpfec(12345);
    ASSERT_TRUE(fec != nullptr, "fec created");
    ASSERT_GT(fec->MaxPacketOverhead(), 0u, "overhead > 0");
    TEST_PASS("test_forward_error_correction");
    return true;
}

bool test_ulpfec_header() {
    webrtc::UlpfecHeaderReader reader;
    webrtc::UlpfecHeaderWriter writer;
    ASSERT_GT(writer.MaxPacketOverhead(), 0u, "writer overhead > 0");
    TEST_PASS("test_ulpfec_header");
    return true;
}

bool test_flexfec_header() {
    webrtc::FlexfecHeaderReader reader;
    webrtc::FlexfecHeaderWriter writer;
    ASSERT_GT(writer.MaxPacketOverhead(), 0u, "writer overhead > 0");
    TEST_PASS("test_flexfec_header");
    return true;
}

static void fill_rtp_header(uint8_t* data, uint16_t seq, uint32_t ts, uint32_t ssrc, uint8_t pt, bool marker) {
    data[0] = 0x80;
    data[1] = pt | (marker ? 0x80 : 0x00);
    data[2] = (seq >> 8) & 0xff;
    data[3] = seq & 0xff;
    data[4] = (ts >> 24) & 0xff;
    data[5] = (ts >> 16) & 0xff;
    data[6] = (ts >> 8) & 0xff;
    data[7] = ts & 0xff;
    data[8] = (ssrc >> 24) & 0xff;
    data[9] = (ssrc >> 16) & 0xff;
    data[10] = (ssrc >> 8) & 0xff;
    data[11] = ssrc & 0xff;
}

static std::vector<uint8_t> build_h264_idr_frame() {
    std::vector<uint8_t> frame;
    auto append_start_code = [&](bool long_start) {
        if (long_start) {
            frame.push_back(0x00);
            frame.push_back(0x00);
            frame.push_back(0x00);
            frame.push_back(0x01);
        } else {
            frame.push_back(0x00);
            frame.push_back(0x00);
            frame.push_back(0x01);
        }
    };

    append_start_code(true);
    uint8_t sps[] = {
        0x67, 0x42, 0xc0, 0x29, 0xd9, 0x00, 0x78, 0x02,
        0x27, 0xe5, 0x84, 0x00, 0x00, 0x03, 0x00, 0x04,
        0x00, 0x00, 0x03, 0x00, 0xc2, 0x3c, 0x3c, 0x60
    };
    frame.insert(frame.end(), sps, sps + sizeof(sps));

    append_start_code(false);
    uint8_t pps[] = {
        0x68, 0xce, 0x3c, 0x80
    };
    frame.insert(frame.end(), pps, pps + sizeof(pps));

    append_start_code(false);
    frame.push_back(0x65);
    frame.push_back(0x88);
    frame.push_back(0x84);
    frame.push_back(0x00);
    const int idr_payload_size = 800;
    for (int i = 0; i < idr_payload_size; i++) {
        frame.push_back(static_cast<uint8_t>((i * 7 + 13) & 0xff));
    }

    return frame;
}

static std::vector<rtc::CopyOnWriteBuffer> packetize_h264_to_rtp(
    const std::vector<uint8_t>& h264_frame,
    uint32_t ssrc,
    uint32_t timestamp,
    uint8_t payload_type,
    uint16_t start_seq,
    size_t max_payload_len) {
    std::vector<rtc::CopyOnWriteBuffer> rtp_packets;

    webrtc::RTPFragmentationHeader frag_header;
    std::vector<webrtc::H264::NaluIndex> nalu_indices =
        webrtc::H264::FindNaluIndices(h264_frame.data(), h264_frame.size());
    frag_header.VerifyAndAllocateFragmentationHeader(nalu_indices.size());
    for (size_t i = 0; i < nalu_indices.size(); i++) {
        frag_header.fragmentationOffset[i] = nalu_indices[i].payload_start_offset;
        frag_header.fragmentationLength[i] = nalu_indices[i].payload_size;
    }

    webrtc::RtpPacketizerH264::PayloadSizeLimits limits;
    limits.max_payload_len = static_cast<int>(max_payload_len);
    limits.first_packet_reduction_len = 0;
    limits.last_packet_reduction_len = 0;
    limits.single_packet_reduction_len = 0;

    webrtc::RtpPacketizerH264 packetizer(
        rtc::MakeArrayView(h264_frame.data(), h264_frame.size()),
        limits,
        webrtc::H264PacketizationMode::NonInterleaved,
        frag_header);

    size_t num_packets = packetizer.NumPackets();
    uint16_t seq = start_seq;

    webrtc::RtpPacketToSend rtp_packet(nullptr);
    for (size_t i = 0; i < num_packets; i++) {
        rtp_packet.SetPayloadType(payload_type);
        rtp_packet.SetSequenceNumber(seq);
        rtp_packet.SetTimestamp(timestamp);
        rtp_packet.SetSsrc(ssrc);
        bool marker = false;
        bool ret = packetizer.NextPacket(&rtp_packet);
        if (!ret) break;
        if (i == num_packets - 1) {
            rtp_packet.SetMarker(true);
        }
        rtc::CopyOnWriteBuffer buf(rtp_packet.data(), rtp_packet.size());
        rtp_packets.push_back(buf);
        seq++;
    }

    return rtp_packets;
}

bool test_ulpfec_encode_decode() {
    const uint32_t kSsrc = 12345;
    const uint8_t kPt = 96;
    const uint32_t kTimestamp = 10000;

    std::vector<uint8_t> h264_frame = build_h264_idr_frame();
    ASSERT_GT(h264_frame.size(), 0u, "h264 frame built");

    std::vector<rtc::CopyOnWriteBuffer> rtp_packets = packetize_h264_to_rtp(
        h264_frame, kSsrc, kTimestamp, kPt, 0, 500);
    ASSERT_GE(rtp_packets.size(), 3u, "at least 3 rtp packets");
    size_t num_media = rtp_packets.size();

    auto fec = webrtc::ForwardErrorCorrection::CreateUlpfec(kSsrc);
    ASSERT_TRUE(fec != nullptr, "create ulpfec");

    webrtc::ForwardErrorCorrection::PacketList media_packets;
    for (const auto& pkt_buf : rtp_packets) {
        auto pkt = std::make_unique<webrtc::ForwardErrorCorrection::Packet>();
        pkt->data = pkt_buf;
        media_packets.push_back(std::move(pkt));
    }

    std::list<webrtc::ForwardErrorCorrection::Packet*> fec_packets;
    int ret = fec->EncodeFec(media_packets, 127, 0, false, webrtc::kFecMaskRandom, &fec_packets);
    ASSERT_EQ(ret, 0, "encode fec");
    ASSERT_GT(fec_packets.size(), 0u, "fec packets generated");

    webrtc::ForwardErrorCorrection::RecoveredPacketList recovered;
    fec->ResetState(&recovered);

    size_t drop_idx = 1;
    for (size_t i = 0; i < num_media; i++) {
        if (i == drop_idx) continue;
        auto rp = std::make_unique<webrtc::ForwardErrorCorrection::ReceivedPacket>();
        rp->ssrc = kSsrc;
        rp->seq_num = webrtc::ForwardErrorCorrection::ParseSequenceNumber(
            const_cast<uint8_t*>(rtp_packets[i].data()));
        rp->is_fec = false;
        rp->pkt = new rtc::RefCountedObject<webrtc::ForwardErrorCorrection::Packet>();
        rp->pkt->data = rtp_packets[i];
        fec->DecodeFec(*rp, &recovered);
    }

    for (auto& fec_pkt : fec_packets) {
        auto rp = std::make_unique<webrtc::ForwardErrorCorrection::ReceivedPacket>();
        rp->ssrc = kSsrc;
        rp->seq_num = webrtc::ForwardErrorCorrection::ParseSequenceNumber(fec_pkt->data.data());
        rp->is_fec = true;
        rp->pkt = new rtc::RefCountedObject<webrtc::ForwardErrorCorrection::Packet>();
        rp->pkt->data = fec_pkt->data;
        fec->DecodeFec(*rp, &recovered);
    }

    int recovered_count = 0;
    for (const auto& rp : recovered) {
        if (rp->was_recovered) recovered_count++;
    }
    ASSERT_GE(recovered_count, 1, "recovered at least 1 packet");
    ASSERT_EQ(recovered.size(), num_media, "all media packets present");

    TEST_PASS("test_ulpfec_encode_decode");
    return true;
}

bool test_flexfec_encode_decode() {
    const uint32_t kFecSsrc = 54321;
    const uint32_t kMediaSsrc = 12345;
    const uint8_t kPt = 96;
    const uint32_t kTimestamp = 20000;

    std::vector<uint8_t> h264_frame = build_h264_idr_frame();
    ASSERT_GT(h264_frame.size(), 0u, "h264 frame built");

    std::vector<rtc::CopyOnWriteBuffer> rtp_packets = packetize_h264_to_rtp(
        h264_frame, kMediaSsrc, kTimestamp, kPt, 100, 500);
    ASSERT_GE(rtp_packets.size(), 3u, "at least 3 rtp packets");
    size_t num_media = rtp_packets.size();

    auto fec = webrtc::ForwardErrorCorrection::CreateFlexfec(kFecSsrc, kMediaSsrc);
    ASSERT_TRUE(fec != nullptr, "create flexfec");

    webrtc::ForwardErrorCorrection::PacketList media_packets;
    for (const auto& pkt_buf : rtp_packets) {
        auto pkt = std::make_unique<webrtc::ForwardErrorCorrection::Packet>();
        pkt->data = pkt_buf;
        media_packets.push_back(std::move(pkt));
    }

    std::list<webrtc::ForwardErrorCorrection::Packet*> fec_packets;
    int ret = fec->EncodeFec(media_packets, 127, 0, false, webrtc::kFecMaskRandom, &fec_packets);
    ASSERT_EQ(ret, 0, "encode flexfec");
    ASSERT_GT(fec_packets.size(), 0u, "flexfec packets generated");

    webrtc::ForwardErrorCorrection::RecoveredPacketList recovered;
    fec->ResetState(&recovered);

    size_t drop_idx = 2;
    for (size_t i = 0; i < num_media; i++) {
        if (i == drop_idx) continue;
        auto rp = std::make_unique<webrtc::ForwardErrorCorrection::ReceivedPacket>();
        rp->ssrc = kMediaSsrc;
        rp->seq_num = webrtc::ForwardErrorCorrection::ParseSequenceNumber(
            const_cast<uint8_t*>(rtp_packets[i].data()));
        rp->is_fec = false;
        rp->pkt = new rtc::RefCountedObject<webrtc::ForwardErrorCorrection::Packet>();
        rp->pkt->data = rtp_packets[i];
        fec->DecodeFec(*rp, &recovered);
    }

    for (auto& fec_pkt : fec_packets) {
        auto rp = std::make_unique<webrtc::ForwardErrorCorrection::ReceivedPacket>();
        rp->ssrc = kFecSsrc;
        rp->seq_num = webrtc::ForwardErrorCorrection::ParseSequenceNumber(fec_pkt->data.data());
        rp->is_fec = true;
        rp->pkt = new rtc::RefCountedObject<webrtc::ForwardErrorCorrection::Packet>();
        rp->pkt->data = fec_pkt->data;
        fec->DecodeFec(*rp, &recovered);
    }

    int recovered_count = 0;
    for (const auto& rp : recovered) {
        if (rp->was_recovered) recovered_count++;
    }
    ASSERT_GE(recovered_count, 1, "recovered at least 1 packet");

    TEST_PASS("test_flexfec_encode_decode");
    return true;
}

bool test_flexfec_sender_basic() {
    webrtc::FlexfecSender sender(
        110,
        54321,
        12345,
        "test-mid",
        std::vector<webrtc::RtpExtension>(),
        rtc::ArrayView<const webrtc::RtpExtensionSize>(),
        nullptr,
        webrtc::Clock::GetRealTimeClock());
    ASSERT_EQ(sender.ssrc(), 54321u, "ssrc");
    ASSERT_GT(sender.MaxPacketOverhead(), 0u, "overhead > 0");
    ASSERT_FALSE(sender.FecAvailable(), "no fec initially");

    webrtc::FecProtectionParams params;
    params.fec_rate = 50;
    params.max_fec_frames = 1;
    params.fec_mask_type = webrtc::kFecMaskRandom;
    sender.SetFecParameters(params);

    TEST_PASS("test_flexfec_sender_basic");
    return true;
}

bool test_ulpfec_generator_basic() {
    webrtc::UlpfecGenerator generator;
    ASSERT_GT(generator.MaxPacketOverhead(), 0u, "overhead > 0");
    ASSERT_FALSE(generator.FecAvailable(), "no fec initially");
    ASSERT_EQ(generator.NumAvailableFecPackets(), 0u, "0 fec packets");

    webrtc::FecProtectionParams params;
    params.fec_rate = 50;
    params.max_fec_frames = 1;
    params.fec_mask_type = webrtc::kFecMaskRandom;
    generator.SetFecParameters(params);

    TEST_PASS("test_ulpfec_generator_basic");
    return true;
}

// ============================================================================
// 9. GCC 拥塞控制
// ============================================================================

bool test_goog_cc_factory() {
    webrtc::GoogCcFactoryConfig config;
    auto factory = std::make_unique<webrtc::GoogCcNetworkControllerFactory>(std::move(config));
    ASSERT_TRUE(factory != nullptr, "factory");
    webrtc::NetworkControllerConfig cc_config;
    cc_config.constraints.at_time = webrtc::Timestamp::ms(0);
    cc_config.stream_based_config = webrtc::StreamsConfig();
    auto controller = factory->Create(cc_config);
    ASSERT_TRUE(controller != nullptr, "controller");
    TEST_PASS("test_goog_cc_factory");
    return true;
}

bool test_units() {
    using namespace webrtc;
    auto rate = DataRate::bps(1000000);
    ASSERT_EQ(rate.bps<int64_t>(), 1000000, "bps");
    ASSERT_EQ(rate.kbps<int64_t>(), 1000, "kbps");
    auto delta = TimeDelta::ms(500);
    ASSERT_EQ(delta.ms<int64_t>(), 500, "ms");
    auto size = DataSize::bytes(1500);
    ASSERT_EQ(size.bytes<int64_t>(), 1500, "bytes");
    auto ts = Timestamp::ms(10000);
    ASSERT_EQ(ts.ms<int64_t>(), 10000, "ts ms");
    ASSERT_TRUE(ts.IsFinite(), "finite");
    TEST_PASS("test_units");
    return true;
}

// ============================================================================
// 10. 系统封装
// ============================================================================

bool test_clock() {
    webrtc::Clock* clock = webrtc::Clock::GetRealTimeClock();
    ASSERT_TRUE(clock != nullptr, "clock");
    int64_t t1 = clock->TimeInMilliseconds();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    int64_t t2 = clock->TimeInMilliseconds();
    ASSERT_GE(t2 - t1, 5, "time advances");
    int64_t ntp = clock->CurrentNtpInMilliseconds();
    ASSERT_GT(ntp, 0, "ntp > 0");
    TEST_PASS("test_clock");
    return true;
}

// ============================================================================
// 11. 日志
// ============================================================================

bool test_logging() {
    rtc::LogMessage::LogToDebug(rtc::LS_INFO);
    RTC_LOG(LS_INFO) << "test info";
    RTC_LOG(LS_WARNING) << "test warning";
    RTC_LOG(LS_ERROR) << "test error";
    TEST_PASS("test_logging");
    return true;
}

// ============================================================================
// 12. IP / Socket 地址
// ============================================================================

bool test_ip_address() {
    rtc::IPAddress ip4(0x7f000001);
    ASSERT_EQ(ip4.family(), AF_INET, "ipv4 family");
    ASSERT_EQ(ip4.ToString(), "127.0.0.1", "ipv4 string");
    rtc::IPAddress ip6;
    ASSERT_TRUE(ip6.IsNil(), "nil ipv6");
    TEST_PASS("test_ip_address");
    return true;
}

bool test_socket_address() {
    rtc::SocketAddress addr("192.168.0.1", 8080);
    ASSERT_EQ(addr.ipaddr().ToString(), "192.168.0.1", "ip");
    ASSERT_EQ(addr.port(), 8080, "port");
    ASSERT_EQ(addr.ToString(), "192.168.0.1:8080", "full string");
    TEST_PASS("test_socket_address");
    return true;
}

// ============================================================================
// 主函数
// ============================================================================

int main() {
    std::cout << "==================================================" << std::endl;
    std::cout << " libwebrtc 全量接口测试 Demo" << std::endl;
    std::cout << "==================================================" << std::endl;

    std::cout << "\n--- [1] STUN 模块 ---" << std::endl;
    RUN_TEST(test_stun_basic);
    RUN_TEST(test_stun_xor_address);
    RUN_TEST(test_stun_error_code);
    RUN_TEST(test_stun_software);
    RUN_TEST(test_stun_fingerprint);
    RUN_TEST(test_stun_write_read);

    std::cout << "\n--- [2] 消息摘要 / 哈希 ---" << std::endl;
    RUN_TEST(test_message_digest_md5);
    RUN_TEST(test_message_digest_sha1);
    RUN_TEST(test_message_digest_factory);
    RUN_TEST(test_hmac);

    std::cout << "\n--- [3] 线程模块 ---" << std::endl;
    RUN_TEST(test_thread_basic);
    RUN_TEST(test_thread_invoke);
    RUN_TEST(test_event);
    RUN_TEST(test_critical_section);

    std::cout << "\n--- [4] TaskQueue ---" << std::endl;
    RUN_TEST(test_task_queue_factory);

    std::cout << "\n--- [5] RTP 模块 ---" << std::endl;
    RUN_TEST(test_rtp_packet_basic);
    RUN_TEST(test_rtp_packet_payload);
    RUN_TEST(test_rtp_header_extension_map);

    std::cout << "\n--- [6] 音频模块 ---" << std::endl;
    RUN_TEST(test_audio_frame);
    RUN_TEST(test_channel_layout);
    RUN_TEST(test_audio_util_float_s16);
    RUN_TEST(test_signal_processing);

    std::cout << "\n--- [7] 视频模块 ---" << std::endl;
    RUN_TEST(test_encoded_image);
    RUN_TEST(test_video_rotation);
    RUN_TEST(test_video_codec_type);
    RUN_TEST(test_h264_sps_parser);
    RUN_TEST(test_video_bitrate_allocation);

    std::cout << "\n--- [8] FEC 模块 ---" << std::endl;
    RUN_TEST(test_forward_error_correction);
    RUN_TEST(test_ulpfec_header);
    RUN_TEST(test_flexfec_header);
    RUN_TEST(test_ulpfec_generator_basic);
    RUN_TEST(test_flexfec_sender_basic);
    RUN_TEST(test_ulpfec_encode_decode);
    RUN_TEST(test_flexfec_encode_decode);

    std::cout << "\n--- [9] GCC 拥塞控制 ---" << std::endl;
    RUN_TEST(test_goog_cc_factory);
    RUN_TEST(test_units);

    std::cout << "\n--- [10] 系统封装 ---" << std::endl;
    RUN_TEST(test_clock);

    std::cout << "\n--- [11] 日志模块 ---" << std::endl;
    RUN_TEST(test_logging);

    std::cout << "\n--- [12] IP/Socket 地址 ---" << std::endl;
    RUN_TEST(test_ip_address);
    RUN_TEST(test_socket_address);

    std::cout << "\n==================================================" << std::endl;
    std::cout << " 总计: " << (g_pass + g_fail)
              << "  通过: " << g_pass
              << "  失败: " << g_fail << std::endl;
    std::cout << "==================================================" << std::endl;

    return g_fail == 0 ? 0 : 1;
}
