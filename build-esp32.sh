#!/bin/bash
# Build script for ESP32

echo "======================================"
echo "LVGL Calculator - ESP32 Build"
echo "======================================"
echo ""

# Get the directory where the script is located
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$SCRIPT_DIR"

# Check if IDF is set up
if [ -z "$IDF_PATH" ]; then
    echo "ERROR: ESP-IDF not found!"
    echo "Please run: source ~/esp/esp-idf/export.sh"
    echo "Or wherever your ESP-IDF is installed"
    echo ""
    read -p "Press Enter to exit..."
    exit 1
fi

echo "ESP-IDF found at: $IDF_PATH"
echo ""

# Navigate to ESP32 project
cd esp32

echo "Step 1: Cleaning previous build..."
idf.py fullclean
echo ""

echo "Step 2: Configuring project..."
idf.py set-target esp32
echo ""

echo "Step 3: Building project..."
idf.py build

if [ $? -eq 0 ]; then
    echo ""
    echo "======================================"
    echo "Build completed successfully!"
    echo "======================================"
    echo ""
    echo "To flash to ESP32, run:"
    echo "  cd esp32"
    echo "  idf.py flash monitor"
    echo ""
    read -p "Do you want to flash now? (y/n) " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Flashing to ESP32..."
        idf.py flash monitor
    fi
else
    echo ""
    echo "======================================"
    echo "ERROR: Build failed!"
    echo "======================================"
    echo ""
    read -p "Press Enter to exit..."
    exit 1
fi
