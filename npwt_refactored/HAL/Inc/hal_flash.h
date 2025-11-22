/****************************************************************************
 * 文件名: hal_flash.h
 * 功能: Flash硬件抽象层
 * 
 * 说明: 
 *   提供Flash存储器的底层读写接口
 *   封装PIC18F46J11的Flash操作细节
 * 
 * 特性:
 *   - 支持64字节块擦除
 *   - 支持16位字读写
 *   - 写操作期间自动禁用中断
 * 
 * 创建日期: 2025-11-21
 ****************************************************************************/

#ifndef HAL_FLASH_H
#define HAL_FLASH_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * Flash地址定义
 ****************************************************************************/
/* 注意：程序代码占用到约0xE3F5，必须使用程序代码范围之外的地址 */
/* 原地址0xA000在程序代码范围内，会导致覆盖程序代码，使设备无法启动 */
/* 使用Flash最后2%区域（0xFAC0 - 0xFFF7），约1.3KB，足够保存参数 */
#define HAL_FLASH_CONFIG_ADDRESS    0xFAC0U    // 配置数据起始地址（Flash最后2%区域，64字节对齐）

/****************************************************************************
 * Flash操作函数
 ****************************************************************************/

/**
 * @name      HAL_Flash_Init
 * @brief     初始化Flash模块
 * @param     无
 * @retval    无
 * @note      当前无需特殊初始化，保留接口以备将来扩展
 */
void HAL_Flash_Init(void);

/**
 * @name      HAL_Flash_EraseBlock
 * @brief     擦除Flash块（64字节）
 * @param     address - 要擦除的块起始地址（必须是64字节对齐）
 * @retval    true=成功, false=失败
 * @note      擦除操作会禁用中断，操作完成后恢复
 */
bool HAL_Flash_EraseBlock(uint32_t address);

/**
 * @name      HAL_Flash_WriteWord
 * @brief     写入一个字（16位）到Flash
 * @param     address - 写入地址（必须是2字节对齐）
 * @param     data - 要写入的数据（16位，unsigned int类型，与未重构工程保持一致）
 * @retval    true=成功, false=失败
 * @note      写入前必须确保该地址所在的块已被擦除
 *           写入操作会禁用中断，操作完成后恢复
 */
bool HAL_Flash_WriteWord(uint32_t address, unsigned int data);

/**
 * @name      HAL_Flash_ReadWord
 * @brief     从Flash读取一个字（16位）
 * @param     address - 读取地址（必须是2字节对齐）
 * @retval    读取的数据（16位，unsigned int类型，与未重构工程保持一致）
 * @note      读取操作不会修改Flash内容，可以随时调用
 */
unsigned int HAL_Flash_ReadWord(uint32_t address);

/**
 * @name      HAL_Flash_ReadBlock
 * @brief     从Flash读取一个块（64字节）
 * @param     address - 读取起始地址（必须是64字节对齐）
 * @param     buffer - 数据缓冲区（至少64字节）
 * @param     size - 要读取的字节数（建议64字节）
 * @retval    true=成功, false=失败（参数无效）
 * @note      用于批量读取，提高效率
 */
bool HAL_Flash_ReadBlock(uint32_t address, uint8_t* buffer, uint16_t size);

/**
 * @name      HAL_Flash_WriteBlock
 * @brief     写入一个块到Flash（64字节）
 * @param     address - 写入起始地址（必须是64字节对齐）
 * @param     buffer - 数据缓冲区（至少64字节）
 * @param     size - 要写入的字节数（建议64字节）
 * @retval    true=成功, false=失败
 * @note      写入前必须确保该块已被擦除
 *           内部会按字（16位）写入
 */
bool HAL_Flash_WriteBlock(uint32_t address, const uint8_t* buffer, uint16_t size);

#endif /* HAL_FLASH_H */

