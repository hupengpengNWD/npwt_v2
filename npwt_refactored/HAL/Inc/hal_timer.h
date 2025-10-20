/****************************************************************************
 * 文件名: hal_timer.h
 * 功能: 定时器硬件抽象层
 * 
 * 说明: 
 *   封装定时器初始化和操作
 *   提供系统滴答和PWM功能
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 定时器操作函数
 ****************************************************************************/

/**
 * 函数: HAL_Timer_Init
 * 功能: 初始化系统定时器
 * 说明: Timer0用于20ms系统滴答
 */
void HAL_Timer_Init(void);

/**
 * 函数: HAL_Timer_GetTick
 * 功能: 获取系统运行时间
 * 返回: 系统运行的毫秒数
 */
uint32_t HAL_Timer_GetTick(void);

/**
 * 函数: HAL_PWM_Init
 * 功能: 初始化PWM模块
 * 说明: Timer3用于PWM控制气泵速度
 */
void HAL_PWM_Init(void);

/**
 * 函数: HAL_PWM_SetDuty
 * 功能: 设置PWM占空比
 * 参数: duty - 占空比值（0-10）
 */
void HAL_PWM_SetDuty(uint8_t duty);

/**
 * 函数: HAL_Watchdog_Clear
 * 功能: 清除看门狗
 */
static inline void HAL_Watchdog_Clear(void)
{
	asm("clrwdt");
}

#endif /* HAL_TIMER_H */

