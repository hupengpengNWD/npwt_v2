/****************************************************************************
 * 文件名: hal_flash.c
 * 功能: Flash硬件抽象层实现
 * 
 * 说明: 
 *   实现PIC18F46J11的Flash读写操作
 *   参考老版本工程的Flash.c实现
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
//static void HAL_Flash_WriteCycle(void)
//{
//    WREN = 1;
//    
//    CARRY = 0;
//    if(GIE) 
//        CARRY = 1;
//    GIE = 0;
//    
//    EECON2 = 0x55;
//    EECON2 = 0xAA;
//    
//    WR = 1;
//    NOP();
//    NOP();
//    NOP();
//    NOP();
//    NOP();
//    NOP();
//    while(WR);
//    WREN = 0;
//    
//    /* 修复中断恢复逻辑：如果之前中断使能，写入后恢复中断 */
//    if(CARRY) 
//        GIE = 0;//hpp
//}


static void HAL_Flash_WriteCycle(void)
{
    WREN = 1;          // 允许写

    // 备份全局中断状态到 CARRY
    CARRY = 0;
    if (GIE)
        CARRY = 1;
    GIE = 0;           // 关总中断，防止写过程中被打断

    EECON2 = 0x55;     // 解锁序列
    EECON2 = 0xAA;

    WR = 1;            // 启动写/擦除
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    NOP();
    while (WR);        // 等待完成

    WREN = 0;          // 禁止写

    // 写完后恢复全局中断
    if (CARRY)
        GIE = 1;
}


/**
 * @brief 初始化Flash模块
 * @note  参考lj/flash_driver.c中的Flash_Init()实现
 * @note  老版本工程的Flash.c没有Flash_Init()函数，所以这里也保持为空
 */
void HAL_Flash_Init(void)
{
    /* 老版本工程的Flash.c没有Flash_Init()函数，所以这里也保持为空 */
    /* 与老版本工程保持一致 */
}

/**
 * @brief 擦除Flash块（64字节）
 * @note  与老版本工程的Flash_Erase()完全一致
 */
//bool HAL_Flash_EraseBlock(uint32_t address)
//{
//    /* 检查地址是否64字节对齐 */
//    // if ((address & 0x3F) != 0) {
//    //     return false;
//    // }
//    
//    /* 设置地址指针（与老版本工程的计算方式一致） */
//    TBLPTRL = ((address) & 0xFF);
//    TBLPTRH = (((address) >> 8) & 0xFF);
//    TBLPTRU = (((address) >> 8) >> 8);
//    
//    FREE = 1;
//    HAL_Flash_WriteCycle();
//    
//    return true;
//}

bool HAL_Flash_EraseBlock(uint32_t address)
{
    // 1KB 对齐到块起始
    address &= 0xFFFFFC00u;
    
    // 64 字节对齐
//    address &= 0xFFFFFFC0u;   

    TBLPTRL = (uint8_t)(address & 0xFFu);
    TBLPTRH = (uint8_t)((address >> 8) & 0xFFu);
    TBLPTRU = (uint8_t)((address >> 16) & 0xFFu);

    // 访问程序 Flash
//    EEPGD = 1;   // Program memory
//    CFGS  = 0;   // 不是配置字

    // 擦除模式
    WPROG = 0;   // 不用 2-byte 编程模式
    FREE  = 1;   // 擦除 Enable

    HAL_Flash_WriteCycle();

    FREE  = 0;   // 擦除结束，清 FREE
    return true;
}




/**
 * @brief 写入一个字（16位）到Flash
 * @note  与老版本工程的Write_One_Word()完全一致，使用unsigned int类型
 */
//bool HAL_Flash_WriteWord(uint32_t address, unsigned int data)
//{
//    /* 检查地址是否2字节对齐 */
//    // if ((address & 0x01) != 0) {
//    //     return false;
//    // }
//    
//    /* 设置地址指针（与老版本工程的计算方式一致） */
//    TBLPTRL = ((address) & 0xFF);
//    TBLPTRH = (((address) >> 8) & 0xFF);
//    TBLPTRU = (((address) >> 8) >> 8);
//    
//    TABLAT = data>>8;
//    asm("\tTBLWT*+");
//    TABLAT = data;
//    asm("\tTBLWT*");
//    
//    FREE = 0;
//    WPROG = 1;
//    HAL_Flash_WriteCycle();
//    
//    return true;
//}

bool HAL_Flash_WriteWord(uint32_t address, unsigned int data)
{
    // 必须 2 字节对齐
    if (address & 0x01u)
        return false;

    // 设置地址（字节地址）
    TBLPTRL = (uint8_t)(address & 0xFFu);
    TBLPTRH = (uint8_t)((address >> 8) & 0xFFu);
    TBLPTRU = (uint8_t)((address >> 16) & 0xFFu);

    // 装载要写的 16-bit 数据，高字节在前
    TABLAT = (uint8_t)(data >> 8);
    asm("TBLWT*+");           // 写高字节并地址+1
    TABLAT = (uint8_t)(data & 0xFF);
    asm("TBLWT*");            // 写低字节

    // 访问程序 Flash，按“单 word 编程”
//    EEPGD = 1;   // Program memory
//    CFGS  = 0;   // 非配置区
    FREE  = 0;   // 写模式
    WPROG = 1;   // 写 1 word（2 字节），不是 64 字节块

    HAL_Flash_WriteCycle();

    WPROG = 0;   // 用完关掉（可选）
    return true;
}



/**
 * @brief 从Flash读取一个字（16位）
 * @note  与老版本工程的Flash_Read()完全一致，使用unsigned int类型
 */
unsigned int HAL_Flash_ReadWord(uint32_t address)
{
    unsigned int temp;
    
    /* 设置地址指针（与老版本工程的计算方式一致） */
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
 * @note  与老版本工程保持一致，不恢复EECON1寄存器（老版本工程也没有恢复）
 */
// bool HAL_Flash_WriteBlock(uint32_t address, const uint8_t* buffer, uint16_t size)
// {
//     if (buffer == NULL || size == 0) {
//         return false;
//     }
    
//     /* 按字（16位）写入 */
//     const uint16_t* word_buffer = (const uint16_t*)buffer;
//     uint16_t word_count = (size + 1) / 2;  // 向上取整
    
//     for (uint16_t i = 0; i < word_count; i++) {
//         if (!HAL_Flash_WriteWord(address + (i * 2), word_buffer[i])) {
//             return false;
//         }
//     }
    
//     /* 与老版本工程保持一致，不恢复EECON1寄存器 */
//     /* 老版本工程的Write_One_Word()只恢复WREN=0（在Write_Cycle中），不恢复其他位 */
    
//     return true;
// }

