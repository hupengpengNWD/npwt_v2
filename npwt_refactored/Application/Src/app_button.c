/****************************************************************************
 * 文件名: app_button.c
 * 功能: 按键应用层实现
 * 
 * 说明: 
 *   实现按键相关的应用层功能
 *   包含电源按键的配置、回调函数等
 * 
 * 创建日期: 2025-10-23
 ****************************************************************************/

#include "../Inc/app_button.h"
#include "../../Core/Inc/system_enums.h"
#include "../../Drivers/Inc/lcd_driver.h"
#include <stddef.h>

/****************************************************************************
 * 外部函数声明
 ****************************************************************************/
extern void PowerOn(void);
extern void PowerOff(void);

/****************************************************************************
 * 电源控制状态
 ****************************************************************************/
typedef enum {
    POWER_STATE_OFF = 0,      // 关机状态
    POWER_STATE_ON,           // 开机状态
    POWER_STATE_SHUTDOWN_PREPARE  // 关机准备状态（等待按键松开）
} PowerState_e;

/****************************************************************************
 * 全局变量
 ****************************************************************************/

/* 电源控制状态 */
static PowerState_e g_power_state = POWER_STATE_OFF;

/* 按键状态机实例 */
static KeyMachine_t g_power_key_instance;
static KeyMachinePtr_t g_power_key = NULL;

static KeyMachine_t g_up_key_instance;
static KeyMachinePtr_t g_up_key = NULL;

static KeyMachine_t g_down_key_instance;
static KeyMachinePtr_t g_down_key = NULL;

static KeyMachine_t g_cancel_key_instance;
static KeyMachinePtr_t g_cancel_key = NULL;

/* 电源按键配置 */
static KeyConfig_t g_power_key_config = {
    .debounce_ms = 20,
    .short_press_ms = 50,
    .long_press_ms = 1000,
    .ultra_long_press_ms = 3000,
    .trigger = KEY_TRIGGER_LOW,
    .event_mask = KEY_EVENT_MASK_ALL
};

/* 上键配置 */
static KeyConfig_t g_up_key_config = {
    .debounce_ms = 20,
    .short_press_ms = 50,
    .long_press_ms = 1000,
    .ultra_long_press_ms = 3000,
    .trigger = KEY_TRIGGER_LOW,
    .event_mask = KEY_EVENT_MASK_ALL
};

/* 下键配置 */
static KeyConfig_t g_down_key_config = {
    .debounce_ms = 20,
    .short_press_ms = 50,
    .long_press_ms = 1000,
    .ultra_long_press_ms = 3000,
    .trigger = KEY_TRIGGER_LOW,
    .event_mask = KEY_EVENT_MASK_ALL
};

/* 取消/静音键配置 */
static KeyConfig_t g_cancel_key_config = {
    .debounce_ms = 20,
    .short_press_ms = 50,
    .long_press_ms = 1000,
    .ultra_long_press_ms = 3000,
    .trigger = KEY_TRIGGER_LOW,
    .event_mask = KEY_EVENT_MASK_ALL
};

/* 按键处理定时器句柄 */
static SoftTimerHandle_t g_key_process_timer = 0;

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      AppButton_Init
 * @brief     初始化按键应用层
 * @param     无
 * @retval    无
 */
void AppButton_Init(void)
{
    // 初始化按键实例
    g_power_key = &g_power_key_instance;
    g_up_key = &g_up_key_instance;
    g_down_key = &g_down_key_instance;
    g_cancel_key = &g_cancel_key_instance;
    // 注意：电源状态初始化移到定时器启动后，中断使能前
}

/**
 * @name      AppButton_GetPowerKeyConfig
 * @brief     获取电源按键配置
 * @param     无
 * @retval    const KeyConfig_t* - 电源按键配置指针
 */
const KeyConfig_t* AppButton_GetPowerKeyConfig(void)
{
    return &g_power_key_config;
}

/**
 * @name      AppButton_GetPowerKeyInstance
 * @brief     获取电源按键实例
 * @param     无
 * @retval    KeyMachinePtr_t - 电源按键实例指针
 */
KeyMachinePtr_t AppButton_GetPowerKeyInstance(void)
{
    return g_power_key;
}

/**
 * @name      AppButton_GetUpKeyInstance
 * @brief     获取上键状态机实例
 * @param     无
 * @retval    KeyMachinePtr_t - 上键实例指针
 */
KeyMachinePtr_t AppButton_GetUpKeyInstance(void)
{
    return g_up_key;
}

/**
 * @name      AppButton_GetUpKeyConfig
 * @brief     获取上键配置
 * @param     无
 * @retval    const KeyConfig_t* - 上键配置指针
 */
const KeyConfig_t* AppButton_GetUpKeyConfig(void)
{
    return &g_up_key_config;
}

/**
 * @name      AppButton_GetDownKeyInstance
 * @brief     获取下键状态机实例
 * @param     无
 * @retval    KeyMachinePtr_t - 下键实例指针
 */
KeyMachinePtr_t AppButton_GetDownKeyInstance(void)
{
    return g_down_key;
}

/**
 * @name      AppButton_GetDownKeyConfig
 * @brief     获取下键配置
 * @param     无
 * @retval    const KeyConfig_t* - 下键配置指针
 */
const KeyConfig_t* AppButton_GetDownKeyConfig(void)
{
    return &g_down_key_config;
}

/**
 * @name      AppButton_GetCancelKeyInstance
 * @brief     获取取消/静音键状态机实例
 * @param     无
 * @retval    KeyMachinePtr_t - 取消/静音键实例指针
 */
KeyMachinePtr_t AppButton_GetCancelKeyInstance(void)
{
    return g_cancel_key;
}

/**
 * @name      AppButton_GetCancelKeyConfig
 * @brief     获取取消/静音键配置
 * @param     无
 * @retval    const KeyConfig_t* - 取消/静音键配置指针
 */
const KeyConfig_t* AppButton_GetCancelKeyConfig(void)
{
    return &g_cancel_key_config;
}

/**
 * @name      AppButton_GetKeyProcessTimer
 * @brief     获取按键处理定时器句柄
 * @param     无
 * @retval    SoftTimerHandle_t - 按键处理定时器句柄
 */
SoftTimerHandle_t AppButton_GetKeyProcessTimer(void)
{
    return g_key_process_timer;
}

/**
 * @name      AppButton_PowerKeyCallback
 * @brief     电源按键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_PowerKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg)
{
    (void)ptr;
    (void)arg;
    
    switch (event) {
        case KEY_MACHINE_EVENT_PRESS:
        {
            // 按键按下 - 移除调试代码避免干扰
            break;
        }
        
        case KEY_MACHINE_EVENT_LONG_PRESS:
        {
            // 长按1秒 - 已删除开机逻辑，现在只在断电情况下长按确认键通电
            // 通电后立即在main函数中完成电源自锁和开机
            break;
        }
        
        case KEY_MACHINE_EVENT_ULTRA_LONG_PRESS:
        {
            // 长按3秒 - 进入关机准备状态（仅在运行状态下有效）
            if (g_power_state == POWER_STATE_ON) {
                // 进入关机准备状态，关闭背光提示用户
                g_power_state = POWER_STATE_SHUTDOWN_PREPARE;
                LCD_SetBacklight(false);
            }
            break;
        }
        
        case KEY_MACHINE_EVENT_RELEASE:
        {
            // 按键释放 - 如果在关机准备状态，则真正关机
            if (g_power_state == POWER_STATE_SHUTDOWN_PREPARE) {
                PowerOff();
                // 状态更新已在PowerOff()函数中完成
            }
            break;
        }
        
        default:
            break;
    }
}

/**
 * @name      AppButton_UpKeyCallback
 * @brief     上键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_UpKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg)
{
    (void)ptr;
    (void)arg;
    
    switch (event) {
        case KEY_MACHINE_EVENT_PRESS:
        {
            // 按键按下 - 具体功能待实现
            break;
        }
        
        case KEY_MACHINE_EVENT_SHORT_PRESS:
        {
            // 短按 - 向上选择菜单项、增加数值
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_LONG_PRESS:
        {
            // 长按1秒 - 快速增加数值
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_ULTRA_LONG_PRESS:
        {
            // 长按3秒 - 特殊功能
            // 测试：翻转RC4（黄色LED）
            HAL_LED_Yellow_Toggle();
            break;
        }
        
        case KEY_MACHINE_EVENT_RELEASE:
        {
            // 按键释放
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_CLICK:
        {
            // 单击 - 确认性点击操作（按下时间 ≥ 50ms）
            // TODO: 实现具体功能
            // 测试：翻转RC4（黄色LED）
            HAL_LED_Yellow_Toggle();
            break;
        }
        
        default:
            break;
    }
}

/**
 * @name      AppButton_DownKeyCallback
 * @brief     下键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_DownKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg)
{
    (void)ptr;
    (void)arg;
    
    switch (event) {
        case KEY_MACHINE_EVENT_PRESS:
        {
            // 按键按下 - 具体功能待实现
            break;
        }
        
        case KEY_MACHINE_EVENT_SHORT_PRESS:
        {
            // 短按 - 向下选择菜单项、减少数值
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_LONG_PRESS:
        {
            // 长按1秒 - 快速减少数值
            // TODO: 实现具体功能
            // 测试：翻转RC4（黄色LED）
            HAL_LED_Yellow_Toggle();
            break;
        }
        
        case KEY_MACHINE_EVENT_ULTRA_LONG_PRESS:
        {
            // 长按3秒 - 特殊功能
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_RELEASE:
        {
            // 按键释放
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_CLICK:
        {
            // 单击 - 确认性点击操作
            // TODO: 实现具体功能
            break;
        }
        
        default:
            break;
    }
}

/**
 * @name      AppButton_CancelKeyCallback
 * @brief     取消/静音键事件回调函数
 * @param     ptr - 按键状态机指针
 * @param     event - 按键事件
 * @param     arg - 回调参数
 * @retval    无
 */
void AppButton_CancelKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg)
{
    (void)ptr;
    (void)arg;
    
    switch (event) {
        case KEY_MACHINE_EVENT_PRESS:
        {
            // 按键按下 - 具体功能待实现
            break;
        }
        
        case KEY_MACHINE_EVENT_SHORT_PRESS:
        {
            // 短按 - 取消操作、静音
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_LONG_PRESS:
        {
            // 长按1秒 - 特殊功能
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_ULTRA_LONG_PRESS:
        {
            // 长按3秒 - 语言切换等特殊功能
            // TODO: 实现具体功能
            // 测试：翻转RC4（黄色LED）
            HAL_LED_Yellow_Toggle();
            break;
        }
        
        case KEY_MACHINE_EVENT_RELEASE:
        {
            // 按键释放
            // TODO: 实现具体功能
            break;
        }
        
        case KEY_MACHINE_EVENT_CLICK:
        {
            // 单击 - 确认性点击操作
            // TODO: 实现具体功能
            break;
        }
        
        default:
            break;
    }
}

/**
 * @name      AppButton_KeyProcessCallback
 * @brief     按键处理定时器回调函数
 * @param     user_data - 用户数据
 * @retval    无
 */
void AppButton_KeyProcessCallback(void* user_data)
{
    (void)user_data;
    
    /* 处理按键状态机 */
    KeyManager_Process();
}

/**
 * @name      AppButton_PowerKeyReadLevel
 * @brief     电源按键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_PowerKeyReadLevel(void* gpio_drv_ptr)
{
    (void)gpio_drv_ptr;
    
    // 读取确认键状态（PORTB & 0x3C == 0x38表示确认键按下）
    uint8_t key_port = PORTB & 0x3C;
    return (key_port == 0x38) ? 0 : 1; // 低电平触发
}

/**
 * @name      AppButton_UpKeyReadLevel
 * @brief     上键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_UpKeyReadLevel(void* gpio_drv_ptr)
{
    (void)gpio_drv_ptr;
    
    // 读取上键状态（PORTB & 0x3C == 0x34表示上键按下）
    uint8_t key_port = PORTB & 0x3C;
    return (key_port == 0x34) ? 0 : 1; // 低电平触发
}

/**
 * @name      AppButton_DownKeyReadLevel
 * @brief     下键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_DownKeyReadLevel(void* gpio_drv_ptr)
{
    (void)gpio_drv_ptr;
    
    // 读取下键状态（PORTB & 0x3C == 0x2C表示下键按下）
    uint8_t key_port = PORTB & 0x3C;
    return (key_port == 0x2C) ? 0 : 1; // 低电平触发
}

/**
 * @name      AppButton_CancelKeyReadLevel
 * @brief     取消/静音键电平读取函数
 * @param     gpio_drv_ptr - GPIO驱动指针
 * @retval    uint8_t - 按键电平状态
 */
uint8_t AppButton_CancelKeyReadLevel(void* gpio_drv_ptr)
{
    (void)gpio_drv_ptr;
    
    // 读取取消/静音键状态（PORTB & 0x3C == 0x1C表示取消/静音键按下）
    uint8_t key_port = PORTB & 0x3C;
    return (key_port == 0x1C) ? 0 : 1; // 低电平触发
}

/**
 * @name      AppButton_SetKeyProcessTimer
 * @brief     设置按键处理定时器句柄（内部使用）
 * @param     timer_handle - 定时器句柄
 * @retval    无
 */
void AppButton_SetKeyProcessTimer(SoftTimerHandle_t timer_handle)
{
    g_key_process_timer = timer_handle;
}

/**
 * @name      AppButton_SetPowerState
 * @brief     设置电源状态（供PowerOn/PowerOff函数调用）
 * @param     state - 电源状态
 * @retval    无
 */
void AppButton_SetPowerState(uint8_t state)
{
    g_power_state = (PowerState_e)state;
}
