# MediaPlayer Build Script for VS Code
# Properly initializes MSVC + runs cmake build commands

# vswhere.exe is in the x86 Program Files directory
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vspath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath

if (-not $vspath) {
    Write-Host "[ERROR] Visual Studio 2022 not found" -ForegroundColor Red
    exit 1
}

# Load MSVC environment
$vcTools = Join-Path $vspath "VC\Tools\MSVC"
if (Test-Path $vcTools) {
    $vcVersion = (Get-ChildItem $vcTools -Directory | Sort-Object Name -Descending | Select-Object -First 1).Name
    $vcBin = Join-Path $vcTools "$vcVersion\bin\HostX64\x64"

    $winSdkRoot = "C:\Program Files (x86)\Windows Kits\10"
    $winSdkBinDir = (Get-ChildItem "$winSdkRoot\bin" -Directory | Where-Object { $_.Name -match '^10\.' } | Sort-Object Name -Descending | Select-Object -First 1).FullName
    $winSdkBin = Join-Path $winSdkBinDir "x64"

    $env:PATH = "$vcBin;$winSdkBin;C:\Qt\Tools\Ninja;C:\Qt\Tools\CMake_64\bin;$env:PATH"
    $sdkInc = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0"
    $env:INCLUDE = "$sdkInc\shared;$sdkInc\ucrt;$sdkInc\um;$env:INCLUDE"
    $sdkLib = "C:\Program Files (x86)\Windows Kits\10\lib\10.0.22621.0"
    $msvcLib = Join-Path $vcTools "$vcVersion\lib\x64"
    $env:LIB = "$msvcLib;$sdkLib\ucrt\x64;$sdkLib\um\x64;$env:LIB"
    Write-Host "[ENV] MSVC $vcVersion loaded" -ForegroundColor Green
    # Tell CMake where Ninja is (Ninja generator needs this explicitly)
    $env:CMAKE_MAKE_PROGRAM = "C:\Qt\Tools\Ninja\ninja.exe"
}

$cmake = "C:\Qt\Tools\CMake_64\bin\cmake.exe"
$windeployqt = "C:\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe"
$buildDir = "D:\Dev\MediaPlayer\build"
$srcDir = "D:\Dev\MediaPlayer"
$qtPrefix = "C:/Qt/6.10.0/msvc2022_64/lib/cmake"

switch ($args[0]) {
    "config" {
        Write-Host "[CONFIG] CMake Configure..."
        & $cmake -S $srcDir -B $buildDir -G Ninja `
            -DCMAKE_BUILD_TYPE=Debug `
            -DQt6_DIR="C:/Qt/6.10.0/msvc2022_64/lib/cmake/Qt6"
    }
    "clean" {
        Write-Host "[CLEAN] Removing build directory..."
        Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
        Write-Host "[CLEAN] Done" -ForegroundColor Green
    }
    "release" {
        Write-Host "[BUILD] Release..."
        & $cmake --build $buildDir --config Release --parallel
    }
    "run" {
        Write-Host "[RUN] Starting MediaPlayer.exe..."
        & "$buildDir\MediaPlayer.exe"
    }
    "full" {
        Write-Host "[FULL] Clean + Configure + Build..."
        Remove-Item -Recurse -Force $buildDir -ErrorAction SilentlyContinue
        & $cmake -S $srcDir -B $buildDir -G Ninja `
            -DCMAKE_BUILD_TYPE=Debug `
            -DQt6_DIR="C:/Qt/6.10.0/msvc2022_64/lib/cmake/Qt6"
        & $cmake --build $buildDir --config Debug --parallel
    }
    default {
        Write-Host "[BUILD] Debug..."
        & $cmake --build $buildDir --config Debug --parallel
        # Deploy Qt DLLs so the exe can run standalone
        & $windeployqt "$buildDir\MediaPlayer.exe" --no-translations
        Write-Host "[DEPLOY] Qt DLLs deployed" -ForegroundColor Green
    }
}
