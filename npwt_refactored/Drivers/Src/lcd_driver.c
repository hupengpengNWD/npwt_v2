/****************************************************************************
 * 文件名: lcd_driver.c
 * 功能: LCD显示驱动实现
 * 
 * 说明: 
 *   基于JLX1864G LCD控制器
 *   128x64点阵LCD
 *   使用旧项目的BIOS函数
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/lcd_driver.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Core/Inc/system_config.h"

/* 引用旧项目的LCD BIOS函数 */
extern void Init_LCD(void);
extern void ClrBlk(void);
extern void PutStr(unsigned char page, unsigned char column, const unsigned char *puts);
extern void PutNO(unsigned char page, unsigned char column, unsigned short NO);
extern void PutGB1616(unsigned char page, unsigned char column, unsigned char index);

/* 俄语字符串（从旧代码引用） */
extern const unsigned char rus_model[];
extern const unsigned char rus_continuous[];
extern const unsigned char rus_intermittent[];
extern const unsigned char rus_leak_Alarm[];
extern const unsigned char rus_blockage_Alarm[];
extern const unsigned char rus_battery_low[];
extern const unsigned char rus_liquid_full[];
extern const unsigned char rus_psressure[];

/**
 * 函数: LCD_Driver_Init
 * 功能: 初始化LCD
 */
void LCD_Driver_Init(void)
{
	Init_LCD();  // 调用BIOS初始化
	LCD_Clear();
}

/**
 * 函数: LCD_Clear
 * 功能: 清屏
 */
void LCD_Clear(void)
{
	ClrBlk();
}

/**
 * 函数: LCD_SetBacklight
 * 功能: 设置背光
 */
void LCD_SetBacklight(bool on)
{
	if (on) {
		HAL_LED_Yellow_On();
	} else {
		HAL_LED_Yellow_Off();
	}
}

/**
 * 函数: LCD_DisplayStartup
 * 功能: 显示开机画面
 */
void LCD_DisplayStartup(void)
{
	LCD_Clear();
	
	/* 显示产品名称和版本 */
	LCD_DisplayString(2, 20, "NPWT System");
	LCD_DisplayString(4, 30, "v2.0");
	
	/* 开启背光 */
	LCD_SetBacklight(true);
}

/**
 * 函数: LCD_DisplayMode
 * 功能: 显示工作模式
 */
void LCD_DisplayMode(WorkMode_e mode, uint8_t language)
{
	/* 显示"模式："标题 */
	if (language == LANGUAGE_RUSSIAN) {
		PutStr(0, 0, rus_model);
	} else {
		LCD_DisplayString(0, 0, "Mode:");
	}
	
	/* 显示具体模式 */
	switch (mode)
	{
		case MODE_STANDBY:
			LCD_DisplayString(0, 60, "Standby");
			break;
			
		case MODE_CONTINUOUS:
			if (language == LANGUAGE_RUSSIAN) {
				PutStr(0, 60, rus_continuous);
			} else {
				LCD_DisplayString(0, 60, "Continuous");
			}
			break;
			
		case MODE_INTERMITTENT:
			if (language == LANGUAGE_RUSSIAN) {
				PutStr(0, 60, rus_intermittent);
			} else {
				LCD_DisplayString(0, 60, "Intermittent");
			}
			break;
			
		case MODE_PAUSE:
			LCD_DisplayString(0, 60, "Pause");
			break;
			
		case MODE_ERROR:
			LCD_DisplayString(0, 60, "ERROR");
			break;
			
		default:
			break;
	}
}

/**
 * 函数: LCD_DisplayPressure
 * 功能: 显示压力值
 */
void LCD_DisplayPressure(uint16_t target, uint16_t current)
{
	/* 显示目标压力 */
	LCD_DisplayString(2, 0, "Target:");
	LCD_DisplayNumber(2, 70, target);
	LCD_DisplayString(2, 100, "mmHg");
	
	/* 显示当前压力 */
	LCD_DisplayString(4, 0, "Current:");
	LCD_DisplayNumber(4, 70, current);
	LCD_DisplayString(4, 100, "mmHg");
}

/**
 * 函数: LCD_DisplayBattery
 * 功能: 显示电池电量
 */
void LCD_DisplayBattery(uint8_t level, bool is_charging)
{
	/* 显示电池图标和电量 */
	uint8_t page = 6;
	uint8_t column = 102;
	
	/* 根据电量等级显示不同图标 */
	switch (level)
	{
		case 0:
			/* 显示空电池（需要实现图标函数） */
			LCD_DisplayString(page, column, "E");
			break;
		case 1:
			LCD_DisplayString(page, column, "L");
			break;
		case 2:
		case 3:
			LCD_DisplayString(page, column, "M");
			break;
		case 4:
			LCD_DisplayString(page, column, "F");
			break;
	}
	
	/* 显示充电标志 */
	if (is_charging) {
		LCD_DisplayString(page, column + 10, "+");
	}
}

/**
 * 函数: LCD_DisplayError
 * 功能: 显示故障信息
 */
void LCD_DisplayError(ErrorCode_e error, uint8_t language)
{
	LCD_Clear();
	
	switch (error)
	{
		case ERROR_LEAKAGE:
			if (language == LANGUAGE_RUSSIAN) {
				PutStr(2, 10, rus_leak_Alarm);
			} else {
				LCD_DisplayString(2, 10, "Air Leakage!");
			}
			break;
			
		case ERROR_BLOCKAGE:
			if (language == LANGUAGE_RUSSIAN) {
				PutStr(2, 10, rus_blockage_Alarm);
			} else {
				LCD_DisplayString(2, 10, "Tube Blocked!");
			}
			break;
			
		case ERROR_LIQUID_FULL:
			if (language == LANGUAGE_RUSSIAN) {
				PutStr(2, 10, rus_liquid_full);
			} else {
				LCD_DisplayString(2, 10, "Tank Full!");
			}
			break;
			
		case ERROR_BATTERY_LOW:
			if (language == LANGUAGE_RUSSIAN) {
				PutStr(2, 10, rus_battery_low);
			} else {
				LCD_DisplayString(2, 10, "Low Battery!");
			}
			break;
			
		default:
			break;
	}
}

/**
 * 函数: LCD_DisplayNumber
 * 功能: 显示数字
 */
void LCD_DisplayNumber(uint8_t page, uint8_t column, uint16_t number)
{
	PutNO(page, column, number);
}

/**
 * 函数: LCD_DisplayString
 * 功能: 显示ASCII字符串
 */
void LCD_DisplayString(uint8_t page, uint8_t column, const char *str)
{
	PutStr(page, column, (const unsigned char *)str);
}

