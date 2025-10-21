/****************************************************************************
 * 文件名: battery_manager.c
 * 功能: 电池管理器实现
 * 
 * 说明: 
 *   实现电池电量检测、充电检测、自动关机保护
 *   电量等级：0格(低于269), 1格(277), 2格(296), 3格(304), 4格(319+)
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/battery_manager.h"
#include "../../Drivers/Inc/adc_driver.h"
#include "../../Core/Inc/system_config.h"
#include "../../Core/Inc/mcu_config.h"  // 包含 PORTCbits 寄存器定义

/**
 * 函数: BatteryManager_Init
 * 功能: 初始化电池管理器
 */
void BatteryManager_Init(BatteryData_t *data)
{
	data->level = 0;
	data->voltage_adc = 0;
	data->is_charging = false;
	data->is_low = false;
	data->shutdown_timer = 0;
}

/**
 * 函数: BatteryManager_Update
 * 功能: 更新电池状态
 */
bool BatteryManager_Update(BatteryData_t *data)
{
	/* 读取电池电压 */
	data->voltage_adc = ADC_ReadBattery();
	
	/* 检查充电状态（BAT_CHARGE引脚：0=充电中，1=未充电） */
	data->is_charging = !PORTCbits.RC0;
	
	/* 计算电量等级 */
	if (data->voltage_adc >= BAT_LEVEL_FULL) {
		data->level = 4;
		data->is_low = false;
	} else if (data->voltage_adc >= BAT_LEVEL_75) {
		data->level = 3;
		data->is_low = false;
	} else if (data->voltage_adc >= BAT_LEVEL_50) {
		data->level = 2;
		data->is_low = false;
	} else if (data->voltage_adc >= BAT_LEVEL_25) {
		data->level = 1;
		data->is_low = true;  // 低电警告
	} else {
		data->level = 0;
		data->is_low = true;  // 低电警告
	}
	
	/* 检查是否需要自动关机 */
	if (data->voltage_adc < BAT_SHUTDOWN_VOLTAGE && !data->is_charging)
	{
		/* 低于关机电压，启动关机倒计时 */
		if (data->shutdown_timer++ >= AUTO_SHUTDOWN_DELAY)
		{
			return false;  // 需要关机
		}
	}
	else
	{
		/* 电压正常，清除关机计时器 */
		data->shutdown_timer = 0;
	}
	
	return true;  // 电池正常
}

/**
 * 函数: BatteryManager_GetLevel
 * 功能: 获取电池电量等级
 */
uint8_t BatteryManager_GetLevel(const BatteryData_t *data)
{
	return data->level;
}

/**
 * 函数: BatteryManager_IsCharging
 * 功能: 检查是否正在充电
 */
bool BatteryManager_IsCharging(const BatteryData_t *data)
{
	return data->is_charging;
}

/**
 * 函数: BatteryManager_ShouldShutdown
 * 功能: 判断是否应该自动关机
 */
bool BatteryManager_ShouldShutdown(const BatteryData_t *data)
{
	return (data->shutdown_timer >= AUTO_SHUTDOWN_DELAY);
}

