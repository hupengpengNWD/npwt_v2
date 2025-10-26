/****************************************************************************
 * 文件名: alarm_manager.h
 * 功能: 报警管理器
 * 
 * 说明: 
 *   管理声音和LED报警
 *   支持静音功能
 *   不同故障有不同的报警模式
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef ALARM_MANAGER_H
#define ALARM_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_types.h"

/* 注意：AlarmType_e 已在 system_enums.h 中定义 */

/****************************************************************************
 * 报警管理器函数
 ****************************************************************************/

/**
 * 函数: AlarmManager_Init
 * 功能: 初始化报警管理器
 */
void AlarmManager_Init(AlarmData_t *data);

/**
 * 函数: AlarmManager_Update
 * 功能: 更新报警状态（主循环调用）
 * 参数: data - 报警数据
 *       battery - 电池数据
 *       fault - 故障数据
 */
void AlarmManager_Update(AlarmData_t *data,
                        const BatteryData_t *battery,
                        const FaultData_t *fault);

/**
 * 函数: AlarmManager_SetMute
 * 功能: 设置/取消静音
 * 参数: mute - true=静音, false=取消静音
 */
void AlarmManager_SetMute(AlarmData_t *data, bool mute);

/**
 * 函数: AlarmManager_IsMuted
 * 功能: 检查是否静音
 */
bool AlarmManager_IsMuted(const AlarmData_t *data);

/**
 * 函数: AlarmManager_CancelMute
 * 功能: 自动取消静音（超时后）
 */
void AlarmManager_CancelMute(AlarmData_t *data);

/**
 * 函数: AlarmManager_PlayBeep
 * 功能: 播放蜂鸣器（内部调用）
 */
void AlarmManager_PlayBeep(AlarmData_t *data);

/**
 * 函数: AlarmManager_Beep
 * 功能: 播放指定次数的蜂鸣音（用于按键音和提示音）
 * 参数: times - 蜂鸣次数（1=短音, 2=双音, 3=三音）
 */
void AlarmManager_Beep(uint8_t times);

#endif /* ALARM_MANAGER_H */

