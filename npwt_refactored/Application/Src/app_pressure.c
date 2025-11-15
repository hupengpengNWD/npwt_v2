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
#include "../../Middleware/Inc/soft_timer.h"
#include <stdbool.h>
#include <stdint.h>

/****************************************************************************
 * 内部常量定义
 ****************************************************************************/

#define PRESSURE_FILTER_COUNT      5   // 滤波采样次数（10ms采样 * 5点滑动平均）

#define PRESSURE_CONTROL_SAMPLE_TIME_S    (0.010f)   // 控制循环采样周期（10ms）
#define PRESSURE_CONTROL_DEADBAND_MMHG    (5.0f)     // 允许的稳态误差
#define PRESSURE_CONTROL_OUTPUT_MIN       (-100.0f)
#define PRESSURE_CONTROL_OUTPUT_MAX       (100.0f)
#define PRESSURE_CONTROL_MIN_DUTY_VALUE   700U       // 70% duty = 700/1000（动态调节基准）
#define PRESSURE_CONTROL_VALVE_THRESHOLD  (5.0f)     // 控制输出小于该值则不开阀
#define PRESSURE_CONTROL_REENGAGE_THRESHOLD_MMHG  (5.0f)  // 再次介入需要超过的误差
#define PRESSURE_MM_FILTER_ALPHA          (0.25f)   // 额外一阶IIR平滑系数（0~1）
#define PRESSURE_CONTROL_TARGET_OFFSET    (4U)      // 控制用目标偏移（防止停泵后下跌）
#define PRESSURE_CONTROL_TARGET_MAX       320U
#define PRESSURE_CONTROL_SAFE_LIMIT       320U       // 超出则强制泄气
#define PRESSURE_CONTROL_TARGET_MIN       0U        // 允许的最小控制目标（mmHg）

/****************************************************************************
 * 内部变量
 ****************************************************************************/

/* 滤波缓冲区（3次采样） */
static uint16_t g_pressure_filter_buf[PRESSURE_FILTER_COUNT];
static uint8_t g_pressure_filter_index = 0;   // 当前缓冲区索引
static uint16_t g_pressure_adc_filtered = 0;  // ADC原始值（滤波后）
static uint16_t g_pressure_adc_raw = 0;       // ADC原始值（最后一次采样，未滤波）
static uint16_t g_pressure_zero_offset = 0;   // 零点偏移值（adc_zero）
static bool g_calibration_pending = false;    // 延迟校准标志：在下一次UpdateADC时更新零点
static SoftTimerHandle_t g_pressure_bleed_timer = 0;        // 开机放气定时器

/* 间歇控制相关变量 */
static bool g_intermittent_active = false;
static bool g_intermittent_high_phase = true;
static SoftTimerHandle_t g_intermittent_timer = 0;
static uint16_t g_intermittent_high_target = 0;
static uint16_t g_intermittent_low_target = 0;
static uint32_t g_intermittent_high_duration_ms = 0;
static uint32_t g_intermittent_low_duration_ms = 0;
static SoftTimerHandle_t g_intermittent_release_timer = 0;
static bool g_intermittent_release_active = false;

#define INTERMITTENT_RELEASE_DURATION_MS   1000UL

/* 转换系数（valueK） */
static float g_pressure_conversion_factor = PRESSURE_CONVERSION_FACTOR_DEFAULT;  // 默认2.75

/* PID 控制相关变量 */
static PIDController_t g_pressure_pid;                     // PID 控制器实例
static bool g_pressure_control_enabled = false;            // 是否启用压力闭环控制
static bool g_pressure_control_fault = false;              // 控制是否进入故障状态
static AppPressureControlMode_e g_pressure_control_mode = APP_PRESSURE_CONTROL_MODE_CONTINUOUS; // 当前控制模式
static uint16_t g_pressure_control_user_target = 0;        // 用户设定的目标压力（mmHg）
static uint16_t g_pressure_control_target = 0;             // 应用偏移后的内部目标压力
static float g_pressure_last_output = 0.0f;                // 上一次 PID 输出值
static uint16_t g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE; // 当前动态最小占空比
static bool g_pressure_hold_active = false;                // 是否处于保持（停泵）状态
static float g_pressure_mmHg_filtered = 0.0f;              // 平滑处理后的压力值
static bool g_pressure_mmHg_initialized = false;           // 平滑滤波是否已经初始化

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
/**
 * @brief  在用户设定值基础上应用控制偏移
 * @param  user_target_mmHg 用户设定的目标压力（已限幅）
 * @return 应用偏移后的内部控制目标，自动限制在安全范围内
 */
static uint16_t PressureControl_ApplyOffset(uint16_t user_target_mmHg);
/**
 * @brief 根据误差大小选择动态最小占空比
 * @param abs_error 目标与实测的误差绝对值（mmHg）
 * @return 保证泵有足够扭矩的最小 PWM 占空比（0~PWM_DUTY_MAX）
 */
static uint16_t PressureControl_SelectMinDuty(float abs_error);
static void AppPressure_BleedTimerCallback(void* user_data);
static void AppPressure_IntermittentTimerCallback(void* user_data);
static void AppPressure_StartIntermittentPhase(bool high_phase);
static void AppPressure_LowPhaseReleaseCallback(void* user_data);

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
    g_pressure_control_user_target = PRESSURE_CONTROL_TARGET_MIN;
    g_pressure_control_target = PRESSURE_CONTROL_TARGET_MIN;
    g_pressure_last_output = 0.0f;
    g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE;
    g_pressure_hold_active = false;
    g_pressure_mmHg_filtered = 0.0f;
    g_pressure_mmHg_initialized = false;

    /* 创建开机放气定时器（单次3秒，用于开机零点校准） */
    if (g_pressure_bleed_timer == 0) {
        g_pressure_bleed_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                  3000,
                                                  AppPressure_BleedTimerCallback,
                                                  NULL);
    } else {
        SoftTimer_Stop(g_pressure_bleed_timer);
        SoftTimer_SetCallback(g_pressure_bleed_timer, AppPressure_BleedTimerCallback, NULL);
        SoftTimer_SetPeriod(g_pressure_bleed_timer, 3000);
    }

    if (g_intermittent_timer == 0) {
        g_intermittent_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                1000,
                                                AppPressure_IntermittentTimerCallback,
                                                NULL);
    } else {
        SoftTimer_Stop(g_intermittent_timer);
        SoftTimer_SetCallback(g_intermittent_timer, AppPressure_IntermittentTimerCallback, NULL);
        SoftTimer_SetPeriod(g_intermittent_timer, 1000);
    }

    if (g_intermittent_release_timer == 0) {
        g_intermittent_release_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                        INTERMITTENT_RELEASE_DURATION_MS,
                                                        AppPressure_LowPhaseReleaseCallback,
                                                        NULL);
    } else {
        SoftTimer_Stop(g_intermittent_release_timer);
        SoftTimer_SetCallback(g_intermittent_release_timer, AppPressure_LowPhaseReleaseCallback, NULL);
        SoftTimer_SetPeriod(g_intermittent_release_timer, INTERMITTENT_RELEASE_DURATION_MS);
    }
    g_intermittent_release_active = false;
    g_intermittent_active = false;

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

    /* Step1：控制未启用直接返回，避免多余计算 */
    if (!g_pressure_control_enabled) {
        return;
    }

    /* Step2：获取当前压力值（单位 mmHg，已滤波且扣除零点） */
    uint16_t current_pressure = AppPressure_GetPressureValue();

    /*
     * Step3：安全保护
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

    /* Step4：计算目标与实测的偏差，进入 PID 计算链 */
    /*
     * setpoint  ：内部控制目标，已包含偏移量
     * measurement ：当前测得的压力值
     * error      ：正值表示需要继续抽气，负值表示压力已经超出目标
     * abs_error  ：误差的绝对值，用于动态占空比等后续逻辑
     */
    float setpoint = (float)g_pressure_control_target;
    float measurement = (float)current_pressure;
    float error = setpoint - measurement;
    float abs_error = (error >= 0.0f) ? error : -error;
    if (g_pressure_hold_active) {
        uint16_t reengage_threshold = 0U;
        uint16_t threshold_offset = (uint16_t)PRESSURE_CONTROL_REENGAGE_THRESHOLD_MMHG;
        if (g_pressure_control_user_target > threshold_offset) {
            reengage_threshold = (uint16_t)(g_pressure_control_user_target - threshold_offset);
        }

        if (current_pressure <= reengage_threshold) {
            g_pressure_hold_active = false;
            PID_Reset(&g_pressure_pid);
        } else {
            return;
        }
    }
    /* Step5：死区判断，目标值 ~ 目标值+5mmHg 视为达标，重置 PID 防止抖动 */
    if ((error <= 0.0f) && (current_pressure >= (uint16_t)(g_pressure_control_target + PRESSURE_CONTROL_DEADBAND_MMHG))) {
        PID_Reset(&g_pressure_pid);
        g_pressure_last_output = 0.0f;
        PressureControl_ApplyOutput(0.0f);
        g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE;
        g_pressure_hold_active = true;
        return;
    }

    /* Step6：执行 PID 运算，输出范围 -100 ~ 100，正值抽气，负值泄气 */
    float output = PID_Update(&g_pressure_pid, setpoint, measurement);
    g_pressure_last_output = output;

    /* Step7：把 PID 输出映射到实际执行器（泵 PWM + 双阀门） */
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
    uint16_t instantaneous = PressureConvert_ADCToMMHG(g_pressure_adc_filtered);
    float sample = (float)instantaneous;

    if (!g_pressure_mmHg_initialized) {
        g_pressure_mmHg_filtered = sample;
        g_pressure_mmHg_initialized = true;
    } else {
        g_pressure_mmHg_filtered += PRESSURE_MM_FILTER_ALPHA * (sample - g_pressure_mmHg_filtered);
    }

    if (g_pressure_mmHg_filtered < 0.0f) {
        g_pressure_mmHg_filtered = 0.0f;
    } else if (g_pressure_mmHg_filtered > (float)PRESSURE_CONTROL_TARGET_MAX) {
        g_pressure_mmHg_filtered = (float)PRESSURE_CONTROL_TARGET_MAX;
    }

    return (uint16_t)(g_pressure_mmHg_filtered + 0.5f);
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
 * @brief 开机放气定时器回调：关闭阀门并触发零点校准
 */
static void AppPressure_BleedTimerCallback(void* user_data)
{
    (void)user_data;

//    HAL_Valve2_Close();
    HAL_Valve1_Close();
    AppPressure_CalibrateZero();
}

/**
 * @name      AppPressure_BleedAndCalibrateZero
 * @brief     开机放气并在结束后完成零点校准
 * @note
 *   1. 打开放气阀释放系统残余压力
 *   2. 启动3秒一次性的软定时器
 *   3. 定时器回调关闭阀门并触发零点校准
 *   4. 若定时器创建失败，则立即关闭阀门并直接校准
 */
void AppPressure_BleedAndCalibrateZero(void)
{
    /* 打开电磁阀释放压力 */
//    HAL_Valve2_Open();
    HAL_Valve1_Open();

    if (g_pressure_bleed_timer != 0) {
        SoftTimer_Stop(g_pressure_bleed_timer);
        SoftTimer_SetPeriod(g_pressure_bleed_timer, 3000);
        SoftTimer_Start(g_pressure_bleed_timer);
    } else {
        g_pressure_bleed_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                  3000,
                                                  AppPressure_BleedTimerCallback,
                                                  NULL);
        if (g_pressure_bleed_timer != 0) {
            SoftTimer_Start(g_pressure_bleed_timer);
        } else {
            /* 定时器创建失败，立即关闭阀门并执行零点校准 */
            HAL_Valve1_Close();
            AppPressure_CalibrateZero();
        }
    }
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
    uint16_t clamped_user_target = PressureControl_ClampTarget(target_mmHg);
    g_pressure_control_user_target = clamped_user_target;
    uint16_t adjusted_target = PressureControl_ApplyOffset(clamped_user_target);
    g_pressure_control_target = PressureControl_ClampTarget(adjusted_target);
    g_pressure_control_mode = mode;
    g_pressure_control_fault = false;
    g_pressure_last_output = 0.0f;

    PID_Reset(&g_pressure_pid);
    PressureControl_ResetOutputs();
    g_pressure_hold_active = false;

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

    if (g_intermittent_active) {
        g_intermittent_active = false;
        if (g_intermittent_timer != 0) {
            SoftTimer_Stop(g_intermittent_timer);
        }
        g_intermittent_high_phase = true;
    }
}

/**
 * @name      AppPressure_UpdateTarget
 * @brief     更新目标压力
 * @param     target_mmHg - 新目标（自动限幅）
 */
void AppPressure_UpdateTarget(uint16_t target_mmHg)
{
    uint16_t clamped_user_target = PressureControl_ClampTarget(target_mmHg);
    g_pressure_control_user_target = clamped_user_target;
    uint16_t adjusted_target = PressureControl_ApplyOffset(clamped_user_target);
    g_pressure_control_target = PressureControl_ClampTarget(adjusted_target);
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
 * @name      AppPressure_IsMotorRunning
 * @brief     查询电机是否实际正在运行
 * @retval    true=电机正在运行, false=电机已停止
 * @note      检查PID控制是否启用、是否处于保持状态、PID输出是否大于0
 *           用于判断是否有实际负载，以决定电池电量显示的补偿策略
 *           
 *           判断逻辑：
 *           1. 如果控制未启用，电机肯定没运行
 *           2. 如果处于保持状态（压力达标），电机已停止
 *           3. 如果PID输出<=0，电机已停止
 *           4. 否则，电机正在运行（有实际负载）
 */
bool AppPressure_IsMotorRunning(void)
{
    /* 如果控制未启用，电机肯定没运行 */
    if (!g_pressure_control_enabled) {
        return false;
    }
    
    /* 如果处于保持状态（压力达标，电机已停止），电机没运行 */
    if (g_pressure_hold_active) {
        return false;
    }
    
    /* 如果PID输出<=0，电机已停止（非正输出表示不需要抽气） */
    if (g_pressure_last_output <= 0.0f) {
        return false;
    }
    
    /* 否则，电机正在运行（有实际负载） */
    return true;
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
    g_pressure_hold_active = false;
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
    g_pressure_hold_active = false;
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

/**
 * @brief  在用户设定值基础上应用控制偏移
 * @param  user_target_mmHg 用户设定的目标压力（已限幅）
 * @return 应用偏移后的内部控制目标，自动限制在安全范围内
 */
static uint16_t PressureControl_ApplyOffset(uint16_t user_target_mmHg)
{
    uint32_t adjusted = (uint32_t)user_target_mmHg + (uint32_t)PRESSURE_CONTROL_TARGET_OFFSET;
    if (adjusted > PRESSURE_CONTROL_TARGET_MAX) {
        adjusted = PRESSURE_CONTROL_TARGET_MAX;
    }
    return (uint16_t)adjusted;
}

/**
 * @brief 根据误差大小选择动态最小占空比
 * @param abs_error 目标与实测的误差绝对值（mmHg）
 * @return 保证泵有足够扭矩的最小 PWM 占空比（0~PWM_DUTY_MAX）
 */
static uint16_t PressureControl_SelectMinDuty(float abs_error)
{
    if (abs_error >= 25.0f) {
        return 320U;   /* 误差很大，30% duty 足够启动 */
    }
    if (abs_error >= 15.0f) {
        return 360U;   /* 误差中等，提高到 36% 保障抽气 */
    }
    if (abs_error >= 10.0f) {
        return 500U;   /* 误差 10 左右仍需至少 50% 才能持续抽气 */
    }
    if (abs_error >= 7.0f) {
        return 550U;   /* 更接近目标时拉高到 55% 保证扭矩 */
    }
    if (abs_error >= 5.0f) {
        return 600U;   /* 距离死区很近仍保持 60%，避免停顿 */
    }
    return 650U;       /* 极近目标也维持较高占空比，直至进入死区 */
}
 
/**
 * @brief     启动间歇控制的当前阶段
 * @param     high_phase true 表示高压阶段，false 表示低压阶段
 * @note      负责切换 PID 目标并重新启动阶段定时器
 */
static void AppPressure_StartIntermittentPhase(bool high_phase)
{
    if (!g_intermittent_active) {
        return;
    }

    if (g_intermittent_timer == 0) {
        g_intermittent_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                1000,
                                                AppPressure_IntermittentTimerCallback,
                                                NULL);
        if (g_intermittent_timer == 0) {
            g_intermittent_active = false;
            AppPressure_StopControl();
            return;
        }
    }

    g_intermittent_high_phase = high_phase;

    /* 高压阶段：直接启动 PID 并按设定时间运行 */
    if (high_phase) {
        if (g_intermittent_release_timer != 0) {
            SoftTimer_Stop(g_intermittent_release_timer);
        }
        g_intermittent_release_active = false;

        uint16_t target = g_intermittent_high_target;
        AppPressure_StartControl(target, APP_PRESSURE_CONTROL_MODE_INTERMITTENT_HIGH);

        uint32_t duration = g_intermittent_high_duration_ms;
        if (duration == 0U) {
            duration = 1000U;
        }

        SoftTimer_Stop(g_intermittent_timer);
        SoftTimer_SetPeriod(g_intermittent_timer, duration);
        SoftTimer_Start(g_intermittent_timer);
        return;
    }

    /* 低压阶段：先主动泄压，再启动 PID 控制低压目标 */
    g_intermittent_release_active = true;
    g_pressure_control_enabled = false;
    PressureControl_ForceRelease();

    if (g_intermittent_release_timer == 0) {
        g_intermittent_release_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                        INTERMITTENT_RELEASE_DURATION_MS,
                                                        AppPressure_LowPhaseReleaseCallback,
                                                        NULL);
        if (g_intermittent_release_timer == 0) {
            /* 如果释放定时器创建失败，直接启动低压控制，避免停滞 */
            g_intermittent_release_active = false;
            AppPressure_StartControl(g_intermittent_low_target,
                                     APP_PRESSURE_CONTROL_MODE_INTERMITTENT_LOW);
            uint32_t duration = g_intermittent_low_duration_ms;
            if (duration == 0U) {
                duration = 1000U;
            }
            SoftTimer_Stop(g_intermittent_timer);
            SoftTimer_SetPeriod(g_intermittent_timer, duration);
            SoftTimer_Start(g_intermittent_timer);
            return;
        }
    }

    SoftTimer_Stop(g_intermittent_timer);
    SoftTimer_Stop(g_intermittent_release_timer);
    SoftTimer_SetPeriod(g_intermittent_release_timer, INTERMITTENT_RELEASE_DURATION_MS);
    SoftTimer_Start(g_intermittent_release_timer);
}

/**
 * @brief     低压阶段释放定时器回调
 * @param     user_data 定时器回调参数（未使用）
 * @note      放气结束后启动低压 PID 控制并开启阶段计时
 */
static void AppPressure_LowPhaseReleaseCallback(void* user_data)
{
    (void)user_data;

    g_intermittent_release_active = false;

    uint16_t target = g_intermittent_low_target;
    AppPressure_StartControl(target, APP_PRESSURE_CONTROL_MODE_INTERMITTENT_LOW);

    uint32_t duration = g_intermittent_low_duration_ms;
    if (duration == 0U) {
        duration = 1000U;
    }

    if (g_intermittent_timer != 0) {
        SoftTimer_Stop(g_intermittent_timer);
        SoftTimer_SetPeriod(g_intermittent_timer, duration);
        SoftTimer_Start(g_intermittent_timer);
    }
}

/**
 * @brief     间歇阶段定时器回调
 * @param     user_data 定时器回调参数（未使用）
 * @note      定时到期后切换到下一个阶段
 */
static void AppPressure_IntermittentTimerCallback(void* user_data)
{
    (void)user_data;

    if (!g_intermittent_active) {
        return;
    }

    AppPressure_StartIntermittentPhase(!g_intermittent_high_phase);
}

/**
 * @name      AppPressure_StartIntermittentTherapy
 * @brief     启动间歇治疗循环
 * @param     high_target_mmHg 高压目标（mmHg）
 * @param     low_target_mmHg  低压目标（mmHg）
 * @param     high_time_min    高压维持时间（分钟）
 * @param     low_time_min     低压维持时间（分钟）
 */
void AppPressure_StartIntermittentTherapy(uint16_t high_target_mmHg,
                                          uint16_t low_target_mmHg,
                                          uint16_t high_time_min,
                                          uint16_t low_time_min)
{
    AppPressure_StopIntermittentTherapy();

    g_intermittent_high_target = PressureControl_ClampTarget(high_target_mmHg);
    g_intermittent_low_target = PressureControl_ClampTarget(low_target_mmHg);

    if (g_intermittent_low_target >= g_intermittent_high_target) {
        if (g_intermittent_high_target > 5U) {
            g_intermittent_low_target = (uint16_t)(g_intermittent_high_target - 5U);
        } else {
            g_intermittent_low_target = g_intermittent_high_target;
        }
    }

    g_intermittent_high_duration_ms = (uint32_t)high_time_min * 60000UL;
    g_intermittent_low_duration_ms  = (uint32_t)low_time_min  * 60000UL;

    if (g_intermittent_high_duration_ms == 0U) {
        g_intermittent_high_duration_ms = 60000UL;
    }
    if (g_intermittent_low_duration_ms == 0U) {
        g_intermittent_low_duration_ms = 60000UL;
    }

    g_intermittent_active = true;
    AppPressure_StartIntermittentPhase(true);
}

/**
 * @name      AppPressure_StopIntermittentTherapy
 * @brief     停止间歇治疗循环并复位阶段定时器
 */
void AppPressure_StopIntermittentTherapy(void)
{
    if (g_intermittent_timer != 0) {
        SoftTimer_Stop(g_intermittent_timer);
    }
    if (g_intermittent_release_timer != 0) {
        SoftTimer_Stop(g_intermittent_release_timer);
    }
    g_intermittent_active = false;
    g_intermittent_high_phase = true;
    g_intermittent_release_active = false;
    AppPressure_StopControl();
}

/**
 * @name      AppPressure_GetCurrentTarget
 * @brief     获取当前闭环控制的用户目标压力
 * @retval    用户设定的目标压力（mmHg）
 */
uint16_t AppPressure_GetCurrentTarget(void)
{
    return g_pressure_control_user_target;
}

/**
 * @name      AppPressure_GetCurrentMode
 * @brief     获取当前闭环控制模式
 * @retval    AppPressureControlMode_e - 当前控制模式
 */
AppPressureControlMode_e AppPressure_GetCurrentMode(void)
{
    return g_pressure_control_mode;
}
 

