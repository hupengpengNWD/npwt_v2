/****************************************************************************
 * 文件名: system_types.h
 * 功能: 系统数据类型定义
 * 
 * 说明: 
 *   定义系统中使用的所有数据结构
 *   采用模块化设计，每个模块有独立的结构体
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 压力控制器数据结构
 ****************************************************************************/
typedef struct {
	uint16_t target_pressure;       // 目标压力（mmHg）
	uint16_t current_pressure;      // 当前压力（mmHg）
	uint16_t upper_threshold;       // 上限阈值
	uint16_t lower_threshold;       // 下限阈值
	uint16_t adc_zero;              // 零点校准值
	float    calibration_k;         // 校准系数
	bool     is_stable;             // 压力是否稳定
} PressureData_t;

/****************************************************************************
 * 电池管理器数据结构
 ****************************************************************************/
typedef struct {
	uint8_t  level;                 // 电量等级：0-4
	uint16_t voltage_adc;           // 电压ADC值
	bool     is_charging;           // 是否充电中
	bool     is_low;                // 是否低电
	uint16_t shutdown_timer;        // 关机倒计时
} BatteryData_t;

/****************************************************************************
 * 故障检测器数据结构
 ****************************************************************************/
typedef struct {
	ErrorCode_e active_error;       // 当前故障
	bool     leakage_detected;      // 泄漏检测
	bool     blockage_detected;     // 堵塞检测
	bool     liquid_full;           // 液位满
	uint16_t leakage_timer;         // 泄漏计时器
	uint32_t blockage_timer;        // 堵塞计时器
	uint16_t pressure_history[8];   // 压力历史记录
	uint8_t  history_index;         // 历史记录索引
} FaultData_t;

/****************************************************************************
 * 报警管理器数据结构
 ****************************************************************************/
typedef struct {
	bool     is_muted;              // 是否静音
	uint16_t mute_timer;            // 静音计时器
	uint8_t  alarm_type;            // 报警类型
	uint16_t beep_counter;          // 蜂鸣计数器
	bool     beep_active;           // 蜂鸣器是否激活
} AlarmData_t;

/****************************************************************************
 * 气泵控制器数据结构
 ****************************************************************************/
typedef struct {
	bool     is_enabled;            // 气泵使能
	uint8_t  pwm_duty;              // PWM占空比
	bool     valve1_open;           // 电磁阀1状态
	bool     valve2_open;           // 电磁阀2状态
	uint16_t degas_timer;           // 放气计时器
} PumpData_t;

/****************************************************************************
 * UI管理器数据结构
 ****************************************************************************/
typedef struct {
	uint8_t  language;              // 当前语言
	bool     backlight_on;          // 背光状态
	uint16_t backlight_timer;       // 背光计时器
	bool     is_locked;             // 按键锁定
	uint16_t lock_timer;            // 锁定计时器
} UIData_t;

/****************************************************************************
 * 系统状态结构
 ****************************************************************************/
typedef struct {
	WorkMode_e      mode;           // 当前工作模式
	PressureData_t  pressure;       // 压力数据
	BatteryData_t   battery;        // 电池数据
	FaultData_t     fault;          // 故障数据
	AlarmData_t     alarm;          // 报警数据
	PumpData_t      pump;           // 气泵数据
	UIData_t        ui;             // UI数据
	uint32_t        uptime_ms;      // 系统运行时间
	bool            system_ready;   // 系统就绪标志
} SystemState_t;

/****************************************************************************
 * Flash配置数据结构
 ****************************************************************************/
typedef struct {
	uint16_t pressure_setting;      // 压力设置
	uint8_t  language_setting;      // 语言设置
	float    calibration_k1;        // 校准系数K1
	float    calibration_k2;        // 校准系数K2
	uint16_t usage_hours;           // 使用时长（小时）
	uint32_t crc;                   // 校验值
} FlashConfig_t;

#endif /* SYSTEM_TYPES_H */

