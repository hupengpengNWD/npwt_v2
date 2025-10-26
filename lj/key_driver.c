/****************************************************************************
 * 文件名: key_driver.c
 * 功能: 按键扫描驱动实现
 * 
 * 说明: 
 *   实现按键扫描、消抖、长按检测
 *   按键连接到PORTB的RB2-RB5（低电平有效）
 * 
 * 按键映射（与未重构工程一致）:
 *   RB2 = 确认键 (KEY_CONFIRM / KEY_OK)    → 0x38 (bit2=0)
 *   RB3 = 上键   (KEY_UP)                  → 0x34 (bit3=0)
 *   RB4 = 下键   (KEY_DOWN)                → 0x2C (bit4=0)
 *   RB5 = 取消/静音键 (KEY_CANCEL / KEY_MUTE) → 0x1C (bit5=0)
 * 
 * 注意：
 *   - PORTB bit0-1 用于电磁阀输出（VAL1/VAL2），不参与按键扫描
 *   - 读取时使用掩码 0x3C 屏蔽 bit0-1
 * 
 * 创建日期: 2025-10-20
 * 修改日期: 2025-10-22 - 修正按键映射，与未重构工程一致
 ****************************************************************************/

#include "../Inc/key_driver.h"
#include "../../Core/Inc/system_config.h"
#include "../../HAL/Inc/hal_gpio.h"  // 包含 mcu_config.h → <xc.h> 寄存器定义

/* 按键GPIO读取宏 */
#define GET_KEY_PORT()  (PORTB & 0x3C)  // 读取RB2-RB5（屏蔽bit0-1）

/* 按键值定义（低电平有效，与未重构工程一致） */
#define KEY_PORT_UP      0x34  // 0b00110100 RB3=0（上键）
#define KEY_PORT_DOWN    0x2C  // 0b00101100 RB4=0（下键）
#define KEY_PORT_CONFIRM 0x38  // 0b00111000 RB2=0（确认/OK键）
#define KEY_PORT_CANCEL  0x1C  // 0b00011100 RB5=0（取消/静音键）
#define KEY_PORT_NONE    0x3C  // 0b00111100 全部=1（无按键）

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

/* 注意：Key_GetEvent, Key_IsLongPress, Key_GetIdleTime 等接口已删除
 * 这些功能现在直接在 app_input.c 中通过访问 KeyData_t 结构体实现 */

