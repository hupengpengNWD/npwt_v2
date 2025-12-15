/****************************************************************************
 * 文件名: app_pressure.c
 * 功能: 压力传感器管理应用层实现
 * 
 * 说明: 
 *   - ADC采集频率：约每4ms（250Hz，由调用频率决定）
 *   - 滤波方式：3次采样滑动平均（参考adc_press_filter）
 *   - 转换公式：压力值(mmHg) = (adc_ps0 - adc_zero) / valueK
 *   - valueK默认值：2.75（参考老版本工程）
 * 
 * 实现细节：
 *   1. 3次采样平均滤波：每次UpdateADC时更新一个样本，计算平均值
 *   2. 零点校准：记录启动时的ADC值作为零点偏移
 *   3. ADC到mmHg转换：使用转换系数和零点偏移计算
 * 
 * 创建日期: 2025-01-XX
 ****************************************************************************/

#include "../Inc/app_pressure.h"
#include "../../Core/Inc/system_config.h"  // 包含系统配置（PRESSURE_BLEED_DURATION_MS等）
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

/****************************************************************************
 * 漏气报警超时时间查表（基于理论公式：t = (V/S) × ln[760/(760-Pset)]）
 * 
 * 计算条件：
 *   - 基准容器体积：V = 600ml（查表基准值）
 *   - 泵流速：S = 0.42 L/min
 *   - 安全余量：理论时间 × 2.5
 *   - 查表范围：20-300 mmHg，每10mmHg一档
 * 
 * 支持的容器体积（通过SYSTEM_CONTAINER_VOLUME_ML配置）：
 *   - 600ml（默认，缩放系数 1.000）
 *   - 400ml（缩放系数 0.667，约2/3）
 *   - 140ml（缩放系数 0.233，约1/4.3）
 * 
 * 体积缩放说明：
 *   建压时间与容器体积成正比：t_actual = t_base × (V_actual / V_base)
 *   GetLeakAlarmTimeout()函数会根据配置的容器体积自动缩放超时时间
 * 
 * 示例：120mmHg目标压力
 *   - 600ml容器：37秒（查表基准值）
 *   - 400ml容器：25秒（37 × 400/600）
 *   - 140ml容器：9秒（37 × 140/600）
 ****************************************************************************/
static const uint16_t g_leak_alarm_timeout_table[] = {
    /*  20 mmHg */  6,    /* 理论2.3秒  × 2.5 = 5.7秒  */
    /*  30 mmHg */  9,    /* 理论3.5秒  × 2.5 = 8.6秒  */
    /*  40 mmHg */  12,   /* 理论4.6秒  × 2.5 = 11.6秒 */
    /*  50 mmHg */  15,   /* 理论5.8秒  × 2.5 = 14.6秒 */
    /*  60 mmHg */  18,   /* 理论7.0秒  × 2.5 = 17.6秒 */
    /*  70 mmHg */  21,   /* 理论8.3秒  × 2.5 = 20.7秒 */
    /*  80 mmHg */  24,   /* 理论9.5秒  × 2.5 = 23.8秒 */
    /*  90 mmHg */  27,   /* 理论10.8秒 × 2.5 = 27.0秒 */
    /* 100 mmHg */  30,   /* 理论12.1秒 × 2.5 = 30.2秒 */
    /* 110 mmHg */  34,   /* 理论13.4秒 × 2.5 = 33.5秒 */
    /* 120 mmHg */  37,   /* 理论14.7秒 × 2.5 = 36.8秒 */
    /* 130 mmHg */  40,   /* 理论16.1秒 × 2.5 = 40.2秒 */
    /* 140 mmHg */  44,   /* 理论17.5秒 × 2.5 = 43.7秒 */
    /* 150 mmHg */  47,   /* 理论18.9秒 × 2.5 = 47.1秒 */
    /* 160 mmHg */  51,   /* 理论20.3秒 × 2.5 = 50.7秒 */
    /* 170 mmHg */  54,   /* 理论21.7秒 × 2.5 = 54.2秒 */
    /* 180 mmHg */  58,   /* 理论23.2秒 × 2.5 = 57.9秒 */
    /* 190 mmHg */  62,   /* 理论24.7秒 × 2.5 = 61.7秒 */
    /* 200 mmHg */  66,   /* 理论26.2秒 × 2.5 = 65.5秒 */
    /* 210 mmHg */  69,   /* 理论27.8秒 × 2.5 = 69.4秒 */
    /* 220 mmHg */  73,   /* 理论29.3秒 × 2.5 = 73.3秒 */
    /* 230 mmHg */  77,   /* 理论30.9秒 × 2.5 = 77.4秒 */
    /* 240 mmHg */  82,   /* 理论32.6秒 × 2.5 = 81.5秒 */
    /* 250 mmHg */  86,   /* 理论34.2秒 × 2.5 = 85.6秒 */
    /* 260 mmHg */  90,   /* 理论35.9秒 × 2.5 = 89.7秒 */
    /* 270 mmHg */  94,   /* 理论37.6秒 × 2.5 = 93.9秒 */
    /* 280 mmHg */  98,   /* 理论39.3秒 × 2.5 = 98.2秒 */
    /* 290 mmHg */  103,  /* 理论41.0秒 × 2.5 = 102.5秒 */
    /* 300 mmHg */  107   /* 理论42.9秒 × 2.5 = 107.3秒 */
};

#define LEAK_ALARM_TIMEOUT_TABLE_SIZE   (sizeof(g_leak_alarm_timeout_table) / sizeof(g_leak_alarm_timeout_table[0]))
#define LEAK_ALARM_TIMEOUT_BASE_PRESSURE  20U   /* 查表基准压力：20 mmHg */
#define LEAK_ALARM_TIMEOUT_STEP_PRESSURE  10U   /* 查表步进：10 mmHg */

#define PRESSURE_CONTROL_SAMPLE_TIME_S    (0.010f)   // 控制循环采样周期（10ms）
#define PRESSURE_CONTROL_OUTPUT_MIN       (-100.0f)
#define PRESSURE_CONTROL_OUTPUT_MAX       (100.0f)
#define PRESSURE_CONTROL_MIN_DUTY_VALUE   700U       // 70% duty = 700/1000（动态调节基准）
#define PRESSURE_CONTROL_VALVE_THRESHOLD  (5.0f)     // 控制输出小于该值则不开阀
#define PRESSURE_MM_FILTER_ALPHA          (0.25f)   // 额外一阶IIR平滑系数（0~1）
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

/* 泄漏报警超时检测相关变量 */
static bool g_leak_alarm_triggered = false;                // 泄漏报警已触发标志
static uint32_t g_leak_alarm_start_time_ms = 0;           // 泄漏报警计时开始时间（毫秒）
static bool g_leak_alarm_target_reached = false;           // 是否已达到目标压力（用于清除超时检测）
static uint8_t g_leak_alarm_in_range_count = 0;            // 气压在区间内的连续计数（用于区间次数统计）

/* 管路堵塞报警检测相关变量 */
static bool g_blockage_alarm_triggered = false;            // 管路堵塞报警已触发标志
static uint32_t g_blockage_alarm_hold_start_tick = 0;      // 进入保持状态的tick数，0表示未进入保持状态
static bool g_blockage_alarm_hold_active = false;          // 是否已进入保持状态（用于检测管路堵塞）

/* 过压报警检测相关变量（检测入口堵塞导致压力过高） */
static bool g_overpressure_alarm_triggered = false;        // 过压报警已触发标志
static bool g_overpressure_alarm_by_build_time = false;    // 过压报警是否由建立时间过短触发（条件2）
static uint32_t g_overpressure_alarm_start_tick = 0;       // 过压检测开始时刻（用于延迟判断）
static bool g_overpressure_alarm_delay_active = false;     // 是否启用延迟判断（持续过压才触发）

/* 泵工作原因记录 */
static PumpWorkReason_e g_pump_work_reason = PUMP_REASON_IDLE;  // 泵工作原因（闲置、建立负压或维持补充）

/* 建立负压时间记录（用于液位满报警检测） */
#if ENABLE_LIQUID_FULL_CONDITION2
static uint32_t g_build_start_time_ms = 0;  // 建立负压开始时间戳（毫秒）
//static uint16_t g_build_start_pressure = 0;  // 建立负压开始时的起始气压（mmHg）- 已屏蔽，节省空间
#endif

#if ENABLE_LIQUID_FULL_CONDITION2
/* 液位满报警建立时间阈值表（29档，V=60ml, S=0.42L/min） */
/* 公式: t = (V/S) × ln[760/(760-Pset)] */
/* 注意：数组存储理论时间（毫秒），使用时需乘以安全系数 PRESSURE_BUILD_TIME_SAFETY_FACTOR */
/* 单位: 毫秒（理论值，未乘以安全系数） */
static const uint32_t g_build_time_threshold_table[] = {
    /* 索引 | Pset(mmHg) | 理论时间(秒) | 理论时间(ms) */
      228U,  // [ 0]  20mmHg: 0.228585s = 228ms（理论值）
      345U,  // [ 1]  30mmHg: 0.345205s = 345ms（理论值）
      463U,  // [ 2]  40mmHg: 0.463433s = 463ms（理论值）
      583U,  // [ 3]  50mmHg: 0.583315s = 583ms（理论值）
      704U,  // [ 4]  60mmHg: 0.704898s = 704ms（理论值）
      828U,  // [ 5]  70mmHg: 0.828230s = 828ms（理论值）
      953U,  // [ 6]  80mmHg: 0.953363s = 953ms（理论值）
     1080U,  // [ 7]  90mmHg: 1.080349s = 1080ms（理论值）
     1209U,  // [ 8] 100mmHg: 1.209245s = 1209ms（理论值）
     1340U,  // [ 9] 110mmHg: 1.340109s = 1340ms（理论值）
     1473U,  // [10] 120mmHg: 1.473002s = 1473ms（理论值）
     1607U,  // [11] 130mmHg: 1.607988s = 1607ms（理论值）
     1745U,  // [12] 140mmHg: 1.745134s = 1745ms（理论值）
     1884U,  // [13] 150mmHg: 1.884510s = 1884ms（理论值）
     2026U,  // [14] 160mmHg: 2.026190s = 2026ms（理论值）
     2170U,  // [15] 170mmHg: 2.170251s = 2170ms（理论值）
     2316U,  // [16] 180mmHg: 2.316774s = 2316ms（理论值）
     2465U,  // [17] 190mmHg: 2.465846s = 2465ms（理论值）
     2617U,  // [18] 200mmHg: 2.617557s = 2617ms（理论值）
     2772U,  // [19] 210mmHg: 2.772001s = 2772ms（理论值）
     2929U,  // [20] 220mmHg: 2.929280s = 2929ms（理论值）
     3089U,  // [21] 230mmHg: 3.089498s = 3089ms（理论值）
     3252U,  // [22] 240mmHg: 3.252768s = 3252ms（理论值）
     3419U,  // [23] 250mmHg: 3.419209s = 3419ms（理论值）
     3588U,  // [24] 260mmHg: 3.588946s = 3588ms（理论值）
     3762U,  // [25] 270mmHg: 3.762112s = 3762ms（理论值）
     3938U,  // [26] 280mmHg: 3.938849s = 3938ms（理论值）
     4119U,  // [27] 290mmHg: 4.119306s = 4119ms（理论值）
     4303U   // [28] 300mmHg: 4.303645s = 4303ms（理论值）
};
#endif  // ENABLE_LIQUID_FULL_CONDITION2

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

/**
 * @name      GetLeakAlarmTimeout
 * @brief     根据目标压力查表获取漏气报警超时时间
 * @param     target_mmHg - 目标压力（mmHg）
 * @retval    超时时间（毫秒）
 * @note      使用查表法，基于理论公式 t=(V/S)×ln[760/(760-Pset)] 预计算
 *           超时时间 = 理论时间 × 2.5（安全余量）
 */
static uint32_t GetLeakAlarmTimeout(uint16_t target_mmHg);
static void AppPressure_LowPhaseReleaseCallback(void* user_data);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/* 电磁阀泄气状态标志：用于避免电池在泄气重负载阶段被误判为低电 */
static bool g_valve_bleeding_active = false;
/* 关阀后的冷却窗口（延迟判定解除），单位：AppPressure_Process调用周期（10ms） */
#define BLEED_COOLDOWN_TICKS   200U  /* 约2秒 */
/* 冷却计数器：
 * - 用于阀门关闭后继续维持“泄气中”判定的一段时间（避免关阀瞬态引起的电压/压力抖动）
 * - 计数单位为 AppPressure_Process 的调用间隔（10ms/tick）
 * - 当 g_bleed_cooldown_ticks > 0 时，AppPressure_IsBleeding() 仍返回 true
 * - 在 AppPressure_Process() 中每次调用自动递减，直至归零
 */
static uint16_t g_bleed_cooldown_ticks = 0;

/**
 * @name      AppPressure_IsBleeding
 * @brief     查询当前是否处于泄气阶段（阀门打开中）
 * @retval    true=泄气中, false=未泄气
 */
bool AppPressure_IsBleeding(void)
{
    return (g_valve_bleeding_active || (g_bleed_cooldown_ticks > 0U));
}
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

    /* 创建开机放气定时器（用于开机零点校准） */
    if (g_pressure_bleed_timer == 0) {
        g_pressure_bleed_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                  PRESSURE_BLEED_DURATION_MS,
                                                  AppPressure_BleedTimerCallback,
                                                  NULL);
    } else {
        SoftTimer_Stop(g_pressure_bleed_timer);
        SoftTimer_SetCallback(g_pressure_bleed_timer, AppPressure_BleedTimerCallback, NULL);
        SoftTimer_SetPeriod(g_pressure_bleed_timer, PRESSURE_BLEED_DURATION_MS);
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

    /* 冷却窗口递减：阀关闭后的短时内维持“泄气中”判定，避免关阀瞬态引起误判 */
    if (g_bleed_cooldown_ticks > 0U) {
        g_bleed_cooldown_ticks--;
    }

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
            /* 压力下降，退出保持状态，需要重新抽气补充 */
            g_pressure_hold_active = false;
            PID_Reset(&g_pressure_pid);
            /* 设置泵工作原因：从保持状态恢复抽气为维持补充 */
            g_pump_work_reason = PUMP_REASON_MAINTAINING;
        } else {
            /* 在保持状态下，检测管路堵塞报警（在return之前执行） */
            if (g_blockage_alarm_hold_active && g_pressure_control_user_target > 0) {
                /* 在保持状态下，PID输出为0，所以不会有负压补充 */
                /* 检查是否超时 */
                if (g_blockage_alarm_hold_start_tick > 0) {
                    uint32_t current_tick = SoftTimer_GetTickCount();
                    uint32_t elapsed_ticks = current_tick - g_blockage_alarm_hold_start_tick;
                    uint32_t elapsed_time_ms = elapsed_ticks * SOFT_TIMER_TICK_MS;
                    
                    /* 如果超时且没有负压补充，触发管路堵塞报警 */
                    if (elapsed_time_ms >= PRESSURE_BLOCKAGE_ALARM_TIMEOUT_MS) {
                        g_blockage_alarm_triggered = true;
                    }
                }
            }
            
            /* 在保持状态下，持续检测过压报警（入口堵塞检测） */
            if (g_pressure_control_user_target > 0) {
                /* 条件1：压力超过阈值（目标值 + 目标值的百分比） */
                uint16_t overpressure_threshold = g_pressure_control_user_target + (g_pressure_control_user_target * PRESSURE_OVERPRESSURE_ALARM_THRESHOLD_MMHG / 100);
                
                if (current_pressure >= overpressure_threshold) {
                    /* 压力异常高，可能是入口被堵 */
                    if (!g_overpressure_alarm_delay_active) {
                        /* 首次检测到过压，启动延迟计时 */
                        g_overpressure_alarm_delay_active = true;
                        g_overpressure_alarm_start_tick = SoftTimer_GetTickCount();
                    } else {
                        /* 检查延迟时间 */
                        uint32_t elapsed_ticks = SoftTimer_GetTickCount() - g_overpressure_alarm_start_tick;
                        uint32_t elapsed_time_ms = elapsed_ticks * SOFT_TIMER_TICK_MS;
                        
                        if (elapsed_time_ms >= PRESSURE_OVERPRESSURE_ALARM_DELAY_MS) {
                            /* 持续过压，触发报警 */
                            g_overpressure_alarm_triggered = true;
                            g_overpressure_alarm_by_build_time = false;  // 条件1触发，标记为非建立时间触发
                        }
                    }
                } else {
                    /* 压力恢复正常，清除延迟状态 */
                    g_overpressure_alarm_delay_active = false;
                    g_overpressure_alarm_start_tick = 0;
                }
            }
            
            return;
        }
    }
    /* Step5：死区判断，目标值 ~ 目标值+5mmHg 视为达标，重置 PID 防止抖动 */
    if ((error <= 0.0f) && (current_pressure >= (uint16_t)(g_pressure_control_target + PRESSURE_CONTROL_DEADBAND_MMHG))) {
        PID_Reset(&g_pressure_pid);
        g_pressure_last_output = 0.0f;
        PressureControl_ApplyOutput(0.0f);
        g_pressure_min_duty_current = PRESSURE_CONTROL_MIN_DUTY_VALUE;
        
        /* 如果刚进入保持状态，启动管路堵塞报警检测 */
        if (!g_pressure_hold_active) {
            g_pressure_hold_active = true;
            g_blockage_alarm_hold_active = true;
            g_blockage_alarm_hold_start_tick = SoftTimer_GetTickCount();
            g_blockage_alarm_triggered = false;  // 重置堵塞报警标志
            
            /* 【新增】进入保持状态时立即检查过压（入口堵塞快速检测） */
            if (g_pressure_control_user_target > 0) {
                /* 条件1：压力超过阈值（目标值 + 目标值的百分比） */
                uint16_t overpressure_threshold = g_pressure_control_user_target + (g_pressure_control_user_target * PRESSURE_OVERPRESSURE_ALARM_THRESHOLD_MMHG / 100);
                
                /* 条件1：压力超过阈值 */
                if (current_pressure >= overpressure_threshold) {
                    /* 立即触发过压报警（无延迟，快速响应） */
                    g_overpressure_alarm_triggered = true;
                    g_overpressure_alarm_by_build_time = false;  // 条件1触发，标记为非建立时间触发
                }
                
#if ENABLE_LIQUID_FULL_CONDITION2
                /* 条件2：建立负压时间过短（新条件，用于检测液位满，29档查表法） */
                /* 附加条件：建立负压时的起始气压必须小于10mmHg - 已屏蔽，节省空间 */
                if (g_build_start_time_ms > 0) {  // && g_build_start_pressure < 10) {
                    uint32_t current_time_ms = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
                    uint32_t build_duration_ms = current_time_ms - g_build_start_time_ms;
                    
                    /* 根据目标压力查表获取阈值 */
                    uint16_t target = g_pressure_control_user_target;
                    uint16_t index;
                    
                    /* 边界处理：目标压力 < 20 使用20mmHg的阈值，> 300 使用300mmHg的阈值 */
                    if (target < PRESSURE_BUILD_TIME_THRESHOLD_BASE_PRESSURE) {
                        index = 0;  // 使用20mmHg的阈值
                    } else {
                        /* 计算查表索引 */
                        index = (target - PRESSURE_BUILD_TIME_THRESHOLD_BASE_PRESSURE) / PRESSURE_BUILD_TIME_THRESHOLD_STEP_PRESSURE;
                        /* 索引超出范围，使用最后一个值（300mmHg对应的阈值） */
                        if (index >= PRESSURE_BUILD_TIME_THRESHOLD_TABLE_SIZE) {
                            index = PRESSURE_BUILD_TIME_THRESHOLD_TABLE_SIZE - 1;
                        }
                    }
                    
                    /* 查表获取理论时间（毫秒），然后乘以安全系数 */
                    uint32_t theoretical_time_ms = g_build_time_threshold_table[index];
                    uint32_t time_threshold_ms = (uint32_t)(theoretical_time_ms * PRESSURE_BUILD_TIME_SAFETY_FACTOR);
                    
                    /* 如果建立时间小于阈值，触发过压报警 */
                    if (build_duration_ms < time_threshold_ms) {
                        g_overpressure_alarm_triggered = true;
                        g_overpressure_alarm_by_build_time = true;  // 条件2触发，标记为建立时间触发
                    }
                    
                    /* 判断后立即重置开始时间（方案A：判断后立即重置） */
                    g_build_start_time_ms = 0;
                    //g_build_start_pressure = 0;  // 已屏蔽，节省空间
                }
#endif  // ENABLE_LIQUID_FULL_CONDITION2
            }
        }
        
        /* 达到目标压力，清除泄漏报警超时检测 */
        g_leak_alarm_target_reached = true;
        g_leak_alarm_start_time_ms = 0;
        g_leak_alarm_in_range_count = 0;  // 重置区间计数
        
        return;
    }
    
    /* 如果不在保持状态，清除管路堵塞报警检测 */
    if (g_pressure_hold_active) {
        g_pressure_hold_active = false;
        g_blockage_alarm_hold_active = false;
        g_blockage_alarm_hold_start_tick = 0;
        g_blockage_alarm_triggered = false;
    }
    
    /* Step5.5：泄漏报警超时检测（使用动态查表法） */
    if (!g_leak_alarm_target_reached && g_pressure_control_user_target > 0) {
        /* 如果还未达到目标压力，检查是否超时 */
        if (g_leak_alarm_start_time_ms == 0) {
            /* 首次进入，记录开始时间 */
            g_leak_alarm_start_time_ms = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
        } else {
            /* 计算已用时间 */
            uint32_t current_time_ms = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
            uint32_t elapsed_time_ms = current_time_ms - g_leak_alarm_start_time_ms;
            
            /* 根据目标压力动态获取超时时间（查表法） */
            uint32_t timeout_ms = GetLeakAlarmTimeout(g_pressure_control_user_target);
            
            /* 如果还未超时，重置计数器（确保只在超时后开始统计） */
            if (elapsed_time_ms < timeout_ms) {
                g_leak_alarm_in_range_count = 0;
            }
            /* 如果超时，开始统计气压在区间内的次数 */
            else if (elapsed_time_ms >= timeout_ms) {
                /* 检查当前气压是否在区间[min, max]内 */
                if (current_pressure >= PRESSURE_LEAK_ALARM_MIN_MMHG && 
                    current_pressure <= PRESSURE_LEAK_ALARM_MAX_MMHG) {
                    /* 气压在区间内，计数器+1 */
                    g_leak_alarm_in_range_count++;
                    /* 连续N次在区间内，触发泄漏报警 */
                    if (g_leak_alarm_in_range_count >= PRESSURE_LEAK_ALARM_CONSECUTIVE_COUNT) {
                        g_leak_alarm_triggered = true;
                    }
                } else {
                    /* 气压不在区间内，重置计数器 */
                    g_leak_alarm_in_range_count = 0;
                }
            }
        }
    }

    /* Step6：执行 PID 运算，输出范围 -100 ~ 100，正值抽气，负值泄气 */
    float output = PID_Update(&g_pressure_pid, setpoint, measurement);
    g_pressure_last_output = output;
    
    /* Step6.5：管路堵塞报警检测（当不在保持状态时，检查PID输出） */
    if (g_blockage_alarm_hold_active && g_pressure_control_user_target > 0) {
        /* 如果PID输出大于阈值，说明有负压补充（有漏气），重置定时器 */
        if (output > PRESSURE_BLOCKAGE_ALARM_PID_THRESHOLD) {
            /* 有负压补充，重置定时器 */
            g_blockage_alarm_hold_start_tick = SoftTimer_GetTickCount();
            g_blockage_alarm_triggered = false;  // 清除已触发的标志（如果之前已触发）
        } else {
            /* 没有负压补充，检查是否超时 */
            if (g_blockage_alarm_hold_start_tick > 0) {
                uint32_t current_tick = SoftTimer_GetTickCount();
                uint32_t elapsed_ticks = current_tick - g_blockage_alarm_hold_start_tick;
                uint32_t elapsed_time_ms = elapsed_ticks * SOFT_TIMER_TICK_MS;
                
                /* 如果超时且没有负压补充，触发管路堵塞报警 */
                if (elapsed_time_ms >= PRESSURE_BLOCKAGE_ALARM_TIMEOUT_MS) {
                    g_blockage_alarm_triggered = true;
                }
            }
        }
    }

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
// uint16_t AppPressure_GetADCValue(void)
// {
//     return g_pressure_adc_filtered;
// }

/**
 * @name      AppPressure_GetRawADCValue
 * @brief     获取压力传感器ADC原始值（未滤波，最后一次采样值）
 * @retval    ADC采样值（10位：0-1023）
 */
// uint16_t AppPressure_GetRawADCValue(void)
// {
//     return g_pressure_adc_raw;
// }

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

    HAL_Valve2_Close();
    HAL_Valve1_Close();
    g_valve_bleeding_active = false;
    g_bleed_cooldown_ticks = BLEED_COOLDOWN_TICKS;
    AppPressure_CalibrateZero();
}

/**
 * @name      AppPressure_BleedAndCalibrateZero
 * @brief     开机放气并在结束后完成零点校准
 * @note
 *   1. 打开放气阀释放系统残余压力
 *   2. 启动一次性的软定时器（持续时间由PRESSURE_BLEED_DURATION_MS定义）
 *   3. 定时器回调关闭阀门并触发零点校准
 *   4. 若定时器创建失败，则立即关闭阀门并直接校准
 */
void AppPressure_BleedAndCalibrateZero(void)
{
    /* 打开电磁阀释放压力 */
    HAL_Valve2_Open();
    HAL_Valve1_Open();
    g_valve_bleeding_active = true;
    g_bleed_cooldown_ticks = 0U;

    if (g_pressure_bleed_timer != 0) {
        SoftTimer_Stop(g_pressure_bleed_timer);
        SoftTimer_SetPeriod(g_pressure_bleed_timer, PRESSURE_BLEED_DURATION_MS);
        SoftTimer_Start(g_pressure_bleed_timer);
    } else {
        g_pressure_bleed_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                  PRESSURE_BLEED_DURATION_MS,
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
// void AppPressure_SetConversionFactor(float factor)
// {
//     if (factor > 0.0f) {  // 有效性检查
//         g_pressure_conversion_factor = factor;
//     }
// }

/**
 * @name      AppPressure_GetConversionFactor
 * @brief     获取当前转换系数
 * @retval    转换系数（float）
 */
// float AppPressure_GetConversionFactor(void)
// {
//     return g_pressure_conversion_factor;
// }

/**
 * @name      AppPressure_GetZeroOffset
 * @brief     获取零点偏移值（adc_zero）
 * @retval    零点偏移ADC值（10位：0-1023）
 */
// uint16_t AppPressure_GetZeroOffset(void)
// {
//     return g_pressure_zero_offset;
// }

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

    /* 启动泄漏报警超时检测（仅在目标压力大于0时） */
    if (clamped_user_target > 0) {
        g_leak_alarm_triggered = false;
        g_leak_alarm_target_reached = false;
        g_leak_alarm_start_time_ms = 0;  // 在Process中首次检测时记录开始时间
        g_leak_alarm_in_range_count = 0;  // 重置区间计数
    } else {
        /* 目标压力为0，清除泄漏报警检测 */
        g_leak_alarm_triggered = false;
        g_leak_alarm_target_reached = true;
        g_leak_alarm_start_time_ms = 0;
        g_leak_alarm_in_range_count = 0;  // 重置区间计数
    }
    
    /* 重置管路堵塞报警检测 */
    g_blockage_alarm_triggered = false;
    g_blockage_alarm_hold_active = false;
    g_blockage_alarm_hold_start_tick = 0;
    
    /* 重置过压报警检测 */
    g_overpressure_alarm_triggered = false;
    g_overpressure_alarm_by_build_time = false;
    g_overpressure_alarm_delay_active = false;
    g_overpressure_alarm_start_tick = 0;
    
    /* 设置泵工作原因：启动压力控制时为建立负压 */
    g_pump_work_reason = PUMP_REASON_BUILDING;
    
#if ENABLE_LIQUID_FULL_CONDITION2
    /* 记录建立负压开始时间（用于液位满报警检测） */
    g_build_start_time_ms = SoftTimer_GetTickCount() * SOFT_TIMER_TICK_MS;
    //g_build_start_pressure = AppPressure_GetPressureValue();  // 已屏蔽，节省空间
#endif  // ENABLE_LIQUID_FULL_CONDITION2

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

    /* 停止控制时清除泄漏报警超时检测 */
    g_leak_alarm_triggered = false;
    g_leak_alarm_target_reached = true;
    g_leak_alarm_start_time_ms = 0;
    g_leak_alarm_in_range_count = 0;  // 重置区间计数
    
    /* 停止控制时清除管路堵塞报警检测 */
    g_blockage_alarm_triggered = false;
    g_blockage_alarm_hold_active = false;
    g_blockage_alarm_hold_start_tick = 0;
    g_pressure_hold_active = false;
    
    /* 停止控制时清除过压报警检测 */
    g_overpressure_alarm_triggered = false;
    g_overpressure_alarm_delay_active = false;
    g_overpressure_alarm_start_tick = 0;
    
    /* 设置泵工作原因：停止控制时为闲置 */
    g_pump_work_reason = PUMP_REASON_IDLE;
    
#if ENABLE_LIQUID_FULL_CONDITION2
    /* 清除建立负压时间记录 */
    g_build_start_time_ms = 0;
    //g_build_start_pressure = 0;  // 已屏蔽，节省空间
#endif  // ENABLE_LIQUID_FULL_CONDITION2

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
// void AppPressure_UpdateTarget(uint16_t target_mmHg)
// {
//     uint16_t clamped_user_target = PressureControl_ClampTarget(target_mmHg);
//     g_pressure_control_user_target = clamped_user_target;
//     uint16_t adjusted_target = PressureControl_ApplyOffset(clamped_user_target);
//     g_pressure_control_target = PressureControl_ClampTarget(adjusted_target);
// }

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
 * @name      AppPressure_GetLastOutput
 * @brief     获取上一次PID输出值
 * @retval    PID输出值（-100 ~ 100，正值抽气，负值泄气）
 * @note      用于检测是否有负压补充（判断管路是否堵塞）
 */
float AppPressure_GetLastOutput(void)
{
    return g_pressure_last_output;
}

/**
 * @name      AppPressure_HasControlFault
 * @brief     查询控制故障标志
 * @retval    true=存在故障（已强制泄气）, false=无故障
 */
// bool AppPressure_HasControlFault(void)
// {
//     return g_pressure_control_fault;
// }

/**
 * @name      AppPressure_ClearControlFault
 * @brief     清除故障标志（不改变执行器状态）
 */
// void AppPressure_ClearControlFault(void)
// {
//     g_pressure_control_fault = false;
// }

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * @name      GetLeakAlarmTimeout
 * @brief     根据目标压力查表获取漏气报警超时时间（支持多种容器体积）
 * @param     target_mmHg - 目标压力（mmHg）
 * @retval    超时时间（毫秒）
 * @note      使用查表法，基于理论公式 t=(V/S)×ln[760/(760-Pset)] 预计算
 *           查表基准：V=600ml, S=0.42L/min，理论时间×2.5安全余量
 *           支持体积：600ml, 400ml, 140ml（通过SYSTEM_CONTAINER_VOLUME_ML配置）
 *           时间缩放：t_actual = t_base × (V_actual / V_base)
 */
static uint32_t GetLeakAlarmTimeout(uint16_t target_mmHg)
{
    /* 限制在查表范围内 */
    if (target_mmHg < LEAK_ALARM_TIMEOUT_BASE_PRESSURE) {
        target_mmHg = LEAK_ALARM_TIMEOUT_BASE_PRESSURE;  // 最小20mmHg
    }
    
    /* 计算查表索引 */
    uint16_t index = (target_mmHg - LEAK_ALARM_TIMEOUT_BASE_PRESSURE) / LEAK_ALARM_TIMEOUT_STEP_PRESSURE;
    
    /* 索引超出范围，使用最后一个值（300mmHg对应的超时时间） */
    if (index >= LEAK_ALARM_TIMEOUT_TABLE_SIZE) {
        index = LEAK_ALARM_TIMEOUT_TABLE_SIZE - 1;
    }
    
    /* 查表获取600ml容器的基准超时时间（秒） */
    uint32_t base_timeout_sec = (uint32_t)g_leak_alarm_timeout_table[index];
    
    /* 根据实际容器体积缩放超时时间 */
    /* 公式：t_actual = t_base × (V_actual / V_base) */
    /* 为避免浮点运算，使用整数乘除：timeout_ms = (base_timeout_sec * V_actual * 1000) / V_base */
    uint32_t timeout_ms = (base_timeout_sec * SYSTEM_CONTAINER_VOLUME_ML * 1000UL) / LEAK_ALARM_TIMEOUT_BASE_VOLUME_ML;
    
    return timeout_ms;
}

/**
 * @name      PressureFilter_Update
 * @brief     更新滤波缓冲区并计算平均值（参考老版本工程adc_press_filter）
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
    
    /* 边界检查（参考老版本工程） */
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
 * @brief     ADC值转换为mmHg（参考老版本工程DISP_MainA）
 * @param     adc_value - ADC值（滤波后）
 * @retval    压力值（mmHg）
 * @note      转换公式：压力值(mmHg) = (adc_ps0 - adc_zero) / valueK
 */
static uint16_t PressureConvert_ADCToMMHG(uint16_t adc_value)
{
    float pressure_float;
    
    /* 参考老版本工程：if (adc_ps0 > adc_zero) */
    if (adc_value > g_pressure_zero_offset) {
        /* 转换公式：i = (float)(adc_ps0 - adc_zero) / valueK */
        pressure_float = (float)(adc_value - g_pressure_zero_offset) / g_pressure_conversion_factor;
    } else {
        /* 如果ADC值小于等于零点，返回0 */
        pressure_float = 0.0f;
    }
    
    /* 转换为整数（mmHg） */
    uint16_t pressure = (uint16_t)pressure_float;
    
    /* 边界检查：压力范围0-320mmHg（参考老版本工程注释） */
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
    g_valve_bleeding_active = false;
    g_bleed_cooldown_ticks = BLEED_COOLDOWN_TICKS;
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
    g_valve_bleeding_active = true;
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
         * - duty = 动态最小占空比 + 剩余占空比 * normalized，避免进入"嗡嗡不抽"区间
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
        g_valve_bleeding_active = false;
        g_bleed_cooldown_ticks = BLEED_COOLDOWN_TICKS;
        
        /* 泵工作原因保持不变（由外部在状态转换时设置为BUILDING或MAINTAINING） */
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
            g_valve_bleeding_active = true;
            g_bleed_cooldown_ticks = 0U;
        } else {
            HAL_Valve1_Close();
            HAL_Valve2_Close();
            g_valve_bleeding_active = false;
            g_bleed_cooldown_ticks = BLEED_COOLDOWN_TICKS;
        }
        
        /* 非正输出时泵不工作，但不改变原因状态（因为可能马上又需要抽气） */
        /* 只有在StopControl时才会设置为IDLE */
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

/**
 * @name      AppPressure_IsLeakAlarmTriggered
 * @brief     查询是否已触发泄漏报警（超时未达到目标压力）
 * @retval    true=已触发泄漏报警, false=未触发
 */
bool AppPressure_IsLeakAlarmTriggered(void)
{
    return g_leak_alarm_triggered;
}

/**
 * @name      AppPressure_ClearLeakAlarm
 * @brief     清除泄漏报警标志
 */
void AppPressure_ClearLeakAlarm(void)
{
    g_leak_alarm_triggered = false;
    g_leak_alarm_target_reached = true;
    g_leak_alarm_start_time_ms = 0;
    g_leak_alarm_in_range_count = 0;  // 重置区间计数
}

/**
 * @name      AppPressure_IsBlockageAlarmTriggered
 * @brief     查询是否已触发管路堵塞报警（负压稳定后2分钟内没有PID补充）
 * @retval    true=已触发管路堵塞报警, false=未触发
 */
bool AppPressure_IsBlockageAlarmTriggered(void)
{
    return g_blockage_alarm_triggered;
}

/**
 * @name      AppPressure_ClearBlockageAlarm
 * @brief     清除管路堵塞报警标志
 */
void AppPressure_ClearBlockageAlarm(void)
{
    g_blockage_alarm_triggered = false;
    /* 注意：不清除g_blockage_alarm_hold_active和g_blockage_alarm_hold_start_tick */
    /* 因为可能只是暂时清除标志，但检测逻辑应该继续 */
}

/**
 * @name      AppPressure_IsOverpressureAlarmTriggered
 * @brief     查询是否已触发过压报警（入口堵塞导致压力异常高）
 * @retval    true=已触发过压报警, false=未触发
 */
bool AppPressure_IsOverpressureAlarmTriggered(void)
{
    return g_overpressure_alarm_triggered;
}

/**
 * @name      AppPressure_IsOverpressureAlarmByBuildTime
 * @brief     查询过压报警是否由建立时间过短触发（条件2：液位满）
 * @retval    true=由建立时间过短触发, false=由压力超过阈值触发（条件1）
 * @note      用于区分两种触发方式，液位满报警不应因压力稳定而自动退出
 */
bool AppPressure_IsOverpressureAlarmByBuildTime(void)
{
    return g_overpressure_alarm_by_build_time;
}

/**
 * @name      AppPressure_ClearOverpressureAlarm
 * @brief     清除过压报警标志
 */
void AppPressure_ClearOverpressureAlarm(void)
{
    g_overpressure_alarm_triggered = false;
    g_overpressure_alarm_by_build_time = false;
    g_overpressure_alarm_delay_active = false;
    g_overpressure_alarm_start_tick = 0;
}

/**
 * @name      AppPressure_GetPressureDeviation
 * @brief     获取当前压力与目标的偏差（用于UI显示和判断）
 * @retval    压力偏差（mmHg），正值表示超过目标，负值表示低于目标
 */
int16_t AppPressure_GetPressureDeviation(void)
{
    uint16_t current_pressure = AppPressure_GetPressureValue();
    int16_t deviation = (int16_t)current_pressure - (int16_t)g_pressure_control_user_target;
    return deviation;
}

/**
 * @name      AppPressure_GetPumpWorkReason
 * @brief     获取当前泵工作原因
 * @retval    泵工作原因枚举值
 */
// PumpWorkReason_e AppPressure_GetPumpWorkReason(void)
// {
//     return g_pump_work_reason;
// } 
 

