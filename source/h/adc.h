/****************************************************************************
 * 文件名: adc.h
 * 功能: ADC驱动模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __adc_h_
#define __adc_h_

/* 函数声明 */
extern void adc_init(void);
extern unsigned int Get_adc_result(unsigned char channel);

#endif
