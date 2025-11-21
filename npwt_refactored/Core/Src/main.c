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
#include "../../HAL/Inc/hal_lcd.h"
#include "../../Middleware/Inc/soft_timer.h"
#include "../../Middleware/Inc/key_machine.h"
#include "../../Application/Inc/app_button.h"
#include "../../Application/Inc/app_beep.h"
#include "../../Application/Inc/app_ui.h"
#include "../../Application/Inc/app_battery.h"
#include "../../Application/Inc/app_pressure.h"
#include "../../Application/Inc/app_alarm.h"
#include "../../HAL/Inc/hal_adc.h"
#include "../../Middleware/Inc/display.h"
#include "../../Middleware/Inc/pwm.h"
#include <stddef.h> // For NULL

/****************************************************************************
 * 全局变量
 ****************************************************************************/
extern volatile unsigned char FLG_SYS_10MS;

/* 系统状态 */
static SystemState_t g_system;
static volatile bool g_pressure_process_flag = false;

/****************************************************************************
 * 外部函数声明
 ****************************************************************************/
extern void PowerOn(void);
extern void PowerOff(void);

/****************************************************************************
 * 本地函数声明
 ****************************************************************************/
/**
 * @name      LED_ToggleCallback
 * @brief     黄色LED翻转定时器回调函数
 * @param     user_data - 用户数据
 * @retval    无
 */
void LED_ToggleCallback(void* user_data);
void PressureADC_Callback(void* user_data);
static void PressureProcess_TimerCallback(void* user_data);


/****************************************************************************
 * 电源控制函数
 ****************************************************************************/

/**
 * @name      PowerOn
 * @brief     开机操作 - 拉高RC2保持MOSFET导通
 * @param     无
 * @retval    无
 */
void PowerOn(void)
{
    // 拉高RC2保持MOSFET导通（使用HAL函数）
    HAL_Power_Hold();  // LATCbits.LATC2 = 1
    
    // 开启背光
    HAL_LCD_Backlight_On();
    
    // 更新状态
    // 注意：电源状态现在由app_button模块管理
    // 状态更新在AppButton_PowerKeyCallback中处理
    // 但为了保持与备份文件的一致性，这里也更新状态
    AppButton_SetPowerState(1); // POWER_STATE_ON = 1
}

/**
 * @name      PowerOff
 * @brief     关机操作 - 释放RC2让MOSFET断开
 * @param     无
 * @retval    无
 */
void PowerOff(void)
{
//     释放RC2让MOSFET断开（使用HAL函数）
    HAL_Power_Release();  // LATCbits.LATC2 = 0
    
    // 关闭背光
    HAL_LCD_Backlight_Off();
    
    // 更新状态
    // 注意：电源状态现在由app_button模块管理
    // 状态更新在AppButton_PowerKeyCallback中处理
    // 但为了保持与备份文件的一致性，这里也更新状态
    AppButton_SetPowerState(0); // POWER_STATE_OFF = 0
    
    // 进入死循环等待断电
    while(1) { 
        // 看门狗已禁用，无需喂狗
    }
}

/****************************************************************************
 * @name      LCD_ProcessCallback
 * @brief     LCD处理定时器回调函数（5ms）
 * @param     无
 * @retval    无
 ****************************************************************************/
void LCD_ProcessCallback(void)
{
    // LCD状态机处理（非阻塞轮询）
    HAL_LCD_Process();
    
    // Display模块队列处理（非阻塞轮询）
    Display_Process();
}

/****************************************************************************
 * @name      LED_ToggleCallback
 * @brief     黄色LED翻转定时器回调函数
 * @param     user_data - 用户数据
 * @retval    无
 ****************************************************************************/
void LED_ToggleCallback(void* user_data)
{
    (void)user_data;
    
    // 翻转黄色LED（RC4）
    HAL_LED_Yellow_Toggle();
}

/****************************************************************************
 * @name      BatteryADC_Callback
 * @brief     电池ADC采集定时器回调函数（500ms）
 * @param     user_data - 用户数据
 * @retval    无
 ****************************************************************************/
void BatteryADC_Callback(void* user_data)
{
    (void)user_data;
    
    // 读取电池ADC（通道2，10次采样滤波）
    uint16_t adc_value = HAL_ADC_ReadFiltered(ADC_CHANNEL_BATTERY, 10);
    
    // 更新电池管理模块
    AppBattery_UpdateADC(adc_value);
}

void PressureADC_Callback(void* user_data)
{
    (void)user_data;

    // 读取压力ADC（通道0，4次采样滤波）
    uint16_t adc_value = HAL_ADC_ReadFiltered(ADC_CHANNEL_PRESSURE, 4);

    // 更新压力管理模块
    AppPressure_UpdateADC(adc_value);
}

static void PressureProcess_TimerCallback(void* user_data)
{
    (void)user_data;
    g_pressure_process_flag = true;
}

/****************************************************************************
 * @name      main
 * @brief     主函数 - 系统初始化和主循环
 * @param     无
 * @retval    无
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
    
    /* 2.1 初始化ADC模块 */
    HAL_ADC_Init();
    
    /* 2.2 立即设置RC2为高电平，确保电源自锁 */
    HAL_Power_Hold();  // 保持总线电源
    HAL_MotorPWR_Enable();  // 打开电机5V电源使能
    
    /* 2.3 立即打开LCD背光并设置电源状态为开机 */
    HAL_LCD_Backlight_On();
    AppButton_SetPowerState(1); // POWER_STATE_ON = 1
    
    /* 3. 初始化定时器 */
    HAL_Timer0_Init();     // Timer0: 10ms
    HAL_Timer1_Init();    // Timer1: 10ms
    HAL_Timer3_Init();       // Timer3: 1ms
    
    /* 3.1 注册Timer3的5ms回调 - LCD处理 */
    HAL_Timer3_RegisterCallback_5ms(LCD_ProcessCallback);
    
    /* 3.2 初始化PWM模块 */
    PWM_Init();
    
    /* 3.3 测试PWM：设置50%占空比并启动（用于测试） */
//    PWM_SetDuty(500);    // 50%占空比
//    PWM_Start();         // 启动PWM
    
    /* 4. 初始化软件定时器模块 */
    SoftTimer_Init();
    
    /* 5. 初始化按键管理器 */
    KeyManager_Init();
    
    /* 6. 初始化按键应用层 */
    AppButton_Init();
    
    /* 6.1 初始化蜂鸣器应用层 */
    AppBeep_Init();
    
    /* 6.2 初始化Display模块 */
    Display_Init();
    
    /* 6.3 初始化电池管理模块 */
    AppBattery_Init();

    /* 6.4 初始化压力管理模块 */
    AppPressure_Init();
    
    /* 6.5 初始化报警管理模块 */
    AppAlarm_Init();
    
    /* 6.6 初始化UI模块（FSM状态机） */
    AppUI_Init();
    
    /* 6.4 显示开机界面（由AppUI_Init内部处理） */
//     Display_ShowStartupInterface(); // 已由AppUI模块处理
    
    /* 7. 创建电源按键 */
    KeyMachinePtr_t power_key = AppButton_GetPowerKeyInstance();
    const KeyConfig_t* power_key_config = AppButton_GetPowerKeyConfig();
    KeyMachine_Initialize(power_key, power_key_config, AppButton_PowerKeyCallback, NULL);
    KeyMachine_SetDriverInterface(power_key, (void*)0x1234, AppButton_PowerKeyReadLevel);
    
    // 注册到按键管理器
    KeyManager_RegisterStaticKey(power_key);
    
    /* 7.1 创建上键 */
    KeyMachinePtr_t up_key = AppButton_GetUpKeyInstance();
    const KeyConfig_t* up_key_config = AppButton_GetUpKeyConfig();
    KeyMachine_Initialize(up_key, up_key_config, AppButton_UpKeyCallback, NULL);
    KeyMachine_SetDriverInterface(up_key, (void*)0x1234, AppButton_UpKeyReadLevel);
    
    // 注册到按键管理器
    KeyManager_RegisterStaticKey(up_key);
    
    /* 7.2 创建下键 */
    KeyMachinePtr_t down_key = AppButton_GetDownKeyInstance();
    const KeyConfig_t* down_key_config = AppButton_GetDownKeyConfig();
    KeyMachine_Initialize(down_key, down_key_config, AppButton_DownKeyCallback, NULL);
    KeyMachine_SetDriverInterface(down_key, (void*)0x1234, AppButton_DownKeyReadLevel);
    
    // 注册到按键管理器
    KeyManager_RegisterStaticKey(down_key);
    
    /* 7.3 创建取消/静音键 */
    KeyMachinePtr_t cancel_key = AppButton_GetCancelKeyInstance();
    const KeyConfig_t* cancel_key_config = AppButton_GetCancelKeyConfig();
    KeyMachine_Initialize(cancel_key, cancel_key_config, AppButton_CancelKeyCallback, NULL);
    KeyMachine_SetDriverInterface(cancel_key, (void*)0x1234, AppButton_CancelKeyReadLevel);
    
    // 注册到按键管理器
    KeyManager_RegisterStaticKey(cancel_key);
    
    /* 8. 创建按键处理定时器（每10ms执行一次） */
    SoftTimerHandle_t key_process_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 10, AppButton_KeyProcessCallback, NULL);
    if (key_process_timer != 0) {
        SoftTimer_Start(key_process_timer);
        AppButton_SetKeyProcessTimer(key_process_timer);
    }
    
    /* 8.1 创建蜂鸣器处理定时器（每20ms执行一次，避免与按键处理冲突） */
    SoftTimerHandle_t beep_process_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 20, AppBeep_BeepProcessCallback, NULL);
//    SoftTimerHandle_t beep_process_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 20, NULL, NULL);
    if (beep_process_timer != 0) {
        SoftTimer_Start(beep_process_timer);
        AppBeep_SetBeepProcessTimer(beep_process_timer);
    }
    
    /* 8.2 创建蜂鸣器控制实例 */
    PushPullPtr_t buzzer = AppBeep_GetBuzzerInstance();
    PushPull_Initialize(buzzer, AppBeep_BuzzerCallback, NULL);                 
    PushPull_SetDriverInterface(buzzer, (void*)0x1234, AppBeep_BuzzerWriteHigh, AppBeep_BuzzerWriteLow, AppBeep_BuzzerToggle);
    
    /* 8.3 开始蜂鸣器二维时序模式（支持多种模式切换） */
//    AppBeep_StartBeep2D();  // 测试代码，已屏蔽
    
    /* 8.4 创建黄色LED翻转定时器（每1秒执行一次） */
//    SoftTimerHandle_t led_toggle_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 1000, LED_ToggleCallback, NULL);
//    if (led_toggle_timer != 0) {
//        SoftTimer_Start(led_toggle_timer);
//    }
    
    /* 8.5 创建电池ADC采集定时器（每500ms执行一次） */
    SoftTimerHandle_t battery_adc_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 500, BatteryADC_Callback, NULL);
    if (battery_adc_timer != 0) {
        SoftTimer_Start(battery_adc_timer);
    }
    
    /* 8.6 创建电池处理定时器（每100ms执行一次） */
    SoftTimerHandle_t battery_process_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 100, AppBattery_Process, NULL);
    if (battery_process_timer != 0) {
        SoftTimer_Start(battery_process_timer);
    }

    /* 8.7 创建压力ADC采集定时器（每10ms执行一次） */
    SoftTimerHandle_t pressure_adc_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 10, PressureADC_Callback, NULL);
    if (pressure_adc_timer != 0) {
        SoftTimer_Start(pressure_adc_timer);
    }

    /* 8.8 创建压力处理定时器（每10ms执行一次） */
    SoftTimerHandle_t pressure_process_timer = SoftTimer_Create(SOFT_TIMER_MODE_PERIODIC, 10, PressureProcess_TimerCallback, NULL);

    if (pressure_process_timer != 0) {
        SoftTimer_Start(pressure_process_timer);
    }
  
    /* 9. 使能全局中断 */
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
               
               /* 报警处理（每10ms轮询一次，在UI处理之前） */
               AppAlarm_Process();
               
               /* UI状态机处理（每10ms轮询一次） */
               AppUI_Process();

               if (g_pressure_process_flag) {
                   g_pressure_process_flag = false;
                   AppPressure_Process(NULL);
               }
           }
    }
}

/****************************************************************************
 * @name      ISR
 * @brief     中断服务程序 - 处理定时器中断
 * @param     无
 * @retval    无
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
    
    /* Timer3中断：1ms - 调用HAL层处理 */
    HAL_Timer3_ISR();
}