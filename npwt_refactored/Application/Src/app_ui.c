/**
 * @file    app_ui.c
 * @brief   应用层UI模块实现
 * @date    2025-01-27
 * 
 * 应用层UI模块，使用FSM组件实现按键触发UI界面切换功能
 */

#include "../Inc/app_ui.h"
#include "../../Middleware/Inc/pwm.h"  // PWM控制接口（用于停止泵电机）
#include <stdbool.h>
/****************************************************************************
 * 常量定义（参考未重构工程）
 ****************************************************************************/
#define PRESSURE_HIGH_MAX     300    // 最大压力值（mmHg）
#define PRESSURE_HIGH_MIN     20     // 最小压力值（mmHg）
#define PRESSURE_LOW_MAX      100    // 最大低压值（mmHg）
#define PRESSURE_LOW_MIN      10     // 最小低压值（mmHg）
#define PRESSURE_STEP         10     // 压力调整步进值（mmHg）

#define TIME_HIGH_MAX         99     // 最大高压时间（分钟）
#define TIME_HIGH_MIN         1      // 最小高压时间（分钟）
#define TIME_LOW_MAX          99     // 最大低压时间（分钟）
#define TIME_LOW_MIN          1      // 最小低压时间（分钟）
#define TIME_STEP             1      // 时间调整步进值（分钟）

#define PRESSURE_SET_DEFAULT      120    // 默认压力值（mmHg，设置界面使用）
#define PRESSURE_LOW_DEFAULT      80     // 默认低压值（mmHg）
#define TIME_HIGH_DEFAULT         1      // 默认高压时间（分钟）
#define TIME_LOW_DEFAULT          1      // 默认低压时间（分钟）

// 实时压力显示刷新控制
#define UI_PRESSURE_REFRESH_INTERVAL_TICKS   20    // 连续模式压力刷新间隔（10ms Tick）；20=200ms
#define UI_PRESSURE_REFRESH_THRESHOLD_MMHG   0     // 最小刷新差值阈值（mmHg）

#define UI_LOCK_TIMEOUT_TICKS                3000U // 自动锁定超时时间：30s @10ms Tick
#define UI_LOCK_ICON_X                       78U   // 锁定图标显示坐标X（第一行，目标压力和电池图标之间）
#define UI_LOCK_ICON_Y                       0U    // 锁定图标显示坐标Y（第一行，页0）
#define UI_MUTE_ICON_X                       94U   // 静音图标显示坐标X（第一行，锁定图标后面）
#define UI_MUTE_ICON_Y                       0U    // 静音图标显示坐标Y（第一行，页0）

#define UI_IDLE_TIMEOUT_TICKS                6000U // 空闲超时时间：1分钟 @10ms Tick (60秒)
#define UI_IDLE_TEXT_X                       40U    // "Pump Idle"文本显示X坐标（居中显示）
#define UI_IDLE_TEXT_Y                       3U     // "Pump Idle"文本显示Y坐标（屏幕中间）
#define UI_LEAK_ALARM_TEXT_X                  20U    // "Leak Alarms"文本显示X坐标（居中显示）
#define UI_LEAK_ALARM_TEXT_Y                  3U     // "Leak Alarms"文本显示Y坐标（屏幕中间）
#define UI_BLOCKAGE_ALARM_TEXT_X               10U    // "Blockage Alarm"文本显示X坐标（居中显示）
#define UI_BLOCKAGE_ALARM_TEXT_Y               3U     // "Blockage Alarm"文本显示Y坐标（屏幕中间）
#define UI_OVERPRESSURE_ALARM_TEXT_X           5U     // "Canister Full"文本显示X坐标（居中显示）
#define UI_OVERPRESSURE_ALARM_TEXT_Y           3U     // "Canister Full"文本显示Y坐标（屏幕中间）

#include "../Inc/app_button.h"    // 获取KeyEvent_t和队列接口
#include "../Inc/app_battery.h"   // 电池管理模块
#include "../Inc/app_pressure.h"  // 压力管理模块
#include "../Inc/app_alarm.h"     // 报警管理模块
#include "../Inc/app_beep.h"      // 蜂鸣器管理模块
#include "../Inc/app_settings.h"  // 参数保存/加载模块
#include "../Inc/app_language.h"  // 多语言管理模块
#include "../../Core/Inc/system_config.h"  // 系统配置（包含电池图标坐标宏、PRESSURE_LEAK_ALARM_PUMP_STOP_DELAY_MS等）
#include "../../Middleware/Inc/soft_timer.h"  // 软件定时器
#include "../../Middleware/Inc/fsm.h"
#include "../../Middleware/Inc/display.h"
#include "../../Middleware/Inc/key_machine.h"
#include "../../Middleware/Inc/soft_timer.h"
#include "../../HAL/Inc/hal_gpio.h"  // HAL层GPIO接口（用于背光控制）
#include <stdio.h>

/****************************************************************************
 * 私有变量
 ****************************************************************************/

/* FSM实例 */
static st_fsm g_ui_fsm;

/* UI上下文 */
static UIContext_t g_ui_context;

/* 按键事件队列指针（从app_button模块获取） */
static st_queue_ptr g_key_event_queue = NULL;

/* 初始化超时定时器句柄 */
static SoftTimerHandle_t g_init_timeout_timer = 0;           // 7秒超时定时器（进入待机模式）
static SoftTimerHandle_t g_init_switch_timer = 0;            // 4秒切换定时器（从Logo切换到字符串）
static SoftTimerHandle_t g_leak_alarm_pump_stop_timer = 0;  // 泄漏报警延迟停止泵电机定时器
static bool g_leak_alarm_beep_state = false;  // 泄漏报警声音报警状态：true=应该响, false=应该静音

/* 连续模式界面上次显示的实时压力值（用于检测显示是否需要刷新） */
static uint16_t g_last_display_pressure = 0xFFFF;
static uint16_t g_pressure_refresh_tick = 0;  // 连续模式压力刷新计数器（10ms Tick）

/* 压力零点是否已完成初始化校准 */
static bool g_pressure_zero_calibrated = false;

/****************************************************************************
 * 私有函数声明
 ****************************************************************************/

static void AppUI_StateEntry_SYS(void* arg, st_fsm_event event);
static void AppUI_StateEntry_WAT(void* arg, st_fsm_event event);
static void AppUI_StateEntry_LIX(void* arg, st_fsm_event event);
static void AppUI_StateEntry_JIX(void* arg, st_fsm_event event);
static void AppUI_StateEntry_ZHT(void* arg, st_fsm_event event);
static void AppUI_StateEntry_SET(void* arg, st_fsm_event event);
static void AppUI_StateEntry_SET_Pressure(void* arg, st_fsm_event event);
static void AppUI_StateEntry_SET_HP_Pressure(void* arg, st_fsm_event event);
static void AppUI_StateEntry_SET_LP_Pressure(void* arg, st_fsm_event event);
static void AppUI_StateEntry_SET_Time(void* arg, st_fsm_event event);
static void AppUI_StateEntry_ZHT_ToTherapy(void* arg, st_fsm_event event);
static void AppUI_SwitchWorkMode(void* arg, st_fsm_event event);
static void AppUI_AdjustPressureUp(void* arg, st_fsm_event event);
static void AppUI_AdjustPressureDown(void* arg, st_fsm_event event);
static void AppUI_AdjustTimeUp(void* arg, st_fsm_event event);
static void AppUI_AdjustTimeDown(void* arg, st_fsm_event event);
static void AppUI_SwitchTimeEdit(void* arg, st_fsm_event event);
static void AppUI_TimeSetting_ExitToPause(void* arg, st_fsm_event event);

static void AppUI_Display_SYS(void);
static void AppUI_Display_WAT(void);
static void AppUI_Display_LIX(void);
static void AppUI_Display_JIX(void);
static void AppUI_Display_ZHT(void);
static void AppUI_Display_SET(void);
static void AppUI_Display_SET_Pressure(void);
static void AppUI_Display_SET_HP_Pressure(void);
static void AppUI_Display_SET_LP_Pressure(void);
static void AppUI_Display_SET_Time(void);
static void AppUI_RefreshPressureValue(void);  // 刷新连续模式压力值显示（局部刷新）
static void AppUI_ResetLockTimer(void);
static void AppUI_ShowLockIcon(void);
static void AppUI_HideLockIcon(void);
static void AppUI_EnterAutoLock(void);
static void AppUI_ExitAutoLock(void);
static void AppUI_UpdateAutoLock(void);
static bool AppUI_IsLockableState(UIState_e state);
static bool AppUI_IsIdleableState(UIState_e state);
static void AppUI_EnterIdle(void);
static void AppUI_ExitIdle(void);
static void AppUI_UpdateIdle(void);
static void AppUI_ResetIdleTimer(void);
static void AppUI_EnterLeakAlarm(void);
static void AppUI_ExitLeakAlarm(void);
static void AppUI_UpdateLeakAlarm(void);
static void AppUI_EnterBlockageAlarm(void);
static void AppUI_ExitBlockageAlarm(void);
static void AppUI_UpdateBlockageAlarm(void);
static void AppUI_EnterOverpressureAlarm(void);
static void AppUI_ExitOverpressureAlarm(void);
static void AppUI_UpdateOverpressureAlarm(void);
static void AppUI_LeakAlarmPumpStopCallback(void* user_data);

/**
 * @name      AppUI_ConvertKeyEvent
 * @brief     将按键事件转换为UI事件
 * @param     key_id    按键编号：0=OK/START，1=UP，2=DN，3=CANCEL
 * @param     key_event 按键机状态机事件（短按、长按、长按释放等）
 * @retval    UI事件枚举，若不需处理返回UI_EVENT_NONE
 */
static UIEvent_e AppUI_ConvertKeyEvent(uint8_t key_id, KeyMachineEvent_e key_event)
{
    /* key_id: 0=OK/START, 1=UP, 2=DN, 3=CANCEL */

    if (key_id == APP_BUTTON_KEY_ID_UNLOCK_COMBO &&
        key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
        return UI_EVENT_UNLOCK;
    }
    
    if (key_event == KEY_MACHINE_EVENT_LONG_PRESS) {
        /* 长按保持：用于启动/快速调整 */
        switch (key_id) {
            case 1: return UI_EVENT_NONE;        // 上键长按：当前无需处理
            case 2: return UI_EVENT_QUICK_DOWN;  // 下键长按：触发快速向下调整
            case 0: return UI_EVENT_START;       // 启动键长按（按住不松手）——直接启动治疗
            default: return UI_EVENT_NONE;
        }
    }
    else if (key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
        /* 长按释放：根据当前界面做"确认/进入下一界面"等操作 */
        switch (key_id) {
            case 0: 
                // 启动键长按释放：根据当前界面决定目标事件
                if (g_ui_context.current_state == UI_STATE_SET) {
                    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
                        return UI_EVENT_CONFIRM_LONG;  // 连续模式：进入连续压力设置
                    } else {
                        return UI_EVENT_CONFIRM;       // 间歇模式：进入高压设置
                    }
                }
                else if (g_ui_context.current_state == UI_STATE_SET_PRESSURE) {
                    return UI_EVENT_CONFIRM_LONG;  // 连续模式设置界面→暂停界面
                }
                else if (g_ui_context.current_state == UI_STATE_LIX || 
                         g_ui_context.current_state == UI_STATE_JIX) {
                    return UI_EVENT_CONFIRM_LONG;  // 治疗界面→暂停界面
                }
                else if (g_ui_context.current_state == UI_STATE_ZHT) {
                    return UI_EVENT_CONFIRM_LONG;  // 暂停界面→恢复治疗
                }
                else if (g_ui_context.current_state == UI_STATE_SET_HP_PRESSURE) {
                    return UI_EVENT_CONFIRM_LONG;  // 间歇模式高压设置→低压设置
                }
                else if (g_ui_context.current_state == UI_STATE_SET_LP_PRESSURE) {
                    return UI_EVENT_CONFIRM_LONG;  // 间歇模式低压设置→时间设置
                }
                else if (g_ui_context.current_state == UI_STATE_SET_TIME) {
                    if (g_ui_context.time_edit_high) {
                        return UI_EVENT_CONFIRM_LONG;  // 高压时间→切换到低压时间
                    } else {
                        return UI_EVENT_CONFIRM;       // 低压时间→退出到暂停界面
                    }
                }
                // 其他状态下默认为普通确认
                return UI_EVENT_CONFIRM;
            case 1: return UI_EVENT_MENU_UP;      // 上键长按释放：菜单向上/参数增加
            case 2: return UI_EVENT_MENU_DOWN;    // 下键长按释放：菜单向下/参数减少
            case 3: return UI_EVENT_CONFIRM;      // 取消键长按释放：按当前需求视作确认/返回
            default: return UI_EVENT_NONE;
        }
    }
    else if (key_event == KEY_MACHINE_EVENT_ULTRA_LONG_PRESS_RELEASE) {
        // 超长按释放：预留扩展功能，当前不处理
        return UI_EVENT_NONE;
    }
 
    // 其他按键事件（短按、短按释放等）：按需在这里继续扩展
    return UI_EVENT_NONE;
}

/**
 * @brief 判断当前UI状态是否需要参与自动锁定逻辑
 * @param state UI状态枚举
 * @retval true  需要计时并可能锁定（连续/间歇治疗界面）
 * @retval false 不需要自动锁定
 */
static bool AppUI_IsLockableState(UIState_e state)
{
    return (state == UI_STATE_LIX) || (state == UI_STATE_JIX) || (state == UI_STATE_ZHT);
}

/**
 * @brief 重置无操作计时器
 */
static void AppUI_ResetLockTimer(void)
{
    g_ui_context.lock_inactive_ticks = 0;
}

/**
 * @brief 在指定坐标显示锁定图标（若尚未显示）
 */
static void AppUI_ShowLockIcon(void)
{
    if (g_ui_context.lock_icon_visible == false) {
        Display_ShowIcon(UI_LOCK_ICON_X, UI_LOCK_ICON_Y, ICON_LOCK);
        g_ui_context.lock_icon_visible = true;
    }
}

/**
 * @brief 清除锁定图标占用区域（若当前可见）
 */
static void AppUI_HideLockIcon(void)
{
    if (g_ui_context.lock_icon_visible) {
        Display_ClearRect(UI_LOCK_ICON_X, UI_LOCK_ICON_Y, 8, 16);
        g_ui_context.lock_icon_visible = false;
    }
}

/**
 * @brief 在指定坐标显示静音图标（若尚未显示）
 */
static void AppUI_ShowMuteIcon(void)
{
    if (AppBeep_IsMuted()) {
        Display_ShowIcon(UI_MUTE_ICON_X, UI_MUTE_ICON_Y, ICON_SILENT);
    }
}

/**
 * @brief 清除静音图标占用区域（若当前可见）
 */
static void AppUI_HideMuteIcon(void)
{
    if (AppBeep_IsMuted() == false) {
        Display_ClearRect(UI_MUTE_ICON_X, UI_MUTE_ICON_Y, 16, 16);
    }
}

/**
 * @brief 更新静音图标显示状态（根据当前静音状态显示或隐藏）
 */
static void AppUI_UpdateMuteIcon(void)
{
    if (AppBeep_IsMuted()) {
        Display_ShowIcon(UI_MUTE_ICON_X, UI_MUTE_ICON_Y, ICON_SILENT);
    } else {
        Display_ClearRect(UI_MUTE_ICON_X, UI_MUTE_ICON_Y, 16, 16);
    }
}

/**
 * @brief 进入自动锁定：置锁标志、显示图标、关闭背光
 */
static void AppUI_EnterAutoLock(void)
{
    if (g_ui_context.auto_lock_active) {
        return;
    }

    g_ui_context.auto_lock_active = true;
    AppUI_SetLockFlag(true);
    AppUI_ShowLockIcon();
    HAL_LCD_Backlight_Off();  // 锁定后关闭背光
}

/**
 * @brief 退出自动锁定：清除锁状态与图标、打开背光，同时复位计时
 */
static void AppUI_ExitAutoLock(void)
{
    if (g_ui_context.auto_lock_active == false && g_ui_context.lock_icon_visible == false) {
        AppUI_ResetLockTimer();
        return;
    }

    g_ui_context.auto_lock_active = false;
    AppUI_SetLockFlag(false);
    AppUI_HideLockIcon();
    HAL_LCD_Backlight_On();  // 解锁后打开背光
    AppUI_ResetLockTimer();
}

/**
 * @brief 自动锁定状态机：在可锁定页面统计无操作时间并触发/退出锁定
 */
static void AppUI_UpdateAutoLock(void)
{
    if (AppUI_IsLockableState(g_ui_context.current_state) == false) {
        if (g_ui_context.auto_lock_active) {
            AppUI_ExitAutoLock();
        } else {
            AppUI_HideLockIcon();
            g_ui_context.lock_inactive_ticks = 0;
        }
        return;
    }

    if (g_ui_context.auto_lock_active) {
        /* 已锁定，等待组合解锁 */
        return;
    }

    if (g_ui_context.lock_inactive_ticks < UI_LOCK_TIMEOUT_TICKS) {
        g_ui_context.lock_inactive_ticks++;
        if (g_ui_context.lock_inactive_ticks >= UI_LOCK_TIMEOUT_TICKS) {
            AppUI_EnterAutoLock();
        }
    }
}

/**
 * @brief 判断当前状态是否可进入空闲检测
 * @param state UI状态
 * @retval true 可进入空闲检测（仅暂停模式）
 * @retval false 不可进入空闲检测
 */
static bool AppUI_IsIdleableState(UIState_e state)
{
    return (state == UI_STATE_ZHT);
}

/**
 * @brief 重置空闲计时器
 */
static void AppUI_ResetIdleTimer(void)
{
    g_ui_context.idle_inactive_ticks = 0;
}

/**
 * @brief 进入空闲状态：显示"Pump Idle"，关闭白色背光，打开黄色背光
 */
static void AppUI_EnterIdle(void)
{
    g_ui_context.idle_active = true;
    g_ui_context.idle_previous_state = g_ui_context.current_state;
    
    /* 清屏并显示"Pump Idle" */
    Display_Clear();
    Display_ShowString(UI_IDLE_TEXT_X, UI_IDLE_TEXT_Y, "Pump Idle", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    /* 关闭白色背光，打开黄色背光 */
    HAL_LCD_Backlight_Off();
    HAL_LED_Yellow_On();
    
    /* 启动蜂鸣器模式2（{8, 250}：响150ms，静5000ms） */
    AppBeep_StartBeep2DMode(2);
}

/**
 * @brief 退出空闲状态：恢复之前的显示，恢复背光状态
 */
static void AppUI_ExitIdle(void)
{
    g_ui_context.idle_active = false;
    
    /* 如果之前处于锁屏状态，清除锁屏状态（因为空闲状态下锁屏检测被暂停） */
    if (g_ui_context.auto_lock_active) {
        AppUI_ExitAutoLock();
    }
    
    /* 恢复之前的显示 */
    Display_Clear();
    switch (g_ui_context.idle_previous_state) {
        case UI_STATE_LIX:
            AppUI_Display_LIX();
            break;
        case UI_STATE_JIX:
            AppUI_Display_JIX();
            break;
        case UI_STATE_ZHT:
            AppUI_Display_ZHT();
            break;
        default:
            break;
    }
    
    /* 恢复背光状态：打开白色背光，关闭黄色背光 */
    HAL_LCD_Backlight_On();
    HAL_LED_Yellow_Off();
    
    /* 停止蜂鸣器模式2的工作 */
    AppBeep_StopBeep();
    
    /* 重置空闲计时器和锁屏计时器 */
    AppUI_ResetIdleTimer();
    AppUI_ResetLockTimer();
}

/**
 * @brief 空闲检测状态机：在可空闲页面统计无操作时间并触发/退出空闲
 */
static void AppUI_UpdateIdle(void)
{
    /* 如果不在可空闲状态，退出空闲并返回 */
    if (AppUI_IsIdleableState(g_ui_context.current_state) == false) {
        if (g_ui_context.idle_active) {
            AppUI_ExitIdle();
        } else {
            AppUI_ResetIdleTimer();
        }
        return;
    }

    /* 如果已进入空闲状态，等待长按恢复 */
    if (g_ui_context.idle_active) {
        return;
    }

    /* 递增空闲计时器，超时则进入空闲状态 */
    if (g_ui_context.idle_inactive_ticks < UI_IDLE_TIMEOUT_TICKS) {
        g_ui_context.idle_inactive_ticks++;
        if (g_ui_context.idle_inactive_ticks >= UI_IDLE_TIMEOUT_TICKS) {
            AppUI_EnterIdle();
        }
    }
}

/**
 * @brief 泄漏报警延迟停止泵电机定时器回调函数
 * @note 停止压力控制（停止泵电机和PID控制），但UI层会继续维护泄漏报警状态
 */
static void AppUI_LeakAlarmPumpStopCallback(void* user_data)
{
    (void)user_data;
    
    /* 延迟时间到，停止压力控制（停止泵电机和PID控制） */
    /* 注意：AppPressure_StopControl() 会清除泄漏报警标志，但UI层通过 */
    /* g_ui_context.leak_alarm_active 来维护泄漏报警状态，所以不影响UI显示 */
    AppPressure_StopControl();
}

/**
 * @brief 进入泄漏报警状态：显示"Leak Alarms"，关闭白色背光，打开黄色背光
 * @note 不立即停止泵电机，而是延迟一段时间（由PRESSURE_LEAK_ALARM_PUMP_STOP_DELAY_MS定义）后再停止
 */
static void AppUI_EnterLeakAlarm(void)
{
    g_ui_context.leak_alarm_active = true;
    g_ui_context.leak_alarm_previous_state = g_ui_context.current_state;
    
    /* 不立即停止压力控制，而是启动延迟停止定时器 */
    if (g_leak_alarm_pump_stop_timer == 0) {
        g_leak_alarm_pump_stop_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE,
                                                        PRESSURE_LEAK_ALARM_PUMP_STOP_DELAY_MS,
                                                        AppUI_LeakAlarmPumpStopCallback,
                                                        NULL);
        if (g_leak_alarm_pump_stop_timer != 0) {
            SoftTimer_Start(g_leak_alarm_pump_stop_timer);
        } else {
            /* 定时器创建失败，立即停止压力控制（安全措施） */
            AppPressure_StopControl();
        }
    } else {
        /* 定时器已存在，重新设置周期并启动 */
        SoftTimer_Stop(g_leak_alarm_pump_stop_timer);
        SoftTimer_SetPeriod(g_leak_alarm_pump_stop_timer, PRESSURE_LEAK_ALARM_PUMP_STOP_DELAY_MS);
        SoftTimer_SetCallback(g_leak_alarm_pump_stop_timer, AppUI_LeakAlarmPumpStopCallback, NULL);
        SoftTimer_Start(g_leak_alarm_pump_stop_timer);
    }
    
    /* 清屏并显示"Leak Alarms" */
    Display_Clear();
    Display_ShowString(UI_LEAK_ALARM_TEXT_X, UI_LEAK_ALARM_TEXT_Y, "Leak Alarms", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    /* 关闭白色背光，打开黄色背光 */
    HAL_LCD_Backlight_Off();
    HAL_LED_Yellow_On();
    
    /* 进入泄漏报警时，立即启动声音报警 */
    /* 在延迟60秒期间（30s到90s），无论电池状态如何，都保持声音报警 */
    /* 延迟60秒后（电机停止后），根据电池状态控制声音报警 */
    g_leak_alarm_beep_state = true;  // 初始状态设为应该响
    AppBeep_StartBeep2DMode(2);
}

/**
 * @brief 退出泄漏报警状态：恢复到暂停模式界面
 */
static void AppUI_ExitLeakAlarm(void)
{
    g_ui_context.leak_alarm_active = false;
    
    /* 停止延迟停止泵电机的定时器（如果还在运行） */
    if (g_leak_alarm_pump_stop_timer != 0) {
        SoftTimer_Stop(g_leak_alarm_pump_stop_timer);
    }
    
    /* 立即停止压力控制（退出泄漏报警时确保停止） */
    AppPressure_StopControl();
    
    /* 清除泄漏报警标志 */
    AppPressure_ClearLeakAlarm();
    
    /* 重置声音报警状态 */
    g_leak_alarm_beep_state = false;
    
    /* 如果之前处于锁屏状态，清除锁屏状态 */
    if (g_ui_context.auto_lock_active) {
        AppUI_ExitAutoLock();
    }
    
    /* 恢复到暂停模式界面 */
    Display_Clear();
    AppUI_StateEntry_ZHT(&g_ui_context, (st_fsm_event){0});
    g_ui_fsm.current_state = UI_STATE_ZHT;  // 手动同步FSM状态
    
    /* 恢复背光状态：打开白色背光，关闭黄色背光 */
    HAL_LCD_Backlight_On();
    HAL_LED_Yellow_Off();
    
    /* 停止蜂鸣器模式2的工作 */
    AppBeep_StopBeep();
    
    /* 重置锁屏计时器 */
    AppUI_ResetLockTimer();
}

/**
 * @brief 泄漏报警检测：检测压力控制模块的泄漏报警标志
 * @note 在泄漏报警状态下，持续检测电池状态，如果电池满电或正在充电则停止声音报警
 */
static void AppUI_UpdateLeakAlarm(void)
{
    /* 检测压力控制模块的泄漏报警标志 */
    if (AppPressure_IsLeakAlarmTriggered()) {
        /* 如果未进入泄漏报警状态，则触发 */
        if (!g_ui_context.leak_alarm_active) {
            AppUI_EnterLeakAlarm();
        }
    }
    
    /* 如果已进入泄漏报警状态，持续检测电池状态并控制声音报警 */
    /* 注意：即使泄漏报警标志被清除，只要UI仍处于报警状态，就保持报警状态 */
    /* 泄漏报警状态只能通过用户手动退出（长按电源键）来清除 */
    if (g_ui_context.leak_alarm_active) {
        /* 检查是否还在延迟60秒期间（30s到90s之间） */
        /* 如果定时器还在运行，说明还在延迟期间 */
        bool in_delay_period = false;
        if (g_leak_alarm_pump_stop_timer != 0) {
            SoftTimerState_e timer_state = SoftTimer_GetState(g_leak_alarm_pump_stop_timer);
            if (timer_state == SOFT_TIMER_STATE_RUNNING) {
                in_delay_period = true;
            }
        }
        
        /* 如果还在延迟期间，无论电池状态如何，都保持声音报警 */
        if (in_delay_period) {
            /* 延迟期间，强制保持声音报警 */
            if (!g_leak_alarm_beep_state) {
                g_leak_alarm_beep_state = true;
                AppBeep_StartBeep2DMode(2);
            }
        } else {
            /* 延迟期间已结束（电机已停止），根据电池状态控制声音报警 */
            BatteryLevel_e battery_level = AppBattery_GetLevelEnum();
            bool is_charging = AppBattery_IsCharging();
            
            /* 判断应该的声音报警状态 */
            bool should_beep = !(battery_level == BATTERY_LEVEL_FULL || is_charging);
            
            /* 只有当状态发生变化时才更新声音报警，避免频繁启动/停止 */
            if (should_beep != g_leak_alarm_beep_state) {
                g_leak_alarm_beep_state = should_beep;
                
                if (should_beep) {
                    /* 应该响：启动/恢复声音报警 */
                    AppBeep_StartBeep2DMode(2);
                } else {
                    /* 应该静音：停止声音报警 */
                    AppBeep_StopBeep();
                }
            }
        }
    }
}

/**
 * @brief 进入管路堵塞报警状态：显示"Blockage Alarm"，关闭白色背光，打开黄色背光
 */
static void AppUI_EnterBlockageAlarm(void)
{
    g_ui_context.blockage_alarm_active = true;
    g_ui_context.blockage_alarm_previous_state = g_ui_context.current_state;
    
    /* 如果之前处于空闲状态，先退出空闲状态（因为管路堵塞报警优先级高于空闲检测） */
    if (g_ui_context.idle_active) {
        /* 注意：这里只清除空闲状态标志，不恢复显示，因为管路堵塞报警会立即覆盖显示 */
        g_ui_context.idle_active = false;
        /* 不清除idle_previous_state，因为ExitBlockageAlarm会使用blockage_alarm_previous_state */
    }
    
    /* 清屏并显示"Blockage Alarm" */
    Display_Clear();
    Display_ShowString(UI_BLOCKAGE_ALARM_TEXT_X, UI_BLOCKAGE_ALARM_TEXT_Y, "Blockage Alarm", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    /* 关闭白色背光，打开黄色背光 */
    HAL_LCD_Backlight_Off();
    HAL_LED_Yellow_On();
    
    /* 启动蜂鸣器模式2（{8, 250}：响150ms，静5000ms） */
    AppBeep_StartBeep2DMode(2);
}

/**
 * @brief 退出管路堵塞报警状态：清理状态，恢复背光（不负责显示，由调用者处理）
 */
static void AppUI_ExitBlockageAlarm(void)
{
    g_ui_context.blockage_alarm_active = false;
    
    /* 如果之前处于锁屏状态，清除锁屏状态（因为管路堵塞报警状态下锁屏检测被暂停） */
    if (g_ui_context.auto_lock_active) {
        AppUI_ExitAutoLock();
    }
    
    /* 恢复背光状态：打开白色背光，关闭黄色背光 */
    HAL_LCD_Backlight_On();
    HAL_LED_Yellow_Off();
    
    /* 停止蜂鸣器模式2的工作 */
    AppBeep_StopBeep();
    
    /* 清除管路堵塞报警标志 */
    AppPressure_ClearBlockageAlarm();
    
    /* 重置锁屏计时器 */
    AppUI_ResetLockTimer();
}

/**
 * @brief 管路堵塞报警检测：检测压力控制模块的管路堵塞报警标志
 * @note 如果PID输出大于阈值（说明有漏气，PID在补充），自动退出管路堵塞报警
 */
static void AppUI_UpdateBlockageAlarm(void)
{
    /* 检测压力控制模块的管路堵塞报警标志 */
    if (AppPressure_IsBlockageAlarmTriggered()) {
        /* 如果未进入管路堵塞报警状态，则触发 */
        if (!g_ui_context.blockage_alarm_active) {
            AppUI_EnterBlockageAlarm();
        }
    }
    
    /* 如果已进入管路堵塞报警状态，检测PID输出 */
    if (g_ui_context.blockage_alarm_active) {
        /* 如果PID输出大于阈值，说明有漏气，PID在补充，自动退出管路堵塞报警 */
        float pid_output = AppPressure_GetLastOutput();
        if (pid_output > PRESSURE_BLOCKAGE_ALARM_PID_THRESHOLD) {
            /* PID输出大于阈值，说明有负压补充，自动退出管路堵塞报警 */
            AppUI_ExitBlockageAlarm();
            /* 恢复到之前的治疗模式（清屏并重新显示） */
            Display_Clear();
            if (g_ui_context.blockage_alarm_previous_state == UI_STATE_LIX) {
                AppUI_Display_LIX();
            } else if (g_ui_context.blockage_alarm_previous_state == UI_STATE_JIX) {
                AppUI_Display_JIX();
            }
        }
    }
}

/**
 * @brief 进入过压报警状态：显示"Canister Full"，关闭白色背光，打开黄色背光
 * @note 收集罐已满导致压力异常高时触发
 */
static void AppUI_EnterOverpressureAlarm(void)
{
    g_ui_context.overpressure_alarm_active = true;
    g_ui_context.overpressure_alarm_previous_state = g_ui_context.current_state;
    
    /* 如果之前处于空闲状态，先退出空闲状态（因为过压报警优先级最高） */
    if (g_ui_context.idle_active) {
        g_ui_context.idle_active = false;
    }
    
    /* 清屏并显示"Canister Full" */
    Display_Clear();
    Display_ShowString(UI_OVERPRESSURE_ALARM_TEXT_X, UI_OVERPRESSURE_ALARM_TEXT_Y, "Canister Full", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    /* 关闭白色背光，打开黄色背光 */
    HAL_LCD_Backlight_Off();
    HAL_LED_Yellow_On();
    
    /* 启动蜂鸣器模式2（{8, 250}：响150ms，静5000ms） */
    AppBeep_StartBeep2DMode(2);
    
    /* 注意：不打开电磁阀，只显示警告，由用户处理 */
}

/**
 * @brief 退出过压报警状态：清理状态，恢复背光
 */
static void AppUI_ExitOverpressureAlarm(void)
{
    g_ui_context.overpressure_alarm_active = false;
    
    /* 如果之前处于锁屏状态，清除锁屏状态 */
    if (g_ui_context.auto_lock_active) {
        AppUI_ExitAutoLock();
    }
    
    /* 恢复背光状态：打开白色背光，关闭黄色背光 */
    HAL_LCD_Backlight_On();
    HAL_LED_Yellow_Off();
    
    /* 停止蜂鸣器 */
    AppBeep_StopBeep();
    
    /* 清除过压报警标志 */
    AppPressure_ClearOverpressureAlarm();
    
    /* 重置空闲计时器和锁屏计时器 */
    AppUI_ResetIdleTimer();
    AppUI_ResetLockTimer();
}

/**
 * @brief 过压报警检测：检测压力控制模块的过压报警标志
 * @note 由条件1（压力超过阈值）触发的报警：当压力恢复正常时自动退出
 *       由条件2（建立时间过短，液位满）触发的报警：只能手动退出，不会自动消失
 */
static void AppUI_UpdateOverpressureAlarm(void)
{
    /* 检测压力控制模块的过压报警标志 */
    if (AppPressure_IsOverpressureAlarmTriggered()) {
        /* 如果未进入过压报警状态，则触发 */
        if (!g_ui_context.overpressure_alarm_active) {
            AppUI_EnterOverpressureAlarm();
        }
    }
    
    /* 如果已进入过压报警状态，检测压力是否恢复正常 */
    if (g_ui_context.overpressure_alarm_active) {
        /* 只有由条件1（压力超过阈值）触发的报警才允许自动退出 */
        /* 由条件2（建立时间过短，液位满）触发的报警不应自动退出，只能手动退出 */
        if (!AppPressure_IsOverpressureAlarmByBuildTime()) {
            /* 获取当前压力与目标的偏差 */
            int16_t deviation = AppPressure_GetPressureDeviation();
            
            /* 如果偏差回落到正常范围（目标+20mmHg以下），自动退出过压报警 */
            if (deviation < (PRESSURE_OVERPRESSURE_ALARM_THRESHOLD_MMHG - 10)) {
                AppUI_ExitOverpressureAlarm();
                /* 恢复到之前的治疗模式（清屏并重新显示） */
                Display_Clear();
                if (g_ui_context.overpressure_alarm_previous_state == UI_STATE_LIX) {
                    AppUI_Display_LIX();
                } else if (g_ui_context.overpressure_alarm_previous_state == UI_STATE_JIX) {
                    AppUI_Display_JIX();
                }
            }
        }
        /* 如果是由建立时间过短触发的报警，不检查压力偏差，保持报警状态直到用户手动退出 */
    }
}

/* 初始化超时定时器回调函数 */
static void AppUI_InitTimeoutCallback(void* user_data);

/****************************************************************************
 * FSM状态转换表（必须在函数声明之后定义）
 ****************************************************************************/

const st_fsm_transition g_ui_transition_table[24] = {
    [0] = {
        .current_state = UI_STATE_SYS,                       /* 当前状态：初始化模式 */
        .trigger_event = UI_EVENT_TIMEOUT,                   /* 触发事件：超时 */
        .action_func   = AppUI_StateEntry_WAT,               /* 动作函数：进入待机模式 */
        .next_state    = UI_STATE_WAT                        /* 下一状态：待机模式 */
    },
    
    [1] = {
        .current_state = UI_STATE_WAT,                       /* 当前状态：待机模式 */
        .trigger_event = UI_EVENT_MENU_UP,                   /* 触发事件：菜单向上 */
        .action_func   = AppUI_StateEntry_SET,               /* 动作函数：进入设置模式 */
        .next_state    = UI_STATE_SET                        /* 下一状态：设置模式 */
    },
    
    [2] = {
        .current_state = UI_STATE_WAT,                       /* 当前状态：待机模式 */
        .trigger_event = UI_EVENT_START,                     /* 触发事件：启动 */
        .action_func   = AppUI_StateEntry_LIX,               /* 动作函数：进入连续工作模式 */
        .next_state    = UI_STATE_LIX                        /* 下一状态：连续工作模式 */
    },
    
    [3] = {
        .current_state = UI_STATE_LIX,                       /* 当前状态：连续工作模式 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,              /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_ZHT,               /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                        /* 下一状态：暂停模式 */
    },
    
    [4] = {
        .current_state = UI_STATE_JIX,                       /* 当前状态：间歇工作模式 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,              /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_ZHT,               /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                        /* 下一状态：暂停模式 */
    },           
    
    [5] = {          
        .current_state = UI_STATE_ZHT,                       /* 当前状态：暂停模式 */
        .trigger_event = UI_EVENT_MENU_UP,                   /* 触发事件：向上方向键长按释放 */
        .action_func   = AppUI_StateEntry_SET,               /* 动作函数：进入设置模式 */
        .next_state    = UI_STATE_SET                        /* 下一状态：设置模式 */
    },
                 
    [6] = {          
        .current_state = UI_STATE_ZHT,                      /* 当前状态：暂停模式 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_ZHT_ToTherapy,    /* 动作函数：根据work_mode_backup进入治疗模式 */
        .next_state    = UI_STATE_LIX                       /* 下一状态：默认连续模式，实际由动作函数决定 */
    },

    [7] = {            
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_SwitchWorkMode,              /* 动作函数：切换工作模式 */
        .next_state    = UI_STATE_SET                       /* 下一状态：保持设置模式 */
    },          

    [8] = {            
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_SwitchWorkMode,              /* 动作函数：切换工作模式 */
        .next_state    = UI_STATE_SET                       /* 下一状态：保持设置模式 */
    },          
    
    [9] = {
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放（连续模式） */
        .action_func   = AppUI_StateEntry_SET_Pressure,     /* 动作函数：进入连续模式压力设置 */
        .next_state    = UI_STATE_SET_PRESSURE              /* 下一状态：连续模式压力设置界面 */
    },
    
    [10] = {
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_CONFIRM,                  /* 触发事件：确认键长按释放（间歇模式映射为普通确认） */
        .action_func   = AppUI_StateEntry_SET_HP_Pressure,  /* 动作函数：进入间歇模式高压设置 */
        .next_state    = UI_STATE_SET_HP_PRESSURE           /* 下一状态：间歇模式高压设置界面 */
    },
    
    [11] = {
        .current_state = UI_STATE_SET_PRESSURE,             /* 当前状态：连续模式压力设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustPressureUp,            /* 动作函数：增加压力值 */
        .next_state    = UI_STATE_SET_PRESSURE              /* 下一状态：保持当前状态 */
    },
    
    [12] = {
        .current_state = UI_STATE_SET_PRESSURE,             /* 当前状态：连续模式压力设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustPressureDown,          /* 动作函数：减少压力值 */
        .next_state    = UI_STATE_SET_PRESSURE              /* 下一状态：保持当前状态 */
    },
    
    [13] = {
        .current_state = UI_STATE_SET_PRESSURE,             /* 当前状态：连续模式压力设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_ZHT,              /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },  
    
    [14] = {
        .current_state = UI_STATE_SET_HP_PRESSURE,          /* 当前状态：间歇模式高压设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustPressureUp,            /* 动作函数：增加高压值 */
        .next_state    = UI_STATE_SET_HP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [15] = {
        .current_state = UI_STATE_SET_HP_PRESSURE,          /* 当前状态：间歇模式高压设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustPressureDown,          /* 动作函数：减少高压值 */
        .next_state    = UI_STATE_SET_HP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [16] = {
        .current_state = UI_STATE_SET_HP_PRESSURE,          /* 当前状态：间歇模式高压设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放*/        
        .action_func   = AppUI_StateEntry_SET_LP_Pressure,  /* 动作函数：进入低压设置界面 */
        .next_state    = UI_STATE_SET_LP_PRESSURE           /* 下一状态：间歇模式低压设置界面 */
    },   
    
    [17] = {
        .current_state = UI_STATE_SET_LP_PRESSURE,          /* 当前状态：间歇模式低压设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustPressureUp,            /* 动作函数：增加低压值 */
        .next_state    = UI_STATE_SET_LP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [18] = {
        .current_state = UI_STATE_SET_LP_PRESSURE,          /* 当前状态：间歇模式低压设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustPressureDown,          /* 动作函数：减少低压值 */
        .next_state    = UI_STATE_SET_LP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [19] = {
        .current_state = UI_STATE_SET_LP_PRESSURE,          /* 当前状态：间歇模式低压设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_SET_Time,         /* 动作函数：进入时间设置界面 */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：间歇模式时间设置界面 */
    },
    
    [20] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustTimeUp,                /* 动作函数：增加时间值 */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：保持当前状态 */
    },
    
    [21] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustTimeDown,              /* 动作函数：减少时间值 */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：保持当前状态 */
    },
    
    [22] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_SwitchTimeEdit,              /* 动作函数：切换编辑项（高压时间/低压时间） */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：保持当前状态 */
    },
    
    [23] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_CONFIRM,                  /* 触发事件：确认键（长按释放，低压时间完成后退出） */
        .action_func   = AppUI_TimeSetting_ExitToPause,     /* 动作函数：退出时间设置并进入暂停 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },   
};

/****************************************************************************
 * 状态进入动作函数实现
 ****************************************************************************/

/**
 * @name      AppUI_StateEntry_SYS
 * @brief     初始化模式进入动作
 */
static void AppUI_StateEntry_SYS(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->current_state = UI_STATE_SYS;
    ctx->work_mode_backup = UI_STATE_LIX;  // 默认连续模式
    ctx->sys_show_logo = true;              // 初始显示Logo
    AppPressure_StopControl();
    AppUI_Display_SYS();
}

/**
 * @name      AppUI_StateEntry_WAT
 * @brief     待机模式进入动作
 */
static void AppUI_StateEntry_WAT(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_WAT;
    AppPressure_StopControl();
    AppUI_ExitAutoLock();
    Display_Clear();
    AppUI_Display_WAT();

    /* 进入待机界面首次执行零点校准（参考未重构工程） */
    if (!g_pressure_zero_calibrated || ctx->last_state == UI_STATE_SYS) {
        AppPressure_BleedAndCalibrateZero();
        g_pressure_zero_calibrated = true;
    }
}

/**
 * @name      AppUI_StateEntry_LIX
 * @brief     连续工作模式进入动作
 */
static void AppUI_StateEntry_LIX(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_LIX;
    ctx->work_mode_backup = UI_STATE_LIX;
    AppUI_ExitAutoLock();
    AppPressure_StartControl(ctx->pressure_high, APP_PRESSURE_CONTROL_MODE_CONTINUOUS);
    Display_Clear();
    AppUI_Display_LIX();
}

/**
 * @name      AppUI_StateEntry_JIX
 * @brief     间歇工作模式进入动作
 */
static void AppUI_StateEntry_JIX(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_JIX;
    ctx->work_mode_backup = UI_STATE_JIX;
    AppUI_ExitAutoLock();
    AppPressure_StartIntermittentTherapy(ctx->pressure_high,
                                         ctx->pressure_low,
                                         ctx->time_high,
                                         ctx->time_low);
    g_last_display_pressure = 0xFFFF;
    g_pressure_refresh_tick = 0;
    Display_Clear();
    AppUI_Display_JIX();
}

/**
 * @name      AppUI_StateEntry_ZHT
 * @brief     暂停模式进入动作
 */
static void AppUI_StateEntry_ZHT(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_ZHT;
    AppPressure_StopControl();
    AppUI_ExitAutoLock();
    Display_Clear();
    AppUI_Display_ZHT();
}

/**
 * @name      AppUI_StateEntry_ZHT_ToTherapy
 * @brief     从暂停界面进入治疗界面（根据work_mode_backup决定进入连续或间歇模式）
 */
static void AppUI_StateEntry_ZHT_ToTherapy(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    // 根据work_mode_backup决定进入哪个治疗模式
    if (ctx->work_mode_backup == UI_STATE_LIX) {
        // 进入连续模式治疗界面
        AppUI_StateEntry_LIX(arg, event);
    } else if (ctx->work_mode_backup == UI_STATE_JIX) {
        // 进入间歇模式治疗界面
        AppUI_StateEntry_JIX(arg, event);
    }
    // 如果work_mode_backup不是治疗模式，则不进行状态转换（保持暂停状态）
}

/**
 * @name      AppUI_StateEntry_SET
 * @brief     设置模式进入动作（模式选择界面）
 */
static void AppUI_StateEntry_SET(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_SET;
    AppPressure_StopControl();
    AppUI_ExitAutoLock();
    Display_Clear();
    AppUI_Display_SET();
}

/**
 * @name      AppUI_StateEntry_SET_Pressure
 * @brief     连续模式压力设置界面进入动作
 */
static void AppUI_StateEntry_SET_Pressure(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_SET_PRESSURE;
    AppPressure_StopControl();
    AppUI_ExitAutoLock();
    Display_Clear();
    AppUI_Display_SET_Pressure();
}

/**
 * @name      AppUI_StateEntry_SET_HP_Pressure
 * @brief     间歇模式高压设置界面进入动作
 */
static void AppUI_StateEntry_SET_HP_Pressure(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_SET_HP_PRESSURE;
    AppPressure_StopControl();
    AppUI_ExitAutoLock();
    Display_Clear();
    AppUI_Display_SET_HP_Pressure();
}

/**
 * @name      AppUI_StateEntry_SET_LP_Pressure
 * @brief     间歇模式低压设置界面进入动作
 */
static void AppUI_StateEntry_SET_LP_Pressure(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_SET_LP_PRESSURE;
    AppPressure_StopControl();
    AppUI_ExitAutoLock();
    Display_Clear();
    AppUI_Display_SET_LP_Pressure();
}

/**
 * @name      AppUI_StateEntry_SET_Time
 * @brief     间歇模式时间设置界面进入动作
 */
static void AppUI_StateEntry_SET_Time(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->last_state = ctx->current_state;
    ctx->current_state = UI_STATE_SET_TIME;
    ctx->time_edit_high = true;  // 重置编辑标志
    AppPressure_StopControl();
    AppUI_ExitAutoLock();
    Display_Clear();
    AppUI_Display_SET_Time();
}

/**
 * @name      AppUI_SwitchWorkMode
 * @brief     切换工作模式（在设置界面中）
 */
static void AppUI_SwitchWorkMode(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    // 切换工作模式：连续 <-> 间歇
    if (ctx->work_mode_backup == UI_STATE_LIX) {
        ctx->work_mode_backup = UI_STATE_JIX;
    } else if (ctx->work_mode_backup == UI_STATE_JIX) {
        ctx->work_mode_backup = UI_STATE_LIX;
    }
    
    // 注意：显示刷新由AppUI_Process统一处理，这里只需要更新work_mode_backup
    // AppUI_Process会检测到work_mode_backup变化并自动刷新显示
}

/**
 * @name      AppUI_AdjustPressureUp
 * @brief     增加压力值
 */
static void AppUI_AdjustPressureUp(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    // 根据当前状态决定调整哪个压力值
    if (ctx->current_state == UI_STATE_SET_PRESSURE) {
        // 连续模式：调整pressure_high
        ctx->pressure_high += ctx->pressure_step;  // 使用变量步长
        if (ctx->pressure_high > PRESSURE_HIGH_MAX) {
            ctx->pressure_high = PRESSURE_HIGH_MIN;
        }
        // 使用局部刷新函数，只刷新压力值区域，避免闪烁
        AppUI_RefreshPressureValue();
    } else if (ctx->current_state == UI_STATE_SET_HP_PRESSURE) {
        // 间歇模式高压：调整pressure_high
        ctx->pressure_high += ctx->pressure_step;  // 使用变量步长
        if (ctx->pressure_high > PRESSURE_HIGH_MAX) {
            ctx->pressure_high = PRESSURE_HIGH_MIN;
        }
        // 确保低压值不超过高压值
        if (ctx->pressure_low >= ctx->pressure_high - ctx->pressure_step) {
            ctx->pressure_low = ctx->pressure_high - ctx->pressure_step;
            if (ctx->pressure_low < PRESSURE_LOW_MIN) {
                ctx->pressure_low = PRESSURE_LOW_MIN;
            }
        }
        AppUI_Display_SET_HP_Pressure();
    } else if (ctx->current_state == UI_STATE_SET_LP_PRESSURE) {
        // 间歇模式低压：调整pressure_low
        ctx->pressure_low += ctx->pressure_step;  // 使用变量步长
        if (ctx->pressure_low > PRESSURE_LOW_MAX || 
            ctx->pressure_low >= ctx->pressure_high - ctx->pressure_step) {
            ctx->pressure_low = PRESSURE_LOW_MIN;
        }
        AppUI_Display_SET_LP_Pressure();
    }
}

/**
 * @name      AppUI_AdjustPressureDown
 * @brief     减少压力值
 */
static void AppUI_AdjustPressureDown(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    // 根据当前状态决定调整哪个压力值
    if (ctx->current_state == UI_STATE_SET_PRESSURE) {
        // 连续模式：调整pressure_high
        if (ctx->pressure_high > ctx->pressure_step) {
            ctx->pressure_high -= ctx->pressure_step;  // 使用变量步长
        } else {
            ctx->pressure_high = PRESSURE_HIGH_MAX;
        }
        // 使用局部刷新函数，只刷新压力值区域，避免闪烁
        AppUI_RefreshPressureValue();
    } else if (ctx->current_state == UI_STATE_SET_HP_PRESSURE) {
        // 间歇模式高压：调整pressure_high
        if (ctx->pressure_high > ctx->pressure_step) {
            ctx->pressure_high -= ctx->pressure_step;  // 使用变量步长
        } else {
            ctx->pressure_high = PRESSURE_HIGH_MAX;
        }
        // 确保低压值不超过高压值
        if (ctx->pressure_low >= ctx->pressure_high - ctx->pressure_step) {
            ctx->pressure_low = ctx->pressure_high - ctx->pressure_step;
            if (ctx->pressure_low < PRESSURE_LOW_MIN) {
                ctx->pressure_low = PRESSURE_LOW_MIN;
            }
        }
        AppUI_Display_SET_HP_Pressure();
    } else if (ctx->current_state == UI_STATE_SET_LP_PRESSURE) {
        // 间歇模式低压：调整pressure_low
        if (ctx->pressure_low > ctx->pressure_step) {
            ctx->pressure_low -= ctx->pressure_step;  // 使用变量步长
        } else {
            ctx->pressure_low = PRESSURE_LOW_MAX;
        }
        // 确保不超过高压值
        if (ctx->pressure_low >= ctx->pressure_high - ctx->pressure_step) {
            ctx->pressure_low = ctx->pressure_high - ctx->pressure_step;
        }
        if (ctx->pressure_low < PRESSURE_LOW_MIN) {
            ctx->pressure_low = PRESSURE_LOW_MIN;
        }
        AppUI_Display_SET_LP_Pressure();
    }
}

/**
 * @name      AppUI_AdjustTimeUp
 * @brief     增加时间值
 */
static void AppUI_AdjustTimeUp(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    if (ctx->time_edit_high) {
        ctx->time_high += TIME_STEP;
        if (ctx->time_high > TIME_HIGH_MAX) {
            ctx->time_high = TIME_HIGH_MIN;
        }
    } else {
        ctx->time_low += TIME_STEP;
        if (ctx->time_low > TIME_LOW_MAX) {
            ctx->time_low = TIME_LOW_MIN;
        }
    }
    AppUI_Display_SET_Time();
}

/**
 * @name      AppUI_AdjustTimeDown
 * @brief     减少时间值
 */
static void AppUI_AdjustTimeDown(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    if (ctx->time_edit_high) {
        if (ctx->time_high > TIME_STEP) {
            ctx->time_high -= TIME_STEP;
        } else {
            ctx->time_high = TIME_HIGH_MAX;
        }
    } else {
        if (ctx->time_low > TIME_STEP) {
            ctx->time_low -= TIME_STEP;
        } else {
            ctx->time_low = TIME_LOW_MAX;
        }
    }
    AppUI_Display_SET_Time();
}

/**
 * @name      AppUI_SwitchTimeEdit
 * @brief     切换时间编辑项（高压/低压）
 */
static void AppUI_SwitchTimeEdit(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;
    
    ctx->time_edit_high = !ctx->time_edit_high;
    AppUI_Display_SET_Time();
}

/**
 * @name      AppUI_TimeSetting_ExitToPause
 * @brief     时间设置界面退出到暂停模式
 */
static void AppUI_TimeSetting_ExitToPause(void* arg, st_fsm_event event)
{
    UIContext_t* ctx = (UIContext_t*)arg;
    (void)event;

    /* 复位编辑标志，便于下次进入时从高压时间开始 */
    ctx->time_edit_high = true;

    /* 复用暂停界面的状态入口，确保上下文一致 */
    AppUI_StateEntry_ZHT(arg, event);
}

/****************************************************************************
 * 显示函数实现
 ****************************************************************************/

/**
 * @name      AppUI_Display_SYS
 * @brief     显示初始化界面
 */
static void AppUI_Display_SYS(void)
{
#define SW 0
    if (g_ui_context.sys_show_logo) {
        // 显示开机Logo（由display模块处理）
        Display_ShowStartupInterface();
    } else {
        // 显示"npwt"和版本号
        Display_Clear();
        // 16x32字体需要4页（32像素），屏幕总共8页（0-7）
        // Y=2时，page_hw=4，占用硬件页4,5,6,7（完整显示）
        // 6x12字体需要2页（12像素），Y=5时，page_hw=1，占用硬件页1,2（不重叠）
#if SW
        static char text_buffer[32] = {0};
        Display_ShowString(40, 2, AppLanguage_GetTextConverted(TEXT_ID_MODE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MODE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
#else        
        Display_ShowString(46, 3, "NPWT", DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
        Display_ShowString(1, 5, "Vcare1000-300se.1.01", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
#endif
    }
}

/**
 * @name      AppUI_Display_WAT
 * @brief     显示待机界面（主菜单）
 */
static void AppUI_Display_WAT(void)
{

    
    // 显示工作模式
    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
        Display_ShowIcon(0, 0, ICON_CONTINUOUS);  // 连续模式图标
    } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
        Display_ShowIcon(0, 0, ICON_INTERMITTENT); // 间歇模式图标
    }
    
    Display_ShowString(25, 0, "-120mmHg", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    Display_ShowIcon(32, 2, ICON_KEY2);  // 按键图标上半部分（页2，指向Settings）
    // 中文需要转换为字库索引数组
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    static char text_buffer1[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    Display_ShowString(48, 2, AppLanguage_GetTextConverted(TEXT_ID_SETTINGS, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_SETTINGS, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
   
    Display_ShowIcon(35, 4, ICON_KEY1);  // 按键图标上半部分（页2，指向Settings）
    Display_ShowString(48, 4, AppLanguage_GetTextConverted(TEXT_ID_THERAPY, text_buffer1, sizeof(text_buffer1)), AppLanguage_GetFontForText(TEXT_ID_THERAPY, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
}

/**
 * @name      AppUI_Display_LIX
 * @brief     显示连续工作模式界面
 */
static void AppUI_Display_LIX(void)
{
    // 显示工作模式
    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
        Display_ShowIcon(0, 0, ICON_CONTINUOUS);  // 连续模式图标
    } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
        Display_ShowIcon(0, 0, ICON_INTERMITTENT); // 间歇模式图标
    }

    // 显示目标压力
    static char target_str[16] = {0};
    uint16_t current_pressure = AppPressure_GetPressureValue();
    g_last_display_pressure = current_pressure;
    snprintf(target_str, sizeof(target_str), "-%03u", (unsigned int)current_pressure);
    Display_ShowString(16, 4, target_str, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    Display_ShowString(80, 4, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);    
    
    // 显示提示操作
    Display_ShowString(12, 6, AppLanguage_GetTextConverted(TEXT_ID_THERAPY_ON, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_THERAPY_ON, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
    // 更新静音图标显示
    AppUI_UpdateMuteIcon();
}

/**
 * @name      AppUI_Display_JIX
 * @brief     显示间歇工作模式界面
 */
static void AppUI_Display_JIX(void)
{
    // 显示工作模式
    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
        Display_ShowIcon(0, 0, ICON_CONTINUOUS);  // 连续模式图标
    } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
        Display_ShowIcon(0, 0, ICON_INTERMITTENT); // 间歇模式图标
    }

    // 显示目标压力
    static char target_str[16] = {0};
    snprintf(target_str, sizeof(target_str), "-%ummHg", (unsigned int)g_ui_context.pressure_high);
    Display_ShowString(25, 0, target_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);

    // 显示间歇模式高压时间
    static char hp_time_str[8] = {0};
    snprintf(hp_time_str, sizeof(hp_time_str), "%02umin", (unsigned int)g_ui_context.time_high);
    Display_ShowString(92, 2, hp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);

    // 显示间歇模式低压时间
    static char lp_time_str[8] = {0};
    snprintf(lp_time_str, sizeof(lp_time_str), "%02umin", (unsigned int)g_ui_context.time_low);
    Display_ShowString(92, 4, lp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 显示实时压力值
    static char pressure_str[16] = {0};
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    uint16_t current_pressure = AppPressure_GetPressureValue();
    snprintf(pressure_str, sizeof(pressure_str), "-%03u", (unsigned int)current_pressure);
    Display_ShowString(0, 4, pressure_str, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
    Display_ShowString(60, 4, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    g_last_display_pressure = current_pressure;
        
    // 显示当前阶段
    Display_ShowString(12, 6, AppLanguage_GetTextConverted(TEXT_ID_HIGH_PHASE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_HIGH_PHASE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
    // 更新静音图标显示
    AppUI_UpdateMuteIcon();
}

/**
 * @name      AppUI_Display_ZHT
 * @brief     显示暂停模式界面
 */
static void AppUI_Display_ZHT(void)
{
    // 显示模式
    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
        Display_ShowIcon(0, 0, ICON_CONTINUOUS);  // 连续模式图标
    } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
        Display_ShowIcon(0, 0, ICON_INTERMITTENT); // 间歇模式图标
    }
    
    // 显示目标压力
    static char target_str[16]={0};
    snprintf(target_str, sizeof(target_str), "-%ummHg", (unsigned int)g_ui_context.pressure_high);
    Display_ShowString(25, 0, target_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 显示暂停状态
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    Display_ShowString(24, 3, AppLanguage_GetTextConverted(TEXT_ID_THERAPY_OFF, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_THERAPY_OFF, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
    // 显示启动按键的图标
    Display_ShowIcon(16, 6, ICON_KEY1);  // 按键图标上半部分（页2，指向Settings）
    Display_ShowString(30, 6, AppLanguage_GetTextConverted(TEXT_ID_THERAPY, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_THERAPY, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
    // 更新静音图标显示
    AppUI_UpdateMuteIcon();
    
}

/**
 * @name      AppUI_Display_SET
 * @brief     显示设置模式界面
 */
static void AppUI_Display_SET(void)
{
    // 显示设置菜单
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    static char text_buffer1[16] = {0}; 
    static char text_buffer2[16] = {0}; 
    static char text_buffer3[16] = {0}; 
    
    // 标题
    Display_ShowString(2, 0, AppLanguage_GetTextConverted(TEXT_ID_MODE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MODE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    Display_ShowIcon(66, 0, ICON_KEY2); // 三角图标  
    Display_ShowString(78, 0, AppLanguage_GetTextConverted(TEXT_ID_SWITCH, text_buffer1, sizeof(text_buffer1)), AppLanguage_GetFontForText(TEXT_ID_SWITCH, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
    //可选模式
    Display_ShowString(2, 2, AppLanguage_GetTextConverted(TEXT_ID_CONTINUOUS, text_buffer2, sizeof(text_buffer2)), AppLanguage_GetFontForText(TEXT_ID_CONTINUOUS, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    Display_ShowString(1, 4, AppLanguage_GetTextConverted(TEXT_ID_INTERMITTENT, text_buffer3, sizeof(text_buffer3)), AppLanguage_GetFontForText(TEXT_ID_INTERMITTENT, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    
    
    // 根据当前选中的模式显示勾号
    if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
        // 连续模式选中：在第2行显示勾号
        Display_ShowIcon(106, 2, ICON_TICK); 
    } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
        // 间歇模式选中：在第4行显示勾号
        Display_ShowIcon(106, 4, ICON_TICK); 
    }
}

/**
 * @name      AppUI_Display_SET_Pressure
 * @brief     显示连续模式压力设置界面
 */
static void AppUI_Display_SET_Pressure(void)
{
//    static char hp_pressure_str[8] = {0};
//    snprintf(hp_pressure_str, sizeof(hp_pressure_str), "%03ummhg", (unsigned int)g_ui_context.pressure_high);
    
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    Display_ShowString(36, 0, AppLanguage_GetTextConverted(TEXT_ID_PRESSURE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_PRESSURE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);

    Display_ShowPressure(48, 4, g_ui_context.pressure_high, true, DISPLAY_FONT_16X32);  // 显示压力值和单位"mmHg"
//    Display_ShowString(48, 4, hp_pressure_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
}

/**
 * @name      AppUI_RefreshPressureValue
 * @brief     刷新连续模式压力值显示（局部刷新，只刷新压力值区域，不刷新"Pressure"文字）
 * @note      用于压力值调整后的动态刷新，避免全屏刷新导致的闪烁
 */
static void AppUI_RefreshPressureValue(void)
{
    // 清除压力值显示区域（包括压力值和单位）
    // 压力值起始位置：列48，页4
    // 16x32字体最多3位数字：48列宽（16*3）
    // 单位"mmHg"：32列宽（4字符*8），在压力值右侧，起始列约98（48+48+2间距）
    // 单位结束列约130，但电池图标在列102（实际起始119），所以清除到列102之前
    // 清除策略：由于Display_ClearRect单次最多支持16列宽，需要分段清除
    
    // 清除压力值区域（列48-96，高度32像素，4页）：分3次清除，每次16列
    Display_ClearRect(48, 4, 16, 32);  // 第1段：列48-64
    Display_ClearRect(64, 4, 16, 32);  // 第2段：列64-80
    Display_ClearRect(80, 4, 16, 32);  // 第3段：列80-96
    
    // 清除单位区域（列96-102，高度16像素，2页，覆盖单位的前4列）
    // 注意：Display_ClearRect限制最小宽度，这里使用16列清除，覆盖列96-112
    // 虽然单位延伸到130列，但清除到102列足够避免与电池图标重叠时的显示问题
    Display_ClearRect(96, 4, 16, 16);  // 单位区域前半部分，高度16像素（2页）
    
    // 重新显示压力值和单位
    Display_ShowPressure(48, 4, g_ui_context.pressure_high, true, DISPLAY_FONT_16X32);
}

/**
 * @name      AppUI_Display_SET_HP_Pressure
 * @brief     显示间歇模式高压设置界面
 */
static void AppUI_Display_SET_HP_Pressure(void)
{
    // 参考未重构工程：显示"Pressure"、"HP Set"和"LP Set"
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    Display_ShowString(36, 0, AppLanguage_GetTextConverted(TEXT_ID_PRESSURE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_PRESSURE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
//    Display_ShowString(14, 2, "HP Set:-", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
//    Display_ShowString(14, 4, "LP Set:-", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    static char hp_pressure_str[8] = {0};
    static char lp_pressure_str[8] = {0};
    
   
    // 显示高压（反转显示，表示当前正在编辑）
    snprintf(hp_pressure_str, sizeof(hp_pressure_str), "%03u", (unsigned int)g_ui_context.pressure_high);
    Display_ShowStringInvert(73, 2, hp_pressure_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowString(93, 2, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    
    // 显示低压（正常显示）
    snprintf(lp_pressure_str, sizeof(lp_pressure_str), "%03u", (unsigned int)g_ui_context.pressure_low);
    Display_ShowString(73, 4, lp_pressure_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowString(93, 4, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    
    Display_ShowString(14, 2, AppLanguage_GetTextConverted(TEXT_ID_HP_SET, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_HP_SET, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    Display_ShowString(14, 4, AppLanguage_GetTextConverted(TEXT_ID_LP_SET, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_LP_SET, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
}

/**
 * @name      AppUI_Display_SET_LP_Pressure
 * @brief     显示间歇模式低压设置界面
 */
static void AppUI_Display_SET_LP_Pressure(void)
{
    // 参考未重构工程：显示"Pressure"、"HP Set"和"LP Set"
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    Display_ShowString(36, 0, AppLanguage_GetTextConverted(TEXT_ID_PRESSURE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_PRESSURE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
//    Display_ShowString(14, 2, "HP Set:-", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
//    Display_ShowString(14, 4, "LP Set:-", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    static char hp_pressure_str[8] = {0};
    static char lp_pressure_str[8] = {0};
    
    
//    // 在显示反转内容之前，重新显示标签，确保负号完整（避免清除操作影响相邻区域）
//    Display_ShowString(6, 2, "HP Set:-", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
//    Display_ShowString(6, 4, "LP Set:-", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    // 显示高压（正常显示）
    snprintf(hp_pressure_str, sizeof(hp_pressure_str), "%03u", (unsigned int)g_ui_context.pressure_high);
    Display_ShowString(73, 2, hp_pressure_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowString(93, 2, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    
    // 显示低压（反转显示，表示当前正在编辑）
    snprintf(lp_pressure_str, sizeof(lp_pressure_str), "%03u", (unsigned int)g_ui_context.pressure_low);
    Display_ShowStringInvert(73, 4, lp_pressure_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    Display_ShowString(93, 4, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    
    Display_ShowString(14, 2, AppLanguage_GetTextConverted(TEXT_ID_HP_SET, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_HP_SET, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    Display_ShowString(14, 4, AppLanguage_GetTextConverted(TEXT_ID_LP_SET, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_LP_SET, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
}

/**
 * @name      AppUI_Display_SET_Time
 * @brief     显示间歇模式时间设置界面
 */
static void AppUI_Display_SET_Time(void)
{
    // 显示"Intermittent"、"HP Time"和"LP Time"
    static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
    Display_ShowString(16, 0, AppLanguage_GetTextConverted(TEXT_ID_INTERMITTENT, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_INTERMITTENT, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
    Display_ShowString(14, 2, AppLanguage_GetTextConverted(TEXT_ID_HP_TIME, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_HP_TIME, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    Display_ShowString(14, 4, AppLanguage_GetTextConverted(TEXT_ID_LP_TIME, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_LP_TIME, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT);
    
    static char hp_time_str[8] = {0};
    static char lp_time_str[8] = {0};
    
    // 根据time_edit_high标志决定哪个时间值反转显示
    if (g_ui_context.time_edit_high) {
        // 编辑高压时间：高压时间反转显示，低压时间正常显示
        snprintf(hp_time_str, sizeof(hp_time_str), "%02u", (unsigned int)g_ui_context.time_high);
        Display_ShowStringInvert(75, 2, hp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
        static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
        Display_ShowString(90, 2, AppLanguage_GetTextConverted(TEXT_ID_MIN, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MIN, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT); 
        
        snprintf(lp_time_str, sizeof(lp_time_str), "%02u", (unsigned int)g_ui_context.time_low);
        Display_ShowString(75, 4, lp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
        Display_ShowString(90, 4, AppLanguage_GetTextConverted(TEXT_ID_MIN, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MIN, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT); 
    } else {
        // 编辑低压时间：高压时间正常显示，低压时间反转显示
        snprintf(hp_time_str, sizeof(hp_time_str), "%02u", (unsigned int)g_ui_context.time_high);
        Display_ShowString(75, 2, hp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
        static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
        Display_ShowString(90, 2, AppLanguage_GetTextConverted(TEXT_ID_MIN, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MIN, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT); 
        
        snprintf(lp_time_str, sizeof(lp_time_str), "%02u", (unsigned int)g_ui_context.time_low);
        Display_ShowStringInvert(75, 4, lp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
        Display_ShowString(90, 4, AppLanguage_GetTextConverted(TEXT_ID_MIN, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MIN, DISPLAY_FONT_6X12), DISPLAY_ALIGN_LEFT); 
    }
}

/****************************************************************************
 * 公共接口实现
 ****************************************************************************/

/**
 * @name      AppUI_InitSwitchCallback
 * @brief     初始化切换定时器回调函数（4秒后从Logo切换到字符串）
 * @param     user_data - 用户数据（未使用）
 * @retval    无
 */
static void AppUI_InitSwitchCallback(void* user_data)
{
    (void)user_data;
    
    /* 切换到显示字符串 */
    g_ui_context.sys_show_logo = false;
    
    /* 重新显示开机界面（此时会显示字符串） */
    AppUI_Display_SYS();
}

/**
 * @name      AppUI_InitTimeoutCallback
 * @brief     初始化超时定时器回调函数（7秒后触发超时事件）
 * @param     user_data - 用户数据（未使用）
 * @retval    无
 */
static void AppUI_InitTimeoutCallback(void* user_data)
{
    (void)user_data;
    
    /* 在状态转换前清空显示队列，确保新状态的显示事件能全部入队 */
    Display_ClearQueue();
    
    /* 创建超时事件并放入FSM队列 */
    st_fsm_event fsm_event = {0};
    fsm_event.event_type = (uint8_t)UI_EVENT_TIMEOUT;
    fsm_event.event_user = 0;
    
    /* 将事件加入FSM队列 */
    g_ui_fsm.event_trigger_queue->put(g_ui_fsm.event_trigger_queue, &fsm_event, sizeof(fsm_event));
}

/**
 * @name      AppUI_Init
 * @brief     初始化UI模块
 */
void AppUI_Init(void)
{
    /* 初始化UI上下文 */
    g_ui_context.current_state = UI_STATE_SYS;
    g_ui_context.last_state = UI_STATE_SYS;
    g_ui_context.work_mode_backup = UI_STATE_LIX;
    g_ui_context.lock_flag = false;
    g_ui_context.auto_lock_active = false;
    g_ui_context.lock_icon_visible = false;
    g_ui_context.lock_inactive_ticks = 0;
    g_ui_context.idle_active = false;
    g_ui_context.idle_inactive_ticks = 0;
    g_ui_context.idle_previous_state = UI_STATE_SYS;
    g_ui_context.leak_alarm_active = false;
    g_ui_context.leak_alarm_previous_state = UI_STATE_SYS;
    g_ui_context.blockage_alarm_active = false;
    g_ui_context.blockage_alarm_previous_state = UI_STATE_SYS;
    g_ui_context.overpressure_alarm_active = false;
    g_ui_context.overpressure_alarm_previous_state = UI_STATE_SYS;
    // settings_sub_state已废弃，现在使用独立的FSM状态
    g_ui_context.pressure_high = PRESSURE_SET_DEFAULT;
    g_ui_context.pressure_low = PRESSURE_LOW_DEFAULT;
    g_ui_context.time_high = TIME_HIGH_DEFAULT;
    g_ui_context.time_low = TIME_LOW_DEFAULT;
    g_ui_context.time_edit_high = true;  // 默认编辑高压时间
    g_ui_context.pressure_step = 5;      // 压力调整步进值，默认5mmHg
    g_ui_context.sys_show_logo = true;   // 初始显示Logo
    g_ui_context.user_data = NULL;
    
    /* 获取按键事件队列指针 */
    g_key_event_queue = AppButton_GetKeyEventQueue();
    
    /* 创建FSM实例 */
    g_ui_fsm.fsm_id = FSM_ID_0;
    g_ui_fsm.trans_table = g_ui_transition_table;
    g_ui_fsm.trans_size = (uint8_t)(sizeof(g_ui_transition_table) / sizeof(g_ui_transition_table[0]));
    g_ui_fsm.current_state = UI_STATE_SYS;
    g_ui_fsm.last_state = UI_STATE_SYS;
    g_ui_fsm.user_arg = &g_ui_context;
    
    /* 创建并初始化FSM */
    if (fsm_create(&g_ui_fsm) == 0) {
        g_ui_fsm.configure(&g_ui_fsm);
        g_ui_fsm.initialize(&g_ui_fsm);
    }
    
    /* 创建初始化切换定时器（4秒，单次模式，从Logo切换到字符串） */
    /* 注意：SoftTimer_Create的参数单位是毫秒，所以4秒应该填写4000 */
    g_init_switch_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE, 
                                           4000,  // 4秒 = 4000毫秒
                                           AppUI_InitSwitchCallback, 
                                           NULL);
    
    /* 创建初始化超时定时器（7秒，单次模式，进入待机模式） */
    /* 注意：SoftTimer_Create的参数单位是毫秒，所以7秒应该填写7000 */
    g_init_timeout_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE, 
                                            7000,  // 7秒 = 7000毫秒
                                            AppUI_InitTimeoutCallback, 
                                            NULL);
    
    if (g_init_switch_timer != 0) {
        /* 启动切换定时器 */
        SoftTimer_Start(g_init_switch_timer);
    }
    
    if (g_init_timeout_timer != 0) {
        /* 启动超时定时器 */
        SoftTimer_Start(g_init_timeout_timer);
    }
    
    /* 进入初始化状态 */
    AppUI_StateEntry_SYS(&g_ui_context, (st_fsm_event){0});
}

/**
 * @name      AppUI_Process
 * @brief     处理UI状态机（每10ms调用一次）
 * 
 * 功能说明：
 * - 处理按键事件队列，转换为FSM事件
 * - 处理FSM事件队列，执行状态转换（状态转换时的显示由状态入口函数处理）
 * - 检测SET状态下work_mode_backup的变化，使用局部清除优化更新勾号位置
 */
void AppUI_Process(void)
{
    /* 检查是否有严重低电报警，如果有则暂停UI显示 */
    if (AppAlarm_IsCriticalBatteryActive())
    {
        /* 严重低电报警时，暂停所有UI处理，只保持报警图标显示 */
        return;
    }
    
    /* 第一步：从按键事件队列读取并转换为FSM事件 */
    if (g_key_event_queue != NULL) {
        KeyEvent_t key_event;
        /* 处理队列中的所有按键事件（批量处理） */
        while (g_key_event_queue->empty(g_key_event_queue) == false) {
            if (g_key_event_queue->get(g_key_event_queue, &key_event, sizeof(key_event))) {
                /* 如果处于空闲状态，检测长按任意键松开事件以退出空闲 */
                if (g_ui_context.idle_active) {
                    if (key_event.key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
                        AppUI_ExitIdle();
                        continue;  // 退出空闲后，不处理该按键事件
                    } else {
                        /* 空闲状态下忽略其他按键事件 */
                        continue;
                    }
                }
                
                /* 如果处于泄漏报警状态，检测长按电源键（key_id=0）松开事件以退出泄漏报警 */
                if (g_ui_context.leak_alarm_active) {
                    if (key_event.key_id == 0 &&  // 电源键/确认键的key_id是0
                        key_event.key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
                        AppUI_ExitLeakAlarm();
                        continue;  // 退出泄漏报警后，不处理该按键事件
                    } else {
                        /* 泄漏报警状态下忽略其他按键事件 */
                        continue;
                    }
                }
                
                /* 如果处于过压报警状态，检测长按电源键（key_id=0）松开事件以退出过压报警 */
                if (g_ui_context.overpressure_alarm_active) {
                    if (key_event.key_id == 0 &&  // 电源键/确认键的key_id是0
                        key_event.key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
                        AppUI_ExitOverpressureAlarm();
                        /* 退出到暂停界面 */
                        AppUI_StateEntry_ZHT(&g_ui_context, (st_fsm_event){0});
                        g_ui_fsm.current_state = UI_STATE_ZHT;  // 手动同步FSM状态
                        continue;  // 退出过压报警后，不处理该按键事件
                    } else {
                        /* 过压报警状态下忽略其他按键事件 */
                        continue;
                    }
                }
                
                /* 如果处于管路堵塞报警状态，检测长按电源键（key_id=0）松开事件以退出管路堵塞报警 */
                if (g_ui_context.blockage_alarm_active) {
                    if (key_event.key_id == 0 &&  // 电源键/确认键的key_id是0
                        key_event.key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
                        AppUI_ExitBlockageAlarm();
                        /* 退出到暂停界面 */
                        AppUI_StateEntry_ZHT(&g_ui_context, (st_fsm_event){0});
                        g_ui_fsm.current_state = UI_STATE_ZHT;  // 手动同步FSM状态
                        continue;  // 退出管路堵塞报警后，不处理该按键事件
                    } else {
                        /* 管路堵塞报警状态下忽略其他按键事件 */
                        continue;
                    }
                }
                
                /* 在暂停模式下，检测长按静音键以切换静音状态 */
                if (g_ui_context.current_state == UI_STATE_ZHT &&
                    key_event.key_id == 3 &&  // 静音键的key_id是3
                    key_event.key_event == KEY_MACHINE_EVENT_LONG_PRESS_RELEASE) {
                    /* 切换静音状态 */
                    AppBeep_SetMute(!AppBeep_IsMuted());
                    /* 更新静音图标显示 */
                    AppUI_UpdateMuteIcon();
                    continue;  // 不处理其他逻辑
                }
                
                /* 记录用户交互：仅在未锁定时重置计时 */
                if (g_ui_context.auto_lock_active == false) {
                    AppUI_ResetLockTimer();
                }
                
                /* 在可空闲状态下，任何按键事件都重置空闲计时器 */
                if (AppUI_IsIdleableState(g_ui_context.current_state)) {
                    AppUI_ResetIdleTimer();
                }

                /* 转换按键事件为UI事件 */
                UIEvent_e ui_event = AppUI_ConvertKeyEvent(key_event.key_id, key_event.key_event);
                
                if (ui_event == UI_EVENT_UNLOCK) {
                    AppUI_ExitAutoLock();
                    continue;
                }

                if (g_ui_context.auto_lock_active) {
                    /* 锁定状态下忽略其他事件 */
                    continue;
                }
                
                if (ui_event != UI_EVENT_NONE) {
                    /* 创建FSM事件并放入队列 */
                    st_fsm_event fsm_event = {0};
                    fsm_event.event_type = (uint8_t)ui_event;
                    fsm_event.event_user = key_event.key_id;
                    
                    /* 将事件加入FSM队列 */
                    g_ui_fsm.event_trigger_queue->put(g_ui_fsm.event_trigger_queue, &fsm_event, sizeof(fsm_event));
                }
            }
        }
    }
    
    /* 第二步：从FSM事件队列获取事件并处理 */
    st_fsm_event event;
    
    /* 检查FSM事件队列 */
    if (g_ui_fsm.event_trigger_queue->empty(g_ui_fsm.event_trigger_queue) == false) {
        if (g_ui_fsm.event_trigger_queue->get(g_ui_fsm.event_trigger_queue, &event, sizeof(event))) {
            /* 处理事件（如果未锁定） */
            if (g_ui_context.lock_flag == false) {
                g_ui_fsm.poll(&g_ui_fsm, event);
            }
        }
    }
    
    /* 过压报警检测（最高优先级，安全相关） */
    AppUI_UpdateOverpressureAlarm();
    
    /* 泄漏报警检测（优先级仅次于过压报警） */
    if (g_ui_context.overpressure_alarm_active == false) {
        AppUI_UpdateLeakAlarm();
    }
    
    /* 管路堵塞报警检测（优先级低于过压和泄漏报警，高于空闲检测） */
    if (g_ui_context.overpressure_alarm_active == false && g_ui_context.leak_alarm_active == false) {
        AppUI_UpdateBlockageAlarm();
    }
    
    /* 空闲检测（优先级高于锁屏，但低于所有报警） */
    if (g_ui_context.overpressure_alarm_active == false && 
        g_ui_context.leak_alarm_active == false && 
        g_ui_context.blockage_alarm_active == false) {
        AppUI_UpdateIdle();
    }
    
    /* 自动锁定检测（在处理完本轮事件后执行，如果处于任何报警或空闲状态则暂停） */
    if (g_ui_context.idle_active == false && 
        g_ui_context.overpressure_alarm_active == false &&
        g_ui_context.leak_alarm_active == false && 
        g_ui_context.blockage_alarm_active == false) {
        AppUI_UpdateAutoLock();
    }
    /* 第三步：检测SET状态下work_mode_backup的变化（用于更新勾号位置） */
    static UIState_e last_work_mode_backup = UI_STATE_COUNT;
    
    if (g_ui_context.current_state == UI_STATE_SET &&
        g_ui_context.work_mode_backup != last_work_mode_backup) {
        /* SET状态下，work_mode_backup变化时使用局部清除优化 */
        if (g_ui_context.work_mode_backup == UI_STATE_LIX) {
            Display_ClearRect(106, 4, 16, 16);
            Display_ShowIcon(106, 2, ICON_TICK);
        } else if (g_ui_context.work_mode_backup == UI_STATE_JIX) {
            Display_ShowIcon(106, 4, ICON_TICK);
            Display_ClearRect(106, 2, 16, 16);
        }
        last_work_mode_backup = g_ui_context.work_mode_backup;
    } else if (g_ui_context.current_state != UI_STATE_SET) {
        // 非SET状态，重置静态变量
        last_work_mode_backup = UI_STATE_COUNT;
    }
    
    /* 第四步：电池显示更新（参考未重构工程的实现） */
    /* 如果处于任何报警或空闲状态，跳过电池显示更新 */
    if (g_ui_context.idle_active || 
        g_ui_context.overpressure_alarm_active ||
        g_ui_context.leak_alarm_active || 
        g_ui_context.blockage_alarm_active) {
        return;  // 报警或空闲状态下，只显示相应报警信息，不更新其他UI元素
    }
    
    /* 开机界面不显示电池（显示Logo和版本号期间） */
    if (g_ui_context.current_state == UI_STATE_SYS) {
        return;  // 开机界面不显示电池
    }
    
    /* 设置界面不显示电池（模式选择、压力设置、时间设置等） */
    if (g_ui_context.current_state == UI_STATE_SET ||
        g_ui_context.current_state == UI_STATE_SET_PRESSURE ||
        g_ui_context.current_state == UI_STATE_SET_HP_PRESSURE ||
        g_ui_context.current_state == UI_STATE_SET_LP_PRESSURE ||
        g_ui_context.current_state == UI_STATE_SET_TIME) {
        return;  // 设置界面不显示电池
    }
    
    static uint16_t battery_display_counter = 0;      // 显示更新计数器（用于降低刷新频率）
    static uint16_t battery_charge_blink_counter = 0; // 充电闪烁计数器（用于充电动画）
    static uint16_t battery_warning_blink_counter = 0; // 低电警告闪烁计数器（用于低电警告闪烁，单位：10ms）
    static bool battery_warning_blink_state = false;  // 低电警告闪烁状态：true=显示，false=隐藏
    static uint8_t last_battery_level = 100;
    static bool last_battery_charging = false;
    
    /* 低电警告闪烁周期：100次×10ms = 1秒 */
    #define BATTERY_WARNING_BLINK_INTERVAL_TICKS  100U
    
    /* 获取当前电池信息 */
    bool battery_changed = AppBattery_IsLevelChanged();
    uint8_t current_battery_level = AppBattery_GetLevel();
    bool current_battery_charging = AppBattery_IsCharging();
    BatteryLevel_e current_battery_level_enum = AppBattery_GetLevelEnum();  // 获取电池等级枚举值，用于判断是否为低电警告
    /* 若处于泄气阶段，冻结电池图标显示，避免瞬时压降导致的误判闪烁/降格 */
    bool freeze_battery_display = AppPressure_IsBleeding();
    
    /* 充电闪烁计数器递增（每10ms递增一次） */
    if (!freeze_battery_display && current_battery_charging) {
        battery_charge_blink_counter++;
        if (battery_charge_blink_counter >= 125) {  // 125次 = 1.25秒，循环5个图标
            battery_charge_blink_counter = 0;
        }
    } else {
        battery_charge_blink_counter = 0;  // 非充电时重置
    }
    
    /* 低电警告闪烁计数器处理（电池等级为WARNING且未充电时） */
    bool is_warning_level = (current_battery_level_enum == BATTERY_LEVEL_WARNING);
    bool warning_blink_state_changed = false;  // 闪烁状态是否发生变化
    
    if (!freeze_battery_display && is_warning_level && !current_battery_charging) {
        /* 低电警告且未充电时，实现1秒周期闪烁 */
        battery_warning_blink_counter++;
        if (battery_warning_blink_counter >= BATTERY_WARNING_BLINK_INTERVAL_TICKS) {
            battery_warning_blink_counter = 0;
            bool old_state = battery_warning_blink_state;
            battery_warning_blink_state = !battery_warning_blink_state;  // 切换显示/隐藏状态
            warning_blink_state_changed = (old_state != battery_warning_blink_state);  // 状态发生变化（应该总是true）
        }
    } else {
        /* 非警告状态或正在充电时，重置闪烁状态 */
        bool old_state = battery_warning_blink_state;
        battery_warning_blink_counter = 0;
        battery_warning_blink_state = false;
        warning_blink_state_changed = (old_state != battery_warning_blink_state);  // 如果从闪烁状态恢复，需要更新显示
    }
    
    /* 判断是否需要更新显示 */
    bool need_update = false;
    
    if (battery_changed || 
        current_battery_level != last_battery_level || 
        current_battery_charging != last_battery_charging) {
        /* 电池信息变化，立即更新显示 */
        need_update = true;
        if (!freeze_battery_display) {
            last_battery_level = current_battery_level;
            last_battery_charging = current_battery_charging;
        }
        battery_display_counter = 0;  // 重置计数器
    } else if (!freeze_battery_display && current_battery_charging) {
        /* 充电时，每20ms更新一次（实现闪烁动画，与未重构工程一致） */
        if (++battery_display_counter >= 2) {  // 2次 = 20ms@10ms
            need_update = true;
            battery_display_counter = 0;
        }
    } else {
        /* 非充电时，每1秒更新一次（降低刷新频率） */
        if (++battery_display_counter >= 100) {  // 100次 = 1秒@10ms
            need_update = true;
            battery_display_counter = 0;
        }
    }
    
    /* 更新显示 */
    if (need_update) {
        uint8_t display_level = current_battery_level;
        if (freeze_battery_display) {
            /* 冻结显示为上一次稳定等级，避免泄气负载导致的短暂降格 */
            display_level = last_battery_level;
        }
        
        /* 充电时实现闪烁效果（参考未重构工程：循环显示不同电量图标） */
        if (!freeze_battery_display && current_battery_charging) {
            /* 根据闪烁计数器选择显示的图标（每25次切换一个图标） */
            uint8_t blink_phase = (uint8_t)(battery_charge_blink_counter / 25);  // 0-4
            switch (blink_phase) {
                case 0: display_level = 0; break;    // BAT0 (0%)
                case 1: display_level = 25; break;  // BAT1 (25%)
                case 2: display_level = 50; break;  // BAT2 (50%)
                case 3: display_level = 75; break;  // BAT3 (75%)
                case 4: display_level = 100; break; // BAT4 (100%)
                default: display_level = current_battery_level; break;
            }
        }
        
        /* 低电警告闪烁处理（电池等级为WARNING且未充电时） */
        if (!freeze_battery_display && is_warning_level && !current_battery_charging) {
            /* 根据闪烁状态决定是否显示图标 */
            if (battery_warning_blink_state) {
                /* 显示状态：显示电池图标 */
                /* 电池空格闪烁：关闭白色背光，点亮黄色背光，与图标同步 */
                HAL_LCD_Backlight_Off();
                HAL_LED_Yellow_On();
                Display_ShowBatteryIcon(BATTERY_ICON_X, BATTERY_ICON_Y, display_level, current_battery_charging);
            } else {
                /* 隐藏状态：按与图标绘制相同的路径清除图标区域，避免偏移不一致 */
                /* 电池空格闪烁：关闭白色背光，熄灭黄色背光，与图标同步 */
                HAL_LCD_Backlight_Off();
                HAL_LED_Yellow_Off();
                Display_ClearIconArea(BATTERY_ICON_X, BATTERY_ICON_Y, ICON_BAT0);
            }
        } else {
            /* 非警告状态或正在充电，正常显示电池图标（参考未重构工程：DISP_Bat000(6, 102)，即页6，列102） */
            /* Display_ShowBatteryIcon参数：x=列坐标，y=页坐标 */
            Display_ShowBatteryIcon(BATTERY_ICON_X, BATTERY_ICON_Y, display_level, current_battery_charging);
    }
    } else if (!freeze_battery_display && warning_blink_state_changed) {
        /* 即使need_update为false，低电警告闪烁状态改变时也需要更新显示 */
        if (is_warning_level && !current_battery_charging) {
            if (battery_warning_blink_state) {
                /* 显示状态：显示电池图标 */
                HAL_LCD_Backlight_Off();
                HAL_LED_Yellow_On();
                Display_ShowBatteryIcon(BATTERY_ICON_X, BATTERY_ICON_Y, current_battery_level, current_battery_charging);
            } else {
                /* 隐藏状态：按与图标绘制相同的路径清除图标区域，避免偏移不一致 */
                HAL_LCD_Backlight_Off();
                HAL_LED_Yellow_Off();
                Display_ClearIconArea(BATTERY_ICON_X, BATTERY_ICON_Y, ICON_BAT0);
            }
        } else {
            /* 从闪烁状态恢复到正常状态，显示正常图标 */
            Display_ShowBatteryIcon(BATTERY_ICON_X, BATTERY_ICON_Y, current_battery_level, current_battery_charging);
        }
    }

    /* 治疗界面的实时压力刷新（连续/间歇共用） */
    /* 如果处于任何报警或空闲状态，跳过实时压力刷新 */
    if (g_ui_context.idle_active || 
        g_ui_context.overpressure_alarm_active ||
        g_ui_context.leak_alarm_active || 
        g_ui_context.blockage_alarm_active) {
        return;  // 报警或空闲状态下，只显示相应报警信息，不更新其他UI元素
    }
    
    bool in_continuous = (g_ui_context.current_state == UI_STATE_LIX);
    bool in_intermittent = (g_ui_context.current_state == UI_STATE_JIX);

    if (in_continuous || in_intermittent) {
        if (++g_pressure_refresh_tick >= UI_PRESSURE_REFRESH_INTERVAL_TICKS) {
            g_pressure_refresh_tick = 0;

            uint16_t display_target = in_continuous
                                       ? g_ui_context.pressure_high
                                       : AppPressure_GetCurrentTarget();
            
            // 显示目标气压值
            static char target_buf[16] = {0};
            snprintf(target_buf, sizeof(target_buf), "-%ummHg", (unsigned int)display_target);
            Display_ShowString(25, 0, target_buf, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);

            uint16_t current_pressure = AppPressure_GetPressureValue();

            bool need_refresh = false;
            if (g_last_display_pressure == 0xFFFF) {
                need_refresh = true;
            } else {
                uint16_t diff = (current_pressure > g_last_display_pressure) ?
                                 (current_pressure - g_last_display_pressure) :
                                 (g_last_display_pressure - current_pressure);
                if (diff >= UI_PRESSURE_REFRESH_THRESHOLD_MMHG) {
                    need_refresh = true;
                }
            }

            if (need_refresh) {
                
                static char pressure_buf[16] = {0};
                g_last_display_pressure = current_pressure;
                
                static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
                if(in_continuous) {                
                    snprintf(pressure_buf, sizeof(pressure_buf), "-%03u", (unsigned int)current_pressure);
                    Display_ShowString(16, 4, pressure_buf, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
                    Display_ShowString(80, 4, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
                    
                }else if (in_intermittent){        
                    snprintf(pressure_buf, sizeof(pressure_buf), "-%03u", (unsigned int)current_pressure);
                    Display_ShowString(0, 4, pressure_buf, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
                    Display_ShowString(60, 4, AppLanguage_GetTextConverted(TEXT_ID_MMHG, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MMHG, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
                }
            }

            if (in_intermittent) {
                // 显示间歇模式高压时间
                static char hp_time_str[8] = {0};
                snprintf(hp_time_str, sizeof(hp_time_str), "%02umin", (unsigned int)g_ui_context.time_high);
                Display_ShowString(92, 2, hp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);

                // 显示间歇模式低压时间
                static char lp_time_str[8] = {0};
                snprintf(lp_time_str, sizeof(lp_time_str), "%02umin", (unsigned int)g_ui_context.time_low);
                Display_ShowString(92, 4, lp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
                 
                // 显示当前模式
                AppPressureControlMode_e mode = AppPressure_GetCurrentMode();
                const char* phase_str = (mode == APP_PRESSURE_CONTROL_MODE_INTERMITTENT_LOW) ? "Low Phase" : "High Phase";
                Display_ShowString(12, 6, phase_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
                
            } else {
                static char text_buffer[16] = {0};  // 必须使用static，因为Display_ShowString会入队保存指针
                Display_ShowString(12, 6, AppLanguage_GetTextConverted(TEXT_ID_THERAPY_ON, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_THERAPY_ON, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
            }
        }
    } else {
        g_pressure_refresh_tick = 0;
        g_last_display_pressure = 0xFFFF;
    }
}

/* 注意：AppUI_OnKeyEvent函数已移除，改为使用队列机制
 * 按键事件现在通过app_button模块的队列传递到app_ui模块
 * 在AppUI_Process中统一从队列读取并处理
 */

/**
 * @name      AppUI_GetCurrentState
 * @brief     获取当前UI状态
 */
UIState_e AppUI_GetCurrentState(void)
{
    return g_ui_context.current_state;
}

/**
 * @name      AppUI_SetLockFlag
 * @brief     设置锁定标志
 */
void AppUI_SetLockFlag(bool locked)
{
    g_ui_context.lock_flag = locked;
}

/**
 * @name      AppUI_GetLockFlag
 * @brief     获取锁定标志
 */
bool AppUI_GetLockFlag(void)
{
    return g_ui_context.lock_flag;
}

/**
 * @name      AppUI_GetContext
 * @brief     获取UI上下文指针（用于参数保存等操作）
 */
UIContext_t* AppUI_GetContext(void)
{
    return &g_ui_context;
}
