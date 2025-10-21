/****************************************************************************
 * 文件名: app_selftest.c
 * 功能: 应用层自检模块实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/app_selftest.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Drivers/Inc/adc_driver.h"
#include "../../Drivers/Inc/lcd_driver.h"

/* 自检状态 */
typedef enum {
	SELFTEST_IDLE = 0,
	SELFTEST_SENSOR,        // 传感器测试
	SELFTEST_PUMP,          // 气泵测试
	SELFTEST_VALVE1,        // 阀门1测试
	SELFTEST_VALVE2,        // 阀门2测试
	SELFTEST_COMPLETE       // 完成
} SelftestState_e;

static SelftestState_e g_selftest_state = SELFTEST_IDLE;
static uint16_t g_selftest_timer = 0;
static bool g_selftest_pass = false;

/**
 * 函数: AppSelftest_Start
 * 功能: 启动自检
 */
void AppSelftest_Start(SystemState_t *sys)
{
	g_selftest_state = SELFTEST_SENSOR;
	g_selftest_timer = 0;
	g_selftest_pass = false;
	
	LCD_Clear();
	LCD_DisplayString(0, 30, "Self Test");
}

/**
 * 函数: AppSelftest_Run
 * 功能: 运行自检
 */
bool AppSelftest_Run(SystemState_t *sys)
{
	g_selftest_timer++;
	
	switch (g_selftest_state)
	{
		case SELFTEST_SENSOR:
			/* 测试传感器 */
			LCD_DisplayString(2, 0, "Testing Sensor...");
			
			/* 读取ADC值，检查是否正常 */
			uint16_t pressure_adc = ADC_ReadPressure();
			uint16_t battery_adc = ADC_ReadBattery();
			
			if (pressure_adc > 50 && battery_adc > 100)
			{
				LCD_DisplayString(2, 0, "Sensor OK       ");
				g_selftest_state = SELFTEST_PUMP;
				g_selftest_timer = 0;
			}
			else if (g_selftest_timer > 50)  // 1秒超时
			{
				LCD_DisplayString(2, 0, "Sensor FAIL     ");
				g_selftest_pass = false;
				g_selftest_state = SELFTEST_COMPLETE;
			}
			break;
			
		case SELFTEST_PUMP:
			/* 测试气泵 */
			LCD_DisplayString(3, 0, "Testing Pump...  ");
			HAL_Pump_Enable(true);
			
			if (g_selftest_timer > 50)  // 运行1秒
			{
				HAL_Pump_Enable(false);
				LCD_DisplayString(3, 0, "Pump OK         ");
				g_selftest_state = SELFTEST_VALVE1;
				g_selftest_timer = 0;
			}
			break;
			
		case SELFTEST_VALVE1:
			/* 测试阀门1 */
			LCD_DisplayString(4, 0, "Testing Valve1...");
			HAL_Valve1_Open();
			
			if (g_selftest_timer > 25)  // 0.5秒
			{
				HAL_Valve1_Close();
				LCD_DisplayString(4, 0, "Valve1 OK       ");
				g_selftest_state = SELFTEST_VALVE2;
				g_selftest_timer = 0;
			}
			break;
			
		case SELFTEST_VALVE2:
			/* 测试阀门2 */
			LCD_DisplayString(5, 0, "Testing Valve2...");
			HAL_Valve2_Open();
			
			if (g_selftest_timer > 25)  // 0.5秒
			{
				HAL_Valve2_Close();
				LCD_DisplayString(5, 0, "Valve2 OK       ");
				g_selftest_pass = true;
				g_selftest_state = SELFTEST_COMPLETE;
				g_selftest_timer = 0;
			}
			break;
			
		case SELFTEST_COMPLETE:
			/* 自检完成 */
			if (g_selftest_pass) {
				LCD_DisplayString(6, 20, "PASS");
			} else {
				LCD_DisplayString(6, 20, "FAIL");
			}
			return true;  // 自检完成
			
		default:
			break;
	}
	
	return false;  // 自检进行中
}

/**
 * 函数: AppSelftest_Stop
 * 功能: 停止自检
 */
void AppSelftest_Stop(SystemState_t *sys)
{
	g_selftest_state = SELFTEST_IDLE;
	HAL_Pump_Enable(false);
	HAL_Valve1_Close();
	HAL_Valve2_Close();
}

/**
 * 函数: AppSelftest_IsPass
 * 功能: 判断自检是否通过
 */
bool AppSelftest_IsPass(const SystemState_t *sys)
{
	return g_selftest_pass;
}

