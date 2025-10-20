/****************************************************************************
 * 文件名: alarm_manager.c
 * 功能: 报警管理器实现
 * 
 * 说明: 
 *   实现声音和LED报警功能
 *   不同故障有不同的报警模式
 * 
 * 报警模式：
 *   - 低电：连续短促音
 *   - 泄漏：双音（嘀嘀）
 *   - 堵塞：三音（嘀嘀嘀）
 *   - 液位满：长音
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/alarm_manager.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Core/Inc/system_config.h"

/* 蜂鸣器状态 */
typedef enum {
	BEEP_STATE_IDLE = 0,
	BEEP_STATE_BEEP1,       // 第一声
	BEEP_STATE_PAUSE1,      // 停顿
	BEEP_STATE_BEEP2,       // 第二声
	BEEP_STATE_PAUSE2,      // 停顿
	BEEP_STATE_BEEP3        // 第三声
} BeepState_e;

static BeepState_e g_beep_state = BEEP_STATE_IDLE;
static uint16_t g_beep_timer = 0;

/**
 * 函数: AlarmManager_Init
 * 功能: 初始化报警管理器
 */
void AlarmManager_Init(AlarmData_t *data)
{
	data->is_muted = false;
	data->mute_timer = 0;
	data->alarm_type = ALARM_TYPE_NONE;
	data->beep_counter = 0;
	data->beep_active = false;
	
	g_beep_state = BEEP_STATE_IDLE;
	g_beep_timer = 0;
	
	/* 关闭蜂鸣器和LED */
	HAL_Buzzer_Off();
	HAL_LED_Green_Off();
	HAL_LED_Yellow_Off();
}

/**
 * 函数: AlarmManager_DetermineAlarmType
 * 功能: 根据系统状态确定报警类型
 */
static AlarmType_e AlarmManager_DetermineAlarmType(const BatteryData_t *battery,
                                                   const FaultData_t *fault)
{
	/* 优先级从高到低 */
	
	/* 1. 严重低电（最高优先级） */
	if (battery->level == 0 && !battery->is_charging)
	{
		return ALARM_TYPE_BATTERY_CRITICAL;
	}
	
	/* 2. 故障报警 */
	if (fault->liquid_full)
		return ALARM_TYPE_LIQUID_FULL;
	
	if (fault->leakage_detected)
		return ALARM_TYPE_LEAKAGE;
	
	if (fault->blockage_detected)
		return ALARM_TYPE_BLOCKAGE;
	
	/* 3. 低电警告 */
	if (battery->is_low && !battery->is_charging)
	{
		return ALARM_TYPE_BATTERY_LOW;
	}
	
	return ALARM_TYPE_NONE;
}

/**
 * 函数: AlarmManager_PlayBeep
 * 功能: 播放蜂鸣器（不同模式）
 */
void AlarmManager_PlayBeep(AlarmData_t *data)
{
	/* 如果静音，不播放 */
	if (data->is_muted)
	{
		HAL_Buzzer_Off();
		return;
	}
	
	/* 根据报警类型播放不同模式 */
	switch (data->alarm_type)
	{
		case ALARM_TYPE_BATTERY_CRITICAL:
		case ALARM_TYPE_LIQUID_FULL:
			/* 连续长音 */
			if (g_beep_timer++ < 30)  // 响600ms
			{
				HAL_Buzzer_On();
			}
			else if (g_beep_timer < 40)  // 停200ms
			{
				HAL_Buzzer_Off();
			}
			else
			{
				g_beep_timer = 0;
			}
			break;
			
		case ALARM_TYPE_LEAKAGE:
		case ALARM_TYPE_BLOCKAGE:
			/* 双音（嘀嘀） */
			switch (g_beep_state)
			{
				case BEEP_STATE_IDLE:
					g_beep_state = BEEP_STATE_BEEP1;
					g_beep_timer = 0;
					break;
					
				case BEEP_STATE_BEEP1:
					HAL_Buzzer_On();
					if (g_beep_timer++ >= 10)  // 响200ms
					{
						g_beep_state = BEEP_STATE_PAUSE1;
						g_beep_timer = 0;
					}
					break;
					
				case BEEP_STATE_PAUSE1:
					HAL_Buzzer_Off();
					if (g_beep_timer++ >= 10)  // 停200ms
					{
						g_beep_state = BEEP_STATE_BEEP2;
						g_beep_timer = 0;
					}
					break;
					
				case BEEP_STATE_BEEP2:
					HAL_Buzzer_On();
					if (g_beep_timer++ >= 10)  // 响200ms
					{
						g_beep_state = BEEP_STATE_PAUSE2;
						g_beep_timer = 0;
					}
					break;
					
				case BEEP_STATE_PAUSE2:
					HAL_Buzzer_Off();
					if (g_beep_timer++ >= 100)  // 停2秒
					{
						g_beep_state = BEEP_STATE_IDLE;
						g_beep_timer = 0;
					}
					break;
					
				default:
					g_beep_state = BEEP_STATE_IDLE;
					break;
			}
			break;
			
		case ALARM_TYPE_BATTERY_LOW:
			/* 单音提示 */
			if (g_beep_timer++ < 5)
			{
				HAL_Buzzer_On();
			}
			else if (g_beep_timer < 100)
			{
				HAL_Buzzer_Off();
			}
			else
			{
				g_beep_timer = 0;
			}
			break;
			
		default:
			HAL_Buzzer_Off();
			g_beep_state = BEEP_STATE_IDLE;
			g_beep_timer = 0;
			break;
	}
	
	/* 防止蜂鸣器一直响（超时保护） */
	if (data->beep_counter++ > BEEPER_MAX_TIME * 50)
	{
		HAL_Buzzer_Off();
		data->beep_counter = 0;
	}
}

/**
 * 函数: AlarmManager_Update
 * 功能: 更新报警状态
 */
void AlarmManager_Update(AlarmData_t *data,
                        const BatteryData_t *battery,
                        const FaultData_t *fault)
{
	/* 确定当前报警类型 */
	data->alarm_type = AlarmManager_DetermineAlarmType(battery, fault);
	
	/* 更新LED指示 */
	if (data->alarm_type != ALARM_TYPE_NONE)
	{
		/* 有报警，LED闪烁 */
		static uint16_t led_timer = 0;
		if (led_timer++ < 25)
		{
			HAL_LED_Green_On();
		}
		else if (led_timer < 50)
		{
			HAL_LED_Green_Off();
		}
		else
		{
			led_timer = 0;
		}
	}
	else
	{
		/* 无报警，LED常亮 */
		HAL_LED_Green_On();
	}
	
	/* 播放蜂鸣器 */
	if (data->alarm_type != ALARM_TYPE_NONE)
	{
		data->beep_active = true;
		AlarmManager_PlayBeep(data);
	}
	else
	{
		data->beep_active = false;
		HAL_Buzzer_Off();
		g_beep_state = BEEP_STATE_IDLE;
	}
	
	/* 静音自动取消（5分钟后） */
	if (data->is_muted)
	{
		AlarmManager_CancelMute(data);
	}
}

/**
 * 函数: AlarmManager_SetMute
 * 功能: 设置静音
 */
void AlarmManager_SetMute(AlarmData_t *data, bool mute)
{
	data->is_muted = mute;
	
	if (mute)
	{
		data->mute_timer = 0;
		HAL_Buzzer_Off();
	}
}

/**
 * 函数: AlarmManager_IsMuted
 * 功能: 检查是否静音
 */
bool AlarmManager_IsMuted(const AlarmData_t *data)
{
	return data->is_muted;
}

/**
 * 函数: AlarmManager_CancelMute
 * 功能: 自动取消静音（5分钟后）
 */
void AlarmManager_CancelMute(AlarmData_t *data)
{
	if (data->mute_timer++ >= MUTE_TIMEOUT)
	{
		data->is_muted = false;
		data->mute_timer = 0;
	}
}

