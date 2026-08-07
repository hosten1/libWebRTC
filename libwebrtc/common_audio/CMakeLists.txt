# ==================== [common_audio] 编译诊断 ====================
message(STATUS "[common_audio] ====== Module: common_audio ======")
message(STATUS "[common_audio] OS=${CMAKE_SYSTEM_NAME} ARCH=${CURRENT_CPU} COMPILER=${CMAKE_CXX_COMPILER_ID}")
message(STATUS "[common_audio] Features: AUDIO_SIG_PROC=${WEBRTC_BUILD_AUDIO_SIGNAL_PROCESSING} AUDIO_TOOL=${WEBRTC_BUILD_WEBRTC_AUDIO_TOOL}")
message(STATUS "[common_audio] CPU: ${CURRENT_CPU} NEON=${ARM_USE_NEON}")

set(WEBRTC_COMMON_AUDIO_SOURCES "")

# ==================== signal_processing 模块 ====================
if(WEBRTC_BUILD_AUDIO_SIGNAL_PROCESSING)
    message(STATUS "[libwebrtc] Building audio signal processing library...")

    # 通用 signal_processing 源文件（所有平台共有）
    set(WEBRTC_SIGNAL_PROCESSING_COMMON_SOURCES
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/auto_corr_to_refl_coef.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/auto_correlation.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/complex_fft_tables.h
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/copy_set_operations.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/cross_correlation.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/division_operations.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/downsample_fast.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/energy.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/filter_ar.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/filter_ma_fast_q12.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/get_hanning_window.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/get_scaling_square.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/ilbc_specific_functions.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/include/real_fft.h
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/include/signal_processing_library.h
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/include/spl_inl.h
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/include/spl_inl_armv7.h
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/levinson_durbin.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/lpc_to_refl_coef.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/min_max_operations.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/randomization_functions.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/real_fft.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/refl_coef_to_lpc.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/resample.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/resample_48khz.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/resample_by_2.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/resample_by_2_internal.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/resample_by_2_internal.h
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/resample_fractional.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/spl_init.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/spl_inl.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/spl_sqrt.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/splitting_filter.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/sqrt_of_one_minus_x_squared.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/vector_scaling_operations.c
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/dot_product_with_scale.cc
        ${WEBRTC_SRC_DIR}/common_audio/signal_processing/dot_product_with_scale.h
    )

    list(APPEND WEBRTC_COMMON_AUDIO_SOURCES ${WEBRTC_SIGNAL_PROCESSING_COMMON_SOURCES})

    # 平台相关的 signal_processing 源文件
    if(CURRENT_CPU STREQUAL "mipsel")
        message(STATUS "[common_audio] Signal processing: MIPS platform")
        # MIPS 平台
        list(APPEND WEBRTC_COMMON_AUDIO_SOURCES
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/complex_bit_reverse_mips.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/complex_fft_mips.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/cross_correlation_mips.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/downsample_fast_mips.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/filter_ar_fast_q12_mips.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/include/spl_inl_mips.h
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/min_max_operations_mips.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/resample_by_2_mips.c
        )
        if(MIPS_DSP_REV GREATER_EQUAL 1)
            message(STATUS "[common_audio] MIPS DSP REV >= 1: adding vector_scaling_operations_mips")
            list(APPEND WEBRTC_COMMON_AUDIO_SOURCES
                ${WEBRTC_SRC_DIR}/common_audio/signal_processing/vector_scaling_operations_mips.c
            )
        endif()
        message(STATUS "[common_audio] MIPS sources: MIPS_DSP_REV=${MIPS_DSP_REV} MIPS_FLOAT_ABI=${MIPS_FLOAT_ABI}")
    else()
        # 非 MIPS 平台（x86, x64, arm 等）使用通用 complex_fft
        message(STATUS "[common_audio] Signal processing: non-MIPS (${CURRENT_CPU}) - using generic complex_fft")
        list(APPEND WEBRTC_COMMON_AUDIO_SOURCES
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/complex_fft.c
        )
    endif()

    # 非 MIPS 平台统一使用 C 版本的 complex_bit_reverse 和 filter_ar_fast_q12
    # （ARM 汇编优化暂未启用，如需启用需配置 ASM 语言支持和对应工具链）
    if(NOT CURRENT_CPU STREQUAL "mipsel")
        list(APPEND WEBRTC_COMMON_AUDIO_SOURCES
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/complex_bit_reverse.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/filter_ar_fast_q12.c
        )
    endif()

    # NEON 优化（arm64 默认支持，armv7+NEON 可选）
    if(CURRENT_CPU STREQUAL "arm64" OR (CURRENT_CPU STREQUAL "arm" AND ARM_USE_NEON))
        list(APPEND WEBRTC_COMMON_AUDIO_SOURCES
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/cross_correlation_neon.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/downsample_fast_neon.c
            ${WEBRTC_SRC_DIR}/common_audio/signal_processing/min_max_operations_neon.c
        )
    endif()

else()
    message(STATUS "[libwebrtc] Signal processing library disabled")
endif()

# ==================== 音频工具模块 ====================
if(WEBRTC_BUILD_WEBRTC_AUDIO_TOOL)
    message(STATUS "[common_audio] AUDIO_TOOL: enabled - adding audio_util.cc")
    list(APPEND WEBRTC_COMMON_AUDIO_SOURCES
        ${WEBRTC_SRC_DIR}/common_audio/audio_util.cc
    )
else()
    message(STATUS "[common_audio] AUDIO_TOOL: disabled")
endif()

# ==================== 第三方依赖 ====================
set(OOURA_OBJECTS "")
set(SPL_SQRT_FLOOR_OBJECTS "")

if(WEBRTC_BUILD_AUDIO_SIGNAL_PROCESSING)
    add_subdirectory(${WEBRTC_SRC_DIR}/common_audio/third_party/ooura)
    add_subdirectory(${WEBRTC_SRC_DIR}/common_audio/third_party/spl_sqrt_floor)
    set(OOURA_OBJECTS $<TARGET_OBJECTS:ooura>)
    set(SPL_SQRT_FLOOR_OBJECTS $<TARGET_OBJECTS:spl_sqrt_floor>)
endif()

# ==================== 构建 OBJECT 库 ====================
if(WEBRTC_COMMON_AUDIO_SOURCES OR OOURA_OBJECTS OR SPL_SQRT_FLOOR_OBJECTS)
    add_library(common_audio_module OBJECT
        ${WEBRTC_COMMON_AUDIO_SOURCES}
        ${OOURA_OBJECTS}
        ${SPL_SQRT_FLOOR_OBJECTS}
    )

    target_include_directories(
        common_audio_module
        PRIVATE
            ${WEBRTC_SRC_DIR}
            ${DEPS_DIR}/abseil-cpp/abseil-cpp
    )

    list(LENGTH WEBRTC_COMMON_AUDIO_SOURCES _src_count)
    message(STATUS "[common_audio] Module sources: ${_src_count} files + ooura/spl_sqrt_floor")
    message(STATUS "[common_audio] common_audio_module configured")
else()
    message(STATUS "[common_audio] Module skipped (no sources)")
endif()

# ==================== [common_audio] 编译摘要 ====================
message(STATUS "[common_audio] ====== End of common_audio ======")
