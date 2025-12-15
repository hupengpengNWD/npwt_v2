/****************************************************************************
 * 文件名: app_battery.c
 * 功能: 电池管理应用层实现
 * 作者: 韦睿医疗
 * 说明: 
 *   参考老版本工程的实现方式
 *   - ADC采集频率：每500ms一次（低频采集）
 *   - 滤波方式：10次滑动平均（在HAL层完成）
 *   - 防抖处理：连续10次相同才更新（1秒@100ms周期）
 *   - 电量等级：根据ADC阈值转换为枚举值
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#include "../Inc/app_battery.h"
#include "../Inc/app_pressure.h"
#include "../../HAL/Inc/hal_adc.h"
#include "../../HAL/Inc/hal_gpio.h"
#include <stdbool.h>

/****************************************************************************
 * 常量定义
 ****************************************************************************/
#define BATTERY_STABLE_COUNT           10    // 防抖次数：10次 × 100ms = 1秒
#define BATTERY_LOAD_COMPENSATION_ADC  10    // 工作模式下负载补偿ADC值（约0.1V的压降补偿）

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
 * @param     is_working_mode - 是否在工作模式（连续或间歇治疗模式）
 * @retval    电量等级枚举值
 * @note      工作模式下，所有阈值降低BATTERY_LOAD_COMPENSATION_ADC，以补偿负载导致的电压下降
 */
static BatteryLevel_e BatteryLevel_Calculate(uint16_t adc_bat, bool is_working_mode);

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
    /* 检查电机是否实际正在运行（有负载时电压会下降，需要补偿） */
    bool is_motor_running = AppPressure_IsMotorRunning();
    /* 
     * 组合检查：只有当控制启用且电机实际运行时，才应用负载补偿
     * - 治疗模式 + 电机运行 → 有负载，需要补偿
     * - 治疗模式 + 电机停止（压力达标） → 无负载，不需要补偿
     * - 待机/暂停模式 → 无负载，不需要补偿
     */
    bool is_working_mode = AppPressure_IsControlEnabled() && is_motor_running;
    /* 根据当前ADC值和工作状态计算电量等级 */
    BatteryLevel_e new_level = BatteryLevel_Calculate(g_battery_adc, is_working_mode);
    
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
// uint16_t AppBattery_GetADCValue(void)
// {
//     return g_battery_adc;
// }

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
 * @brief     根据ADC值计算电量等级（与老版本工程一致）
 * @param     adc_bat - ADC采样值
 * @param     is_working_mode - 是否在工作模式（连续或间歇治疗模式）
 * @retval    电量等级枚举值
 * @note      工作模式下，所有阈值降低BATTERY_LOAD_COMPENSATION_ADC，以补偿负载导致的电压下降
 *           例如：工作模式下，BAT_LEVEL_75从304降到294，更准确反映真实电量
 */
static BatteryLevel_e BatteryLevel_Calculate(uint16_t adc_bat, bool is_working_mode)
{
    /* 分离“严重低电报警阈值”和“低电闪烁阈值” */
    uint16_t threshold_critical;  /* 严重低电报警阈值（独立宏） */
    uint16_t threshold_low;       /* 低电（0格/闪烁）阈值，沿用 BAT_LEVEL_LOW */
    uint16_t threshold_25;
    uint16_t threshold_50;
    uint16_t threshold_75;
    uint16_t threshold_full;

    /* 根据工作状态调整阈值 */
    if (is_working_mode) {
        /* 工作模式下，所有阈值降低BATTERY_LOAD_COMPENSATION_ADC，以补偿负载压降 */
        threshold_critical = BAT_LEVEL_CRITICAL - BATTERY_LOAD_COMPENSATION_ADC; // 265 -> 255
        threshold_low  = BAT_LEVEL_LOW  - BATTERY_LOAD_COMPENSATION_ADC;             // 269 -> 259
        threshold_25   = BAT_LEVEL_25   - BATTERY_LOAD_COMPENSATION_ADC;   // 277 -> 267
        threshold_50   = BAT_LEVEL_50   - BATTERY_LOAD_COMPENSATION_ADC;   // 296 -> 286
        threshold_75   = BAT_LEVEL_75   - BATTERY_LOAD_COMPENSATION_ADC;   // 304 -> 294
        threshold_full = BAT_LEVEL_FULL - BATTERY_LOAD_COMPENSATION_ADC;   // 319 -> 309
    } else {
        /* 非工作模式下，使用原始阈值 */
        threshold_critical = BAT_LEVEL_CRITICAL;
        threshold_low  = BAT_LEVEL_LOW;
        threshold_25   = BAT_LEVEL_25;
        threshold_50   = BAT_LEVEL_50;
        threshold_75   = BAT_LEVEL_75;
        threshold_full = BAT_LEVEL_FULL;
    }

    /* 判定顺序：优先严重低电 → 低电警告 → 25% → 50% → 75% → 满电 */
    if (adc_bat < threshold_critical) {
        return BATTERY_LEVEL_CRITICAL;       /* 严重低电报警（关机前10s显示） */
    } else if (adc_bat < threshold_low) {
        return BATTERY_LEVEL_WARNING;        // 低电警告（<3.6V）
    } else if (adc_bat < threshold_25) {
        return BATTERY_LEVEL_25;             // 25%（<3.7V）
    } else if (adc_bat < threshold_50) {
        return BATTERY_LEVEL_50;             // 50%（<3.8V）
    } else if (adc_bat < threshold_75) {
        return BATTERY_LEVEL_75;             // 75%（<4.0V）
    } else if (adc_bat < threshold_full) {
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

