#!/bin/bash
# 批量替换硬件操作为HAL函数

cd source

echo "批量替换硬件操作为HAL函数..."

# 替换气泵控制
find . -maxdepth 1 -name "*.c" ! -name "system_manager.c" ! -name "state_machine.c" -type f | while read file; do
    # PUMP = 1 → HAL_Pump_Start()
    sed -i '' 's/PUMP\s*=\s*1/HAL_Pump_Start()/g' "$file"
    # PUMP = 0 → HAL_Pump_Stop()
    sed -i '' 's/PUMP\s*=\s*0/HAL_Pump_Stop()/g' "$file"
    
    # VAL1 = 1 → HAL_Valve1_Open()
    sed -i '' 's/VAL1\s*=\s*1/HAL_Valve1_Open()/g' "$file"
    # VAL1 = 0 → HAL_Valve1_Close()
    sed -i '' 's/VAL1\s*=\s*0/HAL_Valve1_Close()/g' "$file"
    
    # VAL2 = 1 → HAL_Valve2_Open()
    sed -i '' 's/VAL2\s*=\s*1/HAL_Valve2_Open()/g' "$file"
    # VAL2 = 0 → HAL_Valve2_Close()
    sed -i '' 's/VAL2\s*=\s*0/HAL_Valve2_Close()/g' "$file"
    
    # GRE = 1 → HAL_LED_Green_On()
    sed -i '' 's/\bGRE\s*=\s*1\b/HAL_LED_Green_On()/g' "$file"
    # GRE = 0 → HAL_LED_Green_Off()
    sed -i '' 's/\bGRE\s*=\s*0\b/HAL_LED_Green_Off()/g' "$file"
    
    # YEL = 1 → HAL_LED_Yellow_On()
    sed -i '' 's/\bYEL\s*=\s*1\b/HAL_LED_Yellow_On()/g' "$file"
    # YEL = 0 → HAL_LED_Yellow_Off()
    sed -i '' 's/\bYEL\s*=\s*0\b/HAL_LED_Yellow_Off()/g' "$file"
    
    # SPEAK = 1 → HAL_Buzzer_On()
    sed -i '' 's/\bSPEAK\s*=\s*1\b/HAL_Buzzer_On()/g' "$file"
    # SPEAK = 0 → HAL_Buzzer_Off()
    sed -i '' 's/\bSPEAK\s*=\s*0\b/HAL_Buzzer_Off()/g' "$file"
    
    # POWER_ON = 1 → HAL_Power_Hold()
    sed -i '' 's/POWER_ON\s*=\s*1/HAL_Power_Hold()/g' "$file"
    # POWER_ON = 0 → HAL_Power_Release()
    sed -i '' 's/POWER_ON\s*=\s*0/HAL_Power_Release()/g' "$file"
    
    # asm("clrwdt") → HAL_Watchdog_Clear()
    sed -i '' 's/asm\s*(\s*"clrwdt"\s*)/HAL_Watchdog_Clear()/g' "$file"
    
    echo "  ✓ 已处理: $(basename $file)"
done

cd ..
echo "硬件操作替换完成！"

