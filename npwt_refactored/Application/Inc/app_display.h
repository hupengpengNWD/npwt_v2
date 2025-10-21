/****************************************************************************
 * 文件名: app_display.h
 * 功能: 应用层显示更新
 * 
 * 说明: 
 *   根据系统状态更新LCD显示
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef APP_DISPLAY_H
#define APP_DISPLAY_H

#include "../../Core/Inc/system_types.h"

/**
 * 函数: AppDisplay_Update
 * 功能: 更新LCD显示（根据当前模式和状态）
 * 参数: sys - 系统状态指针
 */
void AppDisplay_Update(SystemState_t *sys);

#endif /* APP_DISPLAY_H */

