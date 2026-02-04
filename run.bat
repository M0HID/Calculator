@echo off
REM Simple build and run script for Windows

echo ======================================
echo LVGL Calculator - Build and Run
echo ======================================
echo.

cd /d "%~dp0"

echo Building...
C:\msys64\mingw64\bin\mingw32-make.exe -C windows/build -j8

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    echo.
    echo Trying full rebuild...
    C:\msys64\msys2_shell.cmd -mingw64 -defterm -here -no-start -c "cd windows && rm -rf build && cmake -B build -G 'MinGW Makefiles' -DCONFIG_LV_USE_THORVG_INTERNAL=OFF -DCONFIG_LV_BUILD_DEMOS=OFF -DCONFIG_LV_BUILD_EXAMPLES=OFF . && cmake --build build -j8"
    
    if errorlevel 1 (
        echo.
        echo ERROR: Full rebuild failed!
        pause
        exit /b 1
    )
)

echo.
echo ======================================
echo Build completed successfully!
echo ======================================
echo.
echo Starting application...
echo.

cd windows
start "" "bin\main.exe"

echo.
echo Application started! Two windows should appear:
echo   1. Calculator (320x240)
echo   2. Button Panel (320x270)
echo.
