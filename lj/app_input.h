/****************************************************************************
 * 文件名: app_input.h
 * 功能: 应用层输入处理
 * 
 * 说明: 
 *   封装按键输入处理，包含按键音
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef APP_INPUT_H
#define APP_INPUT_H

#include "../../Core/Inc/system_types.h"
#include "../../Drivers/Inc/key_driver.h"

/**
 * 函数: AppInput_Process
 * 功能: 处理按键输入（包含按键音）
 * 参数: sys - 系统状态指针
 *       key_data - 按键数据
 */
void AppInput_Process(SystemState_t *sys, KeyData_t *key_data);

#endif /* APP_INPUT_H */

