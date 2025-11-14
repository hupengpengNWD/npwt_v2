/**
 * @file    app_ui.c
 * @brief   应用层UI模块实现
 * @date    2025-01-27
 * 
 * 应用层UI模块，使用FSM组件实现按键触发UI界面切换功能
 */

#include "../Inc/app_ui.h"

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
#define UI_LOCK_ICON_X                       102U  // 锁定图标显示坐标X
#define UI_LOCK_ICON_Y                       6U    // 锁定图标显示坐标Y

#include "../Inc/app_button.h"    // 获取KeyEvent_t和队列接口
#include "../Inc/app_battery.h"   // 电池管理模块
#include "../Inc/app_pressure.h"  // 压力管理模块
#include "../Inc/app_alarm.h"     // 报警管理模块
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
static SoftTimerHandle_t g_init_timeout_timer = 0;

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
    return (state == UI_STATE_LIX) || (state == UI_STATE_JIX);
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

/* 初始化超时定时器回调函数 */
static void AppUI_InitTimeoutCallback(void* user_data);

/****************************************************************************
 * FSM状态转换表（必须在函数声明之后定义）
 ****************************************************************************/

const st_fsm_transition g_ui_transition_table[32] = {
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
        .current_state = UI_STATE_LIX,                       /* 当前状态：连续工作模式 */
        .trigger_event = UI_EVENT_SETTINGS,                  /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_SET,               /* 动作函数：进入设置模式 */
        .next_state    = UI_STATE_SET                        /* 下一状态：设置模式 */
    },
    
    [5] = {
        .current_state = UI_STATE_JIX,                       /* 当前状态：间歇工作模式 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,              /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_ZHT,               /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                        /* 下一状态：暂停模式 */
    },           
                 
    [6] = {          
        .current_state = UI_STATE_JIX,                       /* 当前状态：间歇工作模式 */
        .trigger_event = UI_EVENT_SETTINGS,                  /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_SET,               /* 动作函数：进入设置模式 */
        .next_state    = UI_STATE_SET                        /* 下一状态：设置模式 */
    },           
                 
    [8] = {          
        .current_state = UI_STATE_ZHT,                       /* 当前状态：暂停模式 */
        .trigger_event = UI_EVENT_SETTINGS,                  /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_SET,               /* 动作函数：进入设置模式 */
        .next_state    = UI_STATE_SET                        /* 下一状态：设置模式 */
    },           
                 
    [9] = {          
        .current_state = UI_STATE_ZHT,                      /* 当前状态：暂停模式 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_ZHT_ToTherapy,    /* 动作函数：根据work_mode_backup进入治疗模式 */
        .next_state    = UI_STATE_LIX                       /* 下一状态：默认连续模式，实际由动作函数决定 */
    },
    
    [10] = {
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_SETTINGS,                 /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_ZHT,              /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },          

    [11] = {            
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_SwitchWorkMode,              /* 动作函数：切换工作模式 */
        .next_state    = UI_STATE_SET                       /* 下一状态：保持设置模式 */
    },          

    [12] = {            
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_SwitchWorkMode,              /* 动作函数：切换工作模式 */
        .next_state    = UI_STATE_SET                       /* 下一状态：保持设置模式 */
    },          
    
    [13] = {
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放（连续模式） */
        .action_func   = AppUI_StateEntry_SET_Pressure,     /* 动作函数：进入连续模式压力设置 */
        .next_state    = UI_STATE_SET_PRESSURE              /* 下一状态：连续模式压力设置界面 */
    },
    
    [14] = {
        .current_state = UI_STATE_SET,                      /* 当前状态：设置模式 */
        .trigger_event = UI_EVENT_CONFIRM,                  /* 触发事件：确认键长按释放（间歇模式映射为普通确认） */
        .action_func   = AppUI_StateEntry_SET_HP_Pressure,  /* 动作函数：进入间歇模式高压设置 */
        .next_state    = UI_STATE_SET_HP_PRESSURE           /* 下一状态：间歇模式高压设置界面 */
    },
    
    [15] = {
        .current_state = UI_STATE_SET_PRESSURE,             /* 当前状态：连续模式压力设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustPressureUp,            /* 动作函数：增加压力值 */
        .next_state    = UI_STATE_SET_PRESSURE              /* 下一状态：保持当前状态 */
    },
    
    [16] = {
        .current_state = UI_STATE_SET_PRESSURE,             /* 当前状态：连续模式压力设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustPressureDown,          /* 动作函数：减少压力值 */
        .next_state    = UI_STATE_SET_PRESSURE              /* 下一状态：保持当前状态 */
    },
    
    [17] = {
        .current_state = UI_STATE_SET_PRESSURE,             /* 当前状态：连续模式压力设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_ZHT,              /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },
    
    [18] = {
        .current_state = UI_STATE_SET_PRESSURE,             /* 当前状态：连续模式压力设置界面 */
        .trigger_event = UI_EVENT_SETTINGS,                 /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_ZHT,              /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },
    
    [19] = {
        .current_state = UI_STATE_SET_HP_PRESSURE,          /* 当前状态：间歇模式高压设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustPressureUp,            /* 动作函数：增加高压值 */
        .next_state    = UI_STATE_SET_HP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [20] = {
        .current_state = UI_STATE_SET_HP_PRESSURE,          /* 当前状态：间歇模式高压设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustPressureDown,          /* 动作函数：减少高压值 */
        .next_state    = UI_STATE_SET_HP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [21] = {
        .current_state = UI_STATE_SET_HP_PRESSURE,          /* 当前状态：间歇模式高压设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放*/        
        .action_func   = AppUI_StateEntry_SET_LP_Pressure,  /* 动作函数：进入低压设置界面 */
        .next_state    = UI_STATE_SET_LP_PRESSURE           /* 下一状态：间歇模式低压设置界面 */
    },
    
    [22] = {
        .current_state = UI_STATE_SET_HP_PRESSURE,          /* 当前状态：间歇模式高压设置界面 */
        .trigger_event = UI_EVENT_SETTINGS,                 /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_ZHT,              /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },
    
    [23] = {
        .current_state = UI_STATE_SET_LP_PRESSURE,          /* 当前状态：间歇模式低压设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustPressureUp,            /* 动作函数：增加低压值 */
        .next_state    = UI_STATE_SET_LP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [24] = {
        .current_state = UI_STATE_SET_LP_PRESSURE,          /* 当前状态：间歇模式低压设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustPressureDown,          /* 动作函数：减少低压值 */
        .next_state    = UI_STATE_SET_LP_PRESSURE           /* 下一状态：保持当前状态 */
    },
    
    [25] = {
        .current_state = UI_STATE_SET_LP_PRESSURE,          /* 当前状态：间歇模式低压设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_StateEntry_SET_Time,         /* 动作函数：进入时间设置界面 */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：间歇模式时间设置界面 */
    },
    
    [26] = {
        .current_state = UI_STATE_SET_LP_PRESSURE,          /* 当前状态：间歇模式低压设置界面 */
        .trigger_event = UI_EVENT_SETTINGS,                 /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_ZHT,              /* 动作函数：进入暂停模式 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },
    
    [27] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_MENU_UP,                  /* 触发事件：菜单向上 */
        .action_func   = AppUI_AdjustTimeUp,                /* 动作函数：增加时间值 */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：保持当前状态 */
    },
    
    [28] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_MENU_DOWN,                /* 触发事件：菜单向下 */
        .action_func   = AppUI_AdjustTimeDown,              /* 动作函数：减少时间值 */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：保持当前状态 */
    },
    
    [29] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_CONFIRM_LONG,             /* 触发事件：确认键长按释放 */
        .action_func   = AppUI_SwitchTimeEdit,              /* 动作函数：切换编辑项（高压时间/低压时间） */
        .next_state    = UI_STATE_SET_TIME                  /* 下一状态：保持当前状态 */
    },
    
    [30] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_CONFIRM,                  /* 触发事件：确认键（长按释放，低压时间完成后退出） */
        .action_func   = AppUI_TimeSetting_ExitToPause,     /* 动作函数：退出时间设置并进入暂停 */
        .next_state    = UI_STATE_ZHT                       /* 下一状态：暂停模式 */
    },
    
    [31] = {
        .current_state = UI_STATE_SET_TIME,                 /* 当前状态：间歇模式时间设置界面 */
        .trigger_event = UI_EVENT_SETTINGS,                 /* 触发事件：设置键 */
        .action_func   = AppUI_StateEntry_ZHT,              /* 动作函数：进入暂停模式 */
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
    // 显示开机Logo（由display模块处理）
    Display_ShowStartupInterface();
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
    
    Display_ShowString(25, 0, "-135 mmHg", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    Display_ShowIcon(32, 2, ICON_KEY2);  // 按键图标上半部分（页2，指向Settings）
    Display_ShowString(48, 2, "Settings", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
   
    Display_ShowIcon(35, 4, ICON_KEY1);  // 按键图标上半部分（页2，指向Settings）
    Display_ShowString(48, 4, "Therapy", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
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
    char target_str[16] = {0};
    uint16_t current_pressure = AppPressure_GetPressureValue();
    g_last_display_pressure = current_pressure;
    snprintf(target_str, sizeof(target_str), "-%03u", (unsigned int)current_pressure);
    Display_ShowString(16, 4, target_str, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
    Display_ShowString(80, 4, "mmhg", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);    

    // 显示提示操作
    Display_ShowString(12, 6, "Therapy On", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
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
    char target_str[16] = {0};
    snprintf(target_str, sizeof(target_str), "-%u mmHg", (unsigned int)g_ui_context.pressure_high);
    Display_ShowString(25, 0, target_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);

    // 显示间歇模式高压时间
    char hp_time_str[8] = {0};
    snprintf(hp_time_str, sizeof(hp_time_str), "%02umin", (unsigned int)g_ui_context.time_high);
    Display_ShowString(92, 2, hp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);

    // 显示间歇模式低压时间
    char lp_time_str[8] = {0};
    snprintf(lp_time_str, sizeof(lp_time_str), "%02umin", (unsigned int)g_ui_context.time_low);
    Display_ShowString(92, 4, lp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 显示实时压力值
    char pressure_str[16] = {0};
    uint16_t current_pressure = AppPressure_GetPressureValue();
    snprintf(pressure_str, sizeof(pressure_str), "-%03u", (unsigned int)current_pressure);
    Display_ShowString(0, 4, pressure_str, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
    Display_ShowString(60, 4, "mmhg", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    g_last_display_pressure = current_pressure;
        
    // 显示当前阶段
    Display_ShowString(12, 6, "High Phase", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
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
    char target_str[16]={0};
    snprintf(target_str, sizeof(target_str), "-%u mmHg", (unsigned int)g_ui_context.pressure_high);
    Display_ShowString(25, 0, target_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
    
    // 显示暂停状态
    Display_ShowString(24, 3, "Therapy Off", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    // 显示启动按键的图标
    Display_ShowIcon(16, 6, ICON_KEY1);  // 按键图标上半部分（页2，指向Settings）
    Display_ShowString(30, 6, "Therapy", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
    
}

/**
 * @name      AppUI_Display_SET
 * @brief     显示设置模式界面
 */
static void AppUI_Display_SET(void)
{
    // 显示设置菜单
    Display_ShowString(2, 0, "Mode", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowIcon(66, 0, ICON_KEY2);  
    Display_ShowString(78, 0, "Switch", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(2, 2, "Continuous", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(1, 4, "Intermittent", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
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
    // 参考未重构工程：显示"Pressure"和压力值
    Display_ShowString(36, 0, "Pressure", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    // 显示压力值（使用16x32大字体，参考未重构工程的DISP_Dig15_32）
    // y=4：16x32字体占用4页（页4-7），避免与"Pressure"文字（页0-1）重叠
    Display_ShowPressure(48, 4, g_ui_context.pressure_high, true, DISPLAY_FONT_16X32);  // 显示压力值和单位"mmHg"
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
    Display_ShowString(36, 0, "Pressure", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(6, 2, "HP Set:-", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(6, 4, "LP Set:-", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    char hp_pressure_str[8] = {0};
    char lp_pressure_str[8] = {0};
    
    // 显示高压
    snprintf(hp_pressure_str, sizeof(hp_pressure_str), "%03ummhg", (unsigned int)g_ui_context.pressure_high);
    Display_ShowString(73, 2, hp_pressure_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    
    // 显示低压
    snprintf(lp_pressure_str, sizeof(lp_pressure_str), "%03ummhg", (unsigned int)g_ui_context.pressure_low);
    Display_ShowString(73, 4, lp_pressure_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
}

/**
 * @name      AppUI_Display_SET_LP_Pressure
 * @brief     显示间歇模式低压设置界面
 */
static void AppUI_Display_SET_LP_Pressure(void)
{
    // 参考未重构工程：显示"Pressure"、"HP Set"和"LP Set"
    Display_ShowString(36, 0, "Pressure", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(6, 2, "HP Set:-", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(6, 4, "LP Set:-", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    char hp_pressure_str[8] = {0};
    char lp_pressure_str[8] = {0};
    
    
    // 显示高压
    snprintf(hp_pressure_str, sizeof(hp_pressure_str), "%03ummhg", (unsigned int)g_ui_context.pressure_high);
    Display_ShowString(73, 2, hp_pressure_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    
    // 显示低压
    snprintf(lp_pressure_str, sizeof(lp_pressure_str), "%03ummhg", (unsigned int)g_ui_context.pressure_low);
    Display_ShowString(73, 4, lp_pressure_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
       
}

/**
 * @name      AppUI_Display_SET_Time
 * @brief     显示间歇模式时间设置界面
 */
static void AppUI_Display_SET_Time(void)
{
    // 显示"Intermittent"、"HP Time"和"LP Time"
    Display_ShowString(16, 0, "Intermittent", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(6, 2, "HP Time:", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    Display_ShowString(6, 4, "LP Time:", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    char hp_time_str[8] = {0};
    char lp_time_str[8] = {0};
    
    // 显示高压时间
    snprintf(hp_time_str, sizeof(hp_time_str), "%02umin", (unsigned int)g_ui_context.time_high);
    Display_ShowString(75, 2, hp_time_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
    
    
    // 显示低压时间
    snprintf(lp_time_str, sizeof(lp_time_str), "%02umin", (unsigned int)g_ui_context.time_low);
    Display_ShowString(75, 4, lp_time_str, DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
 
    // 显示当前选中的时间项（可选：添加选中指示，如箭头或高亮）
    // 当前实现中，通过上下键切换编辑项，这里可以添加视觉反馈
}

/****************************************************************************
 * 公共接口实现
 ****************************************************************************/

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
    // settings_sub_state已废弃，现在使用独立的FSM状态
    g_ui_context.pressure_high = PRESSURE_SET_DEFAULT;
    g_ui_context.pressure_low = PRESSURE_LOW_DEFAULT;
    g_ui_context.time_high = TIME_HIGH_DEFAULT;
    g_ui_context.time_low = TIME_LOW_DEFAULT;
    g_ui_context.time_edit_high = true;  // 默认编辑高压时间
    g_ui_context.pressure_step = 5;      // 压力调整步进值，默认5mmHg
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
    
    /* 创建初始化超时定时器（7秒，单次模式） */
    /* 注意：SoftTimer_Create的参数单位是毫秒，所以7秒应该填写7000 */
    g_init_timeout_timer = SoftTimer_Create(SOFT_TIMER_MODE_ONCE, 
                                            7000,  // 7秒 = 7000毫秒
                                            AppUI_InitTimeoutCallback, 
                                            NULL);
    
    if (g_init_timeout_timer != 0) {
        /* 启动定时器 */
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
                /* 记录用户交互：仅在未锁定时重置计时 */
                if (g_ui_context.auto_lock_active == false) {
                    AppUI_ResetLockTimer();
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
    
    /* 自动锁定检测（在处理完本轮事件后执行） */
    AppUI_UpdateAutoLock();
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
    static uint16_t battery_display_counter = 0;      // 显示更新计数器（用于降低刷新频率）
    static uint16_t battery_charge_blink_counter = 0; // 充电闪烁计数器（用于充电动画）
    static uint8_t last_battery_level = 100;
    static bool last_battery_charging = false;
    
    /* 获取当前电池信息 */
    bool battery_changed = AppBattery_IsLevelChanged();
    uint8_t current_battery_level = AppBattery_GetLevel();
    bool current_battery_charging = AppBattery_IsCharging();
    
    /* 充电闪烁计数器递增（每10ms递增一次） */
    if (current_battery_charging) {
        battery_charge_blink_counter++;
        if (battery_charge_blink_counter >= 125) {  // 125次 = 1.25秒，循环5个图标
            battery_charge_blink_counter = 0;
        }
    } else {
        battery_charge_blink_counter = 0;  // 非充电时重置
    }
    
    /* 判断是否需要更新显示 */
    bool need_update = false;
    
    if (battery_changed || 
        current_battery_level != last_battery_level || 
        current_battery_charging != last_battery_charging) {
        /* 电池信息变化，立即更新显示 */
        need_update = true;
        last_battery_level = current_battery_level;
        last_battery_charging = current_battery_charging;
        battery_display_counter = 0;  // 重置计数器
    } else if (current_battery_charging) {
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
        
        /* 充电时实现闪烁效果（参考未重构工程：循环显示不同电量图标） */
        if (current_battery_charging) {
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
        
        /* 显示电池图标（参考未重构工程：DISP_Bat000(6, 102)，即页6，列102） */
        /* Display_ShowBatteryIcon参数：x=列坐标，y=页坐标 */
        Display_ShowBatteryIcon(102, 0, display_level, current_battery_charging);
    }

    /* 治疗界面的实时压力刷新（连续/间歇共用） */
    bool in_continuous = (g_ui_context.current_state == UI_STATE_LIX);
    bool in_intermittent = (g_ui_context.current_state == UI_STATE_JIX);

    if (in_continuous || in_intermittent) {
        if (++g_pressure_refresh_tick >= UI_PRESSURE_REFRESH_INTERVAL_TICKS) {
            g_pressure_refresh_tick = 0;

            uint16_t display_target = in_continuous
                                       ? g_ui_context.pressure_high
                                       : AppPressure_GetCurrentTarget();
            
            // 显示目标气压值
            char target_buf[16] = {0};
            snprintf(target_buf, sizeof(target_buf), "-%u mmHg", (unsigned int)display_target);
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
                
                char pressure_buf[16] = {0};
                g_last_display_pressure = current_pressure;
                
                if(in_continuous) {                
                    snprintf(pressure_buf, sizeof(pressure_buf), "-%03u", (unsigned int)current_pressure);
                    Display_ShowString(16, 4, pressure_buf, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
                    Display_ShowString(80, 4, "mmhg", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
                    
                }else if (in_intermittent){        
                    snprintf(pressure_buf, sizeof(pressure_buf), "-%03u", (unsigned int)current_pressure);
                    Display_ShowString(0, 4, pressure_buf, DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
                    Display_ShowString(60, 4, "mmhg", DISPLAY_FONT_8X16, DISPLAY_ALIGN_LEFT);
                }
            }

            if (in_intermittent) {
                // 显示间歇模式高压时间
                char hp_time_str[8] = {0};
                snprintf(hp_time_str, sizeof(hp_time_str), "%02umin", (unsigned int)g_ui_context.time_high);
                Display_ShowString(92, 2, hp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);

                // 显示间歇模式低压时间
                char lp_time_str[8] = {0};
                snprintf(lp_time_str, sizeof(lp_time_str), "%02umin", (unsigned int)g_ui_context.time_low);
                Display_ShowString(92, 4, lp_time_str, DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
                 
                // 显示当前模式
                AppPressureControlMode_e mode = AppPressure_GetCurrentMode();
                const char* phase_str = (mode == APP_PRESSURE_CONTROL_MODE_INTERMITTENT_LOW) ? "Low Phase" : "High Phase";
                Display_ShowString(12, 6, phase_str, DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
                
            } else {
                Display_ShowString(12, 6, "Therapy On", DISPLAY_FONT_7X14, DISPLAY_ALIGN_LEFT);
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
