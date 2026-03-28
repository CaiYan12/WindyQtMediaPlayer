@echo off
REM ============================================================
REM MediaPlayer Build Script
REM Usage:
REM   build.bat        = Build Debug
REM   build.bat config = CMake Configure
REM   build.bat clean  = Clean build dir
REM   build.bat release = Build Release
REM   build.bat run   = Run program
REM   build.bat full  = Clean + Configure + Build
REM ============================================================

call "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat" -host_arch=x64 -arch=x64 > nul 2>&1

if "%1"=="config" (
    echo [CONFIG] CMake Configure...
    cmake -S "D:\Dev\MediaPlayer" -B "D:\Dev\MediaPlayer\build" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:/Qt/6.10.0/msvc2022_64/lib/cmake"
) else if "%1"=="clean" (
    echo [CLEAN] Removing build directory...
    Remove-Item -Recurse -Force "D:\Dev\MediaPlayer\build"
) else if "%1"=="release" (
    echo [BUILD] Release...
    cmake --build "D:\Dev\MediaPlayer\build" --config Release --parallel
) else if "%1"=="run" (
    echo [RUN] Starting MediaPlayer.exe...
    "D:\Dev\MediaPlayer\build\MediaPlayer.exe"
) else if "%1"=="full" (
    echo [FULL] Clean + Configure + Build...
    Remove-Item -Recurse -Force "D:\Dev\MediaPlayer\build" -ErrorAction SilentlyContinue
    cmake -S "D:\Dev\MediaPlayer" -B "D:\Dev\MediaPlayer\build" -G Ninja -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH="C:/Qt/6.10.0/msvc2022_64/lib/cmake"
    cmake --build "D:\Dev\MediaPlayer\build" --config Debug --parallel
) else (
    echo [BUILD] Debug...
    cmake --build "D:\Dev\MediaPlayer\build" --config Debug --parallel
)