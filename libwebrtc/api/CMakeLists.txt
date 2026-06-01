

set(WEBRTC_API_SOURCES

    ${WEBRTC_SRC_DIR}/api/transport/bitrate_settings.cc
    ${WEBRTC_SRC_DIR}/api/transport/field_trial_based_config.cc
    ${WEBRTC_SRC_DIR}/api/units/timestamp.cc
    ${WEBRTC_SRC_DIR}/api/units/time_delta.cc
    ${WEBRTC_SRC_DIR}/api/units/data_rate.cc
    ${WEBRTC_SRC_DIR}/api/units/data_size.cc
    ${WEBRTC_SRC_DIR}/api/units/frequency.cc

    ${WEBRTC_SRC_DIR}/api/media_types.cc
    ${WEBRTC_SRC_DIR}/api/rtp_headers.cc
    ${WEBRTC_SRC_DIR}/api/rtp_parameters.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_decoder.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_format.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_codec_pair_id.cc
    ${WEBRTC_SRC_DIR}/api/rtp_packet_info.cc

    ${WEBRTC_SRC_DIR}/api/transport/network_types.cc
    ${WEBRTC_SRC_DIR}/api/network_state_predictor.cc

    ${WEBRTC_SRC_DIR}/api/video/color_space.cc
   
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_encoder.cc


)
# webrtc threading modules
if(WEBRTC_BUILD_WEBRTC_THREAD)
    message(STATUS "[libwebrtc] Adding WebRTC threading modules...")
    list(APPEND WEBRTC_API_SOURCES
    
    ${WEBRTC_SRC_DIR}/api/task_queue/task_queue_base.cc
    )

    if(WIN32)
            list(APPEND WEBRTC_API_SOURCES
                ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_win.cc
                )
    elseif(APPLE)
            list(APPEND WEBRTC_API_SOURCES
                ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_gcd.cc
            )
    elseif(LINUX AND UNIX)
        list(APPEND WEBRTC_API_SOURCES
                ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_stdlib.cc

            )
    elseif(LINUX AND UNIX)
            list(APPEND WEBRTC_API_SOURCES
                ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_libevent.cc
            )
    else()
            list(APPEND WEBRTC_API_SOURCES
                ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_stdlib.cc
            )
        message(STATUS "[libwebrtc] WebRTC threading modules disabled")
    endif()
else()
    message(STATUS "[libwebrtc] WebRTC threading modules disabled")
endif()


# webrtc stun modules end
if(WEBRTC_BUILD_WEBRTC_STUN)
    message(STATUS "[libwebrtc] Adding STUN modules...")
    list(APPEND WEBRTC_API_SOURCES
        ${WEBRTC_SRC_DIR}/api/audio/audio_frame.cc
        ${WEBRTC_SRC_DIR}/api/audio/channel_layout.cc
    )
else()
    message(STATUS "[libwebrtc api] STUN modules disabled")
endif()

# video modules
if(WEBRTC_BUILD_VIDEO)
    message(STATUS "[libwebrtc] Adding video modules...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/api/video/encoded_image.cc
        ${WEBRTC_SRC_DIR}/api/video/video_bitrate_allocation.cc
        ${WEBRTC_SRC_DIR}/api/video/video_frame_buffer.cc
        ${WEBRTC_SRC_DIR}/api/video_codecs/video_codec.cc
        ${WEBRTC_SRC_DIR}/api/video_codecs/video_decoder.cc
        ${WEBRTC_SRC_DIR}/api/video/video_timing.cc
        ${WEBRTC_SRC_DIR}/api/video/video_content_type.cc
        ${WEBRTC_SRC_DIR}/api/video/hdr_metadata.cc
        ${WEBRTC_SRC_DIR}/api/video/color_space.cc
    )
    message(STATUS "[libwebrtc] Video modules added: vp8, vp9, h264 codecs")
else()
    message(STATUS "[libwebrtc] Video modules disabled")
endif()

# gcc modules
if(WEBRTC_BUILD_GCC)
    message(STATUS "[libwebrtc] Adding GCC modules...")
    list(APPEND WEBRTC_API_SOURCES
        ${WEBRTC_SRC_DIR}/api/transport/goog_cc_factory.cc
    )
    message(STATUS "[libwebrtc] GCC modules added: audio_encoder_copy_red.cc")
else()
    message(STATUS "[libwebrtc] GCC modules disabled")
endif()


# add_library(api STATIC ${WEBRTC_API_SOURCES})
add_library(api OBJECT
    ${WEBRTC_API_SOURCES}
)

if(MSVC)
    target_compile_options(api PRIVATE /W0 /utf-8)
else()
    target_compile_options(api PRIVATE -w -include cstdint)
endif()

if(WIN32)
    # Windows 平台（桌面、UWP等）
    message(STATUS "[api] Platform: Windows")
    target_compile_definitions(api PRIVATE
        WEBRTC_WIN
        DISABLE_H265
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )
elseif(ANDROID)
    # Android 平台（优先于 UNIX 判断）
    message(STATUS "[api] Platform: Android")
    target_compile_definitions(api PRIVATE
        WEBRTC_POSIX
        WEBRTC_ANDROID
        DISABLE_H265
    )
elseif(APPLE)
    # Apple 平台（macOS、iOS、tvOS、watchOS）
    if(IOS)
        message(STATUS "[api] Platform: iOS")
        target_compile_definitions(api PRIVATE
            WEBRTC_POSIX
            WEBRTC_IOS
            DISABLE_H265
        )
    else()
        message(STATUS "[api] Platform: macOS")
        target_compile_definitions(api PRIVATE
            WEBRTC_POSIX
            WEBRTC_MAC
            DISABLE_H265
        )
    endif()
elseif(UNIX)
    # 其他类 Unix 系统（Linux 等）
    message(STATUS "[api] Platform: Linux/Unix")
    target_compile_definitions(api PRIVATE
        WEBRTC_POSIX
        WEBRTC_LINUX
        DISABLE_H265
    )
else()
    message(WARNING "[api] Unknown platform!")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    target_compile_definitions(api PRIVATE MS_BIG_ENDIAN)
else()
    target_compile_definitions(api PRIVATE MS_LITTLE_ENDIAN) 
endif()

target_include_directories(
    api
    PRIVATE
        ${WEBRTC_SRC_DIR}
         ${DEPS_DIR}/abseil-cpp/abseil-cpp
)