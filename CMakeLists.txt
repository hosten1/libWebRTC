cmake_minimum_required(VERSION 3.16)
project(libwebrtc LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

set(WEBRTC_SRC_DIR ${PROJECT_SOURCE_DIR}/libwebrtc)
set(DEPS_DIR ${PROJECT_SOURCE_DIR}/deps)

set(WEBRTC_SOURCES
    ${WEBRTC_SRC_DIR}/system_wrappers/source/field_trial.cc
    ${WEBRTC_SRC_DIR}/rtc_base/rate_statistics.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/field_trial_parser.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/alr_experiment.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/field_trial_units.cc
    ${WEBRTC_SRC_DIR}/rtc_base/experiments/rate_control_settings.cc
    ${WEBRTC_SRC_DIR}/rtc_base/network/sent_packet.cc
    ${WEBRTC_SRC_DIR}/call/rtp_transport_controller_send.cc
    ${WEBRTC_SRC_DIR}/api/transport/bitrate_settings.cc
    ${WEBRTC_SRC_DIR}/api/transport/field_trial_based_config.cc
    ${WEBRTC_SRC_DIR}/api/transport/network_types.cc
    ${WEBRTC_SRC_DIR}/api/transport/goog_cc_factory.cc
    ${WEBRTC_SRC_DIR}/api/units/timestamp.cc
    ${WEBRTC_SRC_DIR}/api/units/time_delta.cc
    ${WEBRTC_SRC_DIR}/api/units/data_rate.cc
    ${WEBRTC_SRC_DIR}/api/units/data_size.cc
    ${WEBRTC_SRC_DIR}/api/units/frequency.cc
    ${WEBRTC_SRC_DIR}/api/network_state_predictor.cc
    ${WEBRTC_SRC_DIR}/modules/pacing/interval_budget.cc
    ${WEBRTC_SRC_DIR}/modules/pacing/bitrate_prober.cc
    ${WEBRTC_SRC_DIR}/modules/pacing/paced_sender.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/overuse_detector.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/overuse_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/aimd_rate_control.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/inter_arrival.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/bwe_defines.cc
    ${WEBRTC_SRC_DIR}/modules/remote_bitrate_estimator/remote_bitrate_estimator_abs_send_time.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/include/rtp_rtcp_defines.cc
    ${WEBRTC_SRC_DIR}/modules/bitrate_controller/send_side_bandwidth_estimation.cc
    ${WEBRTC_SRC_DIR}/modules/bitrate_controller/loss_based_bandwidth_estimation.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/goog_cc_network_control.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/probe_bitrate_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/congestion_window_pushback_controller.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/link_capacity_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/alr_detector.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/probe_controller.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/median_slope_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/bitrate_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/trendline_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/delay_based_bwe.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/rtp/send_time_history.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/rtp/transport_feedback_adapter.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/rtp/control_handler.cc
    ${WEBRTC_SRC_DIR}/modules/include/module_common_types.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/sleep.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/clock.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/rw_lock_wrapper.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/rw_lock_posix.cc
    ${WEBRTC_SRC_DIR}/api/video/color_space.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_format.cc
    ${WEBRTC_SRC_DIR}/rtc_base/critical_section.cc
    ${WEBRTC_SRC_DIR}/rtc_base/platform_thread_types.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/common_header.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/transport_feedback.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_history.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_to_send.cc
    ${WEBRTC_SRC_DIR}/rtc_base/bit_buffer.cc
    ${WEBRTC_SRC_DIR}/rtc_base/byte_buffer.cc
    ${WEBRTC_SRC_DIR}/rtc_base/checks.cc
    ${WEBRTC_SRC_DIR}/rtc_base/copy_on_write_buffer.cc
    ${WEBRTC_SRC_DIR}/rtc_base/cpu_time.cc
    ${WEBRTC_SRC_DIR}/rtc_base/crc32.cc
    ${WEBRTC_SRC_DIR}/rtc_base/crypt_string.cc
    ${WEBRTC_SRC_DIR}/rtc_base/data_rate_limiter.cc
    ${WEBRTC_SRC_DIR}/rtc_base/flags.cc
    ${WEBRTC_SRC_DIR}/rtc_base/random.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_encode.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_to_number.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_utils.cc
    ${WEBRTC_SRC_DIR}/rtc_base/string_view.cc
    ${WEBRTC_SRC_DIR}/rtc_base/time_utils.cc
    ${WEBRTC_SRC_DIR}/rtc_base/zero_memory.cc
    ${WEBRTC_SRC_DIR}/rtc_base/strings/string_builder.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/packet.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/red_payload_splitter.cc
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
    ${WEBRTC_SRC_DIR}/api/video/hdr_metadata.cc
    ${WEBRTC_SRC_DIR}/api/video/video_content_type.cc
    ${WEBRTC_SRC_DIR}/api/video/video_timing.cc
    ${WEBRTC_SRC_DIR}/common_video/generic_frame_descriptor/generic_frame_info.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_codec_pair_id.cc
    ${WEBRTC_SRC_DIR}/api/rtp_packet_info.cc
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/dot_product_with_scale.cc
    ${WEBRTC_SRC_DIR}/common_audio/third_party/ooura/fft_size_128/ooura_fft.cc
    ${WEBRTC_SRC_DIR}/common_audio/third_party/ooura/fft_size_256/fft4g.cc
    ${WEBRTC_SRC_DIR}/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.c
    ${WEBRTC_SRC_DIR}/modules/audio_coding/codecs/cng/webrtc_cng.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/decoder_database.cc
    ${WEBRTC_SRC_DIR}/rtc_base/strings/audio_format_to_string.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/cpu_features.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/metrics.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/rtp_to_ntp_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_video_header.cc
    ${WEBRTC_SRC_DIR}/api/video/encoded_image.cc
    ${WEBRTC_SRC_DIR}/api/video/video_bitrate_allocation.cc
    ${WEBRTC_SRC_DIR}/api/video/video_frame.cc
    ${WEBRTC_SRC_DIR}/api/video/video_frame_buffer.cc
    ${WEBRTC_SRC_DIR}/api/video_codecs/video_codec.cc
    ${WEBRTC_SRC_DIR}/api/video_codecs/video_decoder.cc
    ${WEBRTC_SRC_DIR}/api/video_codecs/video_encoder.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/h264_bitstream_parser.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/h264_common.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/pps_parser.cc
    ${WEBRTC_SRC_DIR}/common_video/h264/sps_parser.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/fec_private_tables_bursty.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/fec_private_tables_random.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/flexfec_header_reader_writer.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/flexfec_receiver.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/forward_error_correction.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/forward_error_correction_internal.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/remote_ntp_time_estimator.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_h264.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_video_generic.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_vp8.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_format_vp9.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/ulpfec_header_reader_writer.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/ulpfec_receiver_impl.cc
    ${WEBRTC_SRC_DIR}/modules/video_coding/encoded_frame.cc
    ${WEBRTC_SRC_DIR}/modules/video_coding/include/video_codec_interface.cc
    ${WEBRTC_SRC_DIR}/rtc_base/synchronization/sequence_checker.cc
    ${WEBRTC_SRC_DIR}/api/audio_codecs/audio_encoder.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/codecs/red/audio_encoder_copy_red.cc
    ${WEBRTC_SRC_DIR}/rtc_base/system/file_wrapper.cc
)

include_directories(
    ${PROJECT_SOURCE_DIR}
    ${WEBRTC_SRC_DIR}
    ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

add_library(libwebrtc STATIC ${WEBRTC_SOURCES})

if(APPLE)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_POSIX WEBRTC_MAC)
elseif(UNIX)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_POSIX WEBRTC_LINUX USE_MEDIASOUP_ClASS)
elseif(WIN32)
    target_compile_definitions(libwebrtc PRIVATE WEBRTC_WIN)
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    target_compile_definitions(libwebrtc PRIVATE MS_BIG_ENDIAN)
else()
    target_compile_definitions(libwebrtc PRIVATE MS_LITTLE_ENDIAN)
endif()

add_subdirectory(${DEPS_DIR}/abseil-cpp/abseil-cpp ${PROJECT_BINARY_DIR}/abseil)
target_link_libraries(libwebrtc PRIVATE
    abseil
)

set_target_properties(libwebrtc PROPERTIES OUTPUT_NAME webrtc)

install(TARGETS libwebrtc
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY ${WEBRTC_SRC_DIR}/
        DESTINATION include/webrtc
        FILES_MATCHING PATTERN "*.h")
