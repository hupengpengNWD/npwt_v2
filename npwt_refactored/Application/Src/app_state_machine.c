/****************************************************************************
 * 文件名: app_state_machine.c
 * 功能: 应用状态机实现
 * 
 * 说明: 
 *   表驱动状态机设计
 *   每个状态有独立的处理函数
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/app_state_machine.h"
#include "../Inc/app_settings.h"
#include "../Inc/app_selftest.h"
#include "../../Drivers/Inc/flash_driver.h"  // Flash_SaveSystemSettings
#include "../../Middleware/Inc/alarm_manager.h"  // AlarmManager_Beep
#include "../../Middleware/Inc/pressure_controller.h"
#include "../../Middleware/Inc/battery_manager.h"
#include "../../Middleware/Inc/fault_detector.h"
#include "../../Middleware/Inc/realtime_tasks.h"
#include "../../Middleware/Inc/intermittent_controller.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../HAL/Inc/hal_timer.h"
#include "../../Drivers/Inc/key_driver.h"
#include "../../Drivers/Inc/lcd_driver.h"
#include "../../Drivers/Inc/flash_driver.h"

/* 前向声明：状态处理函数 */
static void State_Init_Handler(SystemState_t *state);
static void State_Standby_Handler(SystemState_t *state);
static void State_Continuous_Handler(SystemState_t *state);
static void State_Intermittent_Handler(SystemState_t *state);
static void State_Pause_Handler(SystemState_t *state);
static void State_Error_Handler(SystemState_t *state);
static void State_Shutdown_Handler(SystemState_t *state);

/* 状态处理函数指针类型 */
typedef void (*StateHandler_t)(SystemState_t *);

/* 状态表项 */
typedef struct {
	WorkMode_e mode;
	StateHandler_t handler;
	const char *name;
} StateTableEntry_t;

/* 状态表定义 */
static const StateTableEntry_t g_state_table[] = {
	{MODE_INIT,         State_Init_Handler,         "初始化"},
	{MODE_STANDBY,      State_Standby_Handler,      "待机"},
	{MODE_CONTINUOUS,   State_Continuous_Handler,   "连续模式"},
	{MODE_INTERMITTENT, State_Intermittent_Handler, "间歇模式"},
	{MODE_PAUSE,        State_Pause_Handler,        "暂停"},
	{MODE_ERROR,        State_Error_Handler,        "故障"},
	{MODE_SHUTDOWN,     State_Shutdown_Handler,     "关机"}
};

#define STATE_TABLE_SIZE (sizeof(g_state_table) / sizeof(g_state_table[0]))

/**
 * 函数: AppStateMachine_Init
 * 功能: 初始化状态机
 */
void AppStateMachine_Init(SystemState_t *state)
{
	state->current_mode = MODE_INIT;
	state->previous_mode = MODE_INIT;
	state->system_ready = false;
}

/**
 * 函数: AppStateMachine_Run
 * 功能: 运行状态机
 */
void AppStateMachine_Run(SystemState_t *state)
{
	/* 查找当前模式的处理函数 */
	for (uint8_t i = 0; i < STATE_TABLE_SIZE; i++)
	{
		if (g_state_table[i].mode == state->current_mode)
		{
			if (g_state_table[i].handler != NULL)
			{
				g_state_table[i].handler(state);
			}
			return;
		}
	}
}

/**
 * 函数: AppStateMachine_SetMode
 * 功能: 切换工作模式
 */
void AppStateMachine_SetMode(SystemState_t *state, WorkMode_e mode)
{
	state->previous_mode = state->current_mode;
	state->current_mode = mode;
}


/****************************************************************************
 * 状态处理函数实现
 ****************************************************************************/

/**
 * 状态: 初始化
 */
static void State_Init_Handler(SystemState_t *state)
{
	/* 显示开机画面 */
	/* 检查启动按键 */
	/* 初始化完成后转入待机模式 */
	
	static uint16_t init_timer = 0;
	
	if (init_timer++ >= 100)  // 2秒后完成初始化
	{
		HAL_Power_Hold();  // 保持电源
		AppStateMachine_SetMode(state, MODE_STANDBY);
		state->system_ready = true;
		init_timer = 0;
	}
}

/**
 * 状态: 待机
 */
static void State_Standby_Handler(SystemState_t *state)
{
	/* 等待用户选择工作模式 */
	/* 显示菜单 */
	/* 处理按键输入 */
	
	HAL_Pump_Stop();
	HAL_Valve1_Close();
	HAL_Valve2_Close();
}

/**
 * 状态: 连续模式
 */
static void State_Continuous_Handler(SystemState_t *state)
{
	/* 更新压力控制 */
	PressureController_Update(&state->pressure, &state->pump);
	
	/* 更新故障检测 */
	FaultDetector_Update(&state->fault, &state->pressure, &state->pump);
	
	/* 检查是否有故障 */
	if (state->fault.current_error != ERROR_NONE)
	{
		AppStateMachine_SetMode(state, MODE_ERROR);
	}
}

/**
 * 状态: 间歇模式
 */
static void State_Intermittent_Handler(SystemState_t *state)
{
	/* 间歇模式：高压-低压-停顿 循环 */
	/* 实现复杂的间歇控制逻辑 */
	
	PressureController_Update(&state->pressure, &state->pump);
	FaultDetector_Update(&state->fault, &state->pressure, &state->pump);
	
	if (state->fault.current_error != ERROR_NONE)
	{
		AppStateMachine_SetMode(state, MODE_ERROR);
	}
}

/**
 * 状态: 暂停
 */
static void State_Pause_Handler(SystemState_t *state)
{
	/* 停止气泵 */
	HAL_Pump_Stop();
	state->pump.is_enabled = false;
	
	/* 等待用户恢复或停止 */
}

/**
 * 状态: 故障
 */
static void State_Error_Handler(SystemState_t *state)
{
	/* 停止气泵 */
	HAL_Pump_Stop();
	state->pump.is_enabled = false;
	
	/* 显示故障信息 */
	/* 发出报警 */
	
	/* 等待用户处理故障 */
}

/**
 * 状态: 关机
 */
static void State_Shutdown_Handler(SystemState_t *state)
{
	/* 显示保存提示 */
	LCD_Clear();
	LCD_DisplayString(2, 20, "Saving...");
	
	/* 保存所有系统设置到Flash */
	Flash_SaveSystemSettings(state);
	
	/* 延迟一下，让显示可见 */
	for (uint16_t i = 0; i < 25; i++)  // 0.5秒
	{
		HAL_Watchdog_Clear();
	}
	
	/* 显示关机提示 */
	LCD_DisplayString(4, 20, "Goodbye!");
	
	/* 关闭所有外设 */
	HAL_Pump_Enable(false);
	HAL_Valve1_Close();
	HAL_Valve2_Close();
	HAL_Buzzer_Off();
	HAL_LED_Green_Off();
	HAL_LED_Yellow_Off();
	
	/* 延迟一下 */
	for (uint16_t i = 0; i < 50; i++)  // 1秒
	{
		HAL_Watchdog_Clear();
	}
	
	/* 关闭LCD和背光 */
	LCD_Clear();
	LCD_SetBacklight(false);
	
	/* 释放电源 */
	HAL_Power_Release();
	
	/* 进入死循环等待断电 */
	while(1) {
		HAL_Watchdog_Clear();
	}
}

/**
 * 函数: AppStateMachine_HandleKey
 * 功能: 处理按键事件
 * 说明: 根据当前模式和按键类型，执行状态转换
 */
void AppStateMachine_HandleKey(SystemState_t *sys, KeyValue_e key, KeyEvent_e event)
{
	/* 按键锁定检查 */
	static uint16_t unlock_sequence_timer = 0;
	static KeyValue_e last_unlock_key = KEY_NONE;
	
	/* 解锁序列：连续按 上-下-上-下 */
	if (sys->ui.is_locked)
	{
		if (key == KEY_UP && last_unlock_key == KEY_NONE)
		{
			last_unlock_key = KEY_UP;
			unlock_sequence_timer = 0;
		}
		else if (key == KEY_DOWN && last_unlock_key == KEY_UP)
		{
			last_unlock_key = KEY_DOWN;
		}
		else if (key == KEY_UP && last_unlock_key == KEY_DOWN)
		{
			/* 解锁成功 */
			sys->ui.is_locked = false;
			last_unlock_key = KEY_NONE;
			AlarmManager_Beep(2);  // 两声提示解锁
			return;
		}
		else
		{
			last_unlock_key = KEY_NONE;
		}
		
		/* 锁定状态下忽略其他按键 */
		return;
	}
	
	/* 只处理按键按下事件 */
	if (event != KEY_EVENT_PRESS && event != KEY_EVENT_LONG_PRESS)
	{
		return;
	}
	
	/* 根据当前模式处理按键 */
	switch (sys->current_mode)
	{
		case MODE_INIT:
			/* 初始化模式：长按确认键开机 */
			if (key == KEY_CONFIRM && event == KEY_EVENT_LONG_PRESS)
			{
				AppStateMachine_SetMode(sys, MODE_STANDBY);
			}
			/* 初始化时长按静音键切换静音 */
			else if (key == KEY_CANCEL && event == KEY_EVENT_LONG_PRESS)
			{
				sys->alarm.is_muted = !sys->alarm.is_muted;
				AlarmManager_Beep(1);
			}
			break;
			
		case MODE_STANDBY:
			/* 待机模式 */
			if (key == KEY_UP)
			{
				/* 增加目标压力 */
				uint16_t new_target = sys->pressure.target_pressure + 5;
				PressureController_SetTarget(&sys->pressure, new_target);
			}
			else if (key == KEY_DOWN)
			{
				/* 减少目标压力 */
				if (sys->pressure.target_pressure > 5)
				{
					uint16_t new_target = sys->pressure.target_pressure - 5;
					PressureController_SetTarget(&sys->pressure, new_target);
				}
			}
			else if (key == KEY_CONFIRM)
			{
				/* 短按确认：进入工作模式 */
				if (sys->settings.continuous_time > 0)  // 根据设置选择模式
				{
					AppStateMachine_SetMode(sys, MODE_CONTINUOUS);
				}
			}
			else if (key == KEY_CONFIRM && event == KEY_EVENT_LONG_PRESS)
			{
				/* 长按确认：进入设置模式 */
				AppSettings_Enter(sys);
				AppStateMachine_SetMode(sys, MODE_SETTINGS);
			}
			else if (key == KEY_UP && event == KEY_EVENT_LONG_PRESS)
			{
				/* 长按上键：进入自检模式 */
				AppSelftest_Start(sys);
				AppStateMachine_SetMode(sys, MODE_SELFTEST);
			}
			else if (key == KEY_CANCEL && event == KEY_EVENT_LONG_PRESS)
			{
				/* 长按取消：关机 */
				AppStateMachine_SetMode(sys, MODE_SHUTDOWN);
			}
			break;
			
		case MODE_CONTINUOUS:
			/* 连续模式 */
			if (key == KEY_CONFIRM)
			{
				/* 暂停 */
				AppStateMachine_SetMode(sys, MODE_PAUSE);
			}
			else if (key == KEY_UP && key == KEY_DOWN && event == KEY_EVENT_LONG_PRESS)
			{
				/* 同时长按上下键：锁定按键 */
				sys->ui.is_locked = true;
				AlarmManager_Beep(3);  // 三声提示锁定
			}
			else if (key == KEY_CANCEL && event == KEY_EVENT_LONG_PRESS)
			{
				/* 长按取消：停止工作，返回待机 */
				AppStateMachine_SetMode(sys, MODE_STANDBY);
			}
			break;
			
		case MODE_INTERMITTENT:
			/* 间歇模式 */
			if (key == KEY_CONFIRM)
			{
				/* 暂停 */
				AppStateMachine_SetMode(sys, MODE_PAUSE);
			}
			else if (key == KEY_CANCEL && event == KEY_EVENT_LONG_PRESS)
			{
				/* 长按取消：停止工作，返回待机 */
				IntermittentController_Stop(&sys->intermittent);
				AppStateMachine_SetMode(sys, MODE_STANDBY);
			}
			break;
			
		case MODE_PAUSE:
			/* 暂停模式 */
			if (key == KEY_CONFIRM)
			{
				/* 继续：返回之前的模式 */
				AppStateMachine_SetMode(sys, sys->previous_mode);
			}
			else if (key == KEY_CANCEL)
			{
				/* 停止，返回待机 */
				AppStateMachine_SetMode(sys, MODE_STANDBY);
			}
			break;
			
		case MODE_SETTINGS:
			/* 设置模式：按键由AppSettings_HandleKey处理 */
			/* 这里不处理，在app_input.c中已分发 */
			break;
			
		case MODE_SELFTEST:
			/* 自检模式：按键停止自检 */
			if (key == KEY_CANCEL)
			{
				AppSelftest_Stop(sys);
				AppStateMachine_SetMode(sys, MODE_STANDBY);
			}
			break;
			
		case MODE_ERROR:
			/* 故障模式：任意键清除故障 */
			if (key != KEY_NONE)
			{
				sys->fault.current_error = ERROR_NONE;
				AppStateMachine_SetMode(sys, MODE_STANDBY);
			}
			break;
			
		default:
			break;
	}
}

