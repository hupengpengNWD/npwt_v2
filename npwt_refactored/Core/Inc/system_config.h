/****************************************************************************
 * 文件名: system_config.h
 * 功能: 系统配置和常量定义
 * 
 * 说明: 
 *   集中管理所有系统配置参数和常量
 *   替代原代码中的魔术数字
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

/****************************************************************************
 * MCU配置
 ****************************************************************************/
#define MCU_CLOCK_HZ            32000000UL  // 系统时钟：32MHz
#define SYSTEM_TICK_MS          20          // 系统周期：20ms

/****************************************************************************
 * 压力控制参数
 ****************************************************************************/
#define PRESSURE_MIN            20          // 最小压力：20mmHg
#define PRESSURE_MAX            300         // 最大压力：300mmHg
#define PRESSURE_DEFAULT        120         // 默认压力：120mmHg
#define PRESSURE_TOLERANCE_LOW  10          // 下限容差：10%
#define PRESSURE_TOLERANCE_HIGH 5           // 上限容差：5%
#define PRESSURE_BLEED_DURATION_MS  3000    // 开机泄压持续时间：3000ms（3秒）
#define PRESSURE_LEAK_ALARM_TIMEOUT_MS  30000  // 泄漏报警超时时间：30000ms（30秒），PID建立负压时如果超时未达到目标值则报警
#define PRESSURE_LEAK_ALARM_PRESSURE_THRESHOLD_MMHG  15  // 泄漏报警气压阈值：超时且当前气压小于此值时触发报警（mmHg，已废弃，现使用区间判断）
#define PRESSURE_LEAK_ALARM_MIN_MMHG  1  // 泄漏报警气压区间最小值：1mmHg（排除传感器故障，0或负值）
#define PRESSURE_LEAK_ALARM_MAX_MMHG  15  // 泄漏报警气压区间最大值：5mmHg（超时且气压在此区间内连续N次则触发）
#define PRESSURE_LEAK_ALARM_CONSECUTIVE_COUNT  5  // 泄漏报警连续计数阈值：连续5次（50ms）气压在区间内则触发
#define PRESSURE_LEAK_ALARM_PUMP_STOP_DELAY_MS  60000  // 泄漏报警后延迟停止泵电机时间：60000ms（60秒/1分钟）
#define PRESSURE_BLOCKAGE_ALARM_TIMEOUT_MS  120000  // 管路堵塞报警超时时间：120000ms（120秒/2分钟），负压稳定后2分钟内没有PID补充则报警
#define PRESSURE_BLOCKAGE_ALARM_PID_THRESHOLD  10.0f  // 管路堵塞报警PID输出阈值：10.0，超过此值认为有负压补充，重置定时器
#define PRESSURE_OVERPRESSURE_ALARM_THRESHOLD_MMHG  15  // 过压报警阈值：压力超过目标值+30mmHg时触发（检测收集罐已满）
#define PRESSURE_OVERPRESSURE_ALARM_DELAY_MS  500  // 过压报警延迟时间：500ms，避免瞬态误报（设为0则立即触发）

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

/* 漏气报警查表基准体积（查表数组是基于600ml容器计算的，不要修改） */
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

/****************************************************************************
 * 电池图标显示坐标
 ****************************************************************************/
#define BATTERY_ICON_X          102         // 电池图标X坐标（列坐标）
#define BATTERY_ICON_Y          0           // 电池图标Y坐标（页坐标）

/****************************************************************************
 * 定时器参数（单位：系统周期20ms）
 ****************************************************************************/
#define VALVE_OPEN_DELAY        50          // 阀门开启延迟：1秒
#define VALVE_CLOSE_DELAY       100         // 阀门关闭延迟：2秒
#define DEGAS_SHORT_DURATION    4           // 短放气时间：80ms
#define DEGAS_LONG_DURATION     16          // 长放气时间：320ms
#define PRESSURE_STABLE_TIME    50          // 压力稳定时间：1秒
#define BEEPER_MAX_TIME         15          // 蜂鸣器最大响铃：300ms
#define BATTERY_CHECK_INTERVAL  50          // 电池检查间隔：1秒
#define AUTO_SHUTDOWN_DELAY     500         // 自动关机延迟：10秒
#define IDLE_TIMEOUT            15000       // 空闲超时：5分钟
#define MUTE_TIMEOUT            15000       // 静音超时：5分钟
#define KEY_LONG_PRESS          100         // 长按阈值：2秒

/****************************************************************************
 * PWM参数
 ****************************************************************************/
#define PWM_DUTY_40_PERCENT     4           // 40%占空比
#define PWM_DUTY_50_PERCENT     5           // 50%占空比
#define PWM_DUTY_60_PERCENT     6           // 60%占空比
#define PWM_PERIOD              10          // PWM周期

/****************************************************************************
 * 语言配置（仅支持英语）
 ****************************************************************************/
#define LANGUAGE_ENGLISH_ONLY   1  // 仅英语模式，无需俄语支持

/* 工作模式和错误代码枚举已移至 system_enums.h */

#endif /* SYSTEM_CONFIG_H */

