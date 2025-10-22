/****************************************************************************
 * 文件名: hal_adc.c
 * 功能: ADC硬件抽象层实现
 * 
 * 说明: 
 *   PIC18F46J11 ADC模块配置
 *   10位分辨率，内部参考电压
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/hal_adc.h"

/**
 * 函数: HAL_ADC_Init
 * 功能: 初始化ADC模块
 */
void HAL_ADC_Init(void)
{
	/* 完全按照未重构工程 SYS_ADCini() 的寄存器值 */
	ADCON0 = 0x01;   // 使能ADC，选择通道AN0
	ADCON1 = 0xbe;   // 右对齐，参考电压VDD/VSS
	ANCON0 = 0x01;   // AN0配置为模拟输入
}

/**
 * 函数: HAL_ADC_Read
 * 功能: 读取指定通道的ADC值
 */
uint16_t HAL_ADC_Read(ADC_Channel_e channel)
{
	/* 选择通道 */
	ADCON0 &= 0b11000011;           // 清除通道选择位
	ADCON0 |= (channel << 2);       // 设置新通道
	
	/* 等待采样保持时间 */
	__delay_us(10);
	
	/* 启动转换 */
	ADCON0bits.GO_DONE = 1;
	
	/* 等待转换完成 */
	while (ADCON0bits.GO_DONE);
	
	/* 读取结果（10位，右对齐） */
	return ((uint16_t)ADRESH << 8) | ADRESL;
}

/**
 * 函数: HAL_ADC_ReadFiltered
 * 功能: 读取ADC值并进行平均滤波
 */
uint16_t HAL_ADC_ReadFiltered(ADC_Channel_e channel, uint8_t samples)
{
	uint32_t sum = 0;
	uint16_t max = 0;
	uint16_t min = 1023;
	
	/* 采集多次 */
	for (uint8_t i = 0; i < samples; i++)
	{
		uint16_t value = HAL_ADC_Read(channel);
		sum += value;
		
		/* 记录最大最小值 */
		if (value > max) max = value;
		if (value < min) min = value;
	}
	
	/* 如果采样数>=4，去掉最大最小值后求平均 */
	if (samples >= 4)
	{
		sum = sum - max - min;
		return (uint16_t)(sum / (samples - 2));
	}
	else
	{
		return (uint16_t)(sum / samples);
	}
}

