/****************************************************************************
 * 文件名: lcd_ui_elements.h
 * 功能: LCD UI元素（光标、图标等）
 * 
 * 说明: 
 *   提供UI装饰元素：三角形光标、锁定图标、静音图标等
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef LCD_UI_ELEMENTS_H
#define LCD_UI_ELEMENTS_H

#include <stdint.h>

/**
 * 函数: LCD_DisplayCursor
 * 功能: 显示三角形光标（►）
 * 参数: page - 页（0-7）
 *       column - 列（0-127）
 */
void LCD_DisplayCursor(uint8_t page, uint8_t column);

/**
 * 函数: LCD_ClearCursor
 * 功能: 清除三角形光标
 * 参数: page - 页（0-7）
 *       column - 列（0-127）
 */
void LCD_ClearCursor(uint8_t page, uint8_t column);

/**
 * 函数: LCD_DisplayLockIcon
 * 功能: 显示锁定图标（对应旧代码DISP_Lock）
 * 参数: page - 页（0-7）
 *       column - 列（0-127）
 */
void LCD_DisplayLockIcon(uint8_t page, uint8_t column);

/**
 * 函数: LCD_ClearLockIcon
 * 功能: 清除锁定图标（对应旧代码DISP_LockClr）
 * 参数: page - 页（0-7）
 *       column - 列（0-127）
 */
void LCD_ClearLockIcon(uint8_t page, uint8_t column);

/**
 * 函数: LCD_DisplayMuteIcon
 * 功能: 显示静音图标（对应旧代码DISP_Buz）
 * 参数: page - 页（0-7）
 *       column - 列（0-127）
 */
void LCD_DisplayMuteIcon(uint8_t page, uint8_t column);

/**
 * 函数: LCD_ClearMuteIcon
 * 功能: 清除静音图标（对应旧代码DISP_BuzClr）
 * 参数: page - 页（0-7）
 *       column - 列（0-127）
 */
void LCD_ClearMuteIcon(uint8_t page, uint8_t column);

#endif /* LCD_UI_ELEMENTS_H */

