#!/bin/bash
###############################################################################
# PICkit3 连接诊断脚本
###############################################################################

IPECMD="/Applications/microchip/mplabx/v5.40/mplab_platform/mplab_ipe/bin/ipecmd.sh"

echo "========================================"
echo "  PICkit3 连接诊断"
echo "========================================"
echo ""

echo "测试1: 检测 PICkit3 硬件..."
$IPECMD -TPPK3 -OL
echo ""

echo "测试2: 尝试读取目标电压..."
$IPECMD -P18F46J11 -TPPK3 -OL
echo ""

echo "测试3: 尝试连接（不下载）..."
$IPECMD -P18F46J11 -TPPK3 -M 2>&1 | tail -20
echo ""

echo "========================================"
echo "  建议"
echo "========================================"
echo ""
echo "如果以上测试都失败，请："
echo "1. 检查 ICSP 6线连接（特别是 VDD 和 GND）"
echo "2. 确认目标板供电正常（3.3V 或 5V）"
echo "3. 确认 MCLR 引脚有上拉电阻"
echo "4. 尝试在 MPLAB X IDE 中下载"
echo ""
echo "ICSP 接线参考："
echo "  PICkit3 Pin 1 (MCLR) → PIC Pin 1  (MCLR)"
echo "  PICkit3 Pin 2 (VDD)  → PIC Pin 11 (VDD)"
echo "  PICkit3 Pin 3 (GND)  → PIC Pin 12 (GND)"
echo "  PICkit3 Pin 4 (PGD)  → PIC Pin 40 (RB7)"
echo "  PICkit3 Pin 5 (PGC)  → PIC Pin 39 (RB6)"
echo ""

