cmake_minimum_required(VERSION 3.16)
project(libwebrtc LANGUAGES C CXX)
message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
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

# ==================== 平台检测与预定义宏 ====================
# 注意：原始 GN 中 is_posix 包含 Linux, Android, macOS, iOS, Fuchsia 等
if(CMAKE_SYSTEM_NAME MATCHES "Linux|Darwin|Android|iOS")
    set(IS_POSIX TRUE)
else()
    set(IS_POSIX FALSE)
endif()

# 编译器检测
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(IS_CLANG TRUE)
else()
    set(IS_CLANG FALSE)
endif()

# use_xcode_clang 大致对应 iOS 平台且使用 AppleClang
if(IOS AND CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(USE_XCODE_CLANG TRUE)
else()
    set(USE_XCODE_CLANG FALSE)
endif()

# 处理器架构检测 
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64|ARM64")
    set(CURRENT_CPU "arm64")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|armv7|armv7-a")
    set(CURRENT_CPU "arm")
    # 简单判断 arm 版本：假设 v7 及以上
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7")
        set(ARM_VERSION 7)
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv8|aarch64")
        set(ARM_VERSION 8)
    else()
        set(ARM_VERSION 6)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "mips|mipsel")
    set(CURRENT_CPU "mipsel")
    # MIPS 相关变量，如需精细控制可提供选项
    set(MIPS_FLOAT_ABI "hard")   # 默认 hard，用户可覆盖
    set(MIPS_ARCH_VARIANT "r2")
    set(MIPS_DSP_REV 1)
else()
    set(CURRENT_CPU "unknown")
endif()

# ==================== 全局编译定义（通过 add_definitions 或 target_compile_definitions） ====================
# 这些定义会应用于所有目标（由于在 add_subdirectory 前设置）
add_definitions(
    -DDISABLE_H265
)

# 平台相关定义
if(WIN32)
    add_definitions(
        -DWEBRTC_WIN
        -DNOMINMAX
        -DWIN32_LEAN_AND_MEAN
        -D_CRT_SECURE_NO_WARNINGS
    )
elseif(ANDROID)
    add_definitions(
        -DWEBRTC_POSIX
        -DWEBRTC_ANDROID
    )
elseif(APPLE)
    if(IOS)
        add_definitions(
            -DWEBRTC_POSIX
            -DWEBRTC_IOS
        )
    else()
        add_definitions(
            -DWEBRTC_POSIX
            -DWEBRTC_MAC
        )
    endif()
elseif(UNIX)
    add_definitions(
        -DWEBRTC_POSIX
        -DWEBRTC_LINUX
    )
endif()

# 架构相关定义
if(CURRENT_CPU STREQUAL "arm64")
    add_definitions(
        -DWEBRTC_ARCH_ARM64
        -DWEBRTC_HAS_NEON
    )
elseif(CURRENT_CPU STREQUAL "arm")
    add_definitions(-DWEBRTC_ARCH_ARM)
    if(ARM_VERSION GREATER_EQUAL 7)
        add_definitions(-DWEBRTC_ARCH_ARM_V7)
        if(ARM_USE_NEON)
            add_definitions(-DWEBRTC_HAS_NEON)
        endif()
    endif()
elseif(CURRENT_CPU STREQUAL "mipsel")
    add_definitions(-DMIPS32_LE)
    if(MIPS_FLOAT_ABI STREQUAL "hard")
        add_definitions(-DMIPS_FPU_LE)
    endif()
    if(MIPS_ARCH_VARIANT STREQUAL "r2")
        add_definitions(-DMIPS32_R2_LE)
    endif()
    if(MIPS_DSP_REV GREATER_EQUAL 1)
        add_definitions(-DMIPS_DSP_R1_LE)
    endif()
    if(MIPS_DSP_REV GREATER_EQUAL 2)
        add_definitions(-DMIPS_DSP_R2_LE)
    endif()
endif()

# 端序定义
if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm|aarch64")
    add_definitions(-DMS_BIG_ENDIAN)
else()
    add_definitions(-DMS_LITTLE_ENDIAN)
endif()

# Fuzzing 模式定义
if(use_fuzzing_engine AND optimize_for_fuzzing)
    add_definitions(-DWEBRTC_UNSAFE_FUZZER_MODE)
endif()

# Windows UNICODE 反定义
if(WIN32 AND NOT build_with_chromium AND rtc_win_undef_unicode)
    add_definitions(
        -UUNICODE
        -U_UNICODE
    )
endif()

# ==================== 编译器警告/优化标志 ====================
# 使用生成器表达式区分 C 和 C++ 语言
# 注意：这些标志会在 add_subdirectory 前全局生效
if(IS_POSIX)
    # C 编译器的标志
    add_compile_options(
        $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
    )
    # Objective-C 编译器的标志（如果使用）
    add_compile_options(
        $<$<COMPILE_LANGUAGE:OBJC>:-Wstrict-prototypes>
        $<$<COMPILE_LANGUAGE:OBJCXX>:-Wstrict-prototypes>
    )
    # C++ 编译器的标志
    add_compile_options(
        $<$<COMPILE_LANGUAGE:CXX>:-Wnon-virtual-dtor>
        $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>
    )
endif()

if(IS_CLANG)
    # Clang 通用警告
    add_compile_options(
        -Wc++11-narrowing
        -Wimplicit-fallthrough
        -Wthread-safety
        -Winconsistent-missing-override
        -Wundef
    )
    # 额外的警告（Clang 版本足够新时）
    if(NOT IS_NACL AND (NOT USE_XCODE_CLANG OR current_toolchain STREQUAL host_toolchain))
        add_compile_options(-Wunused-lambda-capture)
    endif()
endif()

if(WIN32 AND NOT IS_CLANG)
    # MSVC 特定：禁用 C4702（不可达代码）
    add_compile_options(/wd4702)
endif()

if(ANDROID AND NOT IS_CLANG)
    # Android NDK 非 Clang 时禁用内置数学函数优化
    add_compile_options(
        -fno-builtin-cos
        -fno-builtin-sin
        -fno-builtin-cosf
        -fno-builtin-sinf
    )
endif()


# 4. 为 .c 文件设置语言属性
set_source_files_properties(
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/min_max_operations.c
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/spl_init.c
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/spl_inl.c
    PROPERTIES LANGUAGE C
)

option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# 预编译选项 - 按功能控制模块编译
option(WEBRTC_BUILD_WEBRTC_THREAD "Build WebRTC threading modules" ON)
option(WEBRTC_BUILD_WEBRTC_STUN "Build WebRTC STUN related modules" ON)
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
   

    ${WEBRTC_SRC_DIR}/modules/include/module_common_types.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/common_header.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/rtpfb.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtcp_packet/transport_feedback.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_history.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_to_send.cc
  
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/packet.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_generic_frame_descriptor.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_header_extension_map.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_header_extension_size.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_header_extensions.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet_received.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_packet.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/time_util.cc
   
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/dot_product_with_scale.cc
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/min_max_operations.c
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/randomization_functions.c
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/real_fft.c
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/refl_coef_to_lpc.c
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/spl_init.c
    ${WEBRTC_SRC_DIR}/common_audio/signal_processing/spl_inl.c
    ${WEBRTC_SRC_DIR}/modules/audio_coding/codecs/cng/webrtc_cng.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/decoder_database.cc
    ${WEBRTC_SRC_DIR}/modules/audio_coding/neteq/tick_timer.cc
    ${WEBRTC_SRC_DIR}/system_wrappers/source/rtp_to_ntp_estimator.cc

   
)
if(CURRENT_CPU STREQUAL "arm64" OR (CURRENT_CPU STREQUAL "arm" AND ARM_USE_NEON))
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/min_max_operations_neon.c
    )
endif()

message(STATUS "[libwebrtc] Core modules configured. Sources: ${WEBRTC_SOURCES}")



# webrtc stun modules end
if(WEBRTC_BUILD_WEBRTC_STUN)
    message(STATUS "[libwebrtc] Adding STUN modules...")
    list(APPEND WEBRTC_SOURCES
        ${WEBRTC_SRC_DIR}/common_audio/audio_util.cc
        ${WEBRTC_SRC_DIR}/audio/audio_level.cc
    )
else()
    message(STATUS "[libwebrtc] STUN modules disabled")
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
        ${WEBRTC_SRC_DIR}/modules/video_coding/encoded_frame.cc
        ${WEBRTC_SRC_DIR}/modules/video_coding/include/video_codec_interface.cc
        ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/source/rtp_video_header.cc
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

# gcc modules
if(WEBRTC_BUILD_GCC)
    message(STATUS "[libwebrtc] Adding GCC modules...")
    list(APPEND WEBRTC_SOURCES
    # lym pacer
    ${WEBRTC_SRC_DIR}/modules/pacing/interval_budget.cc
    ${WEBRTC_SRC_DIR}/modules/pacing/bitrate_prober.cc
    ${WEBRTC_SRC_DIR}/modules/pacing/paced_sender.cc
    # lyn pacer end
    ${WEBRTC_SRC_DIR}/call/rtp_transport_controller_send.cc
    ${WEBRTC_SRC_DIR}/modules/rtp_rtcp/include/rtp_rtcp_defines.cc
    ${WEBRTC_SRC_DIR}/modules/congestion_controller/goog_cc/bitrate_estimator.cc
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

message(STATUS "[libwebrtc] Total sources configured: ${WEBRTC_SOURCES}")

add_subdirectory(${WEBRTC_SRC_DIR}/rtc_base)
add_subdirectory(${WEBRTC_SRC_DIR}/api)

add_library(libwebrtc STATIC
    $<TARGET_OBJECTS:rtc_base>
    $<TARGET_OBJECTS:api>
    ${WEBRTC_SOURCES}
)

target_include_directories(libwebrtc PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${WEBRTC_SRC_DIR}
    ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

if(MSVC)
    target_compile_options(libwebrtc PUBLIC /W0 /utf-8)
else()
    target_compile_options(libwebrtc PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-include;cstdint>
    )
endif()


add_subdirectory(${DEPS_DIR}/abseil-cpp/abseil-cpp ${PROJECT_BINARY_DIR}/abseil)


# add_subdirectory(${WEBRTC_SRC_DIR}/rtc_base)
# add_subdirectory(common_audio)
# add_subdirectory(${WEBRTC_SRC_DIR}/api)
# add_subdirectory(modules)
# add_subdirectory(system_wrappers)
# 添加 third_party 子项目
add_subdirectory(${WEBRTC_SRC_DIR}/common_audio/third_party/ooura)
add_subdirectory(${WEBRTC_SRC_DIR}/common_audio/third_party/spl_sqrt_floor)

target_link_libraries(libwebrtc PUBLIC
    absl_strings
    absl_base
    absl_optional
    absl_flat_hash_map
    # third_party libraries
    spl_sqrt_floor
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
