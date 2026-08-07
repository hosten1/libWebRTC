libwebrtc_sources = [
  'src/system_wrappers/source/sleep.cc',
      'src/rtc_base/bit_buffer.cc',
      'src/rtc_base/byte_buffer.cc',
      'src/rtc_base/checks.cc',
      'src/rtc_base/copy_on_write_buffer.cc',
      'src/rtc_base/cpu_time.cc',
      'src/rtc_base/crc32.cc',
      'src/rtc_base/crypt_string.cc',
      'src/rtc_base/data_rate_limiter.cc',
      'src/rtc_base/flags.cc',
      'src/rtc_base/random.cc',
      'src/rtc_base/string_encode.cc',
      'src/rtc_base/string_to_number.cc',
      'src/rtc_base/string_utils.cc',
      'src/rtc_base/string_view.cc',
      'src/rtc_base/time_utils.cc',
      'src/rtc_base/zero_memory.cc',
      'src/rtc_base/strings/string_builder.cc',
      'src/modules/audio_coding/neteq/packet.cc',
      'src/modules/audio_coding/neteq/red_payload_splitter.cc',
      'src/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.cc',
      'src/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.cc',
      'src/modules/rtp_rtcp/source/rtp_header_extension_map.cc',
      'src/modules/rtp_rtcp/source/rtp_header_extension_size.cc',
      'src/modules/rtp_rtcp/source/rtp_header_extensions.cc',
      'src/modules/rtp_rtcp/source/rtp_packet_received.cc',
      'src/modules/rtp_rtcp/source/rtp_packet.cc',
      'src/modules/rtp_rtcp/source/time_util.cc',
      'src/modules/rtp_rtcp/include/rtp_rtcp_defines.cc',
      'src/api/media_types.cc'
      'src/api/rtp_headers.cc'
      'src/api/rtp_parameters.cc'
      'src/api/audio_codecs/audio_decoder.cc'
      'src/api/transport/network_types.cc'
      'src/api/units/data_rate.cc'
      'src/api/units/data_size.cc'
      'src/api/units/time_delta.cc'
      'src/api/units/timestamp.cc'
      'src/api/video/color_space.cc'
      'src/api/video/hdr_metadata.cc'
      'src/api/video/video_content_type.cc'
      'src/api/video/video_timing.cc'
]

abseil_cpp_proj = subproject(
  'abseil-cpp',
  default_options: [
    'warning_level=0',
  ],
)

local_include_directories = declare_dependency(
  include_directories: include_directories('libwebrtc/src')
)

libwebrtc = library(
  'libwebrtc',
  libwebrtc_sources,
  dependencies: [
    local_include_directories,
  ],
  include_directories: libwebrtc_include_directories,
  cpp_args: cpp_args,
)

libwebrtc_dep = declare_dependency(
  dependencies: [
    local_include_directories,
  ],
  include_directories: include_directories('.'),
  link_with: libwebrtc,
)
