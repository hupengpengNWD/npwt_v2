/****************************************************************************
 * 文件名: app_pressure.h
 * 功能: 压力传感器管理应用层
 * 
 * 说明: 
 *   压力传感器ADC采集和管理
 *   - ADC原始值读取和滤波（3次采样平均）
 *   - ADC值到mmHg的转换
 *   - 零点校准功能
 *   - 压力值获取接口
 * 
 * 参考未重构工程实现：
 *   - adc.c: adc_press_filter() - 3次采样平均
 *   - npwt_dis_ofile_lcd_02.c: DISP_MainA() - ADC到mmHg转换
 *   - 转换公式：压力值(mmHg) = (adc_ps0 - adc_zero) / valueK
 *   - valueK默认值：2.75（参考未重构工程）
 * 
 * 创建日期: 2025-01-XX
 ****************************************************************************/

#ifndef APP_PRESSURE_H
#define APP_PRESSURE_H

#include "../../Core/Inc/system_config.h"
#include "../../Core/Inc/system_types.h"
#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 压力转换系数定义（参考未重构工程）
 ****************************************************************************/
#define PRESSURE_CONVERSION_FACTOR_DEFAULT    2.75f  // 默认转换系数（valueK）

/****************************************************************************
 * 压力管理接口函数
 ****************************************************************************/

/**
 * @name      AppPressure_Init
 * @brief     初始化压力管理模块
 * @param     无
 * @retval    无
 * @note      初始化滤波缓冲区和转换系数
 */
void AppPressure_Init(void);

/**
 * @name      AppPressure_Process
 * @brief     压力管理处理函数（在定时器中调用，用于周期性处理）
 * @param     user_data - 用户数据（定时器回调参数，未使用）
 * @retval    无
 * @note      当前主要用于滤波处理，可扩展其他逻辑
 */
void AppPressure_Process(void* user_data);

/**
 * @name      AppPressure_UpdateADC
 * @brief     更新压力ADC值（由ADC采集回调调用）
 * @param     adc_value - ADC采样值（10位：0-1023）
 * @retval    无
 * @note      每次ADC采样后调用，内部进行3次采样平均滤波
 */
void AppPressure_UpdateADC(uint16_t adc_value);

/**
 * @name      AppPressure_GetPressureValue
 * @brief     获取当前压力值（mmHg）
 * @param     无
 * @retval    压力值（mmHg，单位：毫米汞柱）
 * @note      返回经过滤波和转换后的压力值
 */
uint16_t AppPressure_GetPressureValue(void);

/**
 * @name      AppPressure_GetADCValue
 * @brief     获取压力传感器ADC原始值（滤波后）
 * @param     无
 * @retval    ADC采样值（10位：0-1023）
 * @note      返回经过3次采样平均滤波后的ADC值
 */
uint16_t AppPressure_GetADCValue(void);

/**
 * @name      AppPressure_GetRawADCValue
 * @brief     获取压力传感器ADC原始值（未滤波，最后一次采样值）
 * @param     无
 * @retval    ADC采样值（10位：0-1023）
 * @note      用于调试或特殊应用
 */
uint16_t AppPressure_GetRawADCValue(void);

/**
 * @name      AppPressure_CalibrateZero
 * @brief     校准压力传感器零点
 * @param     无
 * @retval    无
 * @note      将当前ADC值作为零点偏移值（adc_zero）
 *           应在系统启动时或校准操作时调用
 */
void AppPressure_CalibrateZero(void);

/**
 * @name      AppPressure_SetConversionFactor
 * @brief     设置ADC到mmHg的转换系数（valueK）
 * @param     factor - 转换系数（默认2.75）
 * @retval    无
 * @note      转换公式：压力值(mmHg) = (adc_ps0 - adc_zero) / factor
 */
void AppPressure_SetConversionFactor(float factor);

/**
 * @name      AppPressure_GetConversionFactor
 * @brief     获取当前转换系数
 * @param     无
 * @retval    转换系数（float）
 */
float AppPressure_GetConversionFactor(void);

/**
 * @name      AppPressure_GetZeroOffset
 * @brief     获取零点偏移值（adc_zero）
 * @param     无
 * @retval    零点偏移ADC值（10位：0-1023）
 */
uint16_t AppPressure_GetZeroOffset(void);

#endif /* APP_PRESSURE_H */

