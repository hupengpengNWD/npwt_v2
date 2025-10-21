/****************************************************************************
 * 文件名: app_display.c
 * 功能: 应用层显示更新实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/app_display.h"
#include "../../Drivers/Inc/lcd_driver.h"

/**
 * 函数: AppDisplay_Update
 * 功能: 更新LCD显示
 */
void AppDisplay_Update(SystemState_t *sys)
{
	static uint8_t display_refresh_count = 0;
	static uint8_t last_mode = MODE_INIT;
	
	/* 模式切换时清屏 */
	if (sys->current_mode != last_mode)
	{
		LCD_Clear();
		last_mode = sys->current_mode;
	}
	
	/* 根据当前模式显示不同内容 */
	switch (sys->current_mode)
	{
		case MODE_INIT:
			/* 初始化模式：显示Logo（已在初始化时显示） */
			break;
			
		case MODE_STANDBY:
			/* 待机模式：显示压力设置 */
			if (display_refresh_count++ % 10 == 0)  // 每200ms刷新一次
			{
				LCD_DisplayMode(MODE_STANDBY);
				LCD_DisplayString(2, 0, "Set:");
				LCD_DisplayNumber(2, 50, sys->pressure.target_pressure);
				LCD_DisplayString(2, 90, "mmHg");
				LCD_DisplayString(4, 10, "Press OK to start");
				
				/* 显示静音状态 */
				if (sys->alarm.is_muted)
				{
					LCD_DisplayString(5, 0, "[MUTED]");
				}
				
				LCD_DisplayBatteryIcon(6, 102, sys->battery.percentage);
			}
			break;
			
		case MODE_CONTINUOUS:
			/* 连续模式：显示压力和时间 */
			if (display_refresh_count++ % 5 == 0)  // 每100ms刷新一次
			{
				LCD_DisplayMode(MODE_CONTINUOUS);
				LCD_DisplayPressure(
					sys->pressure.target_pressure,
					sys->pressure.current_pressure
				);
				
				/* 显示运行时间 */
				uint16_t runtime_min = sys->uptime_ms / 60000;
				LCD_DisplayString(5, 0, "Time:");
				LCD_DisplayNumber(5, 50, runtime_min);
				LCD_DisplayString(5, 90, "min");
				
				/* 显示锁定状态 */
				if (sys->ui.is_locked)
				{
					LCD_DisplayString(6, 0, "[LOCK]");
				}
				
				LCD_DisplayBatteryIcon(6, 102, sys->battery.percentage);
			}
			break;
			
		case MODE_INTERMITTENT:
			/* 间歇模式：显示压力、阶段和时间 */
			if (display_refresh_count++ % 5 == 0)
			{
				LCD_DisplayMode(MODE_INTERMITTENT);
				LCD_DisplayPressure(
					sys->pressure.target_pressure,
					sys->pressure.current_pressure
				);
				
				/* 显示当前阶段 */
				LCD_DisplayString(4, 0, "Phase:");
				switch (sys->intermittent.current_phase)
				{
					case INTERMITTENT_PHASE_HIGH:
						LCD_DisplayString(4, 60, "High ");
						break;
					case INTERMITTENT_PHASE_LOW:
						LCD_DisplayString(4, 60, "Low  ");
						break;
					case INTERMITTENT_PHASE_STOP:
						LCD_DisplayString(4, 60, "Stop ");
						break;
				}
				
				/* 显示阶段剩余时间 */
				uint16_t remain_time = 0;
				switch (sys->intermittent.current_phase)
				{
					case INTERMITTENT_PHASE_HIGH:
						remain_time = sys->intermittent.high_time * 60 - sys->intermittent.phase_timer;
						break;
					case INTERMITTENT_PHASE_LOW:
						remain_time = sys->intermittent.low_time * 60 - sys->intermittent.phase_timer;
						break;
					case INTERMITTENT_PHASE_STOP:
						remain_time = sys->intermittent.stop_time * 60 - sys->intermittent.phase_timer;
						break;
				}
				LCD_DisplayNumber(5, 50, remain_time / 60);
				LCD_DisplayString(5, 80, ":");
				LCD_DisplayNumber(5, 90, remain_time % 60);
				
				LCD_DisplayBatteryIcon(6, 102, sys->battery.percentage);
			}
			break;
			
		case MODE_PAUSE:
			/* 暂停模式 */
			LCD_DisplayMode(MODE_PAUSE);
			LCD_DisplayString(3, 30, "Paused");
			LCD_DisplayBatteryIcon(6, 102, sys->battery.percentage);
			break;
			
		case MODE_ERROR:
			/* 错误模式：显示故障 */
			if (sys->fault.current_error != ERROR_NONE)
			{
				LCD_DisplayError(sys->fault.current_error);
			}
			break;
			
		case MODE_SELFTEST:
			/* 自检模式：由AppSelftest_Run()负责显示 */
			break;
			
		case MODE_SETTINGS:
			/* 设置模式：由AppSettings_Display()负责显示 */
			break;
			
		case MODE_SHUTDOWN:
			/* 关机模式 */
			LCD_Clear();
			LCD_DisplayString(2, 20, "Saving...");
			LCD_DisplayString(4, 20, "Shutdown");
			break;
			
		default:
			break;
	}
}

