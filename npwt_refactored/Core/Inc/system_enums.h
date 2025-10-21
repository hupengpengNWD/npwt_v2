/****************************************************************************
 * 文件名: system_enums.h
 * 功能: 系统枚举类型定义
 * 
 * 说明: 
 *   定义系统中使用的所有枚举类型
 *   必须在 system_types.h 之前包含
 * 
 * 创建日期: 2025-10-21
 ****************************************************************************/

#ifndef SYSTEM_ENUMS_H
#define SYSTEM_ENUMS_H

/****************************************************************************
 * 工作模式枚举
 ****************************************************************************/
typedef enum {
	MODE_INIT = 0,          // 初始化模式
	MODE_STANDBY,           // 待机模式
	MODE_CONTINUOUS,        // 连续模式
	MODE_INTERMITTENT,      // 间歇模式
	MODE_PAUSE,             // 暂停模式
	MODE_SETTINGS,          // 设置模式
	MODE_SELFTEST,          // 自检模式
	MODE_ERROR,             // 故障模式
	MODE_SHUTDOWN,          // 关机模式
	MODE_LIX,               // 连续模式（兼容旧代码）
	MODE_JIX                // 间歇模式（兼容旧代码）
} WorkMode_e;

/****************************************************************************
 * 错误代码枚举
 ****************************************************************************/
typedef enum {
	ERROR_NONE = 0,         // 无故障
	ERROR_LEAKAGE,          // 漏气
	ERROR_BLOCKAGE,         // 阻塞
	ERROR_OVERPRESSURE,     // 过压
	ERROR_BATTERY_LOW,      // 电池低电量
	ERROR_BATTERY_CRITICAL, // 电池极低电量
	ERROR_LIQUID_FULL,      // 液满
	ERROR_SENSOR,           // 传感器故障
	ERROR_PUMP,             // 泵故障
	ERROR_VALVE             // 阀门故障
} ErrorCode_e;

/****************************************************************************
 * 间歇模式阶段枚举
 ****************************************************************************/
typedef enum {
	INTERMITTENT_PHASE_HIGH = 0,  // 高压阶段
	INTERMITTENT_PHASE_LOW,       // 低压阶段
	INTERMITTENT_PHASE_STOP       // 停顿阶段
} IntermittentPhase_e;

/****************************************************************************
 * 设置项枚举
 ****************************************************************************/
typedef enum {
	SETTING_NONE = 0,              // 无设置
	SETTING_WORKMODE_SELECT,       // 工作模式选择
	SETTING_CONTINUOUS_TIME,       // 连续模式时间
	SETTING_INTERMITTENT_HIGH_PRESSURE,  // 间歇高压值
	SETTING_INTERMITTENT_LOW_PRESSURE,   // 间歇低压值
	SETTING_INTERMITTENT_HIGH_TIME,      // 间歇高压时间
	SETTING_INTERMITTENT_LOW_TIME,       // 间歇低压时间
	SETTING_INTERMITTENT_STOP_TIME,      // 间歇停顿时间
	SETTING_PRESSURE_CALIBRATION         // 压力校准
} SettingItem_e;

/****************************************************************************
 * 按键值枚举
 ****************************************************************************/
typedef enum {
	KEY_NONE = 0,           // 无按键
	KEY_UP,                 // 上键
	KEY_DOWN,               // 下键
	KEY_CONFIRM,            // 确认键
	KEY_CANCEL,             // 取消键
	KEY_MUT,                // 静音键（短按）
	KEY_MUTL                // 静音键（长按）
} KeyValue_e;

/****************************************************************************
 * 按键事件枚举
 ****************************************************************************/
typedef enum {
	KEY_EVENT_NONE = 0,     // 无事件
	KEY_EVENT_PRESS,        // 按下
	KEY_EVENT_RELEASE,      // 释放
	KEY_EVENT_LONG_PRESS    // 长按
} KeyEvent_e;

/****************************************************************************
 * 蜂鸣器状态枚举
 ****************************************************************************/
typedef enum {
	BEE_STATE_GO = 0,       // 初始状态
	BEE_STATE_ONE,          // 状态1
	BEE_STATE_TWO,          // 状态2
	BEE_STATE_THREE,        // 状态3
	BEE_STATE_END           // 结束状态
} BuzzerState_e;

/****************************************************************************
 * 报警类型枚举
 ****************************************************************************/
typedef enum {
	ALARM_TYPE_NONE = 0,           // 无报警
	ALARM_TYPE_BATTERY_LOW,        // 低电报警
	ALARM_TYPE_BATTERY_CRITICAL,   // 严重低电
	ALARM_TYPE_LEAKAGE,            // 泄漏报警
	ALARM_TYPE_BLOCKAGE,           // 堵塞报警
	ALARM_TYPE_LIQUID_FULL,        // 液位满报警
	ALARM_TYPE_ERROR               // 一般错误
} AlarmType_e;

#endif /* SYSTEM_ENUMS_H */

