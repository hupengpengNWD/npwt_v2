/****************************************************************************
 * 文件名: main.c
 * 功能: 主程序入口（渐进式集成版本）
 * 
 * 说明: 
 *   逐步集成功能，每次添加一个模块并测试
 *   当前阶段：步骤1 - 电源控制和开机检测
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#include "../Inc/system_config.h"
#include "../Inc/system_types.h"
#include "../Inc/system_enums.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../HAL/Inc/hal_timer.h"

/****************************************************************************
 * 全局变量
 ****************************************************************************/
extern volatile unsigned char FLG_SYS_10MS;

/* 系统状态 */
static SystemState_t g_system;

/****************************************************************************
 * 步骤1：电源控制模块
 ****************************************************************************/

/* 电源控制状态 */
typedef enum {
	POWER_STATE_INIT,      // 初始化，检测按键
	POWER_STATE_RUNNING,   // 运行中，电源已自锁
	POWER_STATE_SHUTDOWN   // 关机
} PowerState_e;

static PowerState_e power_state = POWER_STATE_INIT;
static uint16_t init_timer = 0;
static uint16_t key_hold_timer = 0;

/**
 * 函数: PowerControl_Update
 * 功能: 电源控制更新（每10ms调用）
 */
void PowerControl_Update(void)
{
	switch (power_state)
	{
	case POWER_STATE_INIT:
	{
		init_timer++;
		
		/* 前50个周期（500ms）检测按键 */
		if (init_timer <= 50)
		{
			uint8_t key = PORTB & 0x3C;
			if (key == 0x38)  // 确认键按下
			{
				key_hold_timer++;
			}
			else
			{
				key_hold_timer = 0;
			}
		}
		/* 第51个周期判断 */
		else if (init_timer == 51)
		{
			if (key_hold_timer < 30)  // 按键不足300ms
			{
				// 开机失败，复位
				HAL_Power_Release();
				init_timer = 0;
				key_hold_timer = 0;
			}
			else  // 按键足够
			{
				// 开机成功
				HAL_Power_Hold();
				LATCbits.LATC6 = 1;  // 背光
				power_state = POWER_STATE_RUNNING;
			}
		}
		/* 7秒后仍未开机，复位 */
		else if (init_timer >= 700)
		{
			init_timer = 0;
			key_hold_timer = 0;
		}
		break;
	}
	
	case POWER_STATE_RUNNING:
	{
		// 运行中，电源已自锁
		// 后续添加关机检测
		break;
	}
	
	case POWER_STATE_SHUTDOWN:
	{
		// 关机处理
		HAL_Power_Release();
		while(1) { asm("clrwdt"); }
		break;
	}
	}
}

/****************************************************************************
 * 主函数
 ****************************************************************************/
void main(void)
{
	/* ========== 系统初始化 ========== */
	
	/* 1. 初始化振荡器（与未重构工程SYS_OSC_Ini完全一致） */
	OSCCON = 0b01110000;    // 内部振荡器，8MHz
	OSCTUNE = OSCTUNE | 0x40; // 使能4×PLL → 32MHz
	while (!(OSCCON & 0x08)); // 等待振荡器稳定
	asm("clrwdt");
	
	/* 2. 初始化GPIO */
	HAL_GPIO_Init();
	
	/* 3. 初始化定时器 */
	HAL_Timer_Init();     // Timer0: 10ms
	HAL_Timer1_Init();    // Timer1: 10ms
	HAL_PWM_Init();       // Timer3: 1ms
	
	/* 4. 使能全局中断 */
	T3CONbits.TMR3ON = 1; // 启动Timer3
	GIE = 1;
	PEIE = 1;
	asm("clrwdt");
	
	/* ========== 主循环 ========== */
	while (1)
	{
		/* 喂狗 */
		asm("clrwdt");
		
		/* 10ms任务 */
		if (FLG_SYS_10MS)
		{
			FLG_SYS_10MS = 0;
			
			/* 步骤1：电源控制更新 */
			PowerControl_Update();
			
			/* 测试：翻转绿色LED指示主循环正常 */
			LATCbits.LATC4 ^= 1;
		}
	}
}

/****************************************************************************
 * 中断服务程序
 ****************************************************************************/
void __interrupt() ISR(void)
{
	/* Timer3中断处理（1ms） */
	HAL_Timer3_ISR();
	
	/* Timer0中断处理（10ms） */
	HAL_Timer_ISR();
	
	/* Timer1中断处理（10ms） */
	HAL_Timer1_ISR();
}
