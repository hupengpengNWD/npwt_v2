/****************************************************************************
 * 文件名: key_driver.c
 * 功能: 按键扫描驱动实现
 * 
 * 说明: 
 *   实现按键扫描、消抖、长按检测
 *   按键连接到PORTB的RB2-RB5
 * 
 * 按键映射（低电平有效）:
 *   RB2 = 上键
 *   RB3 = 下键
 *   RB4 = 确认键
 *   RB5 = 取消键
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/key_driver.h"
#include "../../Core/Inc/system_config.h"
#include "../../HAL/Inc/hal_gpio.h"  // 包含 mcu_config.h → <xc.h> 寄存器定义

/* 按键GPIO读取宏 */
#define GET_KEY_PORT()  (PORTB & 0b00111100)  // 读取RB2-RB5

/* 按键值定义（低电平有效） */
#define KEY_PORT_UP      0b00111011  // RB2=0
#define KEY_PORT_DOWN    0b00110111  // RB3=0
#define KEY_PORT_CONFIRM 0b00101111  // RB4=0
#define KEY_PORT_CANCEL  0b00011111  // RB5=0
#define KEY_PORT_NONE    0b00111111  // 全部=1

/* 消抖次数 */
#define KEY_DEBOUNCE_COUNT  5

/**
 * 函数: Key_Init
 * 功能: 初始化按键驱动
 */
void Key_Init(KeyData_t *data)
{
	data->current_key = KEY_NONE;
	data->last_key = KEY_NONE;
	data->event = KEY_EVENT_NONE;
	data->press_time = 0;
	data->is_long_press = false;
	data->idle_time = 0;
}

/**
 * 函数: Key_PortToValue
 * 功能: 将端口值转换为按键值
 */
static KeyValue_e Key_PortToValue(uint8_t port)
{
	switch (port)
	{
		case KEY_PORT_UP:      return KEY_UP;
		case KEY_PORT_DOWN:    return KEY_DOWN;
		case KEY_PORT_CONFIRM: return KEY_CONFIRM;
		case KEY_PORT_CANCEL:  return KEY_CANCEL;
		default:               return KEY_NONE;
	}
}

/**
 * 函数: Key_Scan
 * 功能: 扫描按键（带消抖）
 */
KeyValue_e Key_Scan(KeyData_t *data)
{
	static uint8_t last_port = KEY_PORT_NONE;
	static uint8_t debounce_cnt = 0;
	
	/* 读取按键端口 */
	uint8_t current_port = GET_KEY_PORT();
	
	/* 消抖处理 */
	if (current_port == last_port)
	{
		if (debounce_cnt++ >= KEY_DEBOUNCE_COUNT)
		{
			debounce_cnt = KEY_DEBOUNCE_COUNT;  // 限制计数器
			
			KeyValue_e key = Key_PortToValue(current_port);
			
			/* 检测按键状态变化 */
			if (key != data->current_key)
			{
				data->last_key = data->current_key;
				data->current_key = key;
				
				if (key == KEY_NONE)
				{
					/* 按键释放 */
					data->event = KEY_EVENT_RELEASE;
					data->press_time = 0;
					data->is_long_press = false;
				}
				else
				{
					/* 按键按下 */
					data->event = KEY_EVENT_PRESS;
					data->press_time = 0;
					data->idle_time = 0;
				}
			}
			else if (key != KEY_NONE)
			{
				/* 按键保持按下状态 */
				data->press_time++;
				
				/* 检测长按（2秒 = 100个周期） */
				if (data->press_time >= KEY_LONG_PRESS && !data->is_long_press)
				{
					data->event = KEY_EVENT_LONG_PRESS;
					data->is_long_press = true;
				}
				else
				{
					data->event = KEY_EVENT_NONE;
				}
			}
			else
			{
				/* 无按键 */
				data->event = KEY_EVENT_NONE;
				data->idle_time++;
			}
		}
	}
	else
	{
		/* 端口值变化，重新开始消抖 */
		last_port = current_port;
		debounce_cnt = 0;
	}
	
	return data->current_key;
}

/**
 * 函数: Key_GetEvent
 * 功能: 获取按键事件
 */
KeyEvent_e Key_GetEvent(const KeyData_t *data)
{
	return data->event;
}

/**
 * 函数: Key_IsLongPress
 * 功能: 判断是否长按
 */
bool Key_IsLongPress(const KeyData_t *data)
{
	return data->is_long_press;
}

/**
 * 函数: Key_GetIdleTime
 * 功能: 获取按键空闲时间
 */
uint32_t Key_GetIdleTime(const KeyData_t *data)
{
	return (uint32_t)data->idle_time * SYSTEM_TICK_MS;
}

