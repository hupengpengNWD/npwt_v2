/****************************************************************************
 * 文件名: realtime_tasks.c
 * 功能: 实时任务管理实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/realtime_tasks.h"
#include "../../HAL/Inc/hal_timer.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../HAL/Inc/hal_adc.h"
#include "../../Drivers/Inc/adc_driver.h"
#include "../Inc/pressure_controller.h"
#include "../../Core/Inc/mcu_config.h"  // 包含 NULL 定义

/* PWM控制参数 */
#define PWM_PERIOD  10  // PWM周期（1ms * 10 = 10ms，100Hz）

/* 内部状态 */
static bool g_pump_enabled = false;
static uint8_t g_pwm_duty = 0;        // PWM占空比（0-10）
static uint8_t g_pwm_counter = 0;     // PWM计数器

/* 系统运行时间 */
static uint32_t g_system_time_5ms = 0;

/* 外部引用：获取间歇模式当前阶段 */
extern IntermittentPhase_e g_intermittent_phase;
extern bool g_is_intermittent_mode;

/* 全局指针定义：压力控制器和泵控制 */
PressureData_t *g_pressure_ctrl = NULL;
PumpData_t *g_pump_ctrl = NULL;

/**
 * 函数: RealtimeTask_1ms
 * 功能: 1ms实时任务（在Timer3中断中执行）
 * 说明: 
 *   - PWM波形生成
 *   - 间歇模式下只在高压阶段开启气泵
 */
static void RealtimeTask_1ms(void)
{
	/* PWM波形生成 */
	if (g_pump_enabled)
	{
		g_pwm_counter++;
		
		/* 在占空比范围内开启气泵 */
		if (g_pwm_counter <= g_pwm_duty)
		{
			/* 间歇模式检查：只在高压阶段开启气泵 */
			if (g_is_intermittent_mode)
			{
				if (g_intermittent_phase == INTERMITTENT_PHASE_HIGH)
				{
					HAL_Pump_Enable(true);
				}
				else
				{
					HAL_Pump_Enable(false);
				}
			}
			else
			{
				/* 连续模式：直接开启 */
				HAL_Pump_Enable(true);
			}
		}
		else
		{
			HAL_Pump_Enable(false);
		}
		
		/* PWM周期结束，重置计数器 */
		if (g_pwm_counter >= PWM_PERIOD)
		{
			g_pwm_counter = 0;
		}
	}
	else
	{
		/* 气泵禁用 */
		HAL_Pump_Enable(false);
		g_pwm_counter = 0;
	}
}

/**
 * 函数: RealtimeTask_5ms
 * 功能: 5ms实时任务（在Timer3中断中执行）
 * 说明: 
 *   - ADC压力采样
 *   - 压力控制算法执行
 *   - 系统时间累加
 */
static void RealtimeTask_5ms(void)
{
	/* ADC压力采样 */
	ADC_SamplePressure();
	
	/* 执行压力控制算法（通过全局指针调用） */
	/* 注意：需要在PressureController_Init中设置g_pressure_ctrl */
	if (g_pressure_ctrl != NULL && g_pump_ctrl != NULL) {
		PressureController_Update(g_pressure_ctrl, g_pump_ctrl);
	}
	
	/* 系统运行时间累加 */
	g_system_time_5ms++;
}

/**
 * 函数: RealtimeTasks_Init
 * 功能: 初始化实时任务
 */
void RealtimeTasks_Init(void)
{
	/* 注册中断回调函数 */
	HAL_Timer3_RegisterCallback_1ms(RealtimeTask_1ms);
	HAL_Timer3_RegisterCallback_5ms(RealtimeTask_5ms);
	
	/* 初始化状态 */
	g_pump_enabled = false;
	g_pwm_duty = 0;
	g_pwm_counter = 0;
	g_system_time_5ms = 0;
}

/**
 * 函数: RealtimeTasks_SetPumpEnable
 * 功能: 设置气泵使能状态
 */
void RealtimeTasks_SetPumpEnable(bool enable)
{
	g_pump_enabled = enable;
	
	if (!enable)
	{
		HAL_Pump_Enable(false);
		g_pwm_counter = 0;
	}
}

/**
 * 函数: RealtimeTasks_SetPWMDuty
 * 功能: 设置PWM占空比
 */
void RealtimeTasks_SetPWMDuty(uint8_t duty)
{
	if (duty > PWM_PERIOD)
	{
		duty = PWM_PERIOD;
	}
	
	g_pwm_duty = duty;
}

