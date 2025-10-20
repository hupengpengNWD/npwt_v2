/****************************************************************************
 * 文件名: pressure_controller.c
 * 功能: 压力控制器实现
 * 
 * 说明: 
 *   双位控制算法（Bang-Bang Control）
 *   - 压力低于下限：开启气泵补气
 *   - 压力高于上限：停止气泵
 *   - 压力在范围内：保持当前状态，进行故障检测
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/pressure_controller.h"
#include "../../Drivers/Inc/adc_driver.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Core/Inc/system_config.h"

/**
 * 函数: PressureController_Init
 * 功能: 初始化压力控制器
 */
void PressureController_Init(PressureData_t *data)
{
	data->target_pressure = PRESSURE_DEFAULT;
	data->current_pressure = 0;
	data->upper_threshold = 0;
	data->lower_threshold = 0;
	data->adc_zero = 0;
	data->calibration_k = 2.75f;  // 默认校准系数
	data->is_stable = false;
	
	/* 计算初始阈值 */
	PressureController_CalculateThresholds(data);
}

/**
 * 函数: PressureController_SetTarget
 * 功能: 设置目标压力
 */
void PressureController_SetTarget(PressureData_t *data, uint16_t target)
{
	/* 限幅检查 */
	if (target < PRESSURE_MIN) target = PRESSURE_MIN;
	if (target > PRESSURE_MAX) target = PRESSURE_MAX;
	
	data->target_pressure = target;
	
	/* 重新计算阈值 */
	PressureController_CalculateThresholds(data);
}

/**
 * 函数: PressureController_CalculateThresholds
 * 功能: 计算动态阈值
 * 
 * 阈值策略：
 *   下阈值 = 目标 - 10%
 *   上阈值 = 目标 + (5%~10%，压力越高，容差越小)
 */
void PressureController_CalculateThresholds(PressureData_t *data)
{
	uint16_t target = data->target_pressure;
	
	/* 计算下阈值：10% */
	data->lower_threshold = target - (target / 10);
	
	/* 计算上阈值：根据压力段调整 */
	if (target > 200) {
		data->upper_threshold = target + (target / 20);         // +5%
	} else if (target >= 80) {
		data->upper_threshold = target + (target / 20) + 4;    // +5% + 4mmHg
	} else {
		data->upper_threshold = target + (target / 10) + 2;    // +10% + 2mmHg
	}
}

/**
 * 函数: PressureController_Update
 * 功能: 压力控制主循环
 * 
 * 控制逻辑：
 *   1. 读取当前压力
 *   2. 与阈值比较
 *   3. 控制气泵和阀门
 */
void PressureController_Update(PressureData_t *data, PumpData_t *pump)
{
	/* 读取当前压力 */
	uint16_t adc_raw = ADC_ReadPressure();
	data->current_pressure = ADC_ConvertToMmHg(adc_raw, data->adc_zero, data->calibration_k);
	
	/* 双位控制判断 */
	if (data->current_pressure <= data->lower_threshold)
	{
		/* 压力过低，开启补气 */
		HAL_Valve2_Close();      // 关闭放气阀
		HAL_Pump_Start();        // 启动气泵
		pump->is_enabled = true;
		data->is_stable = false;
	}
	else if (data->current_pressure >= data->upper_threshold)
	{
		/* 压力过高，停止补气 */
		HAL_Pump_Stop();         // 停止气泵
		pump->is_enabled = false;
		data->is_stable = false;
	}
	else
	{
		/* 压力在正常范围内 */
		data->is_stable = true;
		/* 保持当前气泵状态不变 */
	}
}

/**
 * 函数: PressureController_IsInRange
 * 功能: 判断压力是否在正常范围
 */
bool PressureController_IsInRange(const PressureData_t *data)
{
	return (data->current_pressure >= data->lower_threshold &&
	        data->current_pressure <= data->upper_threshold);
}

