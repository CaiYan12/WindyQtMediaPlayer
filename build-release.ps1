# MediaPlayer v0.1.0 Release Build Script
param(
    [string]$version = "0.1.0"
)

$ErrorActionPreference = "Stop"
$srcDir = "D:\Dev\MediaPlayer"
$buildDir = "$srcDir\build-release"

# Clean old build
if (Test-Path $buildDir) {
    Remove-Item -Recurse -Force $buildDir
}

# Detect MSVC
$vsWhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
$vspath = & $vsWhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vspath) {
    Write-Host "[ERROR] Visual Studio 2022 not found" -ForegroundColor Red
    exit 1
}

$vcTools = Join-Path $vspath "VC\Tools\MSVC"
$vcVersion = (Get-ChildItem $vcTools -Directory | Sort-Object Name -Descending | Select-Object -First 1).Name
$vcBin = Join-Path $vcTools "$vcVersion\bin\HostX64\x64"

$winSdkBinDir = (Get-ChildItem "C:\Program Files (x86)\Windows Kits\10\bin" -Directory | Where-Object { $_.Name -match "^10\." } | Sort-Object Name -Descending | Select-Object -First 1).FullName
$winSdkBin = Join-Path $winSdkBinDir "x64"

$sdkIncShared = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\shared"
$sdkIncUCRT  = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\ucrt"
$sdkIncUm    = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\um"
$sdkIncWinrt = "C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\winrt"
$sdkLib      = "C:\Program Files (x86)\Windows Kits\10\lib\10.0.22621.0"
$msvcLib     = Join-Path $vcTools "$vcVersion\lib\x64"

$env:PATH = "$vcBin;$winSdkBin;C:\Qt\Tools\Ninja;C:\Qt\Tools\CMake_64\bin;$env:PATH"
$env:LIB = "$msvcLib;$sdkLib\ucrt\x64;$sdkLib\um\x64;$env:LIB"
$env:INCLUDE = "$sdkIncShared;$sdkIncUCRT;$sdkIncUm;$sdkIncWinrt;$env:INCLUDE"
$env:CMAKE_MAKE_PROGRAM = "C:\Qt\Tools\Ninja\ninja.exe"

Write-Host "[RELEASE] MediaPlayer v$version" -ForegroundColor Cyan
Write-Host "[ENV]    MSVC $vcVersion" -ForegroundColor Gray

# Configure
Write-Host "[CONFIG] CMake Configure (Release)..." -ForegroundColor Yellow
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" -S $srcDir -B $buildDir -G Ninja -DCMAKE_BUILD_TYPE=Release -DQt6_DIR="C:/Qt/6.10.0/msvc2022_64/lib/cmake/Qt6"
if ($LASTEXITCODE -ne 0) { Write-Host "[ERROR] Configure failed" -ForegroundColor Red; exit 1 }

# Build
Write-Host "[BUILD]  Building Release..." -ForegroundColor Yellow
& "C:\Qt\Tools\CMake_64\bin\cmake.exe" --build $buildDir --config Release --parallel
if ($LASTEXITCODE -ne 0) { Write-Host "[ERROR] Build failed" -ForegroundColor Red; exit 1 }

# Deploy Qt DLLs (Release = no 'd' suffix)
Write-Host "[DEPLOY] Deploying Qt DLLs..." -ForegroundColor Yellow
& "C:\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe" "$buildDir\MediaPlayer.exe" --no-translations --release
if ($LASTEXITCODE -ne 0) { Write-Host "[WARN] windeployqt had issues (may still work)" -ForegroundColor DarkYellow }

# Package into zip
$zipName = "MediaPlayer-v$version-win64"
$zipDir  = "$srcDir\$zipName"
$outputZip = "$srcDir\$zipName.zip"

if (Test-Path $zipDir) { Remove-Item -Recurse -Force $zipDir }
New-Item -ItemType Directory -Path $zipDir | Out-Null

# Copy exe + all deployed files
Copy-Item "$buildDir\MediaPlayer.exe" $zipDir
Copy-Item "$buildDir\*.dll"         $zipDir -ErrorAction SilentlyContinue
Copy-Item "$buildDir\platforms"     $zipDir -Recurse
Copy-Item "$buildDir\styles"       $zipDir -Recurse
Copy-Item "$buildDir\imageformats" $zipDir -Recurse
Copy-Item "$buildDir\multimedia"    $zipDir -Recurse
Copy-Item "$buildDir\iconengines"  $zipDir -Recurse
Copy-Item "$buildDir\networkinformation" $zipDir -Recurse -ErrorAction SilentlyContinue
Copy-Item "$buildDir\generic"      $zipDir -Recurse -ErrorAction SilentlyContinue
Copy-Item "$buildDir\tls"          $zipDir -Recurse -ErrorAction SilentlyContinue

# Copy a simple README for the release
$readmeTxt = @"
MediaPlayer v$version - Windows x64
=====================================

System Requirements:
  - Windows 10/11 x64
  - No additional runtime required (Qt DLLs bundled)

How to Run:
  1. Extract all files to any folder
  2. Double-click MediaPlayer.exe to run

Features:
  - Audio/Video playback (MP3, WAV, MP4, AVI, MKV, etc.)
  - Playlist management
  - Spectrum visualization
  - Modern QQ影音-style UI

Build Info:
  - Qt Version: 6.10.0
  - Compiler: MSVC 2022 (x64)
  - Build Date: $(Get-Date -Format "yyyy-MM-dd HH:mm")

Project Page: https://github.com/yourusername/MediaPlayer
"@

$readmeTxt | Out-File -FilePath "$zipDir\README.txt" -Encoding UTF8

# Create zip
if (Test-Path $outputZip) { Remove-Item -Force $outputZip }
Compress-Archive -Path $zipDir -DestinationPath $outputZip

Write-Host ""
Write-Host "[DONE] Release v$version built successfully!" -ForegroundColor Green
Write-Host "  Executable : $zipDir\MediaPlayer.exe" -ForegroundColor Gray
Write-Host "  Package    : $outputZip" -ForegroundColor Gray
Write-Host ""
Write-Host "File size:" -ForegroundColor Cyan
Get-ChildItem $outputZip | ForEach-Object { Write-Host "  $($_.Name) - $([math]::Round($_.Length/1MB, 2)) MB" -ForegroundColor White }
