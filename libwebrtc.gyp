{
  'target_defaults': {
    'dependencies':
    [
      # 'deps/abseil-cpp/abseil-cpp.gyp:abseil'
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
      'libwebrtc/modules/include/module_common_types.cc',
      # red source file
      'libwebrtc/system_wrappers/source/sleep.cc',
      'libwebrtc/system_wrappers/source/clock.cc',
      'libwebrtc/rtc_base/synchronization/rw_lock_wrapper.cc',
      'libwebrtc/rtc_base/synchronization/rw_lock_posix.cc',
      # 'libwebrtc/rtc_base/synchronization/rw_lock_win.cc',
      'libwebrtc/api/video/color_space.cc',
      'libwebrtc/api/audio_codecs/audio_format.cc',
      # red sorce end
     # lym pacer
     'libwebrtc/rtc_base/critical_section.cc',
     'libwebrtc/rtc_base/platform_thread_types.cc',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet.cc',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet/common_header.cc',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet/rtpfb.ccc',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet/transport_feedback.cc',
     'libwebrtc/modules/rtp_rtcp/source/rtp_packet_history.cc',
     'libwebrtc/modules/rtp_rtcp/source/rtp_packet_to_send.cc',
      # lyn pacer end
      'libwebrtc/rtc_base/bit_buffer.cc',
      'libwebrtc/rtc_base/byte_buffer.cc',
      'libwebrtc/rtc_base/checks.cc',
      'libwebrtc/rtc_base/copy_on_write_buffer.cc',
      'libwebrtc/rtc_base/cpu_time.cc',
      'libwebrtc/rtc_base/crc32.cc',
      'libwebrtc/rtc_base/crypt_string.cc',
      'libwebrtc/rtc_base/data_rate_limiter.cc',
      'libwebrtc/rtc_base/flags.cc',
      'libwebrtc/rtc_base/random.cc',
      'libwebrtc/rtc_base/string_encode.cc',
      'libwebrtc/rtc_base/string_to_number.cc',
      'libwebrtc/rtc_base/string_utils.cc',
      'libwebrtc/rtc_base/string_view.cc',
      'libwebrtc/rtc_base/time_utils.cc',
      'libwebrtc/rtc_base/zero_memory.cc',
      'libwebrtc/rtc_base/strings/string_builder.cc',
      'libwebrtc/modules/audio_coding/neteq/packet.cc',
      'libwebrtc/modules/audio_coding/neteq/red_payload_splitter.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_header_extension_map.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_header_extension_size.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_header_extensions.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_packet_received.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_packet.cc',
      'libwebrtc/modules/rtp_rtcp/source/time_util.cc',
      'libwebrtc/api/media_types.cc',
      'libwebrtc/api/rtp_headers.cc',
      'libwebrtc/api/rtp_parameters.cc',
      'libwebrtc/api/audio_codecs/audio_decoder.cc',
   
      'libwebrtc/api/video/hdr_metadata.cc',
      'libwebrtc/api/video/video_content_type.cc',
      'libwebrtc/api/video/video_timing.cc',
      'libwebrtc/common_video/generic_frame_descriptor/generic_frame_info.cc',
      'libwebrtc/modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.cc',
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
      'libwebrtc/api/function_view.h',
      'libwebrtc/api/array_view.h',
      'libwebrtc/modules/include/module_common_types_public.h',
      # lym 
      'libwebrtc/modules/include/module.h',
      'libwebrtc/modules/include/module_common_types.h',
      'libwebrtc/modules/include/module_fec_types.h',
      'libwebrtc/api/video/video_frame_type.h',
      'libwebrtc/api/video/video_codec_type.h',
      'libwebrtc/api/video/video_codec_constants.h',
      'libwebrtc/common_video/generic_frame_descriptor/generic_frame_info.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_dependency_descriptor_reader.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_dependency_descriptor_writer.h',
      #lym end red
      # lym pacer
     'libwebrtc/rtc_base/critical_section.h',
     'libwebrtc/rtc_base/platform_thread_types.h',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet.h',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet/common_header.h',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet/rtpfb.h',
     'libwebrtc/modules/rtp_rtcp/source/rtcp_packet/transport_feedback.h',
     'libwebrtc/modules/rtp_rtcp/source/rtp_packet_history.h',
     'libwebrtc/modules/rtp_rtcp/source/rtp_packet_to_send.h',
      # lyn pacer end
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
     #'libwebrtc/modules/rtp_rtcp/source/rtp_packet/transport_feedback.h',
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
      'libwebrtc/common_types.h',
      'libwebrtc/rtc_base/arraysize.h',
      'libwebrtc/rtc_base/atomic_ops.h',
      'libwebrtc/rtc_base/bit_buffer.h',
      'libwebrtc/rtc_base/buffer.h',
      'libwebrtc/rtc_base/byte_buffer.h',
      'libwebrtc/rtc_base/byte_order.h',
      'libwebrtc/rtc_base/callback.h',
      'libwebrtc/rtc_base/checks.h',
      'libwebrtc/rtc_base/constructor_magic.h',
      'libwebrtc/rtc_base/copy_on_write_buffer.h',
      'libwebrtc/rtc_base/cpu_time.h',
      'libwebrtc/rtc_base/crc32.h',
      'libwebrtc/rtc_base/crypt_string.h',
      'libwebrtc/rtc_base/data_rate_limiter.h',
      'libwebrtc/rtc_base/deprecation.h',
      'libwebrtc/rtc_base/dscp.h',
      'libwebrtc/rtc_base/flags.h',
      'libwebrtc/rtc_base/format_macros.h',
      'libwebrtc/rtc_base/function_view.h',
      'libwebrtc/rtc_base/ref_counted_object.h',
      'libwebrtc/rtc_base/ref_counter.h',
      'libwebrtc/rtc_base/sanitizer.h',
      'libwebrtc/rtc_base/string_encode.h',
      'libwebrtc/rtc_base/string_to_number.h',
      'libwebrtc/rtc_base/string_utils.h',
      'libwebrtc/rtc_base/string_view.h',
      'libwebrtc/rtc_base/stringize_macros.h',
      'libwebrtc/rtc_base/template_util.h',
      'libwebrtc/rtc_base/time_utils.h',
      'libwebrtc/rtc_base/type_traits.h',
      'libwebrtc/rtc_base/zero_memory.h',
      'libwebrtc/rtc_base/numerics/safe_compare.h',
      'libwebrtc/rtc_base/numerics/safe_conversions_impl.h',
      'libwebrtc/rtc_base/numerics/safe_conversions.h',
      'libwebrtc/rtc_base/numerics/safe_minmax.h',
      'libwebrtc/rtc_base/strings/string_builder.h',
      'libwebrtc/rtc_base/system/arch.h',
      'libwebrtc/rtc_base/system/asm_defines.h',
      'libwebrtc/rtc_base/system/fallthrough.h',
      'libwebrtc/rtc_base/system/ignore_warnings.h',
      'libwebrtc/rtc_base/system/inline.h',
      'libwebrtc/rtc_base/system/rtc_export.h',
      'libwebrtc/rtc_base/system/unused.h',
      'libwebrtc/rtc_base/units/unit_base.h',
      'libwebrtc/system_wrappers/include/sleep.h',
      'libwebrtc/system_wrappers/include/ntp_time.h',
      'libwebrtc/modules/audio_coding/neteq/packet.h',
      'libwebrtc/modules/audio_coding/neteq/red_payload_splitter.h',
      'libwebrtc/modules/audio_coding/neteq/tick_timer.h',
      'libwebrtc/modules/rtp_rtcp/include/receive_statistics.h',
      'libwebrtc/modules/rtp_rtcp/include/rtp_cvo.h',
      'libwebrtc/modules/rtp_rtcp/include/rtp_header_extension_map.h',
      'libwebrtc/modules/rtp_rtcp/include/rtp_header_parser.h',
      # 'libwebrtc/modules/rtp_rtcp/include/rtp_rtcp_defines.h',
      'libwebrtc/modules/rtp_rtcp/source/byte_io.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_format.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_header_extension_size.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_header_extensions.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_packet_received.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_packet.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_rtcp_config.h',
      'libwebrtc/modules/rtp_rtcp/source/rtp_video_header.h',
      'libwebrtc/modules/rtp_rtcp/source/time_util.h',
      'libwebrtc/modules/video_coding/codecs/interface/common_constants.h',
      # lym
      'libwebrtc/modules/video_coding/codecs/h264/include/h264_globals.h',
      'libwebrtc/modules/video_coding/codecs/vp8/include/vp8_globals.h',
      'libwebrtc/modules/video_coding/codecs/vp9/include/vp9_globals.h',
      'libwebrtc/rtc_base/thread_annotations.h',
      'libwebrtc/system_wrappers/include/clock.h',
      'libwebrtc/rtc_base/synchronization/rw_lock_wrapper.h',
      'libwebrtc/rtc_base/synchronization/rw_lock_posix.h',
      # 'libwebrtc/rtc_base/synchronization/rw_lock_win.h',
      'libwebrtc/api/video/color_space.h',
      'libwebrtc/api/audio_codecs/audio_format.h',
      # lym end red
      'libwebrtc/api/array_view.h',
      'libwebrtc/api/media_types.h',
      'libwebrtc/api/rtp_headers.h',
      'libwebrtc/api/rtp_parameters.h',
      'libwebrtc/api/scoped_refptr.h',
      'libwebrtc/api/audio_codecs/audio_decoder.h',
      'libwebrtc/api/transport/network_types.h',

      'libwebrtc/api/video/hdr_metadata.h',
      'libwebrtc/api/video/video_content_type.h',
      'libwebrtc/api/video/video_timing.h',
      'libwebrtc/api/video/video_frame_marking.h',
      'libwebrtc/api/video/video_rotation.h',
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
      ['OS != "win"', {
        'defines': [
          'WEBRTC_POSIX',
        ],
      }],
      [ 'OS == "mac"', {
        'defines': [
          'WEBRTC_MAC',
        ],
      }],
      [
        'OS == "linux"', {
            'defines': [
                'WEBRTC_LINUX',
            ],
        },
      ],
     ['OS =="win"', {
        'defines': [
          'WEBRTC_WIN',
        ],
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
      'type': 'static_library',
      'include_dirs': [   # 指定libmath.a头文件路径
        'deps/abseil-cpp/abseil-cpp'
      ],
      'libraries': [      # 指定链接的头文件路径和名称
        'deps/abseil-cpp/abseil.a'
      ]
    }
    
  ]
}
