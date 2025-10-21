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
#include "../../Core/Inc/system_enums.h"  // 使用统一的枚举定义

/* 注意：KeyValue_e 和 KeyEvent_e 已在 system_enums.h 中定义 */

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

/* 注意：以下接口已删除，直接访问 KeyData_t 结构体成员：
 * - Key_GetEvent()      → data->event
 * - Key_IsLongPress()   → data->is_long_press
 * - Key_GetIdleTime()   → data->idle_time
 */

#endif /* KEY_DRIVER_H */

