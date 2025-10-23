/**
  ******************************************************************************
  * @file:    app_beep.c
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   蜂鸣器应用层控制实现
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#include "../Inc/app_beep.h"
#include "../../HAL/Inc/hal_gpio.h"
#include <stddef.h>

/****************************************************************************
 * 全局变量定义
 ****************************************************************************/
static PushPull_t g_buzzer_instance;           // 蜂鸣器控制实例
static PushPullPtr_t g_buzzer = NULL;           // 蜂鸣器控制指针
static SoftTimerHandle_t g_beep_process_timer = 0; // 蜂鸣器处理定时器句柄

// 2秒周期循环时序：开1秒，关1秒
static const uint16_t g_buzzer_seq_array[] = {
    1000,  // 开1秒
    1000   // 关1秒
};

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      AppBeep_Init
 * @brief     初始化蜂鸣器应用层
 * @param     无
 * @retval    无
 */
void AppBeep_Init(void)
{
    // 初始化蜂鸣器实例
    g_buzzer = &g_buzzer_instance;
}

/**
 * @name      AppBeep_GetBuzzerInstance
 * @brief     获取蜂鸣器控制实例
 * @param     无
 * @retval    PushPullPtr_t - 蜂鸣器实例指针
 */
PushPullPtr_t AppBeep_GetBuzzerInstance(void)
{
    return g_buzzer;
}

/**
 * @name      AppBeep_GetBuzzerConfig
 * @brief     获取蜂鸣器配置参数
 * @param     无
 * @retval    const uint16_t* - 时序数组指针
 */
const uint16_t* AppBeep_GetBuzzerConfig(void)
{
    return g_buzzer_seq_array;
}

/**
 * @name      AppBeep_GetBuzzerSeqLength
 * @brief     获取蜂鸣器时序数组长度
 * @param     无
 * @retval    uint32_t - 时序数组长度
 */
uint32_t AppBeep_GetBuzzerSeqLength(void)
{
    return sizeof(g_buzzer_seq_array) / sizeof(g_buzzer_seq_array[0]);
}

/**
 * @name      AppBeep_GetBuzzerSeqCount
 * @brief     获取蜂鸣器重复次数
 * @param     无
 * @retval    uint32_t - 重复次数
 */
uint32_t AppBeep_GetBuzzerSeqCount(void)
{
    return 0; // 0表示无限循环
}

/**
 * @name      AppBeep_BuzzerCallback
 * @brief     蜂鸣器事件回调函数
 * @param     ptr - 蜂鸣器控制指针
 * @param     event - 蜂鸣器事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppBeep_BuzzerCallback(PushPullPtr_t ptr, PushPullEvent_e event, void* arg)
{
    (void)ptr;
    (void)arg;
    
    switch (event) {
        case PUSHPULL_EVENT_STATE_CHANGE:
        {
            // 状态切换事件
            // TODO: 可以添加状态切换时的处理逻辑
            break;
        }
        
        case PUSHPULL_EVENT_SEQUENCE_DONE:
        {
            // 序列完成事件（在无限循环模式下不会触发）
            // TODO: 可以添加序列完成时的处理逻辑
            break;
        }
        
        default:
            break;
    }
}

/**
 * @name      AppBeep_BuzzerWriteHigh
 * @brief     蜂鸣器写高电平函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    无
 */
void AppBeep_BuzzerWriteHigh(void* gpio_drv_ptr)
{
    (void)gpio_drv_ptr;
    HAL_Buzzer_On();
}

/**
 * @name      AppBeep_BuzzerWriteLow
 * @brief     蜂鸣器写低电平函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    无
 */
void AppBeep_BuzzerWriteLow(void* gpio_drv_ptr)
{
    (void)gpio_drv_ptr;
    HAL_Buzzer_Off();
}

/**
 * @name      AppBeep_BuzzerToggle
 * @brief     蜂鸣器翻转函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    无
 */
void AppBeep_BuzzerToggle(void* gpio_drv_ptr)
{
    (void)gpio_drv_ptr;
    // 翻转蜂鸣器状态
    if (g_buzzer && g_buzzer->state == PUSHPULL_STATE_LOW) {
        HAL_Buzzer_On();
        g_buzzer->state = PUSHPULL_STATE_HIGH;
    } else {
        HAL_Buzzer_Off();
        g_buzzer->state = PUSHPULL_STATE_LOW;
    }
}

/**
 * @name      AppBeep_BeepProcessCallback
 * @brief     蜂鸣器处理定时器回调函数
 * @param     user_data - 用户数据
 * @retval    无
 */
void AppBeep_BeepProcessCallback(void* user_data)
{
    (void)user_data;
    
    // 处理蜂鸣器状态机
    if (g_buzzer) {
        PushPull_FSM(g_buzzer);
    }
}

/**
 * @name      AppBeep_SetBeepProcessTimer
 * @brief     设置蜂鸣器处理定时器句柄（内部使用）
 * @param     timer_handle - 定时器句柄
 * @retval    无
 */
void AppBeep_SetBeepProcessTimer(SoftTimerHandle_t timer_handle)
{
    g_beep_process_timer = timer_handle;
}

/**
 * @name      AppBeep_StartBeep
 * @brief     开始蜂鸣器2秒周期循环
 * @param     无
 * @retval    无
 */
void AppBeep_StartBeep(void)
{
    if (g_buzzer) {
        // 设置2秒周期循环序列
        PushPull_SetSequence(g_buzzer, 
                            g_buzzer_seq_array, 
                            AppBeep_GetBuzzerSeqLength(), 
                            AppBeep_GetBuzzerSeqCount());
    }
}

/**
 * @name      AppBeep_StopBeep
 * @brief     停止蜂鸣器
 * @param     无
 * @retval    无
 */
void AppBeep_StopBeep(void)
{
    if (g_buzzer) {
        // 设置为手动模式并关闭蜂鸣器
        PushPull_SetMode(g_buzzer, PUSHPULL_MODE_MANUAL);
        HAL_Buzzer_Off();
        g_buzzer->state = PUSHPULL_STATE_LOW;
    }
}
