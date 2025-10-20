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
#include "../../Middleware/Inc/pressure_controller.h"
#include "../../Middleware/Inc/battery_manager.h"
#include "../../Middleware/Inc/fault_detector.h"
#include "../../HAL/Inc/hal_gpio.h"

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
	state->mode = MODE_INIT;
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
		if (g_state_table[i].mode == state->mode)
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
	state->mode = mode;
}

/**
 * 函数: AppStateMachine_GetMode
 * 功能: 获取当前模式
 */
WorkMode_e AppStateMachine_GetMode(const SystemState_t *state)
{
	return state->mode;
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
	if (state->fault.active_error != ERROR_NONE)
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
	
	if (state->fault.active_error != ERROR_NONE)
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
	pump->is_enabled = false;
	
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
	/* 关闭所有外设 */
	HAL_Pump_Stop();
	HAL_Valve1_Close();
	HAL_Valve2_Close();
	HAL_Buzzer_Off();
	HAL_LED_Green_Off();
	HAL_LED_Yellow_Off();
	
	/* 释放电源 */
	HAL_Power_Release();
	
	/* 进入死循环等待断电 */
	while(1);
}

