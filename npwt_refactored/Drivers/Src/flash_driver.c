/****************************************************************************
 * 文件名: flash_driver.c
 * 功能: Flash存储驱动实现
 * 
 * 说明: 
 *   PIC18F46J11 Flash操作
 *   用于保存系统配置和校准参数
 * 
 * 存储布局（起始地址0xA000）:
 *   +0:  压力设置
 *   +2:  语言设置
 *   +4:  校准系数K1（高16位）
 *   +6:  校准系数K1（低16位）
 *   +8:  校准系数K2
 *   +10: 使用时长
 *   +12: CRC校验
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/flash_driver.h"
#include "../../Core/Inc/system_config.h"
#include "../../Core/Inc/mcu_config.h"  // 包含 <xc.h> 寄存器定义
#include <string.h>

/**
 * 函数: Flash_WriteCycle
 * 功能: Flash写周期（内部函数）
 */
static void Flash_WriteCycle(void)
{
	EECON1bits.WREN = 1;  // 使能写操作
	
	/* 保存并禁用中断 */
	unsigned char gie_backup = INTCONbits.GIE;
	INTCONbits.GIE = 0;
	
	/* 写序列（必须） */
	EECON2 = 0x55;
	EECON2 = 0xAA;
	EECON1bits.WR = 1;  // 启动写操作
	
	/* 等待5条NOP */
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	asm("nop");
	
	/* 等待写完成 */
	while (EECON1bits.WR);
	
	EECON1bits.WREN = 0;  // 禁用写操作
	
	/* 恢复中断 */
	INTCONbits.GIE = gie_backup;
}

/**
 * 函数: Flash_EraseBlock
 * 功能: 擦除Flash块（64字节）
 */
static void Flash_EraseBlock(uint32_t address)
{
	/* 设置地址指针 */
	TBLPTRU = (address >> 16) & 0xFF;
	TBLPTRH = (address >> 8) & 0xFF;
	TBLPTRL = address & 0xFF;
	
	EECON1bits.FREE = 1;  // 使能擦除
	Flash_WriteCycle();
}

/**
 * 函数: Flash_WriteWord
 * 功能: 写入一个字（16位）
 */
static void Flash_WriteWord(uint32_t address, uint16_t data)
{
	/* 设置地址指针 */
	TBLPTRU = (address >> 16) & 0xFF;
	TBLPTRH = (address >> 8) & 0xFF;
	TBLPTRL = address & 0xFF;
	
	/* 写入高字节 */
	TABLAT = (data >> 8) & 0xFF;
	asm("tblwt*+");  // 写入并递增
	
	/* 写入低字节 */
	TABLAT = data & 0xFF;
	asm("tblwt*");   // 写入
	
	EECON1bits.FREE = 0;   // 禁用擦除
	EECON1bits.WPROG = 1;  // 使能字编程
	Flash_WriteCycle();
}

/**
 * 函数: Flash_ReadWord
 * 功能: 读取一个字（16位）
 */
static uint16_t Flash_ReadWord(uint32_t address)
{
	uint16_t temp;
	
	/* 设置地址指针 */
	TBLPTRU = (address >> 16) & 0xFF;
	TBLPTRH = (address >> 8) & 0xFF;
	TBLPTRL = address & 0xFF;
	
	/* 读取高字节 */
	asm("tblrd*+");
	temp = TABLAT;
	temp <<= 8;
	
	/* 读取低字节 */
	asm("tblrd*");
	temp |= TABLAT;
	
	return temp;
}

/**
 * 函数: Flash_CalculateCRC
 * 功能: 计算CRC校验值（简单求和）
 */
static uint32_t Flash_CalculateCRC(const FlashConfig_t *config)
{
	uint32_t crc = 0;
	const uint8_t *p = (const uint8_t *)config;
	
	/* 对除CRC字段外的所有字节求和 */
	for (uint8_t i = 0; i < (sizeof(FlashConfig_t) - sizeof(uint32_t)); i++)
	{
		crc += p[i];
	}
	
	return crc;
}

/**
 * 函数: Flash_Init
 * 功能: 初始化Flash模块
 */
void Flash_Init(void)
{
	/* Flash模块无需特殊初始化 */
	EECON1 = 0;
}

/**
 * 函数: Flash_WriteConfig
 * 功能: 写入配置数据到Flash
 */
bool Flash_WriteConfig(const FlashConfig_t *config)
{
	if (config == NULL) return false;
	
	/* 创建临时副本并计算CRC */
	FlashConfig_t temp;
	memcpy(&temp, config, sizeof(FlashConfig_t));
	temp.crc = Flash_CalculateCRC(&temp);
	
	/* 擦除Flash块 */
	Flash_EraseBlock(FLASH_CONFIG_ADDRESS);
	
	/* 逐个字写入 */
	uint16_t *p = (uint16_t *)&temp;
	uint8_t word_count = sizeof(FlashConfig_t) / 2;
	
	for (uint8_t i = 0; i < word_count; i++)
	{
		Flash_WriteWord(FLASH_CONFIG_ADDRESS + (i * 2), p[i]);
	}
	
	/* 验证写入 */
	FlashConfig_t verify;
	if (!Flash_ReadConfig(&verify))
		return false;
	
	/* 比较数据 */
	if (memcmp(&temp, &verify, sizeof(FlashConfig_t)) != 0)
		return false;
	
	return true;
}

/**
 * 函数: Flash_ReadConfig
 * 功能: 从Flash读取配置数据
 */
bool Flash_ReadConfig(FlashConfig_t *config)
{
	if (config == NULL) return false;
	
	/* 逐个字读取 */
	uint16_t *p = (uint16_t *)config;
	uint8_t word_count = sizeof(FlashConfig_t) / 2;
	
	for (uint8_t i = 0; i < word_count; i++)
	{
		p[i] = Flash_ReadWord(FLASH_CONFIG_ADDRESS + (i * 2));
	}
	
	/* 验证CRC */
	uint32_t calculated_crc = Flash_CalculateCRC(config);
	if (calculated_crc != config->crc)
	{
		/* CRC错误，数据无效 */
		return false;
	}
	
	return true;
}


/****************************************************************************
 * Flash系统配置保存/加载函数
 ****************************************************************************/

/**
 * 保存系统配置到Flash
 */
bool Flash_SaveSystemSettings(SystemState_t *sys) {
    if (sys == NULL) return false;
    
    // 准备要保存的数据（使用现有FlashConfig_t结构）
    FlashConfig_t config;
    config.pressure_setting = sys->pressure.target_pressure;
    config.calibration_k1 = 1.0f;  // 默认校准系数
    config.calibration_k2 = 0.0f;  // 默认校准系数
    config.usage_hours = (uint16_t)(sys->uptime_ms / 3600000);  // 转换为小时
    
    // 写入Flash
    return Flash_WriteConfig(&config);
}

/**
 * 从Flash加载系统配置
 */
bool Flash_LoadSystemSettings(SystemState_t *sys) {
    if (sys == NULL) return false;
    
    FlashConfig_t config = {0};
    
    // 从Flash读取
    if (!Flash_ReadConfig(&config)) {
        return false;
    }
    
    // 恢复配置到系统状态
    sys->pressure.target_pressure = config.pressure_setting;
    
    return true;
}

