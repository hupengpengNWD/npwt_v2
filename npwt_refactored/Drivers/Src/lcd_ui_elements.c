/****************************************************************************
 * 文件名: lcd_ui_elements.c
 * 功能: LCD UI元素实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/lcd_ui_elements.h"

/* 引用旧BIOS函数 */
extern void BIOS_JLX12864_TRANS_CMD(int data);
extern void BIOS_JLX12864_TRANS_DAT(int data);

/* 三角形光标图案（8x8，右向箭头 ►） */
const uint8_t CURSOR_TRIANGLE[8] = {
	0xF8, 0x78, 0x78, 0x38, 0x38, 0x18, 0x18, 0x08
};

/* 锁定图标图案（9x2页=18字节） */
const uint8_t LOCK_ICON[18] = {
	0x00, 0x00, 0xFE, 0x01, 0x01, 0x01, 0xFE, 0x00, 0x00,
	0x00, 0x00, 0x0F, 0x18, 0x18, 0x18, 0x0F, 0x00, 0x00
};

/* 静音/蜂鸣器图标图案（16x2页=32字节） */
const uint8_t MUTE_ICON[32] = {
	0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xE0,
	0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x07, 0x0F, 0x0F, 0x0F,
	0x0F, 0x0F, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00
};

/**
 * 函数: LCD_DisplayCursor
 * 功能: 显示三角形光标
 */
void LCD_DisplayCursor(uint8_t page, uint8_t column)
{
	uint8_t n = (116 - column);
	uint8_t j = n >> 4;
	uint8_t k = n & 0x0F;
	uint8_t i = (6 - page);
	
	/* 设置位置 */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	/* 显示三角形 */
	for (uint8_t idx = 0; idx < 8; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(CURSOR_TRIANGLE[idx]);
	}
}

/**
 * 函数: LCD_ClearCursor
 * 功能: 清除三角形光标
 */
void LCD_ClearCursor(uint8_t page, uint8_t column)
{
	uint8_t n = (116 - column);
	uint8_t j = n >> 4;
	uint8_t k = n & 0x0F;
	uint8_t i = (6 - page);
	
	/* 设置位置 */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	/* 清除（填充0） */
	for (uint8_t idx = 0; idx < 8; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(0x00);
	}
}

/**
 * 函数: LCD_DisplayLockIcon
 * 功能: 显示锁定图标（对应旧代码DISP_Lock）
 */
void LCD_DisplayLockIcon(uint8_t page, uint8_t column)
{
	uint8_t n = (116 - column);
	uint8_t j = n >> 4;
	uint8_t k = n & 0x0F;
	uint8_t i = (6 - page);
	
	/* 显示第一页（上半部分） */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 0; idx < 9; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(LOCK_ICON[idx]);
	}
	
	/* 显示第二页（下半部分） */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i + 1);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 9; idx < 18; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(LOCK_ICON[idx]);
	}
}

/**
 * 函数: LCD_DisplayMuteIcon
 * 功能: 显示静音图标（对应旧代码DISP_Buz）
 */
void LCD_DisplayMuteIcon(uint8_t page, uint8_t column)
{
	uint8_t n = (116 - column);
	uint8_t j = n >> 4;
	uint8_t k = n & 0x0F;
	uint8_t i = (6 - page);
	
	/* 显示第一页（上半部分） */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 0; idx < 16; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(MUTE_ICON[idx]);
	}
	
	/* 显示第二页（下半部分） */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i + 1);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 16; idx < 32; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(MUTE_ICON[idx]);
	}
}

/**
 * 函数: LCD_ClearLockIcon
 * 功能: 清除锁定图标（对应旧代码DISP_LockClr）
 */
void LCD_ClearLockIcon(uint8_t page, uint8_t column)
{
	uint8_t n = (116 - column);
	uint8_t j = n >> 4;
	uint8_t k = n & 0x0F;
	uint8_t i = (6 - page);
	
	/* 清除第一页 */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 0; idx < 9; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(0x00);
	}
	
	/* 清除第二页 */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i + 1);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 0; idx < 9; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(0x00);
	}
}

/**
 * 函数: LCD_ClearMuteIcon
 * 功能: 清除静音图标（对应旧代码DISP_BuzClr）
 */
void LCD_ClearMuteIcon(uint8_t page, uint8_t column)
{
	uint8_t n = (116 - column);
	uint8_t j = n >> 4;
	uint8_t k = n & 0x0F;
	uint8_t i = (6 - page);
	
	/* 清除第一页 */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 0; idx < 16; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(0x00);
	}
	
	/* 清除第二页 */
	BIOS_JLX12864_TRANS_CMD(0xB0 + i + 1);
	BIOS_JLX12864_TRANS_CMD(0x10 + j);
	BIOS_JLX12864_TRANS_CMD(0x00 + k);
	
	for (uint8_t idx = 0; idx < 16; idx++)
	{
		BIOS_JLX12864_TRANS_DAT(0x00);
	}
}
