#!/bin/bash
# =============================================================================
# 编译选项测试脚本
# 模拟不同平台编译场景，验证编译选项是否生效且没有遗漏文件
# =============================================================================
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_BASE="${PROJECT_DIR}/build_test"
PASS=0
FAIL=0
TOTAL=0

# 颜色输出
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# 清理
cleanup() {
    rm -rf "$BUILD_BASE"
}

# 运行测试场景
# 参数: 场景名称, 构建目录, 额外 cmake 参数, 断言列表文件
run_scenario() {
    local name="$1"
    local build_dir="$2"
    shift 2
    local extra_args=("$@")

    echo -e "\n${YELLOW}========================================${NC}"
    echo -e "${YELLOW}[测试场景] ${name}${NC}"
    echo -e "${YELLOW}========================================${NC}"

    mkdir -p "$build_dir"

    # 运行 cmake 配置，捕获 stdout+stderr
    local log_file="${build_dir}/cmake_output.log"
    cmake -S "$PROJECT_DIR" -B "$build_dir" \
        "${extra_args[@]}" \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=OFF \
        -DWEBRTC_BUILD_TESTS=OFF \
        2>&1 | tee "$log_file"

    local cmake_exit=${PIPESTATUS[0]}
    if [ $cmake_exit -ne 0 ]; then
        echo -e "${RED}[FAIL] cmake 配置失败 (exit=$cmake_exit)${NC}"
        return 1
    fi
}

# 断言函数: 检查日志中是否包含某行
assert_contains() {
    local log_file="$1"
    local pattern="$2"
    local desc="$3"
    TOTAL=$((TOTAL + 1))

    if grep -qF "$pattern" "$log_file"; then
        echo -e "  ${GREEN}[PASS]${NC} $desc"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}[FAIL]${NC} $desc"
        echo -e "       期望日志包含: $pattern"
        FAIL=$((FAIL + 1))
    fi
}

# 断言函数: 检查日志中不包含某行
assert_not_contains() {
    local log_file="$1"
    local pattern="$2"
    local desc="$3"
    TOTAL=$((TOTAL + 1))

    if grep -qF "$pattern" "$log_file"; then
        echo -e "  ${RED}[FAIL]${NC} $desc"
        echo -e "       期望日志不包含: $pattern"
        FAIL=$((FAIL + 1))
    else
        echo -e "  ${GREEN}[PASS]${NC} $desc"
        PASS=$((PASS + 1))
    fi
}

# 断言函数: 检查诊断日志中的源文件计数在合理范围
assert_source_count() {
    local log_file="$1"
    local module="$2"
    local var_name="$3"
    local min="$4"
    local desc="$5"
    TOTAL=$((TOTAL + 1))

    local count=$(grep "\[$module\]" "$log_file" | grep "$var_name" | sed "s/.*$var_name//" | sed 's/^[^0-9]*//;s/[^0-9].*$//' | head -1)
    if [ -n "$count" ] && [ "$count" -ge "$min" ] 2>/dev/null; then
        echo -e "  ${GREEN}[PASS]${NC} $desc (count=$count >= $min)"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}[FAIL]${NC} $desc (count=$count, expected >= $min)"
        FAIL=$((FAIL + 1))
    fi
}

# =============================================================================
# 测试场景 1: 默认选项（全部 ON）
# =============================================================================
scenario_default() {
    local build_dir="${BUILD_BASE}/default"
    run_scenario "默认编译 (全部 ON)" "$build_dir"
    local log="${build_dir}/cmake_output.log"

    echo "  --- 验证 STUN 选项 ---"
    assert_contains "$log" "[rtc_base] STUN: enabled - adding STUN sources" "STUN: rtc_base 启用"
    assert_contains "$log" "[api] STUN: enabled - adding stun.cc" "STUN: api 启用"

    echo "  --- 验证 audio_util.cc 已从 STUN 移到 AUDIO_TOOL ---"
    assert_not_contains "$log" "[common_audio] STUN: enabled - adding audio_util.cc" "audio_util.cc 不再属于 STUN"
    assert_contains "$log" "[common_audio] AUDIO_TOOL: enabled - adding audio_util.cc" "audio_util.cc 已归入 AUDIO_TOOL"

    echo "  --- 验证 AUDIO_TOOL ---"
    assert_contains "$log" "[api] Audio tool: enabled" "AUDIO_TOOL: api 启用"
    assert_contains "$log" "[common_audio] AUDIO_TOOL: enabled - adding audio_util.cc" "AUDIO_TOOL: common_audio 启用"

    echo "  --- 验证 AUDIO_SIGNAL_PROCESSING ---"
    assert_contains "$log" "[libwebrtc] Building audio signal processing library..." "AUDIO_SIGNAL_PROCESSING: 启用"

    echo "  --- 验证其他模块 ---"
    assert_contains "$log" "[libwebrtc] Adding WebRTC threading modules..." "THREAD: 启用"
    assert_contains "$log" "[libwebrtc] Adding WebRTC logging modules..." "LOGGING: 启用"
    assert_contains "$log" "[libwebrtc] Adding RED modules" "RED: 启用"
    assert_contains "$log" "[libwebrtc] Adding FEC modules" "FEC: 启用"
    assert_contains "$log" "[libwebrtc] Adding video modules" "VIDEO: 启用"
    assert_contains "$log" "[libwebrtc] Adding GCC modules" "GCC: 启用"

    echo "  --- 验证源文件计数 ---"
    assert_source_count "$log" "rtc_base" "Total source files: " 20 "rtc_base 源文件 >= 20"
    assert_source_count "$log" "modules" "Total source files: " 20 "modules 源文件 >= 20"
    assert_source_count "$log" "common_audio" "Module sources: " 20 "common_audio 源文件 >= 20 (含 signal_processing 库)"
}

# =============================================================================
# 测试场景 2: STUN=OFF
# =============================================================================
scenario_stun_off() {
    local build_dir="${BUILD_BASE}/stun_off"
    run_scenario "STUN 关闭 (WEBRTC_BUILD_WEBRTC_STUN=OFF)" "$build_dir" \
        -DWEBRTC_BUILD_WEBRTC_STUN=OFF
    local log="${build_dir}/cmake_output.log"

    echo "  --- 验证 STUN 已关闭 ---"
    assert_contains "$log" "[rtc_base] STUN: disabled" "STUN: rtc_base 已禁用"
    assert_contains "$log" "[api] STUN: disabled" "STUN: api 已禁用"

    echo "  --- 验证 audio_util.cc 仍被编译（不受 STUN 影响）---"
    assert_contains "$log" "[common_audio] AUDIO_TOOL: enabled - adding audio_util.cc" "audio_util.cc 仍通过 AUDIO_TOOL 编译"

    echo "  --- 验证其他模块不受影响 ---"
    assert_contains "$log" "[libwebrtc] Building audio signal processing library..." "AUDIO_SIGNAL_PROCESSING: 正常"
    assert_contains "$log" "[libwebrtc] Adding WebRTC threading modules..." "THREAD: 正常"
    assert_contains "$log" "[libwebrtc] Adding RED modules" "RED: 正常"
    assert_contains "$log" "[libwebrtc] Adding video modules" "VIDEO: 正常"
}

# =============================================================================
# 测试场景 3: AUDIO_TOOL=OFF
# =============================================================================
scenario_audio_tool_off() {
    local build_dir="${BUILD_BASE}/audio_tool_off"
    run_scenario "AUDIO_TOOL 关闭 (WEBRTC_BUILD_WEBRTC_AUDIO_TOOL=OFF)" "$build_dir" \
        -DWEBRTC_BUILD_WEBRTC_AUDIO_TOOL=OFF
    local log="${build_dir}/cmake_output.log"

    echo "  --- 验证 AUDIO_TOOL 已关闭 ---"
    assert_contains "$log" "[api] Audio tool: disabled" "AUDIO_TOOL: api 已禁用"
    assert_contains "$log" "[common_audio] AUDIO_TOOL: disabled" "AUDIO_TOOL: common_audio 已禁用"

    echo "  --- 验证 audio_util.cc 不编译 ---"
    assert_not_contains "$log" "audio_util.cc" "audio_util.cc 未被编译"

    echo "  --- 验证 STUN 不受影响 ---"
    assert_contains "$log" "[rtc_base] STUN: enabled - adding STUN sources" "STUN: rtc_base 正常"
    assert_contains "$log" "[api] STUN: enabled - adding stun.cc" "STUN: api 正常"

    echo "  --- 验证音频算法不受影响 ---"
    assert_contains "$log" "[libwebrtc] Building audio signal processing library..." "AUDIO_SIGNAL_PROCESSING: 正常"
}

# =============================================================================
# 测试场景 4: AUDIO_SIGNAL_PROCESSING=OFF
# =============================================================================
scenario_sigproc_off() {
    local build_dir="${BUILD_BASE}/sigproc_off"
    run_scenario "音频算法关闭 (WEBRTC_BUILD_AUDIO_SIGNAL_PROCESSING=OFF)" "$build_dir" \
        -DWEBRTC_BUILD_AUDIO_SIGNAL_PROCESSING=OFF
    local log="${build_dir}/cmake_output.log"

    echo "  --- 验证 AUDIO_SIGNAL_PROCESSING 已关闭 ---"
    assert_contains "$log" "[libwebrtc] Signal processing library disabled" "AUDIO_SIGNAL_PROCESSING: 已禁用"

    echo "  --- 验证其他音频模块不受影响 ---"
    assert_contains "$log" "[common_audio] AUDIO_TOOL: enabled - adding audio_util.cc" "AUDIO_TOOL: 正常"
    assert_contains "$log" "[api] Audio tool: enabled" "AUDIO_TOOL: api 正常"

    echo "  --- 验证 STUN 不受影响 ---"
    assert_contains "$log" "[rtc_base] STUN: enabled - adding STUN sources" "STUN: 正常"
}

# =============================================================================
# 测试场景 5: 多选项关闭组合
# =============================================================================
scenario_multi_off() {
    local build_dir="${BUILD_BASE}/multi_off"
    run_scenario "多选项关闭 (STUN=OFF + AUDIO_TOOL=OFF + AUDIO_SIG=OFF)" "$build_dir" \
        -DWEBRTC_BUILD_WEBRTC_STUN=OFF \
        -DWEBRTC_BUILD_WEBRTC_AUDIO_TOOL=OFF \
        -DWEBRTC_BUILD_AUDIO_SIGNAL_PROCESSING=OFF
    local log="${build_dir}/cmake_output.log"

    echo "  --- 验证所有三个选项均已关闭 ---"
    assert_contains "$log" "[rtc_base] STUN: disabled" "STUN: 已禁用"
    assert_contains "$log" "[api] STUN: disabled" "STUN: api 已禁用"
    assert_contains "$log" "[common_audio] AUDIO_TOOL: disabled" "AUDIO_TOOL: 已禁用"
    assert_contains "$log" "[libwebrtc] Signal processing library disabled" "AUDIO_SIGNAL_PROCESSING: 已禁用"

    echo "  --- 验证没有 audio_util.cc ---"
    assert_not_contains "$log" "audio_util.cc" "audio_util.cc 未被编译"

    echo "  --- 验证核心模块仍正常工作 ---"
    assert_contains "$log" "[libwebrtc] Adding WebRTC threading modules..." "THREAD: 正常"
    assert_contains "$log" "[libwebrtc] Adding RED modules" "RED: 正常"
}

# =============================================================================
# 测试场景 6: 仅音频模块
# =============================================================================
scenario_audio_only() {
    local build_dir="${BUILD_BASE}/audio_only"
    run_scenario "仅音频模块 (关闭 THREAD LOGGING RED FEC VIDEO GCC)" "$build_dir" \
        -DWEBRTC_BUILD_WEBRTC_THREAD=OFF \
        -DWEBRTC_BUILD_WEBRTC_LOGGING=OFF \
        -DWEBRTC_BUILD_RED=OFF \
        -DWEBRTC_BUILD_FEC=OFF \
        -DWEBRTC_BUILD_VIDEO=OFF \
        -DWEBRTC_BUILD_COMMON_VIDEO=OFF \
        -DWEBRTC_BUILD_GCC=OFF
    local log="${build_dir}/cmake_output.log"

    echo "  --- 验证音频模块仍启用 ---"
    assert_contains "$log" "[common_audio] AUDIO_TOOL: enabled - adding audio_util.cc" "AUDIO_TOOL: 启用"
    assert_contains "$log" "[libwebrtc] Building audio signal processing library..." "AUDIO_SIGNAL_PROCESSING: 启用"

    echo "  --- 验证其他模块已关闭 ---"
    assert_contains "$log" "[libwebrtc] WebRTC threading modules disabled" "THREAD: 已禁用"
    assert_contains "$log" "[libwebrtc] WebRTC logging modules disabled" "LOGGING: 已禁用"
    assert_contains "$log" "[libwebrtc] RED modules disabled" "RED: 已禁用"
    assert_contains "$log" "[libwebrtc] FEC modules disabled" "FEC: 已禁用"
    assert_contains "$log" "[libwebrtc] Video modules disabled" "VIDEO: 已禁用"
    assert_contains "$log" "[libwebrtc] GCC modules disabled" "GCC: 已禁用"

    echo "  --- 验证源文件计数 ---"
    assert_source_count "$log" "common_audio" "Module sources: " 20 "common_audio >= 20 (音频算法 + 工具)"
}

# =============================================================================
# 主流程
# =============================================================================
echo "============================================"
echo " WebRTC 编译选项测试脚本"
echo " 项目路径: $PROJECT_DIR"
echo " 构建基础: $BUILD_BASE"
echo "============================================"

cleanup

# 运行所有测试场景
scenario_default
scenario_stun_off
scenario_audio_tool_off
scenario_sigproc_off
scenario_multi_off
scenario_audio_only

# 汇总
echo -e "\n============================================"
echo -e " 测试结果汇总"
echo -e "============================================"
echo -e " 总计: $TOTAL  通过: ${GREEN}$PASS${NC}  失败: ${RED}$FAIL${NC}"

if [ "$FAIL" -eq 0 ]; then
    echo -e "${GREEN}所有测试通过！${NC}"
else
    echo -e "${RED}存在 $FAIL 个测试失败，请检查日志。${NC}"
fi

# 清理构建目录
echo -e "\n清理临时构建目录..."
cleanup

exit $FAIL