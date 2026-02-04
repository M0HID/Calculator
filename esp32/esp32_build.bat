@ECHO OFF

REM Initialise ESP-IDF environment
call "C:\Espressif\idf_cmd_init.bat" esp-idf-29323a3f5a0574597d6dbaa0af20c775

REM Go to your project directory
cd /d "C:\Users\Mohid\lvgl projects\lv_pc_calculator\esp32"

ECHO.
ECHO ============================================
ECHO ESP32 ST7789 Migration Build
ECHO ============================================
ECHO.

REM Run build
idf.py build

ECHO.
ECHO ============================================
ECHO Build Complete!
ECHO ============================================
ECHO.
