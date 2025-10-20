/****************************************************************************
 * 文件名: hal_adc.h
 * 功能: ADC硬件抽象层
 * 
 * 说明: 
 *   封装ADC模块的初始化和读取操作
 *   支持压力、电池、液位检测
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef HAL_ADC_H
#define HAL_ADC_H

#include <stdint.h>

/****************************************************************************
 * ADC通道定义
 ****************************************************************************/
typedef enum {
	ADC_CHANNEL_PRESSURE = 0,   // AN0：压力传感器
	ADC_CHANNEL_BATTERY  = 4,   // AN4：电池电压
	ADC_CHANNEL_LIQUID   = 5    // AN5：液位检测
} ADC_Channel_e;

/****************************************************************************
 * ADC操作函数
 ****************************************************************************/

/**
 * 函数: HAL_ADC_Init
 * 功能: 初始化ADC模块
 */
void HAL_ADC_Init(void);

/**
 * 函数: HAL_ADC_Read
 * 功能: 读取指定通道的ADC值
 * 参数: channel - ADC通道
 * 返回: ADC采样值（10位：0-1023）
 */
uint16_t HAL_ADC_Read(ADC_Channel_e channel);

/**
 * 函数: HAL_ADC_ReadFiltered
 * 功能: 读取ADC值（带滤波）
 * 参数: channel - ADC通道
 *       samples - 采样次数（用于平均滤波）
 * 返回: 滤波后的ADC值
 */
uint16_t HAL_ADC_ReadFiltered(ADC_Channel_e channel, uint8_t samples);

#endif /* HAL_ADC_H */

