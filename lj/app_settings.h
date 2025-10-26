/****************************************************************************
 * 文件名: app_settings.h
 * 功能: 应用层设置菜单
 * 
 * 说明: 
 *   实现7项设置菜单的UI和逻辑
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include "../../Core/Inc/system_types.h"
#include "../../Drivers/Inc/key_driver.h"

/**
 * 函数: AppSettings_Init
 * 功能: 初始化设置模块
 */
void AppSettings_Init(SystemSettings_t *settings);

/**
 * 函数: AppSettings_Enter
 * 功能: 进入设置模式
 */
void AppSettings_Enter(SystemState_t *sys);

/**
 * 函数: AppSettings_Exit
 * 功能: 退出设置模式
 */
void AppSettings_Exit(SystemState_t *sys);

/**
 * 函数: AppSettings_HandleKey
 * 功能: 处理设置模式的按键
 */
void AppSettings_HandleKey(SystemState_t *sys, KeyValue_e key);

/**
 * 函数: AppSettings_Display
 * 功能: 显示设置界面
 */
void AppSettings_Display(const SystemState_t *sys);

#endif /* APP_SETTINGS_H */

