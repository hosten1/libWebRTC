param(
    [string]$BuildDir = "d:\github\libWebRTC\build"
)

$testConfigs = @(
    @{ Name = "默认配置"; Params = @() },
    @{ Name = "禁用RED"; Params = @("-DWEBRTC_BUILD_RED=OFF") },
    @{ Name = "禁用FEC"; Params = @("-DWEBRTC_BUILD_FEC=OFF") },
    @{ Name = "禁用RED+FEC"; Params = @("-DWEBRTC_BUILD_RED=OFF", "-DWEBRTC_BUILD_FEC=OFF") },
    @{ Name = "启用视频"; Params = @("-DWEBRTC_BUILD_VIDEO=ON") },
    @{ Name = "启用视频+公共视频"; Params = @("-DWEBRTC_BUILD_VIDEO=ON", "-DWEBRTC_BUILD_COMMON_VIDEO=ON") },
    @{ Name = "启用GCC_ONLY"; Params = @("-DWEBRTC_BUILD_GCC_ONLY=ON") },
    @{ Name = "全部启用"; Params = @("-DWEBRTC_BUILD_RED=ON", "-DWEBRTC_BUILD_FEC=ON", "-DWEBRTC_BUILD_VIDEO=ON", "-DWEBRTC_BUILD_COMMON_VIDEO=ON", "-DWEBRTC_BUILD_GCC_ONLY=ON") },
    @{ Name = "最小配置"; Params = @("-DWEBRTC_BUILD_RED=OFF", "-DWEBRTC_BUILD_FEC=OFF", "-DWEBRTC_BUILD_VIDEO=OFF", "-DWEBRTC_BUILD_COMMON_VIDEO=OFF", "-DWEBRTC_BUILD_GCC_ONLY=OFF") }
)

$successCount = 0
$failCount = 0
$results = @()

foreach ($config in $testConfigs) {
    Write-Host "`n=== 测试配置: $($config.Name) ===" -ForegroundColor Cyan
    
    # 清理build目录
    Remove-Item -Path "$BuildDir\*" -Recurse -Force -ErrorAction SilentlyContinue
    
    # 生成项目
    $cmakeCommand = "cmake .. -G `"MinGW Makefiles`" " + ($config.Params -join " ")
    Write-Host "运行: $cmakeCommand"
    Push-Location $BuildDir
    $cmakeOutput = & cmake .. -G "MinGW Makefiles" $config.Params 2>&1
    Pop-Location
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "CMake 生成失败!" -ForegroundColor Red
        Write-Host $cmakeOutput
        $results += @{ Config = $config.Name; Result = "CMake失败"; Output = $cmakeOutput }
        $failCount++
        continue
    }
    
    Write-Host "CMake 生成成功" -ForegroundColor Green
    
    # 编译
    Write-Host "开始编译..."
    Push-Location $BuildDir
    $makeOutput = & make -j8 2>&1
    Pop-Location
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "编译失败!" -ForegroundColor Red
        Write-Host $makeOutput
        $results += @{ Config = $config.Name; Result = "编译失败"; Output = $makeOutput }
        $failCount++
    } else {
        Write-Host "编译成功!" -ForegroundColor Green
        $results += @{ Config = $config.Name; Result = "成功" }
        $successCount++
    }
}

# 输出总结
Write-Host "`n=== 测试总结 ===" -ForegroundColor Cyan
Write-Host "成功: $successCount / $($testConfigs.Count)" -ForegroundColor Green
Write-Host "失败: $failCount / $($testConfigs.Count)" -ForegroundColor Red

foreach ($result in $results) {
    $color = if ($result.Result -eq "成功") { "Green" } else { "Red" }
    Write-Host ("{0,-20} : {1}" -f $result.Config, $result.Result) -ForegroundColor $color
}

return $results
