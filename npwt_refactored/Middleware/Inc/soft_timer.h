/****************************************************************************
 * 文件名: soft_timer.h
 * 功能: 软件定时器组件头文件
 * 作者: 韦睿医疗
 * 说明: 
 *   基于Timer1的10ms中断实现软件定时器
 *   支持创建、销毁、开始、暂停、设置定时时间和回调函数
 *   参考APD项目的lib_soft_timer设计思路
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#ifndef SOFT_TIMER_H
#define SOFT_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 常量定义
 ****************************************************************************/
#define SOFT_TIMER_MAX_COUNT    12    // 最大定时器数量
#define SOFT_TIMER_TICK_MS      10   // 定时器精度：10ms

/****************************************************************************
 * 类型定义
 ****************************************************************************/

/* 定时器状态 */
typedef enum {
    SOFT_TIMER_STATE_STOPPED = 0,   // 已停止
    SOFT_TIMER_STATE_RUNNING,       // 运行中
    SOFT_TIMER_STATE_TIMEOUT,       // 已超时
    SOFT_TIMER_STATE_PAUSED         // 已暂停
} SoftTimerState_e;

/* 定时器模式 */
typedef enum {
    SOFT_TIMER_MODE_ONCE = 0,       // 单次模式
    SOFT_TIMER_MODE_PERIODIC        // 周期模式
} SoftTimerMode_e;

/* 定时器回调函数类型 */
typedef void (*SoftTimerCallback_t)(void* user_data);

/* 定时器句柄 */
typedef uint8_t SoftTimerHandle_t;

/* 定时器结构体 */
typedef struct {
    SoftTimerHandle_t handle;       // 定时器句柄
    SoftTimerState_e state;         // 当前状态
    SoftTimerMode_e mode;           // 定时器模式
    uint32_t period_ticks;         // 定时周期（10ms为单位）
    uint32_t remaining_ticks;       // 剩余时间（10ms为单位）
    SoftTimerCallback_t callback;   // 回调函数
    void* user_data;               // 用户数据
    bool is_used;                  // 是否被使用
} SoftTimer_t;

/****************************************************************************
 * 全局变量声明
 ****************************************************************************/
extern volatile uint32_t g_soft_timer_tick_count;  // 全局tick计数

/****************************************************************************
 * API函数声明
 ****************************************************************************/

/**
 * 函数: SoftTimer_Init
 * 功能: 初始化软件定时器模块
 * 参数: 无
 * 返回: 无
 * 说明: 必须在其他API调用前执行
 */
void SoftTimer_Init(void);

/**
 * 函数: SoftTimer_Create
 * 功能: 创建软件定时器
 * 参数: 
 *   - mode: 定时器模式（单次/周期）
 *   - period_ms: 定时周期（毫秒）
 *   - callback: 回调函数
 *   - user_data: 用户数据
 * 返回: SoftTimerHandle_t 定时器句柄，0表示失败
 * 说明: 创建后定时器处于停止状态
 */
SoftTimerHandle_t SoftTimer_Create(SoftTimerMode_e mode, 
                                   uint32_t period_ms,
                                   SoftTimerCallback_t callback, 
                                   void* user_data);

/**
 * 函数: SoftTimer_Destroy
 * 功能: 销毁软件定时器
 * 参数: handle - 定时器句柄
 * 返回: bool true-成功, false-失败
 * 说明: 销毁后句柄可重新使用
 */
bool SoftTimer_Destroy(SoftTimerHandle_t handle);

/**
 * 函数: SoftTimer_Start
 * 功能: 启动软件定时器
 * 参数: handle - 定时器句柄
 * 返回: bool true-成功, false-失败
 * 说明: 从当前时间开始计时
 */
bool SoftTimer_Start(SoftTimerHandle_t handle);

/**
 * 函数: SoftTimer_Stop
 * 功能: 停止软件定时器
 * 参数: handle - 定时器句柄
 * 返回: bool true-成功, false-失败
 * 说明: 停止后状态变为STOPPED
 */
bool SoftTimer_Stop(SoftTimerHandle_t handle);

/**
 * 函数: SoftTimer_Pause
 * 功能: 暂停软件定时器
 * 参数: handle - 定时器句柄
 * 返回: bool true-成功, false-失败
 * 说明: 暂停后状态变为PAUSED，剩余时间保持
 */
bool SoftTimer_Pause(SoftTimerHandle_t handle);

/**
 * 函数: SoftTimer_Resume
 * 功能: 恢复软件定时器
 * 参数: handle - 定时器句柄
 * 返回: bool true-成功, false-失败
 * 说明: 从暂停状态恢复运行
 */
bool SoftTimer_Resume(SoftTimerHandle_t handle);

/**
 * 函数: SoftTimer_SetPeriod
 * 功能: 设置定时器周期
 * 参数: 
 *   - handle: 定时器句柄
 *   - period_ms: 新的周期（毫秒）
 * 返回: bool true-成功, false-失败
 * 说明: 设置后定时器重新开始计时
 */
bool SoftTimer_SetPeriod(SoftTimerHandle_t handle, uint32_t period_ms);

/**
 * 函数: SoftTimer_SetCallback
 * 功能: 设置定时器回调函数
 * 参数: 
 *   - handle: 定时器句柄
 *   - callback: 回调函数
 *   - user_data: 用户数据
 * 返回: bool true-成功, false-失败
 * 说明: 可在运行时修改回调函数
 */
bool SoftTimer_SetCallback(SoftTimerHandle_t handle, 
                          SoftTimerCallback_t callback, 
                          void* user_data);

/**
 * 函数: SoftTimer_GetState
 * 功能: 获取定时器状态
 * 参数: handle - 定时器句柄
 * 返回: SoftTimerState_e 定时器状态
 * 说明: 返回当前定时器状态
 */
SoftTimerState_e SoftTimer_GetState(SoftTimerHandle_t handle);

/**
 * 函数: SoftTimer_GetRemainingTime
 * 功能: 获取剩余时间
 * 参数: handle - 定时器句柄
 * 返回: uint32_t 剩余时间（毫秒）
 * 说明: 返回定时器剩余时间
 */
uint32_t SoftTimer_GetRemainingTime(SoftTimerHandle_t handle);

/**
 * 函数: SoftTimer_GetTickCount
 * 功能: 获取全局tick计数
 * 参数: 无
 * 返回: uint32_t tick计数
 * 说明: 用于获取系统运行时间
 */
uint32_t SoftTimer_GetTickCount(void);

/**
 * 函数: SoftTimer_TickUpdate
 * 功能: Timer1中断调用，更新所有定时器
 * 参数: 无
 * 返回: 无
 * 说明: 在Timer1中断中调用，处理所有定时器状态机
 */
void SoftTimer_TickUpdate(void);

#endif /* SOFT_TIMER_H */
