<#
.SYNOPSIS
编译 libwebrtc 为 Android 平台的静态库

.DESCRIPTION
本脚本用于在 Windows 上使用 Android NDK 交叉编译 libwebrtc 为 Android 静态库。

.PARAMETER Arch
目标架构，可选值: arm64-v8a, armeabi-v7a, x86, x86_64，默认 arm64-v8a

.PARAMETER ApiLevel
Android API 级别，默认 24

.PARAMETER BuildDir
构建目录，默认 build-android

.PARAMETER Clean
是否清理旧的构建目录，默认 true

.EXAMPLE
.\build_android.ps1
编译 arm64-v8a 架构

.EXAMPLE
.\build_android.ps1 -Arch armeabi-v7a
编译 armeabi-v7a 架构

.EXAMPLE
.\build_android.ps1 -Arch arm64-v8a -ApiLevel 28
编译 arm64-v8a 架构，API 级别 28
#>

param(
    [string]$Arch = "arm64-v8a",
    [int]$ApiLevel = 24,
    [string]$BuildDir = "build-android",
    [bool]$Clean = $true
)

# 颜色定义
$Cyan = "Cyan"
$Green = "Green"
$Yellow = "Yellow"
$Red = "Red"

Write-Host "`n=== libwebrtc Android 编译脚本 ===" -ForegroundColor $Cyan
Write-Host "架构: $Arch"
Write-Host "API 级别: $ApiLevel"
Write-Host "构建目录: $BuildDir`n"

# 1. 查找 Android NDK
Write-Host "[1/5] 查找 Android NDK..." -ForegroundColor $Cyan
$ndkPaths = @(
    "$env:LOCALAPPDATA\Android\Sdk\ndk",
    "$env:USERPROFILE\AppData\Local\Android\Sdk\ndk",
    "C:\Android\Sdk\ndk",
    "C:\Program Files (x86)\Android\android-sdk\ndk"
)

$ndkPath = $null
foreach ($basePath in $ndkPaths) {
    if (Test-Path $basePath) {
        $ndkVersions = Get-ChildItem $basePath -Directory | Sort-Object Name -Descending
        if ($ndkVersions) {
            $ndkPath = $ndkVersions[0].FullName
            break
        }
    }
}

if (-not $ndkPath) {
    Write-Host "错误: 未找到 Android NDK!" -ForegroundColor $Red
    Write-Host "请通过 Android Studio 安装 NDK 或设置 ANDROID_NDK 环境变量" -ForegroundColor $Yellow
    exit 1
}

$ndkPath = $ndkPath -replace "\\", "/"
Write-Host "找到 NDK: $ndkPath`n" -ForegroundColor $Green

# 2. 设置环境变量
Write-Host "[2/5] 设置环境变量..." -ForegroundColor $Cyan
$env:ANDROID_NDK = $ndkPath
Write-Host "ANDROID_NDK = $env:ANDROID_NDK`n" -ForegroundColor $Green

# 3. 清理旧目录
Write-Host "[3/5] 清理构建目录..." -ForegroundColor $Cyan
$buildPath = Join-Path $PWD $BuildDir
if (Test-Path $buildPath -and $Clean) {
    Write-Host "清理旧的构建目录: $buildPath"
    Remove-Item $buildPath -Recurse -Force -ErrorAction SilentlyContinue
}

if (-not Test-Path $buildPath) {
    New-Item -ItemType Directory -Path $buildPath -Force | Out-Null
}
Write-Host "构建目录: $buildPath`n" -ForegroundColor $Green

# 4. 配置 CMake
Write-Host "[4/5] 配置 CMake..." -ForegroundColor $Cyan
Set-Location $buildPath

$cmakeCmd = @"
cmake .. -G "Unix Makefiles" `
    -DCMAKE_SYSTEM_NAME=Android `
    -DCMAKE_SYSTEM_VERSION=$ApiLevel `
    -DCMAKE_ANDROID_ARCH_ABI=$Arch `
    -DCMAKE_ANDROID_NDK="$ndkPath" `
    -DCMAKE_ANDROID_STL_TYPE=c++_static `
    -DCMAKE_BUILD_TYPE=Release `
    -DWEBRTC_BUILD_RED=OFF `
    -DWEBRTC_BUILD_FEC=OFF `
    -DWEBRTC_BUILD_GCC=OFF `
    -DWEBRTC_BUILD_WEBRTC_LOGGING=OFF
"@

Write-Host "运行: $cmakeCmd`n"
Invoke-Expression $cmakeCmd

if ($LASTEXITCODE -ne 0) {
    Write-Host "`n错误: CMake 配置失败!" -ForegroundColor $Red
    Set-Location ..
    exit 1
}
Write-Host "`nCMake 配置成功!`n" -ForegroundColor $Green

# 5. 编译
Write-Host "[5/5] 开始编译..." -ForegroundColor $Cyan
$startTime = Get-Date
Write-Host "编译开始时间: $startTime`n"

cmake --build . --config Release -j8

if ($LASTEXITCODE -ne 0) {
    Write-Host "`n错误: 编译失败!" -ForegroundColor $Red
    Set-Location ..
    exit 1
}

$endTime = Get-Date
$duration = $endTime - $startTime
Write-Host "`n编译完成!" -ForegroundColor $Green
Write-Host "编译耗时: $($duration.Hours)小时 $($duration.Minutes)分钟 $($duration.Seconds)秒`n"

# 6. 检查输出
Write-Host "=== 检查输出文件 ===" -ForegroundColor $Cyan
$libPath = Join-Path $buildPath "libwebrtc.a"
if (Test-Path $libPath) {
    $libSize = [math]::Round((Get-Item $libPath).Length / 1MB, 2)
    Write-Host "✓ libwebrtc.a 已生成: $libPath" -ForegroundColor $Green
    Write-Host "  大小: ${libSize} MB" -ForegroundColor $Green
} else {
    # 可能在 Release 目录
    $libPath = Join-Path $buildPath "Release/libwebrtc.a"
    if (Test-Path $libPath) {
        $libSize = [math]::Round((Get-Item $libPath).Length / 1MB, 2)
        Write-Host "✓ libwebrtc.a 已生成: $libPath" -ForegroundColor $Green
        Write-Host "  大小: ${libSize} MB" -ForegroundColor $Green
    } else {
        Write-Host "✗ 未找到 libwebrtc.a" -ForegroundColor $Red
    }
}

# 回到原目录
Set-Location ..

Write-Host "`n=== 编译脚本完成 ===" -ForegroundColor $Cyan
