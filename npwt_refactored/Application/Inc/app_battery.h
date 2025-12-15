/****************************************************************************
 * 文件名: app_battery.h
 * 功能: 电池管理应用层
 * 作者: 韦睿医疗
 * 说明: 
 *   电池电量检测和管理
 *   ADC采集、电量等级计算、防抖处理
 *   充电状态检测和显示管理
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#ifndef APP_BATTERY_H
#define APP_BATTERY_H

#include "../../Core/Inc/system_config.h"
//#include "../../Core/Inc/system_types.h"
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 电池电量等级枚举
 ****************************************************************************/
typedef enum {
    BATTERY_LEVEL_CRITICAL = 5,    // 严重低电（<3.5V，对应LOWER_THAN_3_5V）
    BATTERY_LEVEL_WARNING = 10,   // 低电警告（<3.6V，对应LOWER_BAT_WARN_3_6V）
    BATTERY_LEVEL_25 = 1,          // 25%（对应LOWER_THAN_3_7V）
    BATTERY_LEVEL_50 = 2,          // 50%（对应LOWER_THAN_3_8V）
    BATTERY_LEVEL_75 = 3,          // 75%（对应LOWER_THAN_4V）
    BATTERY_LEVEL_FULL = 4         // 100%（对应BAT_FULL）
} BatteryLevel_e;

/****************************************************************************
 * 电池管理接口函数
 ****************************************************************************/

/**
 * @name      AppBattery_Init
 * @brief     初始化电池管理模块
 * @param     无
 * @retval    无
 */
void AppBattery_Init(void);

/**
 * @name      AppBattery_Process
 * @brief     电池管理处理函数（在定时器中调用，每10ms）
 * @param     user_data - 用户数据（定时器回调参数）
 * @retval    无
 */
void AppBattery_Process(void* user_data);

/**
 * @name      AppBattery_UpdateADC
 * @brief     更新电池ADC值（由ADC采集回调调用）
 * @param     adc_value - ADC采样值（10位：0-1023）
 * @retval    无
 */
void AppBattery_UpdateADC(uint16_t adc_value);

/**
 * @name      AppBattery_GetLevel
 * @brief     获取当前电池电量百分比（0-100）
 * @param     无
 * @retval    电池电量百分比（0-100）
 */
uint8_t AppBattery_GetLevel(void);

/**
 * @name      AppBattery_GetLevelEnum
 * @brief     获取电池电量等级枚举值（用于显示判断）
 * @param     无
 * @retval    电池电量等级枚举值
 */
BatteryLevel_e AppBattery_GetLevelEnum(void);

/**
 * @name      AppBattery_IsCharging
 * @brief     获取充电状态
 * @param     无
 * @retval    true=充电中, false=未充电
 */
bool AppBattery_IsCharging(void);

/**
 * @name      AppBattery_GetADCValue
 * @brief     获取电池ADC原始值（用于调试）
 * @param     无
 * @retval    ADC采样值（10位：0-1023）
 */
// uint16_t AppBattery_GetADCValue(void);

/**
 * @name      AppBattery_IsLevelChanged
 * @brief     检查电池电量等级是否发生变化（用于触发显示更新）
 * @param     无
 * @retval    true=已变化, false=未变化
 */
bool AppBattery_IsLevelChanged(void);

#endif /* APP_BATTERY_H */

