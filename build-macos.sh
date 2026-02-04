#!/bin/bash
# Build script for macOS

echo "======================================"
echo "LVGL Calculator - macOS Build"
echo "======================================"
echo

cd "$(dirname "$0")"

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "ERROR: Homebrew not found!"
    echo "Please install Homebrew from https://brew.sh/"
    exit 1
fi

# Check if SDL2 is installed
if ! brew list sdl2 &> /dev/null; then
    echo "SDL2 not found. Installing via Homebrew..."
    brew install sdl2
fi

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "CMake not found. Installing via Homebrew..."
    brew install cmake
fi

echo "Step 1: Configuring and building..."
echo

cd desktop

cmake -B build \
    -DCMAKE_BUILD_TYPE=Release \
    -DCONFIG_LV_USE_THORVG_INTERNAL=OFF \
    -DCONFIG_LV_BUILD_DEMOS=OFF \
    -DCONFIG_LV_BUILD_EXAMPLES=OFF \
    .

if [ $? -ne 0 ]; then
    echo
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

cmake --build build -j$(sysctl -n hw.ncpu)

if [ $? -ne 0 ]; then
    echo
    echo "ERROR: Build failed!"
    exit 1
fi

echo
echo "======================================"
echo "Build completed successfully!"
echo "======================================"
echo
echo "Executable location: desktop/bin/main"
echo
echo "To run: ./desktop/bin/main"
