/****************************************************************************
 * 文件名: app_input.c
 * 功能: 应用层输入处理实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/app_input.h"
#include "../Inc/app_settings.h"
#include "../Inc/app_state_machine.h"
#include "../../Middleware/Inc/alarm_manager.h"
#include "../../Drivers/Inc/key_driver.h"

/**
 * 函数: AppInput_Process
 * 功能: 处理按键输入（包含按键音）
 */
void AppInput_Process(SystemState_t *sys, KeyData_t *key_data)
{
	KeyValue_e key = key_data->current_key;
	KeyEvent_e event = key_data->event;
	
	/* 按键音处理：只在按键按下时响一次 */
	if (event == KEY_EVENT_PRESS)
	{
		AlarmManager_Beep(1);  // 短"嘀"声
	}
	
	/* 根据模式分发按键事件 */
	if (key != KEY_NONE)
	{
		if (sys->current_mode == MODE_SETTINGS)
		{
			/* 设置模式：特殊处理 */
			AppSettings_HandleKey(sys, key);
		}
		else
		{
			/* 其他模式：交给状态机处理 */
			AppStateMachine_HandleKey(sys, key, event);
		}
	}
}

