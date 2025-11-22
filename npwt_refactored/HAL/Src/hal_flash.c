/****************************************************************************
 * 文件名: hal_flash.c
 * 功能: Flash硬件抽象层实现
 * 
 * 说明: 
 *   实现PIC18F46J11的Flash读写操作
 *   参考未重构工程的Flash.c实现
 * 
 * 创建日期: 2025-11-21
 ****************************************************************************/

#include "../Inc/hal_flash.h"
#include "../../Core/Inc/mcu_config.h"  // 包含 <xc.h> 寄存器定义
#include <string.h>

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/

/**
 * @brief Flash写周期（内部函数）
 * @note  必须按照特定序列写入才能生效
 */
static void HAL_Flash_WriteCycle(void);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @brief Flash写周期（内部函数）
 * @note  修复中断恢复逻辑：如果之前中断使能，写入后恢复中断
 */
static void HAL_Flash_WriteCycle(void)
{
    WREN = 1;
    
    CARRY = 0;
    if(GIE) 
        CARRY = 1;
    GIE = 0;
    
    EECON2 = 0x55;
    EECON2 = 0xAA;
    
    WR = 1;
//    NOP();
//    NOP();
//    NOP();
//    NOP();
//    NOP();
//    NOP();
    
    __nop();
    __nop();
    __nop();    
    __nop();
    __nop();
    __nop();
    while(WR);
    WREN = 0;
    
    /* 修复中断恢复逻辑：如果之前中断使能，写入后恢复中断 */
    if(CARRY) 
        GIE = 0;//hpp
}

/**
 * @brief 初始化Flash模块
 * @note  参考lj/flash_driver.c中的Flash_Init()实现
 */
void HAL_Flash_Init(void)
{
    /* 初始化EECON1寄存器，确保所有Flash控制位处于安全状态 */
//    EECON1 = 0;  // 清零所有Flash控制位（WREN, FREE, WPROG等）
}

/**
 * @brief 擦除Flash块（64字节）
 * @note  与未重构工程的Flash_Erase()完全一致
 */
bool HAL_Flash_EraseBlock(uint32_t address)
{
    /* 检查地址是否64字节对齐 */
    // if ((address & 0x3F) != 0) {
    //     return false;
    // }
    
    /* 设置地址指针（与未重构工程的计算方式一致） */
    TBLPTRL = ((address) & 0xFF);
    TBLPTRH = (((address) >> 8) & 0xFF);
    TBLPTRU = (((address) >> 8) >> 8);
    
    FREE = 1;
    HAL_Flash_WriteCycle();
    
    return true;
}

/**
 * @brief 写入一个字（16位）到Flash
 * @note  与未重构工程的Write_One_Word()完全一致，使用unsigned int类型
 */
bool HAL_Flash_WriteWord(uint32_t address, unsigned int data)
{
    /* 检查地址是否2字节对齐 */
    // if ((address & 0x01) != 0) {
    //     return false;
    // }
    
    /* 设置地址指针（与未重构工程的计算方式一致） */
    TBLPTRL = ((address) & 0xFF);
    TBLPTRH = (((address) >> 8) & 0xFF);
    TBLPTRU = (((address) >> 8) >> 8);
    
    TABLAT = data>>8;
    asm("\tTBLWT*+");
    TABLAT = data;
    asm("\tTBLWT*");
    
    FREE = 0;
    WPROG = 1;
    HAL_Flash_WriteCycle();
    
    return true;
}

/**
 * @brief 从Flash读取一个字（16位）
 * @note  与未重构工程的Flash_Read()完全一致，使用unsigned int类型
 */
unsigned int HAL_Flash_ReadWord(uint32_t address)
{
    unsigned int temp;
    
    /* 设置地址指针（与未重构工程的计算方式一致） */
    TBLPTRL = ((address) & 0xFF);
    TBLPTRH = (((address) >> 8) & 0xFF);
    TBLPTRU = (((address) >> 8) >> 8);
    
    asm("\tTBLRD*+");
    temp = TABLAT;
    temp <<= 8;
    asm("\tTBLRD*+");
    temp |= TABLAT;
    
    return temp;
}

/**
 * @brief 从Flash读取一个块（64字节）
 */
bool HAL_Flash_ReadBlock(uint32_t address, uint8_t* buffer, uint16_t size)
{
    if (buffer == NULL || size == 0) {
        return false;
    }
    
    /* 按字（16位）读取 */
    uint16_t* word_buffer = (uint16_t*)buffer;
    uint16_t word_count = (size + 1) / 2;  // 向上取整
    
    for (uint16_t i = 0; i < word_count; i++) {
        word_buffer[i] = HAL_Flash_ReadWord(address + (i * 2));
    }
    
    return true;
}

/**
 * @brief 写入一个块到Flash（64字节）
 * @note  与未重构工程保持一致，不恢复EECON1寄存器（未重构工程也没有恢复）
 */
bool HAL_Flash_WriteBlock(uint32_t address, const uint8_t* buffer, uint16_t size)
{
    if (buffer == NULL || size == 0) {
        return false;
    }
    
    /* 按字（16位）写入 */
    const uint16_t* word_buffer = (const uint16_t*)buffer;
    uint16_t word_count = (size + 1) / 2;  // 向上取整
    
    for (uint16_t i = 0; i < word_count; i++) {
        if (!HAL_Flash_WriteWord(address + (i * 2), word_buffer[i])) {
            return false;
        }
    }
    
    /* 与未重构工程保持一致，不恢复EECON1寄存器 */
    /* 未重构工程的Write_One_Word()只恢复WREN=0（在Write_Cycle中），不恢复其他位 */
    
    return true;
}

