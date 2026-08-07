# libWebRTC

基于 WebRTC 源码裁剪的跨平台 WebRTC 静态库，支持 macOS / iOS / Android。

## 目录结构

```
libwebrtc/          # WebRTC 核心源码
deps/               # 第三方依赖
cmake/              # CMake 配置
test/               # 接口测试 Demo
```

## 编译

### macOS

```bash
bash build_macos.sh
```

### iOS (simulator + device 双架构)

```bash
bash build_ios.sh
```

### Android (arm64-v8a)

```bash
export ANDROID_NDK=/path/to/android-ndk
bash build_android.sh
```

## 测试

### 运行全量接口测试

```bash
mkdir build-macos-test && cd build-macos-test
cmake .. -DWEBRTC_BUILD_TESTS=ON
cmake --build . --target test_demo
./test/test_demo
```

测试覆盖模块：STUN、RTP、音频、视频、FEC(ULPFEC/FlexFEC)、GCC 拥塞控制、线程、TaskQueue 等。

## 模块选项

通过 CMake 选项控制编译模块（默认均为 ON）：

| 选项 | 说明 |
|------|------|
| WEBRTC_BUILD_WEBRTC_STUN | STUN 协议模块 |
| WEBRTC_BUILD_WEBRTC_AUDIO_TOOL | 音频工具模块 |
| WEBRTC_BUILD_AUDIO_SIGNAL_PROCESSING | 音频信号处理 |
| WEBRTC_BUILD_FEC | 前向纠错 (ULPFEC + FlexFEC) |
| WEBRTC_BUILD_VIDEO_TOOL | 视频工具模块 |
| WEBRTC_BUILD_GCC | GCC 拥塞控制 |
| WEBRTC_BUILD_TESTS | 测试 Demo |
