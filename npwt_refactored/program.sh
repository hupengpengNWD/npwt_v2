#!/bin/bash
###############################################################################
# PICkit3 下载脚本
# 功能：编译并下载固件到 PIC18F46J11
###############################################################################

# 颜色定义
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# MPLAB IPE 命令行工具路径
IPECMD="/Applications/microchip/mplabx/v5.40/mplab_platform/mplab_ipe/bin/ipecmd.sh"

# 获取脚本所在目录的绝对路径
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# 固件文件路径（使用绝对路径）
HEX_FILE="${SCRIPT_DIR}/NWPT_VR.X/dist/default/production/NWPT_VR.X.production.hex"

echo ""
echo "========================================"
echo "  NPWT 固件下载工具"
echo "========================================"
echo ""

# 步骤1：编译
echo -e "${YELLOW}🔨 步骤1：编译固件...${NC}"
cd "${SCRIPT_DIR}"
make -C NWPT_VR.X clean > /dev/null 2>&1
make -C NWPT_VR.X

if [ $? -ne 0 ]; then
    echo -e "${RED}❌ 编译失败！请检查代码错误。${NC}"
    exit 1
fi

echo -e "${GREEN}✅ 编译成功！${NC}"
echo ""

# 检查固件文件是否存在
if [ ! -f "$HEX_FILE" ]; then
    echo -e "${RED}❌ 找不到固件文件：$HEX_FILE${NC}"
    exit 1
fi

# 步骤2：下载到 PICkit3
echo -e "${YELLOW}📥 步骤2：下载到 PICkit3...${NC}"
echo "   目标芯片: PIC18F46J11"
echo "   调试器: PICkit3"
echo "   固件文件: $HEX_FILE"
echo ""

$IPECMD \
  -P18F46J11 \
  -TPPK3 \
  -F$HEX_FILE \
  -M \
  -OL

if [ $? -eq 0 ]; then
    echo ""
    echo -e "${GREEN}✅ 下载成功！设备已编程。${NC}"
    echo ""
    
    # 显示内存使用情况
    echo "📊 内存使用统计："
    cd "${SCRIPT_DIR}" && make -C NWPT_VR.X 2>&1 | grep -A 3 "Memory Summary"
else
    echo ""
    echo -e "${RED}❌ 命令行下载失败！${NC}"
    echo ""
    echo -e "${YELLOW}⚠️  ipecmd 工具可能无法连接目标板${NC}"
    echo ""
    echo "📌 建议使用 MPLAB X IDE 下载："
    echo ""
    echo "  1. 打开 MPLAB X IDE v5.40"
    echo "  2. File → Open Project → 选择:"
    echo "     ${SCRIPT_DIR}/NWPT_VR.X"
    echo "  3. 点击 'Make and Program Device' 按钮（绿色箭头）"
    echo ""
    echo "🔍 如果在 IDE 中也失败，请检查："
    echo "  • ICSP 6线连接（特别是 Pin 2 VDD 和 Pin 3 GND）"
    echo "  • 目标板供电（3.0V ~ 3.6V）"
    echo "  • MCLR 引脚上拉电阻（10K）"
    echo "  • RB6/RB7 引脚无外部干扰"
    echo ""
    exit 1
fi

echo ""
echo "========================================"
echo "  完成！"
echo "========================================"

