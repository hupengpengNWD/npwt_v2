/****************************************************************************
 * 文件名: hal_timer.c
 * 功能: 定时器硬件抽象层实现
 * 
 * 说明: 
 *   PIC18F46J11定时器配置
 *   Timer0: 20ms系统滴答
 *   Timer3: PWM控制
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/hal_timer.h"
#include "../../Core/Inc/system_config.h"

/* 系统滴答计数器 */
static volatile uint32_t g_system_tick_ms = 0;

/* 20ms系统滴答标志（与未重构工程完全一致的变量名） */
volatile unsigned char FLG_SYS_10MS = 0;

/**
 * 函数: HAL_Timer0_Init
 * 功能: 初始化Timer0为10ms定时器
 * 说明: Fosc=32MHz, Timer0时钟=Fosc/4=8MHz, 预分频1:8
 *       10ms需要: 8MHz/8 × 0.01s = 10000计数
 *       初值 = 65536 - 10000 = 55536 = 0xD8F0
 */
void HAL_Timer0_Init(void)
{
	INTCON = 0x20;      // 使能Timer0中断
	INTCON2 = 0x80;     // 
	TMR0H = 0xD8;       // 高字节初值
	TMR0L = 0xF0;       // 低字节初值
	T0CON = 0x82;       // 使能Timer0，16位模式，预分频1:8
}

/**
 * 函数: HAL_Timer3_Init
 * 功能: 初始化Timer3为PWM
 */
void HAL_Timer3_Init(void)
{
	/* 完全按照未重构工程 SYS_TMR3_Ini() 的寄存器值 */
	T3CON = 0x34;           // Timer3配置（与未重构工程一致）
	PIE2 = PIE2 | 0x02;     // 使能Timer3中断（不影响其他中断位）
}

/**
 * 函数: HAL_PWM_SetDuty
 * 功能: 设置PWM占空比
 */
void HAL_PWM_SetDuty(uint8_t duty)
{
	/* 占空比范围：0-10 */
	if (duty > 10) duty = 10;
	
	/* 这里需要根据实际硬件设置PWM寄存器 */
	/* 原代码使用pwm_cnt2变量控制 */
}

/**
 * 函数: HAL_Timer_GetTick
 * 功能: 获取系统运行时间
 */
uint32_t HAL_Timer_GetTick(void)
{
	return g_system_tick_ms;
}

/**
 * 函数: HAL_Watchdog_Clear
 * 功能: 清除看门狗
 */
void HAL_Watchdog_Clear(void)
{
	asm("clrwdt");
}

/****************************************************************************
 * Timer1 相关函数
 ****************************************************************************/

/**
 * 函数: HAL_Timer1_Init
 * 功能: 初始化Timer1为10ms定时器
 * 说明: Fosc=32MHz, Timer1时钟=Fosc/4=8MHz, 预分频1:8
 *       10ms需要: 8MHz/8 × 0.01s = 10000计数
 *       初值 = 65536 - 10000 = 55536 = 0xD8F0
 */
void HAL_Timer1_Init(void)
{
	T1CON = 0x30;           // 16位读写，1:8预分频，内部时钟
	TMR1H = 0xD8;           // 初值高字节
	TMR1L = 0xF0;           // 初值低字节
	
	/* 使能Timer1中断 */
	PIE1bits.TMR1IE = 1;    // 使能Timer1中断
	PIR1bits.TMR1IF = 0;    // 清除中断标志
	
	/* 启动Timer1 */
	T1CONbits.TMR1ON = 1;
}

/**
 * 函数: HAL_Timer1_ISR
 * 功能: Timer1中断服务程序（10ms）
 */
void HAL_Timer1_ISR(void)
{
	if (PIR1bits.TMR1IF)
	{
		PIR1bits.TMR1IF = 0;  // 清除中断标志
		
		/* 重载Timer1初值（10ms） */
		TMR1H = 0xD8;
		TMR1L = 0xF0;
		
		/* 在这里添加10ms周期任务 */
	}
}

/****************************************************************************
 * Timer2 相关函数
 ****************************************************************************/

/**
 * 函数: HAL_Timer2_Init
 * 功能: 初始化Timer2
 */
void HAL_Timer2_Init(void)
{
	/* Timer2配置 - 8位定时器 */
	T2CON = 0x00;           // 停止Timer2，1:1预分频和后分频
	PR2 = 0xFF;             // 周期寄存器
	TMR2 = 0x00;            // 初值
	
	/* 使能Timer2中断 */
	PIE1bits.TMR2IE = 1;    // 使能Timer2中断
	PIR1bits.TMR2IF = 0;    // 清除中断标志
	
	/* 启动Timer2 */
	T2CONbits.TMR2ON = 1;
}

/**
 * 函数: HAL_Timer2_ISR
 * 功能: Timer2中断服务程序
 */
void HAL_Timer2_ISR(void)
{
	if (PIR1bits.TMR2IF)
	{
		PIR1bits.TMR2IF = 0;  // 清除中断标志
		
		/* 在这里添加Timer2中断处理逻辑 */
	}
}

/**
 * 函数: HAL_Timer_ISR
 * 功能: Timer0中断服务程序（10ms）
 */
void HAL_Timer_ISR(void)
{
	/* Timer0中断：10ms系统滴答 */
	if (INTCONbits.TMR0IF)
	{
		INTCONbits.TMR0IF = 0;  // 清除标志
		TMR0H = 0xD8;           // 重载初值（10ms）
		TMR0L = 0xF0;
		
		// g_system_tick_ms += 10;  // 累加10ms
		FLG_SYS_10MS = 1;        // 设置10ms标志位
	}
}

/* 中断回调函数（由中间件层注册） */
static void (*g_timer3_callback_1ms)(void) = NULL;   // 1ms回调
static void (*g_timer3_callback_5ms)(void) = NULL;   // 5ms回调

/* 内部计数器 */
static uint8_t g_timer3_1ms_count = 0;

/**
 * 函数: HAL_Timer3_RegisterCallback
 * 功能: 注册Timer3回调函数
 */
void HAL_Timer3_RegisterCallback_1ms(void (*callback)(void))
{
	g_timer3_callback_1ms = callback;
}

void HAL_Timer3_RegisterCallback_5ms(void (*callback)(void))
{
	g_timer3_callback_5ms = callback;
}

/**
 * 函数: HAL_Timer3_ISR
 * 功能: Timer3中断服务程序（每1ms）
 * 说明: 
 *   按照分层架构设计：
 *   - HAL层只负责硬件定时和回调触发
 *   - 具体业务逻辑由Middleware层通过回调实现
 */
void HAL_Timer3_ISR(void)
{
	/* Timer3中断：1ms定时 */
	if (PIR2 & 0x02)  // TMR3IF
	{
		PIR2 = PIR2 & (~0x02);  // 清除中断标志
		TMR3H = 0xfc;           // 重载定时器值（1ms）
		TMR3L = 0x17;
		
		/* 系统心跳计数 */
		g_system_tick_ms += 1;  

		/* 调用1ms回调（如果已注册） */
		if (g_timer3_callback_1ms != NULL)
		{
			g_timer3_callback_1ms();
		}
		
		/* 5ms计数 */
		g_timer3_1ms_count++;
		if (g_timer3_1ms_count >= 5)
		{
			g_timer3_1ms_count = 0;
			
			/* 调用5ms回调（如果已注册） */
			if (g_timer3_callback_5ms != NULL)
			{
				g_timer3_callback_5ms();
			}
		}
	}
}

