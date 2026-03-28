@echo off
REM ============================================================================
REM MSVC 2022 环境初始化脚本
REM 使用方法：在 VS Code 终端中运行: D:\Dev\MediaPlayer\setup-msvc.bat
REM 或者直接双击此脚本
REM ============================================================================

REM 查找 VC Tools 安装目录
for /f "delims=" %%i in ('"%ProgramFiles%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath') do set VS_PATH=%%i

REM 设置 VC Tools 目录
set VC_TOOLS=%VS_PATH%\VC\Tools\MSVC
if not exist "%VC_TOOLS%" (
    echo [ERROR] Cannot find VC Tools at: %VC_TOOLS%
    exit /b 1
)

REM 找到最新版本的 MSVC
for /f "delims=" %%i in ('dir /b /o-n "%VC_TOOLS%"') do set VC_VERSION=%%i
set VC_BIN=%VC_TOOLS%\%VC_VERSION%\bin\HostX64\x64
set VC_INCLUDE=%VC_TOOLS%\%VC_VERSION%\include
set VC_LIB=%VC_TOOLS%\%VC_VERSION%\lib\x64

REM Windows SDK
set WIN_SDK=C:\Program Files (x86)\Windows Kits\10
for /f "delims=" %%i in ('dir /b /o-n "%WIN_SDK%\bin\10.*" 2^>nul') do set WIN_SDK_BIN=%%i
set WIN_SDK_BIN=%WIN_SDK%\bin\%WIN_SDK_BIN%\x64
set WIN_SDK_INCLUDE=%WIN_SDK%\include\%WIN_SDK_BIN%
set WIN_SDK_LIB=%WIN_SDK%\lib\%WIN_SDK_BIN%\ucrt\x64;%WIN_SDK%\lib\%WIN_SDK_BIN%\um\x64

REM 设置 PATH
set "PATH=%VC_BIN%;%WIN_SDK_BIN%;%PATH%"

REM 设置 Include
set "INCLUDE=%VC_INCLUDE%\shared;%VC_INCLUDE%\ucrt;%VC_INCLUDE%\um;%VC_INCLUDE%\winrt;%VC_INCLUDE%\cppwinrt;%INCLUDE%"

REM 设置 Lib
set "LIB=%VC_LIB%;%WIN_SDK_LIB%;%LIB%"

REM 设置 Windows SDK Platform
set "WindowsSDKVersion=%WIN_SDK_BIN:\bin\10.*=%"

REM 验证
echo [OK] MSVC Environment Loaded
echo     VC_BIN   : %VC_BIN%
echo     WIN_SDK  : %WIN_SDK_BIN%
echo     VC_VER   : %VC_VERSION%
echo.
echo Now run in this terminal:
echo   cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Debug
echo   cmake --build build --parallel
echo.
echo Or just use the Tasks in VS Code: Ctrl+Shift+P ^> Tasks: Run Task

REM 启动 PowerShell 或 cmd（可选）
REM start cmd /k
