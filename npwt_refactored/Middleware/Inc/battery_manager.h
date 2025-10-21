/****************************************************************************
 * 文件名: battery_manager.h
 * 功能: 电池管理器
 * 
 * 说明: 
 *   监控电池电量和充电状态
 *   提供低电报警和自动关机保护
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef BATTERY_MANAGER_H
#define BATTERY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_types.h"

/****************************************************************************
 * 电池管理器函数
 ****************************************************************************/

/**
 * 函数: BatteryManager_Init
 * 功能: 初始化电池管理器
 */
void BatteryManager_Init(BatteryData_t *data);

/* 注意：BatteryManager_FirstCheck 已删除，使用 BatteryManager_Update 代替 */

/**
 * 函数: BatteryManager_Update
 * 功能: 更新电池状态（定期调用）
 * 返回: true=电池正常, false=电池耗尽需要关机
 */
bool BatteryManager_Update(BatteryData_t *data);

/**
 * 函数: BatteryManager_GetLevel
 * 功能: 获取电池电量等级
 * 返回: 0-4（0=没电，4=满电）
 */
uint8_t BatteryManager_GetLevel(const BatteryData_t *data);

/**
 * 函数: BatteryManager_IsCharging
 * 功能: 检查是否正在充电
 */
bool BatteryManager_IsCharging(const BatteryData_t *data);

/**
 * 函数: BatteryManager_ShouldShutdown
 * 功能: 判断是否应该自动关机
 * 返回: true=应该关机
 */
bool BatteryManager_ShouldShutdown(const BatteryData_t *data);

#endif /* BATTERY_MANAGER_H */

