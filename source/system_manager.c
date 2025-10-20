/****************************************************************************
 * 文件名: system_manager.c
 * 功能: 系统管理器（单例模式）
 * 
 * 说明: 
 *   封装所有系统状态和控制变量
 *   提供统一的访问接口
 *   降低模块间耦合度
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#include "include.h"
#include "system_types.h"

/****************************************************************************
 * 系统全局状态（私有静态变量）
 ****************************************************************************/

static AudioControl_t     s_audio;      // 音频控制
static BatteryManager_t   s_battery;    // 电池管理
static PressureControl_t  s_pressure;   // 压力控制
static PumpValveControl_t s_pump;       // 气泵阀门控制
static FaultDetector_t    s_fault;      // 故障检测
static SystemState_t      s_system;     // 系统状态
static KeyControl_t       s_key;        // 按键控制
static DisplayControl_t   s_display;    // 显示控制
static FlashConfig_t      s_flash_cfg;  // Flash配置
static SystemFlags_t      s_flags;      // 系统标志

/****************************************************************************
 * 访问接口函数（Getter/Setter）
 ****************************************************************************/

/* === 音频控制访问接口 === */
AudioControl_t* System_GetAudio(void)
{
	return &s_audio;
}

void System_SetAudioFlag(unsigned char flag)
{
	s_audio.audio_flg = flag;
}

unsigned char System_GetAudioFlag(void)
{
	return s_audio.audio_flg;
}

void System_SetMuteFlag(unsigned char flag)
{
	s_audio.mute_flg = flag;
}

unsigned char System_GetMuteFlag(void)
{
	return s_audio.mute_flg;
}

/* === 电池管理访问接口 === */
BatteryManager_t* System_GetBattery(void)
{
	return &s_battery;
}

void System_SetBatteryLevel(unsigned char level)
{
	s_battery.bat_lev = level;
}

unsigned char System_GetBatteryLevel(void)
{
	return s_battery.bat_lev;
}

/* === 压力控制访问接口 === */
PressureControl_t* System_GetPressure(void)
{
	return &s_pressure;
}

void System_SetTargetPressure(unsigned short pressure)
{
	s_pressure.mod_seta_preh = pressure;
}

unsigned short System_GetTargetPressure(void)
{
	return s_pressure.mod_seta_preh;
}

void System_SetCurrentPressure(unsigned short pressure)
{
	s_pressure.adc_ps00 = pressure;
}

unsigned short System_GetCurrentPressure(void)
{
	return s_pressure.adc_ps00;
}

void System_SetPressureDelta(unsigned short hi, unsigned short lo)
{
	s_pressure.con_hi_delta = hi;
	s_pressure.con_lo_delta = lo;
}

/* === 气泵阀门控制访问接口 === */
PumpValveControl_t* System_GetPump(void)
{
	return &s_pump;
}

void System_EnablePump(void)
{
	s_pump.open_bum = 1;
}

void System_DisablePump(void)
{
	s_pump.open_bum = 0;
}

unsigned char System_IsPumpEnabled(void)
{
	return s_pump.open_bum;
}

/* === 故障检测访问接口 === */
FaultDetector_t* System_GetFault(void)
{
	return &s_fault;
}

void System_SetErrorCode(unsigned char code)
{
	s_fault.err_codea = code;
}

unsigned char System_GetErrorCode(void)
{
	return s_fault.err_codea;
}

void System_SetFaultFlag(unsigned char flag)
{
	s_fault.flager_a |= flag;
}

void System_ClearFaultFlag(unsigned char flag)
{
	s_fault.flager_a &= ~flag;
}

unsigned char System_GetFaultFlags(void)
{
	return s_fault.flager_a;
}

/* === 系统状态访问接口 === */
SystemState_t* System_GetState(void)
{
	return &s_system;
}

void System_SetMode(unsigned char mode)
{
	s_system.mod_main_a = mode;
}

unsigned char System_GetMode(void)
{
	return s_system.mod_main_a;
}

void System_SetLockFlag(unsigned char flag)
{
	s_system.lock_flg = flag;
}

unsigned char System_GetLockFlag(void)
{
	return s_system.lock_flg;
}

/* === 按键控制访问接口 === */
KeyControl_t* System_GetKey(void)
{
	return &s_key;
}

void System_SetKeyValue(unsigned char val)
{
	s_key.key_val = val;
}

unsigned char System_GetKeyValue(void)
{
	return s_key.key_val;
}

/* === 显示控制访问接口 === */
DisplayControl_t* System_GetDisplay(void)
{
	return &s_display;
}

void System_SetLanguage(unsigned char lang)
{
	s_display.language = lang;
}

unsigned char System_GetLanguage(void)
{
	return s_display.language;
}

/* === Flash配置访问接口 === */
FlashConfig_t* System_GetFlashConfig(void)
{
	return &s_flash_cfg;
}

/* === 系统标志访问接口 === */
SystemFlags_t* System_GetFlags(void)
{
	return &s_flags;
}

/****************************************************************************
 * 系统初始化函数
 ****************************************************************************/

/**
 * 函数: System_Init
 * 功能: 初始化系统管理器的所有结构体
 * 说明: 在main()函数开始时调用
 */
void System_Init(void)
{
	// 清零所有结构体
	memset(&s_audio, 0, sizeof(AudioControl_t));
	memset(&s_battery, 0, sizeof(BatteryManager_t));
	memset(&s_pressure, 0, sizeof(PressureControl_t));
	memset(&s_pump, 0, sizeof(PumpValveControl_t));
	memset(&s_fault, 0, sizeof(FaultDetector_t));
	memset(&s_system, 0, sizeof(SystemState_t));
	memset(&s_key, 0, sizeof(KeyControl_t));
	memset(&s_display, 0, sizeof(DisplayControl_t));
	memset(&s_flash_cfg, 0, sizeof(FlashConfig_t));
	memset(&s_flags, 0, sizeof(SystemFlags_t));
	
	// 设置默认值
	s_pressure.valueK = 2.75f;  // 默认校准系数
	s_audio.audio_period = AUDIO_PERIOD + 400;
	s_audio.audio_flg = LED_BAT_NORMAL;
}

