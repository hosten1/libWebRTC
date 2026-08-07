# ==================== [call] 编译诊断 ====================
message(STATUS "[call] ====== Module: call ======")
message(STATUS "[call] OS=${CMAKE_SYSTEM_NAME} ARCH=${CURRENT_CPU}")
message(STATUS "[call] Feature: GCC=${WEBRTC_BUILD_GCC}")

# Call module - RTP transport controller
set(WEBRTC_CALL_SOURCES
    ${WEBRTC_SRC_DIR}/call/rtp_transport_controller_send.cc
)

add_library(call_objects OBJECT ${WEBRTC_CALL_SOURCES})

target_include_directories(call_objects PRIVATE
    ${WEBRTC_SRC_DIR}
    ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

list(LENGTH WEBRTC_CALL_SOURCES _total)
message(STATUS "[call] Total source files: ${_total}")
message(STATUS "[call] Source: rtp_transport_controller_send.cc")
message(STATUS "[call] ====== End of call ======")