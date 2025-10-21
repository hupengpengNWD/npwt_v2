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
 *   只配置必需的配置位（XINST），其他使用默认值
 *   详细配置应在MPLAB X项目属性中设置
 ****************************************************************************/

/* 必须禁用扩展指令集（XC8编译器不支持） */
#pragma config XINST = OFF

/* 看门狗定时器配置 */
#pragma config WDTEN = ON          // 看门狗定时器使能
#pragma config WDTPS = 128         // 看门狗分频：1:128

/* 振荡器配置 */
#pragma config OSC = HS            // 高速晶振模式（8MHz）
#pragma config FCMEN = ON          // 故障安全时钟监控使能
#pragma config IESO = ON           // 内外部振荡器切换使能

/* 堆栈和调试 */
#pragma config STVREN = ON         // 堆栈溢出复位使能
#pragma config DEBUG = OFF         // 调试禁用（生产环境）

/* 代码保护禁用 */
#pragma config CP0 = OFF           // 代码保护禁用

/* 
 * 注意：其余配置位（如 WRT, EBTR 等）在 PIC18F46J11 上不可用或名称不同
 * 请在 MPLAB X 项目属性 -> Configuration Bits 中检查和配置
 */

#endif /* MCU_CONFIG_H */

