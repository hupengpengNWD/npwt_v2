/****************************************************************************
 * 文件名: main.c
 * 功能: 主程序入口（新组件测试版本）
 * 
 * 说明: 
 *   用于测试新组件功能
 *   基于步骤1的电源控制功能
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#include "../Inc/system_config.h"
#include "../Inc/system_types.h"
#include "../Inc/system_enums.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../HAL/Inc/hal_timer.h"
#include "../../Middleware/Inc/soft_timer.h"

/****************************************************************************
 * 全局变量
 ****************************************************************************/
extern volatile unsigned char FLG_SYS_10MS;

/* 系统状态 */
static SystemState_t g_system;

/****************************************************************************
 * 电源控制模块（保留步骤1的功能）
 ****************************************************************************/

/* 电源控制状态 */
typedef enum {
	POWER_STATE_INIT,      // 初始化，检测按键
	POWER_STATE_RUNNING,   // 运行中，电源已自锁
	POWER_STATE_SHUTDOWN   // 关机
} PowerState_e;

static PowerState_e power_state = POWER_STATE_INIT;
static uint16_t init_timer = 0;
static uint16_t key_hold_timer = 0;
static uint16_t shutdown_key_timer = 0;  // 关机按键计时器

/**
 * 函数: PowerControl_Update
 * 功能: 电源控制更新（每10ms调用）
 */
void PowerControl_Update(void)
{
	switch (power_state)
	{
	case POWER_STATE_INIT:
	{
		init_timer++;
		
		/* 前50个周期（500ms）检测按键 */
		if (init_timer <= 50)
		{
			uint8_t key = PORTB & 0x3C;
			if (key == 0x38)  // 确认键按下
			{
				key_hold_timer++;
			}
			else
			{
				key_hold_timer = 0;
			}
		}
		/* 第51个周期判断 */
		else if (init_timer == 51)
		{
			if (key_hold_timer < 30)  // 按键不足300ms
			{
				// 开机失败，复位
				HAL_Power_Release();
				init_timer = 0;
				key_hold_timer = 0;
			}
			else  // 按键足够
			{
				// 开机成功
				HAL_Power_Hold();
				LATCbits.LATC6 = 1;  // 背光
				power_state = POWER_STATE_RUNNING;
			}
		}
		/* 7秒后仍未开机，复位 */
		else if (init_timer >= 700)
		{
			init_timer = 0;
			key_hold_timer = 0;
		}
		break;
	}
	
	case POWER_STATE_RUNNING:
	{
		/* 运行中，检测关机按键（长按确认键） */
		uint8_t key = PORTB & 0x3C;
		
		if (key == 0x38)  // 确认键按下
		{
			shutdown_key_timer++;
			
			/* 长按2秒（200个10ms周期）触发关机 */
			if (shutdown_key_timer >= 200)
			{
				power_state = POWER_STATE_SHUTDOWN;
				shutdown_key_timer = 0;
			}
		}
		else
		{
			shutdown_key_timer = 0;  // 松开按键，复位计时器
		}
		break;
	}
	
	case POWER_STATE_SHUTDOWN:
	{
		// 关机处理
		HAL_Power_Release();
		while(1) { asm("clrwdt"); }
		break;
	}
	}
}

/****************************************************************************
 * 软件定时器测试区域
 ****************************************************************************/

/* 测试定时器句柄 */
static SoftTimerHandle_t g_test_timer1 = 0;
static SoftTimerHandle_t g_test_timer2 = 0;
static uint32_t g_test_counter = 0;

/* 测试回调函数1：单次定时器 */
void TestCallback1(void* user_data)
{
	// 翻转RC4 LED（绿色）
	LATCbits.LATC4 ^= 1;
	g_test_counter++;
}

/* 测试回调函数2：周期定时器 */
void TestCallback2(void* user_data)
{
	// 翻转RC6（背光）
	LATCbits.LATC6 ^= 1;
}

/**
 * 函数: SoftTimer_Test
 * 功能: 软件定时器测试函数
 * 说明: 测试软件定时器的各种功能
 */
void SoftTimer_Test(void)
{
	static uint32_t test_timer = 0;
	static bool test_initialized = false;
	
	/* 初始化测试（只执行一次） */
	if (!test_initialized) {
		// 创建单次定时器：1秒后执行
		g_test_timer1 = SoftTimer_Create(SOFT_TIMER_MODE_ONCE, 1000, TestCallback1, NULL);
		
		// 创建周期定时器：500ms周期
		g_test_timer2 = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 500, TestCallback2, NULL);
		
		// 启动定时器
		if (g_test_timer1 != 0) {
			SoftTimer_Start(g_test_timer1);
		}
		if (g_test_timer2 != 0) {
			SoftTimer_Start(g_test_timer2);
		}
		
		test_initialized = true;
	}
	
	/* 每5秒重新启动单次定时器 */
	test_timer++;
	if (test_timer >= 500) {  // 5秒
		test_timer = 0;
		
		if (g_test_timer1 != 0) {
			SoftTimer_Start(g_test_timer1);  // 重新启动
		}
	}
}

/****************************************************************************
 * 主函数
 ****************************************************************************/
void main(void)
{
	/* ========== 系统初始化 ========== */
	
	/* 1. 初始化振荡器（与未重构工程SYS_OSC_Ini完全一致） */
	OSCCON = 0b01110000;    // 内部振荡器，8MHz
	OSCTUNE = OSCTUNE | 0x40; // 使能4×PLL → 32MHz
	while (!(OSCCON & 0x08)); // 等待振荡器稳定
	asm("clrwdt");
	
	/* 2. 初始化GPIO */
	HAL_GPIO_Init();
	
	/* 3. 初始化定时器 */
	HAL_Timer_Init();     // Timer0: 10ms
	HAL_Timer1_Init();    // Timer1: 10ms
	HAL_PWM_Init();       // Timer3: 1ms
	
	/* 4. 初始化软件定时器模块 */
	SoftTimer_Init();
	
	/* 5. 使能全局中断 */
	T3CONbits.TMR3ON = 1; // 启动Timer3
	GIE = 1;
	PEIE = 1;
	asm("clrwdt");
	
	/* ========== 主循环 ========== */
	while (1)
	{
		/* 喂狗 */
		asm("clrwdt");
		
		/* 10ms任务 */
		if (FLG_SYS_10MS)
		{
			FLG_SYS_10MS = 0;
			
			/* 步骤1：电源控制更新 */
			PowerControl_Update();
			
			/* 软件定时器测试 */
			SoftTimer_Test();
		}
	}
}

/****************************************************************************
 * 中断服务程序
 ****************************************************************************/
void __interrupt() ISR(void)
{
	/* Timer0中断：10ms系统滴答 */
	if (T0IF)
	{
		T0IF = 0;
		TMR0 = 0x0BDC;  // 重装定时器值
		FLG_SYS_10MS = 1;
	}
	
	/* Timer1中断：10ms - 软件定时器tick更新 */
	if (TMR1IF)
	{
		TMR1IF = 0;
		TMR1H = 0x0B;
		TMR1L = 0xDC;
		
		/* 更新软件定时器 */
		SoftTimer_TickUpdate();
	}
	
	/* Timer3中断：1ms */
	if (TMR3IF)
	{
		TMR3IF = 0;
		TMR3H = 0xFC;
		TMR3L = 0x18;
	}
}