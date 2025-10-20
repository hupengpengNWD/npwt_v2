/****************************************************************************
 * 文件名: system_manager.c
 * 功能: 系统管理器（单例模式）
 * 
 * 说明: 
 *   封装所有系统状态和控制变量
 *   提供统一的访问接口
 *   降低模块间耦合度
 *   
 *   【兼容模式】：
 *   为了实现全面重构的平滑过渡，本文件提供了兼容层宏定义
 *   旧代码可以继续使用原来的全局变量名
 *   底层已经改为结构体成员访问，但对上层透明
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#include "include.h"
#include "system_types.h"
#include <string.h>

/****************************************************************************
 * 系统全局状态（全局变量，供兼容层使用）
 * 
 * 说明：这些结构体变量在此处定义，其他文件通过兼容层宏访问
 ****************************************************************************/

AudioControl_t     g_audio;      // 音频控制
BatteryManager_t   g_battery;    // 电池管理
PressureControl_t  g_pressure;   // 压力控制
PumpValveControl_t g_pump;       // 气泵阀门控制
FaultDetector_t    g_fault;      // 故障检测
SystemState_t      g_system;     // 系统状态
KeyControl_t       g_key;        // 按键控制
DisplayControl_t   g_display;    // 显示控制
FlashConfig_t      g_flash_cfg;  // Flash配置
SystemFlags_t      g_flags;      // 系统标志

/****************************************************************************
 * 访问接口函数（Getter/Setter）
 ****************************************************************************/

/* === 音频控制访问接口 === */
AudioControl_t* System_GetAudio(void)
{
	return &g_audio;
}

void System_SetAudioFlag(unsigned char flag)
{
	g_audio.audio_flg = flag;
}

unsigned char System_GetAudioFlag(void)
{
	return g_audio.audio_flg;
}

void System_SetMuteFlag(unsigned char flag)
{
	g_audio.mute_flg = flag;
}

unsigned char System_GetMuteFlag(void)
{
	return g_audio.mute_flg;
}

/* === 电池管理访问接口 === */
BatteryManager_t* System_GetBattery(void)
{
	return &g_battery;
}

void System_SetBatteryLevel(unsigned char level)
{
	g_battery.bat_lev = level;
}

unsigned char System_GetBatteryLevel(void)
{
	return g_battery.bat_lev;
}

/* === 压力控制访问接口 === */
PressureControl_t* System_GetPressure(void)
{
	return &g_pressure;
}

void System_SetTargetPressure(unsigned short pressure)
{
	g_pressure.mod_seta_preh = pressure;
}

unsigned short System_GetTargetPressure(void)
{
	return g_pressure.mod_seta_preh;
}

void System_SetCurrentPressure(unsigned short pressure)
{
	g_pressure.adc_ps00 = pressure;
}

unsigned short System_GetCurrentPressure(void)
{
	return g_pressure.adc_ps00;
}

void System_SetPressureDelta(unsigned short hi, unsigned short lo)
{
	g_pressure.con_hi_delta = hi;
	g_pressure.con_lo_delta = lo;
}

/* === 气泵阀门控制访问接口 === */
PumpValveControl_t* System_GetPump(void)
{
	return &g_pump;
}

void System_EnablePump(void)
{
	g_pump.open_bum = 1;
}

void System_DisablePump(void)
{
	g_pump.open_bum = 0;
}

unsigned char System_IsPumpEnabled(void)
{
	return g_pump.open_bum;
}

/* === 故障检测访问接口 === */
FaultDetector_t* System_GetFault(void)
{
	return &g_fault;
}

void System_SetErrorCode(unsigned char code)
{
	g_fault.err_codea = code;
}

unsigned char System_GetErrorCode(void)
{
	return g_fault.err_codea;
}

void System_SetFaultFlag(unsigned char flag)
{
	g_fault.flager_a |= flag;
}

void System_ClearFaultFlag(unsigned char flag)
{
	g_fault.flager_a &= ~flag;
}

unsigned char System_GetFaultFlags(void)
{
	return g_fault.flager_a;
}

/* === 系统状态访问接口 === */
SystemState_t* System_GetState(void)
{
	return &g_system;
}

void System_SetMode(unsigned char mode)
{
	g_system.mod_main_a = mode;
}

unsigned char System_GetMode(void)
{
	return g_system.mod_main_a;
}

void System_SetLockFlag(unsigned char flag)
{
	g_system.lock_flg = flag;
}

unsigned char System_GetLockFlag(void)
{
	return g_system.lock_flg;
}

/* === 按键控制访问接口 === */
KeyControl_t* System_GetKey(void)
{
	return &g_key;
}

void System_SetKeyValue(unsigned char val)
{
	g_key.key_val = val;
}

unsigned char System_GetKeyValue(void)
{
	return g_key.key_val;
}

/* === 显示控制访问接口 === */
DisplayControl_t* System_GetDisplay(void)
{
	return &g_display;
}

void System_SetLanguage(unsigned char lang)
{
	g_display.language = lang;
}

unsigned char System_GetLanguage(void)
{
	return g_display.language;
}

/* === Flash配置访问接口 === */
FlashConfig_t* System_GetFlashConfig(void)
{
	return &g_flash_cfg;
}

/* === 系统标志访问接口 === */
SystemFlags_t* System_GetFlags(void)
{
	return &g_flags;
}

/****************************************************************************
 * 注意：兼容层宏定义已移至 global_compat.h
 * 所有需要访问这些变量的文件应include该头文件
 ****************************************************************************/

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
	memset(&g_audio, 0, sizeof(AudioControl_t));
	memset(&g_battery, 0, sizeof(BatteryManager_t));
	memset(&g_pressure, 0, sizeof(PressureControl_t));
	memset(&g_pump, 0, sizeof(PumpValveControl_t));
	memset(&g_fault, 0, sizeof(FaultDetector_t));
	memset(&g_system, 0, sizeof(SystemState_t));
	memset(&g_key, 0, sizeof(KeyControl_t));
	memset(&g_display, 0, sizeof(DisplayControl_t));
	memset(&g_flash_cfg, 0, sizeof(FlashConfig_t));
	memset(&g_flags, 0, sizeof(SystemFlags_t));
	
	// 设置默认值
	g_pressure.valueK = 2.75f;  // 默认校准系数
	g_audio.audio_period = AUDIO_PERIOD + 400;
	g_audio.audio_flg = LED_BAT_NORMAL;
}

