/****************************************************************************
 * 文件名: app_language.c
 * 功能: 应用层多语言管理模块实现
 * 
 * 创建日期: 2025-01-27
 ****************************************************************************/

#include "../Inc/app_language.h"
#include "../Inc/app_settings.h"
#include <string.h>

/****************************************************************************
 * UTF-8中文字符到字库索引的映射表
 * 
 * 字库字符索引：
 * 按(0) 设(1) 定(2) 参(3) 数(4) 开(5) 始(6) 治(7) 疗(8) 中(9) 机(10) 
 * 连(11) 续(12) 模(13) 式(14) 间(15) 歇(16) 切(17) 换(18) 高(19) 压(20)
 * 低(21) 时(22) 间(23) 运(24) 行(25) 停(26) 止(27)
 * 
 * 映射方式：使用UTF-8字符的3字节作为key，映射到字库索引
 ****************************************************************************/

/**
 * @brief UTF-8中文字符到字库索引的映射结构
 */
typedef struct {
    uint32_t utf8_code;  // UTF-8编码（3字节，高位对齐）
    uint8_t font_index;  // 字库索引
} ChineseCharMap_t;

/**
 * @brief UTF-8中文字符映射表
 * @note UTF-8中文字符编码范围：0xE4-0xE9开头
 */
static const ChineseCharMap_t g_chinese_char_map[] = {
    // 按(0) - 0xE6 0x8C 0x89
    {0x00E68C89, 0},
    // 设(1) - 0xE8 0xAE 0xBE
    {0x00E8AEBE, 1},
    // 定(2) - 0xE5 0xAE 0x9A
    {0x00E5AE9A, 2},
    // 参(3) - 0xE5 0x8F 0x82
    {0x00E58F82, 3},
    // 数(4) - 0xE6 0x95 0xB0
    {0x00E695B0, 4},
    // 开(5) - 0xE5 0xBC 0x80
    {0x00E5BC80, 5},
    // 始(6) - 0xE5 0xA7 0x8B
    {0x00E5A78B, 6},
    // 治(7) - 0xE6 0xB2 0xBB
    {0x00E6B2BB, 7},
    // 疗(8) - 0xE7 0x96 0x97
    {0x00E79697, 8},
    // 中(9) - 0xE4 0xB8 0xAD
    {0x00E4B8AD, 9},
    // 机(10) - 0xE6 0x9C 0xBA
    {0x00E69CBA, 10},
    // 连(11) - 0xE8 0xBF 0x9E
    {0x00E8BF9E, 11},
    // 续(12) - 0xE7 0xBB 0xAD
    {0x00E7BBAD, 12},
    // 模(13) - 0xE6 0xA8 0xA1
    {0x00E6A8A1, 13},
    // 式(14) - 0xE5 0xBC 0x8F
    {0x00E5BC8F, 14},
    // 间(15) - 0xE9 0x97 0xB4
    {0x00E997B4, 15},
    // 歇(16) - 0xE6 0xAD 0x87
    {0x00E6AD87, 16},
    // 切(17) - 0xE5 0x88 0x87
    {0x00E58887, 17},
    // 换(18) - 0xE6 0x8D 0xA2
    {0x00E68DA2, 18},
    // 高(19) - 0xE9 0xAB 0x98
    {0x00E9AB98, 19},
    // 压(20) - 0xE5 0x8E 0x8B
    {0x00E58E8B, 20},
    // 低(21) - 0xE4 0xBD 0x8E
    {0x00E4BD8E, 21},
    // 时(22) - 0xE6 0x97 0xB6
    {0x00E697B6, 22},
    // 力(23) - 0xE5 0x8A 0x9B
    {0x00E58A9B, 23},
    // 运(24) - 0xE8 0xBF 0x90
    {0x00E8BF90, 24},
    // 行(25) - 0xE8 0xA1 0x8C
    {0x00E8A18C, 25},
    // 停(26) - 0xE5 0x81 0x9C
    {0x00E5819C, 26},
    // 止(27) - 0xE6 0xAD 0xA2
    {0x00E6ADA2, 27},
};

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/

/**
 * @brief 查找UTF-8中文字符在映射表中的索引
 * @param utf8_bytes - UTF-8字符的3个字节
 * @return 字库索引，如果未找到返回0xFF
 */
static uint8_t AppLanguage_FindChineseCharIndex(const uint8_t* utf8_bytes);

/**
 * @brief 将UTF-8中文字符串转换为字库索引数组
 * @param utf8_str - UTF-8中文字符串
 * @param index_array - 输出的索引数组
 * @param max_len - 索引数组最大长度
 * @return 转换后的字符数量
 */
static uint8_t AppLanguage_ConvertChineseString(const char* utf8_str, uint8_t* index_array, uint8_t max_len);

#define CHINESE_CHAR_MAP_SIZE (sizeof(g_chinese_char_map) / sizeof(g_chinese_char_map[0]))

/****************************************************************************
 * 内部变量定义
 ****************************************************************************/

/* 当前语言类型（默认英文） */
static LanguageType_e g_current_language = LANGUAGE_ENGLISH;

/****************************************************************************
 * 多语言文本映射表
 ****************************************************************************/

/**
 * @brief 多语言文本映射表
 * @note  第一维：文本ID，第二维：语言类型（0=英文, 1=中文, 2=俄文）
 *        语言索引 = language - 1（因为LANGUAGE_ENGLISH=1）
 */
static const char* g_text_table[TEXT_ID_COUNT][3] = {
    // TEXT_ID_SETTINGS (0)
    {"Settings", "模式设定", "Настройки"},
    
    // TEXT_ID_THERAPY (1)
    {"Therapy", "开始治疗", "Терапия"},
    
    // TEXT_ID_MODE (2)
    {"Mode", "参数设定", "Режим"},
    
    // TEXT_ID_SWITCH (3)
    {"Switch", "切换", "Переключить"},
    
    // TEXT_ID_CONTINUOUS (4)
    {"Continuous", "连续模式", "Непрерывный"},
    
    // TEXT_ID_INTERMITTENT (5)
    {"Intermittent", "间歇模式", "Прерывистый"},
    
    // TEXT_ID_PRESSURE (6)
    {"Pressure", "压力设定", "Давление"},
    
    // TEXT_ID_HP_SET (7)
    {"HP Set: -", "高压压力", "ВП Уст: -"},
    
    // TEXT_ID_LP_SET (8)
    {"LP Set: -", "低压压力", "НП Уст: -"},
    
    // TEXT_ID_HP_TIME (9)
    {"HP Time:", "高压时间:", "ВП Время:"},
    
    // TEXT_ID_LP_TIME (10)
    {"LP Time:", "低压时间:", "НП Время:"},
    
    // TEXT_ID_THERAPY_ON (11)
    {"Therapy On", "治疗中", "Терапия Вкл"},
    
    // TEXT_ID_THERAPY_OFF (12)
    {"Therapy Off", "停机中", "Терапия Выкл"},
    
    // TEXT_ID_HIGH_PHASE (13)
    {"High Phase", "高压治疗", "Высокая фаза"},
    
    // TEXT_ID_LOW_PHASE (14)
    {"Low Phase", "低压治疗", "Низкая фаза"},
    
    // TEXT_ID_MMHG (15)
    {"mmHg", "mmHg", "мм рт.ст."},
    
    // TEXT_ID_MIN (16)
    {"min", "min", "мин"}
};

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      AppLanguage_Init
 * @brief     初始化语言管理模块
 */
void AppLanguage_Init(void)
{
    // 从设置模块获取语言配置
    uint16_t language_value = AppSettings_GetLanguage();
    
    // 将设置值转换为枚举类型（1=英文, 2=中文, 3=俄文）
    if (language_value >= LANGUAGE_ENGLISH && language_value <= LANGUAGE_RUSSIAN) {
        g_current_language = (LanguageType_e)language_value;
    } else {
        g_current_language = LANGUAGE_ENGLISH;  // 无效值，使用默认英文
    }
}

/**
 * @name      AppLanguage_GetCurrent
 * @brief     获取当前语言类型
 */
LanguageType_e AppLanguage_GetCurrent(void)
{
    return g_current_language;
}

/**
 * @name      AppLanguage_SetCurrent
 * @brief     设置当前语言类型
 */
void AppLanguage_SetCurrent(LanguageType_e language)
{
    // 验证语言类型有效性
    if (language >= LANGUAGE_ENGLISH && language <= LANGUAGE_RUSSIAN) {
        g_current_language = language;
    } else {
        g_current_language = LANGUAGE_ENGLISH;  // 无效值，使用默认英文
    }
}

/**
 * @name      AppLanguage_GetText
 * @brief     根据文本ID和当前语言获取对应的文本
 */
const char* AppLanguage_GetText(TextID_e text_id)
{
    return AppLanguage_GetTextByLanguage(text_id, g_current_language);
}

/**
 * @name      AppLanguage_GetTextByLanguage
 * @brief     根据文本ID和指定语言获取对应的文本
 */
const char* AppLanguage_GetTextByLanguage(TextID_e text_id, LanguageType_e language)
{
    // 验证文本ID有效性
    if (text_id >= TEXT_ID_COUNT) {
        return "";  // 无效ID，返回空字符串
    }
    
    // 验证语言类型有效性
    if (language < LANGUAGE_ENGLISH || language > LANGUAGE_RUSSIAN) {
        language = LANGUAGE_ENGLISH;  // 无效语言，使用英文
    }
    
    // 计算语言索引（LANGUAGE_ENGLISH=1 -> 索引0, LANGUAGE_CHINESE=2 -> 索引1, LANGUAGE_RUSSIAN=3 -> 索引2）
    uint8_t lang_index = (uint8_t)(language - 1);
    
    // 获取文本
    const char* text = g_text_table[text_id][lang_index];
    
    // 如果该语言的文本为空，返回英文文本
    if (text == NULL || strlen(text) == 0) {
        text = g_text_table[text_id][0];  // 英文文本（索引0）
    }
    
    return text;
}

/**
 * @name      AppLanguage_GetFontForText
 * @brief     根据文本ID和当前语言获取合适的字体
 * @param     text_id - 文本ID
 * @param     original_font - 原字体（英文/俄文时使用的字体）
 * @retval    DisplayFontType_e - 字体类型
 * @note      中文时：文本使用DISPLAY_FONT_16X16，单位保持原字体
 *            英文/俄文时：保持原字体
 */
DisplayFontType_e AppLanguage_GetFontForText(TextID_e text_id, DisplayFontType_e original_font)
{
    // 单位文本（mmHg, min）始终使用原字体，不受语言影响
    if (text_id == TEXT_ID_MMHG || text_id == TEXT_ID_MIN) {
        return original_font;  // 单位保持原字体
    }
    
    // 如果是中文，需要将不支持中文的字体改为16x16
    if (g_current_language == LANGUAGE_CHINESE) {
        // 6x12和8x16字体不支持中文，需要改为16x16
        if (original_font == DISPLAY_FONT_6X12 || original_font == DISPLAY_FONT_8X16) {
            return DISPLAY_FONT_16X16;
        }
    }
    
    // 其他情况保持原字体
    return original_font;
}

/**
 * @name      AppLanguage_FindChineseCharIndex
 * @brief     查找UTF-8中文字符在映射表中的索引
 */
static uint8_t AppLanguage_FindChineseCharIndex(const uint8_t* utf8_bytes)
{
    // 构建UTF-8编码值（3字节）
    uint32_t utf8_code = ((uint32_t)utf8_bytes[0] << 16) | 
                         ((uint32_t)utf8_bytes[1] << 8) | 
                         (uint32_t)utf8_bytes[2];
    
    // 在映射表中查找
    for (uint8_t i = 0; i < CHINESE_CHAR_MAP_SIZE; i++) {
        if (g_chinese_char_map[i].utf8_code == utf8_code) {
            return g_chinese_char_map[i].font_index;
        }
    }
    
    return 0xFF;  // 未找到
}

/**
 * @name      AppLanguage_ConvertChineseString
 * @brief     将UTF-8中文字符串转换为字库索引数组
 */
static uint8_t AppLanguage_ConvertChineseString(const char* utf8_str, uint8_t* index_array, uint8_t max_len)
{
    if (utf8_str == NULL || index_array == NULL || max_len == 0) {
        return 0;
    }
    
    uint8_t count = 0;
    const uint8_t* str = (const uint8_t*)utf8_str;
    
    while (*str != '\0' && count < max_len) {
        uint8_t ch = *str;
        
        // ASCII字符（0x20-0x7F）：直接使用原值
        if (ch >= 0x20 && ch <= 0x7F) {
            index_array[count++] = ch;
            str++;
        }
        // UTF-8中文字符（3字节，以0xE4-0xE9开头）
        else if (ch >= 0xE4 && ch <= 0xE9) {
            // 检查是否有足够的字节
            if (str[1] != 0 && str[2] != 0) {
                uint8_t font_index = AppLanguage_FindChineseCharIndex(str);
                if (font_index != 0xFF) {
                    // 找到映射，转换为字库索引（0x80 + 索引）
                    index_array[count++] = 0x80 + font_index;
                } else {
                    // 未找到映射，跳过该字符或使用占位符
                    // 这里使用空格作为占位符
                    index_array[count++] = 0x20;  // 空格
                }
                str += 3;  // UTF-8中文字符占3字节
            } else {
                // 字节不完整，跳过
                break;
            }
        }
        // 其他字符：跳过
        else {
            str++;
        }
    }
    
    return count;
}

/**
 * @name      AppLanguage_GetTextConverted
 * @brief     获取转换后的文本（中文时转换为字库索引数组）
 */
const char* AppLanguage_GetTextConverted(TextID_e text_id, char* buffer, uint8_t buffer_size)
{
    if (buffer == NULL || buffer_size == 0) {
        return "";
    }
    
    // 获取原始文本
    const char* original_text = AppLanguage_GetText(text_id);
    
    // 如果是中文，需要转换
    if (g_current_language == LANGUAGE_CHINESE) {
        uint8_t index_array[32];  // 临时索引数组
        uint8_t count = AppLanguage_ConvertChineseString(original_text, index_array, sizeof(index_array));
        
        // 将索引数组复制到缓冲区
        if (count < buffer_size) {
            for (uint8_t i = 0; i < count; i++) {
                buffer[i] = (char)index_array[i];
            }
            buffer[count] = '\0';
            return buffer;
        }
    }
    
    // 英文/俄文：直接返回原字符串
    return original_text;
}

