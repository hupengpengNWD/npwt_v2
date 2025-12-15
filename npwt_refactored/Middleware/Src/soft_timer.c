/****************************************************************************
 * 文件名: soft_timer.c
 * 功能: 软件定时器组件实现
 * 作者: 韦睿医疗
 * 说明: 
 *   基于Timer1的10ms中断实现软件定时器
 *   支持创建、销毁、开始、暂停、设置定时时间和回调函数
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#include "soft_timer.h"

#ifndef NULL
#define NULL ((void*)0)
#endif

/****************************************************************************
 * 全局变量定义
 ****************************************************************************/
volatile uint32_t g_soft_timer_tick_count = 0;  // 全局tick计数

static SoftTimer_t g_timers[SOFT_TIMER_MAX_COUNT];  // 定时器数组
static bool g_module_initialized = false;         // 模块初始化标志

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/
static SoftTimer_t* SoftTimer_GetTimer(SoftTimerHandle_t handle);
static SoftTimerHandle_t SoftTimer_FindFreeHandle(void);
static void SoftTimer_ProcessTimer(SoftTimer_t* timer);

/****************************************************************************
 * API函数实现
 ****************************************************************************/

/**
 * 函数: SoftTimer_Init
 * 功能: 初始化软件定时器模块
 */
void SoftTimer_Init(void)
{
    if (g_module_initialized) {
        return;
    }
    
    // 初始化所有定时器
    for (uint8_t i = 0; i < SOFT_TIMER_MAX_COUNT; i++) {
        g_timers[i].handle = i + 1;  // 句柄从1开始，0表示无效
        g_timers[i].state = SOFT_TIMER_STATE_STOPPED;
        g_timers[i].mode = SOFT_TIMER_MODE_ONCE;
        g_timers[i].period_ticks = 0;
        g_timers[i].remaining_ticks = 0;
        g_timers[i].callback = NULL;
        g_timers[i].user_data = NULL;
        g_timers[i].is_used = false;
    }
    
    g_soft_timer_tick_count = 0;
    g_module_initialized = true;
}

/**
 * 函数: SoftTimer_Create
 * 功能: 创建软件定时器
 */
SoftTimerHandle_t SoftTimer_Create(SoftTimerMode_e mode, 
                                   uint32_t period_ms,
                                   SoftTimerCallback_t callback, 
                                   void* user_data)
{
    if (!g_module_initialized) {
        return 0;
    }
    
    SoftTimerHandle_t handle = SoftTimer_FindFreeHandle();
    if (handle == 0) {
        return 0;  // 没有空闲句柄
    }
    
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL) {
        return 0;
    }
    
    // 配置定时器
    timer->mode = mode;
    timer->period_ticks = (period_ms + SOFT_TIMER_TICK_MS - 1) / SOFT_TIMER_TICK_MS;  // 向上取整
    timer->remaining_ticks = timer->period_ticks;
    timer->callback = callback;
    timer->user_data = user_data;
    timer->state = SOFT_TIMER_STATE_STOPPED;
    timer->is_used = true;
    
    return handle;
}

/**
 * 函数: SoftTimer_Destroy
 * 功能: 销毁软件定时器
 */
bool SoftTimer_Destroy(SoftTimerHandle_t handle)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return false;
    }
    
    // 停止定时器
    timer->state = SOFT_TIMER_STATE_STOPPED;
    timer->is_used = false;
    timer->callback = NULL;
    timer->user_data = NULL;
    
    return true;
}

/**
 * 函数: SoftTimer_Start
 * 功能: 启动软件定时器
 */
bool SoftTimer_Start(SoftTimerHandle_t handle)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return false;
    }
    
    timer->remaining_ticks = timer->period_ticks;
    timer->state = SOFT_TIMER_STATE_RUNNING;
    
    return true;
}

/**
 * 函数: SoftTimer_Stop
 * 功能: 停止软件定时器
 */
bool SoftTimer_Stop(SoftTimerHandle_t handle)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return false;
    }
    
    timer->state = SOFT_TIMER_STATE_STOPPED;
    timer->remaining_ticks = 0;
    
    return true;
}

/**
 * 函数: SoftTimer_Pause
 * 功能: 暂停软件定时器
 */
bool SoftTimer_Pause(SoftTimerHandle_t handle)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return false;
    }
    
    if (timer->state == SOFT_TIMER_STATE_RUNNING) {
        timer->state = SOFT_TIMER_STATE_PAUSED;
        return true;
    }
    
    return false;
}

/**
 * 函数: SoftTimer_Resume
 * 功能: 恢复软件定时器
 */
bool SoftTimer_Resume(SoftTimerHandle_t handle)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return false;
    }
    
    if (timer->state == SOFT_TIMER_STATE_PAUSED) {
        timer->state = SOFT_TIMER_STATE_RUNNING;
        return true;
    }
    
    return false;
}

/**
 * 函数: SoftTimer_SetPeriod
 * 功能: 设置定时器周期
 */
bool SoftTimer_SetPeriod(SoftTimerHandle_t handle, uint32_t period_ms)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return false;
    }
    
    timer->period_ticks = (period_ms + SOFT_TIMER_TICK_MS - 1) / SOFT_TIMER_TICK_MS;
    
    // 如果定时器正在运行，重新开始计时
    if (timer->state == SOFT_TIMER_STATE_RUNNING) {
        timer->remaining_ticks = timer->period_ticks;
    }
    
    return true;
}

/**
 * 函数: SoftTimer_SetCallback
 * 功能: 设置定时器回调函数
 */
bool SoftTimer_SetCallback(SoftTimerHandle_t handle, 
                          SoftTimerCallback_t callback, 
                          void* user_data)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return false;
    }
    
    timer->callback = callback;
    timer->user_data = user_data;
    
    return true;
}

/**
 * 函数: SoftTimer_GetState
 * 功能: 获取定时器状态
 */
SoftTimerState_e SoftTimer_GetState(SoftTimerHandle_t handle)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return SOFT_TIMER_STATE_STOPPED;
    }
    
    return timer->state;
}

/**
 * 函数: SoftTimer_GetRemainingTime
 * 功能: 获取剩余时间
 */
uint32_t SoftTimer_GetRemainingTime(SoftTimerHandle_t handle)
{
    SoftTimer_t* timer = SoftTimer_GetTimer(handle);
    if (timer == NULL || !timer->is_used) {
        return 0;
    }
    
    return timer->remaining_ticks * SOFT_TIMER_TICK_MS;
}

/**
 * 函数: SoftTimer_GetTickCount
 * 功能: 获取全局tick计数
 */
uint32_t SoftTimer_GetTickCount(void)
{
    return g_soft_timer_tick_count;
}

/**
 * 函数: SoftTimer_TickUpdate
 * 功能: Timer1中断调用，更新所有定时器
 */
void SoftTimer_TickUpdate(void)
{
    if (!g_module_initialized) {
        return;
    }
    
    g_soft_timer_tick_count++;
    
    // 处理所有定时器
    for (uint8_t i = 0; i < SOFT_TIMER_MAX_COUNT; i++) {
        if (g_timers[i].is_used) {
            SoftTimer_ProcessTimer(&g_timers[i]);
        }
    }
}

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * 函数: SoftTimer_GetTimer
 * 功能: 根据句柄获取定时器指针
 */
static SoftTimer_t* SoftTimer_GetTimer(SoftTimerHandle_t handle)
{
    if (handle == 0 || handle > SOFT_TIMER_MAX_COUNT) {
        return NULL;
    }
    
    return &g_timers[handle - 1];
}

/**
 * 函数: SoftTimer_FindFreeHandle
 * 功能: 查找空闲的定时器句柄
 */
static SoftTimerHandle_t SoftTimer_FindFreeHandle(void)
{
    for (uint8_t i = 0; i < SOFT_TIMER_MAX_COUNT; i++) {
        if (!g_timers[i].is_used) {
            return i + 1;  // 句柄从1开始
        }
    }
    
    return 0;  // 没有空闲句柄
}

/**
 * 函数: SoftTimer_ProcessTimer
 * 功能: 处理单个定时器的状态机
 */
static void SoftTimer_ProcessTimer(SoftTimer_t* timer)
{
    if (timer == NULL || !timer->is_used) {
        return;
    }
    
    switch (timer->state) {
        case SOFT_TIMER_STATE_RUNNING:
        {
            if (timer->remaining_ticks > 0) {
                timer->remaining_ticks--;
            }
            
            if (timer->remaining_ticks == 0) {
                timer->state = SOFT_TIMER_STATE_TIMEOUT;
                
                // 调用回调函数
                if (timer->callback != NULL) {
                    timer->callback(timer->user_data);
                }
                
                // 若回调未改变状态，则根据模式处理超时
                if (timer->state == SOFT_TIMER_STATE_TIMEOUT) {
                if (timer->mode == SOFT_TIMER_MODE_ONCE) {
                    timer->state = SOFT_TIMER_STATE_STOPPED;
                } else if (timer->mode == SOFT_TIMER_MODE_PERIODIC) {
                    timer->remaining_ticks = timer->period_ticks;
                    timer->state = SOFT_TIMER_STATE_RUNNING;
                    }
                }
            }
            break;
        }
        
        case SOFT_TIMER_STATE_PAUSED:
        case SOFT_TIMER_STATE_STOPPED:
        case SOFT_TIMER_STATE_TIMEOUT:
        default:
            // 这些状态不需要处理
            break;
    }
}
