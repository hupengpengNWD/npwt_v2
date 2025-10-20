/****************************************************************************
 * 文件名: npwt_dis_main.h
 * 功能: 主程序头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 *   所有数据已封装到 system_manager.c 的结构体中
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __main__h__
#define __main__h__

/* 函数声明 */
extern void LEVEL_WarnA(void);
extern void BUZ_KeyCls(void);

#endif
