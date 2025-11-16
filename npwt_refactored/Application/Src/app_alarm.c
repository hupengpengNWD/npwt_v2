/****************************************************************************
 * 文件名: app_alarm.c
 * 功能: 报警管理应用层实现
 * 
 * 说明: 
 *   统一管理各类报警功能
 *   目前实现电池严重低电报警功能
 * 
 * 创建日期: 2025-01-XX
 ****************************************************************************/

#include "../Inc/app_alarm.h"
#include "../Inc/app_battery.h"
#include "../Inc/app_pressure.h"
#include "../../Middleware/Inc/display.h"
#include "../../Middleware/Inc/soft_timer.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Library/lcd_icon_data.h"

/****************************************************************************
 * 常量定义
 ****************************************************************************/
#define SHUTDOWN_DELAY_MS    10000U   // 关机延迟时间：10秒
#define SHUTDOWN_DELAY_TICKS (SHUTDOWN_DELAY_MS / SOFT_TIMER_TICK_MS)  // 1000个tick

/****************************************************************************
 * 私有变量
 ****************************************************************************/

/* 报警状态 */
static AlarmType_e g_alarm_state = ALARM_TYPE_NONE;

/* 关机定时器句柄 */
static SoftTimerHandle_t g_shutdown_timer = 0;

/* 图标是否已显示标志 */
static bool g_icon_displayed = false;

/****************************************************************************
 * 私有函数声明
 ****************************************************************************/

/**
 * @name      AppAlarm_ShutdownCallback
 * @brief     关机定时器回调函数
 * @param     user_data - 用户数据（未使用）
 * @retval    无
 */
static void AppAlarm_ShutdownCallback(void* user_data);

/**
 * @name      AppAlarm_EnterCriticalBatteryAlarm
 * @brief     进入严重低电报警状态
 * @param     无
 * @retval    无
 */
static void AppAlarm_EnterCriticalBatteryAlarm(void);

/**
 * @name      AppAlarm_ExitCriticalBatteryAlarm
 * @brief     退出严重低电报警状态
 * @param     无
 * @retval    无
 */
static void AppAlarm_ExitCriticalBatteryAlarm(void);

/****************************************************************************
 * 私有函数实现
 ****************************************************************************/

/**
 * @name      AppAlarm_ShutdownCallback
 * @brief     关机定时器回调函数
 * @param     user_data - 用户数据（未使用）
 * @retval    无
 */
static void AppAlarm_ShutdownCallback(void* user_data)
{
    (void)user_data;
    
    /* 执行关机操作 */
    HAL_Power_Release();
}

/**
 * @name      AppAlarm_EnterCriticalBatteryAlarm
 * @brief     进入严重低电报警状态
 * @param     无
 * @retval    无
 */
static void AppAlarm_EnterCriticalBatteryAlarm(void)
{
    /* 更新报警状态 */
    g_alarm_state = ALARM_TYPE_BATTERY_CRITICAL;
    
    /* 严重低电显示期间：关闭白色背光，打开黄色背光作为显著告警 */
    HAL_LCD_Backlight_Off();
    HAL_LED_Yellow_On();
    
    /* 显示低电图标（只显示一次，之后保持显示） */
    if (!g_icon_displayed)
    {
        Display_ShowImageStartupFormat(0, 0, 128, 64, LOW_BATTERY_IMAGE);
        g_icon_displayed = true;
    }
    
    /* 创建10秒关机定时器 */
    if (g_shutdown_timer == 0)
    {
        g_shutdown_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE, SHUTDOWN_DELAY_MS, AppAlarm_ShutdownCallback, NULL);    
        if (g_shutdown_timer != 0)
        {
            SoftTimer_Start(g_shutdown_timer);
        }
    }
    else
    {
        /* 如果定时器已存在，重新启动 */
        SoftTimer_Stop(g_shutdown_timer);
        SoftTimer_SetPeriod(g_shutdown_timer, SHUTDOWN_DELAY_MS);
        SoftTimer_Start(g_shutdown_timer);
    }
}

/**
 * @name      AppAlarm_ExitCriticalBatteryAlarm
 * @brief     退出严重低电报警状态
 * @param     无
 * @retval    无
 */
static void AppAlarm_ExitCriticalBatteryAlarm(void)
{
    /* 更新报警状态 */
    g_alarm_state = ALARM_TYPE_NONE;
    g_icon_displayed = false;
    
    /* 恢复背光状态：关闭黄色背光，打开白色背光 */
    HAL_LED_Yellow_Off();
    HAL_LCD_Backlight_On();
    
    /* 停止关机定时器 */
    if (g_shutdown_timer != 0)
    {
        SoftTimer_Stop(g_shutdown_timer);
        g_shutdown_timer = 0;
    }
}

/****************************************************************************
 * 公共函数实现
 ****************************************************************************/

/**
 * @name      AppAlarm_Init
 * @brief     初始化报警管理模块
 * @param     无
 * @retval    无
 */
void AppAlarm_Init(void)
{
    /* 初始化报警状态 */
    g_alarm_state = ALARM_TYPE_NONE;
    g_shutdown_timer = 0;
    g_icon_displayed = false;
}

/**
 * @name      AppAlarm_Process
 * @brief     报警处理函数（在主循环10ms任务中调用）
 * @param     无
 * @retval    无
 * @note      检测报警条件，更新报警状态，处理报警逻辑
 */
void AppAlarm_Process(void)
{
    /* 若处于泄气阶段（电磁阀打开），暂不进行严重低电报警评估，避免瞬时压降误判 */
    if (AppPressure_IsBleeding()) {
        return;
    }
    
    /* 获取电池电量等级和充电状态 */
    BatteryLevel_e battery_level = AppBattery_GetLevelEnum();
    bool is_charging = AppBattery_IsCharging();
    
    /* 检测严重低电报警条件 */
    if (battery_level == BATTERY_LEVEL_CRITICAL && !is_charging)
//    if (battery_level == BATTERY_LEVEL_25 && !is_charging) // hpp
    {
        /* 进入严重低电报警状态 */
        if (g_alarm_state != ALARM_TYPE_BATTERY_CRITICAL)
        {
            AppAlarm_EnterCriticalBatteryAlarm();
        }
        else
        {
            /* 如果已经在报警状态，确保图标持续显示 */
            if (!g_icon_displayed)
            {
                Display_ShowImageStartupFormat(0, 0, 128, 64, LOW_BATTERY_IMAGE);
                g_icon_displayed = true;
            }
        }
    }
    else
    {
        /* 恢复正常状态（电池电量恢复或开始充电） */
        if (g_alarm_state == ALARM_TYPE_BATTERY_CRITICAL)
//        if (g_alarm_state == BATTERY_LEVEL_25)
        {
            AppAlarm_ExitCriticalBatteryAlarm();
        }
    }
}

/**
 * @name      AppAlarm_GetActiveAlarm
 * @brief     获取当前激活的报警类型
 * @param     无
 * @retval    当前激活的报警类型（ALARM_TYPE_NONE表示无报警）
 */
AlarmType_e AppAlarm_GetActiveAlarm(void)
{
    return g_alarm_state;
}

/**
 * @name      AppAlarm_IsCriticalBatteryActive
 * @brief     查询是否处于严重低电报警状态
 * @param     无
 * @retval    true=处于严重低电报警状态, false=未处于报警状态
 */
bool AppAlarm_IsCriticalBatteryActive(void)
{
    return (g_alarm_state == ALARM_TYPE_BATTERY_CRITICAL);
}

