/****************************************************************************
 * 文件名: lcd_driver.c
 * 功能: LCD显示驱动实现（完全重构版本）
 * 
 * 说明: 
 *   基于JLX1864G LCD控制器
 *   128x64点阵LCD
 *   不依赖旧BIOS，完全重新实现
 * 
 * 创建日期: 2025-10-21
 ****************************************************************************/

#include "../Inc/lcd_driver.h"
#include "../Inc/lcd_images.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Core/Inc/system_config.h"
#include "../../Core/Inc/system_enums.h"
#include "../../Core/Inc/mcu_config.h"
#include <string.h>

/****************************************************************************
 * LCD硬件控制引脚定义
 ****************************************************************************/
#define LCD_CS   LATCbits.LATC0
#define LCD_RS   LATCbits.LATC1  
#define LCD_RD   LATCbits.LATC2
#define LCD_WR   LATCbits.LATC3
#define LCD_RES  LATCbits.LATC4
#define LCD_DATA LATD

/****************************************************************************
 * LCD命令定义
 ****************************************************************************/
#define JLX12864G_RES   0xE2    // 复位
#define JLX12864G_ON    0xAF    // 开显示
#define JLX12864G_OFF   0xAE    // 关显示

/****************************************************************************
 * 外部字模数组（从旧代码移植）
 ****************************************************************************/
extern const unsigned char arry_dig[];      // 数字字模 12x12
extern const unsigned char arry_dig22[];    // 大数字字模 32x15
extern const unsigned char arry_char[];     // 字符字模 6x12

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/
static void LCD_Delay(uint16_t ms);
static void LCD_SendCommand(uint8_t cmd);
static void LCD_SendData(uint8_t data);
static void LCD_SetPosition(uint8_t page, uint8_t column);

/****************************************************************************
 * 旧BIOS兼容函数（用于编译通过，实际可能未使用）
 ****************************************************************************/
void BIOS_JLX12864_TRANS_CMD(int data) {
    LCD_SendCommand((uint8_t)data);
}

void BIOS_JLX12864_TRANS_DAT(int data) {
    LCD_SendData((uint8_t)data);
}

/****************************************************************************
 * 底层硬件操作函数
 ****************************************************************************/

/**
 * 延时函数
 */
static void LCD_Delay(uint16_t ms) {
    for (uint16_t i = 0; i < ms; i++) {
        for (uint16_t j = 0; j < 100; j++) {
            __asm("nop");
        }
    }
}

/**
 * 发送命令到LCD
 */
static void LCD_SendCommand(uint8_t cmd) {
    LCD_CS = 0;
    LCD_RS = 0;
    LCD_RD = 1;
    LCD_WR = 0;
    LCD_DATA = cmd;
    LCD_RD = 0;
    LCD_CS = 1;
}

/**
 * 发送数据到LCD
 */
static void LCD_SendData(uint8_t data) {
    LCD_CS = 0;
    LCD_RS = 1;
    LCD_RD = 1;
    LCD_WR = 0;
    LCD_DATA = data;
    LCD_CS = 1;
    LCD_RD = 0;
}

/**
 * 设置显示位置
 * @param page: 页地址 0-7
 * @param column: 列地址 0-127
 */
static void LCD_SetPosition(uint8_t page, uint8_t column) {
    LCD_SendCommand(0xB0 + page);              // 设置页地址
    LCD_SendCommand(0x10 + (column >> 4));     // 设置列地址高4位
    LCD_SendCommand(0x00 + (column & 0x0F));   // 设置列地址低4位
}

/****************************************************************************
 * 公共API函数实现
 ****************************************************************************/

/**
 * 函数: LCD_Driver_Init
 * 功能: 初始化LCD硬件
 */
void LCD_Driver_Init(void) {
    // 复位LCD
    LCD_RES = 0;
    LCD_Delay(200);
    LCD_RES = 1;
    LCD_Delay(200);
    
    // 初始化命令序列
    LCD_SendCommand(JLX12864G_RES);    // 复位
    LCD_Delay(50);
    
    LCD_SendCommand(0xA2);             // 1/9偏压比
    LCD_SendCommand(0xA1);             // SEG方向
    LCD_SendCommand(0xC0);             // COM方向
    
    // 电源控制
    LCD_SendCommand(0x2C);
    LCD_Delay(5);
    LCD_SendCommand(0x2E);
    LCD_Delay(5);
    LCD_SendCommand(0x2F);
    LCD_Delay(50);
    
    // 对比度设置
    LCD_SendCommand(0x25);
    LCD_SendCommand(0x81);
    LCD_SendCommand(0x0C);
    LCD_Delay(10);
    
    // 其他设置
    LCD_SendCommand(0xAC);             // 静态显示
    LCD_SendCommand(0x00);
    LCD_SendCommand(0x40);             // 起始行
    
    LCD_SendCommand(JLX12864G_ON);     // 开显示
    
    // 初始化后清屏
    LCD_Clear();
}

/**
 * 函数: LCD_Clear
 * 功能: 清屏
 */
void LCD_Clear(void) {
    for (uint8_t page = 0; page < 8; page++) {
        LCD_SetPosition(page, 0);
        for (uint8_t col = 0; col < 128; col++) {
            LCD_SendData(0x00);
        }
    }
}


/**
 * 函数: LCD_DisplayNumber
 * 功能: 显示数字（与头文件声明匹配）
 */
void LCD_DisplayNumber(uint8_t page, uint8_t column, uint16_t number) {
    // 将数字拆分成各位显示
    if (number > 9999) number = 9999;
    
    uint8_t digits[5];
    uint8_t count = 0;
    
    if (number == 0) {
        digits[0] = 0;
        count = 1;
    } else {
        uint16_t temp = number;
        while (temp > 0) {
            digits[count++] = temp % 10;
            temp /= 10;
        }
    }
    
    // 从最高位开始显示
    uint8_t current_col = column;
    for (int8_t i = count - 1; i >= 0; i--) {
        uint16_t offset = digits[i] * 12;
        
        // 第一行
        LCD_SetPosition(page, current_col);
        for (uint8_t j = 0; j < 6; j++) {
            uint16_t data = (uint16_t)arry_dig[offset + j] | ((uint16_t)arry_dig[offset + j + 6] << 8);
            data = (data << 3) & 0xFF;
            LCD_SendData((uint8_t)data);
        }
        
        // 第二行
        LCD_SetPosition(page + 1, current_col);
        for (uint8_t j = 0; j < 6; j++) {
            uint16_t data = ((uint16_t)arry_dig[offset + j + 6] << 8) | (uint16_t)arry_dig[offset + j];
            data = (data << 3) >> 8;
            LCD_SendData((uint8_t)data);
        }
        
        current_col += 6;
    }
}

/**
 * 函数: LCD_DisplayString
 * 功能: 显示字符串（支持数字和大写字母）
 */
void LCD_DisplayString(uint8_t page, uint8_t column, const char *str) {
    uint8_t current_col = column;
    
    while (*str) {
        uint8_t ch = *str;
        
        if (ch >= '0' && ch <= '9') {
            // 显示数字
            LCD_DisplayNumber(page, current_col, ch - '0');
            current_col += 6;
        } else if (ch >= 'A' && ch <= 'Z') {
            // 显示大写字母
            uint16_t offset = (ch - 'A') * 12;
            
            // 第一行
            LCD_SetPosition(page, current_col);
            for (uint8_t i = 0; i < 6; i++) {
                uint16_t data = (uint16_t)arry_char[offset + i] | ((uint16_t)arry_char[offset + i + 6] << 8);
                data = (data >> 3) & 0xFF;
                LCD_SendData((uint8_t)data);
            }
            
            // 第二行
            LCD_SetPosition(page + 1, current_col);
            for (uint8_t i = 0; i < 6; i++) {
                uint16_t data = ((uint16_t)arry_char[offset + i + 6] << 8) | (uint16_t)arry_char[offset + i];
                data = (data >> 3) >> 8;
                LCD_SendData((uint8_t)data);
            }
            current_col += 6;
        } else if (ch == ' ') {
            current_col += 6;
        }
        str++;
    }
}


/**
 * 函数: LCD_DisplayPressure
 * 功能: 显示压力值（与头文件声明匹配）
 */
void LCD_DisplayPressure(uint16_t target, uint16_t current) {
    // 显示目标压力
    LCD_DisplayNumber(2, 20, target);
    LCD_DisplayString(2, 50, "mmHg");
    
    // 显示当前压力
    LCD_DisplayNumber(3, 20, current);
}

/**
 * 函数: LCD_DisplayMode
 * 功能: 显示工作模式（与头文件声明匹配）
 */
void LCD_DisplayMode(WorkMode_e mode) {
    switch (mode) {
        case MODE_STANDBY:
            LCD_DisplayString(0, 20, "STANDBY");
            break;
        case MODE_CONTINUOUS:
            LCD_DisplayString(0, 20, "CONTINUE");
            break;
        case MODE_INTERMITTENT:
            LCD_DisplayString(0, 20, "INTERVAL");
            break;
        default:
            break;
    }
}


/**
 * 函数: LCD_DisplayStartup
 * 功能: 显示开机Logo
 */
void LCD_DisplayStartup(void) {
    LCD_Clear();
    // 显示Logo（需要从lcd_images.h获取）
    // LCD_DisplayImage(logo_data);
    LCD_DisplayString(3, 30, "NPWT V1 0");
}


/**
 * 函数: LCD_DisplayBatteryIcon
 * 功能: 显示电池图标（与头文件声明匹配）
 */
void LCD_DisplayBatteryIcon(uint8_t page, uint8_t column, uint8_t battery_percent) {
    // 根据电量百分比显示电池图标
    // 简化实现：显示"BAT"文字
    LCD_DisplayString(page, column, "BAT");
    LCD_DisplayNumber(page, column + 20, battery_percent);
}


/**
 * 函数: LCD_SetBacklight
 * 功能: 设置背光（简化实现）
 */
void LCD_SetBacklight(bool on) {
    // PIC18F46J11没有硬件背光控制，这里是空实现
    (void)on;
}

/**
 * 函数: LCD_DisplayError
 * 功能: 显示错误信息
 */
void LCD_DisplayError(ErrorCode_e error) {
    LCD_Clear();
    LCD_DisplayString(2, 20, "ERROR");
    LCD_DisplayNumber(3, 20, (uint16_t)error);
}
