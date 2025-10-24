/**
 * @file    app_ui.h
 * @brief   应用层UI模块头文件
 * @date    2025-01-27
 * 
 * 应用层UI模块，负责调用display模块的接口实现显示功能
 */

#ifndef __APP_UI_H__
#define __APP_UI_H__

/****************************************************************************
 * 包含文件
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 类型定义
 ****************************************************************************/

/**
 * @brief UI状态
 */
typedef enum {
    UI_STATE_NORMAL = 0,    // 正常状态
    UI_STATE_TESTING = 1    // 测试状态
} UIState_e;

/****************************************************************************
 * 公共接口声明
 ****************************************************************************/

/**
 * @name      AppUI_Init
 * @brief     初始化UI模块
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
 * @name      AppUI_ShowNormalInterface
 * @brief     显示正常界面
 * @param     无
 * @retval    无
 */
void AppUI_ShowNormalInterface(void);

/**
 * @name      AppUI_ShowImageTest
 * @brief     显示图片测试
 * @param     无
 * @retval    无
 */
void AppUI_ShowImageTest(void);

/**
 * @name      AppUI_ShowChineseTest
 * @brief     显示中文测试
 * @param     无
 * @retval    无
 */
void AppUI_ShowChineseTest(void);

/**
 * @name      AppUI_ShowEnglishTest
 * @brief     显示英文测试
 * @param     无
 * @retval    无
 */
void AppUI_ShowEnglishTest(void);

#endif /* __APP_UI_H__ */