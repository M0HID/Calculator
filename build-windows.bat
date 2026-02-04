@echo off
REM Build script for Windows using MSYS2 MinGW64

echo ======================================
echo LVGL Calculator - Windows Build
echo ======================================
echo.

cd /d "%~dp0"

REM Check if MSYS2 is installed
if not exist "C:\msys64\msys2_shell.cmd" (
    echo ERROR: MSYS2 not found at C:\msys64\
    echo Please install MSYS2 from https://www.msys2.org/
    echo.
    pause
    exit /b 1
)

echo Step 1: Configuring and building...
echo.
C:\msys64\msys2_shell.cmd -mingw64 -defterm -here -no-start -c "cd desktop && cmake -B build -G 'MinGW Makefiles' -DCONFIG_LV_USE_THORVG_INTERNAL=OFF -DCONFIG_LV_BUILD_DEMOS=OFF -DCONFIG_LV_BUILD_EXAMPLES=OFF . && cmake --build build -j8"

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    pause
    exit /b 1
)

echo.
echo ======================================
echo Build completed successfully!
echo ======================================
echo.
echo Executable location: desktop\bin\main.exe
echo.
echo Press any key to run the application...
pause > nul

cd desktop
start "" "bin\main.exe"

echo.
echo Application started!
echo.
pause
