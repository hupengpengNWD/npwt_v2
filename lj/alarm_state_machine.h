/****************************************************************************
 * 文件名: alarm_state_machine.h
 * 功能: 复杂蜂鸣器状态机（对应旧代码BEE_TWO）
 * 
 * 说明: 
 *   实现复杂的蜂鸣器控制状态机
 *   支持多状态声音报警序列
 *   包含超时保护和按键音分离
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef ALARM_STATE_MACHINE_H
#define ALARM_STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_enums.h"  // 使用统一的枚举定义

/* 注意：BuzzerState_e 已在 system_enums.h 中定义 */

/* 蜂鸣器控制结构体 */
typedef struct {
	BuzzerState_e current_state;  // 当前状态
	uint16_t audio_basic;         // 声音基础计数
	uint16_t audio_period;        // 声音周期计数
	uint16_t speaker_delay;       // 扬声器延时计数器
	bool speaker_state;           // 扬声器状态
	bool mute_enabled;            // 静音使能
	uint16_t max_beep_time;       // 最大响铃时间
} AlarmStateMachine_t;

/****************************************************************************
 * 蜂鸣器状态机函数
 ****************************************************************************/

/**
 * 函数: AlarmStateMachine_Init
 * 功能: 初始化蜂鸣器状态机
 */
void AlarmStateMachine_Init(AlarmStateMachine_t *machine);

/**
 * 函数: AlarmStateMachine_Process
 * 功能: 处理蜂鸣器状态机（主循环调用）
 */
void AlarmStateMachine_Process(AlarmStateMachine_t *machine);

/**
 * 函数: AlarmStateMachine_Start
 * 功能: 启动蜂鸣器报警
 */
void AlarmStateMachine_Start(AlarmStateMachine_t *machine);

/**
 * 函数: AlarmStateMachine_Stop
 * 功能: 停止蜂鸣器报警
 */
void AlarmStateMachine_Stop(AlarmStateMachine_t *machine);

/**
 * 函数: AlarmStateMachine_SetMute
 * 功能: 设置静音状态
 */
void AlarmStateMachine_SetMute(AlarmStateMachine_t *machine, bool mute);

/**
 * 函数: AlarmStateMachine_BeepKey
 * 功能: 按键音（短促单响）
 */
void AlarmStateMachine_BeepKey(AlarmStateMachine_t *machine);

#endif /* ALARM_STATE_MACHINE_H */

