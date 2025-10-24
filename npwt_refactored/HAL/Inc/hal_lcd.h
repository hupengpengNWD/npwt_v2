/**
  ******************************************************************************
  * @file:    hal_lcd.h
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   LCD硬件抽象层头文件
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#ifndef __HAL_LCD_H
#define __HAL_LCD_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Middleware/Inc/queue.h"

/****************************************************************************
 * LCD硬件引脚定义
 ****************************************************************************/
// LCD控制引脚（与未重构工程完全一致）
#define HAL_LCD_CS_PORT   PORTE
#define HAL_LCD_CS_PIN    2
#define HAL_LCD_RS_PORT   PORTA  
#define HAL_LCD_RS_PIN    7
#define HAL_LCD_RD_PORT   PORTE
#define HAL_LCD_RD_PIN    0
#define HAL_LCD_WR_PORT   PORTE
#define HAL_LCD_WR_PIN    1
#define HAL_LCD_RES_PORT  PORTA
#define HAL_LCD_RES_PIN   6
#define HAL_LCD_DATA_PORT PORTD

/****************************************************************************
 * LCD命令定义
 ****************************************************************************/
#define HAL_LCD_CMD_RESET     0xE2    // 复位
#define HAL_LCD_CMD_DISPLAY_ON  0xAF  // 开显示
#define HAL_LCD_CMD_DISPLAY_OFF 0xAE  // 关显示

/****************************************************************************
 * LCD参数定义
 ****************************************************************************/
#define HAL_LCD_WIDTH     128
#define HAL_LCD_HEIGHT    64
#define HAL_LCD_PAGES     8   // 64/8 = 8页

/****************************************************************************
 * LCD事件类型定义
 ****************************************************************************/
typedef enum {
    HAL_LCD_EVENT_SEND_COMMAND = 0,   // 发送命令
    HAL_LCD_EVENT_SEND_DATA,          // 发送数据
    HAL_LCD_EVENT_SET_POSITION,       // 设置位置
    HAL_LCD_EVENT_CLEAR               // 清屏
} HAL_LCD_EventType_e;

/****************************************************************************
 * LCD事件结构体
 ****************************************************************************/
typedef struct {
    HAL_LCD_EventType_e type;         // 事件类型
    uint8_t cmd;                      // 命令数据
    uint8_t data;                     // 数据字节
    uint8_t page;                     // 页地址
    uint8_t column;                   // 列地址
} HAL_LCD_Event_t;

/****************************************************************************
 * LCD状态机定义
 ****************************************************************************/
typedef enum {
    HAL_LCD_STATE_IDLE = 0,           // 空闲状态
    HAL_LCD_STATE_SEND_COMMAND,      // 发送命令
    HAL_LCD_STATE_SEND_DATA,         // 发送数据
    HAL_LCD_STATE_SET_POSITION,      // 设置位置
    HAL_LCD_STATE_CLEAR_PAGE,        // 清屏：设置页地址
    HAL_LCD_STATE_CLEAR_COLUMN,      // 清屏：设置列地址
    HAL_LCD_STATE_CLEAR_DATA,        // 清屏：发送数据
    HAL_LCD_STATE_DELAY,             // 延时状态
    HAL_LCD_STATE_COMPLETE           // 完成状态
} HAL_LCD_State_e;

typedef struct {
    HAL_LCD_State_e state;            // 当前状态
    uint32_t delay_start_tick;        // 延时开始时间
    uint32_t delay_duration_ms;       // 延时持续时间
    uint8_t clear_page;               // 清屏当前页
    uint8_t clear_column;             // 清屏当前列
    bool is_busy;                     // 是否忙碌
    
    // 当前操作的数据
    uint8_t cmd;                      // 当前命令
    uint8_t data;                     // 当前数据
    uint8_t page;                     // 当前页地址
    uint8_t column;                   // 当前列地址
    
    // 延时相关
    uint32_t current_tick;            // 当前tick计数
    HAL_LCD_State_e next_state;      // 延时后的下一个状态
    
    // 状态机步骤控制
    uint8_t step_counter;             // 步骤计数器
} HAL_LCD_Context_t;

/****************************************************************************
 * 函数声明
 ****************************************************************************/

/**
 * @name      HAL_LCD_Init
 * @brief     初始化LCD硬件
 * @param     无
 * @retval    无
 */
void HAL_LCD_Init(void);


/**
 * @name      HAL_LCD_SetBacklight
 * @brief     设置LCD背光
 * @param     enable - true开启，false关闭
 * @retval    无
 */
void HAL_LCD_SetBacklight(bool enable);

/**
 * @name      HAL_LCD_Delay
 * @brief     LCD延时函数（阻塞版本，仅用于兼容）
 * @param     ms - 延时毫秒数
 * @retval    无
 */
void HAL_LCD_Delay(uint16_t ms);

/**
 * @name      HAL_LCD_SendCommandNonBlocking
 * @brief     非阻塞发送LCD命令
 * @param     cmd - 命令字节
 * @retval    无
 */
void HAL_LCD_SendCommandNonBlocking(uint8_t cmd);

/**
 * @name      HAL_LCD_SendDataNonBlocking
 * @brief     非阻塞发送LCD数据
 * @param     data - 数据字节
 * @retval    无
 */
void HAL_LCD_SendDataNonBlocking(uint8_t data);

/**
 * @name      HAL_LCD_SetPositionNonBlocking
 * @brief     非阻塞设置LCD显示位置
 * @param     page - 页地址 (0-7)
 * @param     column - 列地址 (0-127)
 * @retval    无
 */
void HAL_LCD_SetPositionNonBlocking(uint8_t page, uint8_t column);

/**
 * @name      HAL_LCD_ClearNonBlocking
 * @brief     非阻塞LCD清屏
 * @param     无
 * @retval    无
 */
void HAL_LCD_ClearNonBlocking(void);

/**
 * @name      HAL_LCD_Process
 * @brief     LCD状态机处理函数（非阻塞）
 * @param     无
 * @retval    无
 */
void HAL_LCD_Process(void);

/**
 * @name      HAL_LCD_IsBusy
 * @brief     检查LCD是否忙碌
 * @param     无
 * @retval    bool true-忙碌, false-空闲
 */
bool HAL_LCD_IsBusy(void);


/**
 * @name      HAL_LCD_GetQueueCount
 * @brief     获取HAL_LCD队列中事件数量
 * @param     无
 * @retval    队列中事件数量
 */
uint8_t HAL_LCD_GetQueueCount(void);

/**
 * @name      HAL_LCD_ClearQueue
 * @brief     清空HAL_LCD队列
 * @param     无
 * @retval    无
 */
void HAL_LCD_ClearQueue(void);

#endif /* __HAL_LCD_H */
