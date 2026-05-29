#!/bin/bash

CURRENT_WORK=$(pwd)
WORK_NAME=libwebrtc
# 设置 gyp 脚本路径和输出目录
GYP_SCRIPT="libwebrtc.gyp"
OUTPUT_DIR="out"
GYP_ROOT_PATH="$(pwd)/deps/node-gyp/gyp"

# 设置编译环境变量
export GYP_DEFINES="OS=macos root=${GYP_ROOT_PATH} node_byteorder=$(uname -m | grep -q '64' && echo 'little' || echo 'big')"
export CXXFLAGS="-std=c++11 -isystem deps/abseil-cpp/abseil-cpp -I."
export CFLAGS="-std=c11"
export CC=clang
export CXX=clang++

# 判断目录是否存在， deps 目录不存在就创建

mkdir -p deps

# 先检查 build目录是否存在，然后检查是否存在Makefile
if [ ! -d "build" ] || [ ! -f "build/Makefile" ] || [ "$1" = "clean" ]; then
    echo "需要重新配置项目..."
    if [ "$1" = "clean" ]; then
        echo "清理构建文件..."
        rm -rf build
        rm -rf $OUTPUT_DIR
    fi
    
    echo "配置项目..."
     echo "配置项目..."
    node-gyp configure --depth=. --generator-output=$OUTPUT_DIR -f xcode "$GYP_SCRIPT"
fi

# 运行 gyp 构建
echo "开始构建项目..."
# node-gyp build --release --verbose --directory=$OUTPUT_DIR

# 检查构建是否成功
if [ $? -eq 0 ]; then
  echo "Build succeeded!"
else
  echo "Build failed."
fi
