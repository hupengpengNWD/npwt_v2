/****************************************************************************
 * 文件名: app_settings.c
 * 功能: 应用层参数保存/加载管理实现
 * 
 * 说明: 
 *   实现参数保存到Flash和从Flash加载的功能
 *   提供与UI模块和压力模块的接口
 * 
 * 创建日期: 2025-11-21
 ****************************************************************************/

#include "../Inc/app_settings.h"
#include "../../HAL/Inc/hal_flash.h"
#include "../../Core/Inc/system_config.h"
#include <string.h>

/****************************************************************************
 * 内部变量定义
 ****************************************************************************/

/* 参数数据缓存（RAM中） */
static AppSettingsFlashData_t g_settings_data;

/* 初始化标志 */
static bool g_settings_initialized = false;

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/

/**
 * @brief 验证参数有效性
 */
static bool AppSettings_ValidateData(const AppSettingsFlashData_t* data);

/**
 * @brief 设置默认值
 */
static void AppSettings_SetDefaults(void);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @brief 验证参数有效性
 */
static bool AppSettings_ValidateData(const AppSettingsFlashData_t* data)
{
    if (data == NULL) {
        return false;
    }
    
    /* 验证工作模式 */
    if (data->work_mode != UI_STATE_LIX && data->work_mode != UI_STATE_JIX) {
        return false;
    }
    
    /* 验证压力值范围 */
    if (data->pressure_high < PRESSURE_MIN || data->pressure_high > PRESSURE_MAX) {
        return false;
    }
    
    if (data->pressure_low < PRESSURE_MIN || data->pressure_low > PRESSURE_MAX) {
        return false;
    }
    
    if (data->pressure_low >= data->pressure_high) {
        return false;  // 低压必须小于高压
    }
    
    /* 验证时间值范围（0-99分钟） */
    if (data->time_high > 99 || data->time_low > 99) {
        return false;
    }
    
    /* 验证语言值范围（1=英文, 2=中文, 3=俄文） */
    if (data->language < 1 || data->language > 3) {
        return false;
    }
    
    return true;
}

/**
 * @brief 设置默认值
 */
static void AppSettings_SetDefaults(void)
{
    g_settings_data.work_mode = UI_STATE_LIX;              // 默认连续模式
    g_settings_data.pressure_high = PRESSURE_DEFAULT;      // 默认120mmHg
    g_settings_data.pressure_low = 80;                     // 默认80mmHg（间歇模式）
    g_settings_data.time_high = 5;                         // 默认5分钟
    g_settings_data.time_low = 2;                          // 默认2分钟
    g_settings_data.language = 1;                          // 默认英文（1=英文, 2=中文, 3=俄文）
    
    /* 清零预留字段 */
    memset(g_settings_data.reserved, 0, sizeof(g_settings_data.reserved));
}

/**
 * @brief 初始化参数保存模块
 */
void AppSettings_Init(void)
{
    if (g_settings_initialized) {
        return;
    }
    
    /* 初始化Flash模块 */
    HAL_Flash_Init();
    
    /* 尝试从Flash加载参数 */
//    if (!AppSettings_Load()) {
        /* 加载失败，使用默认值 */
        AppSettings_SetDefaults();
//    }
    
    g_settings_initialized = true;
}

/**
 * @brief 从Flash加载参数
 * @note  与未重构工程保持一致，只读取20字节（10个word）
 */
bool AppSettings_Load(void)
{
    AppSettingsFlashData_t temp_data;
    
    /* 先清零整个结构体，确保reserved字段为0 */
    memset(&temp_data, 0, sizeof(temp_data));
    
    /* 按照未重构工程的格式，逐个读取10个word（20字节） */
    temp_data.pressure_high = HAL_Flash_ReadWord(HAL_FLASH_CONFIG_ADDRESS + 0);
    temp_data.time_high     = HAL_Flash_ReadWord(HAL_FLASH_CONFIG_ADDRESS + 2);
    temp_data.time_low      = HAL_Flash_ReadWord(HAL_FLASH_CONFIG_ADDRESS + 4);
    temp_data.work_mode     = HAL_Flash_ReadWord(HAL_FLASH_CONFIG_ADDRESS + 6);
    /* addr+8, addr+10, addr+12, addr+14: 校准系数K1-K4，不需要读取 */
    /* addr+16: 静音标志+语言，不需要读取 */
    temp_data.pressure_low  = HAL_Flash_ReadWord(HAL_FLASH_CONFIG_ADDRESS + 18);
    
    /* 验证数据有效性 */
    if (!AppSettings_ValidateData(&temp_data)) {
        return false;  // 数据无效
    }
    
    /* 数据有效，复制到内部缓存 */
    memcpy(&g_settings_data, &temp_data, sizeof(g_settings_data));
    
    return true;
}

/**
 * @brief 保存参数到Flash
 * @note  与未重构工程保持一致，只写入20字节（10个word），不进行CRC校验
 * 
 * 数据格式（与未重构工程一致）:
 *   addr+0:  pressure_high  (高压值，对应未重构工程的mod_seta_preh_bak)
 *   addr+2:  time_high      (高压时间，对应未重构工程的mod_seta_ont)
 *   addr+4:  time_low       (低压时间，对应未重构工程的mod_seta_oft)
 *   addr+6:  work_mode      (工作模式，对应未重构工程的mod_main_baka)
 *   addr+8:  0              (校准系数K1，未重构工程使用，重构工程不需要)
 *   addr+10: 0              (校准系数K2，未重构工程使用，重构工程不需要)
 *   addr+12: 0              (校准系数K3，未重构工程使用，重构工程不需要)
 *   addr+14: 0              (校准系数K4，未重构工程使用，重构工程不需要)
 *   addr+16: 0              (静音标志+语言，未重构工程使用，重构工程不需要)
 *   addr+18: pressure_low   (低压值，对应未重构工程的mod_seta_prel)
 */
bool AppSettings_Save(void)
{
    /* 检查Flash是否需要擦除：读取第一个word，如果是0xFFFF则不需要擦除 */
    unsigned int first_word = HAL_Flash_ReadWord(HAL_FLASH_CONFIG_ADDRESS);
    
    /* 如果Flash不是0xFFFF（已擦除状态），需要先擦除 */
    if (first_word != 0xFFFF) {
        /* 擦除Flash块 */
        if (!HAL_Flash_EraseBlock(HAL_FLASH_CONFIG_ADDRESS)) {
            return false;
        }
    }
    
    /* 按照未重构工程的格式，逐个写入10个word（20字节） */
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 0,  g_settings_data.pressure_high)) {
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 2,  g_settings_data.time_high)) {
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 4,  g_settings_data.time_low)) {
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 6,  g_settings_data.work_mode)) {
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 8,  0)) {  // 校准系数K1，不需要
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 10, 0)) {  // 校准系数K2，不需要
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 12, 0)) {  // 校准系数K3，不需要
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 14, 0)) {  // 校准系数K4，不需要
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 16, 0)) {  // 静音标志+语言，不需要
        return false;
    }
    if (!HAL_Flash_WriteWord(HAL_FLASH_CONFIG_ADDRESS + 18, g_settings_data.pressure_low)) {
        return false;
    }
    
    return true;
}

/**
 * @brief 从UI上下文更新参数
 */
void AppSettings_UpdateFromUI(const UIContext_t* ui_context)
{
    if (ui_context == NULL) {
        return;
    }
    
    /* 如果g_settings_data未初始化，先初始化默认值 */
    if (!g_settings_initialized) {
        AppSettings_SetDefaults();
    }
    
    /* 更新工作模式 */
    if (ui_context->work_mode_backup == UI_STATE_LIX || ui_context->work_mode_backup == UI_STATE_JIX) {
        g_settings_data.work_mode = ui_context->work_mode_backup;
    }
    
    /* 更新压力值 */
    g_settings_data.pressure_high = ui_context->pressure_high;
    g_settings_data.pressure_low = ui_context->pressure_low;
    
    /* 更新时间值 */
    g_settings_data.time_high = ui_context->time_high;
    g_settings_data.time_low = ui_context->time_low;
}


/**
 * @brief 将保存的参数应用到UI上下文
 */
void AppSettings_ApplyToUI(UIContext_t* ui_context)
{
    if (ui_context == NULL) {
        return;
    }
    
    /* 恢复工作模式 */
    if (g_settings_data.work_mode == UI_STATE_LIX || g_settings_data.work_mode == UI_STATE_JIX) {
        ui_context->work_mode_backup = g_settings_data.work_mode;
    }
    
    /* 恢复压力值 */
    ui_context->pressure_high = g_settings_data.pressure_high;
    ui_context->pressure_low = g_settings_data.pressure_low;
    
    /* 恢复时间值 */
    ui_context->time_high = g_settings_data.time_high;
    ui_context->time_low = g_settings_data.time_low;
}


/**
 * @brief 获取保存的工作模式
 */
UIState_e AppSettings_GetWorkMode(void)
{
    return (UIState_e)g_settings_data.work_mode;
}

/**
 * @brief 获取保存的高压值
 */
uint16_t AppSettings_GetPressureHigh(void)
{
    return g_settings_data.pressure_high;
}

/**
 * @brief 获取保存的低压值
 */
uint16_t AppSettings_GetPressureLow(void)
{
    return g_settings_data.pressure_low;
}

/**
 * @brief 获取保存的高压时间
 */
uint16_t AppSettings_GetTimeHigh(void)
{
    return g_settings_data.time_high;
}

/**
 * @brief 获取保存的低压时间
 */
uint16_t AppSettings_GetTimeLow(void)
{
    return g_settings_data.time_low;
}

/**
 * @brief 重置所有参数为默认值
 */
void AppSettings_ResetToDefaults(void)
{
    AppSettings_SetDefaults();
    /* 可以选择立即保存到Flash */
    /* AppSettings_Save(); */
}

/**
 * @brief 获取保存的语言设置
 */
uint16_t AppSettings_GetLanguage(void)
{
    return g_settings_data.language;
}

/**
 * @brief 设置语言并保存
 */
bool AppSettings_SetLanguage(uint16_t language)
{
    // 验证语言值范围（1=英文, 2=中文, 3=俄文）
    if (language < 1 || language > 3) {
        return false;
    }
    
    g_settings_data.language = language;
    return true;
}

