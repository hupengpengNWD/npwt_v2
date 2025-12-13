/****************************************************************************
 * 文件名: app_alarm.h
 * 功能: 报警管理应用层
 * 
 * 说明: 
 *   统一管理各类报警功能
 *   包括电池报警、故障报警等
 * 
 * 创建日期: 2025-01-XX
 ****************************************************************************/

#ifndef APP_ALARM_H
#define APP_ALARM_H

#include "../../Core/Inc/system_config.h"
#include "../../Core/Inc/system_types.h"
#include "../../Core/Inc/system_enums.h"  // 使用系统定义的 AlarmType_e
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 报警管理接口函数
 ****************************************************************************/

/**
 * @name      AppAlarm_Init
 * @brief     初始化报警管理模块
 * @param     无
 * @retval    无
 */
void AppAlarm_Init(void);

/**
 * @name      AppAlarm_Process
 * @brief     报警处理函数（在主循环10ms任务中调用）
 * @param     无
 * @retval    无
 * @note      检测报警条件，更新报警状态，处理报警逻辑
 */
void AppAlarm_Process(void);

/**
 * @name      AppAlarm_GetActiveAlarm
 * @brief     获取当前激活的报警类型
 * @param     无
 * @retval    当前激活的报警类型（ALARM_TYPE_NONE表示无报警）
 */
// AlarmType_e AppAlarm_GetActiveAlarm(void);

/**
 * @name      AppAlarm_IsCriticalBatteryActive
 * @brief     查询是否处于严重低电报警状态
 * @param     无
 * @retval    true=处于严重低电报警状态, false=未处于报警状态
 */
bool AppAlarm_IsCriticalBatteryActive(void);

#endif /* APP_ALARM_H */

