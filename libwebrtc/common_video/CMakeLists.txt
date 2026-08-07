# ==================== [common_video] 编译诊断 ====================
message(STATUS "[common_video] ====== Module: common_video ======")
message(STATUS "[common_video] OS=${CMAKE_SYSTEM_NAME} ARCH=${CURRENT_CPU}")

# Common video module - H264 parsing and generic frame descriptor
set(WEBRTC_COMMON_VIDEO_SOURCES
    ${WEBRTC_SRC_DIR}/common_video/h264/h264_bitstream_parser.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/h264_common.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/pps_parser.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/sps_parser.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/sps_vui_rewriter.cc
    ${WEBRTC_SRC_DIR}/common_video/generic_frame_descriptor/generic_frame_info.cc
)

add_library(common_video OBJECT ${WEBRTC_COMMON_VIDEO_SOURCES})

target_include_directories(common_video PRIVATE
    ${WEBRTC_SRC_DIR}
    ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

list(LENGTH WEBRTC_COMMON_VIDEO_SOURCES _total)
message(STATUS "[common_video] Total source files: ${_total}")
message(STATUS "[common_video] Sources: h264_bitstream_parser, h264_common, pps_parser, sps_parser, sps_vui_rewriter, generic_frame_info")
message(STATUS "[common_video] ====== End of common_video ======")