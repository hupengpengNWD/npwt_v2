/**
  ******************************************************************************
  * @file:    app_ui.h
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   UI应用层头文件
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#ifndef __APP_UI_H
#define __APP_UI_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Middleware/Inc/display.h"

/****************************************************************************
 * UI测试模式定义
 ****************************************************************************/
typedef enum {
    UI_TEST_MODE_NORMAL = 0,      // 正常模式
    UI_TEST_MODE_ENGLISH,         // 英文测试模式
    UI_TEST_MODE_CHINESE,         // 中文测试模式
    UI_TEST_MODE_IMAGE,           // 图像测试模式
    UI_TEST_MODE_COUNT            // 测试模式总数
} UITestMode_e;

/****************************************************************************
 * UI状态定义
 ****************************************************************************/
typedef enum {
    UI_STATE_IDLE = 0,            // 空闲状态
    UI_STATE_TESTING,             // 测试状态
    UI_STATE_DISPLAYING           // 显示状态
} UIState_e;

/****************************************************************************
 * 函数声明
 ****************************************************************************/

/**
 * @name      AppUI_Init
 * @brief     初始化UI应用层
 * @param     无
 * @retval    无
 */
void AppUI_Init(void);

/**
 * @name      AppUI_Update
 * @brief     更新UI显示
 * @param     无
 * @retval    无
 */
void AppUI_Update(void);

/**
 * @name      AppUI_SetTestMode
 * @brief     设置测试模式
 * @param     mode - 测试模式
 * @retval    无
 */
void AppUI_SetTestMode(UITestMode_e mode);

/**
 * @name      AppUI_GetTestMode
 * @brief     获取当前测试模式
 * @param     无
 * @retval    当前测试模式
 */
UITestMode_e AppUI_GetTestMode(void);

/**
 * @name      AppUI_NextTestMode
 * @brief     切换到下一个测试模式
 * @param     无
 * @retval    无
 */
void AppUI_NextTestMode(void);

/**
 * @name      AppUI_PreviousTestMode
 * @brief     切换到上一个测试模式
 * @param     无
 * @retval    无
 */
void AppUI_PreviousTestMode(void);

/**
 * @name      AppUI_SetBacklight
 * @brief     设置背光
 * @param     enable - true开启，false关闭
 * @retval    无
 */
void AppUI_SetBacklight(bool enable);

/**
 * @name      AppUI_ShowSystemInfo
 * @brief     显示系统信息
 * @param     无
 * @retval    无
 */
void AppUI_ShowSystemInfo(void);

/**
 * @name      AppUI_ShowTestMenu
 * @brief     显示测试菜单
 * @param     无
 * @retval    无
 */
void AppUI_ShowTestMenu(void);

/**
 * @name      AppUI_HandleKeyPress
 * @brief     处理按键事件
 * @param     key_code - 按键代码
 * @retval    无
 */
void AppUI_HandleKeyPress(uint8_t key_code);

#endif /* __APP_UI_H */
