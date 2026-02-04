@echo off
echo Starting LVGL Calculator...
echo.
cd /d "%~dp0\windows"
start "" "bin\main.exe"
echo.
echo Application started!
echo Two windows should appear:
echo   1. Calculator (320x240)
echo   2. Button Panel (320x270)
echo.
echo If you don't see the windows, check your taskbar.
echo.
pause
