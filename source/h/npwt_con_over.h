/****************************************************************************
 * 文件名: npwt_con_over.h
 * 功能: 压力控制模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __npwt_con_over_h_
#define __npwt_con_over_h_

/* 函数声明 */
extern void CONTR_falla(void);
extern void CONTR_fallaNew(void);
extern void STAT_conNew(void);
extern void STAT_conNewa(void);

#endif
