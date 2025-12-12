#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdint.h>
#include <stdbool.h>
#include "queue.h"

/****************************************************************************
 * 字体精简配置
 ****************************************************************************/

/**
 * @brief 启用字体数组精简功能
 * @note  启用后，只保留实际使用的字符，可节省约1.61 KB Flash空间
 *        禁用后，使用完整的ASCII字符集（0x20-0x7E）
 * @def   默认值：1（启用）
 */
#ifndef DISPLAY_FONT_COMPACT_ENABLE
#define DISPLAY_FONT_COMPACT_ENABLE 1
#endif

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
    DISPLAY_FONT_16X16 = 1,   // 16x16字体 (中文字符)
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
    DISPLAY_EVENT_SHOW_STARTUP_INTERFACE = 10, // 显示开机界面
    DISPLAY_EVENT_SHOW_ICON = 11,         // 显示图标
    DISPLAY_EVENT_CLEAR_RECT = 12,        // 局部清除
    DISPLAY_EVENT_SHOW_STRING_INVERT = 13 // 显示字符串（反转）
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
 * @name      Display_ClearQueue
 * @brief     清空显示队列（清除所有未处理的显示事件）
 * @param     无
 * @retval    无
 */
void Display_ClearQueue(void);

/**
 * @name      Display_Clear
 * @brief     清屏
 * @param     无
 * @retval    无
 */
void Display_Clear(void);

/**
 * @name      Display_ClearRect
 * @brief     清除LCD矩形区域
 * @param     x - 起始列坐标（软件坐标，0-127，0=左侧）
 * @param     y - 起始页坐标（软件坐标，0-7，0=顶部）
 * @param     width - 区域宽度（列数，1-128）
 * @param     height - 区域高度（像素数，1-64）
 * @retval    无
 */
void Display_ClearRect(uint8_t x, uint8_t y, uint8_t width, uint8_t height);

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
 * @name      Display_ShowStringInvert
 * @brief     显示字符串（反转显示）
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     str - 字符串
 * @param     font - 字体类型
 * @param     align - 对齐方式
 * @retval    无
 * @note      用于设置界面中当前编辑项的显示，实现反转效果
 */
void Display_ShowStringInvert(uint8_t x, uint8_t y, const char* str, DisplayFontType_e font, DisplayAlignType_e align);

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

/****************************************************************************
 * 图标数据结构
 ****************************************************************************/

/**
 * @brief 图标数据信息结构体
 */
typedef struct {
    uint8_t width;                      // 图标宽度（像素）
    uint8_t height;                     // 图标高度（像素）
    const unsigned char* data;          // 图标数据指针
} IconData_t;

/****************************************************************************
 * 图标类型枚举
 ****************************************************************************/

/**
 * @brief 图标类型枚举
 */
typedef enum {
    ICON_KEY1 = 0,              // 按键图标1（16x16）- key2015的上半部分
    ICON_KEY2,                  // 按键图标2（16x16）- key2015的下半部分
    ICON_CONTINUOUS,            // 连续模式图标（24x16）
    ICON_INTERMITTENT,          // 间歇模式图标（24x16）
    ICON_SILENT,                // 静音图标（16x16）
    ICON_LOCK,                  // 锁定图标（8x16）
    ICON_UNLOCK,                // 解锁图标（8x16，与锁定图标相同但用于清除）
    ICON_TICK,                  // 勾号图标（16x16）
    ICON_BAT0,                  // 电池图标0%（24x16）
    ICON_BAT1,                  // 电池图标25%（24x16）
    ICON_BAT2,                  // 电池图标50%（24x16）
    ICON_BAT3,                  // 电池图标75%（24x16）
    ICON_BAT4,                  // 电池图标100%（24x16）
    ICON_COUNT                  // 图标总数
} IconType_e;

/**
 * @name      Display_ShowIcon
 * @brief     显示图标
 * @param     x - X坐标（列）
 * @param     y - Y坐标（页）
 * @param     icon_type - 图标类型（IconType_e枚举）
 * @retval    无
 */
void Display_ShowIcon(uint8_t x, uint8_t y, uint8_t icon_type);

/**
 * @name      Display_ShowPressure
 * @brief     显示压力值
 * @param     x - X坐标
 * @param     y - Y坐标
 * @param     pressure - 压力值
 * @param     show_unit - 是否显示单位
 * @param     font - 字体类型（可选，默认使用DISPLAY_FONT_16X16）
 * @retval    无
 */
void Display_ShowPressure(uint8_t x, uint8_t y, uint16_t pressure, bool show_unit, DisplayFontType_e font);

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
//void Display_ShowImageTest(void);

/**
 * @name      Display_ShowImageStartupFormat
 * @brief     显示标准格式（行优先、上到下、左到右）的启动 LOGO 或全屏图片
 * @param     x - X坐标（列）
 * @param     y - Y坐标（页）
 * @param     width - 图片宽度（像素）
 * @param     height - 图片高度（像素）
 * @param     image_data - 图片数据指针
 * @retval    无
 * @note      用于显示128x64全屏图片，如启动LOGO或报警图标
 */
void Display_ShowImageStartupFormat(uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t* image_data);

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
void Display_ClearIconArea(uint8_t x, uint8_t y, IconType_e icon_type);

/**
 * @name      Display_ShowChineseTest
 * @brief     显示中文测试界面
 * @param     无
 * @retval    无
 */
//void Display_ShowChineseTest(void);

/**
 * @name      Display_ShowEnglishTest
 * @brief     显示英文测试界面
 * @param     无
 * @retval    无
 */
//void Display_ShowEnglishTest(void);


#endif /* __DISPLAY_H__ */