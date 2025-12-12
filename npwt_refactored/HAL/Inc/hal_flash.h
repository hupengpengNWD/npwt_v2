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
 *   1. 不在程序代码范围内（避免覆盖程序）
 *   2. 不在常量数据范围内（避免覆盖LOGO等数据）
 *   3. 64字节对齐（Flash块对齐要求）
 * 
 * 当前地址：0xE7A0 (更新于代码结束地址0xE588和mediumconst起始地址0xEA0E时)
 *   - 64字节对齐
 *   - 距离程序代码结束（0xE588）：536字节 (0.52KB)
 *   - 距离mediumconst起始（0xEA0E）：558字节 (0.54KB)
 *   - 安全边距：代码方向512字节，常量数据方向512字节
 *   - 配置数据占用：64字节 (1个块)
 *   - 配置数据范围：0xE7A0 - 0xE7DF
 * 
 * 历史更新记录：
 *   - 0xD000 → 0xD640 (代码使用率从89%增至94.2%，增加查表法、过压报警等功能)
 *   - 0xD640 → 0xDCC0 (临时地址，存在被代码覆盖风险)
 *   - 0xDCC0 → 0xEA00 (代码结束地址0xE5EE，使用1KB安全边距)
 *   - 0xEA00 → 0xE700 (优化：减小安全边距至256字节，但距离代码结束地址太近)
 *   - 0xE700 → 0xE900 (修复：增加安全边距，但距离mediumconst太近，导致logo和字体数据被覆盖)
 *   - 0xE900 → 0xE7C0 (修复：调整到更安全的位置，但距离代码结束只有568字节，代码增长时可能覆盖)
 *   - 0xE7C0 → 0xE8C0 (修复：增加代码方向的安全边距，但距离mediumconst只有270字节，导致logo乱码)
 *   - 0xE8C0 → 0xE7A0 (修复：平衡两个方向的安全边距，都保持至少512字节)
 * 
 * 问题分析：
 *   - 0xE7C0距离代码结束只有568字节，如果代码增长可能覆盖配置数据
 *   - 0xE8C0距离mediumconst只有270字节，Flash块擦除时可能影响常量数据（logo、字体）
 *   - 0xE7A0距离代码结束536字节，距离mediumconst起始558字节，两个方向都有足够的安全边距
 * 
 * 如何调整地址（当代码增加新功能时）：
 *   1. 编译项目，生成map文件
 *   2. 运行脚本：python3 scripts/calculate_safe_flash_address.py [map_file] [safety_margin_kb]
 *   3. 检查代码最大结束地址和mediumconst起始地址
 *   4. 确保配置地址在代码结束地址之后，mediumconst起始地址之前
 *   5. 保持至少512字节的代码方向安全边距，512字节的常量数据方向安全边距
 *   6. 更新本文件中的 HAL_FLASH_CONFIG_ADDRESS 宏定义
 * 
 * 详细说明请参考：scripts/README_FLASH_ADDRESS.md
 */
#define HAL_FLASH_CONFIG_ADDRESS    0xE400u    // 1K 0-E3FF,E800-FFF7
//#define HAL_FLASH_CONFIG_ADDRESS   0xE540u // 128Byte  0-E53F,E5C0-FFF7


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

