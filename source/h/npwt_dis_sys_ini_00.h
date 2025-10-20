/****************************************************************************
 * 文件名: npwt_dis_sys_ini_00.h
 * 功能: 系统初始化模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __npwt_dis_sys_ini_00_h_
#define __npwt_dis_sys_ini_00_h_

/* 定时器结构体定义（软件定时器） */
typedef struct timer_st{
	unsigned long RecTickVal;    // 记录时刻值
	unsigned long TimeOutVal;    // 超时时间点
	unsigned char IsTimeOut;     // 超时标志
}TIMER;

/* 函数声明 */
extern void SYS_OSC_Ini(void);
extern void SYS_IO_Ini(void);
extern void SYS_TMR0_Ini(void);
extern void SYS_TMR3_Ini(void);
extern void SYS_ADCini(void);
extern void Isr(void);

extern unsigned long T1_TimeGet(void);
extern void T1_SetTimer(TIMER *timer, unsigned long nTime);
extern unsigned char IsTimeOut(TIMER *timer);

#endif
