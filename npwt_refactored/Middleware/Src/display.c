#include "display.h"
#include "hal_lcd.h"
#include <stdio.h>
#include <string.h>

// 外部字体数据声明
extern const unsigned char en_char_6x12[];    
extern const unsigned char en_char_7x14[];    
extern const unsigned char en_char_8x16[];
extern const unsigned char digit_char_16x32[];  // 16x32数字字模（仅包含0-9）

// 外部图标数据声明（在Library/lcd_icon_data.c中定义）
extern const unsigned char icon_image_lock_8x16[];
extern const unsigned char icon_image_unlock_8x16[];
extern const unsigned char icon_image_key1_16x16[];
extern const unsigned char icon_image_key2_16x16[];
extern const unsigned char icon_image_silent_16x16[];
extern const unsigned char icon_image_tick_16x16[];
extern const unsigned char icon_image_intermittent_24x16[];
extern const unsigned char icon_image_continuous_24x16[];
// 电池图标数据声明（24x16像素）
extern const unsigned char icon_image_bat0_24x16[];  // 0%电池图标
extern const unsigned char icon_image_bat1_24x16[];  // 25%电池图标
extern const unsigned char icon_image_bat2_24x16[];  // 50%电池图标
extern const unsigned char icon_image_bat3_24x16[];  // 75%电池图标
extern const unsigned char icon_image_bat4_24x16[];  // 100%电池图标     

/****************************************************************************
 * 字体信息结构体
 ****************************************************************************/

typedef struct {
    uint8_t width;                   // 字体宽度（像素列数），渲染时的列步进
    uint8_t height;                  // 字体高度（像素行数/页累计字节），用于按字形字节数定位
    const unsigned char* char_font;  // 扩展字符字库指针（中文、俄文等非ASCII字符，如 arry_char/arry_char2）
    const unsigned char* ascii_font; // ASCII字符字库指针（包含数字0-9、字母A-Z/a-z及符号，如 8x16 ASCII）
} FontInfo_t;

/****************************************************************************
 * 私有变量
 ****************************************************************************/

static st_queue g_display_queue;              // 循环队列对象
static uint8_t g_display_queue_buffer[(31 + 1) * sizeof(DisplayEvent_t)];

// 字体信息表
// height字段在Display_SendASCII中用作每个字符的字节数来计算偏移
// 对于16x32字体：32像素高 = 4页，每字符字节数 = 16列 × 4页 = 64字节
static const FontInfo_t g_font_info[] = {
    {6, 12, NULL, en_char_6x12},           // 6x12: 每字符12字节
    {7, 14, NULL, en_char_7x14},           // 7x14: 每字符14字节
    {8, 16, NULL, en_char_8x16},           // 8x16: 每字符16字节
    {16, 64, NULL, digit_char_16x32},      // 16x32: 每字符64字节（16列×4页）
    {40, 80, NULL, NULL}                   // 40x80: 预留
};

/****************************************************************************
 * 私有函数声明
 ****************************************************************************/

static void Display_ProcessEvent(const DisplayEvent_t* event);
static void Display_ClearInternal(void);
static void Display_ClearRectInternal(uint8_t x, uint8_t y, uint8_t width, uint8_t height);
static void Display_SetPositionInternal(uint8_t x, uint8_t y);
static void Display_SetBacklightInternal(bool white_on, bool yellow_on);
static void Display_ShowStringInternal(uint8_t x, uint8_t y, const char* str, DisplayFontType_e font, DisplayAlignType_e align);
static void Display_ShowNumberInternal(uint8_t x, uint8_t y, uint16_t number, DisplayFontType_e font, DisplayAlignType_e align);
static void Display_ShowImageInternal(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data);
/* Display_ShowImageStartupFormat 已在 display.h 中声明为公开函数，此处不再声明 */
static void Display_ShowPressureInternal(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit, DisplayFontType_e font);
static void Display_ShowWorkModeInternal(uint8_t x, uint8_t y, DisplayWorkMode_e mode);
static void Display_ShowErrorInternal(uint8_t x, uint8_t y, DisplayErrorCode_e error);
static void Display_ShowBatteryIconInternal(uint8_t x, uint8_t y, uint8_t battery_level, bool is_charging);
static void Display_ShowStartupInterfaceInternal(void);
static void Display_ShowIconInternal(uint8_t x, uint8_t y, IconType_e icon_type);

// 字符处理辅助函数
static void Display_SendCharData(uint8_t page, uint8_t column, const uint8_t* char_data, uint8_t width, uint8_t height);
static void Display_SendASCII(uint8_t page, uint8_t column, uint8_t ascii_char, DisplayFontType_e font);
static void Display_SendCharFont(uint8_t page, uint8_t column, uint8_t char_code, DisplayFontType_e font);
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
 * @name      Display_ClearQueue
 * @brief     清空显示队列（清除所有未处理的显示事件）
 * @param     无
 * @retval    无
 */
void Display_ClearQueue(void)
{
    // 清空显示队列，移除所有未处理的事件
    (void)g_display_queue.clears(&g_display_queue);  // 清空所有事件
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
 * @name      Display_ClearRect
 * @brief     清除LCD矩形区域
 * @param     x - 起始列坐标（软件坐标，0-127，0=左侧）
 * @param     y - 起始页坐标（软件坐标，0-7，0=顶部）
 * @param     width - 区域宽度（列数，1-128）
 * @param     height - 区域高度（像素数，1-64）
 * @retval    无
 */
void Display_ClearRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_CLEAR_RECT,
        .x = x,
        .y = y,
        .image_width = width,
        .image_height = height
    };
    
    // 将局部清除事件加入队列
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
 * @name      Display_ShowIcon
 * @brief     显示图标（使用队列）
 * @param     x - X坐标（列）
 * @param     y - Y坐标（页）
 * @param     icon_type - 图标类型（IconType_e枚举值）
 * @retval    无
 */
void Display_ShowIcon(uint8_t x, uint8_t y, uint8_t icon_type)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_ICON,
        .x = x,
        .y = y,
        .byte_data = icon_type  // 使用byte_data存储图标类型
    };
    
    // 将显示图标事件加入队列
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
void Display_ShowPressure(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit, DisplayFontType_e font)
{
    DisplayEvent_t event = {
        .type = DISPLAY_EVENT_SHOW_PRESSURE,
        .x = x,
        .y = y,
        .number_data = pressure,
        .bool_data = show_unit,
        .font = font
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
            
        case DISPLAY_EVENT_CLEAR_RECT: // 局部清除
            Display_ClearRectInternal(event->x, event->y, event->image_width, event->image_height);
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
            Display_ShowPressureInternal(event->x, event->y, event->number_data, event->bool_data, event->font);
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
            
        case DISPLAY_EVENT_SHOW_ICON: // 显示图标
            Display_ShowIconInternal(event->x, event->y, (IconType_e)event->byte_data);
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
 * @name      Display_ClearRectInternal
 * @brief     内部局部清除函数（使用写入空白数据方案，复用Display_SendCharData逻辑）
 * @param     x - 起始列坐标（软件坐标，0-127）
 * @param     y - 起始页坐标（软件坐标，0-7）
 * @param     width - 区域宽度（列数）
 * @param     height - 区域高度（像素数）
 * @retval    无
 */
static void Display_ClearRectInternal(uint8_t x, uint8_t y, uint8_t width, uint8_t height)
{
    // 参数验证
    if (width == 0 || height == 0) {
        return;
    }
    if (y >= 8) {
        return;
    }
    if (x >= 128) {
        return;
    }
    
    // 计算需要写入的字节数
    uint8_t pages = (uint8_t)((height + 7U) / 8U);  // 向上取整页数
    uint8_t data_size = (uint8_t)(width * pages);
    
    // 使用静态缓冲区（最大支持 width=16, pages=4 => 64字节）
    static uint8_t blank_data[64] = {0};
    
    if (data_size > sizeof(blank_data)) {
        return;  // 超出支持范围，返回
    }
    
    // 使用Display_SendCharData写入空白数据（完全复用显示逻辑）
    Display_SendCharData(y, x, blank_data, width, (uint8_t)(pages * 8U));
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

        // ASCII字符（0x20-0x7F）：数字、字母、符号等使用ASCII字库
        if (ch >= 0x20 && ch <= 0x7F) {
            if (font_info->ascii_font != NULL) {
                Display_SendASCII(y, current_col, ch, font);
                current_col += font_info->width;
            } else {
                // ASCII字库不可用，占位前进
                current_col += font_info->width;
            }
        }
        // 非ASCII字符（0x80-0xFF）：中文、俄文等使用扩展字符字库
        else if (ch >= 0x80) {
            if (font_info->char_font != NULL) {
                Display_SendCharFont(y, current_col, ch, font);
                current_col += font_info->width;
            } else {
                // 扩展字符字库不可用，占位前进
                current_col += font_info->width;
            }
        }
        // 控制字符（0x00-0x1F）：占位前进（空格0x20已在ASCII分支处理）
        else {
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
 * @name      Display_ShowImageStartupFormat
 * @brief     显示标准格式（行优先、上到下、左到右）的启动 LOGO
 * @note      不改变通用图像接口，避免影响既有逻辑
 */
void Display_ShowImageStartupFormat(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data)
{
    if (image_data == NULL) {
        return;
    }

    uint8_t pages_needed = (height + 7) / 8;

    for (uint8_t page = 0; page < pages_needed; page++) {
        uint8_t target_page = (uint8_t)((y + page) & 0x07);
        uint8_t page_hw = (uint8_t)((6 - target_page + 8) & 0x07);
 
        const uint8_t* page_ptr = image_data + (page * width);
        for (uint8_t col = 0; col < width; col++) {
            if (((x-6) + col) >= 128) {
                break;
            }
            HAL_LCD_SetPositionNonBlocking(page_hw, (uint8_t)((x-6) + col));
            HAL_LCD_SendDataNonBlocking(page_ptr[col]);
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
 * @param     font - 字体类型
 * @retval    无
 */
static void Display_ShowPressureInternal(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit, DisplayFontType_e font)
{
    char pressure_str[16];
    snprintf(pressure_str, sizeof(pressure_str), "%03d", pressure);

    // 获取字体信息以计算单位位置
    const FontInfo_t* font_info = Display_GetFontInfo(font);
    uint8_t unit_y_offset = 0;  // 单位垂直偏移（页）
    uint8_t unit_x_offset = 0;
 
    // 根据字体大小计算单位位置偏移
    if (font == DISPLAY_FONT_16X32) {
        // 16x32字体：每个数字16列宽，根据实际字符串长度计算
        uint8_t digit_count = (uint8_t)strlen(pressure_str);
        unit_x_offset = font_info->width * digit_count + 2;  // 数字宽度 + 2列间距
        unit_y_offset = 0;  // 单位显示在压力值顶部，与压力值同一水平线
    } else {
        // 其他字体：使用默认偏移
        uint8_t digit_count = (uint8_t)strlen(pressure_str);
        unit_x_offset = font_info->width * digit_count + 2;  // 数字宽度 + 2列间距
        unit_y_offset = 0;  // 同一行显示
    }

    // 显示压力值
    Display_ShowStringInternal(x, y, pressure_str, font, DISPLAY_ALIGN_LEFT);

    // 如果需要显示单位（使用较小的字体显示单位，保持可读性）
    if (show_unit) {
        // 单位使用8x16字体，位置在数字右侧，垂直对齐
        Display_ShowStringInternal(x + unit_x_offset, y + unit_y_offset, "mmhg", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
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
    
    Display_ShowStringInternal(x, y, mode_str, DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
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
    
    Display_ShowStringInternal(x, y, error_str, DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
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
    IconType_e battery_icon_type;
    
    // 根据电池电量选择对应的图标类型（与未重构工程一致：bat_lev 0-4对应0%, 25%, 50%, 75%, 100%）
    if (battery_level <= 20) {
        battery_icon_type = ICON_BAT0;  // 0%电池图标
    } else if (battery_level <= 40) {
        battery_icon_type = ICON_BAT1;  // 25%电池图标
    } else if (battery_level <= 60) {
        battery_icon_type = ICON_BAT2;  // 50%电池图标
    } else if (battery_level <= 80) {
        battery_icon_type = ICON_BAT3;  // 75%电池图标
    } else {
        battery_icon_type = ICON_BAT4;  // 100%电池图标
    }
    
    // 使用统一的图标显示函数（自动处理列偏移等）
    Display_ShowIconInternal(x, y, battery_icon_type);
    
    // 如果正在充电，可以在图标旁边显示充电指示（可选）
    // 注意：未重构工程中充电状态通过LED显示，这里暂时不显示文字
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
//    Display_ShowStringInternal(0, 0, "VR NPWT DEVICE", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowImageStartupFormat(0, 0, 128, 64, LOGO_STARTUP_IMAGE);
}

/****************************************************************************
 * 图标数据查找表（在display.c中定义）
 ****************************************************************************/

static const IconData_t g_icon_table[ICON_COUNT] = {
    {16, 16, icon_image_key1_16x16},           // ICON_KEY1
    {16, 16, icon_image_key2_16x16},           // ICON_KEY2
    {24, 16, icon_image_continuous_24x16},    // ICON_CONTINUOUS
    {24, 16, icon_image_intermittent_24x16},  // ICON_INTERMITTENT
    {16, 16, icon_image_silent_16x16},        // ICON_SILENT
    {8, 16, icon_image_lock_8x16},            // ICON_LOCK
    {8, 16, icon_image_unlock_8x16},          // ICON_UNLOCK
    {16, 16, icon_image_tick_16x16},          // ICON_TICK
    {24, 16, icon_image_bat0_24x16},          // ICON_BAT0 - 电池图标0%
    {24, 16, icon_image_bat1_24x16},          // ICON_BAT1 - 电池图标25%
    {24, 16, icon_image_bat2_24x16},          // ICON_BAT2 - 电池图标50%
    {24, 16, icon_image_bat3_24x16},          // ICON_BAT3 - 电池图标75%
    {24, 16, icon_image_bat4_24x16}           // ICON_BAT4 - 电池图标100%
};

/****************************************************************************
 * 图标辅助函数
 ****************************************************************************/

/**
 * @name      IconData_Get
 * @brief     获取指定图标的数据信息（内部函数）
 * @param     icon_type - 图标类型
 * @retval    图标数据指针，如果类型无效返回NULL
 */
static const IconData_t* IconData_Get(IconType_e icon_type)
{
    if (icon_type >= ICON_COUNT) {
        return NULL;
    }
    return &g_icon_table[icon_type];
}

/**
 * @name      Display_ShowIconInternal
 * @brief     内部显示图标函数
 * @param     x - X坐标（列）
 * @param     y - Y坐标（页）
 * @param     icon_type - 图标类型
 * @retval    无
 */
static void Display_ShowIconInternal(uint8_t x, uint8_t y, IconType_e icon_type)
{
    const IconData_t* icon_data = IconData_Get(icon_type);
    
    if (icon_data == NULL || icon_data->data == NULL) {
        return;
    }
    
    uint8_t column_offset = 0; // hpp
    if (icon_data->width == 24) {
        column_offset = 17;
    }
    
    // 图标数据按照字符字模规则存储，使用Display_SendCharData倒序写入
    // 图标数据格式与字符相同（下半在前，上半在后），所以使用相同的倒序写入方式
    Display_SendCharData(y, x+column_offset, icon_data->data, icon_data->width, icon_data->height);
}

/**
 * @name      Display_ClearIconArea
 * @brief     按与图标绘制相同的映射路径清除图标区域
 * @param     x - X坐标（列）
 * @param     y - Y坐标（页）
 * @param     icon_type - 图标类型（用于获取宽高与写入路径一致）
 * @retval    无
 * @note
 *   - 使用与 Display_ShowIconInternal 完全一致的列/页寻址与数据写入路径
 *   - 逐列写入0数据，避免坐标系/列偏移差异导致清除不完整
 */
void Display_ClearIconArea(uint8_t x, uint8_t y, IconType_e icon_type)
{
    const IconData_t* icon_data = IconData_Get(icon_type);
    if (icon_data == NULL) {
        return;
    }
    uint8_t width = icon_data->width;
    uint8_t height = icon_data->height;

    /* 与 Display_ShowIconInternal 保持一致的列偏移补偿 */
    uint8_t column_offset = 0; // hpp
    if (width == 24) {
        column_offset = 17;
    }

    /* 根据高度准备清零缓冲区（字模格式：16像素高=2*width，32像素高=4*width） */
    uint8_t zero_buf[64];
    uint16_t bytes = (height == 32) ? (uint16_t)(width * 4U) : (uint16_t)(width * 2U);
    if (bytes > sizeof(zero_buf)) {
        return; // 安全保护，理论不会超过
    }
    for (uint16_t i = 0; i < bytes; i++) {
        zero_buf[i] = 0x00;
    }

    /* 复用字符发送路径，保证与图标绘制相同的页/列映射 */
    Display_SendCharData(y, (uint8_t)(x + column_offset), zero_buf, width, height);
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

#if 0 // 原实现（整体屏蔽保留）：为旧arry_char设计的重组逻辑，不支持标准ASCII字库
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
        // 默认路径：7x14 自定义点阵（两页各 width 字节，带位移重组）
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
#else
    // 新实现：支持标准ASCII字库格式（en_char_7x14, en_char_8x16等）和图标数据
    // 坐标转换：Display层使用自然坐标（0=顶部），HAL层需要硬件坐标（6=顶部）
    uint8_t page_hw = (uint8_t)(6 - (page & 0x07));  // 软件页0→硬件页6，软件页6→硬件页0
    
    // 根据字符高度选择不同的数据格式处理
    if (height == 32) {
        // 32像素高（4页）：16x32数字字模格式
        // 数据格式：按页顺序存储，每页16字节（16列）
        // char_data[0..15] = 第1页，char_data[16..31] = 第2页
        // char_data[32..47] = 第3页，char_data[48..63] = 第4页
        // 写入顺序：第4页→第3页→第2页→第1页（从下到上，倒序）
        uint8_t bytes_per_page = width;  // 16列 = 16字节/页
        
        // 第4页（最上页，字节48-63）→ page_hw（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw, column);
        for (int16_t idx = (int16_t)(4 * bytes_per_page - 1); idx >= (int16_t)(3 * bytes_per_page); idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }
        
        // 第3页（字节32-47）→ page_hw+1（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw + 1, column);
        for (int16_t idx = (int16_t)(3 * bytes_per_page - 1); idx >= (int16_t)(2 * bytes_per_page); idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }
        
        // 第2页（字节16-31）→ page_hw+2（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw + 2, column);
        for (int16_t idx = (int16_t)(2 * bytes_per_page - 1); idx >= (int16_t)(1 * bytes_per_page); idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }
        
        // 第1页（最下页，字节0-15）→ page_hw+3（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw + 3, column);
        for (int16_t idx = (int16_t)(1 * bytes_per_page - 1); idx >= 0; idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }
    } else {
        // 16像素高（2页）：标准ASCII字库格式（en_char_7x14, en_char_8x16等）和图标数据
        // 字库构造规则：char_data[0..(width-1)] = 下半部分，char_data[width..(2*width-1)] = 上半部分
        // 每列1字节，总字节数 = width * 2（下半部分width字节，上半部分width字节）
        // 数据格式：char_data[0..(width-1)] = 下半部分（列0到列width-1，每列8像素）
        //          char_data[width..(2*width-1)] = 上半部分（列0到列width-1，每列8像素）
        // 先写上半部分：char_data[(2*width-1)]..char_data[width] → page_hw（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw, column);
        for (int16_t idx = (int16_t)(2 * width - 1); idx >= (int16_t)width; idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }
        
        // 再写下半部分：char_data[(width-1)]..char_data[0] → page_hw+1（倒序）
        HAL_LCD_SetPositionNonBlocking(page_hw + 1, column);
        for (int16_t idx = (int16_t)(width - 1); idx >= 0; idx--) {
            HAL_LCD_SendDataNonBlocking(char_data[idx]);
        }
    }
#endif
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
        return &g_font_info[0];  // 默认返回7x14字体
    }
    return &g_font_info[font];
}

/**
 * @name      Display_SendCharFont
 * @brief     发送扩展字符（非ASCII字符，如中文、俄文等）
 * @param     page - 页地址
 * @param     column - 列地址
 * @param     char_code - 字符编码（0x80-0xFF）
 * @param     font - 字体类型
 * @retval    无
 */
static void Display_SendCharFont(uint8_t page, uint8_t column, uint8_t char_code, DisplayFontType_e font)
{
    const FontInfo_t* font_info = Display_GetFontInfo(font);
    if (font_info->char_font == NULL) {
        return;
    }
    
    // 扩展字符索引计算（0x80-0xFF）
    // 当前实现：假设从0x80开始连续存储，每个字符占用height字节
    // 对于俄文（Cyrillic）：offset = (char_code - 0x80) * height
    // 对于中文等：可能需要其他索引方式，这里先使用简单的连续索引
    if (char_code < 0x80) {
        // 非扩展字符：不应调用此函数
        return;
    }
    
    // 计算字符在字库中的偏移量
    uint16_t offset = (char_code - 0x80) * font_info->height;
    
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
    
    uint16_t offset;
    uint8_t pixel_height;  // 像素高度（用于Display_SendCharData）
    
    // 16x32字体特殊处理：只包含数字0-9（ASCII 48-57）
    if (font == DISPLAY_FONT_16X32) {
        // 只处理数字字符 '0'-'9' (ASCII 48-57)
        if (ascii_char >= '0' && ascii_char <= '9') {
            // 数字字模按顺序存储：'0'在偏移0，'1'在偏移64，...，'9'在偏移576
            uint8_t digit = ascii_char - '0';
            offset = digit * font_info->height;  // height = 64字节/字符（在g_font_info中设置）
            pixel_height = 32;  // 32像素高
        } else {
            // 非数字字符不支持，直接返回
            return;
        }
    } else {
        // 其他字体：ASCII字符从32开始（空格）
        if (ascii_char < 32) {
            ascii_char = 32;  // 转换为空格
        }
        offset = (ascii_char - 32) * font_info->height;  // 每个ASCII字符的字节数
        pixel_height = font_info->height;  // 对于16像素以下字体，字节数 = 像素高度
    }
    
    Display_SendCharData(page, column, &font_info->ascii_font[offset], font_info->width, pixel_height);
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
    Display_ShowStringInternal(0, 0, "IMAGE TEST", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    
    // 显示开机Logo图片 (128x64像素)
    Display_ShowImageInternal(0, 14, 128, 64, LOGO_STARTUP_IMAGE);
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
    Display_ShowStringInternal(0, 0, "CHINESE TEST", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    
    // 显示中文字符测试
    Display_ShowStringInternal(0, 14, "Hello World", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 28, "NPWT System", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 42, "Test Mode", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 56, "Press Keys", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
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
    Display_ShowStringInternal(0, 0, "ENGLISH TEST", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    
    // 显示英文字符测试
    Display_ShowStringInternal(0, 14, "ABCDEFGHIJK", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 28, "LMNOPQRSTUV", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 42, "WXYZ0123456", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    Display_ShowStringInternal(0, 56, "789!@#$%^&*", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
}