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
#include "system_enums.h"  // 必须先包含枚举定义

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
	bool     control_enabled;       // 控制使能
} PressureData_t;

/* 为了兼容旧接口 */
typedef PressureData_t PressureControl_t;

/****************************************************************************
 * 间歇模式控制数据
 ****************************************************************************/
typedef struct {
	IntermittentPhase_e current_phase;  // 当前阶段
	uint16_t high_pressure;             // 高压目标值（mmHg）
	uint16_t low_pressure;              // 低压目标值（mmHg）
	uint16_t high_time;                 // 高压持续时间（分钟）
	uint16_t low_time;                  // 低压持续时间（分钟）
	uint16_t stop_time;                 // 停顿时间（分钟）
	uint16_t phase_timer;               // 阶段计时器（秒）
} IntermittentControl_t;

/****************************************************************************
 * 系统设置数据
 ****************************************************************************/
typedef struct {
	SettingItem_e current_item;     // 当前设置项
	uint8_t  cursor_position;       // 光标位置
	uint16_t continuous_time;       // 连续模式时间（分钟）
	bool     is_in_setting_mode;    // 是否在设置模式
} SystemSettings_t;

/****************************************************************************
 * 电池管理器数据结构
 ****************************************************************************/
typedef struct {
	uint8_t  level;                 // 电量等级：0-4
	uint8_t  percentage;            // 电量百分比：0-100
	uint16_t voltage_adc;           // 电压ADC值
	bool     is_charging;           // 是否充电中
	bool     is_low;                // 是否低电
	uint16_t shutdown_timer;        // 关机倒计时
} BatteryData_t;

/* 电池管理器类型别名（必须在 SystemState_t 之前定义） */
typedef BatteryData_t BatteryManager_t;

/****************************************************************************
 * 故障检测器数据结构
 ****************************************************************************/
typedef struct {
	ErrorCode_e active_error;       // 当前故障
	ErrorCode_e current_error;      // 当前故障（别名）
	bool     leakage_detected;      // 泄漏检测
	bool     blockage_detected;     // 堵塞检测
	bool     liquid_full;           // 液位满
	bool     sensor_error;          // 传感器故障
	bool     overpressure;          // 过压故障
	uint16_t leakage_timer;         // 泄漏计时器
	uint16_t leakage_count;         // 泄漏次数计数
	uint32_t blockage_timer;        // 堵塞计时器
	uint16_t pressure_history[8];   // 压力历史记录
	uint8_t  history_index;         // 历史记录索引
} FaultData_t;

/* 故障检测器类型别名（必须在 SystemState_t 之前定义） */
typedef FaultData_t FaultDetector_t;

/****************************************************************************
 * 报警管理器数据结构
 ****************************************************************************/
typedef struct {
	bool     is_muted;              // 是否静音
	uint16_t mute_timer;            // 静音计时器
	AlarmType_e alarm_type;         // 报警类型（使用枚举类型，避免精度丢失）
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
	bool     backlight_on;          // 背光状态
	uint16_t backlight_timer;       // 背光计时器
	bool     is_locked;             // 按键锁定
	uint16_t lock_timer;            // 锁定计时器
} UIData_t;

/* 注意：语言选项已移除，新架构仅支持英语显示 */

/* 注意：FaultDetector_t 和 BatteryManager_t 类型别名定义在下方 */

/****************************************************************************
 * 系统状态结构
 ****************************************************************************/
typedef struct {
	WorkMode_e           current_mode;      // 当前工作模式
	WorkMode_e           previous_mode;     // 上一个模式（用于暂停恢复）
	PressureControl_t    pressure;          // 压力控制
	IntermittentControl_t intermittent;     // 间歇模式控制
	BatteryManager_t     battery;           // 电池管理
	FaultDetector_t      fault;             // 故障检测
	SystemSettings_t     settings;          // 系统设置
	uint32_t             uptime_ms;         // 系统运行时间（毫秒）
	uint16_t             idle_time_sec;     // 空闲时间（秒）
	uint16_t             idle_timer_sec;    // 空闲计时器
	bool                 idle_timeout_flag; // 空闲超时标志
	
	/* 以下为兼容旧结构 */
	AlarmData_t     alarm;          // 报警数据
	PumpData_t      pump;           // 气泵数据
	UIData_t        ui;             // UI数据
	bool            system_ready;   // 系统就绪标志
	bool            mute_enabled;   // 静音使能
	bool            key_locked;     // 按键锁定
} SystemState_t;

/****************************************************************************
 * Flash配置数据结构
 ****************************************************************************/
typedef struct {
	uint16_t pressure_setting;      // 压力设置
	float    calibration_k1;        // 校准系数K1
	float    calibration_k2;        // 校准系数K2
	uint16_t usage_hours;           // 使用时长（小时）
	uint32_t crc;                   // 校验值
} FlashConfig_t;

/* 注意：语言设置已移除，仅支持英语 */
/* 注意：BatteryManager_t 和 FaultDetector_t 别名已在上方定义 */

#endif /* SYSTEM_TYPES_H */

