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

typedef enum {
    APP_PRESSURE_CONTROL_MODE_CONTINUOUS = 0,
    APP_PRESSURE_CONTROL_MODE_INTERMITTENT_HIGH,
    APP_PRESSURE_CONTROL_MODE_INTERMITTENT_LOW
} AppPressureControlMode_e;

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

void AppPressure_StartControl(uint16_t target_mmHg, AppPressureControlMode_e mode);
void AppPressure_StopControl(void);
void AppPressure_UpdateTarget(uint16_t target_mmHg);
bool AppPressure_IsControlEnabled(void);
/**
 * @name      AppPressure_IsMotorRunning
 * @brief     查询电机是否实际正在运行
 * @retval    true=电机正在运行, false=电机已停止
 * @note      检查PID控制是否启用、是否处于保持状态、PID输出是否大于0
 *           用于判断是否有实际负载，以决定电池电量显示的补偿策略
 */
bool AppPressure_IsMotorRunning(void);
bool AppPressure_HasControlFault(void);
void AppPressure_ClearControlFault(void);

/**
 * @name      AppPressure_BleedAndCalibrateZero
 * @brief     开机放气并在放气结束后执行零点校准
 * @note      打开电磁阀约3秒后关闭，再触发一次零点校准
 */
void AppPressure_BleedAndCalibrateZero(void);

/**
 * @name      AppPressure_StartIntermittentTherapy
 * @brief     启动间歇治疗循环（高压→低压循环）
 * @param     high_target_mmHg 高压目标（mmHg）
 * @param     low_target_mmHg  低压目标（mmHg）
 * @param     high_time_min    高压维持时间（分钟）
 * @param     low_time_min     低压维持时间（分钟）
 */
void AppPressure_StartIntermittentTherapy(uint16_t high_target_mmHg,
                                          uint16_t low_target_mmHg,
                                          uint16_t high_time_min,
                                          uint16_t low_time_min);

/**
 * @name      AppPressure_StopIntermittentTherapy
 * @brief     停止间歇治疗循环并关闭闭环控制
 */
void AppPressure_StopIntermittentTherapy(void);

/**
 * @name      AppPressure_GetCurrentTarget
 * @brief     获取当前闭环控制的用户目标（mmHg）
 */
uint16_t AppPressure_GetCurrentTarget(void);

/**
 * @name      AppPressure_GetCurrentMode
 * @brief     获取当前闭环控制模式（连续/间歇高压/间歇低压）
 */
AppPressureControlMode_e AppPressure_GetCurrentMode(void);

/**
 * @name      AppPressure_IsBleeding
 * @brief     查询是否处于泄气阶段（阀门打开中）
 * @retval    true=泄气中, false=未泄气
 */
bool AppPressure_IsBleeding(void);

#endif /* APP_PRESSURE_H */

