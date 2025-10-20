/****************************************************************************
 * 文件名: npwt_con_ifile_adc.h
 * 功能: ADC采集模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __npwt_con_ifile_adc_h_
#define __npwt_con_ifile_adc_h_

/* 函数声明 */
extern void ADC_Ps90(void);
extern void ADC_Bat90(void);
extern void ADC_Lew90(void);

#endif
