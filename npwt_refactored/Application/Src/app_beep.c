/**
  ******************************************************************************
  * @file:    app_beep.c
  * @author:  hupengpeng
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
static bool g_key_beep_active = false;         // 按键音是否正在播放
static bool g_key_beep_started = false;        // 按键音是否已经开始播放（用于区分初始状态和完成状态）
static bool g_beep_mute_enabled = false;       // 全局静音标志：true=静音，false=正常

// 2秒周期循环时序：开1秒，关1秒（转换为tick数，20ms为单位）
// 二维时序数组：支持多种不同的蜂鸣器模式
// 每行包含2个元素：[响铃时间, 静音时间] (单位：tick，20ms为单位)
static const uint16_t g_buzzer_seq_2d_array[][2] = {
    {8, 25},    // 模式0：响150ms，静500ms (150ms/20ms=8, 500ms/20ms=25)
    {8, 150},   // 模式1：响150ms，静3000ms (150ms/20ms=8, 3000ms/20ms=150)
    {8, 250},   // 模式2：响150ms，静5000ms (150ms/20ms=8, 5000ms/20ms=250)
    {2, 3}      // 模式3：按键音（40ms响，60ms停）(40ms/20ms=2, 60ms/20ms=3)
};

#define BUZZER_2D_COUNT   (sizeof(g_buzzer_seq_2d_array) / sizeof(g_buzzer_seq_2d_array[0]))
#define BUZZER_2D_MODE_KEY_BEEP  3  // 按键音模式索引

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
            // 如果按键音正在播放，播放完成后停止
            if (g_key_beep_active) {
                g_key_beep_active = false;
                AppBeep_StopBeep();
            }
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
    
    // 静音模式下不处理蜂鸣器状态机，并确保硬件关闭
    if (g_beep_mute_enabled) {
        if (g_buzzer) {
            HAL_Buzzer_Off();  // 确保硬件关闭
            g_buzzer->state = PUSHPULL_STATE_LOW;
        }
        return;
    }
    
    // 安全检查：确保蜂鸣器实例有效
    if (g_buzzer == NULL) {
        return;
    }
    
    // 安全检查：确保二维时序数组有效
    if (g_buzzer->seq_2d_array == NULL || g_buzzer->seq_2d_count == 0) {
        return;
    }
    
    // 保存执行前的状态
    uint32_t seq_index_before = g_buzzer->seq_index;
    uint32_t repeat_count_before = g_buzzer->seq_2d_repeat_count;
    
    // 安全检查：确保索引在有效范围内
    if (g_buzzer->seq_2d_index >= g_buzzer->seq_2d_count) {
        g_buzzer->seq_2d_index = 0;  // 重置索引
    }
    
    // 处理蜂鸣器状态机
    PushPull_FSM(g_buzzer);
    
    // 检查按键音是否已经开始播放
    if (g_key_beep_active && g_buzzer->seq_index != 0) {
        g_key_beep_started = true;  // 标记已开始播放
    }
    
    // 检查按键音是否播放完成
    // 按键音设置：seq_2d_count = 1, seq_2d_repeat_count = 1（只播放一次）
    // 完成标志：当 repeat_count 从 1 变成 0 时，FSM会立即重置为1，并重置 seq_index 为 0
    // 所以检查：如果 repeat_count_before == 1 且在FSM执行后 repeat_count == 1 且 seq_index == 0，
    // 且 seq_2d_index == 0，且已经开始播放，表示完成了一次循环并重新开始，此时应该停止
    if (g_key_beep_active && g_key_beep_started && g_buzzer->seq_2d_array != NULL && 
        g_buzzer->seq_2d_count == 1 && repeat_count_before == 1) {
        // 如果FSM执行后状态重置为初始状态，说明完成了一次循环
        if (g_buzzer->seq_index == 0 && g_buzzer->seq_2d_repeat_count == 1 &&
            g_buzzer->seq_2d_index == 0) {
            // 按键音播放完成（完成了一次循环并重新开始），立即停止
            g_key_beep_active = false;
            g_key_beep_started = false;
            AppBeep_StopBeep();
            return;  // 立即返回，防止FSM继续执行导致循环
        }
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
 * @name      AppBeep_StopBeep
 * @brief     停止蜂鸣器
 * @param     无
 * @retval    无
 * @remark    清空序列数组，防止FSM继续运行造成无限循环
 */
void AppBeep_StopBeep(void)
{
    if (g_buzzer) {
        // 清空序列数组，防止FSM继续运行
        PushPull_Set2DSequence(g_buzzer, NULL, 0, 0);
        
        // 设置为手动模式并关闭蜂鸣器
        PushPull_SetMode(g_buzzer, PUSHPULL_MODE_MANUAL);
        HAL_Buzzer_Off();
        g_buzzer->state = PUSHPULL_STATE_LOW;
    }
}

/**
 * @name      AppBeep_SetMute
 * @brief     设置静音状态
 * @param     mute - true=静音，false=正常
 * @retval    无
 * @remark    开启静音时立即停止当前蜂鸣器
 */
void AppBeep_SetMute(bool mute)
{
    g_beep_mute_enabled = mute;
    if (mute) {
        AppBeep_StopBeep();  // 开启静音时立即停止蜂鸣器
    }
}

/**
 * @name      AppBeep_IsMuted
 * @brief     获取静音状态
 * @param     无
 * @retval    true=静音，false=正常
 */
bool AppBeep_IsMuted(void)
{
    return g_beep_mute_enabled;
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
 * @name      AppBeep_StartBeep2DMode
 * @brief     启动蜂鸣器指定模式的二维时序
 * @param     mode_index - 模式索引（0-3）
 * @retval    无
 * @remark    启动指定模式的蜂鸣器，无限循环播放
 */
void AppBeep_StartBeep2DMode(uint32_t mode_index)
{
    if (g_buzzer == NULL) {
        return;
    }
    
    // 静音模式下不启动蜂鸣器
    if (g_beep_mute_enabled) {
        return;
    }
    
    // 检查模式索引是否有效
    if (mode_index >= BUZZER_2D_COUNT) {
        return;
    }
    
    // 设置二维时序数组
    PushPull_Set2DSequence(g_buzzer, 
                         AppBeep_GetBuzzer2DConfig(), 
                         AppBeep_GetBuzzer2DCount(), 
                         0); // 每行无限循环
    
    // 设置指定的模式索引
    g_buzzer->seq_2d_index = mode_index;
    
    // 重置当前模式的执行状态
    g_buzzer->seq_index = 0;
    g_buzzer->tick = 0;
    
    // 确保设置为序列模式
    PushPull_SetMode(g_buzzer, PUSHPULL_MODE_SEQUENCE);
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

/**
 * @name      AppBeep_BeepKey
 * @brief     播放按键音（40ms响，60ms停，单次播放）
 * @param     无
 * @retval    无
 * @remark    按键音会打断当前正在播放的蜂鸣器声音
 */
void AppBeep_BeepKey(void)
{
    if (g_buzzer == NULL) {
        return;
    }
    
    // 静音模式下不播放按键音
    if (g_beep_mute_enabled) {
        return;
    }
    
    // 先停止当前正在播放的蜂鸣器，确保按键音能立即播放
    AppBeep_StopBeep();
    
    // 创建指向按键音模式的指针数组（PushPull_Set2DSequence 需要二维数组指针）
    // 按键音模式在 g_buzzer_seq_2d_array 中的索引为 BUZZER_2D_MODE_KEY_BEEP (3)
    // 直接使用 g_buzzer_seq_2d_array，避免通过 AppBeep_GetBuzzer2DConfig() 获取指针
    static const uint16_t* key_beep_seq_ptr[1];
    key_beep_seq_ptr[0] = g_buzzer_seq_2d_array[BUZZER_2D_MODE_KEY_BEEP];
    
    // 设置按键音标志
    g_key_beep_active = true;
    g_key_beep_started = false;  // 重置开始标志
    
    // 设置按键音序列：只播放一次（repeat_count = 1）
    PushPull_Set2DSequence(g_buzzer, key_beep_seq_ptr, 1, 1);
    
    // 确保设置为序列模式
    PushPull_SetMode(g_buzzer, PUSHPULL_MODE_SEQUENCE);
}
