/****************************************************************************
 * 文件名: key_driver.h
 * 功能: 按键扫描驱动
 * 
 * 说明: 
 *   实现按键扫描、消抖、长按检测
 *   支持4个按键：上、下、确认、取消
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef KEY_DRIVER_H
#define KEY_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 按键定义
 ****************************************************************************/
typedef enum {
	KEY_NONE = 0,           // 无按键
	KEY_UP,                 // 上键
	KEY_DOWN,               // 下键
	KEY_CONFIRM,            // 确认键
	KEY_CANCEL              // 取消键
} KeyValue_e;

/****************************************************************************
 * 按键事件
 ****************************************************************************/
typedef enum {
	KEY_EVENT_NONE = 0,     // 无事件
	KEY_EVENT_PRESS,        // 短按
	KEY_EVENT_LONG_PRESS,   // 长按
	KEY_EVENT_RELEASE       // 释放
} KeyEvent_e;

/****************************************************************************
 * 按键数据结构
 ****************************************************************************/
typedef struct {
	KeyValue_e  current_key;    // 当前按键
	KeyValue_e  last_key;       // 上次按键
	KeyEvent_e  event;          // 按键事件
	uint16_t    press_time;     // 按下时间（系统周期）
	bool        is_long_press;  // 是否长按
	uint16_t    idle_time;      // 空闲时间
} KeyData_t;

/****************************************************************************
 * 按键驱动函数
 ****************************************************************************/

/**
 * 函数: Key_Init
 * 功能: 初始化按键驱动
 */
void Key_Init(KeyData_t *data);

/**
 * 函数: Key_Scan
 * 功能: 扫描按键（每20ms调用一次）
 * 返回: 按键值
 */
KeyValue_e Key_Scan(KeyData_t *data);

/**
 * 函数: Key_GetEvent
 * 功能: 获取按键事件
 * 返回: 按键事件类型
 */
KeyEvent_e Key_GetEvent(const KeyData_t *data);

/**
 * 函数: Key_IsLongPress
 * 功能: 判断是否长按
 * 返回: true=长按, false=短按或未按
 */
bool Key_IsLongPress(const KeyData_t *data);

/**
 * 函数: Key_GetIdleTime
 * 功能: 获取按键空闲时间
 * 返回: 空闲时间（毫秒）
 */
uint32_t Key_GetIdleTime(const KeyData_t *data);

#endif /* KEY_DRIVER_H */

