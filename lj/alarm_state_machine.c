/****************************************************************************
 * 文件名: alarm_state_machine.c
 * 功能: 复杂蜂鸣器状态机实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/alarm_state_machine.h"
#include "../../HAL/Inc/hal_gpio.h"

/* 蜂鸣器控制参数 */
#define AUDIO_TIM        20      // 第一次响铃持续时间（周期数）
#define AUDIO_PERIOD     15      // 停顿时间（周期数）
#define MAX_BEE_TIME     200     // 最大响铃时间（超时保护）

/**
 * 函数: AlarmStateMachine_Init
 * 功能: 初始化蜂鸣器状态机
 */
void AlarmStateMachine_Init(AlarmStateMachine_t *machine)
{
	if (machine != NULL) {
		/* 初始化状态 */
		machine->current_state = BEE_STATE_GO;
		machine->audio_basic = 0;
		machine->audio_period = 0;
		machine->speaker_delay = 0;
		machine->speaker_state = false;
		machine->mute_enabled = false;
		machine->max_beep_time = MAX_BEE_TIME;
		
		/* 关闭蜂鸣器 */
		HAL_Buzzer_Off();
	}
}

/**
 * 函数: AlarmStateMachine_Process
 * 功能: 处理蜂鸣器状态机（主循环调用）
 * 说明: 对应旧代码AUDIO()函数中的BEE_TWO状态机
 */
void AlarmStateMachine_Process(AlarmStateMachine_t *machine)
{
	if (machine != NULL) {
		/* 超时保护：防止蜂鸣器一直响 */
		if (machine->speaker_state && machine->speaker_delay++ > machine->max_beep_time)
		{
			machine->speaker_delay = 15;
			machine->audio_period = 0;
			machine->current_state = BEE_STATE_TWO;
			HAL_Buzzer_Off();
			machine->speaker_state = false;
			return;
		}
		
		/* 如果静音，直接关闭蜂鸣器 */
		if (machine->mute_enabled)
		{
			HAL_Buzzer_Off();
			machine->speaker_state = false;
			return;
		}
		
		/* 状态机控制蜂鸣器的响铃模式 */
		switch (machine->current_state)
	{
		default:
			HAL_Buzzer_Off();
			machine->current_state = BEE_STATE_GO;
			machine->speaker_state = false;
			break;
			
		case BEE_STATE_GO:  // 初始状态：准备第一次响
		{
			machine->audio_period = 0;
			machine->audio_basic = 0;
			HAL_Buzzer_Off();
			machine->speaker_state = false;
			machine->current_state = BEE_STATE_ONE;
			break;
		}
		
		case BEE_STATE_ONE:  // 第一次响：持续AUDIO_TIM个周期
		{
			HAL_Buzzer_On();
			machine->speaker_state = true;
			if (machine->audio_basic++ > AUDIO_TIM)
			{
				machine->current_state = BEE_STATE_TWO;  // 转入停顿状态
				machine->audio_period = 0;
			}
			break;
		}
		
		case BEE_STATE_TWO:  // 停顿状态：持续AUDIO_PERIOD个周期
		{
			HAL_Buzzer_Off();
			machine->speaker_state = false;
			if (machine->audio_period++ > AUDIO_PERIOD)
			{
				machine->current_state = BEE_STATE_THREE;  // 转入第二次响或循环
			}
			break;
		}
		
		case BEE_STATE_THREE:  // 准备循环或结束
		{
			machine->audio_period = 0;
			machine->audio_basic = 0;
			HAL_Buzzer_Off();
			machine->speaker_state = false;
			machine->current_state = BEE_STATE_ONE;  // 重新开始循环
			break;
		}
	}
	}
}

/**
 * 函数: AlarmStateMachine_Start
 * 功能: 启动蜂鸣器报警
 */
void AlarmStateMachine_Start(AlarmStateMachine_t *machine)
{
	if (machine != NULL) {
		machine->current_state = BEE_STATE_GO;
		machine->audio_basic = 0;
		machine->audio_period = 0;
		machine->speaker_delay = 0;
	}
}

/**
 * 函数: AlarmStateMachine_Stop
 * 功能: 停止蜂鸣器报警
 */
void AlarmStateMachine_Stop(AlarmStateMachine_t *machine)
{
	if (machine != NULL) {
		machine->current_state = BEE_STATE_END;
		HAL_Buzzer_Off();
		machine->speaker_state = false;
		machine->audio_basic = 0;
		machine->audio_period = 0;
		machine->speaker_delay = 0;
	}
}

/**
 * 函数: AlarmStateMachine_SetMute
 * 功能: 设置静音状态
 */
void AlarmStateMachine_SetMute(AlarmStateMachine_t *machine, bool mute)
{
	if (machine != NULL) {
		machine->mute_enabled = mute;
		
		if (mute)
		{
			HAL_Buzzer_Off();
			machine->speaker_state = false;
		}
	}
}

/**
 * 函数: AlarmStateMachine_BeepKey
 * 功能: 按键音（短促单响）
 * 说明: 对应旧代码AUDIO_Key()函数
 */
void AlarmStateMachine_BeepKey(AlarmStateMachine_t *machine)
{
	if (machine == NULL || machine->mute_enabled)
		return;
	
	/* 短促按键音：开启100ms后关闭 */
	HAL_Buzzer_On();
	/* 注意：实际应用中需要定时器控制关闭时间 */
}

