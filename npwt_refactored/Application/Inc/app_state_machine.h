/****************************************************************************
 * 文件名: app_state_machine.h
 * 功能: 应用状态机
 * 
 * 说明: 
 *   管理系统的工作模式和状态转换
 *   使用表驱动设计，清晰易维护
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef APP_STATE_MACHINE_H
#define APP_STATE_MACHINE_H

#include <stdint.h>
#include "../../Core/Inc/system_types.h"

/****************************************************************************
 * 状态机函数
 ****************************************************************************/

/**
 * 函数: AppStateMachine_Init
 * 功能: 初始化应用状态机
 * 参数: state - 系统状态指针
 */
void AppStateMachine_Init(SystemState_t *state);

/**
 * 函数: AppStateMachine_Run
 * 功能: 运行状态机（每个周期调用一次）
 * 参数: state - 系统状态指针
 */
void AppStateMachine_Run(SystemState_t *state);

/**
 * 函数: AppStateMachine_SetMode
 * 功能: 切换工作模式
 * 参数: state - 系统状态指针
 *       mode - 新模式
 */
void AppStateMachine_SetMode(SystemState_t *state, WorkMode_e mode);

/**
 * 函数: AppStateMachine_GetMode
 * 功能: 获取当前工作模式
 */
WorkMode_e AppStateMachine_GetMode(const SystemState_t *state);

/**
 * 函数: AppStateMachine_HandleKey
 * 功能: 处理按键事件（根据当前模式分发按键）
 * 参数: sys - 系统状态指针
 *       key - 按键值
 *       event - 按键事件
 */
void AppStateMachine_HandleKey(SystemState_t *sys, KeyValue_e key, KeyEvent_e event);

#endif /* APP_STATE_MACHINE_H */

