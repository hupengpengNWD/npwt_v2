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
#include "../../Drivers/Inc/lcd_driver.h"
#include "../../Drivers/Inc/key_driver.h"
#include "../../Middleware/Inc/pressure_controller.h"
#include "../../Middleware/Inc/battery_manager.h"
#include "../../Middleware/Inc/fault_detector.h"
#include "../../Middleware/Inc/alarm_manager.h"
#include "../../Middleware/Inc/realtime_tasks.h"
#include "../../Middleware/Inc/intermittent_controller.h"
#include "../../Application/Inc/app_state_machine.h"
#include "../../Application/Inc/app_input.h"
#include "../../Application/Inc/app_display.h"
#include "../../Application/Inc/app_settings.h"
#include "../../Application/Inc/app_selftest.h"

/****************************************************************************
 * 系统状态（全局单例）
 ****************************************************************************/
static SystemState_t g_system;
static KeyData_t g_key_data;

/****************************************************************************
 * 静态辅助函数声明
 ****************************************************************************/
static void System_CheckIdle(SystemState_t *sys);
static void System_VentControl(SystemState_t *sys);

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
	LCD_Driver_Init();
	Key_Init(&g_key_data);
	
	/* 显示开机Logo */
	LCD_DisplayStartup();
	
	/* 初始化报警管理器 */
	AlarmManager_Init(&g_system.alarm);
}

/**
 * 函数: System_InitModules
 * 功能: 初始化所有功能模块
 */
static void System_InitModules(void)
{
	/* 初始化中间件层 */
	PressureController_Init(&g_system.pressure);
	/* 注意：BatteryManager 和 FaultDetector 使用结构体初始化，不需要 Init 函数 */
	/* 电池和故障检测器在首次 Update 时自动初始化 */
	IntermittentController_Init(&g_system.intermittent);
	
	/* 首次电池检测 */
	BatteryManager_Update(&g_system.battery);
	
	/* 初始化实时任务（注册Timer3中断回调） */
	RealtimeTasks_Init();
	
	/* 初始化应用层 */
	AppStateMachine_Init(&g_system);
	AppSettings_Init(&g_system.settings);
}

/**
 * 函数: System_LoadConfig
 * 功能: 从Flash加载配置
 */
static void System_LoadConfig(void)
{
	/* 加载所有系统设置 */
	if (!Flash_LoadSystemSettings(&g_system))
	{
		/* 加载失败或首次使用，使用默认配置 */
		PressureController_SetTarget(&g_system.pressure, PRESSURE_DEFAULT);
		g_system.pressure.calibration_k = 2.75f;
		g_system.settings.continuous_time = 9999;
		g_system.intermittent.high_time = 10;
		g_system.intermittent.low_time = 2;
		g_system.intermittent.stop_time = 3;
		g_system.intermittent.high_pressure = 120;
		g_system.intermittent.low_pressure = 80;
	}
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
			
			/* 1. 按键扫描与处理 */
			if (g_system.current_mode != MODE_INIT)
			{
				Key_Scan(&g_key_data);
				AppInput_Process(&g_system, &g_key_data);
			}
			
			/* 2. 间歇模式更新（每秒） */
			static uint16_t intermittent_timer = 0;
			if (g_system.current_mode == MODE_INTERMITTENT)
			{
				if (intermittent_timer++ >= 50)  // 1秒
				{
					intermittent_timer = 0;
					IntermittentController_Update(&g_system.intermittent, &g_system.pressure);
				}
			}
			else
			{
				intermittent_timer = 0;
			}
			
			/* 3. 状态机运行（模式控制） */
			AppStateMachine_Run(&g_system);
			
			/* 4. LCD显示更新 */
			if (g_system.current_mode == MODE_SETTINGS)
			{
				AppSettings_Display(&g_system);
			}
			else
			{
				AppDisplay_Update(&g_system);
			}
			
			/* 5. 故障检测 */
			if (g_system.current_mode != MODE_INIT && 
			    g_system.current_mode != MODE_SHUTDOWN &&
			    g_system.current_mode != MODE_SETTINGS &&
			    g_system.current_mode != MODE_SELFTEST)
			{
				FaultDetector_Update(&g_system.fault, &g_system.pressure, &g_system.pump);
			}
			
			/* 6. 自检模式处理 */
			if (g_system.current_mode == MODE_SELFTEST)
			{
				if (AppSelftest_Run(&g_system))
				{
					/* 自检完成，返回待机 */
					AppSelftest_Stop(&g_system);
					AppStateMachine_SetMode(&g_system, MODE_STANDBY);
				}
			}
			
			/* 7. 声音报警处理 */
			AlarmManager_Update(&g_system.alarm, &g_system.battery, &g_system.fault);
			
			/* 8. 电池管理（每1秒） */
			static uint16_t battery_timer = 0;
			if (battery_timer++ >= BATTERY_CHECK_INTERVAL)
			{
				battery_timer = 0;
				
				if (!BatteryManager_Update(&g_system.battery))
				{
					AppStateMachine_SetMode(&g_system, MODE_SHUTDOWN);
				}
			}
			
			/* 9. 空闲检测（每1秒） */
			static uint16_t idle_timer = 0;
			if (idle_timer++ >= 50)  // 1秒
			{
				idle_timer = 0;
				System_CheckIdle(&g_system);
			}
			
			/* 10. 排气控制（泄漏补偿） */
			System_VentControl(&g_system);
		}
	}
}

/****************************************************************************
 * 系统辅助函数实现
 ****************************************************************************/

/**
 * 函数: System_CheckIdle
 * 功能: 检测系统空闲时间，超过5分钟自动关机
 */
static void System_CheckIdle(SystemState_t *sys)
{
	static uint16_t idle_seconds = 0;
	
	/* 只在待机和暂停模式检测空闲 */
	if (sys->current_mode == MODE_STANDBY || 
	    sys->current_mode == MODE_PAUSE)
	{
		idle_seconds++;
		
		/* 超过5分钟（300秒）自动关机 */
		if (idle_seconds >= 300)
		{
			AppStateMachine_SetMode(sys, MODE_SHUTDOWN);
			idle_seconds = 0;
		}
	}
	else
	{
		/* 工作模式重置计数器 */
		idle_seconds = 0;
	}
}

/**
 * 函数: System_VentControl
 * 功能: 排气控制（泄漏补偿）
 * 说明: 当检测到泄漏时，打开排气阀和气泵进行补气
 */
static void System_VentControl(SystemState_t *sys)
{
	static uint8_t vent_delay = 0;
	static bool vent_active = false;
	
	/* 检查是否需要排气补偿 */
	if (sys->fault.leakage_detected)
	{
		if (!vent_active)
		{
			/* 开启排气阀 */
			HAL_Valve1_Open();
			vent_delay = 0;
			vent_active = true;
		}
		else
		{
			vent_delay++;
			
			if (vent_delay >= 50)  // 1秒后开启气泵
			{
				HAL_Pump_Enable(true);
			}
			
			if (vent_delay >= 100)  // 2秒后关闭排气阀
			{
				HAL_Valve1_Close();
				HAL_Pump_Enable(false);
				vent_active = false;
				vent_delay = 0;
				
				/* 记录泄漏次数 */
				sys->fault.leakage_count++;
			}
		}
	}
	else
	{
		/* 无泄漏，确保排气系统关闭 */
		if (vent_active)
		{
			HAL_Valve1_Close();
			HAL_Pump_Enable(false);
			vent_active = false;
			vent_delay = 0;
		}
	}
}

/****************************************************************************
 * 中断服务程序
 ****************************************************************************/

void __interrupt() ISR(void)
{
	/* Timer3中断：1ms定时（PWM、压力采集、压力控制） */
	HAL_Timer3_ISR();
	
	/* Timer0中断：20ms系统滴答（主循环标志） */
	HAL_Timer_ISR();
}

