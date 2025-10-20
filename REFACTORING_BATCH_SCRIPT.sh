#!/bin/bash
# 批量重构脚本：给所有C文件添加新架构头文件

FILES=(
    "npwt_con_main.c"
    "npwt_con_over.c"
    "npwt_con_ofile_load_00.c"
    "npwt_dis_sys_ini_00.c"
    "npwt_dis_sys_uart_00.c"
    "npwt_dis_ifile_key_00.c"
    "npwt_dis_ofile_lcd_02.c"
    "npwt_con_ifile_adc.c"
    "adc.c"
    "Flash.c"
    "sys_cpu.c"
    "BIOS_JLX1864G_139.c"
)

for file in "${FILES[@]}"; do
    filepath="source/$file"
    if [ -f "$filepath" ]; then
        echo "处理: $file"
        # 在include.h之后添加新架构头文件（如果还没有）
        if ! grep -q "global_compat.h" "$filepath"; then
            sed -i '' '/^#include.*"include\.h"/a\
#include "system_manager.h"\
#include "global_compat.h"\
#include "hardware_abstraction.h"
' "$filepath"
            echo "  ✓ 已添加新架构头文件"
        else
            echo "  - 已包含新架构头文件，跳过"
        fi
    fi
done

echo "批量处理完成！"

