#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

/****************************************************************************
 * 图片数据声明
 ****************************************************************************/

/**
 * @brief 开机Logo图片数据
 * @note  尺寸：128x64像素，8页，每页128字节，共1024字节
 */
extern const uint8_t LOGO_STARTUP_IMAGE[1024];

/****************************************************************************
 * 显示模块类型定义
 ****************************************************************************/

/**
 * @brief 字体类型
 */
typedef enum {
    DISPLAY_FONT_6X12 = 0,    // 6x12字体 (小字体)
    DISPLAY_FONT_7X14 = 1,    // 7x14字体 (数字和字母)
    DISPLAY_FONT_8X16 = 2,    // 8x16字体 (ASCII)
    DISPLAY_FONT_16X32 = 3,   // 16x32字体 (大字体)
    DISPLAY_FONT_40X80 = 4    // 40x80字体 (超大字体)
} DisplayFontType_e;

/**
 * @brief 对齐方式
 */
typedef enum {
    DISPLAY_ALIGN_LEFT = 0,   // 左对齐
    DISPLAY_ALIGN_CENTER = 1, // 居中对齐
    DISPLAY_ALIGN_RIGHT = 2   // 右对齐
} DisplayAlignType_e;


/**
 * @brief 工作模式
 */
typedef enum {
    DISPLAY_WORK_MODE_INIT = 0,        // 初始化
    DISPLAY_WORK_MODE_STANDBY = 1,     // 待机
    DISPLAY_WORK_MODE_CONTINUOUS = 2,   // 连续模式
    DISPLAY_WORK_MODE_INTERMITTENT = 3, // 间歇模式
    DISPLAY_WORK_MODE_PAUSE = 4,        // 暂停
    DISPLAY_WORK_MODE_SETTINGS = 5,      // 设置
    DISPLAY_WORK_MODE_SELFTEST = 6,      // 自检
    DISPLAY_WORK_MODE_ERROR = 7,        // 错误
    DISPLAY_WORK_MODE_SHUTDOWN = 8       // 关机
} DisplayWorkMode_e;

/**
 * @brief 错误代码
 */
typedef enum {
    DISPLAY_ERROR_NONE = 0,           // 无错误
    DISPLAY_ERROR_LEAKAGE = 1,        // 泄漏
    DISPLAY_ERROR_BLOCKAGE = 2,       // 堵塞
    DISPLAY_ERROR_OVERPRESSURE = 3,    // 超压
    DISPLAY_ERROR_BATTERY_LOW = 4,     // 电池低
    DISPLAY_ERROR_BATTERY_CRITICAL = 5, // 电池严重不足
    DISPLAY_ERROR_LIQUID_FULL = 6,     // 液体满
    DISPLAY_ERROR_SENSOR = 7,          // 传感器错误
    DISPLAY_ERROR_PUMP = 8,            // 泵错误
    DISPLAY_ERROR_VALVE = 9            // 阀门错误
} DisplayErrorCode_e;

/**
 * @brief 显示事件类型
 */
typedef enum {
    DISPLAY_EVENT_CLEAR = 0,              // 清屏
    DISPLAY_EVENT_SHOW_STRING = 1,        // 显示字符串
    DISPLAY_EVENT_SHOW_NUMBER = 2,        // 显示数字
    DISPLAY_EVENT_SHOW_IMAGE = 3,         // 显示图像
    DISPLAY_EVENT_SET_POSITION = 4,       // 设置位置
    DISPLAY_EVENT_SET_BACKLIGHT = 5,      // 设置背光
    DISPLAY_EVENT_SHOW_PRESSURE = 6,      // 显示压力
    DISPLAY_EVENT_SHOW_WORK_MODE = 7,     // 显示工作模式
    DISPLAY_EVENT_SHOW_ERROR = 8,         // 显示错误
    DISPLAY_EVENT_SHOW_BATTERY_ICON = 9,  // 显示电池图标
    DISPLAY_EVENT_SHOW_STARTUP_INTERFACE = 10 // 显示开机界面
} DisplayEventType_e;

/****************************************************************************
 * 显示事件结构体（避免union）
 ****************************************************************************/

/**
 * @brief 显示事件结构体
 */
typedef struct {
    DisplayEventType_e type;  // 事件类型
    uint8_t x;                // X坐标
    uint8_t y;                // Y坐标
    
    // 根据事件类型使用不同的数据字段
    const char* str_data;     // 字符串数据
    uint16_t number_data;     // 数字数据
    const uint8_t* image_data; // 图像数据
    bool bool_data;           // 布尔数据
    uint8_t byte_data;        // 字节数据
    
    // 参数
    DisplayFontType_e font;   // 字体
    DisplayAlignType_e align; // 对齐方式
    uint8_t image_width;      // 图像宽度
    uint8_t image_height;     // 图像高度
    DisplayWorkMode_e work_mode; // 工作模式
    DisplayErrorCode_e error_code; // 错误代码
    bool is_charging;         // 是否充电
} DisplayEvent_t;

/****************************************************************************
 * 显示模块接口
 ****************************************************************************/

/**
 * @name      Display_Init
 * @brief     初始化显示模块
 * @param     无
 * @retval    无
 */
void Display_Init(void);

/**
 * @name      Display_Process
 * @brief     处理显示队列（每1ms调用一次）
 * @param     无
 * @retval    无
 */
void Display_Process(void);

/**
 * @name      Display_IsBusy
 * @brief     检查显示模块是否忙碌
 * @param     无
 * @retval    true-忙碌, false-空闲
 */
bool Display_IsBusy(void);

/**
 * @name      Display_Clear
 * @brief     清屏
 * @param     无
 * @retval    无
 */
void Display_Clear(void);

/**
 * @name      Display_SetBacklight
 * @brief     设置背光
 * @param     white_on - 白色背光是否开启
 * @param     yellow_on - 黄色背光是否开启
 * @retval    无
 */
void Display_SetBacklight(bool white_on, bool yellow_on);


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
void Display_ShowString(uint8_t x, uint8_t y, const char* str, DisplayFontType_e font, DisplayAlignType_e align);

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
void Display_ShowNumber(uint8_t x, uint8_t y, uint16_t number, DisplayFontType_e font, DisplayAlignType_e align);

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
 * @name      Display_ShowPressure
 * @brief     显示压力值
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     pressure - 压力值
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
void Display_ShowWorkMode(uint8_t x, uint8_t y, DisplayWorkMode_e mode);

/**
 * @name      Display_ShowError
 * @brief     显示错误信息
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     error - 错误代码
 * @retval    无
 */
void Display_ShowError(uint8_t x, uint8_t y, DisplayErrorCode_e error);

/**
 * @name      Display_ShowBatteryIcon
 * @brief     显示电池图标
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     battery_level - 电池电量(0-100)
 * @param     is_charging - 是否充电
 * @retval    无
 */
void Display_ShowBatteryIcon(uint8_t x, uint8_t y, uint8_t battery_level, bool is_charging);

/**
 * @name      Display_ShowStartupInterface
 * @brief     显示开机界面
 * @param     无
 * @retval    无
 */
void Display_ShowStartupInterface(void);

/**
 * @name      Display_ShowImageTest
 * @brief     显示图片测试界面
 * @param     无
 * @retval    无
 */
void Display_ShowImageTest(void);

/**
 * @name      Display_ShowChineseTest
 * @brief     显示中文测试界面
 * @param     无
 * @retval    无
 */
void Display_ShowChineseTest(void);

/**
 * @name      Display_ShowEnglishTest
 * @brief     显示英文测试界面
 * @param     无
 * @retval    无
 */
void Display_ShowEnglishTest(void);


#endif /* __DISPLAY_H__ */