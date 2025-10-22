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
 * 功能: 开机检测和初始化
 * 
 * 逻辑（与未重构工程一致）：
 *   1. 前50个周期（1秒）检测确认键是否持续按下
 *   2. 累计按键时间必须 ≥ 30个周期（600ms）
 *   3. 按键时间足够 → 开机成功，POWER_ON=1
 *   4. 按键时间不足 → 关机，复位计时器，重新检测
 *   5. 开机后显示LOGO和版本号
 *   6. 350个周期（7秒）后进入待机模式
 */
static void State_Init_Handler(SystemState_t *state)
{
	static uint16_t init_timer = 0;
	static uint16_t key_hold_time = 0;
	static bool power_on_confirmed = false;
	
	init_timer++;
	
	/* 阶段1：按键检测阶段（前50个周期 = 1秒） */
	if (init_timer <= 50)
	{
		/* 检测确认键是否按下（直接读取GPIO） */
		/* 注意：使用与 key_driver.c 相同的方式读取 */
		uint8_t key_port = PORTB & 0x3C;
		if (key_port == 0x38)  // KEY_CONFIRM = 0x38 (RB2=0)
		{
			key_hold_time++;
		}
		else
		{
			key_hold_time = 0;  // 松开按键，计时器清零
		}
	}
	/* 阶段2：判断是否开机（第51个周期） */
	else if (init_timer == 51)
	{
		if (key_hold_time < 30)  // 按键时间不足（< 600ms）
		{
			/* 开机失败：关机，复位，重新检测 */
			HAL_Power_Release();  // POWER_ON = 0
			init_timer = 0;
			key_hold_time = 0;
			power_on_confirmed = false;
			return;
		}
		else  // 按键时间足够（≥ 600ms）
		{
			/* 开机成功 */
			HAL_Power_Hold();  // POWER_ON = 1 (RC2 = 1)
			power_on_confirmed = true;
			
			/* LCD已在 System_InitHardware() 中初始化，这里只需打开背光 */
			LCD_SetBacklight(true);
			
			/* 使能驱动（与未重构工程一致）*/
			LATCbits.LATC3 = 1;    // DRV_EN = 1
			HAL_Valve2_Open();     // VAL2 = 1
			
			/* 蜂鸣器提示（短促一声） */
			HAL_Buzzer_On();
		}
	}
	/* 阶段3：显示开机画面（51~250周期 = 1~5秒） */
	else if (init_timer > 51 && init_timer <= 250 && power_on_confirmed)
	{
		/* 关闭蜂鸣器（在62个周期后，约1.24秒） */
		if (init_timer > 62)
		{
			HAL_Buzzer_Off();
		}
		
		/* 显示开机LOGO（已在 LCD_DisplayStartup 中显示） */
		/* 注意：开机画面只显示一次，在 main.c 的初始化中已调用 */
	}
	/* 阶段4：显示版本号（250~350周期 = 5~7秒） */
	else if (init_timer > 250 && init_timer <= 350 && power_on_confirmed)
	{
		/* 可以在这里显示版本号 */
		/* LCD_DisplayVersion(); */
	}
	/* 阶段5：完成初始化，进入待机模式（350周期 = 7秒后） */
	else if (init_timer >= 350 && power_on_confirmed)
	{
		/* 清屏 */
		LCD_Clear();
		
		/* 进入待机模式 */
		AppStateMachine_SetMode(state, MODE_STANDBY);
		state->system_ready = true;
		
		/* 复位计时器 */
		init_timer = 0;
		key_hold_time = 0;
		power_on_confirmed = false;
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
 * 功能: 保存配置并断电
 * 
 * 逻辑（与未重构工程一致）：
 *   1. 立即停止所有工作
 *   2. 清屏显示关机信息
 *   3. 等待1秒（50个周期）
 *   4. 保存配置到Flash
 *   5. 关闭LCD和背光
 *   6. 断电（POWER_ON = 0）
 */
static void State_Shutdown_Handler(SystemState_t *state)
{
	static uint16_t shutdown_timer = 0;
	static bool flash_saved = false;
	
	/* 阶段1：立即关闭外设（只执行一次） */
	if (shutdown_timer == 0)
	{
		/* 停止所有工作 */
		HAL_Pump_Stop();
		HAL_Pump_Enable(false);
		HAL_Valve1_Close();
		HAL_Valve2_Close();
		HAL_Buzzer_Off();
		HAL_LED_Green_Off();
		HAL_LED_Yellow_Off();
		
		/* 清屏并显示关机信息 */
		LCD_Clear();
		/* 不同于未重构工程，这里可以显示友好的关机信息 */
		/* 未重构工程在低电量时会显示警告，正常关机则关闭LCD */
	}
	
	shutdown_timer++;
	
	/* 阶段2：等待1秒后保存配置（50个周期 = 1秒） */
	if (shutdown_timer >= 50 && !flash_saved)
	{
		/* 检查电池电量（与未重构工程一致） */
		if (!state->battery.is_low)  // 电量足够
		{
			/* 保存配置到Flash */
			Flash_SaveSystemSettings(state);
		}
		/* 电量过低时不保存，避免Flash写入失败 */
		
		flash_saved = true;
		
		/* 关闭背光 */
		LCD_SetBacklight(false);
		
		/* 关闭LCD显示 */
		LCD_Clear();
		/* TODO: 添加 LCD_PowerOff() 如果有的话 */
	}
	
	/* 阶段3：再等待0.2秒后断电（60个周期） */
	if (shutdown_timer >= 60)
	{
		/* 释放电源自锁 */
		HAL_Power_Release();  // POWER_ON = 0
		
		/* 复位计时器（实际不会执行到，因为已断电） */
		shutdown_timer = 0;
		flash_saved = false;
		
		/* 进入死循环等待断电（如果POWER_ON没有生效） */
		while(1)
		{
			HAL_Watchdog_Clear();
		}
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

