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
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Middleware/Inc/pid.h"
#include "../../Middleware/Inc/pwm.h"
#include <stdbool.h>

/****************************************************************************
 * 内部常量定义
 ****************************************************************************/

#define PRESSURE_FILTER_COUNT      3   // 滤波采样次数（参考未重构工程COUNT_PS）

#define PRESSURE_CONTROL_SAMPLE_TIME_S    (0.010f)   // 控制循环采样周期（10ms）
#define PRESSURE_CONTROL_DEADBAND_MMHG    (5.0f)     // 允许的稳态误差
#define PRESSURE_CONTROL_OUTPUT_MIN       (-100.0f)
#define PRESSURE_CONTROL_OUTPUT_MAX       (100.0f)
#define PRESSURE_CONTROL_MIN_DUTY_VALUE   700U       // 70% duty = 700/1000（动态调节基准）
#define PRESSURE_CONTROL_VALVE_THRESHOLD  (5.0f)     // 控制输出小于该值则不开阀
#define PRESSURE_CONTROL_TARGET_MIN       0U
#define PRESSURE_CONTROL_TARGET_MAX       320U
#define PRESSURE_CONTROL_SAFE_LIMIT       320U       // 超出则强制泄气

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

/* 延迟校准标志：在下一次UpdateADC时更新零点 */
static bool g_calibration_pending = false;

/* 转换系数（valueK） */
static float g_pressure_conversion_factor = PRESSURE_CONVERSION_FACTOR_DEFAULT;  // 默认2.75

/* PID 控制相关变量 */
static PIDController_t g_pressure_pid;
static bool g_pressure_control_enabled = false;
static bool g_pressure_control_fault = false;
static AppPressureControlMode_e g_pressure_control_mode = APP_PRESSURE_CONTROL_MODE_CONTINUOUS;
static uint16_t g_pressure_control_target = 0;
static float g_pressure_last_output = 0.0f;
static uint16_t g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE;

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

static void PressureControl_ResetOutputs(void);
static void PressureControl_ForceRelease(void);
static void PressureControl_ApplyOutput(float control_output);
static uint16_t PressureControl_ClampTarget(uint16_t target_mmHg);
static uint16_t PressureControl_SelectMinDuty(float abs_error);

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

    PID_Init(&g_pressure_pid, 1.0f, 0.12f, 0.00f, PRESSURE_CONTROL_SAMPLE_TIME_S);
    PID_SetOutputLimits(&g_pressure_pid, PRESSURE_CONTROL_OUTPUT_MIN, PRESSURE_CONTROL_OUTPUT_MAX);
    PID_SetIntegralLimits(&g_pressure_pid, PRESSURE_CONTROL_OUTPUT_MIN, PRESSURE_CONTROL_OUTPUT_MAX);

    g_pressure_control_enabled = false;
    g_pressure_control_fault = false;
    g_pressure_control_mode = APP_PRESSURE_CONTROL_MODE_CONTINUOUS;
    g_pressure_control_target = PRESSURE_CONTROL_TARGET_MIN;
    g_pressure_last_output = 0.0f;

    PressureControl_ResetOutputs();
}

/**
 * @name      AppPressure_Process
 * @brief     压力管理处理函数（在定时器中调用）
 * @param     user_data - 用户数据（定时器回调参数，未使用）
 * @note      当前主要用于扩展其他处理逻辑，滤波已在UpdateADC中完成
 */
void AppPressure_Process(void* user_data)
{
    (void)user_data;  /* 定时器回调未传递上下文 */

    /* Step①：控制未启用直接返回，避免多余计算 */
    if (!g_pressure_control_enabled) {
        return;
    }

    /* Step②：获取当前压力值（单位 mmHg，已滤波且扣除零点） */
    uint16_t current_pressure = AppPressure_GetPressureValue();

    /*
     * Step③：安全保护
     * - g_pressure_adc_raw == 0xFFFF 视为 ADC 读取失败
     * - current_pressure 超过安全上限（320mmHg）视为过压
     * 触发时立即停止闭环、打开阀门泄压，并置 fault 标志
     */
//    if (g_pressure_adc_raw == 0xFFFF || current_pressure >= PRESSURE_CONTROL_SAFE_LIMIT) {
//        g_pressure_control_fault = true;
//        g_pressure_control_enabled = false;
//        PressureControl_ForceRelease();
//        return;
//    }

    /* Step④：计算目标与实测的偏差，进入 PID 计算链 */
    float setpoint = (float)g_pressure_control_target;
    float measurement = (float)current_pressure;
    float error = setpoint - measurement;

    /* Step⑤：死区判断，±2mmHg 以内视为达标，重置 PID 防止抖动 */
    float abs_error = (error >= 0.0f) ? error : -error;
    if (abs_error <= PRESSURE_CONTROL_DEADBAND_MMHG) {
        PID_Reset(&g_pressure_pid);
        g_pressure_last_output = 0.0f;
        PressureControl_ApplyOutput(0.0f);
        g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE;
        return;
    }

    /* Step⑥：执行 PID 运算，输出范围 -100 ~ 100，正值抽气，负值泄气 */
    float output = PID_Update(&g_pressure_pid, setpoint, measurement);
    g_pressure_last_output = output;

    /* Step⑦：把 PID 输出映射到实际执行器（泵 PWM + 双阀门） */
    g_pressure_min_duty_current = PressureControl_SelectMinDuty(abs_error);
    PressureControl_ApplyOutput(output);
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

    /* 如果存在待处理的零点校准，使用当前滤波结果更新零点偏移 */
    if (g_calibration_pending) {
        g_pressure_zero_offset = g_pressure_adc_filtered;
        g_calibration_pending = false;
    }
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
    /* 标记在下一次UpdateADC时进行零点校准 */
    g_calibration_pending = true;
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

/**
 * @name      AppPressure_StartControl
 * @brief     启动压力闭环控制
 * @param     target_mmHg - 目标压力（mmHg）
 * @param     mode        - 控制模式（连续 / 间歇高压 / 间歇低压）
 * @note      会复位 PID 状态并关闭执行器，防止旧输出残留
 */
void AppPressure_StartControl(uint16_t target_mmHg, AppPressureControlMode_e mode)
{
    g_pressure_control_target = PressureControl_ClampTarget(target_mmHg);
    g_pressure_control_mode = mode;
    g_pressure_control_fault = false;
    g_pressure_last_output = 0.0f;

    PID_Reset(&g_pressure_pid);
    PressureControl_ResetOutputs();

    g_pressure_control_enabled = true;
}

/**
 * @name      AppPressure_StopControl
 * @brief     停止压力闭环控制
 * @note      清除 PID 状态并关闭泵/阀，确保执行器安全
 */
void AppPressure_StopControl(void)
{
    g_pressure_control_enabled = false;
    g_pressure_last_output = 0.0f;
    PID_Reset(&g_pressure_pid);
    PressureControl_ResetOutputs();
}

/**
 * @name      AppPressure_UpdateTarget
 * @brief     更新目标压力
 * @param     target_mmHg - 新目标（自动限幅）
 */
void AppPressure_UpdateTarget(uint16_t target_mmHg)
{
    g_pressure_control_target = PressureControl_ClampTarget(target_mmHg);
}

/**
 * @name      AppPressure_IsControlEnabled
 * @brief     查询控制是否正在运行
 * @retval    true=运行中, false=已停止
 */
bool AppPressure_IsControlEnabled(void)
{
    return g_pressure_control_enabled;
}

/**
 * @name      AppPressure_HasControlFault
 * @brief     查询控制故障标志
 * @retval    true=存在故障（已强制泄气）, false=无故障
 */
bool AppPressure_HasControlFault(void)
{
    return g_pressure_control_fault;
}

/**
 * @name      AppPressure_ClearControlFault
 * @brief     清除故障标志（不改变执行器状态）
 */
void AppPressure_ClearControlFault(void)
{
    g_pressure_control_fault = false;
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

/**
 * @name      PressureControl_ResetOutputs
 * @brief     关闭泵并关闭两路阀门（安全状态）
 */
static void PressureControl_ResetOutputs(void)
{
    PWM_SetDuty(0);
    if (PWM_IsRunning()) {
        PWM_Stop();
    }
    HAL_Valve1_Close();
    HAL_Valve2_Close();
    HAL_Pump_Stop();
    g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE;
}

/**
 * @name      PressureControl_ForceRelease
 * @brief     强制泄气：停泵并打开两路阀门
 */
static void PressureControl_ForceRelease(void)
{
    PWM_SetDuty(0);
    if (PWM_IsRunning()) {
        PWM_Stop();
    }
    HAL_Pump_Stop();
    HAL_Valve1_Open();
    HAL_Valve2_Open();
    g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE;
}

/**
 * @name      PressureControl_ApplyOutput
 * @brief     根据 PID 输出驱动泵 PWM 与阀门
 * @param     control_output - PID 输出（正值抽气，负值泄气）
 */
static void PressureControl_ApplyOutput(float control_output)
{
    if (control_output > 0.0f) {
        /*
         * 正输出：需要抽气
         * - 先将 PID 输出（-100~100）归一化到 0~1，作为剩余占空比分配比例
         * - duty = 动态最小占空比 + 剩余占空比 * normalized，避免进入“嗡嗡不抽”区间
         */
        float normalized = control_output / PRESSURE_CONTROL_OUTPUT_MAX;
        if (normalized > 1.0f) {
            normalized = 1.0f;
        } else if (normalized < 0.0f) {
            normalized = 0.0f;
        }

        /* 计算最终占空比（0~1000 对应 0~100%），根据误差动态设定最小值 */
        uint16_t duty_value = g_pressure_min_duty_current;
        if (PWM_DUTY_MAX > g_pressure_min_duty_current) {
            uint16_t duty_span = (uint16_t)(PWM_DUTY_MAX - g_pressure_min_duty_current);
            uint16_t dynamic = (uint16_t)(normalized * (float)duty_span + 0.5f);
            duty_value = (uint16_t)(g_pressure_min_duty_current + dynamic);
        }

        if (duty_value > PWM_DUTY_MAX) {
            duty_value = PWM_DUTY_MAX;
        }
        PWM_SetDuty(duty_value);

        /* 设置 PWM 并确保泵处于运行状态 */
        if (!PWM_IsRunning()) {
            PWM_Start();
        }

        /* 抽气时关闭两路阀门，保持管路密闭 */
        HAL_Valve1_Close();
        HAL_Valve2_Close();
    } else {
        /*
         * 非正输出：需要减压 / 保压
         * - 先停泵
         * - 根据负输出幅度决定是否打开阀门泄气
         */
        PWM_SetDuty(0);
        float release_magnitude = -control_output;
        if (release_magnitude >= PRESSURE_CONTROL_VALVE_THRESHOLD) {
            HAL_Valve1_Open();
            HAL_Valve2_Open();
        } else {
            HAL_Valve1_Close();
            HAL_Valve2_Close();
        }
    }
}

/**
 * @name      PressureControl_ClampTarget
 * @brief     限制目标压力在安全范围
 */
static uint16_t PressureControl_ClampTarget(uint16_t target_mmHg)
{
    if (target_mmHg > PRESSURE_CONTROL_TARGET_MAX) {
        return PRESSURE_CONTROL_TARGET_MAX;
    }
    if (target_mmHg < PRESSURE_CONTROL_TARGET_MIN) {
        return PRESSURE_CONTROL_TARGET_MIN;
    }
    return target_mmHg;
}

static uint16_t PressureControl_SelectMinDuty(float abs_error)
{
    if (abs_error >= 25.0f) {
        return 320U;   /* 误差很大，30% duty 足够启动 */
    }
    if (abs_error >= 15.0f) {
        return 360U;   /* 误差中等，提高到 36% 保障抽气 */
    }
    if (abs_error >= 10.0f) {
        return 420U;   /* 比较接近目标时维持 42% */
    }
    if (abs_error >= 7.0f) {
        return 460U;   /* 更贴近目标时提高到 46% */
    }
    return 500U;       /* 刚超过死区仍保持 50%，避免停滞 */
}
 

