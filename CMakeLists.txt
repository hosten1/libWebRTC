cmake_minimum_required(VERSION 3.16)
project(libwebrtc LANGUAGES C CXX)
message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 引入公共编译配置模块
include(${PROJECT_SOURCE_DIR}/cmake/webrtc_build_config.cmake)

# 调用公共编译配置宏
webrtc_add_definitions()
webrtc_add_compile_options()

# ==================== 项目路径 ====================
set(WEBRTC_SRC_DIR ${PROJECT_SOURCE_DIR}/libwebrtc)
set(DEPS_DIR ${PROJECT_SOURCE_DIR}/deps)

# ==================== 编译选项 ====================
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# 预编译选项 - 按功能控制模块编译
option(WEBRTC_BUILD_WEBRTC_THREAD "Build WebRTC threading modules" ON)
option(WEBRTC_BUILD_WEBRTC_STUN "Build WebRTC STUN related modules" ON)
option(WEBRTC_BUILD_WEBRTC_AUDIO_TOOL "Build WebRTC audio tool modules" ON)
option(WEBRTC_BUILD_RED "Build RED (RTP redundancy) modules" ON)
option(WEBRTC_BUILD_FEC "Build FEC (Forward Error Correction) modules" ON)
option(WEBRTC_BUILD_VIDEO "Build video-related modules" ON)
option(WEBRTC_BUILD_COMMON_VIDEO "Build common video modules" ON)
option(WEBRTC_BUILD_GCC "Build GCC modules" ON)
option(WEBRTC_BUILD_WEBRTC_LOGGING "Build WebRTC logging related modules" ON)

# 模块依赖检查
if(WEBRTC_BUILD_VIDEO AND NOT WEBRTC_BUILD_COMMON_VIDEO)
    message(STATUS "[libwebrtc] Auto-enabling WEBRTC_BUILD_COMMON_VIDEO (required by video modules)")
    set(WEBRTC_BUILD_COMMON_VIDEO ON CACHE BOOL "Build common video modules" FORCE)
endif()

if(WEBRTC_BUILD_COMMON_VIDEO AND NOT WEBRTC_BUILD_VIDEO)
    message(WARNING "[libwebrtc] WEBRTC_BUILD_COMMON_VIDEO enabled but WEBRTC_BUILD_VIDEO disabled. Common video modules may not be used.")
endif()

# ==================== 子模块 ====================
# 引入各子模块（均创建为 OBJECT 库，遵循 rtc_base 模式）
add_subdirectory(${WEBRTC_SRC_DIR}/rtc_base)
add_subdirectory(${WEBRTC_SRC_DIR}/api)
add_subdirectory(${WEBRTC_SRC_DIR}/system_wrappers)
add_subdirectory(${WEBRTC_SRC_DIR}/common_audio)
add_subdirectory(${WEBRTC_SRC_DIR}/modules)

# 第三方依赖
add_subdirectory(${DEPS_DIR}/abseil-cpp/abseil-cpp ${PROJECT_BINARY_DIR}/abseil)

# ==================== 创建 libwebrtc 主库 ====================
add_library(libwebrtc STATIC
    $<TARGET_OBJECTS:rtc_base>
    $<TARGET_OBJECTS:api>
    $<TARGET_OBJECTS:system_wrappers>
    $<TARGET_OBJECTS:common_audio_module>
    $<TARGET_OBJECTS:webrtc_modules>
)

target_include_directories(libwebrtc PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${WEBRTC_SRC_DIR}
    ${DEPS_DIR}/abseil-cpp/abseil-cpp
)

target_link_libraries(libwebrtc PUBLIC
    abseil
    spl_sqrt_floor
)

if(MSVC)
    target_compile_options(libwebrtc PUBLIC /W0 /utf-8)
else()
    target_compile_options(libwebrtc PRIVATE
        $<$<COMPILE_LANGUAGE:CXX>:-include;cstdint>
    )
endif()

set_target_properties(libwebrtc PROPERTIES OUTPUT_NAME webrtc)

# ==================== 安装 ====================
install(TARGETS libwebrtc
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY ${WEBRTC_SRC_DIR}/
        DESTINATION include/webrtc
        FILES_MATCHING PATTERN "*.h")

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


if(WIN32)
        # ==================== 测试程序 ====================
add_executable(test_real_rtp_packet test/test_real_rtp_packet.cc)
target_link_libraries(test_real_rtp_packet PRIVATE
    libwebrtc
    abseil
)

add_executable(test_webrtc_threading test/test_webrtc_threading.cc)
target_link_libraries(test_webrtc_threading PRIVATE
    libwebrtc
    abseil
)

        if(WIN32)
        target_link_libraries(test_real_rtp_packet PRIVATE ws2_32 winmm)
        target_link_libraries(test_webrtc_threading PRIVATE ws2_32 winmm)
        endif()
endif()

