/****************************************************************************
 * 文件名: app_pressure.c
 * 功能: 压力传感器管理应用层实现
 * 
 * 说明: 
 *   参考未重构工程的实现方式
 *   - ADC采集频率：约每4ms（250Hz，由调用频率决定）
 *   - 滤波方式：3次采样滑动平均（参考adc_press_filter）
 *   - 转换公式：压力值(mmHg) = (adc_ps0 - adc_zero) / valueK
 *   - valueK默认值：2.75（参考未重构工程）
 * 
 * 实现细节：
 *   1. 3次采样平均滤波：每次UpdateADC时更新一个样本，计算平均值
 *   2. 零点校准：记录启动时的ADC值作为零点偏移
 *   3. ADC到mmHg转换：使用转换系数和零点偏移计算
 * 
 * 创建日期: 2025-01-XX
 ****************************************************************************/

#include "../Inc/app_pressure.h"
#include "../../HAL/Inc/hal_adc.h"
#include <stdbool.h>

/****************************************************************************
 * 内部常量定义
 ****************************************************************************/

#define PRESSURE_FILTER_COUNT      3   // 滤波采样次数（参考未重构工程COUNT_PS）

/****************************************************************************
 * 内部变量
 ****************************************************************************/

/* 滤波缓冲区（3次采样） */
static uint16_t g_pressure_filter_buf[PRESSURE_FILTER_COUNT];
static uint8_t g_pressure_filter_index = 0;  // 当前缓冲区索引

/* ADC原始值（滤波后） */
static uint16_t g_pressure_adc_filtered = 0;  // 滤波后的ADC值

/* ADC原始值（最后一次采样，未滤波） */
static uint16_t g_pressure_adc_raw = 0;  // 最后一次原始ADC值

/* 零点偏移值（adc_zero） */
static uint16_t g_pressure_zero_offset = 0;  // 零点偏移ADC值

/* 转换系数（valueK） */
static float g_pressure_conversion_factor = PRESSURE_CONVERSION_FACTOR_DEFAULT;  // 默认2.75

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/

/**
 * @name      PressureFilter_Update
 * @brief     更新滤波缓冲区并计算平均值
 * @param     adc_value - 新的ADC采样值
 * @retval    滤波后的ADC值
 */
static uint16_t PressureFilter_Update(uint16_t adc_value);

/**
 * @name      PressureConvert_ADCToMMHG
 * @brief     ADC值转换为mmHg
 * @param     adc_value - ADC值（滤波后）
 * @retval    压力值（mmHg）
 */
static uint16_t PressureConvert_ADCToMMHG(uint16_t adc_value);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      AppPressure_Init
 * @brief     初始化压力管理模块
 */
void AppPressure_Init(void)
{
    /* 初始化滤波缓冲区 */
    for (uint8_t i = 0; i < PRESSURE_FILTER_COUNT; i++) {
        g_pressure_filter_buf[i] = 0;
    }
    g_pressure_filter_index = 0;
    
    /* 初始化变量 */
    g_pressure_adc_filtered = 0;
    g_pressure_adc_raw = 0;
    g_pressure_zero_offset = 0;
    g_pressure_conversion_factor = PRESSURE_CONVERSION_FACTOR_DEFAULT;
}

/**
 * @name      AppPressure_Process
 * @brief     压力管理处理函数（在定时器中调用）
 * @param     user_data - 用户数据（定时器回调参数，未使用）
 * @note      当前主要用于扩展其他处理逻辑，滤波已在UpdateADC中完成
 */
void AppPressure_Process(void* user_data)
{
    (void)user_data;  // 未使用，消除警告
    
    /* 当前滤波处理已在UpdateADC中完成，此处可扩展其他逻辑 */
    /* 例如：显示层滤波、报警检测等 */
}

/**
 * @name      AppPressure_UpdateADC
 * @brief     更新压力ADC值（由ADC采集回调调用）
 * @param     adc_value - ADC采样值（10位：0-1023）
 * @note      每次ADC采样后调用，内部进行3次采样平均滤波
 */
void AppPressure_UpdateADC(uint16_t adc_value)
{
    /* 保存原始值 */
    g_pressure_adc_raw = adc_value;
    
    /* 更新滤波缓冲区并计算平均值 */
    g_pressure_adc_filtered = PressureFilter_Update(adc_value);
}

/**
 * @name      AppPressure_GetPressureValue
 * @brief     获取当前压力值（mmHg）
 * @retval    压力值（mmHg）
 */
uint16_t AppPressure_GetPressureValue(void)
{
    /* 将滤波后的ADC值转换为mmHg */
    return PressureConvert_ADCToMMHG(g_pressure_adc_filtered);
}

/**
 * @name      AppPressure_GetADCValue
 * @brief     获取压力传感器ADC原始值（滤波后）
 * @retval    ADC采样值（10位：0-1023）
 */
uint16_t AppPressure_GetADCValue(void)
{
    return g_pressure_adc_filtered;
}

/**
 * @name      AppPressure_GetRawADCValue
 * @brief     获取压力传感器ADC原始值（未滤波，最后一次采样值）
 * @retval    ADC采样值（10位：0-1023）
 */
uint16_t AppPressure_GetRawADCValue(void)
{
    return g_pressure_adc_raw;
}

/**
 * @name      AppPressure_CalibrateZero
 * @brief     校准压力传感器零点
 * @note      将当前滤波后的ADC值作为零点偏移值
 */
void AppPressure_CalibrateZero(void)
{
    /* 使用当前滤波后的ADC值作为零点偏移 */
    g_pressure_zero_offset = g_pressure_adc_filtered;
}

/**
 * @name      AppPressure_SetConversionFactor
 * @brief     设置ADC到mmHg的转换系数（valueK）
 * @param     factor - 转换系数（默认2.75）
 */
void AppPressure_SetConversionFactor(float factor)
{
    if (factor > 0.0f) {  // 有效性检查
        g_pressure_conversion_factor = factor;
    }
}

/**
 * @name      AppPressure_GetConversionFactor
 * @brief     获取当前转换系数
 * @retval    转换系数（float）
 */
float AppPressure_GetConversionFactor(void)
{
    return g_pressure_conversion_factor;
}

/**
 * @name      AppPressure_GetZeroOffset
 * @brief     获取零点偏移值（adc_zero）
 * @retval    零点偏移ADC值（10位：0-1023）
 */
uint16_t AppPressure_GetZeroOffset(void)
{
    return g_pressure_zero_offset;
}

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * @name      PressureFilter_Update
 * @brief     更新滤波缓冲区并计算平均值（参考未重构工程adc_press_filter）
 * @param     adc_value - 新的ADC采样值
 * @retval    滤波后的ADC值（3次采样平均）
 */
static uint16_t PressureFilter_Update(uint16_t adc_value)
{
    uint8_t count;
    uint32_t sum = 0;
    
    /* 更新缓冲区 */
    g_pressure_filter_buf[g_pressure_filter_index++] = adc_value;
    if (g_pressure_filter_index >= PRESSURE_FILTER_COUNT) {
        g_pressure_filter_index = 0;
    }
    
    /* 计算平均值 */
    for (count = 0; count < PRESSURE_FILTER_COUNT; count++) {
        sum += g_pressure_filter_buf[count];
    }
    sum = (uint16_t)(sum / PRESSURE_FILTER_COUNT);
    
    /* 边界检查（参考未重构工程） */
    if (sum > 1023) {
        sum = 1023;
    }
    if (sum < 1) {
        sum = 0;
    }
    
    return (uint16_t)sum;
}

/**
 * @name      PressureConvert_ADCToMMHG
 * @brief     ADC值转换为mmHg（参考未重构工程DISP_MainA）
 * @param     adc_value - ADC值（滤波后）
 * @retval    压力值（mmHg）
 * @note      转换公式：压力值(mmHg) = (adc_ps0 - adc_zero) / valueK
 */
static uint16_t PressureConvert_ADCToMMHG(uint16_t adc_value)
{
    float pressure_float;
    
    /* 参考未重构工程：if (adc_ps0 > adc_zero) */
    if (adc_value > g_pressure_zero_offset) {
        /* 转换公式：i = (float)(adc_ps0 - adc_zero) / valueK */
        pressure_float = (float)(adc_value - g_pressure_zero_offset) / g_pressure_conversion_factor;
    } else {
        /* 如果ADC值小于等于零点，返回0 */
        pressure_float = 0.0f;
    }
    
    /* 转换为整数（mmHg） */
    uint16_t pressure = (uint16_t)pressure_float;
    
    /* 边界检查：压力范围0-320mmHg（参考未重构工程注释） */
    if (pressure > 320) {
        pressure = 320;
    }
    
    return pressure;
}

