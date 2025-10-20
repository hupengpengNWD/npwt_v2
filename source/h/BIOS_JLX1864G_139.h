/****************************************************************************
 * 文件名: BIOS_JLX1864G_139.h
 * 功能: LCD BIOS模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   LCD驱动BIOS函数接口
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __BIOS_JLX1864G_139_h_
#define __BIOS_JLX1864G_139_h_

/* LCD BIOS函数声明 */
extern void Init_LCD(void);
extern void Clear_LCD(void);
extern void Display_Str(unsigned char page, unsigned char column, const unsigned char *str);
extern void Display_Num(unsigned char page, unsigned char column, unsigned int num);
extern void Display_Image(unsigned char page, unsigned char column, const unsigned char *image, unsigned char width);

#endif
