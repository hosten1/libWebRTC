# Android 编译指南

本文档说明如何在 **Windows** 上交叉编译 libwebrtc 为 Android 平台的 `.a` 静态库。

## 环境要求

- **操作系统**: Windows 10/11
- **Android NDK**: r25b 或更高版本
- **构建工具**: CMake 3.16+
- **依赖管理**: Git

## 安装 Android NDK

### 方法一：使用 Android Studio（推荐）

1. 安装 [Android Studio](https://developer.android.com/studio)
2. 打开 Android Studio，进入 **SDK Manager**
3. 在 **SDK Tools** 标签页中勾选 **NDK (Side by side)**
4. 选择 NDK 版本（推荐 r25b），点击 **Apply** 安装

### 方法二：手动下载

1. 访问 [NDK 下载页面](https://developer.android.com/ndk/downloads)
2. 下载 Windows 版本的 NDK（如 `android-ndk-r25b-windows.zip`）
3. 解压到本地目录，例如 `C:\Android\ndk-r25b`

## 获取源码

```bash
git clone https://github.com/your-repo/libwebrtc.git
cd libwebrtc
```

## 初始化子模块

```bash
git submodule update --init --recursive
```

## 配置环境变量

打开 **Developer Command Prompt for VS 2022**，设置 NDK 路径：

```bash
set ANDROID_NDK=C:\Android\ndk-r25b
```

## 编译步骤

### 1. 创建构建目录

```bash
mkdir build-android
cd build-android
```

### 2. 配置 CMake（编译 arm64-v8a）

```bash
cmake .. -G "Unix Makefiles" -DCMAKE_SYSTEM_NAME=Android -DCMAKE_SYSTEM_VERSION=24 -DCMAKE_ANDROID_ARCH_ABI=arm64-v8a -DCMAKE_ANDROID_NDK="C:/Users/Administrator/AppData/Local/Android/Sdk/ndk/25.0.8151533" -DCMAKE_ANDROID_STL_TYPE=c++_static -DCMAKE_BUILD_TYPE=Releas
```

### 3. 编译

```bash
 cmake --build . --config Release -j8
```

## 支持的架构

| 架构 | CMAKE_ANDROID_ARCH_ABI | 命令参数 |
|------|----------------------|----------|
| ARM64 | arm64-v8a | `-DCMAKE_ANDROID_ARCH_ABI=arm64-v8a` |
| ARMv7 | armeabi-v7a | `-DCMAKE_ANDROID_ARCH_ABI=armeabi-v7a` |
| x86 | x86 | `-DCMAKE_ANDROID_ARCH_ABI=x86` |
| x86_64 | x86_64 | `-DCMAKE_ANDROID_ARCH_ABI=x86_64` |

## 模块选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `WEBRTC_BUILD_RED` | RED 冗余编码模块 | ON |
| `WEBRTC_BUILD_FEC` | FEC 前向纠错模块 | ON |
| `WEBRTC_BUILD_VIDEO` | 视频编解码模块 | OFF |
| `WEBRTC_BUILD_COMMON_VIDEO` | 公共视频模块 | OFF |

## 输出文件

编译成功后：

- **静态库**: `build-android/libwebrtc.a`
- **头文件**: 安装后位于 `build-android/install/include/webrtc/`

## 安装（可选）

```bash
mingw32-make install
```

## 常见问题

### 1. 找不到 NDK

**原因**: ANDROID_NDK 环境变量未正确设置

**解决**:
```bash
set ANDROID_NDK=C:\path\to\your\ndk
```

### 2. 编译错误：缺少 make 工具

**原因**: 未安装 MinGW 或未添加到 PATH

**解决**:
- 安装 MinGW-w64
- 将 `C:\msys64\mingw64\bin` 添加到系统 PATH

### 3. CMake 配置失败

**原因**: NDK 版本不兼容或路径错误

**解决**:
- 使用推荐的 NDK r25b 版本
- 确保路径中不包含空格或中文

## 在 Android 项目中使用

将编译生成的 `libwebrtc.a` 复制到 Android 项目的 `jniLibs/arm64-v8a/`（或对应架构）目录，然后在 `CMakeLists.txt` 中链接该库。
