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

/****************************************************************************
 * GPIO高层控制函数实现
 ****************************************************************************/

/* 气泵控制 */
void HAL_Pump_Start(void)
{
	LATCbits.LATC7 = 1;  // 气泵启动
}

void HAL_Pump_Stop(void)
{
	LATCbits.LATC7 = 0;  // 气泵停止
}

void HAL_Pump_Enable(bool enable)
{
	LATCbits.LATC7 = enable ? 1 : 0;  // 气泵使能控制
}

/* 电磁阀控制 */
void HAL_Valve1_Open(void)
{
	LATBbits.LATB0 = 1;  // 电磁阀1打开
}

void HAL_Valve1_Close(void)
{
	LATBbits.LATB0 = 0;  // 电磁阀1关闭
}

void HAL_Valve2_Open(void)
{
	LATBbits.LATB1 = 1;  // 电磁阀2打开
}

void HAL_Valve2_Close(void)
{
	LATBbits.LATB1 = 0;  // 电磁阀2关闭
}

/* LED控制 */
void HAL_LED_White_On(void)
{
	LATCbits.LATC6 = 1;  // 白色LED背光开启
}

void HAL_LED_White_Off(void)
{
	LATCbits.LATC6 = 0;  // 白色LED背光关闭
}

void HAL_LED_Yellow_On(void)
{
	LATCbits.LATC4 = 1;  // 黄色LED开启
}

void HAL_LED_Yellow_Off(void)
{
	LATCbits.LATC4 = 0;  // 黄色LED关闭
}

void HAL_LED_Yellow_Toggle(void)
{
	LATCbits.LATC4 ^= 1;  // 黄色LED翻转
}

/* LCD背光控制 */
void HAL_LCD_Backlight_On(void)
{
	LATCbits.LATC6 = 1;  // LCD背光开启（使用白色LED）
}

void HAL_LCD_Backlight_Off(void)
{
	LATCbits.LATC6 = 0;  // LCD背光关闭（使用白色LED）
}

/* 蜂鸣器控制 */
void HAL_Buzzer_On(void)
{
	LATAbits.LATA3 = 1;  // 蜂鸣器开启
}

void HAL_Buzzer_Off(void)
{
	LATAbits.LATA3 = 0;  // 蜂鸣器关闭
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

/* 电机电源使能控制 */
void HAL_MotorPWR_Enable(void)
{
	LATCbits.LATC3 = 1;  // DRV_EN = 1，使能驱动（与未重构工程一致）
}

void HAL_MotorPWR_Disable(void)
{
	LATCbits.LATC3 = 0;  // DRV_EN = 0，禁用驱动
}

/* 电池状态读取 */
bool HAL_Battery_IsCharging(void)
{
	/* 参考未重构工程：BAT_CHARGE == BAT_CHARGING (0) 表示充电中 */
	/* RC1 = 0 表示充电中，RC1 = 1 表示未充电 */
	return (PORTCbits.RC1 == 0);
}

bool HAL_Battery_IsGood(void)
{
	/* 读取RC0引脚（BAT_GOOD） */
	return (PORTCbits.RC0 == 1);
}

