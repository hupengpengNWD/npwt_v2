/****************************************************************************
 * 文件名: app_button.h
 * 功能: 按键应用层接口
 * 
 * 说明: 
 *   提供按键相关的应用层接口
 *   包含电源按键的配置、回调函数等
 * 
 * 创建日期: 2025-10-23
 ****************************************************************************/

#ifndef APP_BUTTON_H
#define APP_BUTTON_H

#include "../../Middleware/Inc/key_machine.h"
#include "../../Middleware/Inc/soft_timer.h"
#include "../../Middleware/Inc/queue.h"

/****************************************************************************
 * 按键事件队列配置
 ****************************************************************************/
#define KEY_EVENT_QUEUE_LENGTH  8    // 按键事件队列长度（足够缓冲快速按键）

/****************************************************************************
 * 按键事件结构体定义
 ****************************************************************************/
/**
 * @brief 按键事件结构体（用于app_button到app_ui的事件传递）
 */
typedef struct {
    uint8_t key_id;                     // 按键ID: 0=OK/START, 1=UP, 2=DN, 3=CANCEL, 4=UP+DN组合, 5=OK+CANCEL组合
    KeyMachineEvent_e key_event;        // 按键事件类型
} KeyEvent_t;

/* 组合按键ID（上+下同时按下，用于解锁） */
#define APP_BUTTON_KEY_ID_UNLOCK_COMBO   (4U)
/* 组合按键ID（OK+CANCEL同时按下，用于语言切换） */
#define APP_BUTTON_KEY_ID_LANGUAGE_COMBO (5U)

/****************************************************************************
 * 函数声明
 ****************************************************************************/

/**
 * @name      AppButton_Init
 * @brief     初始化按键应用层
 * @param     无
 * @retval    无
 */
void AppButton_Init(void);

/**
 * @name      AppButton_GetPowerKeyConfig
 * @brief     获取电源按键配置
 * @param     无
 * @retval    const KeyConfig_t* - 电源按键配置指针
 */
const KeyConfig_t* AppButton_GetPowerKeyConfig(void);

/**
 * @name      AppButton_GetPowerKeyInstance
 * @brief     获取电源按键实例
 * @param     无
 * @retval    KeyMachinePtr_t - 电源按键实例指针
 */
KeyMachinePtr_t AppButton_GetPowerKeyInstance(void);

/**
 * @name      AppButton_GetUpKeyInstance
 * @brief     获取上键状态机实例
 * @param     无
 * @retval    KeyMachinePtr_t - 上键实例指针
 */
KeyMachinePtr_t AppButton_GetUpKeyInstance(void);

/**
 * @name      AppButton_GetUpKeyConfig
 * @brief     获取上键配置
 * @param     无
 * @retval    const KeyConfig_t* - 上键配置指针
 */
const KeyConfig_t* AppButton_GetUpKeyConfig(void);

/**
 * @name      AppButton_GetDownKeyInstance
 * @brief     获取下键状态机实例
 * @param     无
 * @retval    KeyMachinePtr_t - 下键实例指针
 */
KeyMachinePtr_t AppButton_GetDownKeyInstance(void);

/**
 * @name      AppButton_GetDownKeyConfig
 * @brief     获取下键配置
 * @param     无
 * @retval    const KeyConfig_t* - 下键配置指针
 */
const KeyConfig_t* AppButton_GetDownKeyConfig(void);

/**
 * @name      AppButton_GetCancelKeyInstance
 * @brief     获取取消/静音键状态机实例
 * @param     无
 * @retval    KeyMachinePtr_t - 取消/静音键实例指针
 */
KeyMachinePtr_t AppButton_GetCancelKeyInstance(void);

/**
 * @name      AppButton_GetCancelKeyConfig
 * @brief     获取取消/静音键配置
 * @param     无
 * @retval    const KeyConfig_t* - 取消/静音键配置指针
 */
const KeyConfig_t* AppButton_GetCancelKeyConfig(void);

/**
 * @name      AppButton_GetKeyProcessTimer
 * @brief     获取按键处理定时器句柄
 * @param     无
 * @retval    SoftTimerHandle_t - 按键处理定时器句柄
 */
SoftTimerHandle_t AppButton_GetKeyProcessTimer(void);

/**
 * @name      AppButton_PowerKeyCallback
 * @brief     电源按键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_PowerKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg);

/**
 * @name      AppButton_UpKeyCallback
 * @brief     上键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_UpKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg);

/**
 * @name      AppButton_DownKeyCallback
 * @brief     下键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_DownKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg);

/**
 * @name      AppButton_CancelKeyCallback
 * @brief     取消/静音键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_CancelKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg);

/**
 * @name      AppButton_KeyProcessCallback
 * @brief     按键处理定时器回调函数
 * @param     user_data - 用户数据
 * @retval    无
 */
void AppButton_KeyProcessCallback(void* user_data);

/**
 * @name      AppButton_PowerKeyReadLevel
 * @brief     电源按键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_PowerKeyReadLevel(void* gpio_drv_ptr);

/**
 * @name      AppButton_UpKeyReadLevel
 * @brief     上键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_UpKeyReadLevel(void* gpio_drv_ptr);

/**
 * @name      AppButton_DownKeyReadLevel
 * @brief     下键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_DownKeyReadLevel(void* gpio_drv_ptr);

/**
 * @name      AppButton_CancelKeyReadLevel
 * @brief     取消/静音键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_CancelKeyReadLevel(void* gpio_drv_ptr);

/**
 * @name      AppButton_SetPowerState
 * @brief     设置电源状态（供PowerOn/PowerOff函数调用）
 * @param     state - 电源状态
 * @retval    无
 */
void AppButton_SetPowerState(uint8_t state);

/**
 * @name      AppButton_SetKeyProcessTimer
 * @brief     设置按键处理定时器句柄（内部使用）
 * @param     timer_handle - 定时器句柄
 * @retval    无
 */
void AppButton_SetKeyProcessTimer(SoftTimerHandle_t timer_handle);

/**
 * @name      AppButton_GetKeyEventQueue
 * @brief     获取按键事件队列指针（供app_ui模块使用）
 * @param     无
 * @retval    st_queue_ptr - 按键事件队列指针，失败返回NULL
 */
st_queue_ptr AppButton_GetKeyEventQueue(void);

#endif /* APP_BUTTON_H */
