/****************************************************************************
 * 文件名: adc_driver.h
 * 功能: ADC采集驱动
 * 
 * 说明: 
 *   基于HAL层的高级ADC功能
 *   提供压力、电池、液位的采集和转换
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>

/****************************************************************************
 * ADC驱动函数
 ****************************************************************************/

/**
 * 函数: ADC_Driver_Init
 * 功能: 初始化ADC驱动
 */
void ADC_Driver_Init(void);

/**
 * 函数: ADC_ReadPressure
 * 功能: 读取压力传感器值
 * 返回: 压力值（mmHg）
 */
uint16_t ADC_ReadPressure(void);

/**
 * 函数: ADC_ReadBattery
 * 功能: 读取电池电压
 * 返回: 电池ADC值
 */
uint16_t ADC_ReadBattery(void);

/**
 * 函数: ADC_ReadLiquid
 * 功能: 读取液位传感器
 * 返回: 液位ADC值
 */
uint16_t ADC_ReadLiquid(void);

/**
 * 函数: ADC_ConvertToMmHg
 * 功能: 将ADC值转换为压力（mmHg）
 * 参数: adc_value - ADC采样值
 *       zero_point - 零点校准值
 *       k - 校准系数
 * 返回: 压力值（mmHg）
 */
uint16_t ADC_ConvertToMmHg(uint16_t adc_value, uint16_t zero_point, float k);

/**
 * 函数: ADC_SamplePressure
 * 功能: 快速采样压力（在中断中调用）
 */
void ADC_SamplePressure(void);

/**
 * 函数: ADC_GetLatestPressure
 * 功能: 获取最新压力采样值
 * 返回: 最新的ADC值
 */
uint16_t ADC_GetLatestPressure(void);

#endif /* ADC_DRIVER_H */

