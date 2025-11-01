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
    UI_STATE_SYS = 0,      // MOD_SYS: 系统初始化模式
    UI_STATE_WAT = 1,      // MOD_WAT: 等待指令模式（待机）
    UI_STATE_LIX = 2,      // MOD_LIX: 连续工作模式
    UI_STATE_JIX = 3,      // MOD_JIX: 间歇工作模式
    UI_STATE_SET = 4,      // MOD_SET: 参数设定模式
    UI_STATE_ZHT = 5,      // MOD_ZHT: 暂停模式
    UI_STATE_OFF = 7,      // MOD_OFF: 关机模式
    UI_STATE_TK = 8,       // MOD_TK: 空闲超时模式
    UI_STATE_COUNT         // 状态总数
} UIState_e;

/**
 * @brief UI事件类型（基于按键事件和系统事件）
 */
typedef enum {
    UI_EVENT_NONE = 0,           // 无事件
    UI_EVENT_KEY_OK = 1,        // 确认键按下（KEY_OK / KEY_C）
    UI_EVENT_KEY_UP = 2,        // 上键按下
    UI_EVENT_KEY_DN = 3,        // 下键按下
    UI_EVENT_KEY_UP_LONG = 4,   // 上键长按（KEY_UPL）
    UI_EVENT_KEY_DN_LONG = 5,   // 下键长按（KEY_DNL）
    UI_EVENT_KEY_START = 6,     // 启动键按下
    UI_EVENT_TIMEOUT = 7,       // 超时事件
    UI_EVENT_UNLOCK = 8,        // 解锁事件
    UI_EVENT_COUNT              // 事件总数
} UIEvent_e;

/**
 * @brief UI上下文数据
 */
typedef struct {
    UIState_e current_state;        // 当前状态
    UIState_e last_state;           // 上次状态
    UIState_e work_mode_backup;     // 工作模式备份（连续/间歇）
    bool lock_flag;                 // 锁定标志
    uint8_t settings_sub_state;    // 设置模式子状态
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

/* 注意：AppUI_OnKeyEvent函数已移除
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

#endif /* __APP_UI_H__ */
