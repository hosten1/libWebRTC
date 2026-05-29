# Windows 编译指南

本文档说明如何在 Windows 环境下使用 MSVC 编译 libwebrtc 项目。

## 环境要求

- **操作系统**: Windows 10/11
- **编译器**: Visual Studio  2017（或更高版本）
- **构建工具**: CMake 3.16+
- **依赖管理**: Git

## 获取源码

```bash
git clone https://github.com/your-repo/libwebrtc.git
cd libwebrtc
```

## 初始化子模块

项目依赖 Abseil-cpp 库，需要初始化子模块：

```bash
git submodule update --init --recursive
```

## 使用 MSVC 编译

### 1. 安装 Visual Studio

安装 Visual Studio 2017 或更高版本，确保包含以下工作负载：
- **使用 C++ 的桌面开发**
- **Windows SDK**（默认安装）

### 2. 打开 Developer Command Prompt

从开始菜单打开 **"x64 Native Tools Command Prompt for VS 2017"**。

### 3. 创建构建目录

```bash
mkdir build
cd build
```

### 4. 配置 CMake

```bash
cmake .. -G "Visual Studio 15 2017" -A x64
```

**可选参数**：

| 参数 | 说明 | 默认值 |
|------|------|--------|
| `-DWEBRTC_BUILD_RED=OFF` | 禁用 RED 冗余编码模块 | ON |
| `-DWEBRTC_BUILD_FEC=OFF` | 禁用 FEC 前向纠错模块 | ON |
| `-DWEBRTC_BUILD_VIDEO=ON` | 启用视频编解码模块 | OFF |
| `-DWEBRTC_BUILD_COMMON_VIDEO=ON` | 启用公共视频模块 | OFF |

**使用示例**：

```bash
# 禁用 RED 和 FEC 模块
cmake .. -G "Visual Studio 15 2017" -A x64 -DWEBRTC_BUILD_RED=OFF -DWEBRTC_BUILD_FEC=OFF

# 启用视频模块
cmake .. -G "Visual Studio 15 2017" -A x64 -DWEBRTC_BUILD_VIDEO=OFF -DWEBRTC_BUILD_COMMON_VIDEO=OFF -DWEBRTC_BUILD_RED=OFF -DWEBRTC_BUILD_FEC=OFF
```

### 5. 编译

```bash
cmake --build . --config Release
```

### 6. 安装（可选）

```bash
cmake --install . --config Release
```

## 模块选项

项目支持通过 CMake 选项控制编译模块：

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `WEBRTC_BUILD_RED` | RED 冗余编码模块 | ON |
| `WEBRTC_BUILD_FEC` | FEC 前向纠错模块 | ON |
| `WEBRTC_BUILD_VIDEO` | 视频编解码模块 | OFF |
| `WEBRTC_BUILD_COMMON_VIDEO` | 公共视频模块 | OFF |
| `WEBRTC_BUILD_GCC_ONLY` | GCC 特定代码 | OFF |

## 常见问题

### 1. 编译错误：找不到 abseil

**原因**: 子模块未初始化

**解决**:
```bash
git submodule update --init --recursive
```

### 2. 编码问题

项目已配置 UTF-8 编码支持，如仍有问题，检查源文件编码是否为 UTF-8。

### 3. 链接错误

确保所有依赖库正确安装，包括：
- Abseil-cpp
- C++ 标准库

## 输出文件

编译成功后：

- **静态库**: `build/Release/webrtc.lib`
- **头文件**: 安装后位于 `build/install/include/webrtc/`

## 验证编译

```bash
# 检查库文件
dir build\Release\webrtc.lib

# 使用 dumpbin 检查符号
dumpbin /SYMBOLS build\Release\webrtc.lib | head -30
```
