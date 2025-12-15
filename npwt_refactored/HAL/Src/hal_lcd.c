/****************************************************************************
 * 文件名: hal_lcd.c
 * 功能: 显示器硬件抽象层实现
 * 作者: 韦睿医疗
 * 说明: 
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "hal_lcd.h"
#include "hal_timer.h"
#include <string.h>
#include <xc.h>  // PIC18F46J11寄存器定义

/****************************************************************************
 * LCD硬件控制引脚定义（与老版本工程完全一致）
 ****************************************************************************/
#define LCD_CS   LATEbits.LATE2  // PORTE Pin 2 (片选)
#define LCD_RS   LATAbits.LATA7  // PORTA Pin 7 (寄存器选择)
#define LCD_RD   LATEbits.LATE0  // PORTE Pin 0 (读信号)
#define LCD_WR   LATEbits.LATE1  // PORTE Pin 1 (写信号)
#define LCD_RES  LATAbits.LATA6  // PORTA Pin 6 (复位)
#define LCD_DATA LATD            // PORTD (8位数据总线)

/****************************************************************************
 * LCD命令定义
 ****************************************************************************/
#define JLX12864G_RES   0xE2    // 复位
#define JLX12864G_ON    0xAF    // 开显示
#define JLX12864G_OFF   0xAE    // 关显示

/****************************************************************************
 * 私有变量
 ****************************************************************************/

static HAL_LCD_Context_t g_lcd_context;

/****************************************************************************
 * 私有函数声明
 ****************************************************************************/

static void HAL_LCD_SendCommandInternal(uint8_t cmd);
static void HAL_LCD_SendDataInternal(uint8_t data);
static void HAL_LCD_SetPositionInternal(uint8_t page, uint8_t column);
static void HAL_LCD_ClearInternal(void);
static void HAL_LCD_HardwareDelay(uint16_t ms);

/****************************************************************************
 * 公共接口实现
 ****************************************************************************/

/**
 * @name      HAL_LCD_Init
 * @brief     初始化LCD硬件
 * @param     无
 * @retval    无
 */
void HAL_LCD_Init(void)
{
    // 队列模式移除：直接同步写硬件，不再创建队列

    // 初始化LCD上下文
    memset(&g_lcd_context, 0, sizeof(HAL_LCD_Context_t));
    g_lcd_context.state = HAL_LCD_STATE_IDLE;
    g_lcd_context.is_busy = false;
    
    // 硬件初始化（阻塞版本，仅用于初始化）
    // 复位LCD
    LCD_RES = 0;
    HAL_LCD_HardwareDelay(200);
    LCD_RES = 1;
    HAL_LCD_HardwareDelay(200);
    
    // 初始化命令序列
    // 复位命令
    LCD_CS = 0;        // 片选有效
    LCD_RS = 0;        // 命令模式
    LCD_RD = 1;        // 读信号无效
    LCD_WR = 0;        // 写信号有效
    LCD_DATA = JLX12864G_RES;    // 发送复位命令
    LCD_RD = 0;        // 读信号有效
    LCD_CS = 1;        // 片选无效
    HAL_LCD_HardwareDelay(50);
    
    // 1/9偏压比
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xA2; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    // SEG方向
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xA1; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    // COM方向
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xC0; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    // 电源控制
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x2C; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(5);
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x2E; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(5);
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x2F; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(50);
    
    // 对比度设置
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x25; LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x81; LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x0C; LCD_RD = 0; LCD_CS = 1;

    HAL_LCD_HardwareDelay(10);
    
    // 静态显示
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xAC; LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x00; LCD_RD = 0; LCD_CS = 1;
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x40; LCD_RD = 0; LCD_CS = 1;

    HAL_LCD_HardwareDelay(10);
    
    // 开显示
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = JLX12864G_ON; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    // 初始化后清屏（与老版本代码DISP_Clear完全一致）
    uint8_t page, column;
    
    // 清屏8页，每页132列（与老版本代码一致）
    for (page = 0; page < 8; page++) {
        // 设置页地址
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0xB0 + page; LCD_RD = 0; LCD_CS = 1;
        
        // 设置列地址为0
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0x10; LCD_RD = 0; LCD_CS = 1;  // 列地址高4位
        
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0x00; LCD_RD = 0; LCD_CS = 1;  // 列地址低4位
        
        // 清空整页数据（132列，与老版本代码一致）
        for (column = 0; column < 132; column++) {
            LCD_CS = 0; LCD_RS = 1; LCD_RD = 1; LCD_WR = 0;
            LCD_DATA = 0x00; LCD_CS = 1; LCD_RD = 0;  // 发送0x00清空
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
    // 背光控制在main函数中处理，这里仅占位
    (void)enable;
}

/**
 * @name      HAL_LCD_Delay
 * @brief     LCD延时函数（阻塞版本，仅用于兼容）
 * @param     ms - 延时毫秒数
 * @retval    无
 */
// void HAL_LCD_Delay(uint16_t ms)
// {
//     HAL_LCD_HardwareDelay(ms);
// }

/**
 * @name      HAL_LCD_SendCommandNonBlocking
 * @brief     非阻塞发送LCD命令
 * @param     cmd - 命令字节
 * @retval    无
 */
// void HAL_LCD_SendCommandNonBlocking(uint8_t cmd)
// {
//     // 同步执行：直接写命令
//     HAL_LCD_SendCommandInternal(cmd);
// }

/**
 * @name      HAL_LCD_SendDataNonBlocking
 * @brief     非阻塞发送LCD数据
 * @param     data - 数据字节
 * @retval    无
 */
void HAL_LCD_SendDataNonBlocking(uint8_t data)
{
    // 同步执行：直接写数据
    HAL_LCD_SendDataInternal(data);
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
    // page 参数此时已经是硬件坐标（由Display层转换后传入，6=顶部）
    // 列坐标仍然需要反向补偿，且加入可视区左边界偏移修正以避免首列裁切：
    // col_hw = 127 - column - OFFSET（OFFSET=8，可按实机微调）
    uint8_t page_hw = page & 0x07;  // 直接使用，已经是硬件坐标
    const int8_t OFFSET = 6;
    int16_t col_hw  = (int16_t)127 - (int16_t)column - (int16_t)OFFSET;
    if (col_hw < 0)  col_hw = 0;
    if (col_hw > 131) col_hw = 131;

    uint8_t col_high = (uint8_t)((col_hw >> 4) & 0x0F);
    uint8_t col_low  = (uint8_t)(col_hw & 0x0F);

    g_lcd_context.step_counter = 0;
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0; LCD_DATA = (uint8_t)(0xB0 + page_hw); LCD_RD = 0; LCD_CS = 1;
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0; LCD_DATA = (uint8_t)(0x10 + col_high); LCD_RD = 0; LCD_CS = 1;
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0; LCD_DATA = (uint8_t)(0x00 + col_low); LCD_RD = 0; LCD_CS = 1;
}

/**
 * @name      HAL_LCD_ClearNonBlocking
 * @brief     非阻塞LCD清屏
 * @param     无
 * @retval    无
 */
void HAL_LCD_ClearNonBlocking(void)
{
    // 同步执行：直接清屏
    HAL_LCD_ClearInternal();
}

/**
 * @name      HAL_LCD_Process
 * @brief     LCD状态机处理函数（非阻塞）
 * @param     无
 * @retval    无
 */
void HAL_LCD_Process(void)
{
    // 同步模式：无需处理队列与状态机
    (void)0;
}

/**
 * @name      HAL_LCD_IsBusy
 * @brief     检查LCD是否忙碌
 * @param     无
 * @retval    bool true-忙碌, false-空闲
 */
bool HAL_LCD_IsBusy(void)
{
    // 同步模式：大多数情况下立即空闲
    return false;
}


/**
 * @name      HAL_LCD_GetQueueCount
 * @brief     获取HAL_LCD队列中事件数量
 * @param     无
 * @retval    队列中事件数量
 */
uint8_t HAL_LCD_GetQueueCount(void)
{
    // 同步模式：无队列
    return 0;
}

/**
 * @name      HAL_LCD_ClearQueue
 * @brief     清空HAL_LCD队列
 * @param     无
 * @retval    无
 */
void HAL_LCD_ClearQueue(void)
{
    // 同步模式：无操作
    (void)0;
}

/****************************************************************************
 * 私有函数实现
 ****************************************************************************/

/**
 * @name      HAL_LCD_SendCommandInternal
 * @brief     内部发送命令函数（非阻塞版本）
 * @param     cmd - 命令字节
 * @retval    无
 */
static void HAL_LCD_SendCommandInternal(uint8_t cmd)
{
    // 实际的硬件发送命令代码
    LCD_CS = 0;        // 片选有效
    LCD_RS = 0;        // 命令模式
    LCD_RD = 1;        // 读信号无效
    LCD_WR = 0;        // 写信号有效
    LCD_DATA = cmd;    // 发送命令
    LCD_RD = 0;        // 读信号有效
    LCD_CS = 1;        // 片选无效
    
    // 同步直写：不引入额外延时
}

/**
 * @name      HAL_LCD_SendDataInternal
 * @brief     内部发送数据函数（非阻塞版本）
 * @param     data - 数据字节
 * @retval    无
 */
static void HAL_LCD_SendDataInternal(uint8_t data)
{
    // 实际的硬件发送数据代码
    LCD_CS = 0;        // 片选有效
    LCD_RS = 1;        // 数据模式
    LCD_RD = 1;        // 读信号无效
    LCD_WR = 0;        // 写信号有效
    LCD_DATA = data;   // 发送数据
    LCD_CS = 1;        // 片选无效
    LCD_RD = 0;        // 读信号有效
    
    // 同步直写：不引入额外延时
}

/**
 * @name      HAL_LCD_SetPositionInternal
 * @brief     内部设置位置函数（非阻塞版本）
 * @param     page - 页地址
 * @param     column - 列地址
 * @retval    无
 */
static void HAL_LCD_SetPositionInternal(uint8_t page, uint8_t column)
{
    // 同步直写版本：连续完成三步设置
    (void)g_lcd_context.step_counter;
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0; LCD_DATA = (uint8_t)(0xB0 + page); LCD_RD = 0; LCD_CS = 1;
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0; LCD_DATA = (uint8_t)(0x10 + (column >> 4)); LCD_RD = 0; LCD_CS = 1;
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0; LCD_DATA = (uint8_t)(0x00 + (column & 0x0F)); LCD_RD = 0; LCD_CS = 1;
}

/**
 * @name      HAL_LCD_ClearInternal
 * @brief     内部清屏函数
 * @param     无
 * @retval    无
 */
static void HAL_LCD_ClearInternal(void)
{
    // 实际的硬件清屏代码（与老版本代码DISP_Clear完全一致）
    uint8_t page, column;
    
    // 清屏8页，每页132列（与老版本代码一致）
    for (page = 0; page < 8; page++) {
        // 设置页地址
        LCD_CS = 0;        // 片选有效
        LCD_RS = 0;        // 命令模式
        LCD_RD = 1;        // 读信号无效
        LCD_WR = 0;        // 写信号有效
        LCD_DATA = 0xB0 + page;    // 设置页地址
        LCD_RD = 0;        // 读信号有效
        LCD_CS = 1;        // 片选无效
        
        // 设置列地址为0
        LCD_CS = 0;        // 片选有效
        LCD_RS = 0;        // 命令模式
        LCD_RD = 1;        // 读信号无效
        LCD_WR = 0;        // 写信号有效
        LCD_DATA = 0x10;   // 设置列地址高4位为0
        LCD_RD = 0;        // 读信号有效
        LCD_CS = 1;        // 片选无效
        
        LCD_CS = 0;        // 片选有效
        LCD_RS = 0;        // 命令模式
        LCD_RD = 1;        // 读信号无效
        LCD_WR = 0;        // 写信号有效
        LCD_DATA = 0x00;   // 设置列地址低4位为0
        LCD_RD = 0;        // 读信号有效
        LCD_CS = 1;        // 片选无效
        
        // 清空整页数据（132列，与老版本代码一致）
        for (column = 0; column < 132; column++) {
            LCD_CS = 0;        // 片选有效
            LCD_RS = 1;        // 数据模式
            LCD_RD = 1;        // 读信号无效
            LCD_WR = 0;        // 写信号有效
            LCD_DATA = 0x00;   // 发送0x00清空
            LCD_CS = 1;        // 片选无效
            LCD_RD = 0;        // 读信号有效
        }
    }
    
    g_lcd_context.state = HAL_LCD_STATE_COMPLETE;
}

/**
 * @name      HAL_LCD_HardwareDelay
 * @brief     硬件延时函数（阻塞版本，仅用于初始化）
 * @param     ms - 延时毫秒数
 * @retval    无
 */
static void HAL_LCD_HardwareDelay(uint16_t ms)
{
    uint16_t j, k;
    for (j = 0; j < ms; j++) 
        for (k = 0; k < 10; k++);  // 内层循环10次，与原始代码一致
}