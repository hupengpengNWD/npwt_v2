/****************************************************************************
 * 文件名: main.c
 * 功能: 主程序入口
 * 
 * 说明: 
 *   NPWT负压伤口治疗仪主程序
 *   采用现代化分层架构设计
 * 
 * 架构：
 *   Application → Middleware → Drivers → HAL → Hardware
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/system_config.h"
#include "../Inc/system_types.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../HAL/Inc/hal_adc.h"
#include "../../HAL/Inc/hal_timer.h"
#include "../../Drivers/Inc/adc_driver.h"
#include "../../Drivers/Inc/flash_driver.h"
#include "../../Middleware/Inc/pressure_controller.h"
#include "../../Middleware/Inc/battery_manager.h"
#include "../../Middleware/Inc/fault_detector.h"
#include "../../Application/Inc/app_state_machine.h"

#include <pic18f46j11.h>

/****************************************************************************
 * 系统状态（全局单例）
 ****************************************************************************/
static SystemState_t g_system;

/****************************************************************************
 * 系统初始化函数
 ****************************************************************************/

/**
 * 函数: System_InitHardware
 * 功能: 初始化所有硬件
 */
static void System_InitHardware(void)
{
	/* 初始化HAL层 */
	HAL_GPIO_Init();
	HAL_ADC_Init();
	HAL_Timer_Init();
	HAL_PWM_Init();
	
	/* 初始化驱动层 */
	ADC_Driver_Init();
	Flash_Init();
}

/**
 * 函数: System_InitModules
 * 功能: 初始化所有功能模块
 */
static void System_InitModules(void)
{
	/* 初始化中间件层 */
	PressureController_Init(&g_system.pressure);
	BatteryManager_Init(&g_system.battery);
	FaultDetector_Init(&g_system.fault);
	
	/* 初始化应用层 */
	AppStateMachine_Init(&g_system);
}

/**
 * 函数: System_LoadConfig
 * 功能: 从Flash加载配置
 */
static void System_LoadConfig(void)
{
	FlashConfig_t config;
	
	if (Flash_ReadConfig(&config))
	{
		/* 应用配置 */
		PressureController_SetTarget(&g_system.pressure, config.pressure_setting);
		g_system.pressure.calibration_k = config.calibration_k1;
	}
	else
	{
		/* 使用默认配置 */
		PressureController_SetTarget(&g_system.pressure, PRESSURE_DEFAULT);
		g_system.pressure.calibration_k = 2.75f;
	}
	
	/* 新架构仅支持英语显示，无需语言设置 */
}

/****************************************************************************
 * 主函数
 ****************************************************************************/

void main(void)
{
	/* ========== 系统初始化 ========== */
	
	/* 1. 清除看门狗 */
	HAL_Watchdog_Clear();
	
	/* 2. 初始化硬件 */
	System_InitHardware();
	
	/* 3. 加载配置 */
	System_LoadConfig();
	
	/* 4. 初始化功能模块 */
	System_InitModules();
	
	/* 5. 使能全局中断 */
	INTCONbits.GIE = 1;   // 全局中断使能
	INTCONbits.PEIE = 1;  // 外设中断使能
	
	/* ========== 主循环 ========== */
	
	while (1)
	{
		/* 喂狗 */
		HAL_Watchdog_Clear();
		
		/* 等待20ms系统滴答 */
		static uint32_t last_tick = 0;
		uint32_t current_tick = HAL_Timer_GetTick();
		
		if ((current_tick - last_tick) >= SYSTEM_TICK_MS)
		{
			last_tick = current_tick;
			
			/* 更新系统运行时间 */
			g_system.uptime_ms += SYSTEM_TICK_MS;
			
			/* 运行状态机 */
			AppStateMachine_Run(&g_system);
			
			/* 更新电池管理（每1秒） */
			static uint16_t battery_timer = 0;
			if (battery_timer++ >= BATTERY_CHECK_INTERVAL)
			{
				battery_timer = 0;
				
				if (!BatteryManager_Update(&g_system.battery))
				{
					/* 电池耗尽，关机 */
					AppStateMachine_SetMode(&g_system, MODE_SHUTDOWN);
				}
			}
		}
	}
}

/****************************************************************************
 * 中断服务程序
 ****************************************************************************/

void interrupt ISR(void)
{
	/* Timer0中断：20ms系统滴答 */
	HAL_Timer_ISR();
	
	/* 其他中断处理... */
}

