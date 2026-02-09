@echo off
REM Force MinGW + Ninja, ignore Cygwin completely

setlocal EnableExtensions

REM Absolute paths
set MINGW=C:\msys64\mingw64\bin
set CMAKE=C:\msys64\mingw64\bin\cmake.exe
set NINJA=C:\msys64\mingw64\bin\ninja.exe

REM Strip Cygwin by rebuilding PATH locally
set "PATH=%MINGW%;%SystemRoot%\system32;%SystemRoot%;%SystemRoot%\System32\Wbem"

echo ======================================
echo LVGL Calculator - Build and Run
echo ======================================
echo.

cd /d "%~dp0"

echo Building (MinGW + Ninja)...
"%NINJA%" -C "desktop\build"

if errorlevel 1 (
    echo.
    echo Incremental build failed, reconfiguring...

    rmdir /s /q "desktop\build"

    "%CMAKE%" -S "desktop" -B "desktop\build" -G Ninja ^
      -DCMAKE_C_COMPILER=C:/msys64/mingw64/bin/gcc.exe ^
      -DCMAKE_CXX_COMPILER=C:/msys64/mingw64/bin/g++.exe ^
      -DCONFIG_LV_USE_THORVG_INTERNAL=OFF ^
      -DCONFIG_LV_BUILD_DEMOS=OFF ^
      -DCONFIG_LV_BUILD_EXAMPLES=OFF

    if errorlevel 1 (
        echo.
        echo ERROR: Configure failed!
        pause
        exit /b 1
    )

    "%NINJA%" -C "desktop\build" || (
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

".\desktop\bin\main.exe"

echo.
echo Application started!
echo.
