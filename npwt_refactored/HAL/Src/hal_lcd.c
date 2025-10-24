/**
  ******************************************************************************
  * @file:    hal_lcd.c
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   LCD硬件抽象层实现
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#include "hal_lcd.h"
#include "hal_gpio.h"
#include "hal_timer.h"
#include "../../Middleware/Inc/soft_timer.h"
#include <stddef.h>

/****************************************************************************
 * LCD硬件控制宏定义
 ****************************************************************************/
#define LCD_CS   LATEbits.LATE2  // PORTE Pin 2 (片选)
#define LCD_RS   LATAbits.LATA7  // PORTA Pin 7 (寄存器选择)
#define LCD_RD   LATEbits.LATE0  // PORTE Pin 0 (读信号)
#define LCD_WR   LATEbits.LATE1  // PORTE Pin 1 (写信号)
#define LCD_RES  LATAbits.LATA6  // PORTA Pin 6 (复位)
#define LCD_DATA LATD            // PORTD (8位数据总线)

/****************************************************************************
 * 全局变量
 ****************************************************************************/
static HAL_LCD_Context_t g_lcd_context = {0};

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      HAL_LCD_Delay
 * @brief     LCD延时函数（与BIOS_JLX12864_DELAY完全一致）
 * @param     ms - 延时毫秒数
 * @retval    无
 */
void HAL_LCD_Delay(uint16_t ms)
{
    // 与原始BIOS_JLX12864_DELAY完全一致的实现
    volatile uint16_t j, k;
    for (j = 0; j < ms; j++) {
        for (k = 0; k < 10; k++) {
            // 空循环，与原始代码保持一致
        }
    }
}


/**
 * @name      HAL_LCD_SetBacklight
 * @brief     设置LCD背光
 * @param     enable - true开启，false关闭
 * @retval    无
 */
void HAL_LCD_SetBacklight(bool enable)
{
    // 使用RC6控制白色背光
    if (enable) {
        LATCbits.LATC6 = 1;  // 白色背光开启
    } else {
        LATCbits.LATC6 = 0;  // 白色背光关闭
    }
}

/**
 * @name      HAL_LCD_Init
 * @brief     初始化LCD硬件（与SYS_IniLcd完全一致）
 * @param     无
 * @retval    无
 */
void HAL_LCD_Init(void)
{
    // 初始化GPIO引脚
    // PORTD设置为输出（数据总线）
    TRISD = 0x00;
    LATD = 0x00;
    
    // PORTE引脚设置
    TRISEbits.TRISE0 = 0;  // RD输出
    TRISEbits.TRISE1 = 0;  // WR输出
    TRISEbits.TRISE2 = 0;  // CS输出
    
    // PORTA引脚设置
    TRISAbits.TRISA6 = 0;  // RES输出
    TRISAbits.TRISA7 = 0;  // RS输出
    
    // RC6背光控制引脚
    TRISCbits.TRISC6 = 0;  // 背光控制输出
    
    // 初始化引脚状态
    LCD_CS = 1;    // 片选无效
    LCD_RS = 0;    // 命令模式
    LCD_RD = 1;    // 读无效
    LCD_WR = 1;    // 写无效
    LCD_RES = 1;   // 复位无效
    
    // LCD复位序列（与SYS_IniLcd完全一致）
    LCD_RES = 0;
    HAL_LCD_Delay(200);    // BIOS_JLX12864_DELAY(200)
    LCD_RES = 1;
    HAL_LCD_Delay(200);    // BIOS_JLX12864_DELAY(200)
    
    // LCD初始化命令序列（与SYS_IniLcd完全一致）
    // 发送复位命令
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = HAL_LCD_CMD_RESET;  // JLX12864G_RES
    LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_Delay(50);     // BIOS_JLX12864_DELAY(50)
    
    // 发送配置命令
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xA2;  // 1/9偏压比
    LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xA1;  // SEG方向
    LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xC0;  // COM方向
    LCD_RD = 0; LCD_CS = 1;
    
    // 电源控制序列
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x2C;
    LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_Delay(5);      // BIOS_JLX12864_DELAY(5)
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x2E;
    LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_Delay(5);      // BIOS_JLX12864_DELAY(5)
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x2F;
    LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_Delay(50);     // BIOS_JLX12864_DELAY(50)
    
    // 对比度设置
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x25;
    LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x81;
    LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x0C;
    LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_Delay(10);    // BIOS_JLX12864_DELAY(10)
    
    // 其他设置
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xAC;  // 静态显示
    LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x00;
    LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x40;  // 起始行
    LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = HAL_LCD_CMD_DISPLAY_ON;  // JLX12864G_ON
    LCD_RD = 0; LCD_CS = 1;
    
    // 清空显示（直接硬件操作）
    uint8_t i, j;
    for (i = 0; i < 9; i++) {  // 9页（0-8页）
        // 设置页地址
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0xB0 + i;  // 页地址
        LCD_RD = 0; LCD_CS = 1;
        
        // 设置列地址
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0x10;      // 列地址高4位 = 0
        LCD_RD = 0; LCD_CS = 1;
        
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0x00;      // 列地址低4位 = 0
        LCD_RD = 0; LCD_CS = 1;
        
        // 清空数据
        for (j = 0; j < 132; j++) {     // 132列（0-131列）
            LCD_CS = 0; LCD_RS = 1; LCD_RD = 1; LCD_WR = 0;
            LCD_DATA = 0x00;     // 清空数据
            LCD_CS = 1; LCD_RD = 0;
        }
    }
    
    // 开启背光
    HAL_LCD_SetBacklight(true);
}

/****************************************************************************
 * 非阻塞LCD函数实现
 ****************************************************************************/

/**
 * @name      HAL_LCD_NonBlockingDelay
 * @brief     非阻塞延时检查
 * @param     duration_ms - 延时毫秒数
 * @retval    bool true-延时完成, false-延时未完成
 */
bool HAL_LCD_NonBlockingDelay(uint32_t duration_ms)
{
    uint32_t current_tick = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
    
    if (g_lcd_context.delay_start_tick == 0) {
        // 开始新的延时
        g_lcd_context.delay_start_tick = current_tick;
        g_lcd_context.delay_duration_ms = duration_ms;
        return false;
    }
    
    // 检查延时是否完成
    if ((current_tick - g_lcd_context.delay_start_tick) >= g_lcd_context.delay_duration_ms) {
        // 延时完成，重置
        g_lcd_context.delay_start_tick = 0;
        g_lcd_context.delay_duration_ms = 0;
        return true;
    }
    
    return false;
}

/**
 * @name      HAL_LCD_IsBusy
 * @brief     检查LCD是否忙碌
 * @param     无
 * @retval    bool true-忙碌, false-空闲
 */
bool HAL_LCD_IsBusy(void)
{
    return g_lcd_context.is_busy;
}

/**
 * @name      HAL_LCD_SendCommandNonBlocking
 * @brief     非阻塞发送LCD命令
 * @param     cmd - 命令字节
 * @retval    无
 */
void HAL_LCD_SendCommandNonBlocking(uint8_t cmd)
{
    if (g_lcd_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_lcd_context.pending_cmd = cmd;
    g_lcd_context.state = HAL_LCD_STATE_SEND_COMMAND;
    g_lcd_context.is_busy = true;
    g_lcd_context.delay_start_tick = 0;
}

/**
 * @name      HAL_LCD_SendDataNonBlocking
 * @brief     非阻塞发送LCD数据
 * @param     data - 数据字节
 * @retval    无
 */
void HAL_LCD_SendDataNonBlocking(uint8_t data)
{
    if (g_lcd_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_lcd_context.pending_data = data;
    g_lcd_context.state = HAL_LCD_STATE_SEND_DATA;
    g_lcd_context.is_busy = true;
    g_lcd_context.delay_start_tick = 0;
}

/**
 * @name      HAL_LCD_SetPositionNonBlocking
 * @brief     非阻塞设置LCD显示位置
 * @param     page - 页地址 (0-7)
 * @param     column - 列地址 (0-127)
 * @retval    无
 */
void HAL_LCD_SetPositionNonBlocking(uint8_t page, uint8_t column)
{
    if (g_lcd_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_lcd_context.pending_page = page;
    g_lcd_context.pending_column = column;
    g_lcd_context.state = HAL_LCD_STATE_SET_POSITION;
    g_lcd_context.is_busy = true;
    g_lcd_context.delay_start_tick = 0;
}

/**
 * @name      HAL_LCD_ClearNonBlocking
 * @brief     非阻塞LCD清屏
 * @param     无
 * @retval    无
 */
void HAL_LCD_ClearNonBlocking(void)
{
    g_lcd_context.state = HAL_LCD_STATE_CLEAR_PAGE;
    g_lcd_context.clear_page = 0;
    g_lcd_context.clear_column = 0;
    g_lcd_context.is_busy = true;
}

/**
 * @name      HAL_LCD_Process
 * @brief     LCD状态机处理函数（非阻塞）
 * @param     无
 * @retval    无
 */
void HAL_LCD_Process(void)
{
    if (!g_lcd_context.is_busy) {
        return;
    }
    
    switch (g_lcd_context.state) {
        case HAL_LCD_STATE_SEND_COMMAND:
            // 发送命令（立即执行，无需延时）
            LCD_CS = 0;     // CS=0
            LCD_RS = 0;     // RS=0 (命令模式)
            LCD_RD = 1;     // RD=1
            LCD_WR = 0;     // WR=0
            
            LCD_DATA = g_lcd_context.pending_cmd; // 数据
            
            LCD_RD = 0;     // RD=0 (关键时序)
            LCD_CS = 1;     // CS=1
            
            g_lcd_context.state = HAL_LCD_STATE_COMPLETE;
            break;
            
        case HAL_LCD_STATE_SEND_DATA:
            // 发送数据（立即执行，无需延时）
            LCD_CS = 0;     // CS=0
            LCD_RS = 1;     // RS=1 (数据模式)
            LCD_RD = 1;     // RD=1
            LCD_WR = 0;     // WR=0
            
            LCD_DATA = g_lcd_context.pending_data; // 数据
            
            LCD_CS = 1;     // CS=1
            LCD_RD = 0;     // RD=0
            
            g_lcd_context.state = HAL_LCD_STATE_COMPLETE;
            break;
            
        case HAL_LCD_STATE_SET_POSITION:
            // 设置位置（需要发送3个命令）
            static uint8_t pos_step = 0;
            
            switch (pos_step) {
                case 0:
                    // 发送页地址命令
                    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
                    LCD_DATA = 0xB0 | (g_lcd_context.pending_page & 0x0F);
                    LCD_RD = 0; LCD_CS = 1;
                    pos_step = 1;
                    break;
                case 1:
                    // 发送列地址高4位
                    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
                    LCD_DATA = 0x10 | ((g_lcd_context.pending_column >> 4) & 0x0F);
                    LCD_RD = 0; LCD_CS = 1;
                    pos_step = 2;
                    break;
                case 2:
                    // 发送列地址低4位
                    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
                    LCD_DATA = 0x00 | (g_lcd_context.pending_column & 0x0F);
                    LCD_RD = 0; LCD_CS = 1;
                    pos_step = 0; // 重置步骤
                    g_lcd_context.state = HAL_LCD_STATE_COMPLETE;
                    break;
            }
            break;
            
        case HAL_LCD_STATE_CLEAR_PAGE:
            // 设置页地址
            LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
            LCD_DATA = 0xB0 + g_lcd_context.clear_page;
            LCD_RD = 0; LCD_CS = 1;
            g_lcd_context.state = HAL_LCD_STATE_CLEAR_COLUMN;
            break;
            
        case HAL_LCD_STATE_CLEAR_COLUMN:
            // 设置列地址
            LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
            LCD_DATA = 0x10;  // 列地址高4位 = 0
            LCD_RD = 0; LCD_CS = 1;
            
            LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
            LCD_DATA = 0x00;  // 列地址低4位 = 0
            LCD_RD = 0; LCD_CS = 1;
            
            g_lcd_context.state = HAL_LCD_STATE_CLEAR_DATA;
            g_lcd_context.clear_column = 0;
            break;
            
        case HAL_LCD_STATE_CLEAR_DATA:
            // 发送清空数据
            LCD_CS = 0; LCD_RS = 1; LCD_RD = 1; LCD_WR = 0;
            LCD_DATA = 0x00;
            LCD_CS = 1; LCD_RD = 0;
            
            g_lcd_context.clear_column++;
            
            if (g_lcd_context.clear_column >= 132) {
                // 当前页完成，切换到下一页
                g_lcd_context.clear_page++;
                if (g_lcd_context.clear_page >= 9) {
                    // 所有页完成
                    g_lcd_context.state = HAL_LCD_STATE_COMPLETE;
                } else {
                    g_lcd_context.state = HAL_LCD_STATE_CLEAR_PAGE;
                }
            }
            break;
            
        case HAL_LCD_STATE_COMPLETE:
            g_lcd_context.is_busy = false;
            break;
            
        default:
            g_lcd_context.is_busy = false;
            break;
    }
}
