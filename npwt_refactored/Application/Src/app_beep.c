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

// 2秒周期循环时序：开1秒，关1秒（转换为tick数，50ms为单位）
// 二维时序数组：支持多种不同的蜂鸣器模式
// 每行包含2个元素：[静音时间, 响铃时间] (单位：tick，50ms为单位)
static const uint16_t g_buzzer_seq_2d_array[][2] = {
    {3, 10},    // 模式0：静500ms，响150ms (500ms/50ms=10, 150ms/50ms=3)
    {3, 20},    // 模式1：静1000ms，响150ms (1000ms/50ms=20, 150ms/50ms=3)
    {3, 40},    // 模式2：静2000ms，响150ms (2000ms/50ms=40, 150ms/50ms=3)
    {3, 60},    // 模式3：静3000ms，响150ms (3000ms/50ms=60, 150ms/50ms=3)
    {3, 80},    // 模式4：静4000ms，响150ms (4000ms/50ms=80, 150ms/50ms=3)
    {3, 100}    // 模式5：静5000ms，响150ms (5000ms/50ms=100, 150ms/50ms=3)
};

#define BUZZER_2D_COUNT   (sizeof(g_buzzer_seq_2d_array) / sizeof(g_buzzer_seq_2d_array[0]))

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
 * @name      AppBeep_GetBuzzer2DConfig
 * @brief     获取蜂鸣器二维时序配置数组
 * @param     无
 * @retval    const uint16_t** - 二维时序数组指针
 */
const uint16_t** AppBeep_GetBuzzer2DConfig(void)
{
    // 创建指针数组，指向二维数组的每一行
    static const uint16_t* seq_pointers[BUZZER_2D_COUNT];
    static bool initialized = false;
    
    if (!initialized) {
        for (uint32_t i = 0; i < BUZZER_2D_COUNT; i++) {
            seq_pointers[i] = g_buzzer_seq_2d_array[i];
        }
        initialized = true;
    }
    
    return seq_pointers;
}

/**
 * @name      AppBeep_GetBuzzer2DCount
 * @brief     获取蜂鸣器二维数组行数
 * @param     无
 * @retval    uint32_t - 二维数组行数
 */
uint32_t AppBeep_GetBuzzer2DCount(void)
{
    return BUZZER_2D_COUNT;
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
 * @brief     蜂鸣器处理定时器回调函数（安全版本）
 * @param     user_data - 用户数据
 * @retval    无
 */
void AppBeep_BeepProcessCallback(void* user_data)
{
    (void)user_data;
    
    // 安全检查：确保蜂鸣器实例有效
    if (g_buzzer == NULL) {
        return;
    }
    
    // 安全检查：确保二维时序数组有效
    if (g_buzzer->seq_2d_array == NULL || g_buzzer->seq_2d_count == 0) {
        return;
    }
    
    // 安全检查：确保索引在有效范围内
    if (g_buzzer->seq_2d_index >= g_buzzer->seq_2d_count) {
        g_buzzer->seq_2d_index = 0;  // 重置索引
    }
    
    // 处理蜂鸣器状态机
    PushPull_FSM(g_buzzer);
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

/**
 * @name      AppBeep_StartBeep2D
 * @brief     开始蜂鸣器二维时序模式
 * @param     无
 * @retval    无
 */
void AppBeep_StartBeep2D(void)
{
    if (g_buzzer) {
        // 设置二维时序数组
        PushPull_Set2DSequence(g_buzzer, 
                             AppBeep_GetBuzzer2DConfig(), 
                             AppBeep_GetBuzzer2DCount(), 
                             0); // 每行无限循环
        // 确保设置为序列模式
        PushPull_SetMode(g_buzzer, PUSHPULL_MODE_SEQUENCE);
    }
}



/**
 * @name      AppBeep_SwitchToNext2DMode
 * @brief     切换到下一个二维模式
 * @param     无
 * @retval    无
 */
void AppBeep_SwitchToNext2DMode(void)
{
    if (g_buzzer) {
        // 确保是二维模式
        if (g_buzzer->seq_2d_array == NULL) {
            AppBeep_StartBeep2D();
        }
        
        // 切换到下一个模式
        g_buzzer->seq_2d_index++;
        if (g_buzzer->seq_2d_index >= g_buzzer->seq_2d_count) {
            g_buzzer->seq_2d_index = 0;
        }
        
        // 重置当前模式的执行状态
        g_buzzer->seq_index = 0;
        g_buzzer->tick = 0;
    }
}
