/****************************************************************************
 * 文件名: app_language.h
 * 功能: 应用层多语言管理模块
 * 
 * 说明: 
 *   管理系统的多语言支持，根据语言变量值显示对应语言的文本
 *   支持语言：1=英文, 2=中文, 3=俄文（默认英文）
 * 
 * 创建日期: 2025-01-27
 ****************************************************************************/

#ifndef APP_LANGUAGE_H
#define APP_LANGUAGE_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 语言类型定义
 ****************************************************************************/

/**
 * @brief 语言类型枚举
 */
typedef enum {
    LANGUAGE_ENGLISH = 1,    // 英文
    LANGUAGE_CHINESE = 2,    // 中文
    LANGUAGE_RUSSIAN = 3     // 俄文
} LanguageType_e;

/****************************************************************************
 * 文本ID定义（用于多语言文本映射）
 ****************************************************************************/

/**
 * @brief 文本ID枚举
 * @note  每个界面或功能模块的文本都有唯一的ID
 */
typedef enum {
    // 待机界面
    TEXT_ID_SETTINGS = 0,
    TEXT_ID_THERAPY,
    
    // 设置界面
    TEXT_ID_MODE,
    TEXT_ID_SWITCH,
    TEXT_ID_CONTINUOUS,
    TEXT_ID_INTERMITTENT,
    
    // 压力设置界面
    TEXT_ID_PRESSURE,
    TEXT_ID_HP_SET,
    TEXT_ID_LP_SET,
    
    // 时间设置界面
    TEXT_ID_HP_TIME,
    TEXT_ID_LP_TIME,
    
    // 工作模式界面
    TEXT_ID_THERAPY_ON,
    TEXT_ID_THERAPY_OFF,
    TEXT_ID_HIGH_PHASE,
    
    // 单位
    TEXT_ID_MMHG,
    TEXT_ID_MIN,
    
    // 文本ID总数（用于边界检查）
    TEXT_ID_COUNT
} TextID_e;

/****************************************************************************
 * 公共接口声明
 ****************************************************************************/

/**
 * @name      AppLanguage_Init
 * @brief     初始化语言管理模块
 * @param     无
 * @retval    无
 * @note      在系统启动时调用，会从设置中加载语言配置
 */
void AppLanguage_Init(void);

/**
 * @name      AppLanguage_GetCurrent
 * @brief     获取当前语言类型
 * @param     无
 * @retval    LanguageType_e - 当前语言类型
 */
LanguageType_e AppLanguage_GetCurrent(void);

/**
 * @name      AppLanguage_SetCurrent
 * @brief     设置当前语言类型
 * @param     language - 语言类型
 * @retval    无
 * @note      设置后需要保存到Flash才能持久化
 */
void AppLanguage_SetCurrent(LanguageType_e language);

/**
 * @name      AppLanguage_GetText
 * @brief     根据文本ID和当前语言获取对应的文本
 * @param     text_id - 文本ID
 * @retval    const char* - 文本字符串指针
 * @note      如果文本ID无效或该语言不支持，返回英文文本
 */
const char* AppLanguage_GetText(TextID_e text_id);

/**
 * @name      AppLanguage_GetTextByLanguage
 * @brief     根据文本ID和指定语言获取对应的文本
 * @param     text_id - 文本ID
 * @param     language - 语言类型
 * @retval    const char* - 文本字符串指针
 * @note      如果文本ID无效或该语言不支持，返回英文文本
 */
const char* AppLanguage_GetTextByLanguage(TextID_e text_id, LanguageType_e language);

#endif /* APP_LANGUAGE_H */

