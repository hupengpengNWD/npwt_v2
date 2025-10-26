#include "hal_lcd.h"
#include "hal_timer.h"
#include <string.h>
#include <xc.h>  // PIC18F46J11寄存器定义

/****************************************************************************
 * LCD硬件控制引脚定义（与未重构工程完全一致）
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

static QueueHandle_t g_hal_lcd_queue = 0;
static HAL_LCD_Context_t g_lcd_context;

/****************************************************************************
 * 私有函数声明
 ****************************************************************************/

static void HAL_LCD_ProcessEvent(const HAL_LCD_Event_t* event);
static void HAL_LCD_SendCommandInternal(uint8_t cmd);
static void HAL_LCD_SendDataInternal(uint8_t data);
static void HAL_LCD_SetPositionInternal(uint8_t page, uint8_t column);
static void HAL_LCD_ClearInternal(void);
static void HAL_LCD_HardwareDelay(uint16_t ms);
static void HAL_LCD_StartDelay(uint32_t ms, HAL_LCD_State_e next_state);
static bool HAL_LCD_IsDelayComplete(void);

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
    // 初始化HAL_LCD队列
    QueueConfig_t queue_config = {
        .capacity = 8,
        .element_size = sizeof(HAL_LCD_Event_t),
        .enable_blocking = false,
        .timeout_ms = 0
    };
    g_hal_lcd_queue = Queue_Create(&queue_config);
    if (g_hal_lcd_queue == 0xFF) {
        // 队列创建失败，系统无法正常工作
        while(1);  // 死循环，等待看门狗复位
    }
    
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
    HAL_LCD_HardwareDelay(1);
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x81; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x0C; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(10);
    
    // 其他设置
    // 静态显示
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0xAC; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x00; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    // 起始行
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = 0x40; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    // 开显示
    LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
    LCD_DATA = JLX12864G_ON; LCD_RD = 0; LCD_CS = 1;
    HAL_LCD_HardwareDelay(1);
    
    // 初始化后清屏（与未重构代码DISP_Clear完全一致）
    uint8_t page, column;
    
    // 清屏8页，每页132列（与未重构代码一致）
    for (page = 0; page < 8; page++) {
        // 设置页地址
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0xB0 + page; LCD_RD = 0; LCD_CS = 1;
        
        // 设置列地址为0
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0x10; LCD_RD = 0; LCD_CS = 1;  // 列地址高4位
        
        LCD_CS = 0; LCD_RS = 0; LCD_RD = 1; LCD_WR = 0;
        LCD_DATA = 0x00; LCD_RD = 0; LCD_CS = 1;  // 列地址低4位
        
        // 清空整页数据（132列，与未重构代码一致）
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
void HAL_LCD_Delay(uint16_t ms)
{
    HAL_LCD_HardwareDelay(ms);
}

/**
 * @name      HAL_LCD_SendCommandNonBlocking
 * @brief     非阻塞发送LCD命令
 * @param     cmd - 命令字节
 * @retval    无
 */
void HAL_LCD_SendCommandNonBlocking(uint8_t cmd)
{
    HAL_LCD_Event_t event = {
        .type = HAL_LCD_EVENT_SEND_COMMAND,
        .cmd = cmd
    };
    
    // 将发送命令事件加入队列
    if (g_hal_lcd_queue != 0) {
        Queue_Enqueue(g_hal_lcd_queue, &event);
    }
}

/**
 * @name      HAL_LCD_SendDataNonBlocking
 * @brief     非阻塞发送LCD数据
 * @param     data - 数据字节
 * @retval    无
 */
void HAL_LCD_SendDataNonBlocking(uint8_t data)
{
    HAL_LCD_Event_t event = {
        .type = HAL_LCD_EVENT_SEND_DATA,
        .data = data
    };
    
    // 将发送数据事件加入队列
    if (g_hal_lcd_queue != 0) {
        Queue_Enqueue(g_hal_lcd_queue, &event);
    }
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
    HAL_LCD_Event_t event = {
        .type = HAL_LCD_EVENT_SET_POSITION,
        .page = page,
        .column = column
    };
    
    // 将设置位置事件加入队列
    if (g_hal_lcd_queue != 0) {
        Queue_Enqueue(g_hal_lcd_queue, &event);
    }
}

/**
 * @name      HAL_LCD_ClearNonBlocking
 * @brief     非阻塞LCD清屏
 * @param     无
 * @retval    无
 */
void HAL_LCD_ClearNonBlocking(void)
{
    HAL_LCD_Event_t event = {
        .type = HAL_LCD_EVENT_CLEAR
    };
    
    // 将清屏事件加入队列
    if (g_hal_lcd_queue != 0) {
        Queue_Enqueue(g_hal_lcd_queue, &event);
    }
}

/**
 * @name      HAL_LCD_Process
 * @brief     LCD状态机处理函数（非阻塞）
 * @param     无
 * @retval    无
 */
void HAL_LCD_Process(void)
{
    HAL_LCD_Event_t event = {0};
    
    // 如果当前有操作在进行，处理状态机
    if (g_lcd_context.is_busy) {
        // 处理当前状态机
        switch (g_lcd_context.state) {
            case HAL_LCD_STATE_SEND_COMMAND:// 发送命令
                HAL_LCD_SendCommandInternal(g_lcd_context.cmd);
                break;
                
            case HAL_LCD_STATE_SEND_DATA:// 发送数据
                HAL_LCD_SendDataInternal(g_lcd_context.data);
                break;
                
            case HAL_LCD_STATE_SET_POSITION:// 设置位置
                HAL_LCD_SetPositionInternal(g_lcd_context.page, g_lcd_context.column);
                break;
                
            case HAL_LCD_STATE_DELAY:// 延时状态
                // 检查延时是否完成
                if (HAL_LCD_IsDelayComplete()) {
                    // 延时完成，切换到下一个状态
                    g_lcd_context.state = g_lcd_context.next_state;
                    
                    // 如果下一个状态是COMPLETE，则完成当前操作
                    if (g_lcd_context.state == HAL_LCD_STATE_COMPLETE) {
                        g_lcd_context.is_busy = false;
                    }
                }
                break;
                
            case HAL_LCD_STATE_CLEAR_PAGE:
            case HAL_LCD_STATE_CLEAR_COLUMN:
            case HAL_LCD_STATE_CLEAR_DATA:
                HAL_LCD_ClearInternal();
                break;
                
            case HAL_LCD_STATE_COMPLETE:
                g_lcd_context.is_busy = false;
                g_lcd_context.state = HAL_LCD_STATE_IDLE;
                break;
                
            default:
                g_lcd_context.is_busy = false;
                g_lcd_context.state = HAL_LCD_STATE_IDLE;
                break;
        }
    } else {
        // 如果空闲，从队列中取出新事件
        if (Queue_Dequeue(g_hal_lcd_queue, &event)) {
            HAL_LCD_ProcessEvent(&event);
        }
    }
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
 * @name      HAL_LCD_GetQueueCount
 * @brief     获取HAL_LCD队列中事件数量
 * @param     无
 * @retval    队列中事件数量
 */
uint8_t HAL_LCD_GetQueueCount(void)
{
    if (g_hal_lcd_queue == 0) {
        return 0;
    }
    return (uint8_t)Queue_GetCount(g_hal_lcd_queue);
}

/**
 * @name      HAL_LCD_ClearQueue
 * @brief     清空HAL_LCD队列
 * @param     无
 * @retval    无
 */
void HAL_LCD_ClearQueue(void)
{
    if (g_hal_lcd_queue != 0) {
        Queue_Clear(g_hal_lcd_queue);
    }
}

/****************************************************************************
 * 私有函数实现
 ****************************************************************************/

/**
 * @name      HAL_LCD_ProcessEvent
 * @brief     处理HAL_LCD事件
 * @param     event - HAL_LCD事件
 * @retval    无
 */
static void HAL_LCD_ProcessEvent(const HAL_LCD_Event_t* event)
{
    if (event == NULL) {
        return;
    }
    
    switch (event->type) {
        case HAL_LCD_EVENT_SEND_COMMAND:
            g_lcd_context.cmd = event->cmd;
            g_lcd_context.state = HAL_LCD_STATE_SEND_COMMAND;
            g_lcd_context.is_busy = true;
            break;
            
        case HAL_LCD_EVENT_SEND_DATA:
            g_lcd_context.data = event->data;
            g_lcd_context.state = HAL_LCD_STATE_SEND_DATA;
            g_lcd_context.is_busy = true;
            break;
            
        case HAL_LCD_EVENT_SET_POSITION:
            g_lcd_context.page = event->page;
            g_lcd_context.column = event->column;
            g_lcd_context.step_counter = 0;  // 重置步骤计数器
            g_lcd_context.state = HAL_LCD_STATE_SET_POSITION;
            g_lcd_context.is_busy = true;
            break;
            
        case HAL_LCD_EVENT_CLEAR:
            g_lcd_context.state = HAL_LCD_STATE_CLEAR_PAGE;
            g_lcd_context.clear_page = 0;
            g_lcd_context.clear_column = 0;
            g_lcd_context.is_busy = true;
            break;
            
        default:
            // 未知事件类型，忽略
            break;
    }
}

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
    
    // 开始延时，参考原始代码的延时需求
    // 原始代码中每次命令后都有延时，这里使用1ms延时
    HAL_LCD_StartDelay(1, HAL_LCD_STATE_COMPLETE);
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
    
    // 开始延时，参考原始代码的延时需求
    // 原始代码中每次数据后都有延时，这里使用1ms延时
    HAL_LCD_StartDelay(1, HAL_LCD_STATE_COMPLETE);
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
    // 根据步骤计数器执行不同的步骤
    switch (g_lcd_context.step_counter) {
        case 0:
            // 第一步：设置页地址
            LCD_CS = 0;        // 片选有效
            LCD_RS = 0;        // 命令模式
            LCD_RD = 1;        // 读信号无效
            LCD_WR = 0;        // 写信号有效
            LCD_DATA = 0xB0 + page;    // 设置页地址
            LCD_RD = 0;        // 读信号有效
            LCD_CS = 1;        // 片选无效
            
            // 保存参数到上下文
            g_lcd_context.page = page;
            g_lcd_context.column = column;
            g_lcd_context.step_counter = 1;
            
            // 延时后执行下一步
            HAL_LCD_StartDelay(1, HAL_LCD_STATE_SET_POSITION);
            g_lcd_context.state = HAL_LCD_STATE_DELAY;
            break;
            
        case 1:
            // 第二步：设置列地址高4位
            LCD_CS = 0;        // 片选有效
            LCD_RS = 0;        // 命令模式
            LCD_RD = 1;        // 读信号无效
            LCD_WR = 0;        // 写信号有效
            LCD_DATA = 0x10 + (g_lcd_context.column >> 4);    // 设置列地址高4位
            LCD_RD = 0;        // 读信号有效
            LCD_CS = 1;        // 片选无效
            
            g_lcd_context.step_counter = 2;
            
            // 延时后执行下一步
            HAL_LCD_StartDelay(1, HAL_LCD_STATE_SET_POSITION);
            g_lcd_context.state = HAL_LCD_STATE_DELAY;
            break;
            
        case 2:
            // 第三步：设置列地址低4位
            LCD_CS = 0;        // 片选有效
            LCD_RS = 0;        // 命令模式
            LCD_RD = 1;        // 读信号无效
            LCD_WR = 0;        // 写信号有效
            LCD_DATA = 0x00 + (g_lcd_context.column & 0x0F);    // 设置列地址低4位
            LCD_RD = 0;        // 读信号有效
            LCD_CS = 1;        // 片选无效
            
            // 完成设置位置
            HAL_LCD_StartDelay(1, HAL_LCD_STATE_COMPLETE);
            g_lcd_context.state = HAL_LCD_STATE_DELAY;
            break;
            
        default:
            // 重置步骤计数器
            g_lcd_context.step_counter = 0;
            break;
    }
}

/**
 * @name      HAL_LCD_ClearInternal
 * @brief     内部清屏函数
 * @param     无
 * @retval    无
 */
static void HAL_LCD_ClearInternal(void)
{
    // 实际的硬件清屏代码（与未重构代码DISP_Clear完全一致）
    uint8_t page, column;
    
    // 清屏8页，每页132列（与未重构代码一致）
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
        
        // 清空整页数据（132列，与未重构代码一致）
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


/**
 * @name      HAL_LCD_StartDelay
 * @brief     开始非阻塞延时
 * @param     ms - 延时毫秒数
 * @param     next_state - 延时后的下一个状态
 * @retval    无
 */
static void HAL_LCD_StartDelay(uint32_t ms, HAL_LCD_State_e next_state)
{
    g_lcd_context.delay_start_tick = HAL_Timer_GetTick();
    g_lcd_context.delay_duration_ms = ms;
    g_lcd_context.next_state = next_state;
    g_lcd_context.state = HAL_LCD_STATE_DELAY;
}

/**
 * @name      HAL_LCD_IsDelayComplete
 * @brief     检查延时是否完成
 * @param     无
 * @retval    true - 延时完成，false - 延时未完成
 */
static bool HAL_LCD_IsDelayComplete(void)
{
    uint32_t current_tick = HAL_Timer_GetTick();
    uint32_t elapsed_ms = current_tick - g_lcd_context.delay_start_tick;
    return (elapsed_ms >= g_lcd_context.delay_duration_ms);
}