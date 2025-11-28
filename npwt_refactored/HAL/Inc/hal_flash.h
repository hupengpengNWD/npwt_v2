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
/* 注意：Flash地址必须满足以下条件：
 *   1. 在未使用的CODE区域内：0xC5BC - 0xE13D
 *   2. 不在程序代码范围内（避免覆盖程序）
 *   3. 不在常量数据范围内（避免覆盖LOGO等数据）
 *   4. 64字节对齐（Flash块对齐要求）
 * 
 * 当前地址：0xD640 (更新于代码使用率94.2%时)
 *   - 在未使用的CODE区域内，安全
 *   - 64字节对齐
 *   - 距离程序代码结束（0xD217）：1065字节 (1.04KB)
 *   - 距离常量数据开始（0xE13E）：2814字节 (2.75KB)
 *   - 安全边距：1KB
 *   - 可用扩展空间：2.75KB (约可支持后续2800字节代码增长)
 * 
 * 历史更新记录：
 *   - 0xD000 → 0xD640 (代码使用率从89%增至94.2%，增加查表法、过压报警等功能)
 * 
 * 如何调整地址（当代码增加新功能时）：
 *   1. 编译项目，生成map文件
 *   2. 运行脚本：python3 scripts/calculate_safe_flash_address.py NWPT_VR.X/dist/default/production/NWPT_VR.X.production.map 1
 *   3. 脚本会自动计算安全的Flash地址
 *   4. 更新本文件中的 HAL_FLASH_CONFIG_ADDRESS 宏定义
 * 
 * 详细说明请参考：scripts/README_FLASH_ADDRESS.md
 */
#define HAL_FLASH_CONFIG_ADDRESS    0xD640U    // 配置数据起始地址（使用脚本计算，1KB安全边距），当Os优化等级时配置为0xD2C0U时可以获得更多的配置控空间

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

