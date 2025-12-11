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
    // TEXT_ID_SETTINGS
    {"Settings", "设置", "Настройки"},
    
    // TEXT_ID_THERAPY
    {"Therapy", "治疗", "Терапия"},
    
    // TEXT_ID_MODE
    {"Mode", "模式", "Режим"},
    
    // TEXT_ID_SWITCH
    {"Switch", "切换", "Переключить"},
    
    // TEXT_ID_CONTINUOUS
    {"Continuous", "连续", "Непрерывный"},
    
    // TEXT_ID_INTERMITTENT
    {"Intermittent", "间歇", "Прерывистый"},
    
    // TEXT_ID_PRESSURE
    {"Pressure", "压力", "Давление"},
    
    // TEXT_ID_HP_SET
    {"HP Set: -", "高压设置: -", "ВП Уст: -"},
    
    // TEXT_ID_LP_SET
    {"LP Set: -", "低压设置: -", "НП Уст: -"},
    
    // TEXT_ID_HP_TIME
    {"HP Time:", "高压时间:", "ВП Время:"},
    
    // TEXT_ID_LP_TIME
    {"LP Time:", "低压时间:", "НП Время:"},
    
    // TEXT_ID_THERAPY_ON
    {"Therapy On", "治疗中", "Терапия Вкл"},
    
    // TEXT_ID_THERAPY_OFF
    {"Therapy Off", "治疗关闭", "Терапия Выкл"},
    
    // TEXT_ID_HIGH_PHASE
    {"High Phase", "高压阶段", "Высокая фаза"},
    
    // TEXT_ID_MMHG
    {"mmHg", "mmHg", "мм рт.ст."},
    
    // TEXT_ID_MIN
    {"min", "分钟", "мин"}
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

