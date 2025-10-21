/****************************************************************************
 * 文件名: flash_driver.h
 * 功能: Flash存储驱动
 * 
 * 说明: 
 *   提供Flash读写接口
 *   用于保存系统配置和校准参数
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef FLASH_DRIVER_H
#define FLASH_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_types.h"

/****************************************************************************
 * Flash地址定义
 ****************************************************************************/
#define FLASH_CONFIG_ADDRESS    0xA000      // 配置数据起始地址

/****************************************************************************
 * Flash操作函数
 ****************************************************************************/

/**
 * 函数: Flash_Init
 * 功能: 初始化Flash模块
 */
void Flash_Init(void);

/**
 * 函数: Flash_WriteConfig
 * 功能: 写入配置数据到Flash
 * 参数: config - 配置数据指针
 * 返回: true=成功, false=失败
 */
bool Flash_WriteConfig(const FlashConfig_t *config);

/**
 * 函数: Flash_ReadConfig
 * 功能: 从Flash读取配置数据
 * 参数: config - 配置数据指针
 * 返回: true=成功, false=失败
 */
bool Flash_ReadConfig(FlashConfig_t *config);

/**
 * 函数: Flash_EraseConfig
 * 功能: 擦除Flash配置区
 * 返回: true=成功, false=失败
 */
bool Flash_EraseConfig(void);

/**
 * 函数: Flash_SaveSystemSettings
 * 功能: 保存系统设置到Flash
 * 参数: sys - 系统状态结构指针
 * 返回: true=成功, false=失败
 */
bool Flash_SaveSystemSettings(SystemState_t *sys);

/**
 * 函数: Flash_LoadSystemSettings
 * 功能: 从Flash加载系统设置
 * 参数: sys - 系统状态结构指针
 * 返回: true=成功, false=失败或CRC错误
 */
bool Flash_LoadSystemSettings(SystemState_t *sys);

#endif /* FLASH_DRIVER_H */

