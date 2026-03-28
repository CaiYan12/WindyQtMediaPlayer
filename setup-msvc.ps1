# MSVC 2022 Environment Setup for MediaPlayer
# Run this first in your VS Code terminal before building

$vspath = & "$env:ProgramFiles\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vspath) {
    Write-Host "[ERROR] Visual Studio 2022 not found" -ForegroundColor Red
    exit 1
}

$vcTools = Join-Path $vspath "VC\Tools\MSVC"
if (-not (Test-Path $vcTools)) {
    Write-Host "[ERROR] VC Tools not found at $vcTools" -ForegroundColor Red
    exit 1
}

# Get latest MSVC version
$vcVersion = (Get-ChildItem $vcTools -Directory | Sort-Object Name -Descending | Select-Object -First 1).Name

$vcBin     = Join-Path $vcTools "$vcVersion\bin\HostX64\x64"
$vcInclude = Join-Path $vcTools "$vcVersion\include"
$vcLib     = Join-Path $vcTools "$vcVersion\lib\x64"

# Find Windows SDK
$winSdkRoot = "C:\Program Files (x86)\Windows Kits\10"
$winSdkBinVersions = Get-ChildItem "$winSdkRoot\bin" -Directory | Where-Object { $_.Name -match '^10\.' } | Sort-Object Name -Descending
$winSdkBin = Join-Path $winSdkRoot "bin\$($winSdkBinVersions[0].Name)\x64"
$winSdkInclude = Join-Path $winSdkRoot "include\$($winSdkBinVersions[0].Name)"
$winSdkLib = Join-Path $winSdkRoot "lib\$($winSdkBinVersions[0].Name)"

# Set environment
$env:PATH = "$vcBin;$winSdkBin;$env:PATH"
$env:INCLUDE = "$vcInclude\shared;$vcInclude\ucrt;$vcInclude\um;$vcInclude\winrt;$env:INCLUDE"
$env:LIB = "$vcLib;$winSdkLib\ucrt\x64;$winSdkLib\um\x64;$env:LIB"

Write-Host "[OK] MSVC 2022 Environment Loaded" -ForegroundColor Green
Write-Host "     VC Version : $vcVersion"
Write-Host "     VC Bin     : $vcBin"
Write-Host "     Windows SDK : $($winSdkBinVersions[0].Name)"
Write-Host ""
Write-Host "Now run these commands in THIS terminal:" -ForegroundColor Yellow
Write-Host "  cmake -S . -B build -G 'Ninja' -DCMAKE_BUILD_TYPE=Debug" -ForegroundColor Cyan
Write-Host "  cmake --build build --parallel" -ForegroundColor Cyan
Write-Host ""
Write-Host "Or use Ctrl+Shift+B in VS Code" -ForegroundColor Gray
