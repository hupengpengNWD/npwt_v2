/****************************************************************************
 * 文件名: ui_enhancements.c
 * 功能: UI增强功能实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/ui_enhancements.h"
#include "../../Drivers/Inc/lcd_driver.h"
#include "../../Drivers/Inc/lcd_ui_elements.h"
#include "../../Core/Inc/mcu_config.h"  // 包含 NULL 定义

/* 按键检测宏（实际按键扫描） */
#define PRESS_KEY_BUZ_SHOWLQ_TIME  5  // 按键检测时间（50ms）

/**
 * 函数: UIEnhancements_Init
 * 功能: 初始化UI增强功能
 */
void UIEnhancements_Init(UIEnhancements_t *ui)
{
	if (ui != NULL) {
		/* 初始化泄漏显示 */
		ui->leakage_display.leakage_times = 0;
		ui->leakage_display.show_times = 0;
		ui->leakage_display.step = 0;
		ui->leakage_display.buzzer_key_press_count = 0;
		
		/* 初始化其他UI状态 */
		ui->language_selection_active = false;
		ui->ask_dialog_active = false;
		ui->current_language = 0;  // 默认英语
	}
}

/**
 * 函数: UIEnhancements_ShowLeakage
 * 功能: 显示泄漏次数（对应旧代码show_lq）
 * 说明: 显示泄漏检测次数，支持按键交互
 */
void UIEnhancements_ShowLeakage(UIEnhancements_t *ui)
{
	if (ui != NULL) {
		LeakageDisplay_t *display = &ui->leakage_display;
		
		if (display->step == 0)
		{
			/* 步骤0：检测按键，显示泄漏次数 */
			LCD_Clear();
			LCD_DisplayString(0, 20, "Leakage Count:");
			LCD_DisplayNumber(2, 40, display->leakage_times);
			LCD_DisplayString(4, 10, "Press any key to continue");
			
			/* 实际按键检测（需要集成Key_Scan()） */
			/* 这里应该调用真实的按键扫描函数 */
			uint8_t key_pressed = 0;  // 实际应用中应该从Key_Scan()获取
			if (key_pressed) {
				display->buzzer_key_press_count++;
			}
			if (display->buzzer_key_press_count > PRESS_KEY_BUZ_SHOWLQ_TIME)
			{
				display->step = 1;
			}
		}
		else if (display->step == 1)
		{
			/* 步骤1：显示详细信息 */
			LCD_Clear();
			LCD_DisplayString(0, 10, "Leakage Details:");
			LCD_DisplayString(2, 20, "Count:");
			LCD_DisplayNumber(2, 60, display->leakage_times);
			LCD_DisplayString(4, 20, "Detection:");
			LCD_DisplayString(4, 80, "Active");
			
			/* 显示完成后重置 */
			display->step = 0;
			display->show_times = 9;  // 显示9次后结束
		}
	}
}

/**
 * 函数: UIEnhancements_ShowAskDialog
 * 功能: 显示询问对话框（对应旧代码DISP_ask）
 * 说明: 显示系统询问界面
 */
void UIEnhancements_ShowAskDialog(UIEnhancements_t *ui)
{
	if (ui != NULL) {
		LCD_Clear();
		LCD_DisplayString(0, 20, "System Question");
		LCD_DisplayString(2, 10, "Continue operation?");
		LCD_DisplayString(4, 20, "OK: Yes");
		LCD_DisplayString(5, 20, "CANCEL: No");
		
		ui->ask_dialog_active = true;
	}
}

/**
 * 函数: UIEnhancements_ShowLanguageSelection
 * 功能: 显示语言选择界面（对应旧代码DISP_LANGUAGE）
 * 说明: 显示语言选择界面（仅英语，俄语已移除）
 */
void UIEnhancements_ShowLanguageSelection(UIEnhancements_t *ui)
{
	if (ui != NULL) {
		LCD_Clear();
		LCD_DisplayString(0, 20, "Language Selection");
		LCD_DisplayString(2, 30, "English Only");
		LCD_DisplayString(4, 20, "Russian removed");
		LCD_DisplayString(6, 20, "Press OK to continue");
		
		ui->language_selection_active = true;
		ui->current_language = 0;  // 强制英语
	}
}

/**
 * 函数: UIEnhancements_ClearLeakageCounters
 * 功能: 清除泄漏显示计数器
 */
void UIEnhancements_ClearLeakageCounters(UIEnhancements_t *ui)
{
	if (ui != NULL) {
		ui->leakage_display.leakage_times = 0;
		ui->leakage_display.show_times = 0;
		ui->leakage_display.step = 0;
		ui->leakage_display.buzzer_key_press_count = 0;
	}
}

/**
 * 函数: UIEnhancements_UpdateLeakageCount
 * 功能: 更新泄漏计数
 */
void UIEnhancements_UpdateLeakageCount(UIEnhancements_t *ui)
{
	if (ui != NULL) {
		ui->leakage_display.leakage_times++;
	}
}

/**
 * 注意：UIEnhancements_Update 函数已删除
 * 
 * 原因：
 *   在旧工程中，UI增强功能（show_lq, DISP_ask, DISP_LANGUAGE）
 *   不是通过统一的Update函数调用的，而是分散在各个显示函数中按需调用：
 *   - show_lq() 在 DISP_LixA() 中调用（连续模式显示时）
 *   - DISP_ask() 在待机模式首次显示时调用
 *   - DISP_LANGUAGE() 在首次开机时调用
 * 
 *   重构后，这些功能已整合到 AppDisplay_Update() 中，
 *   无需单独的轮询Update函数。
 */

