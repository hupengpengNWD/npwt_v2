/****************************************************************************
 * 文件名: hal_gpio.h
 * 功能: GPIO硬件抽象层
 * 
 * 说明: 
 *   提供统一的GPIO操作接口
 *   隔离硬件寄存器操作细节
 *   便于移植到不同MCU
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * GPIO引脚定义（PIC18F46J11）
 ****************************************************************************/

/* 气泵控制 */
#define HAL_GPIO_PUMP_PORT      PORTC
#define HAL_GPIO_PUMP_PIN       2

/* 电磁阀控制 */
#define HAL_GPIO_VALVE1_PORT    PORTB
#define HAL_GPIO_VALVE1_PIN     0

#define HAL_GPIO_VALVE2_PORT    PORTB
#define HAL_GPIO_VALVE2_PIN     1

/* LED指示灯 */
#define HAL_GPIO_LED_GREEN_PORT PORTC
#define HAL_GPIO_LED_GREEN_PIN  3

#define HAL_GPIO_LED_YELLOW_PORT PORTC
#define HAL_GPIO_LED_YELLOW_PIN  4

/* 蜂鸣器 */
#define HAL_GPIO_BUZZER_PORT    PORTC
#define HAL_GPIO_BUZZER_PIN     5

/* 电源控制 */
#define HAL_GPIO_POWER_PORT     PORTC
#define HAL_GPIO_POWER_PIN      6

/****************************************************************************
 * GPIO操作函数
 ****************************************************************************/

/**
 * 函数: HAL_GPIO_Init
 * 功能: 初始化所有GPIO
 */
void HAL_GPIO_Init(void);

/**
 * 函数: HAL_GPIO_WritePin
 * 功能: 写GPIO引脚
 * 参数: pin - 引脚编号, state - 1或0
 */
void HAL_GPIO_WritePin(volatile uint8_t *port, uint8_t pin, bool state);

/**
 * 函数: HAL_GPIO_ReadPin
 * 功能: 读GPIO引脚
 * 返回: true=高电平, false=低电平
 */
bool HAL_GPIO_ReadPin(volatile uint8_t *port, uint8_t pin);

/**
 * 函数: HAL_GPIO_TogglePin
 * 功能: 翻转GPIO引脚
 */
void HAL_GPIO_TogglePin(volatile uint8_t *port, uint8_t pin);

/****************************************************************************
 * 高层GPIO控制接口（内联函数，零开销）
 ****************************************************************************/

/* 气泵控制 */
static inline void HAL_Pump_Start(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_PUMP_PORT, HAL_GPIO_PUMP_PIN, true);
}

static inline void HAL_Pump_Stop(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_PUMP_PORT, HAL_GPIO_PUMP_PIN, false);
}

/* 电磁阀控制 */
static inline void HAL_Valve1_Open(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE1_PORT, HAL_GPIO_VALVE1_PIN, true);
}

static inline void HAL_Valve1_Close(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE1_PORT, HAL_GPIO_VALVE1_PIN, false);
}

static inline void HAL_Valve2_Open(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE2_PORT, HAL_GPIO_VALVE2_PIN, true);
}

static inline void HAL_Valve2_Close(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_VALVE2_PORT, HAL_GPIO_VALVE2_PIN, false);
}

/* LED控制 */
static inline void HAL_LED_Green_On(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_GREEN_PORT, HAL_GPIO_LED_GREEN_PIN, true);
}

static inline void HAL_LED_Green_Off(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_GREEN_PORT, HAL_GPIO_LED_GREEN_PIN, false);
}

static inline void HAL_LED_Yellow_On(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_YELLOW_PORT, HAL_GPIO_LED_YELLOW_PIN, true);
}

static inline void HAL_LED_Yellow_Off(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_LED_YELLOW_PORT, HAL_GPIO_LED_YELLOW_PIN, false);
}

/* 蜂鸣器控制 */
static inline void HAL_Buzzer_On(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_BUZZER_PORT, HAL_GPIO_BUZZER_PIN, true);
}

static inline void HAL_Buzzer_Off(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_BUZZER_PORT, HAL_GPIO_BUZZER_PIN, false);
}

/* 电源控制 */
static inline void HAL_Power_Hold(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_POWER_PORT, HAL_GPIO_POWER_PIN, true);
}

static inline void HAL_Power_Release(void)
{
	HAL_GPIO_WritePin(&HAL_GPIO_POWER_PORT, HAL_GPIO_POWER_PIN, false);
}

#endif /* HAL_GPIO_H */

