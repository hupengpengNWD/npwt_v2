/****************************************************************************
 * 文件名: config_constants.h
 * 功能: 系统常量定义（消除魔术数字）
 * 
 * 说明: 
 *   将代码中所有硬编码的数字定义为有意义的常量
 *   提高代码可读性和可维护性
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef CONFIG_CONSTANTS_H
#define CONFIG_CONSTANTS_H

/****************************************************************************
 * 时间相关常量（单位：主循环周期数，1周期=20ms）
 ****************************************************************************/

/* 电磁阀控制时间 */
#define VALVE1_OPEN_DELAY_CYCLES        50      // 电磁阀1开启延迟：50×20ms = 1秒
#define VALVE1_CLOSE_DELAY_CYCLES       100     // 电磁阀1关闭延迟：100×20ms = 2秒
#define VALVE2_DEGAS_SHORT_CYCLES       4       // 放气阀短时放气：4×20ms = 0.08秒
#define VALVE2_DEGAS_LONG_CYCLES        16      // 放气阀长时放气：16×20ms = 0.32秒（低压时）
#define VALVE2_STABILIZE_CYCLES         50      // 压力稳定等待：50×20ms = 1秒

/* 压力控制时间 */
#define PRESSURE_HOLD_MIN_CYCLES        15      // 压力保持最小时间
#define PRESSURE_HOLD_FACTOR            20      // 压力保持时间系数
#define PRESSURE_CHECK_AFTER_STOP       499     // 停泵后压力检测延迟

/* 故障检测时间 */
#define LEAK_CANCEL_TIME_CYCLES         200     // 取消泄漏标志：200×20ms = 4秒
#define CANISTER_FULL_CHECK_TIME        8000    // 液位满判断时间：8000×20ms = 160秒
#define BLOCKAGE_RECORD_INTERVAL        3000    // 堵塞检测记录间隔
#define BLOCKAGE_RECORD_INTERVAL_LOW    5000    // 低压下堵塞检测记录间隔

/* 补气控制时间 */
#define PUMP_RESTART_DELAY_CYCLES       10      // 补气重启延迟
#define PUMP_NORMAL_CHECK_CYCLES        49      // 正常压力检测周期
#define DEGAS_TIMEOUT_CYCLES            1000    // 放气超时（目标压力为0时）

/****************************************************************************
 * 压力相关常量（单位：mmHg）
 ****************************************************************************/

/* 压力范围 */
#define PRESSURE_MIN_THRESHOLD          50      // 最低压力阈值（用于延时判断）
#define PRESSURE_REPORT_MIN             70      // 堵塞报告最小压力
#define PRESSURE_NO_REPORT_MAX          57      // 不报告堵塞的压力上限
#define PRESSURE_ABSOLUTE_MAX           320     // 压力绝对最大值

/* 压力判断阈值 */
#define PRESSURE_ZERO_OFFSET            20      // 零点压力偏移
#define PRESSURE_DELTA_MIN              5       // 压力差最小值（mmHg）

/****************************************************************************
 * 计数器阈值
 ****************************************************************************/

/* 故障检测计数 */
#define CANISTER_FULL_COUNT_THRESHOLD   4       // 液位满判断：膨胀次数阈值
#define BLOCKAGE_STEADY_COUNT           7       // 堵塞判断：稳定次数阈值
#define GOOD_CHECK_RESET_COUNT          49      // 正常检测重置计数

/* 间歇模式时间因子 */
#define JX_MODE_EXTRA_DELAY_CYCLES      4       // 间歇模式额外延迟

/****************************************************************************
 * ADC相关常量
 ****************************************************************************/

/* 采集周期 */
#define ADC_SAMPLE_INTERVAL_CYCLES      4       // ADC采样间隔：每5次中断采样一次

/****************************************************************************
 * PWM相关常量
 ****************************************************************************/

/* PWM占空比设定值（对应40%-60%） */
#define PWM_DUTY_40_PERCENT             4       // 40%占空比
#define PWM_DUTY_50_PERCENT             5       // 50%占空比
#define PWM_DUTY_60_PERCENT             6       // 60%占空比

/* 压力段判断阈值 */
#define PRESSURE_SEGMENT_LOW            20      // 低压段上限
#define PRESSURE_SEGMENT_MID_LOW        70      // 中低压段上限
#define PRESSURE_SEGMENT_MID_HIGH       100     // 中高压段上限
#define PRESSURE_SEGMENT_HIGH           290     // 高压段上限

/****************************************************************************
 * 系统状态常量
 ****************************************************************************/

/* 蜂鸣器状态 */
#define BUZZER_STATE_STOPPED            1       // 蜂鸣器已停止
#define BUZZER_STATE_RUNNING            0       // 蜂鸣器正在响

/* 排气标志掩码 */
#define PUMP_ENABLE_MASK                0xf0    // 允许气泵工作的掩码
#define PUMP_DISABLE_MASK               0x0f    // 禁止气泵工作的掩码

/* 按键长按时间 */
#define KEY_LONG_PRESS_THRESHOLD        100     // 长按判断阈值：100×20ms = 2秒

/* 初始化延迟 */
#define INIT_BOOT_SCREEN_TIME           50      // 开机画面时间
#define INIT_VERSION_DISPLAY_TIME       5000    // 版本显示时间
#define INIT_TOTAL_TIME                 7000    // 总初始化时间

/* 按键启动判断 */
#define KEY_START_MIN_HOLD_TIME         30      // 按键启动最小保持时间

/****************************************************************************
 * 字库相关常量
 ****************************************************************************/
#define FONT_6X12_BYTES_PER_CHAR        12      // 6×12字体每字符字节数
#define FONT_8X16_BYTES_PER_CHAR        24      // 8×16字体每字符字节数（英文）
#define FONT_CHINESE_BYTES_PER_CHAR     24      // 中文字符每字符字节数

#endif /* CONFIG_CONSTANTS_H */

