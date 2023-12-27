{
  'target_defaults': {
    'dependencies':
    [
      'deps/abseil-cpp/abseil-cpp.gyp:abseil',
      '../libuv/uv.gyp:libuv',
      '../openssl/openssl.gyp:openssl'
    ],
    'direct_dependent_settings': {
      'include_dirs':
      [
        '.',
        'libwebrtc'
      ]
    },
    'sources':
    [
      # C++ source files.
      'libwebrtc/system_wrappers/source/field_trial.cc',
      'libwebrtc/rtc_base/rate_statistics.cc',
      'libwebrtc/rtc_base/experiments/field_trial_parser.cc',
      'libwebrtc/rtc_base/experiments/alr_experiment.cc',
      'libwebrtc/rtc_base/experiments/field_trial_units.cc',
      'libwebrtc/rtc_base/experiments/rate_control_settings.cc',
      'libwebrtc/rtc_base/network/sent_packet.cc',
      'libwebrtc/call/rtp_transport_controller_send.cc',
      'libwebrtc/api/transport/bitrate_settings.cc',
      'libwebrtc/api/transport/field_trial_based_config.cc',
      'libwebrtc/api/transport/network_types.cc',
      'libwebrtc/api/transport/goog_cc_factory.cc',
      'libwebrtc/api/units/timestamp.cc',
      'libwebrtc/api/units/time_delta.cc',
      'libwebrtc/api/units/data_rate.cc',
      'libwebrtc/api/units/data_size.cc',
      'libwebrtc/api/units/frequency.cc',
      'libwebrtc/api/network_state_predictor.cc',
      'libwebrtc/modules/pacing/interval_budget.cc',
      'libwebrtc/modules/pacing/bitrate_prober.cc',
      'libwebrtc/modules/pacing/paced_sender.cc',
      'libwebrtc/modules/remote_bitrate_estimator/overuse_detector.cc',
      'libwebrtc/modules/remote_bitrate_estimator/overuse_estimator.cc',
      'libwebrtc/modules/remote_bitrate_estimator/aimd_rate_control.cc',
      'libwebrtc/modules/remote_bitrate_estimator/inter_arrival.cc',
      'libwebrtc/modules/remote_bitrate_estimator/bwe_defines.cc',
      'libwebrtc/modules/remote_bitrate_estimator/remote_bitrate_estimator_abs_send_time.cc',
      'libwebrtc/modules/rtp_rtcp/include/rtp_rtcp_defines.cc',
      'libwebrtc/modules/bitrate_controller/send_side_bandwidth_estimation.cc',
      'libwebrtc/modules/bitrate_controller/loss_based_bandwidth_estimation.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/goog_cc_network_control.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/probe_bitrate_estimator.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/congestion_window_pushback_controller.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/link_capacity_estimator.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/alr_detector.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/probe_controller.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/median_slope_estimator.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/bitrate_estimator.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/trendline_estimator.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/delay_based_bwe.cc',
      'libwebrtc/modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator.cc',
      'libwebrtc/modules/congestion_controller/rtp/send_time_history.cc',
      'libwebrtc/modules/congestion_controller/rtp/transport_feedback_adapter.cc',
      'libwebrtc/modules/congestion_controller/rtp/control_handler.cc',
      # red source file
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
      # 'src/rtc_base/string_to_number.cc',
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
      # 重复
      # 'src/modules/rtp_rtcp/include/rtp_rtcp_defines.cc',
      'src/api/media_types.cc',
      'src/api/rtp_headers.cc',
      'src/api/rtp_parameters.cc',
      'src/api/audio_codecs/audio_decoder.cc',
      'src/api/transport/network_types.cc',
      'src/api/units/data_rate.cc',
      'src/api/units/data_size.cc',
         # 重复
      # 'src/api/units/time_delta.cc',
      # 重复
      # 'src/api/units/timestamp.cc',
      # 'src/api/video/color_space.cc',
      'src/api/video/hdr_metadata.cc',
      'src/api/video/video_content_type.cc',
      'src/api/video/video_timing.cc',
      # C++ include files.
      'libwebrtc/system_wrappers/source/field_trial.h',
      'libwebrtc/rtc_base/rate_statistics.h',
      'libwebrtc/rtc_base/experiments/field_trial_parser.h',
      'libwebrtc/rtc_base/experiments/field_trial_units.h',
      'libwebrtc/rtc_base/experiments/alr_experiment.h',
      'libwebrtc/rtc_base/experiments/rate_control_settings.h',
      'libwebrtc/rtc_base/network/sent_packet.h',
      'libwebrtc/rtc_base/units/unit_base.h',
      'libwebrtc/rtc_base/constructor_magic.h',
      'libwebrtc/rtc_base/numerics/safe_minmax.h',
      'libwebrtc/rtc_base/numerics/safe_conversions.h',
      'libwebrtc/rtc_base/numerics/safe_conversions_impl.h',
      'libwebrtc/rtc_base/numerics/percentile_filter.h',
      'libwebrtc/rtc_base/numerics/safe_compare.h',
      'libwebrtc/rtc_base/system/unused.h',
      'libwebrtc/rtc_base/type_traits.h',
      'libwebrtc/call/rtp_transport_controller_send.h',
      'libwebrtc/call/rtp_transport_controller_send_interface.h',
      'libwebrtc/api/transport/webrtc_key_value_config.h',
      'libwebrtc/api/transport/network_types.h',
      'libwebrtc/api/transport/bitrate_settings.h',
      'libwebrtc/api/transport/network_control.h',
      'libwebrtc/api/transport/field_trial_based_config.h',
      'libwebrtc/api/transport/goog_cc_factory.h',
      'libwebrtc/api/bitrate_constraints.h',
      'libwebrtc/api/units/frequency.h',
      'libwebrtc/api/units/data_size.h',
      'libwebrtc/api/units/time_delta.h',
      'libwebrtc/api/units/data_rate.h',
      'libwebrtc/api/units/timestamp.h',
      'libwebrtc/api/network_state_predictor.h',
      'libwebrtc/modules/include/module_common_types_public.h',
      'libwebrtc/modules/pacing/interval_budget.h',
      'libwebrtc/modules/pacing/paced_sender.h',
      'libwebrtc/modules/pacing/packet_router.h',
      'libwebrtc/modules/pacing/bitrate_prober.h',
      'libwebrtc/modules/remote_bitrate_estimator/inter_arrival.h',
      'libwebrtc/modules/remote_bitrate_estimator/overuse_detector.h',
      'libwebrtc/modules/remote_bitrate_estimator/overuse_estimator.h',
      'libwebrtc/modules/remote_bitrate_estimator/bwe_defines.h',
      'libwebrtc/modules/remote_bitrate_estimator/aimd_rate_control.h',
      'libwebrtc/modules/remote_bitrate_estimator/remote_bitrate_estimator_abs_send_time.h',
      'libwebrtc/modules/remote_bitrate_estimator/include/remote_bitrate_estimator.h',
      'libwebrtc/modules/rtp_rtcp/include/rtp_rtcp_defines.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_packet/transport_feedback.h',
      'libwebrtc/modules/bitrate_controller/loss_based_bandwidth_estimation.h',
      'libwebrtc/modules/bitrate_controller/send_side_bandwidth_estimation.h',
      'libwebrtc/modules/congestion_controller/goog_cc/bitrate_estimator.h',
      'libwebrtc/modules/congestion_controller/goog_cc/link_capacity_estimator.h',
      'libwebrtc/modules/congestion_controller/goog_cc/median_slope_estimator.h',
      'libwebrtc/modules/congestion_controller/goog_cc/probe_controller.h',
      'libwebrtc/modules/congestion_controller/goog_cc/trendline_estimator.h',
      'libwebrtc/modules/congestion_controller/goog_cc/goog_cc_network_control.h',
      'libwebrtc/modules/congestion_controller/goog_cc/delay_increase_detector_interface.h',
      'libwebrtc/modules/congestion_controller/goog_cc/acknowledged_bitrate_estimator.h',
      'libwebrtc/modules/congestion_controller/goog_cc/congestion_window_pushback_controller.h',
      'libwebrtc/modules/congestion_controller/goog_cc/delay_based_bwe.h',
      'libwebrtc/modules/congestion_controller/goog_cc/probe_bitrate_estimator.h',
      'libwebrtc/modules/congestion_controller/goog_cc/alr_detector.h',
      'libwebrtc/modules/congestion_controller/rtp/send_time_history.h',
      'libwebrtc/modules/congestion_controller/rtp/transport_feedback_adapter.h',
      'libwebrtc/modules/congestion_controller/rtp/control_handler.h',
      'libwebrtc/mediasoup_helpers.h'
      # red include file
      'src/common_types.h',
      'src/rtc_base/arraysize.h',
      'src/rtc_base/atomic_ops.h',
      'src/rtc_base/bit_buffer.h',
      'src/rtc_base/buffer.h',
      'src/rtc_base/byte_buffer.h',
      'src/rtc_base/byte_order.h',
      'src/rtc_base/callback.h',
      'src/rtc_base/checks.h',
      'src/rtc_base/constructor_magic.h',
      'src/rtc_base/copy_on_write_buffer.h',
      'src/rtc_base/cpu_time.h',
      'src/rtc_base/crc32.h',
      'src/rtc_base/crypt_string.h',
      'src/rtc_base/data_rate_limiter.h',
      'src/rtc_base/deprecation.h',
      'src/rtc_base/dscp.h',
      'src/rtc_base/flags.h',
      'src/rtc_base/format_macros.h',
      'src/rtc_base/function_view.h',
      'src/rtc_base/ref_counted_object.h',
      'src/rtc_base/ref_counter.h',
      'src/rtc_base/sanitizer.h',
      'src/rtc_base/string_encode.h',
      # 'src/rtc_base/string_to_number.h',
      'src/rtc_base/string_utils.h',
      'src/rtc_base/string_view.h',
      'src/rtc_base/stringize_macros.h',
      'src/rtc_base/template_util.h',
      'src/rtc_base/time_utils.h',
      'src/rtc_base/type_traits.h',
      'src/rtc_base/zero_memory.h',
      'src/rtc_base/numerics/safe_compare.h',
      'src/rtc_base/numerics/safe_conversions_impl.h',
      'src/rtc_base/numerics/safe_conversions.h',
      'src/rtc_base/numerics/safe_minmax.h',
      'src/rtc_base/strings/string_builder.h',
      'src/rtc_base/system/arch.h',
      'src/rtc_base/system/asm_defines.h',
      'src/rtc_base/system/fallthrough.h',
      'src/rtc_base/system/ignore_warnings.h',
      'src/rtc_base/system/inline.h',
      'src/rtc_base/system/rtc_export.h',
      'src/rtc_base/system/unused.h',
      'src/rtc_base/units/unit_base.h',
      'src/system_wrappers/include/sleep.h',
      'src/system_wrappers/include/ntp_time.h',
      'src/modules/audio_coding/neteq/packet.h',
      'src/modules/audio_coding/neteq/red_payload_splitter.h',
      'src/modules/audio_coding/neteq/tick_timer.h',
      'src/modules/rtp_rtcp/include/receive_statistics.h',
      'src/modules/rtp_rtcp/include/rtp_cvo.h',
      'src/modules/rtp_rtcp/include/rtp_header_extension_map.h',
      'src/modules/rtp_rtcp/include/rtp_header_parser.h',
      # 'src/modules/rtp_rtcp/include/rtp_rtcp_defines.h',
      'src/modules/rtp_rtcp/source/byte_io.h',
      'src/modules/rtp_rtcp/source/rtp_format.h',
      'src/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.h',
      'src/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.h',
      'src/modules/rtp_rtcp/source/rtp_header_extension_size.h',
      'src/modules/rtp_rtcp/source/rtp_header_extensions.h',
      'src/modules/rtp_rtcp/source/rtp_packet_received.h',
      'src/modules/rtp_rtcp/source/rtp_packet.h',
      'src/modules/rtp_rtcp/source/rtp_rtcp_config.h',
      'src/modules/rtp_rtcp/source/rtp_video_header.h',
      'src/modules/rtp_rtcp/source/time_util.h',
      'src/modules/video_coding/codecs/interface/common_constants.h',
      'src/api/array_view.h',
      'src/api/media_types.h',
      'src/api/rtp_headers.h',
      'src/api/rtp_parameters.h',
      'src/api/scoped_refptr.h',
      'src/api/audio_codecs/audio_decoder.h',
      'src/api/transport/network_types.h',
      'src/api/units/data_rate.h',
      'src/api/units/data_size.h',
      # 重复
      # 'src/api/units/time_delta.h',
      # 重复
      # 'src/api/units/timestamp.h',
      # 'src/api/video/color_space.h',
      'src/api/video/hdr_metadata.h',
      'src/api/video/video_content_type.h',
      'src/api/video/video_timing.h',
      'src/api/video/video_frame_marking.h',
      'src/api/video/video_rotation.h',
    ],
    'include_dirs':
    [
      'libwebrtc',
      '../../include',
      '../json/single_include/nlohmann'
    ],
    'conditions':
    [
      # Endianness.
      [ 'node_byteorder == "big"', {
          # Define Big Endian.
          'defines': [ 'MS_BIG_ENDIAN' ]
        }, {
          # Define Little Endian.
          'defines': [ 'MS_LITTLE_ENDIAN' ]
      }],

      # Platform-specifics.

      [ 'OS != "win"', {
        'cflags': [ '-std=c++11' ]
      }],

      [ 'OS == "mac"', {
        'xcode_settings':
        {
          'OTHER_CPLUSPLUSFLAGS' : [ '-std=c++11' ]
        }
      }]
    ]
  },
  'targets':
  [
    {
      'target_name': 'libwebrtc',
      'type': 'static_library'
    }
  ]
}
