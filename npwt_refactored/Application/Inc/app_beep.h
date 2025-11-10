/**
  ******************************************************************************
  * @file:    app_beep.h
  * @author:  hupengpeng
  * @date:    2025-01-23
  * @brief:   蜂鸣器应用层控制头文件
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#ifndef APP_BEEP_H
#define APP_BEEP_H

#include "../../Middleware/Inc/pushpull.h"
#include "../../Middleware/Inc/soft_timer.h"
#include <stdint.h>
#include <stdbool.h>

/**
 * @name      AppBeep_Init
 * @brief     初始化蜂鸣器应用层
 * @param     无
 * @retval    无
 */
void AppBeep_Init(void);

/**
 * @name      AppBeep_GetBuzzerInstance
 * @brief     获取蜂鸣器控制实例
 * @param     无
 * @retval    PushPullPtr_t - 蜂鸣器实例指针
 */
PushPullPtr_t AppBeep_GetBuzzerInstance(void);

/**
 * @name      AppBeep_GetBuzzerConfig
 * @brief     获取蜂鸣器配置参数
 * @param     无
 * @retval    const uint16_t* - 时序数组指针
 */
const uint16_t* AppBeep_GetBuzzerConfig(void);

/**
 * @name      AppBeep_GetBuzzer2DConfig
 * @brief     获取蜂鸣器二维时序配置数组
 * @param     无
 * @retval    const uint16_t** - 二维时序数组指针
 */
const uint16_t** AppBeep_GetBuzzer2DConfig(void);

/**
 * @name      AppBeep_GetBuzzer2DCount
 * @brief     获取蜂鸣器二维数组行数
 * @param     无
 * @retval    uint32_t - 二维数组行数
 */
uint32_t AppBeep_GetBuzzer2DCount(void);

/**
 * @name      AppBeep_GetBuzzerSeqLength
 * @brief     获取蜂鸣器时序数组长度
 * @param     无
 * @retval    uint32_t - 时序数组长度
 */
uint32_t AppBeep_GetBuzzerSeqLength(void);

/**
 * @name      AppBeep_GetBuzzerSeqCount
 * @brief     获取蜂鸣器重复次数
 * @param     无
 * @retval    uint32_t - 重复次数
 */
uint32_t AppBeep_GetBuzzerSeqCount(void);

/**
 * @name      AppBeep_BuzzerCallback
 * @brief     蜂鸣器事件回调函数
 * @param     ptr - 蜂鸣器控制指针
 * @param     event - 蜂鸣器事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppBeep_BuzzerCallback(PushPullPtr_t ptr, PushPullEvent_e event, void* arg);

/**
 * @name      AppBeep_BuzzerWriteHigh
 * @brief     蜂鸣器写高电平函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    无
 */
void AppBeep_BuzzerWriteHigh(void* gpio_drv_ptr);

/**
 * @name      AppBeep_BuzzerWriteLow
 * @brief     蜂鸣器写低电平函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    无
 */
void AppBeep_BuzzerWriteLow(void* gpio_drv_ptr);

/**
 * @name      AppBeep_BuzzerToggle
 * @brief     蜂鸣器翻转函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    无
 */
void AppBeep_BuzzerToggle(void* gpio_drv_ptr);

/**
 * @name      AppBeep_BeepProcessCallback
 * @brief     蜂鸣器处理定时器回调函数
 * @param     user_data - 用户数据
 * @retval    无
 */
void AppBeep_BeepProcessCallback(void* user_data);

/**
 * @name      AppBeep_SetBeepProcessTimer
 * @brief     设置蜂鸣器处理定时器句柄（内部使用）
 * @param     timer_handle - 定时器句柄
 * @retval    无
 */
void AppBeep_SetBeepProcessTimer(SoftTimerHandle_t timer_handle);

/**
 * @name      AppBeep_StartBeep
 * @brief     开始蜂鸣器2秒周期循环
 * @param     无
 * @retval    无
 */
void AppBeep_StartBeep(void);

/**
 * @name      AppBeep_StartBeep2D
 * @brief     开始蜂鸣器二维时序模式
 * @param     无
 * @retval    无
 */
void AppBeep_StartBeep2D(void);



/**
 * @name      AppBeep_SwitchToNext2DMode
 * @brief     切换到下一个二维模式
 * @param     无
 * @retval    无
 */
void AppBeep_SwitchToNext2DMode(void);

/**
 * @name      AppBeep_StopBeep
 * @brief     停止蜂鸣器
 * @param     无
 * @retval    无
 */
void AppBeep_StopBeep(void);

#endif /* APP_BEEP_H */
