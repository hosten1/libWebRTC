/*
 * Test program for RTP PT manipulator functionality
 */

#include <iostream>
#include <memory>
#include "modules/rtp_rtcp/source/rtp_pt_manipulator_impl.h"
#include "modules/rtp_rtcp/source/rtp_packet.h"

int main() {
  std::cout << "=== RTP PT Manipulator Test ===" << std::endl;
  
  // Step 1: Configure SDP media description (original values)
  webrtc::SdpMediaDescription sdp;
  sdp.media_type = "video";
  sdp.port = 7;
  sdp.protocol = "RTP/SAVPF";
  sdp.payload_types = {124, 127, 97, 123};
  sdp.rtpmap[124] = "red/90000";
  sdp.rtpmap[127] = "VP9/90000";
  sdp.rtpmap[97] = "rtx/90000";
  sdp.rtpmap[123] = "ulpfec/90000";
  sdp.address = "IN IP4 127.0.0.1";
  
  std::cout << "\n1. Configured SDP (original):" << std::endl;
  std::cout << "   m=video 7 RTP/SAVPF 124 127 97 123" << std::endl;
  std::cout << "   c=IN IP4 127.0.0.1" << std::endl;
  std::cout << "   a=rtpmap:127 VP9/90000" << std::endl;
  std::cout << "   a=rtpmap:97 rtx/90000" << std::endl;
  std::cout << "   a=rtpmap:124 red/90000" << std::endl;
  std::cout << "   a=rtpmap:123 ulpfec/90000" << std::endl;
  
  // Create manipulator with original SDP
  std::unique_ptr<webrtc::RtpPtManipulatorImpl> manipulator(
      new webrtc::RtpPtManipulatorImpl());
  manipulator->ConfigureSdp(sdp);
  
  // Step 2: Create test RTP packets
  std::cout << "\n2. Creating test RTP packets..." << std::endl;
  
  // Create RED packet with embedded ULPFEC and VP9
  webrtc::RtpPacket red_packet;
  rtc::CopyOnWriteBuffer red_buffer;
  red_buffer.EnsureCapacity(100);
  uint8_t* data = red_buffer.data();
  
  // RTP header (12 bytes)
  data[0] = 0x80;  // Version=2, padding=0, extension=0, CSRC count=0
  data[1] = 124;   // Marker=0, Payload Type=124 (RED)
  data[2] = 0x00; data[3] = 0x01;  // Sequence number
  data[4] = 0x00; data[5] = 0x00; data[6] = 0x00; data[7] = 0x01;  // Timestamp
  data[8] = 0x00; data[9] = 0x00; data[10] = 0x00; data[11] = 0x01;  // SSRC
  
  // RED payload: ULPFEC block followed by VP9 block
  size_t payload_offset = 12;
  
  // ULPFEC block in RED: marker=0, pt=123, length=10
  data[payload_offset++] = 0x7B;  // pt=123 (ulpfec)
  data[payload_offset++] = 0x00;  // length high byte
  data[payload_offset++] = 0x0A;  // length low byte = 10
  // ULPFEC payload (10 bytes minimum)
  data[payload_offset++] = 0x00;  // F bit=0, reserved=0, L bit=0
  data[payload_offset++] = 0x7F;  // PT recovery=127 (VP9), mask=0
  for (int i = 0; i < 8; i++) {
    data[payload_offset++] = 0x00;
  }
  
  // VP9 block in RED: marker=1 (last), pt=127, length=5
  data[payload_offset++] = 0xFF;  // marker=1, pt=127 (VP9)
  data[payload_offset++] = 0x00;  // length high byte
  data[payload_offset++] = 0x05;  // length low byte = 5
  // VP9 payload
  for (int i = 0; i < 5; i++) {
    data[payload_offset++] = 0xAA;
  }
  
  red_buffer.SetSize(payload_offset);
  bool parsed = red_packet.Parse(red_buffer);
  
  if (!parsed) {
    std::cout << "Failed to parse RED packet!" << std::endl;
    return 1;
  }
  std::cout << "   RED packet created successfully" << std::endl;
  
  // Step 3: Parse PT values
  std::cout << "\n3. Parsing PT values from RED packet..." << std::endl;
  webrtc::RtpPayloadTypes pt_values = manipulator->ParsePtValues(red_packet);
  
  std::cout << "   Parsed PT values:" << std::endl;
  if (pt_values.red_pt) std::cout << "     RED PT: " << static_cast<int>(*pt_values.red_pt) << std::endl;
  if (pt_values.ulpfec_pt) std::cout << "     ULPFEC PT: " << static_cast<int>(*pt_values.ulpfec_pt) << std::endl;
  if (pt_values.vp9_pt) std::cout << "     VP9 PT: " << static_cast<int>(*pt_values.vp9_pt) << std::endl;
  if (pt_values.rtx_pt) std::cout << "     RTX PT: " << static_cast<int>(*pt_values.rtx_pt) << std::endl;
  
  // Verify initial values
  bool test1_pass = true;
  if (!pt_values.red_pt || *pt_values.red_pt != 124) {
    std::cout << "   FAIL: RED PT should be 124" << std::endl;
    test1_pass = false;
  }
  if (!pt_values.ulpfec_pt || *pt_values.ulpfec_pt != 123) {
    std::cout << "   FAIL: ULPFEC PT should be 123" << std::endl;
    test1_pass = false;
  }
  if (!pt_values.vp9_pt || *pt_values.vp9_pt != 127) {
    std::cout << "   FAIL: VP9 PT should be 127" << std::endl;
    test1_pass = false;
  }
  if (test1_pass) {
    std::cout << "   PASS: All PT values correctly parsed" << std::endl;
  }
  
  // Step 4: Modify PT values
  std::cout << "\n4. Modifying PT values..." << std::endl;
  std::cout << "   New PT mapping:" << std::endl;
  std::cout << "     RED: 124 -> 104" << std::endl;
  std::cout << "     ULPFEC: 123 -> 106" << std::endl;
  std::cout << "     VP9: 127 -> 102" << std::endl;
  std::cout << "     RTX: 97 -> 103" << std::endl;
  
  webrtc::RtpPayloadTypes new_pt_values;
  new_pt_values.red_pt = 104;
  new_pt_values.ulpfec_pt = 106;
  new_pt_values.vp9_pt = 102;
  // 不设置 rtx_pt，因为测试包中没有 RTX 内容
  
  bool modified = manipulator->ModifyPtValues(&red_packet, new_pt_values);
  if (!modified) {
    std::cout << "   FAIL: Failed to modify RTP packet" << std::endl;
    return 1;
  }
  std::cout << "   PASS: RTP packet modified successfully" << std::endl;
  
  // Step 5: Update SDP configuration with new PT values for verification
  std::cout << "\n5. Updating SDP configuration for verification..." << std::endl;
  webrtc::SdpMediaDescription new_sdp;
  new_sdp.media_type = "video";
  new_sdp.port = 7;
  new_sdp.protocol = "RTP/SAVPF";
  new_sdp.payload_types = {102, 103, 104, 106};
  new_sdp.rtpmap[104] = "red/90000";
  new_sdp.rtpmap[102] = "VP9/90000";
  new_sdp.rtpmap[103] = "rtx/90000";
  new_sdp.rtpmap[106] = "ulpfec/90000";
  new_sdp.address = "IN IP4 127.0.0.1";
  
  manipulator->ConfigureSdp(new_sdp);
  
  // Step 6: Verify modification
  std::cout << "\n6. Verifying modification..." << std::endl;
  webrtc::RtpPayloadTypes modified_pt_values = manipulator->ParsePtValues(red_packet);
  
  std::cout << "   Modified PT values:" << std::endl;
  if (modified_pt_values.red_pt) std::cout << "     RED PT: " << static_cast<int>(*modified_pt_values.red_pt) << std::endl;
  if (modified_pt_values.ulpfec_pt) std::cout << "     ULPFEC PT: " << static_cast<int>(*modified_pt_values.ulpfec_pt) << std::endl;
  if (modified_pt_values.vp9_pt) std::cout << "     VP9 PT: " << static_cast<int>(*modified_pt_values.vp9_pt) << std::endl;
  if (modified_pt_values.rtx_pt) std::cout << "     RTX PT: " << static_cast<int>(*modified_pt_values.rtx_pt) << std::endl;
  
  // Verify modification
  bool test2_pass = manipulator->VerifyModification(red_packet, new_pt_values);
  
  if (test2_pass) {
    std::cout << "   PASS: Modification verified successfully" << std::endl;
  } else {
    std::cout << "   FAIL: Modification verification failed" << std::endl;
    return 1;
  }
  
  // Step 7: Verify updated SDP mapping
  std::cout << "\n7. New SDP would be:" << std::endl;
  std::cout << "   m=video 7 RTP/SAVPF 102 103 104 106" << std::endl;
  std::cout << "   c=IN IP4 127.0.0.1" << std::endl;
  std::cout << "   a=rtpmap:102 VP9/90000" << std::endl;
  std::cout << "   a=rtpmap:103 rtx/90000" << std::endl;
  std::cout << "   a=rtpmap:104 red/90000" << std::endl;
  std::cout << "   a=rtpmap:106 ulpfec/90000" << std::endl;
  
  std::cout << "\n=== All tests completed ===" << std::endl;
  std::cout << (test1_pass && test2_pass ? "SUCCESS" : "FAILURE") << std::endl;
  
  return 0;
}
