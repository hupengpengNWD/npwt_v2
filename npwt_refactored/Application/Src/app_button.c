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
#include "../Inc/app_beep.h"
#include "../../Core/Inc/system_enums.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Middleware/Inc/queue.h"
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

/* 按键事件队列 */
static st_queue g_key_event_queue;
static uint8_t g_key_event_queue_buffer[KEY_EVENT_QUEUE_LENGTH * sizeof(KeyEvent_t)];
static bool g_key_event_queue_initialized = false;

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      AppButton_InitKeyEventQueue
 * @brief     初始化按键事件队列（内部函数）
 * @param     无
 * @retval    无
 */
static void AppButton_InitKeyEventQueue(void)
{
    if (g_key_event_queue_initialized == false) {
        lib_queue_create(&g_key_event_queue);
        g_key_event_queue.configure(&g_key_event_queue);  // 先配置函数指针
        g_key_event_queue.initialize(&g_key_event_queue,
                                     KEY_EVENT_QUEUE_LENGTH,
                                     sizeof(KeyEvent_t),
                                     g_key_event_queue_buffer);  // 后初始化参数
        g_key_event_queue_initialized = true;
    }
}

/**
 * @name      AppButton_GetKeyEventQueue
 * @brief     获取按键事件队列指针（供app_ui模块使用）
 * @param     无
 * @retval    st_queue_ptr - 按键事件队列指针，失败返回NULL
 */
st_queue_ptr AppButton_GetKeyEventQueue(void)
{
    if (g_key_event_queue_initialized) {
        return &g_key_event_queue;
    }
    return NULL;
}

/**
 * @name      AppButton_PutKeyEvent
 * @brief     将按键事件放入队列（内部函数）
 * @param     key_id - 按键ID
 * @param     event - 按键事件
 * @retval    bool - true表示成功，false表示队列满
 */
static bool AppButton_PutKeyEvent(uint8_t key_id, KeyMachineEvent_e event)
{
    KeyEvent_t key_event = {
        .key_id = key_id,
        .key_event = event
    };
    
    uint8_t result = false;
    
    /* 确保队列已初始化 */
    if (g_key_event_queue_initialized == false) {
        AppButton_InitKeyEventQueue();
    }
    
    /* 队列满时丢弃最旧的事件（保证新事件能进入） */
    if (g_key_event_queue.full(&g_key_event_queue)) {
        KeyEvent_t dummy;
        g_key_event_queue.get(&g_key_event_queue, &dummy, sizeof(dummy));
    }
    
    /* 放入新事件 */
    result = g_key_event_queue.put(&g_key_event_queue, &key_event, sizeof(key_event));
    
    return result;
}

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
    
    // 初始化按键事件队列
    AppButton_InitKeyEventQueue();
    
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
 * 
 * 功能说明：
 * - 短按/单击：发送UI事件作为确认键（key_id=0）
 * - 长按1秒：保留给未来可能的开机功能（不发送UI事件）
 * - 长按3秒：进入关机准备状态（不发送UI事件，系统级操作）
 * - 释放：如果在关机准备状态，执行关机（不发送UI事件，系统级操作）
 */
void AppButton_PowerKeyCallback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg)
{
    (void)ptr;
    (void)arg;
    
    /* 对于普通操作事件（短按/单击），发送UI事件作为确认键 */
    /* 注意：长按事件（LONG_PRESS、ULTRA_LONG_PRESS）和RELEASE事件不发送UI事件，
     * 因为它们是电源控制相关的系统级操作
     */
    if (event == KEY_MACHINE_EVENT_CLICK || event == KEY_MACHINE_EVENT_SHORT_PRESS) {
        /* 发送UI事件（key_id=0表示OK/确认键） */
        AppButton_PutKeyEvent(0, event);
        /* 注意：这里不return，继续执行后面的switch-case，以防有额外处理 */
    }
    
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
            // 注意：此事件不发送UI事件，保留给未来可能的开机功能
            break;
        }
        
        case KEY_MACHINE_EVENT_ULTRA_LONG_PRESS:
        {
            // 长按3秒 - 进入关机准备状态（仅在运行状态下有效）
            // 注意：此事件不发送UI事件，因为这是系统级电源控制操作
            if (g_power_state == POWER_STATE_ON) {
                // 进入关机准备状态，关闭背光提示用户
                g_power_state = POWER_STATE_SHUTDOWN_PREPARE;
                HAL_LCD_Backlight_Off();
            }
            break;
        }
        
        case KEY_MACHINE_EVENT_RELEASE:
        {
            // 按键释放 - 如果在关机准备状态，则真正关机
            // 注意：此事件不发送UI事件，因为这是系统级电源控制操作
            if (g_power_state == POWER_STATE_SHUTDOWN_PREPARE) {
                PowerOff();
                // 状态更新已在PowerOff()函数中完成
            }
            break;
        }
        
        case KEY_MACHINE_EVENT_CLICK:
        case KEY_MACHINE_EVENT_SHORT_PRESS:
        {
            // 短按/单击事件已在函数开头发送UI事件
            // 这里可以添加额外的处理逻辑（如果需要）
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
    /* 将按键事件放入队列（key_id=1表示上键） */
    AppButton_PutKeyEvent(1, event);
    
    switch (event) {
        case KEY_MACHINE_EVENT_PRESS:
        {
            // 按键按下 - 具体功能待实现
            break;
        }
        
        case KEY_MACHINE_EVENT_SHORT_PRESS:
        {
            // 短按 - 向上选择菜单项、增加数值（已通过AppUI_OnKeyEvent发送给UI模块）
            break;
        }
        
        case KEY_MACHINE_EVENT_LONG_PRESS:
        {
            // 长按1秒 - 快速增加数值（已通过AppUI_OnKeyEvent发送给UI模块）
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
            break;
        }
        
        case KEY_MACHINE_EVENT_CLICK:
        {
            // 单击 - 切换到下一个蜂鸣器二维模式（保留原功能）
            AppBeep_SwitchToNext2DMode();
            // 同时发送UI事件（已在函数开头发送）
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
    
    /* 将按键事件放入队列（key_id=2表示下键） */
    AppButton_PutKeyEvent(2, event);
    
    switch (event) {
        case KEY_MACHINE_EVENT_PRESS:
        {
            // 按键按下 - 具体功能待实现
            break;
        }
        
        case KEY_MACHINE_EVENT_SHORT_PRESS:
        {
            // 短按 - 向下选择菜单项、减少数值（已通过AppUI_OnKeyEvent发送给UI模块）
            break;
        }
        
        case KEY_MACHINE_EVENT_LONG_PRESS:
        {
            // 长按1秒 - 快速减少数值（已通过AppUI_OnKeyEvent发送给UI模块）
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
            break;
        }
        
        case KEY_MACHINE_EVENT_CLICK:
        {
            // 单击（已通过AppUI_OnKeyEvent发送给UI模块）
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
    
    /* 将按键事件放入队列（key_id=3表示取消/静音键） */
    AppButton_PutKeyEvent(3, event);
    
    switch (event) {
        case KEY_MACHINE_EVENT_PRESS:
        {
            // 按键按下 - 具体功能待实现
            break;
        }
        
        case KEY_MACHINE_EVENT_SHORT_PRESS:
        {
            // 短按 - 确认/启动操作（已通过AppUI_OnKeyEvent发送给UI模块）
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
