/****************************************************************************
 * 文件名: mcu_config.h
 * 功能: MCU硬件配置和寄存器定义
 * 
 * 说明: 
 *   包含 PIC18F46J11 的头文件和配置位设置
 *   兼容 XC8 和 HT-PICC 编译器
 * 
 * 创建日期: 2025-10-21
 ****************************************************************************/

#ifndef MCU_CONFIG_H
#define MCU_CONFIG_H

/* 根据编译器选择对应的头文件 */
//#if defined(__XC8)
//    /* XC8 编译器（MPLAB X 默认） */
//    #include <xc.h>
//#elif defined(_HTC_H_)
    /* HT-PICC 编译器（旧工程） */
//    #include <htc.h>
//#else
//    /* 默认使用 XC8 */
    #include <xc.h>
//#endif

#include <stdint.h>
#include <stdbool.h>

/* 定义 NULL（如果标准库未定义） */
#ifndef NULL
#define NULL ((void*)0)
#endif

/* 定义延时宏（需要定义 _XTAL_FREQ） */
#ifndef _XTAL_FREQ
#define _XTAL_FREQ 8000000UL  // 8MHz 晶振
#endif

/* 延时宏 */
#define __delay_us(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000000.0)))
#define __delay_ms(x) _delay((unsigned long)((x)*(_XTAL_FREQ/4000.0)))

/* NOP宏定义（与未重构工程一致） */
#define NOP()       asm("nop")

/* 注意：WREN, GIE, WR, CARRY, FREE, WPROG 等位变量
 * 已由 XC8 编译器的 pic18f46j11.h 头文件直接提供定义
 * 无需在此定义宏，可直接使用这些名称
 */

/****************************************************************************
 * PIC18F46J11 配置位设置
 * 
 * 说明：
 *   完整的配置位定义，与原工程配置一致
 *   包含所有必要的配置：振荡器、看门狗、电源管理等
 * 
 * 重要配置说明：
 *   - OSC = INTOSCPLL：使用内部振荡器+PLL（8MHz×4=32MHz）
 *   - WDTEN = ON：看门狗使能
 *   - WDTPS = 16384：看门狗分频比（约65秒超时）
 *   - XINST = OFF：禁用扩展指令集（XC8编译器要求）
 *   - CP0 = OFF：代码保护禁用（方便调试和下载）
 ****************************************************************************/

/* CONFIG1L - 看门狗和堆栈配置 */
#pragma config WDTEN = OFF          // 看门狗定时器禁用（开发阶段）
#pragma config STVREN = ON          // 堆栈溢出/下溢复位使能
#pragma config XINST = OFF          // 扩展指令集禁用（XC8要求）

/* CONFIG1H - 代码保护 */
#pragma config CP0 = OFF            // 程序存储器不受代码保护

/* CONFIG2L - 振荡器配置 */
#pragma config OSC = INTOSCPLL      // 内部振荡器+PLL（8MHz×4=32MHz）
#pragma config T1DIG = ON           // 辅助振荡器时钟源可选
#pragma config LPT1OSC = OFF        // Timer1高功率操作模式
#pragma config FCMEN = ON           // 故障安全时钟监控使能
#pragma config IESO = ON            // 内外部振荡器切换使能

/* CONFIG2H - 看门狗分频 */
#pragma config WDTPS = 16384        // 看门狗后分频器 1:16384

/* CONFIG3L - 深度睡眠和实时时钟配置 */
#pragma config DSWDTOSC = INTOSCREF // 深度睡眠看门狗时钟源选择INTRC
#pragma config RTCOSC = T1OSCREF    // RTCC时钟源选择T1OSC/T1CKI
#pragma config DSBOREN = ON         // 深度睡眠BOR使能
#pragma config DSWDTEN = OFF        // 深度睡眠看门狗定时器禁用
#pragma config DSWDTPS = G2         // 深度睡眠看门狗后分频器 1:2,147,483,648

/* CONFIG3H - IO锁定和MSSP配置 */
#pragma config IOL1WAY = OFF        // IOLOCK位可根据需要设置和清除
#pragma config MSSP7B_EN = MSK7     // MSSP地址屏蔽 7位地址屏蔽模式

/* CONFIG4L - 写/擦除保护配置 */
#pragma config WPFP = PAGE_63       // 写/擦除保护页起始/结束位置
#pragma config WPEND = PAGE_WPFP    // 写/擦除保护区域选择
#pragma config WPCFG = OFF          // 配置字页不受擦除/写保护

/* CONFIG4H - 写保护禁用 */
#pragma config WPDIS = OFF          // 写保护禁用位（忽略WPFP/WPEND区域）

#endif /* MCU_CONFIG_H */

