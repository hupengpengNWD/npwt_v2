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

#include "../../Core/Inc/mcu_config.h"  // 包含MCU硬件定义

/****************************************************************************
 * GPIO引脚定义（PIC18F46J11）
 * 与未重构工程完全一致
 ****************************************************************************/

/* 电磁阀控制 */
#define HAL_GPIO_VALVE1_PORT    PORTB
#define HAL_GPIO_VALVE1_PIN     0  // RB0 (Pin 33) - VAL1

#define HAL_GPIO_VALVE2_PORT    PORTB
#define HAL_GPIO_VALVE2_PIN     1  // RB1 (Pin 34) - VAL2

/* 电源控制（电源自锁）- 最重要！ */
#define HAL_GPIO_POWER_PORT     PORTC
#define HAL_GPIO_POWER_PIN      2  // RC2 (Pin 13) - POWER_ON 自锁信号

/* 驱动使能 */
#define HAL_GPIO_DRV_EN_PORT    PORTC
#define HAL_GPIO_DRV_EN_PIN     3  // RC3 (Pin 14) - DRV_EN

/* LED指示灯 */
#define HAL_GPIO_LED_WHITE_PORT PORTC
#define HAL_GPIO_LED_WHITE_PIN  6  // RC6 (Pin 17) - 白色LED背光 (WHITE)

#define HAL_GPIO_LED_YELLOW_PORT PORTC
#define HAL_GPIO_LED_YELLOW_PIN  4  // RC4 (Pin 15) - 黄色LED (YEL)

/* 气泵控制 */
#define HAL_GPIO_PUMP_PORT      PORTC
#define HAL_GPIO_PUMP_PIN       7  // RC7 (Pin 18) - 气泵控制

/* 蜂鸣器（在PORTA上，不是PORTC！） */
#define HAL_GPIO_BUZZER_PORT    PORTA
#define HAL_GPIO_BUZZER_PIN     3  // RA3 (Pin 5) - SPEAK 蜂鸣器

/* 电池状态输入（在PORTC上） */
#define HAL_GPIO_BAT_GOOD_PORT  PORTC
#define HAL_GPIO_BAT_GOOD_PIN   0  // RC0 (Pin 11) - BAT_GOOD 电池电量良好信号

#define HAL_GPIO_BAT_CHARGE_PORT PORTC
#define HAL_GPIO_BAT_CHARGE_PIN  1  // RC1 (Pin 12) - BAT_CHARGE 电池充电状态信号

/****************************************************************************
 * GPIO操作函数
 ****************************************************************************/

/**
 * 函数: HAL_GPIO_Init
 * 功能: 初始化所有GPIO
 */
void HAL_GPIO_Init(void);

/****************************************************************************
 * GPIO高层控制接口（普通函数声明，消除inline警告）
 ****************************************************************************/

/* 气泵控制 */
void HAL_Pump_Start(void);
void HAL_Pump_Stop(void);
void HAL_Pump_Enable(bool enable);

/* 电磁阀控制 */
void HAL_Valve1_Open(void);
void HAL_Valve1_Close(void);
void HAL_Valve2_Open(void);
void HAL_Valve2_Close(void);

/* LED控制 */
void HAL_LED_White_On(void);
void HAL_LED_White_Off(void);
void HAL_LED_Yellow_On(void);
void HAL_LED_Yellow_Off(void);
void HAL_LED_Yellow_Toggle(void);

/* LCD背光控制 */
void HAL_LCD_Backlight_On(void);
void HAL_LCD_Backlight_Off(void);

/* 蜂鸣器控制 */
void HAL_Buzzer_On(void);
void HAL_Buzzer_Off(void);

/* 电源控制 */
void HAL_Power_Hold(void);
void HAL_Power_Release(void);

/* 电机电源使能控制 */
void HAL_MotorPWR_Enable(void);
void HAL_MotorPWR_Disable(void);

/* 电池状态读取 */
bool HAL_Battery_IsCharging(void);
bool HAL_Battery_IsGood(void);

#endif /* HAL_GPIO_H */

