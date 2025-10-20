/****************************************************************************
 * 文件名: Flash.h
 * 功能: Flash存储模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __Flash_h_
#define __Flash_h_

/* Flash地址定义 */
#define addr  0xa000  // 用户数据存储起始地址

/* 函数声明 */
extern void Flash_Write(unsigned int address, unsigned int data);
extern unsigned int Flash_Read(unsigned int address);

#endif
