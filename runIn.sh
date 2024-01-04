#!/bin/bash

# 设置 gyp 脚本路径和输出目录
GYP_SCRIPT="libwebrtc.gyp"
OUTPUT_DIR="out"
GYP_ROOT_PATH='/Users/luoyongmeng/Documents/lym/gyp-build-system/node-gyp/gyp'
export GYP_DEFINES="OS=macos root=${GYP_ROOT_PATH}"    #修正此行，确保正确设置GYP_DEFINES环境变量

# 清理之前的构建
rm -rf $OUTPUT_DIR

# 运行 gyp 生成 Xcode 项目文件
node-gyp  "$GYP_SCRIPT" --depth=. -f xcode --generator-output=$OUTPUT_DIR

# 使用 xcodebuild 构建项目
xcodebuild -project "$OUTPUT_DIR/YourProject.xcodeproj" -configuration Release

# 检查构建是否成功
if [ $? -eq 0 ]; then
  echo "Build succeeded!"
else
  echo "Build failed."
fi
