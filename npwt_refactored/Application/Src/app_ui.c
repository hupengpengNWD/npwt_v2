/**
 * @file    app_ui.c
 * @brief   应用层UI模块实现
 * @date    2025-01-27
 * 
 * 应用层UI模块，负责调用display模块的接口实现显示功能
 */

#include "app_ui.h"
#include "display.h"

/****************************************************************************
 * 私有变量
 ****************************************************************************/

// UI状态
static UIState_e g_ui_state = UI_STATE_NORMAL;

/****************************************************************************
 * 公共接口实现
 ****************************************************************************/

/**
 * @name      AppUI_Init
 * @brief     初始化UI模块
 * @param     无
 * @retval    无
 */
void AppUI_Init(void)
{
    g_ui_state = UI_STATE_NORMAL;
}

/**
 * @name      AppUI_Update
 * @brief     更新UI显示
 * @param     无
 * @retval    无
 */
void AppUI_Update(void)
{
    // 根据当前状态更新显示
    switch (g_ui_state) {
        case UI_STATE_NORMAL:
            AppUI_ShowNormalInterface();
            break;
        default:
            break;
    }
}

/**
 * @name      AppUI_ShowNormalInterface
 * @brief     显示正常界面
 * @param     无
 * @retval    无
 */
void AppUI_ShowNormalInterface(void)
{
    // 清屏
    Display_Clear();
    
    // 显示系统信息
    Display_ShowString(0, 0, "NPWT SYSTEM", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowString(0, 12, "READY", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      AppUI_ShowImageTest
 * @brief     显示图片测试
 * @param     无
 * @retval    无
 */
void AppUI_ShowImageTest(void)
{
    // 调用display模块的图片测试接口
    Display_ShowImageTest();
}

/**
 * @name      AppUI_ShowChineseTest
 * @brief     显示中文测试
 * @param     无
 * @retval    无
 */
void AppUI_ShowChineseTest(void)
{
    // 调用display模块的中文测试接口
    Display_ShowChineseTest();
}

/**
 * @name      AppUI_ShowEnglishTest
 * @brief     显示英文测试
 * @param     无
 * @retval    无
 */
void AppUI_ShowEnglishTest(void)
{
    // 调用display模块的英文测试接口
    Display_ShowEnglishTest();
}