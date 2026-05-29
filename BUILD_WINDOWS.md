# Windows 编译指南

本文档说明如何在 Windows 环境下编译 libwebrtc 项目。

## 环境要求

- **操作系统**: Windows 10/11
- **编译器**: MinGW-w64 (GCC) 或 MSVC (Visual Studio)
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

## 使用 MinGW 编译（推荐）

### 1. 安装 MinGW-w64

推荐使用 MSYS2 安装 MinGW-w64：

```bash
# 安装 MSYS2
# 下载地址: https://www.msys2.org/

# 使用pacman安装GCC
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make
```

### 2. 配置环境变量

将 MinGW 添加到系统 PATH：

```
C:\msys64\mingw64\bin
```

### 3. 创建构建目录

```bash
mkdir build
cd build
```

### 4. 配置 CMake

```bash
cmake .. -G "MinGW Makefiles"
```

### 5. 编译

```bash
mingw32-make -j8
```

### 6. 安装（可选）

```bash
mingw32-make install
```

## 使用 MSVC 编译

### 1. 安装 Visual Studio

安装 Visual Studio 2022 或更高版本，确保包含 "使用 C++ 的桌面开发" 工作负载。

### 2. 打开 Developer Command Prompt

从开始菜单打开 "x64 Native Tools Command Prompt for VS 2022"。

### 3. 创建构建目录

```bash
mkdir build
cd build
```

### 4. 配置 CMake

```bash
cmake .. -G "Visual Studio 17 2022" -A x64
```

### 5. 编译

```bash
cmake --build . --config Release
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

### 使用示例

```bash
# 启用视频模块
cmake .. -G "MinGW Makefiles" -DWEBRTC_BUILD_VIDEO=ON -DWEBRTC_BUILD_COMMON_VIDEO=ON

# 禁用 RED 和 FEC
cmake .. -G "MinGW Makefiles" -DWEBRTC_BUILD_RED=OFF -DWEBRTC_BUILD_FEC=OFF

# 最小配置（全部禁用可选模块）
cmake .. -G "MinGW Makefiles" -DWEBRTC_BUILD_RED=OFF -DWEBRTC_BUILD_FEC=OFF -DWEBRTC_BUILD_VIDEO=OFF -DWEBRTC_BUILD_COMMON_VIDEO=OFF
```

## 常见问题

### 1. 编译错误：找不到 abseil

**原因**: 子模块未初始化

**解决**:
```bash
git submodule update --init --recursive
```

### 2. 编译错误：MSVC 选项被传递给 GCC

**原因**: Abseil 的 CMakeLists.txt 使用了 `WIN32` 判断，应该使用 `MSVC`

**解决**: 编辑 `deps/abseil-cpp/abseil-cpp/CMakeLists.txt`，将 `if(WIN32)` 改为 `if(MSVC)`

### 3. 编码问题

项目已配置 UTF-8 编码支持，如仍有问题，检查源文件编码是否为 UTF-8。

### 4. 链接错误

确保所有依赖库正确安装，包括：
- Abseil-cpp
- C++ 标准库

## 输出文件

编译成功后：

- **静态库**: `build/libwebrtc.a` (MinGW) 或 `build/Release/libwebrtc.lib` (MSVC)
- **头文件**: 安装后位于 `include/webrtc/`

## 验证编译

```bash
# 检查库文件
ls -la build/*.a    # MinGW
ls -la build/*.lib  # MSVC

# 检查符号（MinGW）
nm build/libwebrtc.a | head -20
```
