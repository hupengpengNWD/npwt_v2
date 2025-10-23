/****************************************************************************
 * 文件名: main.c
 * 功能: 主程序入口（基于按键状态机的电源控制版本）
 * 
 * 说明: 
 *   使用key_machine组件实现电源控制
 *   长按1秒开机，长按3秒关机
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#include "../Inc/system_config.h"
#include "../Inc/system_types.h"
#include "../Inc/system_enums.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../HAL/Inc/hal_timer.h"
#include "../../Middleware/Inc/soft_timer.h"
#include "../../Middleware/Inc/key_machine.h"
#include "../../Drivers/Inc/lcd_driver.h"
#include <stddef.h> // For NULL

/****************************************************************************
 * 全局变量
 ****************************************************************************/
extern volatile unsigned char FLG_SYS_10MS;

/* 系统状态 */
static SystemState_t g_system;

/* 电源控制状态 */
typedef enum {
    POWER_STATE_OFF = 0,      // 关机状态
    POWER_STATE_ON,           // 开机状态
    POWER_STATE_SHUTDOWN_PREPARE  // 关机准备状态（等待按键松开）
} PowerState_e;

static PowerState_e g_power_state = POWER_STATE_OFF;

/* 按键状态机实例 */
static KeyMachine_t g_power_key_instance;
static KeyMachinePtr_t g_power_key = NULL;

/* 电源按键配置 */
static KeyConfig_t g_power_key_config = {
    .debounce_ms = 20,
    .short_press_ms = 50,
    .long_press_ms = 1000,
    .ultra_long_press_ms = 3000,
    .trigger = KEY_TRIGGER_LOW,
    .event_mask = KEY_EVENT_MASK_ALL
};

/****************************************************************************
 * 按键读取函数
 ****************************************************************************/
static uint8_t PowerKey_ReadLevel(void* gpio_drv_ptr) {
    // 读取确认键状态（PORTB & 0x3C == 0x38表示确认键按下）
    uint8_t key_port = PORTB & 0x3C;
    return (key_port == 0x38) ? 0 : 1; // 低电平触发
}

/****************************************************************************
 * 电源控制函数
 ****************************************************************************/

/**
 * 函数: PowerOn
 * 功能: 开机操作
 * 说明: 拉高RC2保持MOSFET导通
 */
void PowerOn(void)
{
    // 拉高RC2保持MOSFET导通（使用HAL函数）
    HAL_Power_Hold();  // LATCbits.LATC2 = 1
    
    // 开启背光
    LCD_SetBacklight(true);
    
    // 更新状态
    g_power_state = POWER_STATE_ON;
}

/**
 * 函数: PowerOff
 * 功能: 关机操作
 * 说明: 释放RC2让MOSFET断开
 */
void PowerOff(void)
{
//     释放RC2让MOSFET断开（使用HAL函数）
    HAL_Power_Release();  // LATCbits.LATC2 = 0
    
    // 关闭背光
    LCD_SetBacklight(false);
    
    // 更新状态
    g_power_state = POWER_STATE_OFF;
    
    // 进入死循环等待断电
    while(1) { 
        // 看门狗已禁用，无需喂狗
    }
}

/****************************************************************************
 * 按键事件处理函数
 ****************************************************************************/

/**
 * 函数: PowerKey_Callback
 * 功能: 电源按键事件回调（新接口适配器）
 */
void PowerKey_Callback(KeyMachinePtr_t ptr, KeyMachineEvent_e event, void* arg)
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
            // 长按1秒 - 开机（仅在关机状态下有效）
            if (g_power_state == POWER_STATE_OFF) {
                PowerOn();
            }
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
            }
            break;
        }
        
        default:
            break;
    }
}

/****************************************************************************
 * 按键处理定时器
 ****************************************************************************/

/* 按键处理定时器句柄 */
static SoftTimerHandle_t g_key_process_timer = 0;

/* 按键处理回调函数：每10ms处理一次按键状态机 */
void KeyProcessCallback(void* user_data)
{
    /* 处理按键状态机 */
    KeyManager_Process();
}

/****************************************************************************
 * 主函数
 ****************************************************************************/
void main(void)
{
    /* ========== 系统初始化 ========== */
    
    /* 1. 初始化振荡器（与未重构工程SYS_OSC_Ini完全一致） */
    OSCCON = 0b01110000;    // 内部振荡器，8MHz
    OSCTUNE = OSCTUNE | 0x40; // 使能4×PLL → 32MHz
    while (!(OSCCON & 0x08)); // 等待振荡器稳定
    asm("clrwdt");
    
    /* 2. 初始化GPIO */
    HAL_GPIO_Init();
    
    /* 2.1 立即设置RC2为高电平，确保电源自锁 */
    HAL_Power_Hold();  // LATCbits.LATC2 = 1
    
    /* 3. 初始化定时器 */
    HAL_Timer_Init();     // Timer0: 10ms
    HAL_Timer1_Init();    // Timer1: 10ms
    HAL_PWM_Init();       // Timer3: 1ms
    
    /* 4. 初始化软件定时器模块 */
    SoftTimer_Init();
    
    /* 5. 初始化按键管理器 */
    KeyManager_Init();
    
    /* 6. 创建电源按键 */
    g_power_key = &g_power_key_instance;
    KeyMachine_Initialize(g_power_key, &g_power_key_config, PowerKey_Callback, NULL);
    KeyMachine_SetDriverInterface(g_power_key, (void*)0x1234, PowerKey_ReadLevel);
    
    // 注册到按键管理器
    KeyManager_RegisterStaticKey(g_power_key);
    
    /* 7. 创建按键处理定时器（每10ms执行一次） */
    g_key_process_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 10, KeyProcessCallback, NULL);
    if (g_key_process_timer != 0) {
        SoftTimer_Start(g_key_process_timer);
    }
    
    /* 7. 初始化电源状态 */
    g_power_state = POWER_STATE_OFF;
    
    /* 8. 使能全局中断 */
    T3CONbits.TMR3ON = 1; // 启动Timer3
    GIE = 1;
    PEIE = 1;
    
    /* ========== 主循环 ========== */
    while (1)
    {
        /* 10ms任务 */
        if (FLG_SYS_10MS)
        {
            FLG_SYS_10MS = 0;
            
            /* 软件定时器处理（按键处理已由定时器自动处理） */
            // 这里可以添加其他10ms任务
        }
    }
}

/****************************************************************************
 * 中断服务程序
 ****************************************************************************/
void __interrupt() ISR(void)
{
    /* Timer0中断：10ms系统滴答 */
    if (T0IF)
    {
        T0IF = 0;
        TMR0 = 0xD8F0;  // 重装定时器值（与初始化一致）
        FLG_SYS_10MS = 1;
    }
    
    /* Timer1中断：10ms - 软件定时器tick更新 */
    if (TMR1IF)
    {
        TMR1IF = 0;
        TMR1H = 0xD8;  // 重装定时器值（与初始化一致）
        TMR1L = 0xF0;
        
        /* 更新软件定时器 */
        SoftTimer_TickUpdate();
    }
    
    /* Timer3中断：1ms */
    if (TMR3IF)
    {
        TMR3IF = 0;
        TMR3H = 0xFC;
        TMR3L = 0x18;
    }
}