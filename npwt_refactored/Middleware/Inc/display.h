/**
  ******************************************************************************
  * @file:    display.h
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   显示驱动层头文件
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_enums.h"

/****************************************************************************
 * 字体类型定义
 ****************************************************************************/
typedef enum {
    FONT_TYPE_ASCII_8X16 = 0,    // ASCII 8x16字体
    FONT_TYPE_ASCII_6X12,        // ASCII 6x12字体
    FONT_TYPE_DIGIT_12X12,       // 数字 12x12字体
    FONT_TYPE_DIGIT_32X15,       // 大数字 32x15字体
    FONT_TYPE_CHINESE_16X16      // 中文 16x16字体
} FontType_e;

/****************************************************************************
 * 对齐方式定义
 ****************************************************************************/
typedef enum {
    ALIGN_LEFT = 0,     // 左对齐
    ALIGN_CENTER,       // 居中对齐
    ALIGN_RIGHT         // 右对齐
} AlignType_e;

/****************************************************************************
 * 显示状态机定义
 ****************************************************************************/
typedef enum {
    DISPLAY_STATE_IDLE = 0,              // 空闲状态
    DISPLAY_STATE_CLEARING,              // 清屏状态
    DISPLAY_STATE_SHOWING_PRESSURE,      // 显示压力
    DISPLAY_STATE_SHOWING_MODE,          // 显示模式
    DISPLAY_STATE_SHOWING_BATTERY,       // 显示电池
    DISPLAY_STATE_SHOWING_ERROR,         // 显示错误
    DISPLAY_STATE_SHOWING_STRING,        // 显示字符串
    DISPLAY_STATE_SHOWING_NUMBER,        // 显示数字
    DISPLAY_STATE_SHOWING_IMAGE,         // 显示图像
    DISPLAY_STATE_COMPLETE               // 完成状态
} DisplayState_e;

/****************************************************************************
 * 显示模式定义
 ****************************************************************************/
typedef enum {
    DISPLAY_MODE_NORMAL = 0,             // 正常显示模式
    DISPLAY_MODE_TEST_ENGLISH,          // 测试英文显示
    DISPLAY_MODE_TEST_CHINESE,          // 测试中文显示
    DISPLAY_MODE_TEST_IMAGE,            // 测试图像显示
    DISPLAY_MODE_ERROR,                 // 错误显示模式
    DISPLAY_MODE_BATTERY_LOW             // 电池低电量模式
} DisplayMode_e;

/**
 * @name      Display_Init
 * @brief     初始化显示驱动
 * @param     无
 * @retval    无
 */
void Display_Init(void);

/**
 * @name      Display_Clear
 * @brief     清空显示
 * @param     无
 * @retval    无
 */
void Display_Clear(void);

/**
 * @name      Display_SetBacklight
 * @brief     设置背光
 * @param     enable - true开启，false关闭
 * @retval    无
 */
void Display_SetBacklight(bool enable);

/**
 * @name      Display_SetMode
 * @brief     设置显示模式
 * @param     mode - 显示模式
 * @retval    无
 */
void Display_SetMode(DisplayMode_e mode);

/**
 * @name      Display_GetMode
 * @brief     获取当前显示模式
 * @param     无
 * @retval    当前显示模式
 */
DisplayMode_e Display_GetMode(void);

/**
 * @name      Display_ShowString
 * @brief     显示字符串
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     str - 字符串
 * @param     font - 字体类型
 * @param     align - 对齐方式
 * @retval    无
 */
void Display_ShowString(uint8_t x, uint8_t y, const char* str, FontType_e font, AlignType_e align);

/**
 * @name      Display_ShowNumber
 * @brief     显示数字
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     number - 数字
 * @param     font - 字体类型
 * @param     align - 对齐方式
 * @retval    无
 */
void Display_ShowNumber(uint8_t x, uint8_t y, int32_t number, FontType_e font, AlignType_e align);

/**
 * @name      Display_ShowImage
 * @brief     显示图像
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     width - 图像宽度
 * @param     height - 图像高度
 * @param     image_data - 图像数据
 * @retval    无
 */
void Display_ShowImage(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data);

/**
 * @name      Display_TestEnglish
 * @brief     测试英文显示
 * @param     无
 * @retval    无
 */
void Display_TestEnglish(void);

/**
 * @name      Display_TestChinese
 * @brief     测试中文显示
 * @param     无
 * @retval    无
 */
void Display_TestChinese(void);

/**
 * @name      Display_TestImage
 * @brief     测试图像显示
 * @param     无
 * @retval    无
 */
void Display_TestImage(void);

/**
 * @name      Display_Process
 * @brief     非阻塞显示处理函数（状态机）
 * @param     无
 * @retval    无
 */
void Display_Process(void);

/**
 * @name      Display_IsBusy
 * @brief     检查显示是否忙碌
 * @param     无
 * @retval    true-忙碌，false-空闲
 */
bool Display_IsBusy(void);

/****************************************************************************
 * 专业显示接口
 ****************************************************************************/

/**
 * @name      Display_ShowPressure
 * @brief     显示压力值（大数字）
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     pressure - 压力值（mmHg）
 * @param     show_unit - 是否显示单位
 * @retval    无
 */
void Display_ShowPressure(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit);

/**
 * @name      Display_ShowWorkMode
 * @brief     显示工作模式
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     mode - 工作模式
 * @retval    无
 */
void Display_ShowWorkMode(uint8_t x, uint8_t y, WorkMode_e mode);

/**
 * @name      Display_ShowBatteryIcon
 * @brief     显示电池图标
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     level - 电量等级（0-4）
 * @param     is_charging - 是否充电中
 * @retval    无
 */
void Display_ShowBatteryIcon(uint8_t x, uint8_t y, uint8_t level, bool is_charging);

/**
 * @name      Display_ShowError
 * @brief     显示错误信息
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     error - 错误代码
 * @retval    无
 */
void Display_ShowError(uint8_t x, uint8_t y, ErrorCode_e error);

/****************************************************************************
 * 界面模板接口
 ****************************************************************************/

/**
 * @name      Display_ShowMainInterface
 * @brief     显示主界面
 * @param     pressure - 当前压力
 * @param     mode - 工作模式
 * @param     battery_level - 电池电量
 * @retval    无
 */
void Display_ShowMainInterface(uint16_t pressure, WorkMode_e mode, uint8_t battery_level);

/**
 * @name      Display_ShowErrorInterface
 * @brief     显示错误界面
 * @param     error - 错误代码
 * @retval    无
 */
void Display_ShowErrorInterface(ErrorCode_e error);

/**
 * @name      Display_ShowBatteryLowInterface
 * @brief     显示电池低电量界面
 * @param     无
 * @retval    无
 */
void Display_ShowBatteryLowInterface(void);

/**
 * @name      Display_ShowStartupInterface
 * @brief     显示开机界面
 * @param     无
 * @retval    无
 */
void Display_ShowStartupInterface(void);

#endif /* __DISPLAY_H */
