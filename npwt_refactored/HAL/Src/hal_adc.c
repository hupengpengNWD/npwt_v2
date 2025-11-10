/****************************************************************************
 * 文件名: hal_adc.c
 * 功能: ADC硬件抽象层实现
 * 
 * 说明: 
 *   PIC18F46J11 ADC模块配置
 *   10位分辨率，内部参考电压
 *   配置通道：AN0（压力）、AN1（电机电流）、AN2（电池电压）
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/hal_adc.h"

/**
 * 函数: HAL_ADC_Init
 * 功能: 初始化ADC模块
 * 说明: 
 *   配置AN0（压力）、AN1（电机电流）、AN2（电池电压）为模拟输入
 *   参考未重构工程 adc_init() 的实现
 */
void HAL_ADC_Init(void)
{
	/* 配置AN0、AN1、AN2为模拟输入（PCFG0=0, PCFG1=0, PCFG2=0） */
	ANCON0bits.PCFG0 = 0;  // AN0（RA0）：压力传感器 - 模拟输入
	ANCON0bits.PCFG1 = 0;  // AN1（RA1）：电机电流 - 模拟输入
	ANCON0bits.PCFG2 = 0;  // AN2（RA2）：电池电压 - 模拟输入
	
	/* ADC模块配置 */
	ADCON0 = 0x01;   // 使能ADC，选择通道AN0（默认通道）
	ADCON1 = 0xbe;   // 右对齐，参考电压VDD/VSS
	ADCON0bits.ADON = 1;  // 开启ADC模块
}

/**
 * 函数: HAL_ADC_Read
 * 功能: 读取指定通道的ADC值
 * 说明: 
 *   包含超时保护机制，防止ADC转换异常导致的死锁
 *   正常转换时间约12μs，超时时间设置为200μs
 */
uint16_t HAL_ADC_Read(ADC_Channel_e channel)
{
	/* 选择通道 */
	ADCON0 &= 0b11000011;           // 清除通道选择位
	ADCON0 |= (channel << 2);       // 设置新通道
	
	/* 启动转换 */
	/* 未重构工程无延迟也能正常工作，说明通道切换后无需额外延迟 */
	// __delay_us(5);  // 屏蔽：与未重构工程保持一致
	ADCON0bits.GO_DONE = 1;
	
	/* 等待转换完成（带超时保护） */
	/* 超时计数：500次循环 ≈ 200μs @ 32MHz */
	/* 正常转换时间约12μs，超时时间远大于正常时间 */
	uint16_t timeout = 500;
	while (ADCON0bits.GO_DONE && (timeout > 0))
	{
		timeout--;
	}
	
	/* 如果超时，返回0xFFFF（表示错误） */
	if (timeout == 0)
	{
		/* 超时处理：停止转换并返回错误值 */
		ADCON0bits.GO_DONE = 0;  // 清除转换标志
		return 0xFFFF;  // 返回0xFFFF表示读取失败（ADC有效值范围0-1023）
	}
	
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

