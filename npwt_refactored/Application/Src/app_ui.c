/**
  ******************************************************************************
  * @file:    app_ui.c
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   UI应用层实现
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#include "app_ui.h"
#include "../../Middleware/Inc/display.h"
#include <stddef.h>

/****************************************************************************
 * 全局变量定义
 ****************************************************************************/
static UITestMode_e g_ui_test_mode = UI_TEST_MODE_NORMAL;
static UIState_e g_ui_state = UI_STATE_IDLE;
static bool g_backlight_enabled = true;

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/
static void AppUI_UpdateDisplay(void);
static void AppUI_ShowModeInfo(void);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      AppUI_Init
 * @brief     初始化UI应用层
 * @param     无
 * @retval    无
 */
void AppUI_Init(void)
{
    // 初始化显示驱动
    Display_Init();
    
    // 设置默认状态
    g_ui_test_mode = UI_TEST_MODE_NORMAL;
    g_ui_state = UI_STATE_IDLE;
    g_backlight_enabled = true;
    
    // 显示初始界面
    AppUI_ShowSystemInfo();
}

/**
 * @name      AppUI_Update
 * @brief     更新UI显示
 * @param     无
 * @retval    无
 */
void AppUI_Update(void)
{
    AppUI_UpdateDisplay();
}

/**
 * @name      AppUI_UpdateDisplay
 * @brief     更新显示内容
 * @param     无
 * @retval    无
 */
static void AppUI_UpdateDisplay(void)
{
    switch (g_ui_test_mode) {
        case UI_TEST_MODE_NORMAL:
            AppUI_ShowSystemInfo();
            break;
        case UI_TEST_MODE_ENGLISH:
            Display_SetMode(DISPLAY_MODE_TEST_ENGLISH);
            Display_Process();
            break;
        case UI_TEST_MODE_CHINESE:
            Display_SetMode(DISPLAY_MODE_TEST_CHINESE);
            Display_Process();
            break;
        case UI_TEST_MODE_IMAGE:
            Display_SetMode(DISPLAY_MODE_TEST_IMAGE);
            Display_Process();
            break;
        default:
            AppUI_ShowSystemInfo();
            break;
    }
}

/**
 * @name      AppUI_SetTestMode
 * @brief     设置测试模式
 * @param     mode - 测试模式
 * @retval    无
 */
void AppUI_SetTestMode(UITestMode_e mode)
{
    if (mode < UI_TEST_MODE_COUNT) {
        g_ui_test_mode = mode;
        g_ui_state = UI_STATE_TESTING;
        AppUI_UpdateDisplay();
    }
}

/**
 * @name      AppUI_GetTestMode
 * @brief     获取当前测试模式
 * @param     无
 * @retval    当前测试模式
 */
UITestMode_e AppUI_GetTestMode(void)
{
    return g_ui_test_mode;
}

/**
 * @name      AppUI_NextTestMode
 * @brief     切换到下一个测试模式
 * @param     无
 * @retval    无
 */
void AppUI_NextTestMode(void)
{
    g_ui_test_mode++;
    if (g_ui_test_mode >= UI_TEST_MODE_COUNT) {
        g_ui_test_mode = UI_TEST_MODE_NORMAL;
    }
    
    g_ui_state = UI_STATE_TESTING;
    AppUI_UpdateDisplay();
}

/**
 * @name      AppUI_PreviousTestMode
 * @brief     切换到上一个测试模式
 * @param     无
 * @retval    无
 */
void AppUI_PreviousTestMode(void)
{
    if (g_ui_test_mode == UI_TEST_MODE_NORMAL) {
        g_ui_test_mode = UI_TEST_MODE_IMAGE;
    } else {
        g_ui_test_mode--;
    }
    
    g_ui_state = UI_STATE_TESTING;
    AppUI_UpdateDisplay();
}

/**
 * @name      AppUI_SetBacklight
 * @brief     设置背光
 * @param     enable - true开启，false关闭
 * @retval    无
 */
void AppUI_SetBacklight(bool enable)
{
    g_backlight_enabled = enable;
    Display_SetBacklight(enable);
}

/**
 * @name      AppUI_ShowSystemInfo
 * @brief     显示系统信息
 * @param     无
 * @retval    无
 */
void AppUI_ShowSystemInfo(void)
{
    Display_Clear();
    
    // 显示系统标题
    Display_ShowString(64, 0, "NPWT System", FONT_TYPE_ASCII_8X16, ALIGN_CENTER);
    
    // 显示当前模式
    AppUI_ShowModeInfo();
    
    // 显示操作提示
    Display_ShowString(64, 32, "Press Up/Down", FONT_TYPE_ASCII_6X12, ALIGN_CENTER);
    Display_ShowString(64, 44, "to Test Display", FONT_TYPE_ASCII_6X12, ALIGN_CENTER);
    
    // 显示背光状态
    if (g_backlight_enabled) {
        Display_ShowString(64, 56, "Backlight: ON", FONT_TYPE_ASCII_6X12, ALIGN_CENTER);
    } else {
        Display_ShowString(64, 56, "Backlight: OFF", FONT_TYPE_ASCII_6X12, ALIGN_CENTER);
    }
}

/**
 * @name      AppUI_ShowModeInfo
 * @brief     显示模式信息
 * @param     无
 * @retval    无
 */
static void AppUI_ShowModeInfo(void)
{
    const char* mode_names[] = {
        "Normal Mode",
        "English Test",
        "Chinese Test", 
        "Image Test"
    };
    
    if (g_ui_test_mode < UI_TEST_MODE_COUNT) {
        Display_ShowString(64, 16, mode_names[g_ui_test_mode], FONT_TYPE_ASCII_6X12, ALIGN_CENTER);
    }
}

/**
 * @name      AppUI_ShowTestMenu
 * @brief     显示测试菜单
 * @param     无
 * @retval    无
 */
void AppUI_ShowTestMenu(void)
{
    Display_Clear();
    
    Display_ShowString(64, 0, "Test Menu", FONT_TYPE_ASCII_8X16, ALIGN_CENTER);
    Display_ShowString(0, 16, "1. English Test", FONT_TYPE_ASCII_6X12, ALIGN_LEFT);
    Display_ShowString(0, 28, "2. Chinese Test", FONT_TYPE_ASCII_6X12, ALIGN_LEFT);
    Display_ShowString(0, 40, "3. Image Test", FONT_TYPE_ASCII_6X12, ALIGN_LEFT);
    Display_ShowString(0, 52, "4. Normal Mode", FONT_TYPE_ASCII_6X12, ALIGN_LEFT);
}

/**
 * @name      AppUI_HandleKeyPress
 * @brief     处理按键事件
 * @param     key_code - 按键代码
 * @retval    无
 */
void AppUI_HandleKeyPress(uint8_t key_code)
{
    switch (key_code) {
        case 1: // Up键
            AppUI_NextTestMode();
            break;
        case 2: // Down键
            AppUI_PreviousTestMode();
            break;
        case 3: // Cancel键
            AppUI_SetTestMode(UI_TEST_MODE_NORMAL);
            break;
        case 4: // Power键
            // 切换背光
            AppUI_SetBacklight(!g_backlight_enabled);
            break;
        default:
            break;
    }
}
