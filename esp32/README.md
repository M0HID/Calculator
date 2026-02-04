# ESP32 Build Instructions

## ST7789 Display Migration

This project has been migrated from ILI9341 to ST7789 display driver.

### Changes Made:
- ✅ Removed ILI9341 dependency from `main/idf_component.yml`
- ✅ Updated `main/driver.c` with ST7789 headers and configuration
- ✅ Set proper rotation for landscape mode (320x240)

## Building

### Prerequisites:
- ESP-IDF v5.5.1 installed at `C:\Espressif\`
- ESP32-S3 development board

### Build Commands:

#### Method 1: Using the build script
Double-click `esp32_build.bat` or run from command prompt:
```cmd
esp32_build.bat
```

#### Method 2: Using ESP-IDF Command Prompt
1. Open ESP-IDF 5.5 CMD from Start Menu
2. Navigate to this directory:
   ```cmd
   cd "C:\Users\Mohid\lvgl projects\lv_pc_calculator\esp32"
   ```
3. Build:
   ```cmd
   idf.py build
   ```
4. Flash:
   ```cmd
   idf.py flash monitor
   ```

### Display Configuration
- Display: ST7789 (320x240)
- Orientation: Landscape
- SPI Speed: 40 MHz
- Color Order: BGR

### Pin Configuration (ESP32-S3)
Defined in `main/driver.c`:
- SCLK: GPIO 7
- MOSI: GPIO 9
- DC: GPIO 3
- RST: GPIO 2
- CS: GPIO 1
- Backlight: Not used (-1)

## Troubleshooting

If display shows wrong orientation or colors:
- Check rotation settings in `main/driver.c` line 115-116
- Try changing `LCD_RGB_ELEMENT_ORDER_BGR` to `LCD_RGB_ELEMENT_ORDER_RGB`

If build fails:
- Make sure you're using Windows CMD (not Git Bash)
- Clean build directory: `idf.py fullclean`
