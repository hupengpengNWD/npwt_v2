/****************************************************************************
 * 文件名: npwt_con_ofile_load_00.h
 * 功能: 气泵控制模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __npwt_con_ofile_load_00_h_
#define __npwt_con_ofile_load_00_h_

/* 函数声明 */
extern void OPEN_PwmA(void);
extern void CLS_PwmA(void);
extern void PRESS_ConA(void);
extern void PRESS_ConSubA(void);

#endif
