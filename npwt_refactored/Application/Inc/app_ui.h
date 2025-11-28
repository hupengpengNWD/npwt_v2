/**
 * @file    app_ui.h
 * @brief   应用层UI模块头文件
 * @date    2025-01-27
 * 
 * 应用层UI模块，使用FSM组件实现按键触发UI界面切换功能
 */

#ifndef __APP_UI_H__
#define __APP_UI_H__

/****************************************************************************
 * 包含文件
 ****************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "../../Middleware/Inc/fsm.h"
#include "../../Middleware/Inc/display.h"
#include "../../Middleware/Inc/key_machine.h"

/****************************************************************************
 * 类型定义
 ****************************************************************************/

/**
 * @brief UI系统状态（对应未重构工程的mod_main_a）
 */
typedef enum {
    UI_STATE_SYS = 0,              // MOD_SYS: 系统初始化模式
    UI_STATE_WAT = 1,              // MOD_WAT: 等待指令模式（待机）
    UI_STATE_LIX = 2,              // MOD_LIX: 连续工作模式
    UI_STATE_JIX = 3,              // MOD_JIX: 间歇工作模式
    UI_STATE_SET = 4,              // MOD_SET: 参数设定模式（模式选择界面）
    UI_STATE_SET_PRESSURE = 9,     // 连续模式压力设置界面
    UI_STATE_SET_HP_PRESSURE = 10, // 间歇模式高压设置界面
    UI_STATE_SET_LP_PRESSURE = 11, // 间歇模式低压设置界面
    UI_STATE_SET_TIME = 12,        // 间歇模式时间设置界面
    UI_STATE_ZHT = 5,              // MOD_ZHT: 暂停模式
    UI_STATE_OFF = 7,              // MOD_OFF: 关机模式
    UI_STATE_TK = 8,               // MOD_TK: 空闲超时模式
    UI_STATE_COUNT                 // 状态总数
} UIState_e;

/**
 * @brief 设置模式子状态（已废弃，改为使用独立的FSM状态）
 * @deprecated 现在使用UI_STATE_SET、UI_STATE_SET_PRESSURE等独立状态
 */
typedef enum {
    SET_SUB_STATE_MODE_SELECT = 0,      // 模式选择界面（对应UI_STATE_SET）
    SET_SUB_STATE_PRESSURE = 1,         // 压力设置界面（对应UI_STATE_SET_PRESSURE）
    SET_SUB_STATE_HP_PRESSURE = 2,      // 高压设置界面（对应UI_STATE_SET_HP_PRESSURE）
    SET_SUB_STATE_LP_PRESSURE = 3,      // 低压设置界面（对应UI_STATE_SET_LP_PRESSURE）
    SET_SUB_STATE_TIME = 4,             // 时间设置界面（对应UI_STATE_SET_TIME）
    SET_SUB_STATE_COUNT
} SettingsSubState_e;

/**
 * @brief UI事件类型（基于按键事件和系统事件）
 */
typedef enum {
    UI_EVENT_NONE = 0,           // 无事件
    UI_EVENT_CONFIRM = 1,        // 确认/选择操作（原KEY_OK）
    UI_EVENT_MENU_UP = 2,        // 菜单向上/增加操作（原KEY_UP）
    UI_EVENT_MENU_DOWN = 3,      // 菜单向下/减少操作（原KEY_DN）
    UI_EVENT_SETTINGS = 4,       // 设置菜单操作（原KEY_UP_LONG，进入/退出设置）
    UI_EVENT_QUICK_DOWN = 5,     // 快速向下/快速减少（原KEY_DN_LONG）
    UI_EVENT_START = 6,          // 启动治疗（原KEY_START）
    UI_EVENT_TIMEOUT = 7,        // 超时事件
    UI_EVENT_UNLOCK = 8,         // 解锁事件
    UI_EVENT_CONFIRM_LONG = 9,   // 长按确认键释放（进入第二个设置界面）
    UI_EVENT_COUNT               // 事件总数
} UIEvent_e;

/**
 * @brief UI上下文数据
 */
typedef struct {
    UIState_e current_state;        // 当前状态
    UIState_e last_state;           // 上次状态
    UIState_e work_mode_backup;     // 工作模式备份（连续/间歇）
    bool lock_flag;                 // 锁定标志
    bool auto_lock_active;          // 是否处于自动锁定状态
    bool lock_icon_visible;         // 锁定图标是否已显示
    uint16_t lock_inactive_ticks;   // 无操作计时（10ms Tick）
    bool idle_active;               // 是否处于空闲状态
    uint16_t idle_inactive_ticks;  // 空闲计时器（10ms Tick）
    UIState_e idle_previous_state;  // 空闲前的状态（用于恢复显示）
    bool leak_alarm_active;         // 是否处于泄漏报警状态
    UIState_e leak_alarm_previous_state;  // 泄漏报警前的状态（用于恢复显示）
    bool blockage_alarm_active;     // 是否处于管路堵塞报警状态
    UIState_e blockage_alarm_previous_state;  // 管路堵塞报警前的状态（用于恢复显示）
    bool overpressure_alarm_active;  // 是否处于过压报警状态（入口堵塞）
    UIState_e overpressure_alarm_previous_state;  // 过压报警前的状态（用于恢复显示）
    SettingsSubState_e settings_sub_state;  // 设置模式子状态
    uint16_t pressure_high;         // 高压值（mmHg，连续模式使用，间歇模式也使用）
    uint16_t pressure_low;          // 低压值（mmHg，仅间歇模式使用）
    uint16_t time_high;             // 高压时间（分钟，仅间歇模式使用）
    uint16_t time_low;              // 低压时间（分钟，仅间歇模式使用）
    bool time_edit_high;            // 时间编辑标志：true=编辑高压时间，false=编辑低压时间
    uint16_t pressure_step;         // 压力调整步进值（mmHg，默认5，可通过变量修改）
    void* user_data;                // 用户数据指针
} UIContext_t;

/****************************************************************************
 * 公共接口声明
 ****************************************************************************/

/**
 * @name      AppUI_Init
 * @brief     初始化UI模块
 * @param     无
 * @retval    无
 */
void AppUI_Init(void);

/**
 * @name      AppUI_Process
 * @brief     处理UI状态机（每20ms调用一次）
 * @param     无
 * @retval    无
 */
void AppUI_Process(void);

/* AppUI_OnKeyEvent函数已移除
 * 按键事件现在通过app_button模块的队列机制传递
 * app_ui模块通过AppUI_Process从队列读取并处理事件
 */

/**
 * @name      AppUI_GetCurrentState
 * @brief     获取当前UI状态
 * @param     无
 * @retval    UIState_e - 当前状态
 */
UIState_e AppUI_GetCurrentState(void);

/**
 * @name      AppUI_SetLockFlag
 * @brief     设置锁定标志
 * @param     locked - 是否锁定
 * @retval    无
 */
void AppUI_SetLockFlag(bool locked);

/**
 * @name      AppUI_GetLockFlag
 * @brief     获取锁定标志
 * @param     无
 * @retval    bool - 是否锁定
 */
bool AppUI_GetLockFlag(void);

/**
 * @name      AppUI_GetContext
 * @brief     获取UI上下文指针（用于参数保存等操作）
 * @param     无
 * @retval    UIContext_t* - UI上下文指针
 */
UIContext_t* AppUI_GetContext(void);

#endif /* __APP_UI_H__ */
