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

/****************************************************************************
 * PIC18F46J11 配置位设置
 * 
 * 说明：
 *   配置位已在 config_bits.h 中完整定义
 *   该文件与未重构工程的配置完全一致
 *   包含所有必要的配置：振荡器、看门狗、电源管理等
 * 
 * 重要配置说明：
 *   - OSC = INTOSCPLL：使用内部振荡器+PLL（32MHz）
 *   - WDTEN = ON：看门狗使能
 *   - WDTPS = 16384：看门狗分频比
 *   - XINST = OFF：禁用扩展指令集（XC8要求）
 *   - CP0 = OFF：代码保护禁用
 ****************************************************************************/

/* 配置位在 config_bits.h 中定义 */
#include "config_bits.h"

#endif /* MCU_CONFIG_H */

