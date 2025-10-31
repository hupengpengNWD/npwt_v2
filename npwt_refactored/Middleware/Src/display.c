#include "display.h"
#include "hal_lcd.h"
#include <stdio.h>
#include <string.h>

// 外部字体数据声明
extern const unsigned char arry_dig[];      // 数字字模 6x12
extern const unsigned char arry_char[];     // 字符字模 6x12
extern const unsigned char arry_digs12[];   // 数字字模 8x16
extern const unsigned char arry_char2[];    // 字符字模 8x16
extern const unsigned char arry_dig22[];    // 数字字模 16x32
extern const unsigned char arry_char22[];   // 字符字模 16x32
extern const unsigned char arry_dig40[];    // 数字字模 40x80
extern const unsigned char en_char_8x16[];     // ASCII字模 8x16（重命名）

/****************************************************************************
 * 字体信息结构体
 ****************************************************************************/

typedef struct {
    uint8_t width;                   // 字体宽度（像素列数），渲染时的列步进
    uint8_t height;                  // 字体高度（像素行数/页累计字节），用于按字形字节数定位
    const unsigned char* digit_font; // 数字字库指针（'0'-'9' 等），为空表示不支持数字位图直取
    const unsigned char* char_font;  // 字母/符号字库指针（自定义表，如 arry_char/arry_char2）
    const unsigned char* ascii_font; // 标准 ASCII 字库指针（如 8x16 ASCII），为空表示该字体类型不走 ASCII 表
} FontInfo_t;

/****************************************************************************
 * 私有变量
 ****************************************************************************/

static st_queue g_display_queue;              // 循环队列对象
static uint8_t g_display_queue_buffer[(8 + 1) * sizeof(DisplayEvent_t)];

// 字体信息表
static const FontInfo_t g_font_info[] = {
    {6, 12, arry_dig, arry_char, NULL},           // DISPLAY_FONT_6X12
    {8, 16, arry_digs12, NULL, en_char_8x16},        // DISPLAY_FONT_8X16 
    {16, 32, arry_dig22, arry_char22, NULL},      // DISPLAY_FONT_16X32
    {40, 80, arry_dig40, NULL, NULL}              // DISPLAY_FONT_40X80
};

/****************************************************************************
 * 私有函数声明
 ****************************************************************************/

static void Display_ProcessEvent(const DisplayEvent_t* event);
static void Display_ClearInternal(void);
static void Display_SetPositionInternal(uint8_t x, uint8_t y);
static void Display_SetBacklightInternal(bool white_on, bool yellow_on);
static void Display_ShowStringInternal(uint8_t x, uint8_t y, const char* str, DisplayFontType_e font, DisplayAlignType_e align);
static void Display_ShowNumberInternal(uint8_t x, uint8_t y, uint16_t number, DisplayFontType_e font, DisplayAlignType_e align);
static void Display_ShowImageInternal(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data);
static void Display_ShowPressureInternal(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit);
static void Display_ShowWorkModeInternal(uint8_t x, uint8_t y, DisplayWorkMode_e mode);
static void Display_ShowErrorInternal(uint8_t x, uint8_t y, DisplayErrorCode_e error);
static void Display_ShowBatteryIconInternal(uint8_t x, uint8_t y, uint8_t battery_level, bool is_charging);
static void Display_ShowStartupInterfaceInternal(void);

// 字符处理辅助函数
static void Display_SendCharData(uint8_t page, uint8_t column, const uint8_t* char_data, uint8_t width, uint8_t height);
static void Display_SendDigit(uint8_t page, uint8_t column, uint8_t digit, DisplayFontType_e font);
static void Display_SendLetter(uint8_t page, uint8_t column, uint8_t letter, DisplayFontType_e font);
static void Display_SendASCII(uint8_t page, uint8_t column, uint8_t ascii_char, DisplayFontType_e font);
static const FontInfo_t* Display_GetFontInfo(DisplayFontType_e font);

/****************************************************************************
 * 公共接口实现
 ****************************************************************************/

/**
 * @name      Display_Init
 * @brief     初始化显示模块
 * @param     无
 * @retval    无
 */
void Display_Init(void)
{
    // 初始化HAL层LCD
    HAL_LCD_Init();
    
    // 初始化显示队列（容量8，物理长度=容量+1）
    lib_queue_create(&g_display_queue);
    g_display_queue.configure(&g_display_queue);
    g_display_queue.initialize(&g_display_queue, 8 + 1, sizeof(DisplayEvent_t), g_display_queue_buffer);
    
    // 清屏
    // Display_Clear();
}

/**
 * @name      Display_Process
 * @brief     处理显示队列（每1ms调用一次）
 * @param     无
 * @retval    无
 */
void Display_Process(void)
{
    DisplayEvent_t event = {0};
    
    // 如果HAL层忙碌，直接返回
    if (HAL_LCD_IsBusy()) {
        return;
    }
    
    // 从队列中取出一个事件进行处理
    if (g_display_queue.get(&g_display_queue, &event, 1)) {
        Display_ProcessEvent(&event);
    }
}

/**
 * @name      Display_IsBusy
 * @brief     检查显示模块是否忙碌
 * @param     无
 * @retval    true-忙碌, false-空闲
 */
bool Display_IsBusy(void)
{
    return HAL_LCD_IsBusy();
}

/**
 * @name      Display_Clear
 * @brief     清屏
 * @param     无
 * @retval    无
 */
void Display_Clear(void)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_CLEAR,
        .x = 0,
        .y = 0
    };
    
    // 将清屏事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
}

/**
 * @name      Display_SetBacklight
 * @brief     设置背光
 * @param     white_on - 白色背光是否开启
 * @param     yellow_on - 黄色背光是否开启
 * @retval    无
 */
void Display_SetBacklight(bool white_on, bool yellow_on)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SET_BACKLIGHT,
        .x = 0,
        .y = 0,
        .bool_data = white_on,
        .byte_data = yellow_on ? 1 : 0
    };
    
    // 将设置背光事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
}


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
void Display_ShowString(uint8_t x, uint8_t y, const char* str, DisplayFontType_e font, DisplayAlignType_e align)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_STRING,
        .x = x,
        .y = y,
        .str_data = str,
        .font = font,
        .align = align
    };
    
    // 将显示字符串事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
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
void Display_ShowNumber(uint8_t x, uint8_t y, uint16_t number, DisplayFontType_e font, DisplayAlignType_e align)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_NUMBER,
        .x = x,
        .y = y,
        .number_data = number,
        .font = font,
        .align = align
    };
    
    // 将显示数字事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
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
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_IMAGE,
        .x = x,
        .y = y,
        .image_data = image_data,
        .image_width = width,
        .image_height = height
    };
    
    // 将显示图像事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
}

/**
 * @name      Display_ShowPressure
 * @brief     显示压力值
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     pressure - 压力值
 * @param     show_unit - 是否显示单位
 * @retval    无
 */
void Display_ShowPressure(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_PRESSURE,
        .x = x,
        .y = y,
        .number_data = pressure,
        .bool_data = show_unit
    };
    
    // 将显示压力事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
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
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_WORK_MODE,
        .x = x,
        .y = y,
        .work_mode = mode
    };
    
    // 将显示工作模式事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
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
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_ERROR,
        .x = x,
        .y = y,
        .error_code = error
    };
    
    // 将显示错误事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
}

/**
 * @name      Display_ShowBatteryIcon
 * @brief     显示电池图标
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     battery_level - 电池电量(0-100)
 * @param     is_charging - 是否充电
 * @retval    无
 */
void Display_ShowBatteryIcon(uint8_t x, uint8_t y, uint8_t battery_level, bool is_charging)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_BATTERY_ICON,
        .x = x,
        .y = y,
        .byte_data = battery_level,
        .bool_data = is_charging
    };
    
    // 将显示电池图标事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
}

/**
 * @name      Display_ShowStartupInterface
 * @brief     显示开机界面
 * @param     无
 * @retval    无
 */
void Display_ShowStartupInterface(void)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_STARTUP_INTERFACE,
        .x = 0,
        .y = 0
    };
    
    // 将显示开机界面事件加入队列
    (void)g_display_queue.put(&g_display_queue, &event, 1);
}


/****************************************************************************
 * 私有函数实现
 ****************************************************************************/

/**
 * @name      Display_ProcessEvent
 * @brief     处理显示事件
 * @param     event - 显示事件
 * @retval    无
 */
static void Display_ProcessEvent(const DisplayEvent_t* event)
{
    if (event == NULL) {
        return;
    }
    
    switch (event->type) {
        case DISPLAY_EVENT_CLEAR: // 清屏
            Display_ClearInternal();
            break;
            
        case DISPLAY_EVENT_SHOW_STRING: // 显示字符串
            Display_ShowStringInternal(event->x, event->y, event->str_data, event->font, event->align);
            break;
            
        case DISPLAY_EVENT_SHOW_NUMBER: // 显示数字
            Display_ShowNumberInternal(event->x, event->y, event->number_data, event->font, event->align);
            break;
            
        case DISPLAY_EVENT_SHOW_IMAGE: // 显示图像
            Display_ShowImageInternal(event->x, event->y, event->image_width, event->image_height, event->image_data);
            break;
            
        case DISPLAY_EVENT_SET_POSITION: // 设置位置
            Display_SetPositionInternal(event->x, event->y);
            break;
            
        case DISPLAY_EVENT_SET_BACKLIGHT: // 设置背光
            Display_SetBacklightInternal(event->bool_data, event->byte_data != 0);
            break;
            
        case DISPLAY_EVENT_SHOW_PRESSURE: // 显示压力
            Display_ShowPressureInternal(event->x, event->y, event->number_data, event->bool_data);
            break;
            
        case DISPLAY_EVENT_SHOW_WORK_MODE: // 显示工作模式
            Display_ShowWorkModeInternal(event->x, event->y, event->work_mode);
            break;
            
        case DISPLAY_EVENT_SHOW_ERROR: // 显示错误
            Display_ShowErrorInternal(event->x, event->y, event->error_code);
            break;
            
        case DISPLAY_EVENT_SHOW_BATTERY_ICON: // 显示电池图标
            Display_ShowBatteryIconInternal(event->x, event->y, event->byte_data, event->bool_data);
            break;
            
        case DISPLAY_EVENT_SHOW_STARTUP_INTERFACE: // 显示开机界面
            Display_ShowStartupInterfaceInternal();
            break;
            
        default:
            // 未知事件类型，忽略
            break;
    }
}

/**
 * @name      Display_ClearInternal
 * @brief     内部清屏函数
 * @param     无
 * @retval    无
 */
static void Display_ClearInternal(void)
{
    HAL_LCD_ClearNonBlocking();
}

/**
 * @name      Display_SetPositionInternal
 * @brief     内部设置位置函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @retval    无
 */
static void Display_SetPositionInternal(uint8_t x, uint8_t y)
{
    HAL_LCD_SetPositionNonBlocking(x, y);
}

/**
 * @name      Display_SetBacklightInternal
 * @brief     内部设置背光函数
 * @param     white_on - 白色背光是否开启
 * @param     yellow_on - 黄色背光是否开启
 * @retval    无
 */
static void Display_SetBacklightInternal(bool white_on, bool yellow_on)
{
    HAL_LCD_SetBacklight(white_on);  // 暂时只控制白色背光
}

/**
 * @name      Display_ShowStringInternal
 * @brief     内部显示字符串函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     str - 字符串
 * @param     font - 字体类型
 * @param     align - 对齐方式
 * @retval    无
 */
static void Display_ShowStringInternal(uint8_t x, uint8_t y, const char* str, DisplayFontType_e font, DisplayAlignType_e align)
{
    if (str == NULL) {
        return;
    }
    
    const FontInfo_t* font_info = Display_GetFontInfo(font);
    uint8_t current_col = x;
    
    // 遍历字符串中的每个字符
    while (*str) {
        uint8_t ch = *str;

        if (ch >= '0' && ch <= '9') {
            // 数字：优先数字字库，否则回退到ASCII
            if (font_info->digit_font != NULL) {
                Display_SendDigit(y, current_col, ch - '0', font);
            } else if (font_info->ascii_font != NULL) {
                Display_SendASCII(y, current_col, ch, font);
            }
            current_col += font_info->width;
        } else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) {
            // 字母：有字母字库用字母字库，否则回退到ASCII
            if (font_info->char_font != NULL) {
                uint8_t idx = (ch >= 'a') ? (uint8_t)(ch - 'a') : (uint8_t)(ch - 'A');
                Display_SendLetter(y, current_col, idx, font);
            } else if (font_info->ascii_font != NULL) {
                Display_SendASCII(y, current_col, ch, font);
            }
            current_col += font_info->width;
        } else if (ch == ' ') {
            // 空格：前进一个字宽
            current_col += font_info->width;
        } else if (font_info->ascii_font != NULL) {
            // 其他可显示ASCII
            Display_SendASCII(y, current_col, ch, font);
            current_col += font_info->width;
        } else {
            // 不支持：占位前进
            current_col += font_info->width;
        }
        str++;
    }
}

/**
 * @name      Display_ShowNumberInternal
 * @brief     内部显示数字函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     number - 数字
 * @param     font - 字体类型
 * @param     align - 对齐方式
 * @retval    无
 */
static void Display_ShowNumberInternal(uint8_t x, uint8_t y, uint16_t number, DisplayFontType_e font, DisplayAlignType_e align)
{
    char number_str[16];
    snprintf(number_str, sizeof(number_str), "%d", number);
    
    Display_ShowStringInternal(x, y, number_str, font, align);
}

/**
 * @name      Display_ShowImageInternal
 * @brief     内部显示图像函数
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
        return;
    }
    
    // 计算需要多少页来显示图像
    uint8_t pages_needed = (height + 7) / 8;  // 每页8像素高
    
    // 逐页显示图像
    for (uint8_t page = 0; page < pages_needed; page++) {
        HAL_LCD_SetPositionNonBlocking(y + page, x);
        
        // 发送该页的所有列数据
        for (uint8_t col = 0; col < width; col++) {
            uint8_t data_index = page * width + col;
            if (data_index < (width * pages_needed)) {
                HAL_LCD_SendDataNonBlocking(image_data[data_index]);
            }
        }
    }
}

/**
 * @name      Display_ShowPressureInternal
 * @brief     内部显示压力函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     pressure - 压力值
 * @param     show_unit - 是否显示单位
 * @retval    无
 */
static void Display_ShowPressureInternal(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit)
{
    char pressure_str[16];
    snprintf(pressure_str, sizeof(pressure_str), "%d", pressure);
    
    // 显示压力值
    Display_ShowStringInternal(x, y, pressure_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 如果需要显示单位
    if (show_unit) {
        Display_ShowStringInternal(x + 6, y, "kPa", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    }
}

/**
 * @name      Display_ShowWorkModeInternal
 * @brief     内部显示工作模式函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     mode - 工作模式
 * @retval    无
 */
static void Display_ShowWorkModeInternal(uint8_t x, uint8_t y, DisplayWorkMode_e mode)
{
    const char* mode_str = "Unknown";
    
    switch (mode) {
        case DISPLAY_WORK_MODE_INIT:
            mode_str = "Init";
            break;
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
        case DISPLAY_WORK_MODE_SETTINGS:
            mode_str = "Settings";
            break;
        case DISPLAY_WORK_MODE_SELFTEST:
            mode_str = "SelfTest";
            break;
        case DISPLAY_WORK_MODE_ERROR:
            mode_str = "Error";
            break;
        case DISPLAY_WORK_MODE_SHUTDOWN:
            mode_str = "Shutdown";
            break;
        default:
            break;
    }
    
    Display_ShowStringInternal(x, y, mode_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      Display_ShowErrorInternal
 * @brief     内部显示错误函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     error - 错误代码
 * @retval    无
 */
static void Display_ShowErrorInternal(uint8_t x, uint8_t y, DisplayErrorCode_e error)
{
    const char* error_str = "Unknown";
    
    switch (error) {
        case DISPLAY_ERROR_NONE:
            error_str = "No Error";
            break;
        case DISPLAY_ERROR_LEAKAGE:
            error_str = "Leakage";
            break;
        case DISPLAY_ERROR_BLOCKAGE:
            error_str = "Blockage";
            break;
        case DISPLAY_ERROR_OVERPRESSURE:
            error_str = "Over Pressure";
            break;
        case DISPLAY_ERROR_BATTERY_LOW:
            error_str = "Battery Low";
            break;
        case DISPLAY_ERROR_BATTERY_CRITICAL:
            error_str = "Battery Critical";
            break;
        case DISPLAY_ERROR_LIQUID_FULL:
            error_str = "Liquid Full";
            break;
        case DISPLAY_ERROR_SENSOR:
            error_str = "Sensor Error";
            break;
        case DISPLAY_ERROR_PUMP:
            error_str = "Pump Error";
            break;
        case DISPLAY_ERROR_VALVE:
            error_str = "Valve Error";
            break;
        default:
            break;
    }
    
    Display_ShowStringInternal(x, y, error_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      Display_ShowBatteryIconInternal
 * @brief     内部显示电池图标函数
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     battery_level - 电池电量(0-100)
 * @param     is_charging - 是否充电
 * @retval    无
 */
static void Display_ShowBatteryIconInternal(uint8_t x, uint8_t y, uint8_t battery_level, bool is_charging)
{
    char battery_str[16];
    snprintf(battery_str, sizeof(battery_str), "%d%%", battery_level);
    
    // 显示电池电量
    Display_ShowStringInternal(x, y, battery_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 如果正在充电，显示充电图标
    if (is_charging) {
        Display_ShowStringInternal(x + 4, y, "+", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    }
}

/**
 * @name      Display_ShowStartupInterfaceInternal
 * @brief     内部显示开机界面函数
 * @param     无
 * @retval    无
 */
static void Display_ShowStartupInterfaceInternal(void)
{
    // 清屏
    HAL_LCD_ClearNonBlocking();
    
    // 显示开机信息
    Display_ShowStringInternal(0, 0, "BBXCC", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
}

/****************************************************************************
 * 字符处理辅助函数实现
 ****************************************************************************/

/**
 * @name      Display_SendCharData
 * @brief     发送字符数据到LCD
 * @param     page - 页地址
 * @param     column - 列地址
 * @param     char_data - 字符数据指针
 * @param     width - 字符宽度
 * @param     height - 字符高度
 * @retval    无
 */
static void Display_SendCharData(uint8_t page, uint8_t column, const uint8_t* char_data, uint8_t width, uint8_t height)
{
    if (char_data == NULL) {
        return;
    }

    // 坐标转换：Display层使用自然坐标（0=顶部），HAL层需要硬件坐标（6=顶部）
    uint8_t page_hw = (uint8_t)(6 - (page & 0x07));  // 软件页0→硬件页6，软件页6→硬件页0
    
    // 针对不同字模布局分别处理
    if (width == 8 && height == 16) {
        // 与未重构工程一致：先写上半到 page_hw，再写下半到 page_hw+1
        // 上半页：char_data[15]..char_data[8] → page_hw（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw, column);
        for (int8_t idx = 15; idx >= 8; idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }
        // 下半页：char_data[7]..char_data[0] → page_hw+1（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw + 1, column);
        for (int8_t idx = 7; idx >= 0; idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }

    } else {
        // 默认路径：6x12 自定义点阵（两页各 width 字节，带位移重组）
        HAL_LCD_SetPositionNonBlocking(page_hw, column);
        for (uint8_t i = 0; i < width; i++) {
            uint16_t data = (uint16_t)char_data[i] | ((uint16_t)char_data[i + width] << 8);
            data = (data >> 3) & 0xFF;
            HAL_LCD_SendDataNonBlocking((uint8_t)data);
        }
        HAL_LCD_SetPositionNonBlocking(page_hw + 1, column);
        for (uint8_t i = 0; i < width; i++) {
            uint16_t data = ((uint16_t)char_data[i + width] << 8) | (uint16_t)char_data[i];
            data = (data >> 3) >> 8;
            HAL_LCD_SendDataNonBlocking((uint8_t)data);
        }
    }
}

/**
 * @name      Display_GetFontInfo
 * @brief     获取字体信息
 * @param     font - 字体类型
 * @retval    字体信息指针
 */
static const FontInfo_t* Display_GetFontInfo(DisplayFontType_e font)
{
    if (font >= sizeof(g_font_info) / sizeof(g_font_info[0])) {
        return &g_font_info[0];  // 默认返回6x12字体
    }
    return &g_font_info[font];
}

/**
 * @name      Display_SendDigit
 * @brief     发送数字字符
 * @param     page - 页地址
 * @param     column - 列地址
 * @param     digit - 数字(0-9)
 * @param     font - 字体类型
 * @retval    无
 */
static void Display_SendDigit(uint8_t page, uint8_t column, uint8_t digit, DisplayFontType_e font)
{
    if (digit > 9) {
        return;
    }
    
    const FontInfo_t* font_info = Display_GetFontInfo(font);
    if (font_info->digit_font == NULL) {
        return;
    }
    
    uint16_t offset = digit * font_info->height;  // 每个数字的字节数
    Display_SendCharData(page, column, &font_info->digit_font[offset], font_info->width, font_info->height);
}

/**
 * @name      Display_SendLetter
 * @brief     发送字母字符
 * @param     page - 页地址
 * @param     column - 列地址
 * @param     letter - 字母索引(0-25对应A-Z)
 * @param     font - 字体类型
 * @retval    无
 */
static void Display_SendLetter(uint8_t page, uint8_t column, uint8_t letter, DisplayFontType_e font)
{
    if (letter > 25) {
        return;
    }
    
    const FontInfo_t* font_info = Display_GetFontInfo(font);
    if (font_info->char_font == NULL) {
        return;
    }
    
    uint16_t offset = letter * font_info->height;  // 每个字母的字节数
    Display_SendCharData(page, column, &font_info->char_font[offset], font_info->width, font_info->height);
}

/**
 * @name      Display_SendASCII
 * @brief     发送ASCII字符
 * @param     page - 页地址
 * @param     column - 列地址
 * @param     ascii_char - ASCII字符
 * @param     font - 字体类型
 * @retval    无
 */
static void Display_SendASCII(uint8_t page, uint8_t column, uint8_t ascii_char, DisplayFontType_e font)
{
    const FontInfo_t* font_info = Display_GetFontInfo(font);
    if (font_info->ascii_font == NULL) {
        return;
    }
    
    // ASCII字符从32开始（空格）
    if (ascii_char < 32) {
        ascii_char = 32;  // 转换为空格
    }
    
    uint16_t offset = (ascii_char - 32) * font_info->height;  // 每个ASCII字符的字节数
    Display_SendCharData(page, column, &font_info->ascii_font[offset], font_info->width, font_info->height);
}

/****************************************************************************
 * 测试界面实现
 ****************************************************************************/

/**
 * @name      Display_ShowImageTest
 * @brief     显示图片测试界面
 * @param     无
 * @retval    无
 */
void Display_ShowImageTest(void)
{
    // 清屏
    HAL_LCD_ClearNonBlocking();
    
    // 显示图片测试标题
    Display_ShowStringInternal(0, 0, "IMAGE TEST", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 显示开机Logo图片 (128x64像素)
    Display_ShowImageInternal(0, 12, 128, 64, LOGO_STARTUP_IMAGE);
}

/**
 * @name      Display_ShowChineseTest
 * @brief     显示中文测试界面
 * @param     无
 * @retval    无
 */
void Display_ShowChineseTest(void)
{
    // 清屏
    HAL_LCD_ClearNonBlocking();
    
    // 显示中文测试标题
    Display_ShowStringInternal(0, 0, "CHINESE TEST", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 显示中文字符测试
    Display_ShowStringInternal(0, 12, "Hello World", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 24, "NPWT System", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 36, "Test Mode", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 48, "Press Keys", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      Display_ShowEnglishTest
 * @brief     显示英文测试界面
 * @param     无
 * @retval    无
 */
void Display_ShowEnglishTest(void)
{
    // 清屏
    HAL_LCD_ClearNonBlocking();
    
    // 显示英文测试标题
    Display_ShowStringInternal(0, 0, "ENGLISH TEST", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 显示英文字符测试
    Display_ShowStringInternal(0, 12, "ABCDEFGHIJK", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 24, "LMNOPQRSTUV", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 36, "WXYZ0123456", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 48, "789!@#$%^&*", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
}