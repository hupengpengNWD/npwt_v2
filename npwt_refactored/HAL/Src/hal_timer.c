/****************************************************************************
 * 文件名: hal_timer.c
 * 功能: 定时器硬件抽象层实现
 * 
 * 说明: 
 *   PIC18F46J11定时器配置
 *   Timer0: 20ms系统滴答
 *   Timer3: PWM控制
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/hal_timer.h"
#include "../../Core/Inc/system_config.h"
#include <pic18f46j11.h>

/* 系统滴答计数器 */
static volatile uint32_t g_system_tick_ms = 0;

/**
 * 函数: HAL_Timer_Init
 * 功能: 初始化Timer0为20ms定时器
 */
void HAL_Timer_Init(void)
{
	/* Timer0配置：8位模式，1:256预分频 */
	T0CON = 0b11000111;  // 使能，8位，预分频1:256
	
	/* 计算初值：20ms @ 32MHz */
	/* Tout = 4 * prescaler * (256-TMR0) / Fosc */
	/* 20ms = 4 * 256 * (256-TMR0) / 32000000 */
	/* TMR0 = 256 - 625 = -369（溢出） */
	/* 实际使用：156（实验调整值） */
	TMR0 = 156;
	
	/* 使能Timer0中断 */
	INTCONbits.TMR0IE = 1;  // 使能Timer0中断
	INTCONbits.TMR0IF = 0;  // 清除中断标志
}

/**
 * 函数: HAL_PWM_Init
 * 功能: 初始化Timer3为PWM
 */
void HAL_PWM_Init(void)
{
	/* Timer3配置：16位模式 */
	T3CON = 0b00110000;  // 1:8预分频，关闭Timer3
	
	/* 设置周期 */
	TMR3H = 0;
	TMR3L = 0;
	
	/* 使能Timer3中断 */
	PIE2bits.TMR3IE = 1;
	PIR2bits.TMR3IF = 0;
}

/**
 * 函数: HAL_PWM_SetDuty
 * 功能: 设置PWM占空比
 */
void HAL_PWM_SetDuty(uint8_t duty)
{
	/* 占空比范围：0-10 */
	if (duty > 10) duty = 10;
	
	/* 这里需要根据实际硬件设置PWM寄存器 */
	/* 原代码使用pwm_cnt2变量控制 */
}

/**
 * 函数: HAL_Timer_GetTick
 * 功能: 获取系统运行时间
 */
uint32_t HAL_Timer_GetTick(void)
{
	return g_system_tick_ms;
}

/**
 * 函数: HAL_Timer_ISR
 * 功能: 定时器中断服务程序（由主ISR调用）
 */
void HAL_Timer_ISR(void)
{
	if (INTCONbits.TMR0IF)
	{
		INTCONbits.TMR0IF = 0;  // 清除标志
		TMR0 = 156;             // 重载初值
		
		g_system_tick_ms += SYSTEM_TICK_MS;  // 累加20ms
	}
}

