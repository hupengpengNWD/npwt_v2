/****************************************************************************
 * 文件名: system_manager.h
 * 功能: 系统管理器接口声明
 * 
 * 说明: 
 *   提供所有系统状态和控制的访问接口
 *   替代原有的分散全局变量
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include "system_types.h"

/****************************************************************************
 * 系统初始化函数
 ****************************************************************************/
void System_Init(void);

/****************************************************************************
 * 音频控制访问接口
 ****************************************************************************/
AudioControl_t* System_GetAudio(void);
void System_SetAudioFlag(unsigned char flag);
unsigned char System_GetAudioFlag(void);
void System_SetMuteFlag(unsigned char flag);
unsigned char System_GetMuteFlag(void);

/****************************************************************************
 * 电池管理访问接口
 ****************************************************************************/
BatteryManager_t* System_GetBattery(void);
void System_SetBatteryLevel(unsigned char level);
unsigned char System_GetBatteryLevel(void);

/****************************************************************************
 * 压力控制访问接口
 ****************************************************************************/
PressureControl_t* System_GetPressure(void);
void System_SetTargetPressure(unsigned short pressure);
unsigned short System_GetTargetPressure(void);
void System_SetCurrentPressure(unsigned short pressure);
unsigned short System_GetCurrentPressure(void);
void System_SetPressureDelta(unsigned short hi, unsigned short lo);

/****************************************************************************
 * 气泵阀门控制访问接口
 ****************************************************************************/
PumpValveControl_t* System_GetPump(void);
void System_EnablePump(void);
void System_DisablePump(void);
unsigned char System_IsPumpEnabled(void);

/****************************************************************************
 * 故障检测访问接口
 ****************************************************************************/
FaultDetector_t* System_GetFault(void);
void System_SetErrorCode(unsigned char code);
unsigned char System_GetErrorCode(void);
void System_SetFaultFlag(unsigned char flag);
void System_ClearFaultFlag(unsigned char flag);
unsigned char System_GetFaultFlags(void);

/****************************************************************************
 * 系统状态访问接口
 ****************************************************************************/
SystemState_t* System_GetState(void);
void System_SetMode(unsigned char mode);
unsigned char System_GetMode(void);
void System_SetLockFlag(unsigned char flag);
unsigned char System_GetLockFlag(void);

/****************************************************************************
 * 按键控制访问接口
 ****************************************************************************/
KeyControl_t* System_GetKey(void);
void System_SetKeyValue(unsigned char val);
unsigned char System_GetKeyValue(void);

/****************************************************************************
 * 显示控制访问接口
 ****************************************************************************/
DisplayControl_t* System_GetDisplay(void);
void System_SetLanguage(unsigned char lang);
unsigned char System_GetLanguage(void);

/****************************************************************************
 * Flash配置访问接口
 ****************************************************************************/
FlashConfig_t* System_GetFlashConfig(void);

/****************************************************************************
 * 系统标志访问接口
 ****************************************************************************/
SystemFlags_t* System_GetFlags(void);

#endif /* SYSTEM_MANAGER_H */

