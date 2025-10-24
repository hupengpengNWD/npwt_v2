/**
  ******************************************************************************
  * @file:    display.c
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   显示驱动层实现 - 非阻塞状态机架构
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#include "display.h"
#include "../../HAL/Inc/hal_lcd.h"
#include <stddef.h>
#include <string.h>

/****************************************************************************
 * 外部字模数组声明（从原始代码移植）
 ****************************************************************************/
extern const unsigned char arry_dig[];      // 数字字模 12x12
extern const unsigned char arry_dig22[];    // 大数字字模 32x15
extern const unsigned char arry_char[];     // 字符字模 6x12
extern const unsigned char ASCII[];        // ASCII字模 8x16

/****************************************************************************
 * 显示上下文结构体
 ****************************************************************************/
typedef struct {
    DisplayState_e state;                    // 当前状态
    DisplayMode_e mode;                      // 显示模式
    bool is_busy;                           // 是否忙碌
    bool needs_update;                      // 是否需要更新
    
    // 待显示的数据
    uint8_t pending_x, pending_y;           // 待显示位置
    uint16_t pending_pressure;              // 待显示压力
    DisplayWorkMode_e pending_mode;         // 待显示模式
    uint8_t pending_battery_level;          // 待显示电池电量
    bool pending_battery_charging;          // 待显示电池充电状态
    DisplayErrorCode_e pending_error;       // 待显示错误
    const char* pending_string;              // 待显示字符串
    int32_t pending_number;                 // 待显示数字
    FontType_e pending_font;                 // 待显示字体
    AlignType_e pending_align;               // 待显示对齐方式
    const uint8_t* pending_image_data;      // 待显示图像数据
    uint8_t pending_image_width;            // 待显示图像宽度
    uint8_t pending_image_height;           // 待显示图像高度
    
    // 显示进度
    uint8_t current_page;                   // 当前页
    uint8_t current_column;                 // 当前列
    uint8_t string_index;                   // 字符串索引
    uint8_t image_page;                     // 图像页索引
    uint8_t image_column;                   // 图像列索引
} DisplayContext_t;

/****************************************************************************
 * 全局变量定义
 ****************************************************************************/
static DisplayContext_t g_display_context = {0};

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/
static void Display_StateMachine(void);
static void Display_ShowPressureInternal(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit);
static void Display_ShowWorkModeInternal(uint8_t x, uint8_t y, DisplayWorkMode_e mode);
static void Display_ShowBatteryIconInternal(uint8_t x, uint8_t y, uint8_t level, bool is_charging);
static void Display_ShowErrorInternal(uint8_t x, uint8_t y, DisplayErrorCode_e error);
static void Display_ShowStringInternal(uint8_t x, uint8_t y, const char* str, FontType_e font, AlignType_e align);
static void Display_ShowNumberInternal(uint8_t x, uint8_t y, int32_t number, FontType_e font, AlignType_e align);
static void Display_ShowImageInternal(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data);
static void Display_DrawChar(uint8_t x, uint8_t y, char ch, FontType_e font);
static void Display_DrawChinese(uint8_t x, uint8_t y, const uint8_t* chinese_data);
static uint8_t Display_GetFontWidth(FontType_e font);
static uint8_t Display_GetFontHeight(FontType_e font);
static void Display_ConvertNumberToString(int32_t number, char* str, uint8_t max_len);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      Display_Init
 * @brief     初始化显示驱动
 * @param     无
 * @retval    无
 */
void Display_Init(void)
{
    // 初始化HAL层LCD
    HAL_LCD_Init();
    
    // 初始化显示上下文
    memset(&g_display_context, 0, sizeof(DisplayContext_t));
    g_display_context.state = DISPLAY_STATE_IDLE;
    g_display_context.mode = DISPLAY_MODE_NORMAL;
    g_display_context.is_busy = false;
    g_display_context.needs_update = false;
    
    // 清屏
    Display_Clear();
}

/**
 * @name      Display_Clear
 * @brief     清空显示
 * @param     无
 * @retval    无
 */
void Display_Clear(void)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.state = DISPLAY_STATE_CLEARING;
    g_display_context.is_busy = true;
    HAL_LCD_ClearNonBlocking();
}

/**
 * @name      Display_SetBacklight
 * @brief     设置背光
 * @param     enable - true开启，false关闭
 * @retval    无
 */
void Display_SetBacklight(bool enable)
{
    HAL_LCD_SetBacklight(enable);
}

/**
 * @name      Display_SetMode
 * @brief     设置显示模式
 * @param     mode - 显示模式
 * @retval    无
 */
void Display_SetMode(DisplayMode_e mode)
{
    g_display_context.mode = mode;
    g_display_context.needs_update = true;
}

/**
 * @name      Display_GetMode
 * @brief     获取当前显示模式
 * @param     无
 * @retval    当前显示模式
 */
DisplayMode_e Display_GetMode(void)
{
    return g_display_context.mode;
}

/**
 * @name      Display_Process
 * @brief     非阻塞显示处理函数（状态机）
 * @param     无
 * @retval    无
 */
void Display_Process(void)
{
    // 如果HAL层忙碌，直接返回（HAL_LCD_Process已在Timer3中断中调用）
    if (HAL_LCD_IsBusy()) {
        return;
    }
    
    // 处理显示状态机
    Display_StateMachine();
}

/**
 * @name      Display_IsBusy
 * @brief     检查显示是否忙碌
 * @param     无
 * @retval    true-忙碌，false-空闲
 */
bool Display_IsBusy(void)
{
    return g_display_context.is_busy || HAL_LCD_IsBusy();
}

/****************************************************************************
 * 专业显示接口实现
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
void Display_ShowPressure(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.pending_x = x;
    g_display_context.pending_y = y;
    g_display_context.pending_pressure = pressure;
    g_display_context.state = DISPLAY_STATE_SHOWING_PRESSURE;
    g_display_context.is_busy = true;
    g_display_context.current_page = 0;
    g_display_context.current_column = 0;
}

/**
 * @name      Display_ShowWorkMode
 * @brief     显示工作模式
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     mode - 工作模式
 * @retval    无
 */
void Display_ShowWorkMode(uint8_t x, uint8_t y, DisplayWorkMode_e mode)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.pending_x = x;
    g_display_context.pending_y = y;
    g_display_context.pending_mode = mode;
    g_display_context.state = DISPLAY_STATE_SHOWING_MODE;
    g_display_context.is_busy = true;
}

/**
 * @name      Display_ShowBatteryIcon
 * @brief     显示电池图标
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     level - 电量等级（0-4）
 * @param     is_charging - 是否充电中
 * @retval    无
 */
void Display_ShowBatteryIcon(uint8_t x, uint8_t y, uint8_t level, bool is_charging)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.pending_x = x;
    g_display_context.pending_y = y;
    g_display_context.pending_battery_level = level;
    g_display_context.pending_battery_charging = is_charging;
    g_display_context.state = DISPLAY_STATE_SHOWING_BATTERY;
    g_display_context.is_busy = true;
}

/**
 * @name      Display_ShowError
 * @brief     显示错误信息
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     error - 错误代码
 * @retval    无
 */
void Display_ShowError(uint8_t x, uint8_t y, DisplayErrorCode_e error)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.pending_x = x;
    g_display_context.pending_y = y;
    g_display_context.pending_error = error;
    g_display_context.state = DISPLAY_STATE_SHOWING_ERROR;
    g_display_context.is_busy = true;
}

/****************************************************************************
 * 界面模板接口实现
 ****************************************************************************/

/**
 * @name      Display_ShowMainInterface
 * @brief     显示主界面
 * @param     pressure - 当前压力
 * @param     mode - 工作模式
 * @param     battery_level - 电池电量
 * @retval    无
 */
void Display_ShowMainInterface(uint16_t pressure, DisplayWorkMode_e mode, uint8_t battery_level)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    // 先清屏
    Display_Clear();
    
    // 设置待显示数据
    g_display_context.pending_pressure = pressure;
    g_display_context.pending_mode = mode;
    g_display_context.pending_battery_level = battery_level;
    g_display_context.pending_battery_charging = false;
    
    // 状态机会依次显示各个组件
    g_display_context.state = DISPLAY_STATE_CLEARING;
    g_display_context.is_busy = true;
}

/**
 * @name      Display_ShowErrorInterface
 * @brief     显示错误界面
 * @param     error - 错误代码
 * @retval    无
 */
void Display_ShowErrorInterface(DisplayErrorCode_e error)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    // 先清屏
    Display_Clear();
    
    // 设置错误信息
    g_display_context.pending_error = error;
    g_display_context.state = DISPLAY_STATE_CLEARING;
    g_display_context.is_busy = true;
}

/**
 * @name      Display_ShowBatteryLowInterface
 * @brief     显示电池低电量界面
 * @param     无
 * @retval    无
 */
void Display_ShowBatteryLowInterface(void)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    // 先清屏
    Display_Clear();
    
    // 显示电池低电量信息
    g_display_context.pending_string = "Battery Low";
    g_display_context.pending_font = FONT_TYPE_ASCII_8X16;
    g_display_context.pending_align = ALIGN_CENTER;
    g_display_context.state = DISPLAY_STATE_CLEARING;
    g_display_context.is_busy = true;
}

/**
 * @name      Display_ShowStartupInterface
 * @brief     显示开机界面
 * @param     无
 * @retval    无
 */
void Display_ShowStartupInterface(void)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    // 先清屏
    Display_Clear();
    
    // 显示开机Logo
    g_display_context.pending_string = "NPWT System";
    g_display_context.pending_font = FONT_TYPE_ASCII_8X16;
    g_display_context.pending_align = ALIGN_CENTER;
    g_display_context.state = DISPLAY_STATE_CLEARING;
    g_display_context.is_busy = true;
}

/****************************************************************************
 * 基础显示接口实现（保持兼容性）
 ****************************************************************************/

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
void Display_ShowString(uint8_t x, uint8_t y, const char* str, FontType_e font, AlignType_e align)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.pending_x = x;
    g_display_context.pending_y = y;
    g_display_context.pending_string = str;
    g_display_context.pending_font = font;
    g_display_context.pending_align = align;
    g_display_context.state = DISPLAY_STATE_SHOWING_STRING;
    g_display_context.is_busy = true;
    g_display_context.string_index = 0;
}

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
void Display_ShowNumber(uint8_t x, uint8_t y, int32_t number, FontType_e font, AlignType_e align)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.pending_x = x;
    g_display_context.pending_y = y;
    g_display_context.pending_number = number;
    g_display_context.pending_font = font;
    g_display_context.pending_align = align;
    g_display_context.state = DISPLAY_STATE_SHOWING_NUMBER;
    g_display_context.is_busy = true;
}

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
void Display_ShowImage(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data)
{
    if (g_display_context.is_busy) {
        return; // 如果忙碌，忽略请求
    }
    
    g_display_context.pending_x = x;
    g_display_context.pending_y = y;
    g_display_context.pending_image_width = width;
    g_display_context.pending_image_height = height;
    g_display_context.pending_image_data = image_data;
    g_display_context.state = DISPLAY_STATE_SHOWING_IMAGE;
    g_display_context.is_busy = true;
    g_display_context.image_page = 0;
    g_display_context.image_column = 0;
}

/****************************************************************************
 * 测试函数实现
 ****************************************************************************/

/**
 * @name      Display_TestEnglish
 * @brief     测试英文显示
 * @param     无
 * @retval    无
 */
void Display_TestEnglish(void)
{
    Display_ShowString(0, 0, "Hello World!", FONT_TYPE_ASCII_8X16, ALIGN_LEFT);
}

/**
 * @name      Display_TestChinese
 * @brief     测试中文显示
 * @param     无
 * @retval    无
 */
void Display_TestChinese(void)
{
    // 暂时用英文代替中文测试
    Display_ShowString(0, 0, "Chinese Test", FONT_TYPE_ASCII_8X16, ALIGN_LEFT);
}

/**
 * @name      Display_TestImage
 * @brief     测试图像显示
 * @param     无
 * @retval    无
 */
void Display_TestImage(void)
{
    // 简单的测试图像（8x8像素）
    static const uint8_t test_image[] = {
        0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF  // 8x8边框
    };
    Display_ShowImage(60, 28, 8, 8, test_image);
}

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * @name      Display_StateMachine
 * @brief     显示状态机处理
 * @param     无
 * @retval    无
 */
static void Display_StateMachine(void)
{
    switch (g_display_context.state) {
        case DISPLAY_STATE_IDLE:
            // 空闲状态，检查是否需要更新
            if (g_display_context.needs_update) {
                g_display_context.needs_update = false;
                g_display_context.state = DISPLAY_STATE_CLEARING;
                HAL_LCD_ClearNonBlocking();
            }
            break;
            
        case DISPLAY_STATE_CLEARING:
            // 清屏完成，检查是否有待显示的内容
            if (!HAL_LCD_IsBusy()) {
                // 根据模式决定下一步
                switch (g_display_context.mode) {
                    case DISPLAY_MODE_NORMAL:
                        if (g_display_context.pending_pressure > 0) {
                            g_display_context.state = DISPLAY_STATE_SHOWING_PRESSURE;
                        } else {
                            g_display_context.state = DISPLAY_STATE_COMPLETE;
                        }
                        break;
                        
                    case DISPLAY_MODE_ERROR:
                        g_display_context.state = DISPLAY_STATE_SHOWING_ERROR;
                        break;
                        
                    case DISPLAY_MODE_BATTERY_LOW:
                        g_display_context.state = DISPLAY_STATE_SHOWING_STRING;
                        break;
                        
                    default:
                        g_display_context.state = DISPLAY_STATE_COMPLETE;
                        break;
                }
            }
            break;
            
        case DISPLAY_STATE_SHOWING_PRESSURE:
            Display_ShowPressureInternal(g_display_context.pending_x, g_display_context.pending_y, 
                                        g_display_context.pending_pressure, true);
            break;
            
        case DISPLAY_STATE_SHOWING_MODE:
            Display_ShowWorkModeInternal(g_display_context.pending_x, g_display_context.pending_y, 
                                        g_display_context.pending_mode);
            break;
            
        case DISPLAY_STATE_SHOWING_BATTERY:
            Display_ShowBatteryIconInternal(g_display_context.pending_x, g_display_context.pending_y, 
                                           g_display_context.pending_battery_level, 
                                           g_display_context.pending_battery_charging);
            break;
            
        case DISPLAY_STATE_SHOWING_ERROR:
            Display_ShowErrorInternal(g_display_context.pending_x, g_display_context.pending_y, 
                                     g_display_context.pending_error);
            break;
            
        case DISPLAY_STATE_SHOWING_STRING:
            Display_ShowStringInternal(g_display_context.pending_x, g_display_context.pending_y, 
                                      g_display_context.pending_string, g_display_context.pending_font, 
                                      g_display_context.pending_align);
            break;
            
        case DISPLAY_STATE_SHOWING_NUMBER:
            Display_ShowNumberInternal(g_display_context.pending_x, g_display_context.pending_y, 
                                      g_display_context.pending_number, g_display_context.pending_font, 
                                      g_display_context.pending_align);
            break;
            
        case DISPLAY_STATE_SHOWING_IMAGE:
            Display_ShowImageInternal(g_display_context.pending_x, g_display_context.pending_y, 
                                     g_display_context.pending_image_width, g_display_context.pending_image_height, 
                                     g_display_context.pending_image_data);
            break;
            
        case DISPLAY_STATE_COMPLETE:
            g_display_context.is_busy = false;
            g_display_context.state = DISPLAY_STATE_IDLE;
            break;
            
        default:
            g_display_context.is_busy = false;
            g_display_context.state = DISPLAY_STATE_IDLE;
            break;
    }
}

/**
 * @name      Display_ShowPressureInternal
 * @brief     内部压力显示函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     pressure - 压力值
 * @param     show_unit - 是否显示单位
 * @retval    无
 */
static void Display_ShowPressureInternal(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit)
{
    // 简化实现：显示压力数字
    char pressure_str[16];
    Display_ConvertNumberToString(pressure, pressure_str, sizeof(pressure_str));
    
    // 使用大字体显示压力
    Display_ShowStringInternal(x, y, pressure_str, FONT_TYPE_DIGIT_32X15, ALIGN_CENTER);
    
    if (show_unit) {
        Display_ShowStringInternal(x + 40, y + 20, "mmHg", FONT_TYPE_ASCII_8X16, ALIGN_LEFT);
    }
    
    g_display_context.state = DISPLAY_STATE_COMPLETE;
}

/**
 * @name      Display_ShowWorkModeInternal
 * @brief     内部工作模式显示函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     mode - 工作模式
 * @retval    无
 */
static void Display_ShowWorkModeInternal(uint8_t x, uint8_t y, DisplayWorkMode_e mode)
{
    const char* mode_str = "Unknown";
    
    switch (mode) {
        case DISPLAY_WORK_MODE_STANDBY:
            mode_str = "Standby";
            break;
        case DISPLAY_WORK_MODE_CONTINUOUS:
            mode_str = "Continuous";
            break;
        case DISPLAY_WORK_MODE_INTERMITTENT:
            mode_str = "Intermittent";
            break;
        case DISPLAY_WORK_MODE_PAUSE:
            mode_str = "Pause";
            break;
        case DISPLAY_WORK_MODE_ERROR:
            mode_str = "Error";
            break;
        default:
            mode_str = "Unknown";
            break;
    }
    
    Display_ShowStringInternal(x, y, mode_str, FONT_TYPE_ASCII_8X16, ALIGN_LEFT);
    g_display_context.state = DISPLAY_STATE_COMPLETE;
}

/**
 * @name      Display_ShowBatteryIconInternal
 * @brief     内部电池图标显示函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     level - 电量等级
 * @param     is_charging - 是否充电中
 * @retval    无
 */
static void Display_ShowBatteryIconInternal(uint8_t x, uint8_t y, uint8_t level, bool is_charging)
{
    // 简化实现：显示电池电量百分比
    char battery_str[16];
    Display_ConvertNumberToString(level * 25, battery_str, sizeof(battery_str)); // 0-4 -> 0-100%
    
    Display_ShowStringInternal(x, y, battery_str, FONT_TYPE_ASCII_8X16, ALIGN_RIGHT);
    
    if (is_charging) {
        Display_ShowStringInternal(x + 20, y, "+", FONT_TYPE_ASCII_8X16, ALIGN_LEFT);
    }
    
    g_display_context.state = DISPLAY_STATE_COMPLETE;
}

/**
 * @name      Display_ShowErrorInternal
 * @brief     内部错误显示函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     error - 错误代码
 * @retval    无
 */
static void Display_ShowErrorInternal(uint8_t x, uint8_t y, DisplayErrorCode_e error)
{
    const char* error_str = "Unknown Error";
    
    switch (error) {
        case DISPLAY_ERROR_NONE:
            error_str = "No Error";
            break;
        case DISPLAY_ERROR_OVERPRESSURE:
            error_str = "Over Pressure";
            break;
        case DISPLAY_ERROR_LEAKAGE:
            error_str = "Leakage";
            break;
        case DISPLAY_ERROR_BATTERY_LOW:
            error_str = "Battery Low";
            break;
        case DISPLAY_ERROR_SENSOR:
            error_str = "Sensor Fault";
            break;
        case DISPLAY_ERROR_PUMP:
            error_str = "Pump Fault";
            break;
        case DISPLAY_ERROR_VALVE:
            error_str = "Valve Fault";
            break;
        default:
            error_str = "Unknown Error";
            break;
    }
    
    Display_ShowStringInternal(x, y, error_str, FONT_TYPE_ASCII_8X16, ALIGN_CENTER);
    g_display_context.state = DISPLAY_STATE_COMPLETE;
}

/**
 * @name      Display_ShowStringInternal
 * @brief     内部字符串显示函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     str - 字符串
 * @param     font - 字体类型
 * @param     align - 对齐方式
 * @retval    无
 */
static void Display_ShowStringInternal(uint8_t x, uint8_t y, const char* str, FontType_e font, AlignType_e align)
{
    if (str == NULL) {
        g_display_context.state = DISPLAY_STATE_COMPLETE;
        return;
    }
    
    uint8_t font_width = Display_GetFontWidth(font);
    uint8_t font_height = Display_GetFontHeight(font);
    uint8_t current_x = x;
    uint8_t current_y = y;
    
    // 处理对齐方式
    if (align == ALIGN_CENTER) {
        uint8_t str_len = (uint8_t)strlen(str);
        current_x = (128 - str_len * font_width) / 2;
    } else if (align == ALIGN_RIGHT) {
        uint8_t str_len = (uint8_t)strlen(str);
        current_x = 128 - str_len * font_width;
    }
    
    // 逐个字符显示
    while (*str && g_display_context.string_index < strlen(g_display_context.pending_string)) {
        Display_DrawChar(current_x, current_y, *str, font);
        current_x += font_width;
        str++;
        g_display_context.string_index++;
        
        // 检查是否需要换行
        if (current_x + font_width > 128) {
            current_x = x;
            current_y += font_height;
        }
    }
    
    g_display_context.state = DISPLAY_STATE_COMPLETE;
}

/**
 * @name      Display_ShowNumberInternal
 * @brief     内部数字显示函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     number - 数字
 * @param     font - 字体类型
 * @param     align - 对齐方式
 * @retval    无
 */
static void Display_ShowNumberInternal(uint8_t x, uint8_t y, int32_t number, FontType_e font, AlignType_e align)
{
    char number_str[16];
    Display_ConvertNumberToString(number, number_str, sizeof(number_str));
    
    Display_ShowStringInternal(x, y, number_str, font, align);
}

/**
 * @name      Display_ShowImageInternal
 * @brief     内部图像显示函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     width - 图像宽度
 * @param     height - 图像高度
 * @param     image_data - 图像数据
 * @retval    无
 */
static void Display_ShowImageInternal(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data)
{
    if (image_data == NULL) {
        g_display_context.state = DISPLAY_STATE_COMPLETE;
        return;
    }
    
    uint8_t pages = (height + 7) / 8; // 计算需要的页数
    
    for (uint8_t page = 0; page < pages; page++) {
        HAL_LCD_SetPositionNonBlocking(page, x);
        
        // 检查LCD是否空闲（HAL_LCD_Process已在Timer3中断中调用）
        if (HAL_LCD_IsBusy()) {
            return; // 如果忙碌，等待下次中断处理
        }
        
        for (uint8_t col = 0; col < width; col++) {
            HAL_LCD_SendDataNonBlocking(image_data[page * width + col]);
            
            // 等待LCD空闲
            while (HAL_LCD_IsBusy()) {
                HAL_LCD_Process();
            }
        }
    }
    
    g_display_context.state = DISPLAY_STATE_COMPLETE;
}

/**
 * @name      Display_DrawChar
 * @brief     绘制字符
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     ch - 字符
 * @param     font - 字体类型
 * @retval    无
 */
static void Display_DrawChar(uint8_t x, uint8_t y, char ch, FontType_e font)
{
    uint8_t font_width = Display_GetFontWidth(font);
    uint8_t font_height = Display_GetFontHeight(font);
    uint8_t page = y / 8;
    uint8_t page_offset = y % 8;
    
    // 设置显示位置（非阻塞）
    HAL_LCD_SetPositionNonBlocking(page, x);
    
    // 等待LCD空闲
    while (HAL_LCD_IsBusy()) {
        HAL_LCD_Process();
    }
    
    // 根据字体类型选择字模数据
    const uint8_t* font_data = NULL;
    uint8_t char_offset = 0;
    
    switch (font) {
        case FONT_TYPE_ASCII_8X16:
            font_data = ASCII;
            char_offset = (ch - 32) * 16; // ASCII字符偏移
            break;
            
        case FONT_TYPE_DIGIT_12X12:
            font_data = arry_dig;
            char_offset = (ch - '0') * 12; // 数字字符偏移
            break;
            
        case FONT_TYPE_DIGIT_32X15:
            font_data = arry_dig22;
            char_offset = (ch - '0') * 30; // 大数字字符偏移
            break;
            
        default:
            font_data = ASCII;
            char_offset = (ch - 32) * 16;
            break;
    }
    
    // 绘制字符
    for (uint8_t i = 0; i < font_height; i++) {
        if (page_offset + i < 8) {
            // 在当前页绘制（非阻塞）
            HAL_LCD_SendDataNonBlocking(font_data[char_offset + i]);
        } else {
            // 需要跨页绘制（简化处理，只绘制当前页）
            HAL_LCD_SendDataNonBlocking(0x00);
        }
        
        // 检查LCD是否空闲（HAL_LCD_Process已在Timer3中断中调用）
        if (HAL_LCD_IsBusy()) {
            return; // 如果忙碌，等待下次中断处理
        }
    }
}

/**
 * @name      Display_DrawChinese
 * @brief     绘制中文字符
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     chinese_data - 中文字模数据
 * @retval    无
 */
static void Display_DrawChinese(uint8_t x, uint8_t y, const uint8_t* chinese_data)
{
    uint8_t page = y / 8;
    uint8_t page_offset = y % 8;
    
    // 设置显示位置（非阻塞）
    HAL_LCD_SetPositionNonBlocking(page, x);
    
    // 等待LCD空闲
    while (HAL_LCD_IsBusy()) {
        HAL_LCD_Process();
    }
    
    // 绘制16x16中文字符（非阻塞）
    for (uint8_t i = 0; i < 16; i++) {
        if (page_offset + i < 8) {
            HAL_LCD_SendDataNonBlocking(chinese_data[i]);
        } else {
            HAL_LCD_SendDataNonBlocking(0x00);
        }
        
        // 检查LCD是否空闲（HAL_LCD_Process已在Timer3中断中调用）
        if (HAL_LCD_IsBusy()) {
            return; // 如果忙碌，等待下次中断处理
        }
    }
    
    // 绘制第二列
    HAL_LCD_SetPositionNonBlocking(page, x + 1);
    
    // 等待LCD空闲
    while (HAL_LCD_IsBusy()) {
        HAL_LCD_Process();
    }
    
    for (uint8_t i = 0; i < 16; i++) {
        if (page_offset + i < 8) {
            HAL_LCD_SendDataNonBlocking(chinese_data[i + 16]);
        } else {
            HAL_LCD_SendDataNonBlocking(0x00);
        }
        
        // 检查LCD是否空闲（HAL_LCD_Process已在Timer3中断中调用）
        if (HAL_LCD_IsBusy()) {
            return; // 如果忙碌，等待下次中断处理
        }
    }
}

/**
 * @name      Display_GetFontWidth
 * @brief     获取字体宽度
 * @param     font - 字体类型
 * @retval    字体宽度（像素）
 */
static uint8_t Display_GetFontWidth(FontType_e font)
{
    switch (font) {
        case FONT_TYPE_ASCII_8X16:
            return 8;
        case FONT_TYPE_ASCII_6X12:
            return 6;
        case FONT_TYPE_DIGIT_12X12:
            return 12;
        case FONT_TYPE_DIGIT_32X15:
            return 32;
        case FONT_TYPE_CHINESE_16X16:
            return 16;
        default:
            return 8;
    }
}

/**
 * @name      Display_GetFontHeight
 * @brief     获取字体高度
 * @param     font - 字体类型
 * @retval    字体高度（像素）
 */
static uint8_t Display_GetFontHeight(FontType_e font)
{
    switch (font) {
        case FONT_TYPE_ASCII_8X16:
            return 16;
        case FONT_TYPE_ASCII_6X12:
            return 12;
        case FONT_TYPE_DIGIT_12X12:
            return 12;
        case FONT_TYPE_DIGIT_32X15:
            return 15;
        case FONT_TYPE_CHINESE_16X16:
            return 16;
        default:
            return 16;
    }
}

/**
 * @name      Display_ConvertNumberToString
 * @brief     数字转字符串
 * @param     number - 数字
 * @param     str - 输出字符串
 * @param     max_len - 最大长度
 * @retval    无
 */
static void Display_ConvertNumberToString(int32_t number, char* str, uint8_t max_len)
{
    if (str == NULL || max_len == 0) {
        return;
    }
    
    int32_t temp = number;
    uint8_t i = 0;
    
    // 处理负数
    if (number < 0) {
        str[i++] = '-';
        temp = -number;
    }
    
    // 转换为字符串（简化版本）
    if (temp == 0) {
        str[i++] = '0';
    } else {
        // 简单的数字转换
        char digits[16];
        uint8_t digit_count = 0;
        
        while (temp > 0) {
            digits[digit_count++] = (char)('0' + (temp % 10));
            temp /= 10;
        }
        
        // 反转数字
        for (int8_t j = digit_count - 1; j >= 0; j--) {
            str[i++] = digits[j];
        }
    }
    
    str[i] = '\0';
}