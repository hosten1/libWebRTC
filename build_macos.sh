#!/bin/bash
set -e

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
cd "$SCRIPT_DIR"

if [[ "$OSTYPE" != "darwin"* ]]; then
    echo "Error: This script only runs on macOS"
    exit 1
fi

BUILD_DIR="build-macos"
ARCH="x86_64"
DEPLOYMENT_TARGET="10.15"

echo "=== macOS Build Configuration ==="
echo "Architecture: $ARCH"
echo "Deployment Target: $DEPLOYMENT_TARGET"
echo "Build Directory: $BUILD_DIR"
echo ""

rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "Running CMake..."
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_ARCHITECTURES="$ARCH" \
    -DCMAKE_OSX_DEPLOYMENT_TARGET="$DEPLOYMENT_TARGET"

echo ""
echo "Building..."
cmake --build . --config Release -j$(sysctl -n hw.ncpu)

echo ""
echo "=== Build Complete ==="
echo "Library: $SCRIPT_DIR/$BUILD_DIR/libwebrtc.a"
file "$SCRIPT_DIR/$BUILD_DIR/libwebrtc.a"
lipo -info "$SCRIPT_DIR/$BUILD_DIR/libwebrtc.a"