
# ==================== [api] 编译诊断 ====================
message(STATUS "[api] ====== Module: api ======")
message(STATUS "[api] OS=${CMAKE_SYSTEM_NAME} ARCH=${CURRENT_CPU}")
message(STATUS "[api] Features: THREAD=${WEBRTC_BUILD_WEBRTC_THREAD} AUDIO=${WEBRTC_BUILD_WEBRTC_AUDIO_TOOL} VIDEO=${WEBRTC_BUILD_VIDEO} GCC=${WEBRTC_BUILD_GCC} STUN=${WEBRTC_BUILD_WEBRTC_STUN}")

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
    ${WEBRTC_SRC_DIR}/api/video/hdr_metadata.cc
    ${WEBRTC_SRC_DIR}/api/video/video_content_type.cc
   
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_encoder.cc


)
# webrtc threading modules
if(WEBRTC_BUILD_WEBRTC_THREAD)
    message(STATUS "[api] Thread: enabled - adding task_queue_base.cc")
    list(APPEND WEBRTC_API_SOURCES
    
    ${WEBRTC_SRC_DIR}/api/task_queue/task_queue_base.cc
    )

    if(WIN32)
        message(STATUS "[api] TaskQueue factory: win")
        list(APPEND WEBRTC_API_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_win.cc
        )
    elseif(APPLE)
        message(STATUS "[api] TaskQueue factory: GCD (Apple)")
        list(APPEND WEBRTC_API_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_gcd.cc
        )
    elseif(LINUX AND UNIX)
        message(STATUS "[api] TaskQueue factory: libevent (Linux)")
        list(APPEND WEBRTC_API_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_libevent.cc
        )
    else()
        message(STATUS "[api] TaskQueue factory: stdlib (fallback for ${CMAKE_SYSTEM_NAME})")
        list(APPEND WEBRTC_API_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_stdlib.cc
        )
    endif()
else()
    message(STATUS "[api] Thread: disabled")
endif()


# webrtc audio tool modules end
if(WEBRTC_BUILD_WEBRTC_AUDIO_TOOL)
    message(STATUS "[api] Audio tool: enabled")
    list(APPEND WEBRTC_API_SOURCES
        ${WEBRTC_SRC_DIR}/api/audio/audio_frame.cc
        ${WEBRTC_SRC_DIR}/api/audio/channel_layout.cc
    )
else()
    message(STATUS "[api] Audio tool: disabled")
endif()

# video modules
if(WEBRTC_BUILD_VIDEO)
    message(STATUS "[api] Video: enabled")
    list(APPEND WEBRTC_API_SOURCES
        ${WEBRTC_SRC_DIR}/api/video/encoded_image.cc
        ${WEBRTC_SRC_DIR}/api/video/video_bitrate_allocation.cc
        ${WEBRTC_SRC_DIR}/api/video/video_frame_buffer.cc
        ${WEBRTC_SRC_DIR}/api/video/video_frame.cc
        ${WEBRTC_SRC_DIR}/api/video_codecs/video_codec.cc
        ${WEBRTC_SRC_DIR}/api/video_codecs/video_decoder.cc
        ${WEBRTC_SRC_DIR}/api/video_codecs/video_encoder.cc
        ${WEBRTC_SRC_DIR}/api/video/video_timing.cc
        ${WEBRTC_SRC_DIR}/api/video/video_content_type.cc
        ${WEBRTC_SRC_DIR}/api/video/hdr_metadata.cc
        ${WEBRTC_SRC_DIR}/api/video/color_space.cc
    )
    message(STATUS "[api] Video sources: encoded_image, bitrate_allocation, frame_buffer, frame, codec, decoder, encoder, timing, content_type, hdr_metadata, color_space")
else()
    message(STATUS "[api] Video: disabled")
endif()

# gcc modules
if(WEBRTC_BUILD_GCC)
    message(STATUS "[api] GCC: enabled")
    list(APPEND WEBRTC_API_SOURCES
        ${WEBRTC_SRC_DIR}/api/transport/goog_cc_factory.cc
    )
else()
    message(STATUS "[api] GCC: disabled")
endif()

if(WEBRTC_BUILD_WEBRTC_STUN)
    message(STATUS "[api] STUN: enabled - adding stun.cc")
    list(APPEND WEBRTC_API_SOURCES
       ${WEBRTC_SRC_DIR}/api/transport/stun.cc
    )
else()
    message(STATUS "[api] STUN: disabled")
endif()



# add_library(api STATIC ${WEBRTC_API_SOURCES})
add_library(api OBJECT
    ${WEBRTC_API_SOURCES}
)

target_include_directories(
    api
    PRIVATE
        ${WEBRTC_SRC_DIR}
         ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

# ==================== [api] 编译摘要 ====================
list(LENGTH WEBRTC_API_SOURCES _total)
message(STATUS "[api] Total source files: ${_total}")
message(STATUS "[api] ====== End of api ======")