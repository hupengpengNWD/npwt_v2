/****************************************************************************
 * 文件名: hardware_abstraction.h
 * 功能: 硬件抽象层（HAL）
 * 
 * 说明: 
 *   将硬件操作封装为函数接口，提高代码移植性
 *   隔离硬件细节，使上层代码更清晰
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef HARDWARE_ABSTRACTION_H
#define HARDWARE_ABSTRACTION_H

#include "include.h"

/****************************************************************************
 * 气泵控制接口
 ****************************************************************************/

/**
 * 函数: HAL_Pump_Start
 * 功能: 启动气泵
 */
static inline void HAL_Pump_Start(void)
{
	PUMP = 1;
}

/**
 * 函数: HAL_Pump_Stop
 * 功能: 停止气泵
 */
static inline void HAL_Pump_Stop(void)
{
	PUMP = 0;
}

/**
 * 函数: HAL_Pump_IsRunning
 * 功能: 检查气泵是否在运行
 * 返回: 1=运行中, 0=已停止
 */
static inline unsigned char HAL_Pump_IsRunning(void)
{
	return PUMP;
}

/****************************************************************************
 * 电磁阀控制接口
 ****************************************************************************/

/**
 * 函数: HAL_Valve1_Open
 * 功能: 开启电磁阀1（排气阀）
 */
static inline void HAL_Valve1_Open(void)
{
	VAL1 = 1;
}

/**
 * 函数: HAL_Valve1_Close
 * 功能: 关闭电磁阀1（排气阀）
 */
static inline void HAL_Valve1_Close(void)
{
	VAL1 = 0;
}

/**
 * 函数: HAL_Valve2_Open
 * 功能: 开启电磁阀2（放气阀）
 */
static inline void HAL_Valve2_Open(void)
{
	VAL2 = 1;
}

/**
 * 函数: HAL_Valve2_Close
 * 功能: 关闭电磁阀2（放气阀）
 */
static inline void HAL_Valve2_Close(void)
{
	VAL2 = 0;
}

/****************************************************************************
 * LED指示灯控制接口
 ****************************************************************************/

/**
 * 函数: HAL_LED_Green_On
 * 功能: 开启绿色LED（工作指示）
 */
static inline void HAL_LED_Green_On(void)
{
	GRE = 1;
}

/**
 * 函数: HAL_LED_Green_Off
 * 功能: 关闭绿色LED
 */
static inline void HAL_LED_Green_Off(void)
{
	GRE = 0;
}

/**
 * 函数: HAL_LED_Yellow_On
 * 功能: 开启黄色LED（背光）
 */
static inline void HAL_LED_Yellow_On(void)
{
	YEL = 1;
}

/**
 * 函数: HAL_LED_Yellow_Off
 * 功能: 关闭黄色LED
 */
static inline void HAL_LED_Yellow_Off(void)
{
	YEL = 0;
}

/****************************************************************************
 * 蜂鸣器控制接口
 ****************************************************************************/

/**
 * 函数: HAL_Buzzer_On
 * 功能: 开启蜂鸣器
 */
static inline void HAL_Buzzer_On(void)
{
	SPEAK = 1;
}

/**
 * 函数: HAL_Buzzer_Off
 * 功能: 关闭蜂鸣器
 */
static inline void HAL_Buzzer_Off(void)
{
	SPEAK = 0;
}

/****************************************************************************
 * 电源控制接口
 ****************************************************************************/

/**
 * 函数: HAL_Power_Hold
 * 功能: 保持电源（自锁）
 */
static inline void HAL_Power_Hold(void)
{
	POWER_ON = 1;
}

/**
 * 函数: HAL_Power_Release
 * 功能: 释放电源（关机）
 */
static inline void HAL_Power_Release(void)
{
	POWER_ON = 0;
}

/****************************************************************************
 * 电池状态读取接口
 ****************************************************************************/

/**
 * 函数: HAL_Battery_IsGood
 * 功能: 检查电池电量是否良好
 * 返回: 1=良好, 0=不足
 */
static inline unsigned char HAL_Battery_IsGood(void)
{
	return BAT_GOOD;
}

/**
 * 函数: HAL_Battery_IsCharging
 * 功能: 检查是否正在充电
 * 返回: 0=充电中, 1=未充电
 */
static inline unsigned char HAL_Battery_IsCharging(void)
{
	return BAT_CHARGE;
}

/****************************************************************************
 * 看门狗控制接口
 ****************************************************************************/

/**
 * 函数: HAL_Watchdog_Clear
 * 功能: 清除看门狗
 */
static inline void HAL_Watchdog_Clear(void)
{
	asm("clrwdt");
}

#endif /* HARDWARE_ABSTRACTION_H */

