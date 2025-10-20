/****************************************************************************
 * 文件名: adc_driver.c
 * 功能: ADC采集驱动实现
 * 
 * 说明: 
 *   实现ADC采集、滤波、转换功能
 *   提供稳定可靠的传感器数据
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/adc_driver.h"
#include "../../HAL/Inc/hal_adc.h"
#include "../../Core/Inc/system_config.h"

/**
 * 函数: ADC_Driver_Init
 * 功能: 初始化ADC驱动
 */
void ADC_Driver_Init(void)
{
	HAL_ADC_Init();
}

/**
 * 函数: ADC_ReadPressure
 * 功能: 读取压力传感器值（带滤波）
 */
uint16_t ADC_ReadPressure(void)
{
	/* 采集10次求平均，去掉最大最小值 */
	return HAL_ADC_ReadFiltered(ADC_CHANNEL_PRESSURE, 10);
}

/**
 * 函数: ADC_ReadBattery
 * 功能: 读取电池电压（带滤波）
 */
uint16_t ADC_ReadBattery(void)
{
	/* 采集5次求平均 */
	return HAL_ADC_ReadFiltered(ADC_CHANNEL_BATTERY, 5);
}

/**
 * 函数: ADC_ReadLiquid
 * 功能: 读取液位传感器
 */
uint16_t ADC_ReadLiquid(void)
{
	/* 采集3次求平均 */
	return HAL_ADC_ReadFiltered(ADC_CHANNEL_LIQUID, 3);
}

/**
 * 函数: ADC_ConvertToMmHg
 * 功能: 将ADC值转换为压力（mmHg）
 * 
 * 算法: P = (ADC - Zero) * K
 */
uint16_t ADC_ConvertToMmHg(uint16_t adc_value, uint16_t zero_point, float k)
{
	int32_t delta = (int32_t)adc_value - (int32_t)zero_point;
	
	if (delta < 0) delta = 0;
	
	float pressure = (float)delta * k;
	
	/* 限幅 */
	if (pressure > PRESSURE_MAX) pressure = PRESSURE_MAX;
	if (pressure < 0) pressure = 0;
	
	return (uint16_t)pressure;
}

