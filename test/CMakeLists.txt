# WebRTC 测试程序
# 通过根 CMakeLists.txt 中 WEBRTC_BUILD_TESTS 选项控制编译

# ==================== RTP 包测试 ====================
add_executable(test_real_rtp_packet test_real_rtp_packet.cc)
target_link_libraries(test_real_rtp_packet PRIVATE
    libwebrtc
    abseil
)

# ==================== 线程测试 ====================
add_executable(test_webrtc_threading test_webrtc_threading.cc)
target_link_libraries(test_webrtc_threading PRIVATE
    libwebrtc
    abseil
)

# Windows 平台特定链接
if(WIN32)
    target_link_libraries(test_real_rtp_packet PRIVATE ws2_32 winmm)
    target_link_libraries(test_webrtc_threading PRIVATE ws2_32 winmm)
endif()

# ==================== STUN 测试（需要 OpenSSL） ====================
if(WEBRTC_BUILD_WEBRTC_STUN)
    add_executable(test_stun test_stun.cc)
    target_link_libraries(test_stun PRIVATE
        libwebrtc
        abseil
        OpenSSL::SSL
        OpenSSL::Crypto
    )

    if(WIN32)
        target_link_libraries(test_stun PRIVATE ws2_32 winmm)
    elseif(APPLE)
        if(IOS)
            target_link_libraries(test_stun PRIVATE "-framework CoreFoundation" "-framework Foundation")
        else()
            target_link_libraries(test_stun PRIVATE "-framework CoreFoundation" "-framework Foundation" "-framework AppKit")
        endif()
    endif()
endif()