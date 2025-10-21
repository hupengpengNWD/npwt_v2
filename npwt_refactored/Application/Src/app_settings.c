/****************************************************************************
 * 文件名: app_settings.c
 * 功能: 应用层设置菜单实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/app_settings.h"
#include "../../Drivers/Inc/lcd_driver.h"
#include "../../Drivers/Inc/lcd_ui_elements.h"
#include "../../Middleware/Inc/alarm_manager.h"
#include "../../Drivers/Inc/flash_driver.h"  // Flash_SaveSystemSettings
#include "../Inc/app_state_machine.h"  // AppStateMachine_SetMode

/* 前向声明 */
static void AppSettings_HandleKeyLongPress(SystemState_t *sys, KeyValue_e key);

/* 设置值范围 */
#define CONTINUOUS_TIME_MIN     1
#define CONTINUOUS_TIME_MAX     9999
#define INTERMITTENT_TIME_MIN   1
#define INTERMITTENT_TIME_MAX   9999
#define PRESSURE_MIN_VALUE      20
#define PRESSURE_MAX_VALUE      300

/**
 * 函数: AppSettings_Init
 * 功能: 初始化设置模块
 */
void AppSettings_Init(SystemSettings_t *settings)
{
	settings->current_item = SETTING_NONE;
	settings->cursor_position = 0;
	settings->continuous_time = 9999;  // 默认无限时间
	settings->is_in_setting_mode = false;
}

/**
 * 函数: AppSettings_Enter
 * 功能: 进入设置模式
 */
void AppSettings_Enter(SystemState_t *sys)
{
	sys->settings.is_in_setting_mode = true;
	sys->settings.current_item = SETTING_WORKMODE_SELECT;
	sys->settings.cursor_position = 0;
}

/**
 * 函数: AppSettings_Exit
 * 功能: 退出设置模式（保存设置）
 */
void AppSettings_Exit(SystemState_t *sys)
{
	sys->settings.is_in_setting_mode = false;
	sys->settings.current_item = SETTING_NONE;
	
	/* 保存设置到Flash */
	Flash_SaveSystemSettings(sys);
}

/**
 * 函数: AppSettings_HandleKey
 * 功能: 处理设置模式的按键
 */
void AppSettings_HandleKey(SystemState_t *sys, KeyValue_e key)
{
	SystemSettings_t *settings = &sys->settings;
	static KeyValue_e last_key = KEY_NONE;
	static uint16_t long_press_counter = 0;
	
	/* 检测长按（用于快速调节） */
	if (key == last_key && (key == KEY_UP || key == KEY_DOWN))
	{
		long_press_counter++;
		
		/* 长按超过20个周期（400ms）后进入快速调节 */
		if (long_press_counter > 20 && long_press_counter % 5 == 0)
		{
			AppSettings_HandleKeyLongPress(sys, key);
			return;
		}
	}
	else
	{
		long_press_counter = 0;
		last_key = key;
	}
	
	switch (key)
	{
		case KEY_UP:
			/* 上键：增加值或移动光标 */
			switch (settings->current_item)
			{
				case SETTING_WORKMODE_SELECT:
					/* 切换工作模式 */
					if (sys->current_mode == MODE_CONTINUOUS) {
						sys->current_mode = MODE_INTERMITTENT;
					} else {
						sys->current_mode = MODE_CONTINUOUS;
					}
					break;
					
				case SETTING_CONTINUOUS_TIME:
					if (settings->continuous_time < CONTINUOUS_TIME_MAX) {
						settings->continuous_time++;
					}
					break;
					
				case SETTING_INTERMITTENT_HIGH_TIME:
					if (sys->intermittent.high_time < INTERMITTENT_TIME_MAX) {
						sys->intermittent.high_time++;
					}
					break;
					
				case SETTING_INTERMITTENT_LOW_TIME:
					if (sys->intermittent.low_time < INTERMITTENT_TIME_MAX) {
						sys->intermittent.low_time++;
					}
					break;
					
				case SETTING_INTERMITTENT_STOP_TIME:
					if (sys->intermittent.stop_time < INTERMITTENT_TIME_MAX) {
						sys->intermittent.stop_time++;
					}
					break;
					
				case SETTING_INTERMITTENT_LOW_PRESSURE:
					if (sys->intermittent.low_pressure < sys->intermittent.high_pressure - 10) {
						sys->intermittent.low_pressure += 5;
					}
					break;
					
				case SETTING_PRESSURE_CALIBRATION:
					sys->pressure.calibration_k += 0.01f;
					break;
					
				default:
					break;
			}
			break;
			
		case KEY_DOWN:
			/* 下键：减少值 */
			switch (settings->current_item)
			{
				case SETTING_CONTINUOUS_TIME:
					if (settings->continuous_time > CONTINUOUS_TIME_MIN) {
						settings->continuous_time--;
					}
					break;
					
				case SETTING_INTERMITTENT_HIGH_TIME:
					if (sys->intermittent.high_time > INTERMITTENT_TIME_MIN) {
						sys->intermittent.high_time--;
					}
					break;
					
				case SETTING_INTERMITTENT_LOW_TIME:
					if (sys->intermittent.low_time > INTERMITTENT_TIME_MIN) {
						sys->intermittent.low_time--;
					}
					break;
					
				case SETTING_INTERMITTENT_STOP_TIME:
					if (sys->intermittent.stop_time > INTERMITTENT_TIME_MIN) {
						sys->intermittent.stop_time--;
					}
					break;
					
				case SETTING_INTERMITTENT_LOW_PRESSURE:
					if (sys->intermittent.low_pressure > PRESSURE_MIN_VALUE) {
						sys->intermittent.low_pressure -= 5;
					}
					break;
					
				case SETTING_PRESSURE_CALIBRATION:
					if (sys->pressure.calibration_k > 0.1f) {
						sys->pressure.calibration_k -= 0.01f;
					}
					break;
					
				default:
					break;
			}
			break;
			
		case KEY_CONFIRM:
			/* 确认键：切换到下一设置项 */
			if (settings->current_item < SETTING_PRESSURE_CALIBRATION) {
				settings->current_item++;
			} else {
				/* 最后一项，退出设置 */
				AppSettings_Exit(sys);
				AppStateMachine_SetMode(sys, MODE_STANDBY);
			}
			break;
			
		case KEY_CANCEL:
			/* 取消键：退出设置（不保存） */
			AppSettings_Exit(sys);
			AppStateMachine_SetMode(sys, MODE_STANDBY);
			break;
			
		default:
			break;
	}
}

/**
 * 函数: AppSettings_Display
 * 功能: 显示设置界面（带光标）
 */
void AppSettings_Display(const SystemState_t *sys)
{
	const SystemSettings_t *settings = &sys->settings;
	
	static uint8_t last_item = SETTING_NONE;
	
	/* 切换设置项时清屏 */
	if (settings->current_item != last_item)
	{
		LCD_Clear();
		last_item = settings->current_item;
	}
	
	LCD_DisplayString(0, 20, "-- Settings --");
	
	/* 根据当前设置项显示对应界面 */
	switch (settings->current_item)
	{
		case SETTING_WORKMODE_SELECT:
			LCD_DisplayString(2, 10, "Work Mode:");
			
			/* 显示光标（三角形） */
			LCD_DisplayCursor(3, 10);
			
			if (sys->current_mode == MODE_CONTINUOUS) {
				LCD_DisplayString(3, 25, "Continuous");
			} else {
				LCD_DisplayString(3, 25, "Intermittent");
			}
			break;
			
		case SETTING_CONTINUOUS_TIME:
			LCD_DisplayString(2, 10, "Continuous Time:");
			LCD_DisplayCursor(3, 10);
			LCD_DisplayNumber(3, 40, settings->continuous_time);
			LCD_DisplayString(3, 80, "min");
			break;
			
		case SETTING_INTERMITTENT_HIGH_TIME:
			LCD_DisplayString(2, 10, "High P Time:");
			LCD_DisplayCursor(3, 10);
			LCD_DisplayNumber(3, 40, sys->intermittent.high_time);
			LCD_DisplayString(3, 80, "min");
			break;
			
		case SETTING_INTERMITTENT_LOW_TIME:
			LCD_DisplayString(2, 10, "Low P Time:");
			LCD_DisplayCursor(3, 10);
			LCD_DisplayNumber(3, 40, sys->intermittent.low_time);
			LCD_DisplayString(3, 80, "min");
			break;
			
		case SETTING_INTERMITTENT_STOP_TIME:
			LCD_DisplayString(2, 10, "Stop Time:");
			LCD_DisplayCursor(3, 10);
			LCD_DisplayNumber(3, 40, sys->intermittent.stop_time);
			LCD_DisplayString(3, 80, "min");
			break;
			
		case SETTING_INTERMITTENT_LOW_PRESSURE:
			LCD_DisplayString(2, 10, "Low Pressure:");
			LCD_DisplayCursor(3, 10);
			LCD_DisplayNumber(3, 40, sys->intermittent.low_pressure);
			LCD_DisplayString(3, 90, "mmHg");
			break;
			
		case SETTING_PRESSURE_CALIBRATION:
			LCD_DisplayString(2, 10, "Calibration K:");
			LCD_DisplayCursor(3, 10);
			/* 显示浮点数（简化为整数部分+小数部分） */
			uint16_t k_int = (uint16_t)sys->pressure.calibration_k;
			uint16_t k_dec = (uint16_t)((sys->pressure.calibration_k - k_int) * 100);
			LCD_DisplayNumber(3, 40, k_int);
			LCD_DisplayString(3, 60, ".");
			LCD_DisplayNumber(3, 70, k_dec);
			break;
			
		default:
			break;
	}
	
	/* 显示操作提示 */
	LCD_DisplayString(6, 0, "UP/DN:Adjust");
	LCD_DisplayString(7, 0, "OK:Next CANCEL:Exit");
}

/**
 * 函数: AppSettings_HandleKeyLongPress
 * 功能: 处理设置模式的长按快速调节
 * 说明: 长按上/下键时快速增减数值
 */
static void AppSettings_HandleKeyLongPress(SystemState_t *sys, KeyValue_e key)
{
	SystemSettings_t *settings = &sys->settings;
	
	/* 长按加速调节（每次+10或+5） */
	switch (key)
	{
		case KEY_UP:
			switch (settings->current_item)
			{
				case SETTING_CONTINUOUS_TIME:
					settings->continuous_time += 10;
					if (settings->continuous_time > 9999) {
						settings->continuous_time = 9999;
					}
					break;
					
				case SETTING_INTERMITTENT_HIGH_TIME:
				case SETTING_INTERMITTENT_LOW_TIME:
				case SETTING_INTERMITTENT_STOP_TIME:
					/* 快速调节：+5分钟 */
					if (settings->current_item == SETTING_INTERMITTENT_HIGH_TIME) {
						sys->intermittent.high_time += 5;
					} else if (settings->current_item == SETTING_INTERMITTENT_LOW_TIME) {
						sys->intermittent.low_time += 5;
					} else {
						sys->intermittent.stop_time += 5;
					}
					break;
					
				case SETTING_INTERMITTENT_LOW_PRESSURE:
					sys->intermittent.low_pressure += 10;
					break;
					
				default:
					break;
			}
			break;
			
		case KEY_DOWN:
			switch (settings->current_item)
			{
				case SETTING_CONTINUOUS_TIME:
					if (settings->continuous_time > 10) {
						settings->continuous_time -= 10;
					}
					break;
					
				case SETTING_INTERMITTENT_HIGH_TIME:
				case SETTING_INTERMITTENT_LOW_TIME:
				case SETTING_INTERMITTENT_STOP_TIME:
					/* 快速调节：-5分钟 */
					if (settings->current_item == SETTING_INTERMITTENT_HIGH_TIME) {
						if (sys->intermittent.high_time > 5) {
							sys->intermittent.high_time -= 5;
						}
					} else if (settings->current_item == SETTING_INTERMITTENT_LOW_TIME) {
						if (sys->intermittent.low_time > 5) {
							sys->intermittent.low_time -= 5;
						}
					} else {
						if (sys->intermittent.stop_time > 5) {
							sys->intermittent.stop_time -= 5;
						}
					}
					break;
					
				case SETTING_INTERMITTENT_LOW_PRESSURE:
					if (sys->intermittent.low_pressure > 10) {
						sys->intermittent.low_pressure -= 10;
					}
					break;
					
				default:
					break;
			}
			break;
			
		default:
			break;
	}
}

