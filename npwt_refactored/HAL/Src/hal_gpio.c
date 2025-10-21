/****************************************************************************
 * 文件名: hal_gpio.c
 * 功能: GPIO硬件抽象层实现
 * 
 * 说明: 
 *   封装GPIO操作的底层实现
 *   与具体MCU相关
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../../Core/Inc/system_config.h"
#include "../Inc/hal_gpio.h"

/**
 * 函数: HAL_GPIO_Init
 * 功能: 初始化所有GPIO引脚
 */
void HAL_GPIO_Init(void)
{
	/* 配置PORTA：ADC输入和音频控制 */
	TRISA = 0b00110111;   // RA0,1,2,4,5=输入，RA3,5=输出
	LATA = 0;
	PORTA = 0;
	
	/* 配置PORTB：电磁阀输出和按键输入 */
	TRISB = 0b11111100;   // RB0,1=输出（阀门），RB2-7=输入（按键）
	PORTB = 0;
	LATB = 0;
	
	/* 配置PORTC：气泵、LED、蜂鸣器、电源 */
	TRISC = 0b00000011;   // RC0,1=输入（电池状态），RC2-7=输出
	PORTC = 0;
	LATC = 0;
	
	/* 配置PORTD：LCD数据线 */
	LATD = 0;
	TRISD = 0;            // 全部输出
	PORTD = 0;
	
	/* 配置PORTE：LCD控制线 */
	LATE = 0;
	TRISE = 0;            // 全部输出
	PORTE = 0;
}

/**
 * 函数: HAL_GPIO_WritePin
 * 功能: 写GPIO引脚
 */
void HAL_GPIO_WritePin(volatile uint8_t *port, uint8_t pin, bool state)
{
	if (state) {
		*port |= (1 << pin);   // 置位
	} else {
		*port &= ~(1 << pin);  // 清零
	}
}

/**
 * 函数: HAL_GPIO_ReadPin
 * 功能: 读GPIO引脚
 */
bool HAL_GPIO_ReadPin(volatile uint8_t *port, uint8_t pin)
{
	return (*port & (1 << pin)) != 0;
}

/**
 * 函数: HAL_GPIO_TogglePin
 * 功能: 翻转GPIO引脚
 */
void HAL_GPIO_TogglePin(volatile uint8_t *port, uint8_t pin)
{
	*port ^= (1 << pin);
}

