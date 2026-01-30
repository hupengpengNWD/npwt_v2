/****************************************************************************
 * 文件名: system_config.h
 * 功能: 系统配置和常量定义
 * 作者: 韦睿医疗
 * 说明: 
 *   集中管理所有系统配置参数和常量
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/*******************************************************************battery_changed*********
 * MCU配置
 ****************************************************************************/
//#define MCU_CLOCK_HZ            32000000UL  // 系统时钟：32MHz
//#define SYSTEM_TICK_MS          20          // 系统周期：20ms

/****************************************************************************
 * 压力控制参数
 ****************************************************************************/
#define PRESSURE_BLEED_DURATION_MS                        4000  // 开机泄压持续时间：4000ms（4秒）

#define PRESSURE_LEAK_ALARM_MIN_MMHG                         1  // 泄漏报警气压区间最小值：1mmHg（排除传感器故障，0或负值）
#define PRESSURE_LEAK_ALARM_MAX_MMHG                        15  // 泄漏报警气压区间最大值：5mmHg（超时且气压在此区间内连续N次则触发）
#define PRESSURE_LEAK_ALARM_CONSECUTIVE_COUNT                5  // 泄漏报警连续计数阈值：连续5次（50ms）气压在区间内则触发
#define PRESSURE_LEAK_ALARM_PUMP_STOP_DELAY_MS           60000  // 泄漏报警后延迟停止泵电机时间：60000ms（60秒/1分钟）

#define PRESSURE_BLOCKAGE_ALARM_TIMEOUT_MS              120000  // 管路堵塞报警超时时间：120000ms（120s/2min），负压稳定后2分钟内没有PID补充则报警
#define PRESSURE_BLOCKAGE_ALARM_PID_THRESHOLD            10.0f  // 管路堵塞报警PID输出阈值：10.0，超过此值认为有负压补充，重置定时器

#define PRESSURE_OVERPRESSURE_ALARM_THRESHOLD_MMHG          20  // 过压报警阈值百分比：压力超过目标值+目标值的20%时触发（检测收集罐已满）
#define PRESSURE_OVERPRESSURE_ALARM_DELAY_MS               500  // 过压报警延迟时间：500ms，避免瞬态误报（设为0则立即触发）

#define PRESSURE_CONTROL_TARGET_OFFSET                      4U  // 控制用目标偏移（防止停泵后下跌）：用户目标值+4mmHg作为内部控制目标
#define PRESSURE_CONTROL_DEADBAND_MMHG                    5.0f  // 允许的稳态误差（死区）：压力达到内部控制目标+5mmHg时进入(稳定)保持状态
#define PRESSURE_CONTROL_REENGAGE_THRESHOLD_MMHG          5.0f  // 再次介入需要超过的误差：压力下降到用户目标值-5mmHg时重新启动PID控制

/****************************************************************************
 * 系统语言配置
 ****************************************************************************/
#ifndef DEFAULT_LANGUAGE
#define DEFAULT_LANGUAGE    1  // 默认语言：1=英文, 2=中文, 3=俄文
#endif

/****************************************************************************
 * 开机Logo选择配置
 ****************************************************************************/
#ifndef STARTUP_LOGO_SELECT
#define STARTUP_LOGO_SELECT    0  // 开机Logo选择：0=VR, 1=Dynarex, 2=Savage（Averto ）, 3=foreign
#endif

/****************************************************************************
 * 设备版本信息配置
 ****************************************************************************/
#ifndef DEVICE_VERSION_STRING
#define DEVICE_VERSION_STRING    "Vcare1000-300se.1.01"  // 设备版本号
#endif

/****************************************************************************
 * 默认治疗模式配置
 ****************************************************************************/
#ifndef DEFAULT_WORK_MODE
#define DEFAULT_WORK_MODE    2  // 默认治疗模式：2=连续模式(UI_STATE_LIX), 3=间歇模式(UI_STATE_JIX)
#endif

/****************************************************************************
 * UI压力和时间设置参数
 ****************************************************************************/
/* 压力设置范围 */
#define PRESSURE_HIGH_MAX     300    // 最大压力值（mmHg）
#define PRESSURE_HIGH_MIN     20     // 最小压力值（mmHg）
#define PRESSURE_LOW_MAX      100    // 最大低压值（mmHg）
#define PRESSURE_LOW_MIN      10     // 最小低压值（mmHg）
#define PRESSURE_STEP         5      // 压力调整步进值（mmHg）

/* 时间设置范围 */
#define TIME_HIGH_MAX         99     // 最大高压时间（min）
#define TIME_HIGH_MIN         1      // 最小高压时间（min）
#define TIME_LOW_MAX          99     // 最大低压时间（min）
#define TIME_LOW_MIN          1      // 最小低压时间（min）
#define TIME_STEP             1      // 时间调整步进值（min）

/* 默认值 */
#define PRESSURE_DEFAULT          120    // 默认（高）压力（mmHg）
#define PRESSURE_LOW_DEFAULT      80     // 默认低压值（mmHg）
#define TIME_HIGH_DEFAULT         1      // 默认高压时间（min）
#define TIME_LOW_DEFAULT          1      // 默认低压时间（min）

/****************************************************************************
 * UI显示刷新控制参数
 ****************************************************************************/
#define UI_PRESSURE_REFRESH_INTERVAL_TICKS   20       // 连续模式压力刷新间隔（10ms Tick）；20=200ms
#define UI_PRESSURE_REFRESH_THRESHOLD_MMHG   0        // 压力最小刷新差值阈值（mmHg）

/****************************************************************************
 * UI连续调节控制参数
 ****************************************************************************/
#ifndef ENABLE_CONTINUOUS_ADJUST
#define ENABLE_CONTINUOUS_ADJUST    0       // 是否启用连续调节功能：0=禁用（节省Flash空间），1=启用
#endif
#if (ENABLE_CONTINUOUS_ADJUST == 1)
#define UI_CONTINUOUS_ADJUST_INTERVAL_MS     500      // 连续调节间隔（毫秒）：按下时每500ms调节一次
#endif

/****************************************************************************
 * UI超时控制参数
 ****************************************************************************/
#define UI_LOCK_TIMEOUT_TICKS                3000U    // 自动锁定超时时间：30s @10ms Tick
#define UI_IDLE_TIMEOUT_TICKS                6000U    // 空闲超时时间：1min @10ms Tick (60s)

/* 基于建立负压时间的液位满报警阈值（29档，查表法） */
/* 算法2：建立负压时间过短检测（液位满报警） */
#ifndef ENABLE_LIQUID_FULL_CONDITION2
#define ENABLE_LIQUID_FULL_CONDITION2  0  // 默认关闭，节省代码空间
#endif

#if ENABLE_LIQUID_FULL_CONDITION2
#define PRESSURE_BUILD_TIME_SAFETY_FACTOR  2.5f  // 建立时间安全系数（理论时间×此系数）
#define PRESSURE_BUILD_TIME_THRESHOLD_BASE_PRESSURE  20U   // 查表基准压力：20 mmHg
#define PRESSURE_BUILD_TIME_THRESHOLD_STEP_PRESSURE  10U   // 查表步进：10 mmHg
#define PRESSURE_BUILD_TIME_THRESHOLD_TABLE_SIZE  29U      // 查表大小：29档（20-300mmHg，步进10）
#endif

/****************************************************************************
 * 容器体积配置
 * 说明：
 *   - 漏气报警超时时间与容器体积成正比：t ∝ V
 *   - 理论公式：t = (V/S) × ln[760/(760-Pset)]
 *   - 已知泵流速：S = 0.42 L/min
 *   - 支持体积：600ml(默认)、400ml、140ml
 ****************************************************************************/
/* 支持的容器体积选项（单位：ml） */
#define CONTAINER_VOLUME_600ML    600
#define CONTAINER_VOLUME_400ML    400
#define CONTAINER_VOLUME_140ML    140

/* 当前系统使用的容器体积（修改此处选择不同容器） */
#ifndef SYSTEM_CONTAINER_VOLUME_ML
    #define SYSTEM_CONTAINER_VOLUME_ML    CONTAINER_VOLUME_600ML  // 默认600ml
#endif

/* 漏气报警查表基准体积（查表数组是基于600ml容器计算的） */
#define LEAK_ALARM_TIMEOUT_BASE_VOLUME_ML    600

/****************************************************************************
 * 电池电压阈值（ADC值）
 ****************************************************************************/ 
#define BAT_LEVEL_FULL          319         // 满电
#define BAT_LEVEL_75            304         // 75%
#define BAT_LEVEL_50            296         // 50%
#define BAT_LEVEL_25            277         // 25%
#define BAT_LEVEL_LOW           269         // 低电
#define BAT_LEVEL_CRITICAL      260         // 严重低电报警阈值（约≈3.45V）用于触发 ALARM_TYPE_BATTERY_CRITICAL 判定


#endif /* SYSTEM_CONFIG_H */

