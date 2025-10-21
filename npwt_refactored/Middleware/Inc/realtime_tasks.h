/****************************************************************************
 * 文件名: realtime_tasks.h
 * 功能: 实时任务管理
 * 
 * 说明: 
 *   管理在Timer3中断中执行的实时任务
 *   - ADC采样
 *   - 压力控制
 *   - PWM生成
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef REALTIME_TASKS_H
#define REALTIME_TASKS_H

#include "../../Core/Inc/system_types.h"

/**
 * 函数: RealtimeTasks_Init
 * 功能: 初始化实时任务（注册中断回调）
 */
void RealtimeTasks_Init(void);

/**
 * 函数: RealtimeTasks_SetPumpEnable
 * 功能: 设置气泵使能状态
 * 参数: enable - true=允许气泵工作, false=禁止
 */
void RealtimeTasks_SetPumpEnable(bool enable);

/**
 * 函数: RealtimeTasks_SetPWMDuty
 * 功能: 设置PWM占空比
 * 参数: duty - 占空比（0-10）
 */
void RealtimeTasks_SetPWMDuty(uint8_t duty);

#endif /* REALTIME_TASKS_H */

