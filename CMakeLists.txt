cmake_minimum_required(VERSION 3.16)
project(libwebrtc LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(
        /utf-8
        /Zc:__cplusplus
    )
else()
    add_compile_options(-finput-charset=UTF-8)
endif()

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# 预编译选项 - 按功能控制模块编译
option(WEBRTC_BUILD_WEBRTC_THREAD "Build WebRTC threading modules" ON)
option(WEBRTC_BUILD_RED "Build RED (RTP redundancy) modules" ON)
option(WEBRTC_BUILD_FEC "Build FEC (Forward Error Correction) modules" ON)
option(WEBRTC_BUILD_VIDEO "Build video-related modules" ON)
option(WEBRTC_BUILD_COMMON_VIDEO "Build common video modules" ON)
option(WEBRTC_BUILD_GCC "Build GCC modules" ON)
option(WEBRTC_BUILD_WEBRTC_LOGGING "Build WebRTC logging related modules" ON)

set(WEBRTC_SRC_DIR ${PROJECT_SOURCE_DIR}/libwebrtc)
set(DEPS_DIR ${PROJECT_SOURCE_DIR}/deps)

# 模块依赖检查
if(WEBRTC_BUILD_VIDEO AND NOT WEBRTC_BUILD_COMMON_VIDEO)
    message(STATUS "[libwebrtc] Auto-enabling WEBRTC_BUILD_COMMON_VIDEO (required by video modules)")
    set(WEBRTC_BUILD_COMMON_VIDEO ON CACHE BOOL "Build common video modules" FORCE)
endif()

if(WEBRTC_BUILD_COMMON_VIDEO AND NOT WEBRTC_BUILD_VIDEO)
    message(WARNING "[libwebrtc] WEBRTC_BUILD_COMMON_VIDEO enabled but WEBRTC_BUILD_VIDEO disabled. Common video modules may not be used.")
endif()

if(WEBRTC_BUILD_GCC AND (NOT WEBRTC_BUILD_FEC))
    message(STATUS "[libwebrtc] GCC modules do not require FEC, proceeding...")
endif()

# 核心基础模块（始终编译）
set(WEBRTC_SOURCES
    # C++ source files.
    ${WEBRTC_SRC_DIR}/system_wrappers/source/field_trial.cc
    ${WEBRTC_SRC_DIR}/rtc_base/rate_statistics.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/field_trial_parser.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/alr_experiment.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/field_trial_units.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/rate_control_settings.cc
    ${WEBRTC_SRC_DIR}/api/transport/bitrate_settings.cc
    ${WEBRTC_SRC_DIR}/api/transport/field_trial_based_config.cc
    ${WEBRTC_SRC_DIR}/api/units/timestamp.cc
    ${WEBRTC_SRC_DIR}/api/units/time_delta.cc
    ${WEBRTC_SRC_DIR}/api/units/data_rate.cc
    ${WEBRTC_SRC_DIR}/api/units/data_size.cc
    ${WEBRTC_SRC_DIR}/api/units/frequency.cc

    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/include/rtp_rtcp_defines.cc
    ${WEBRTC_SRC_DIR}/modules/include/module_common_types.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/common_header.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/rtpfb.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/transport_feedback.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_history.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_to_send.cc
    ${WEBRTC_SRC_DIR}/rtc_base/bit_buffer.cc
    ${WEBRTC_SRC_DIR}/rtc_base/byte_buffer.cc
    ${WEBRTC_SRC_DIR}/rtc_base/checks.cc
    ${WEBRTC_SRC_DIR}/rtc_base/random.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_encode.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_to_number.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_utils.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_view.cc
    ${WEBRTC_SRC_DIR}/rtc_base/zero_memory.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/packet.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_header_extension_map.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_header_extension_size.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_header_extensions.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_received.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/time_util.cc
    ${WEBRTC_SRC_DIR}/api/media_types.cc
    ${WEBRTC_SRC_DIR}/api/rtp_headers.cc
    ${WEBRTC_SRC_DIR}/api/rtp_parameters.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_decoder.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_format.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_codec_pair_id.cc
    ${WEBRTC_SRC_DIR}/api/rtp_packet_info.cc
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/dot_product_with_scale.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/codecs/cng/webrtc_cng.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/decoder_database.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/tick_timer.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/rtp_to_ntp_estimator.cc
    ${WEBRTC_SRC_DIR}/rtc_base/critical_section.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/yield.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/yield_policy.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/rw_lock_wrapper.cc
    ${WEBRTC_SRC_DIR}/rtc_base/crc32.cc
    ${WEBRTC_SRC_DIR}/rtc_base/system/file_wrapper.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/jitter_upper_bound_experiment.cc
    ${WEBRTC_SRC_DIR}/rtc_base/network/sent_packet.cc
    ${WEBRTC_SRC_DIR}/call/rtp_transport_controller_send.cc
    ${WEBRTC_SRC_DIR}/api/transport/network_types.cc
    ${WEBRTC_SRC_DIR}/api/network_state_predictor.cc

    ${WEBRTC_SRC_DIR}/system_wrappers/source/sleep.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/clock.cc
    ${WEBRTC_SRC_DIR}/api/video/color_space.cc
    ${WEBRTC_SRC_DIR}/rtc_base/copy_on_write_buffer.cc
    ${WEBRTC_SRC_DIR}/rtc_base/cpu_time.cc
    ${WEBRTC_SRC_DIR}/rtc_base/crypt_string.cc
    ${WEBRTC_SRC_DIR}/rtc_base/data_rate_limiter.cc
    ${WEBRTC_SRC_DIR}/rtc_base/flags.cc
    ${WEBRTC_SRC_DIR}/rtc_base/time_utils.cc
    ${WEBRTC_SRC_DIR}/rtc_base/strings/string_builder.cc
    ${WEBRTC_SRC_DIR}/rtc_base/platform_thread_types.cc
    ${WEBRTC_SRC_DIR}/rtc_base/strings/audio_format_to_string.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/cpu_features.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/metrics.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_encoder.cc

    ${WEBRTC_SRC_DIR}/rtc_base/third_party/base64/base64.cc

)

message(STATUS "[libwebrtc] Core modules configured. Sources: ${WEBRTC_SOURCES}")

if(WIN32)
# 添加Windows特有资源
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/rtc_base/win32.cc
        ${WEBRTC_SRC_DIR}/rtc_base/win32_socket_server.cc
        ${WEBRTC_SRC_DIR}/rtc_base/win32_window.cc
        ${WEBRTC_SRC_DIR}/rtc_base/synchronization/rw_lock_win.cc
    )
else()
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/rtc_base/synchronization/rw_lock_posix.cc
    )
endif()

# webrtc threading modules
if(WEBRTC_BUILD_WEBRTC_THREAD)
    message(STATUS "[libwebrtc] Adding WebRTC threading modules...")
    list(APPEND WEBRTC_SOURCES
    ${WEBRTC_SRC_DIR}/rtc_base/third_party/sigslot/sigslot.cc
    ${WEBRTC_SRC_DIR}/rtc_base/location.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/yield_policy.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/yield.cc
    ${WEBRTC_SRC_DIR}/rtc_base/async_socket.cc
    ${WEBRTC_SRC_DIR}/rtc_base/async_resolver_interface.cc
    ${WEBRTC_SRC_DIR}/rtc_base/event.cc
    ${WEBRTC_SRC_DIR}/rtc_base/event_tracer.cc
    ${WEBRTC_SRC_DIR}/rtc_base/platform_thread.cc
    ${WEBRTC_SRC_DIR}/rtc_base/socket.cc
    ${WEBRTC_SRC_DIR}/rtc_base/signal_thread.cc
    ${WEBRTC_SRC_DIR}/rtc_base/thread.cc
    ${WEBRTC_SRC_DIR}/rtc_base/message_handler.cc
    ${WEBRTC_SRC_DIR}/rtc_base/message_queue.cc
    ${WEBRTC_SRC_DIR}/rtc_base/net_helpers.cc
    ${WEBRTC_SRC_DIR}/rtc_base/net_helper.cc
    ${WEBRTC_SRC_DIR}/rtc_base/socket_address.cc
    ${WEBRTC_SRC_DIR}/rtc_base/ip_address.cc
    ${WEBRTC_SRC_DIR}/api/task_queue/task_queue_base.cc
    ${WEBRTC_SRC_DIR}/rtc_base/task_queue.cc
    ${WEBRTC_SRC_DIR}/rtc_base/task_utils/repeating_task.cc
    ${WEBRTC_SRC_DIR}/rtc_base/task_utils/pending_task_safety_flag.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/sequence_checker.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/sequence_checker_internal.cc
    )
    if(WIN32)
        list(APPEND WEBRTC_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_win.cc
            ${WEBRTC_SRC_DIR}/rtc_base/task_queue_win.cc
            )
    elseif(APPLE)
        list(APPEND WEBRTC_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_gcd.cc
            ${WEBRTC_SRC_DIR}/rtc_base/task_queue_gcd.cc
        )
    elseif(LINUX AND UNIX)
        list(APPEND WEBRTC_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_libevent.cc
            ${WEBRTC_SRC_DIR}/rtc_base/task_queue_libevent.cc
        )
    else()
        list(APPEND WEBRTC_SOURCES
            ${WEBRTC_SRC_DIR}/api/task_queue/default_task_queue_factory_stdlib.cc
            ${WEBRTC_SRC_DIR}/rtc_base/task_queue_stdlib.cc
        )
    endif()
    
    message(STATUS "[libwebrtc] WebRTC threading modules added")
else()
    message(STATUS "[libwebrtc] WebRTC threading modules disabled")
endif()
# red source file
if(WEBRTC_BUILD_RED)
    message(STATUS "[libwebrtc] Adding RED modules (RTP redundancy)...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/modules/audio_coding/codecs/red/audio_encoder_copy_red.cc
        ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/red_payload_splitter.cc
    )
    message(STATUS "[libwebrtc] RED modules added: audio_encoder_copy_red.cc")
else()
    message(STATUS "[libwebrtc] RED modules disabled")
endif()
# red sorce end

# lym fec decode
if(WEBRTC_BUILD_FEC)
    message(STATUS "[libwebrtc] Adding FEC modules (Forward Error Correction)...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/fec_private_tables_bursty.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/fec_private_tables_random.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/flexfec_header_reader_writer.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/flexfec_receiver.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/forward_error_correction.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/forward_error_correction_internal.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/remote_ntp_time_estimator.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/ulpfec_header_reader_writer.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/ulpfec_receiver_impl.cc
    )
    message(STATUS "[libwebrtc] FEC modules added: bursty/random tables, flexfec, ulpfec")
else()
    message(STATUS "[libwebrtc] FEC modules disabled")
endif()
# lym fec end

# video modules
if(WEBRTC_BUILD_VIDEO)
    message(STATUS "[libwebrtc] Adding video modules...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/api/video/encoded_image.cc
        ${WEBRTC_SRC_DIR}/api/video/video_bitrate_allocation.cc
        ${WEBRTC_SRC_DIR}/api/video/video_frame_buffer.cc
        ${WEBRTC_SRC_DIR}/api/video_codecs/video_codec.cc
        ${WEBRTC_SRC_DIR}/api/video_codecs/video_decoder.cc
        ${WEBRTC_SRC_DIR}/modules/video_coding/encoded_frame.cc
        ${WEBRTC_SRC_DIR}/modules/video_coding/include/video_codec_interface.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_video_header.cc
        ${WEBRTC_SRC_DIR}/api/video/video_timing.cc
        ${WEBRTC_SRC_DIR}/api/video/video_content_type.cc
        ${WEBRTC_SRC_DIR}/api/video/hdr_metadata.cc
        ${WEBRTC_SRC_DIR}/api/video/color_space.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_vp8.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_vp9.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_h264.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_video_generic.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.cc
    )
    message(STATUS "[libwebrtc] Video modules added: vp8, vp9, h264 codecs")
else()
    message(STATUS "[libwebrtc] Video modules disabled")
endif()
# video modules end

# common video modules
if(WEBRTC_BUILD_COMMON_VIDEO)
    message(STATUS "[libwebrtc] Adding common video modules...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/common_video/h264/h264_bitstream_parser.cc
        ${WEBRTC_SRC_DIR}/common_video/h264/h264_common.cc
        ${WEBRTC_SRC_DIR}/common_video/h264/pps_parser.cc
        ${WEBRTC_SRC_DIR}/common_video/h264/sps_parser.cc
        ${WEBRTC_SRC_DIR}/common_video/h264/sps_vui_rewriter.cc
        ${WEBRTC_SRC_DIR}/common_video/generic_frame_descriptor/generic_frame_info.cc
    )
    message(STATUS "[libwebrtc] Common video modules added: h264 parsers")
else()
    message(STATUS "[libwebrtc] Common video modules disabled")
endif()
# common video modules end

# common_audio ooura fft
if(WEBRTC_BUILD_FEC OR WEBRTC_BUILD_VIDEO OR WEBRTC_BUILD_COMMON_VIDEO)
    message(STATUS "[libwebrtc] Adding common_audio ooura FFT modules...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/common_audio/third_party/ooura/fft_size_128/ooura_fft.cc
        ${WEBRTC_SRC_DIR}/common_audio/third_party/ooura/fft_size_128/ooura_fft_sse2.cc
        ${WEBRTC_SRC_DIR}/common_audio/third_party/ooura/fft_size_256/fft4g.cc
    )
    message(STATUS "[libwebrtc] Common audio FFT modules added")
endif()

# gcc modules
if(WEBRTC_BUILD_GCC)
    message(STATUS "[libwebrtc] Adding GCC modules...")
    list(APPEND WEBRTC_SOURCES
    # lym pacer
    ${WEBRTC_SRC_DIR}/modules/pacing/interval_budget.cc
    ${WEBRTC_SRC_DIR}/modules/pacing/bitrate_prober.cc
    ${WEBRTC_SRC_DIR}/modules/pacing/paced_sender.cc
    # lyn pacer end
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/bitrate_estimator.cc
    ${WEBRTC_SRC_DIR}/api/transport/goog_cc_factory.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/overuse_detector.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/overuse_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/aimd_rate_control.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/bwe_defines.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/inter_arrival.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/remote_bitrate_estimator_abs_send_time.cc
    ${WEBRTC_SRC_DIR}/modules/bitrate_controller/send_side_bandwidth_estimation.cc
    ${WEBRTC_SRC_DIR}/modules/bitrate_controller/loss_based_bandwidth_estimation.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/goog_cc_network_control.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/probe_bitrate_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/congestion_window_pushback_controller.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/link_capacity_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/alr_detector.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/probe_controller.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/median_slope_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/trendline_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/delay_based_bwe.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/rtp/send_time_history.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/rtp/transport_feedback_adapter.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/rtp/control_handler.cc
    )
    message(STATUS "[libwebrtc] GCC modules added: audio_encoder_copy_red.cc")
else()
    message(STATUS "[libwebrtc] GCC modules disabled")
endif()

# webrtc logging modules
if(WEBRTC_BUILD_WEBRTC_LOGGING)
    message(STATUS "[libwebrtc] Adding WebRTC logging modules...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/rtc_base/log_sinks.cc
        ${WEBRTC_SRC_DIR}/rtc_base/logging.cc
        ${WEBRTC_SRC_DIR}/rtc_base/stream.cc
        ${WEBRTC_SRC_DIR}/rtc_base/file_rotating_stream.cc
        )
    message(STATUS "[libwebrtc] WebRTC logging modules added")
else()
    message(STATUS "[libwebrtc] WebRTC logging modules disabled")
endif()

message(STATUS "[libwebrtc] Total sources configured: ${WEBRTC_SOURCES}")

add_library(libwebrtc STATIC ${WEBRTC_SOURCES})

# 添加编译定义
if(WEBRTC_BUILD_RED)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_BUILD_RED)
endif()
if(WEBRTC_BUILD_FEC)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_BUILD_FEC)
endif()
if(WEBRTC_BUILD_GCC)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_BUILD_GCC)
endif()
if(WEBRTC_BUILD_VIDEO)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_BUILD_VIDEO)
endif()
if(WEBRTC_BUILD_COMMON_VIDEO)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_BUILD_COMMON_VIDEO)
endif()

target_include_directories(libwebrtc PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${WEBRTC_SRC_DIR}
    ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

if(MSVC)
    target_compile_options(libwebrtc PRIVATE /W0 /utf-8)
else()
    target_compile_options(libwebrtc PRIVATE -w -include cstdint)
endif()

if(WIN32)
    # Windows 平台（桌面、UWP等）
    message(STATUS "[libwebrtc] Platform: Windows")
    target_compile_definitions(libwebrtc PRIVATE
        WEBRTC_WIN
        DISABLE_H265
        NOMINMAX
        WIN32_LEAN_AND_MEAN
        _CRT_SECURE_NO_WARNINGS
    )
elseif(ANDROID)
    # Android 平台（优先于 UNIX 判断）
    message(STATUS "[libwebrtc] Platform: Android")
    target_compile_definitions(libwebrtc PRIVATE
        WEBRTC_POSIX
        WEBRTC_ANDROID
        DISABLE_H265
    )
elseif(APPLE)
    # Apple 平台（macOS、iOS、tvOS、watchOS）
    if(IOS)
        message(STATUS "[libwebrtc] Platform: iOS")
        target_compile_definitions(libwebrtc PRIVATE
            WEBRTC_POSIX
            WEBRTC_IOS
            DISABLE_H265
        )
    else()
        message(STATUS "[libwebrtc] Platform: macOS")
        target_compile_definitions(libwebrtc PRIVATE
            WEBRTC_POSIX
            WEBRTC_MAC
            DISABLE_H265
        )
    endif()
elseif(UNIX)
    # 其他类 Unix 系统（Linux 等）
    message(STATUS "[libwebrtc] Platform: Linux/Unix")
    target_compile_definitions(libwebrtc PRIVATE
        WEBRTC_POSIX
        WEBRTC_LINUX
        USE_MEDIASOUP_ClASS
        DISABLE_H265
    )
else()
    message(WARNING "[libwebrtc] Unknown platform!")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    target_compile_definitions(libwebrtc PRIVATE MS_BIG_ENDIAN)
else()
    target_compile_definitions(libwebrtc PRIVATE MS_LITTLE_ENDIAN)
endif()

add_subdirectory(${DEPS_DIR}/abseil-cpp/abseil-cpp ${PROJECT_BINARY_DIR}/abseil)

target_link_libraries(libwebrtc PRIVATE
    absl_strings
    absl_base
    absl_optional
    absl_flat_hash_map
)

set_target_properties(libwebrtc PROPERTIES OUTPUT_NAME webrtc)

install(TARGETS libwebrtc
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# 安装头文件 - 包含所有模块的头文件
install(DIRECTORY ${WEBRTC_SRC_DIR}/
        DESTINATION include/webrtc
        FILES_MATCHING PATTERN "*.h")

# 安装系统相关头文件目录
install(DIRECTORY ${WEBRTC_SRC_DIR}/system_wrappers/include/
        DESTINATION include/webrtc/system_wrappers/include
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/rtc_base/
        DESTINATION include/webrtc/rtc_base
        FILES_MATCHING PATTERN "*.h"
        PATTERN "network/*" EXCLUDE
        PATTERN "experiments/*" EXCLUDE
        PATTERN "synchronization/*" EXCLUDE
        PATTERN "numerics/*" EXCLUDE
        PATTERN "strings/*" EXCLUDE
        PATTERN "system/*" EXCLUDE)

install(DIRECTORY ${WEBRTC_SRC_DIR}/rtc_base/network/
        DESTINATION include/webrtc/rtc_base/network
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/rtc_base/experiments/
        DESTINATION include/webrtc/rtc_base/experiments
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/rtc_base/synchronization/
        DESTINATION include/webrtc/rtc_base/synchronization
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/rtc_base/numerics/
        DESTINATION include/webrtc/rtc_base/numerics
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/rtc_base/strings/
        DESTINATION include/webrtc/rtc_base/strings
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/rtc_base/system/
        DESTINATION include/webrtc/rtc_base/system
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/api/
        DESTINATION include/webrtc/api
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/modules/
        DESTINATION include/webrtc/modules
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/common_audio/
        DESTINATION include/webrtc/common_audio
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/common_video/
        DESTINATION include/webrtc/common_video
        FILES_MATCHING PATTERN "*.h")

install(DIRECTORY ${WEBRTC_SRC_DIR}/call/
        DESTINATION include/webrtc/call
        FILES_MATCHING PATTERN "*.h")
