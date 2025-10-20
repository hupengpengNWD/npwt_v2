/****************************************************************************
 * 文件名: npwt_dis_sys_ini_00.c
 * 功能: 系统初始化与中断服务程序
 * 
 * 主要功能:
 *   1. 振荡器配置（32MHz）
 *   2. GPIO端口初始化
 *   3. 定时器0初始化（20ms主循环）
 *   4. 定时器3初始化（PWM和UART）
 *   5. ADC模块初始化
 *   6. 中断服务程序（ISR）
 * 
 * 中断服务:
 *   - 定时器3中断：PWM控制、压力采集（每20ms）
 *   - 定时器0中断：设置主循环标志（每20ms）
 * 
 * PWM控制:
 *   - 频率：50Hz（周期20ms，可调整为10kHz）
 *   - 占空比：40%~60%（根据目标压力）
 ****************************************************************************/

#include  "include.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_sys_ini_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_dis_main.h"
#include   "npwt_dis_ifile_key_00.h"
#include   "npwt_con_over.h"

/******************** 系统控制标志和计数器 ********************/
unsigned char FLG_SYS_10MS;           // 主循环20ms标志（由TMR0中断设置）
unsigned char  os_cnt=0;              // 压力采集计数器：每5次中断采集一次
unsigned char  pwm_cnt1=0;            // PWM计数器1：当前计数值
unsigned char  pwm_cnt2=0;            // PWM计数器2：占空比设定值（4~6对应40%~60%）

volatile unsigned long   Timeone_sum = 0;  // 系统运行时间累加器
unsigned char TIMER_FOUR=0;           // 定时器分频计数器
void SYS_OSC_Ini(void)
{
	OSCCON  =  0b01110000  ;
		OSCTUNE =  OSCTUNE|0x40;
 	while( !(OSCCON&0x08) )  ;         
}

/**
 * 函数: SYS_IO_Ini
 * 功能: GPIO端口初始化
 * 说明: 配置所有IO口的输入/输出方向和初始电平
 * 
 * 端口分配:
 *   PORTA: RA0-2,4,5输入（ADC、音频），RA1,3,5输出（音频控制）
 *   PORTB: RB0-1输出（VAL1/VAL2电磁阀），RB2-7输入（按键）
 *   PORTC: RC0-1输入（电池状态），RC2-7输出（电源、气泵、LED）
 *   PORTD: 全部输出（LCD数据线D0-D7）
 *   PORTE: 全部输出（LCD控制线）
 */
void SYS_IO_Ini(void)
{
	TRISA  =  0b00110111;   // bit0,1,2,4,5=输入，bit3,5=输出
	LATA  =  0;             // 输出锁存器清零
	PORTA  =  0;            // 端口清零
	
	TRISB  =  0b11111100;   // bit0,1=输出（VAL1/VAL2），bit2-7=输入（按键）
	PORTB  =  0;
	LATB  =  0;
	
	TRISC  =  0b00000011;   // bit0,1=输入（BAT_GOOD/CHARGE），bit2-7=输出
	PORTC  =  0;
	LATC  =  0;
	
	LATD  =  0;             // PORTD全部输出（LCD数据）
	TRISD  =  0;
	PORTD  =  0;
	
	LATE  =  0;             // PORTE全部输出（LCD控制）
	TRISE  =  0;
	PORTE  =  0;
	
	ANCON0  =  0xff;        // 禁用所有模拟输入（改用数字IO）
	ANCON1  =  0xff;
}
void SYS_TMR0_Ini(void)
{
	INTCON = 0x20;                
	INTCON2 = 0x80;               
	TMR0H = 0xd8;                 
	TMR0L = 0xef;                 
	T0CON = 0x83;                 
}

void SYS_TMR3_Ini(void)
{
	T3CON    =  0x34;
	
	PIE2  =  PIE2|0x02;

}

/**
 * 函数: SYS_ADCini
 * 功能: ADC模块初始化
 * 说明: 配置ADC为AN0通道，右对齐，参考电压为VDD
 */
void SYS_ADCini(void)
{
	ADCON0  =  0x01;   // 使能ADC，选择通道AN0
	ADCON1  =  0xbe;   // 右对齐，参考电压VDD/VSS
	ANCON0  =  0x01;   // AN0配置为模拟输入
}

/**
 * 函数: T1_TimeGet
 * 功能: 获取系统运行时间
 * 返回: 系统时间（单位：约20ms）
 */
unsigned long T1_TimeGet()
{
	return  Timeone_sum;
}

/**
 * 函数: TimeOutSet
 * 功能: 设置定时器超时值
 * 参数: timer - 定时器结构指针, timeout - 超时时间
 */
void TimeOutSet(TIMER *timer, unsigned long timeout)
{
	timer->RecTickVal = T1_TimeGet();           // 记录当前时间
	timer->TimeOutVal = T1_TimeGet() + timeout; // 计算超时时间点
	timer->IsTimeOut = FALSE;                   // 清除超时标志
}

/**
 * 函数: IsTimeOut
 * 功能: 检查定时器是否超时
 * 参数: timer - 定时器结构指针
 * 返回: TRUE=超时, FALSE=未超时
 */
unsigned char IsTimeOut(TIMER *timer)
{
	if(FALSE==timer->IsTimeOut)
	{
		if((long)(T1_TimeGet() - timer->TimeOutVal) >= 0)
		{
			timer->IsTimeOut = TRUE;
		}
	}
	return timer->IsTimeOut;
}

/* 压力采集相关 */
unsigned short   adc_ps00;           // 当前压力值缓存
unsigned char    temp_counter=0;     // 临时计数器
unsigned int     psps[30];           // 压力值历史记录数组
unsigned short   psps_turn=0;        // 压力记录轮次索引

unsigned char run_arrived=0;         // 运行到达标志

/****************************************************************************
 * 函数: Isr (中断服务程序)
 * 功能: 处理所有中断事件
 * 
 * 中断源:
 *   1. 定时器3中断（PIR2.1）：每1ms触发
 *      - PWM波形生成（控制气泵转速）
 *      - 压力采集（每5ms一次）
 *      - 压力控制算法执行
 *      - 系统时间累加
 *   
 *   2. 定时器0中断（INTCON.TMR0IF）：每20ms触发
 *      - 设置主循环标志FLG_SYS_10MS
 ****************************************************************************/
void __interrupt() Isr(void)
{
	/* ========== 中断源1：定时器3（每1ms） ========== */
	if (PIR2&0x02)  // TMR3IF：定时器3中断标志
	{
		PIR2 = PIR2&(~0x02);  // 清除中断标志
		TMR3H =0xfc;          // 重载定时器值（1ms）
		TMR3L =0x17;
		
		/* 任务1：ADC压力采集（每5ms一次） */
		ADC_Ps90();  // 采集压力传感器值
		if(os_cnt++==4)  // 每5次中断执行一次压力控制
		{
			os_cnt=0;
			PRESS_ConA();  // 执行压力控制算法
		}
		
		/* 任务2：系统时间累加（每5ms加1） */
		TIMER_FOUR++;
		if(TIMER_FOUR>4)
		{
			if( Timeone_sum<= 0xfffffffe)
				Timeone_sum++;  // 系统运行时间+1
			else
				Timeone_sum = 0;  // 溢出后清零
			TIMER_FOUR=0;
		}
		/* 任务3：PWM波形生成（控制气泵转速） */
		if(open_bum)  // 如果允许气泵工作
		{
			pwm_cnt1++;           // PWM计数器递增
			oppump_flg =1;        // 设置气泵运行标志
			
			if(pwm_cnt1<=PWM_PERO)  // 在PWM周期内（PWM_PERO=10，对应10ms周期，50Hz）
			{
				// 判断是否允许开启气泵（排除间歇模式的低压阶段）
				if(mod_jixa!=1)  // 非间歇模式
				{
					PUMP=1;  // 开启气泵
				}
				else  // 间歇模式
				{
					if((mod_jixa==1)&&(jx_current_phase==0))  // 间歇模式且在高压阶段
					{
						PUMP=1;  // 开启气泵
					}
				}
			}
			else  // PWM周期结束
				pwm_cnt1=0;  // 重置PWM计数器
		}
		else  // 不允许气泵工作
		{
			PUMP=0;      // 关闭气泵
			pwm_cnt1=0;  // 清零计数器
		}
	}
	
	/* ========== 中断源2：定时器0（每20ms） ========== */
	else if(INTCONbits.TMR0IF)  // TMR0IF：定时器0溢出中断
	{
		INTCONbits.TMR0IF = 0;  // 清除中断标志
		FLG_SYS_10MS = 1;       // 设置主循环执行标志
		
		TMR0H = 0xd8;           // 重载定时器初值
		TMR0L = 0xef;
	}
}

