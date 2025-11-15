/****************************************************************************
 * 文件名: app_battery.c
 * 功能: 电池管理应用层实现
 * 
 * 说明: 
 *   参考未重构工程的实现方式
 *   - ADC采集频率：每500ms一次（低频采集）
 *   - 滤波方式：10次滑动平均（在HAL层完成）
 *   - 防抖处理：连续10次相同才更新（1秒@100ms周期）
 *   - 电量等级：根据ADC阈值转换为枚举值
 * 
 * 创建日期: 2025-01-XX
 ****************************************************************************/

#include "../Inc/app_battery.h"
#include "../../HAL/Inc/hal_adc.h"
#include "../../HAL/Inc/hal_gpio.h"
#include <stdbool.h>

/****************************************************************************
 * 常量定义
 ****************************************************************************/
#define BATTERY_STABLE_COUNT    30    // 防抖次数：10次 × 100ms = 1秒

/****************************************************************************
 * 内部变量
 ****************************************************************************/

/* 电池ADC原始值 */
static uint16_t g_battery_adc = BAT_LEVEL_FULL;  // 初始化为满电

/* 电池电量等级（临时值，用于防抖） */
static BatteryLevel_e g_battery_level_temp = BATTERY_LEVEL_FULL;

/* 电池电量等级（稳定值，防抖后） */
static BatteryLevel_e g_battery_level_stable = BATTERY_LEVEL_FULL;

/* 防抖计数器 */
static uint16_t g_battery_stable_counter = 0;

/* 上次的充电状态 */
static bool g_last_charging_state = false;

/* 电量等级变化标志 */
static bool g_battery_level_changed = false;

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/

/**
 * @name      BatteryLevel_Calculate
 * @brief     根据ADC值计算电量等级
 * @param     adc_bat - ADC采样值
 * @retval    电量等级枚举值
 */
static BatteryLevel_e BatteryLevel_Calculate(uint16_t adc_bat);

/**
 * @name      BatteryLevel_ToPercent
 * @brief     将电量等级枚举转换为百分比
 * @param     level - 电量等级枚举值
 * @retval    百分比（0-100）
 */
static uint8_t BatteryLevel_ToPercent(BatteryLevel_e level);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      AppBattery_Init
 * @brief     初始化电池管理模块
 */
void AppBattery_Init(void)
{
    /* 初始化变量 */
    g_battery_adc = BAT_LEVEL_FULL;
    g_battery_level_temp = BATTERY_LEVEL_FULL;
    g_battery_level_stable = BATTERY_LEVEL_FULL;
    g_battery_stable_counter = 0;
    g_last_charging_state = false;
    g_battery_level_changed = false;
    
    /* 读取初始充电状态 */
    g_last_charging_state = HAL_Battery_IsCharging();
}

/**
 * @name      AppBattery_Process
 * @brief     电池管理处理函数（每100ms调用一次）
 * @param     user_data - 用户数据（定时器回调参数，未使用）
 * @note      处理防抖逻辑和充电状态检测
 */
void AppBattery_Process(void* user_data)
{
    (void)user_data;  // 未使用，消除警告
    /* 根据当前ADC值计算电量等级 */
    BatteryLevel_e new_level = BatteryLevel_Calculate(g_battery_adc);
    
    /* 防抖处理：连续10次相同才更新（1秒@100ms周期） */
    if (new_level == g_battery_level_temp) {
        /* 连续相同，增加计数 */
        if (++g_battery_stable_counter >= BATTERY_STABLE_COUNT) {
            /* 电量等级发生变化 */
            if (g_battery_level_stable != new_level) {
                g_battery_level_stable = new_level;
                g_battery_level_changed = true;  // 设置变化标志
            }
            g_battery_stable_counter = BATTERY_STABLE_COUNT;  // 防止溢出
        }
    } else {
        /* 发生变化，重置计数 */
        g_battery_level_temp = new_level;
        g_battery_stable_counter = 0;
    }
    
    /* 检测充电状态变化 */
    bool current_charging = HAL_Battery_IsCharging();
    if (current_charging != g_last_charging_state) {
        g_last_charging_state = current_charging;
        g_battery_level_changed = true;  // 充电状态变化也需要更新显示
    }
}

/**
 * @name      AppBattery_UpdateADC
 * @brief     更新电池ADC值（由ADC采集回调调用）
 * @param     adc_value - ADC采样值（10位：0-1023）
 */
void AppBattery_UpdateADC(uint16_t adc_value)
{
    g_battery_adc = adc_value;
}

/**
 * @name      AppBattery_GetLevel
 * @brief     获取当前电池电量百分比（0-100）
 */
uint8_t AppBattery_GetLevel(void)
{
    return BatteryLevel_ToPercent(g_battery_level_stable);
}

/**
 * @name      AppBattery_GetLevelEnum
 * @brief     获取电池电量等级枚举值
 */
BatteryLevel_e AppBattery_GetLevelEnum(void)
{
    return g_battery_level_stable;
}

/**
 * @name      AppBattery_IsCharging
 * @brief     获取充电状态
 */
bool AppBattery_IsCharging(void)
{
    return HAL_Battery_IsCharging();
}

/**
 * @name      AppBattery_GetADCValue
 * @brief     获取电池ADC原始值（用于调试）
 */
uint16_t AppBattery_GetADCValue(void)
{
    return g_battery_adc;
}

/**
 * @name      AppBattery_IsLevelChanged
 * @brief     检查电池电量等级是否发生变化
 * @note      调用后会自动清除变化标志
 */
bool AppBattery_IsLevelChanged(void)
{
    bool changed = g_battery_level_changed;
    g_battery_level_changed = false;  // 清除标志
    return changed;
}

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * @name      BatteryLevel_Calculate
 * @brief     根据ADC值计算电量等级（与未重构工程一致）
 */
static BatteryLevel_e BatteryLevel_Calculate(uint16_t adc_bat)
{
    /* 参考未重构工程的阈值定义 */
    if (adc_bat < BAT_LEVEL_LOW) {           // < 269
        return BATTERY_LEVEL_CRITICAL;       // 严重低电（<3.5V）
    } else if (adc_bat < BAT_LEVEL_25) {    // < 277
        return BATTERY_LEVEL_WARNING;        // 低电警告（<3.6V）
    } else if (adc_bat < BAT_LEVEL_50) {    // < 296
        return BATTERY_LEVEL_25;             // 25%（<3.7V）
    } else if (adc_bat < BAT_LEVEL_75) {    // < 304
        return BATTERY_LEVEL_50;             // 50%（<3.8V）
    } else if (adc_bat < BAT_LEVEL_FULL) {  // < 319
        return BATTERY_LEVEL_75;             // 75%（<4.0V）
    } else {
        return BATTERY_LEVEL_FULL;           // 100%（>=4.0V）
    }
}

/**
 * @name      BatteryLevel_ToPercent
 * @brief     将电量等级枚举转换为百分比
 */
static uint8_t BatteryLevel_ToPercent(BatteryLevel_e level)
{
    switch (level) {
        case BATTERY_LEVEL_CRITICAL:
        case BATTERY_LEVEL_WARNING:
            return 0;   // 低电和严重低电显示为0%
        case BATTERY_LEVEL_25:
            return 25;
        case BATTERY_LEVEL_50:
            return 50;
        case BATTERY_LEVEL_75:
            return 75;
        case BATTERY_LEVEL_FULL:
            return 100;
        default:
            return 0;
    }
}

