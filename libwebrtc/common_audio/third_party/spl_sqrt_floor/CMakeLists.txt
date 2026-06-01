# spl_sqrt_floor 库
# 用于计算整数平方根的高效实现
if(NOT WEBRTC_SRC_DIR)
    message(FATAL_ERROR "WEBRTC_SRC_DIR required")
endif()
add_library(spl_sqrt_floor STATIC
     ${WEBRTC_SRC_DIR}/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.c
     ${WEBRTC_SRC_DIR}/common_audio/third_party/spl_sqrt_floor/spl_sqrt_floor.h
)

# 添加头文件目录
target_include_directories(spl_sqrt_floor
    PUBLIC
        ${WEBRTC_SRC_DIR}
)

# 设置编译选项
target_compile_options(spl_sqrt_floor PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W0>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w>
)

message(STATUS "[libwebrtc] Added spl_sqrt_floor library")
