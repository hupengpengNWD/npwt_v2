/****************************************************************************
 * 文件名: ui_enhancements.h
 * 功能: UI增强功能（对应旧代码UI细节）
 * 
 * 说明: 
 *   提供高级UI功能：泄漏显示、询问界面、语言选择等
 *   对应旧代码中的show_lq、DISP_ask、DISP_LANGUAGE等
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef UI_ENHANCEMENTS_H
#define UI_ENHANCEMENTS_H

#include <stdint.h>
#include <stdbool.h>

/* 泄漏显示结构体（对应旧代码show_lq_times） */
typedef struct {
	uint16_t leakage_times;           // 泄漏次数
	uint16_t show_times;              // 显示次数
	uint8_t step;                     // 显示步骤
	uint16_t buzzer_key_press_count;  // 蜂鸣器按键计数
} LeakageDisplay_t;

/* UI增强功能结构体 */
typedef struct {
	LeakageDisplay_t leakage_display;  // 泄漏显示
	bool language_selection_active;    // 语言选择激活
	bool ask_dialog_active;           // 询问对话框激活
	uint8_t current_language;         // 当前语言（0=英语，1=俄语）
} UIEnhancements_t;

/****************************************************************************
 * UI增强功能函数
 ****************************************************************************/

/**
 * 函数: UIEnhancements_Init
 * 功能: 初始化UI增强功能
 */
void UIEnhancements_Init(UIEnhancements_t *ui);

/**
 * 函数: UIEnhancements_ShowLeakage
 * 功能: 显示泄漏次数（对应旧代码show_lq）
 */
void UIEnhancements_ShowLeakage(UIEnhancements_t *ui);

/**
 * 函数: UIEnhancements_ShowAskDialog
 * 功能: 显示询问对话框（对应旧代码DISP_ask）
 */
void UIEnhancements_ShowAskDialog(UIEnhancements_t *ui);

/**
 * 函数: UIEnhancements_ShowLanguageSelection
 * 功能: 显示语言选择界面（对应旧代码DISP_LANGUAGE）
 */
void UIEnhancements_ShowLanguageSelection(UIEnhancements_t *ui);

/**
 * 函数: UIEnhancements_ClearLeakageCounters
 * 功能: 清除泄漏显示计数器
 */
void UIEnhancements_ClearLeakageCounters(UIEnhancements_t *ui);

/**
 * 函数: UIEnhancements_UpdateLeakageCount
 * 功能: 更新泄漏计数
 */
void UIEnhancements_UpdateLeakageCount(UIEnhancements_t *ui);

#endif /* UI_ENHANCEMENTS_H */

