/****************************************************************************
 * 文件名: app_settings.h
 * 功能: 应用层参数保存/加载管理
 * 
 * 说明: 
 *   管理所有需要保存到Flash的用户参数和历史数据
 *   提供统一的保存/加载接口
 * 
 * 保存的参数:
 *   1. 工作模式参数（连续/间歇）
 *   2. 压力设置（连续模式目标压力、间歇模式高压/低压）
 *   3. 时间设置（间歇模式高压/低压时间）
 * 
 * 创建日期: 2025-11-21
 ****************************************************************************/

#ifndef APP_SETTINGS_H
#define APP_SETTINGS_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_types.h"
#include "../../Application/Inc/app_ui.h"

/****************************************************************************
 * 参数结构体定义
 ****************************************************************************/

/**
 * @brief Flash保存的参数结构体
 * @note  总大小：约14字节，预留空间到64字节（一个Flash块）
 */
typedef struct {
    /* 工作模式参数（6字节） */
    uint16_t work_mode;              // 上次工作模式（UI_STATE_LIX/UI_STATE_JIX）
    uint16_t pressure_high;          // 高压值（mmHg，连续模式使用，间歇模式也使用）
    uint16_t pressure_low;           // 低压值（mmHg，仅间歇模式使用）
    
    /* 间歇模式时间参数（4字节） */
    uint16_t time_high;              // 高压时间（分钟，仅间歇模式使用）
    uint16_t time_low;               // 低压时间（分钟，仅间歇模式使用）
    
    /* 系统设置参数（2字节） */
    uint16_t language;               // 语言设置（1=英文, 2=中文, 3=俄文，默认1）
    
    /* 预留字段（可选，用于未来扩展） */
    uint16_t reserved[28];           // 预留字段，使结构体大小为64字节（一个Flash块）
} AppSettingsFlashData_t;

/****************************************************************************
 * 公共接口声明
 ****************************************************************************/

/**
 * @name      AppSettings_Init
 * @brief     初始化参数保存模块
 * @param     无
 * @retval    无
 * @note      在系统启动时调用，会尝试从Flash加载参数
 */
void AppSettings_Init(void);

/**
 * @name      AppSettings_Load
 * @brief     从Flash加载参数
 * @param     无
 * @retval    true=加载成功, false=加载失败（使用默认值）
 * @note      如果Flash数据无效，会使用默认值
 */
bool AppSettings_Load(void);

/**
 * @name      AppSettings_Save
 * @brief     保存参数到Flash
 * @param     无
 * @retval    true=保存成功, false=保存失败
 * @note      保存前会擦除Flash块，然后写入数据
 *           建议在关机前或参数修改后调用
 */
bool AppSettings_Save(void);

/**
 * @name      AppSettings_UpdateFromUI
 * @brief     从UI上下文更新参数（准备保存）
 * @param     ui_context - UI上下文指针
 * @retval    无
 * @note      将UI中的参数值更新到内部结构体，但不立即保存
 *           需要调用AppSettings_Save()才会真正写入Flash
 */
void AppSettings_UpdateFromUI(const UIContext_t* ui_context);

/**
 * @name      AppSettings_ApplyToUI
 * @brief     将保存的参数应用到UI上下文
 * @param     ui_context - UI上下文指针
 * @retval    无
 * @note      从内部结构体恢复参数值到UI上下文
 *           在系统启动时调用，恢复上次的设置
 */
void AppSettings_ApplyToUI(UIContext_t* ui_context);

/**
 * @name      AppSettings_GetWorkMode
 * @brief     获取保存的工作模式
 * @param     无
 * @retval    UIState_e - 工作模式（UI_STATE_LIX或UI_STATE_JIX）
 */
// UIState_e AppSettings_GetWorkMode(void);

/**
 * @name      AppSettings_GetPressureHigh
 * @brief     获取保存的高压值
 * @param     无
 * @retval    uint16_t - 高压值（mmHg）
 */
// uint16_t AppSettings_GetPressureHigh(void);

/**
 * @name      AppSettings_GetPressureLow
 * @brief     获取保存的低压值
 * @param     无
 * @retval    uint16_t - 低压值（mmHg）
 */
// uint16_t AppSettings_GetPressureLow(void);

/**
 * @name      AppSettings_GetTimeHigh
 * @brief     获取保存的高压时间
 * @param     无
 * @retval    uint16_t - 高压时间（分钟）
 */
// uint16_t AppSettings_GetTimeHigh(void);

/**
 * @name      AppSettings_GetTimeLow
 * @brief     获取保存的低压时间
 * @param     无
 * @retval    uint16_t - 低压时间（分钟）
 */
// uint16_t AppSettings_GetTimeLow(void);

/**
 * @name      AppSettings_ResetToDefaults
 * @brief     重置所有参数为默认值
 * @param     无
 * @retval    无
 * @note      用于恢复出厂设置或初始化新设备
 */
// void AppSettings_ResetToDefaults(void);

/**
 * @name      AppSettings_GetLanguage
 * @brief     获取保存的语言设置
 * @param     无
 * @retval    uint16_t - 语言值（1=英文, 2=中文, 3=俄文）
 */
uint16_t AppSettings_GetLanguage(void);

/**
 * @name      AppSettings_SetLanguage
 * @brief     设置语言并保存
 * @param     language - 语言值（1=英文, 2=中文, 3=俄文）
 * @retval    bool - true=设置成功, false=设置失败
 * @note      设置后需要调用AppSettings_Save()才能持久化到Flash
 */
bool AppSettings_SetLanguage(uint16_t language);

#endif /* APP_SETTINGS_H */

