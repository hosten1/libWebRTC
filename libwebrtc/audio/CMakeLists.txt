# ==================== [audio] 编译诊断 ====================
message(STATUS "[audio] ====== Module: audio ======")
message(STATUS "[audio] OS=${CMAKE_SYSTEM_NAME} ARCH=${CURRENT_CPU}")
message(STATUS "[audio] Feature: AUDIO_TOOL=${WEBRTC_BUILD_WEBRTC_AUDIO_TOOL}")

# Audio module
set(WEBRTC_AUDIO_SOURCES
    ${WEBRTC_SRC_DIR}/audio/audio_level.cc
)

add_library(audio_objects OBJECT ${WEBRTC_AUDIO_SOURCES})

target_include_directories(audio_objects PRIVATE
    ${WEBRTC_SRC_DIR}
    ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

list(LENGTH WEBRTC_AUDIO_SOURCES _total)
message(STATUS "[audio] Total source files: ${_total}")
message(STATUS "[audio] Source: audio_level.cc")
message(STATUS "[audio] ====== End of audio ======")