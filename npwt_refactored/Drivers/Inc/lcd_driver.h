/****************************************************************************
 * 文件名: lcd_driver.h
 * 功能: LCD显示驱动
 * 
 * 说明: 
 *   封装LCD显示操作
 *   基于BIOS_JLX1864G_139驱动
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_types.h"

/****************************************************************************
 * LCD驱动函数
 ****************************************************************************/

/**
 * 函数: LCD_Driver_Init
 * 功能: 初始化LCD
 */
void LCD_Driver_Init(void);

/**
 * 函数: LCD_Clear
 * 功能: 清屏
 */
void LCD_Clear(void);

/**
 * 函数: LCD_DisplayStartup
 * 功能: 显示开机画面
 */
void LCD_DisplayStartup(void);

/**
 * 函数: LCD_DisplayMode
 * 功能: 显示工作模式
 * 参数: mode - 工作模式
 *       language - 语言（0=俄语，1=英语）
 */
void LCD_DisplayMode(WorkMode_e mode, uint8_t language);

/**
 * 函数: LCD_DisplayPressure
 * 功能: 显示压力值
 * 参数: target - 目标压力（mmHg）
 *       current - 当前压力（mmHg）
 */
void LCD_DisplayPressure(uint16_t target, uint16_t current);

/**
 * 函数: LCD_DisplayBattery
 * 功能: 显示电池电量
 * 参数: level - 电量等级（0-4）
 *       is_charging - 是否充电中
 */
void LCD_DisplayBattery(uint8_t level, bool is_charging);

/**
 * 函数: LCD_DisplayError
 * 功能: 显示故障信息
 * 参数: error - 错误代码
 *       language - 语言
 */
void LCD_DisplayError(ErrorCode_e error, uint8_t language);

/**
 * 函数: LCD_SetBacklight
 * 功能: 设置背光
 * 参数: on - true=开启, false=关闭
 */
void LCD_SetBacklight(bool on);

/**
 * 函数: LCD_DisplayNumber
 * 功能: 在指定位置显示数字
 * 参数: page - 页（0-7）
 *       column - 列（0-127）
 *       number - 数字
 */
void LCD_DisplayNumber(uint8_t page, uint8_t column, uint16_t number);

/**
 * 函数: LCD_DisplayString
 * 功能: 在指定位置显示字符串
 * 参数: page - 页
 *       column - 列
 *       str - 字符串指针
 */
void LCD_DisplayString(uint8_t page, uint8_t column, const char *str);

#endif /* LCD_DRIVER_H */

