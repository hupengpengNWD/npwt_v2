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
 * 功能: 初始化ADC驱动（驱动层数据结构初始化）
 * 
 * 注意：
 *   HAL层硬件初始化（HAL_ADC_Init）由 main.c 中统一调用
 *   驱动层只初始化自己的数据结构和业务逻辑
 */
void ADC_Driver_Init(void)
{
	/* 当前驱动层没有额外的数据结构需要初始化 */
	/* HAL_ADC_Init() 已在 main.c 中调用，避免重复 */
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

/* 最新压力采样值（中断更新） */
static uint16_t g_latest_pressure_sample = 0;

/**
 * 函数: ADC_SamplePressure
 * 功能: 快速采样压力（在中断中调用）
 */
void ADC_SamplePressure(void)
{
	g_latest_pressure_sample = HAL_ADC_Read(ADC_CHANNEL_PRESSURE);
}

/**
 * 函数: ADC_GetLatestPressure
 * 功能: 获取最新压力采样值
 */
uint16_t ADC_GetLatestPressure(void)
{
	return g_latest_pressure_sample;
}

