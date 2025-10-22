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
	/* 完全按照未重构工程 SYS_IO_Ini() 的写法 */
	TRISA  =  0b00110111;
	LATA  =  0;
	PORTA  =  0;
	
	TRISB  =  0b11111100;
	PORTB  =  0;
	LATB  =  0;
	
	TRISC  =  0b00000011;
	PORTC  =  0;
	LATC  =  0;
	
	LATD  =  0;
	TRISD  =  0;
	PORTD  =  0;
	
	LATE  =  0;
	TRISE  =  0;
	PORTE  =  0;
	
	ANCON0  =  0xff;
	ANCON1  =  0xff;
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

/****************************************************************************
 * GPIO高层控制函数实现
 ****************************************************************************/

/* 气泵控制 */
void HAL_Pump_Start(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_PUMP_PORT, HAL_GPIO_PUMP_PIN, true);
}

void HAL_Pump_Stop(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_PUMP_PORT, HAL_GPIO_PUMP_PIN, false);
}

void HAL_Pump_Enable(bool enable)
{
	if (enable) {
		HAL_Pump_Start();
	} else {
		HAL_Pump_Stop();
	}
}

/* 电磁阀控制 */
void HAL_Valve1_Open(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE1_PORT, HAL_GPIO_VALVE1_PIN, true);
}

void HAL_Valve1_Close(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE1_PORT, HAL_GPIO_VALVE1_PIN, false);
}

void HAL_Valve2_Open(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE2_PORT, HAL_GPIO_VALVE2_PIN, true);
}

void HAL_Valve2_Close(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE2_PORT, HAL_GPIO_VALVE2_PIN, false);
}

/* LED控制 */
void HAL_LED_Green_On(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_GREEN_PORT, HAL_GPIO_LED_GREEN_PIN, true);
}

void HAL_LED_Green_Off(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_GREEN_PORT, HAL_GPIO_LED_GREEN_PIN, false);
}

void HAL_LED_Yellow_On(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_YELLOW_PORT, HAL_GPIO_LED_YELLOW_PIN, true);
}

void HAL_LED_Yellow_Off(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_YELLOW_PORT, HAL_GPIO_LED_YELLOW_PIN, false);
}

/* 蜂鸣器控制 */
void HAL_Buzzer_On(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_BUZZER_PORT, HAL_GPIO_BUZZER_PIN, true);
}

void HAL_Buzzer_Off(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_BUZZER_PORT, HAL_GPIO_BUZZER_PIN, false);
}

/* 电源控制 */
void HAL_Power_Hold(void)
{
	LATCbits.LATC2 = 1;  // POWER_ON = 1，保持电源（与未重构工程一致）
}

void HAL_Power_Release(void)
{
	LATCbits.LATC2 = 0;  // POWER_ON = 0，释放电源（与未重构工程一致）
}

