/****************************************************************************
 * 文件名: key_machine.h
 * 功能: 按键状态机组件头文件
 * 
 * 说明: 
 *   基于软件定时器实现按键状态机
 *   支持按键消抖、长按检测、多按键处理
 *   提供统一的按键事件接口
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#ifndef KEY_MACHINE_H
#define KEY_MACHINE_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 常量定义
 ****************************************************************************/
#define KEY_MAX_COUNT        4    // 最大按键数量
#define KEY_DEBOUNCE_MS      20   // 消抖时间（毫秒）
#define KEY_LONG_PRESS_MS    1000 // 长按时间（毫秒）
#define KEY_ULTRA_LONG_PRESS_MS 3000 // 超长按时间（毫秒）

/****************************************************************************
 * 类型定义
 ****************************************************************************/

/* 按键状态 */
typedef enum {
    KEY_STATE_IDLE = 0,      // 空闲状态
    KEY_STATE_PRESSED,       // 按下状态
    KEY_STATE_HELD,          // 长按状态
    KEY_STATE_RELEASED       // 释放状态
} KeyState_e;

/* 按键事件 */
typedef enum {
    KEY_MACHINE_EVENT_NONE = 0,      // 无事件
    KEY_MACHINE_EVENT_PRESS,         // 按下事件
    KEY_MACHINE_EVENT_RELEASE,       // 释放事件
    KEY_MACHINE_EVENT_LONG_PRESS,    // 长按事件（1秒）
    KEY_MACHINE_EVENT_ULTRA_LONG_PRESS, // 超长按事件（3秒）
    KEY_MACHINE_EVENT_CLICK          // 单击事件
} KeyMachineEvent_e;

/* 按键回调函数类型 */
typedef void (*KeyCallback_t)(uint8_t key_id, KeyMachineEvent_e event);

/* 按键结构体 */
typedef struct {
    uint8_t key_id;          // 按键ID
    KeyState_e state;        // 当前状态
    bool is_pressed;         // 当前按下状态
    bool last_pressed;       // 上次按下状态
    uint32_t press_time;     // 按下时间
    uint32_t release_time;   // 释放时间
    KeyCallback_t callback;   // 回调函数
    bool is_used;            // 是否被使用
} Key_t;

/****************************************************************************
 * API函数声明
 ****************************************************************************/

/**
 * 函数: KeyMachine_Init
 * 功能: 初始化按键状态机模块
 * 参数: 无
 * 返回: 无
 * 说明: 必须在其他API调用前执行
 */
void KeyMachine_Init(void);

/**
 * 函数: KeyMachine_RegisterKey
 * 功能: 注册按键
 * 参数: 
 *   - key_id: 按键ID
 *   - callback: 按键事件回调函数
 * 返回: bool true-成功, false-失败
 * 说明: 注册后按键开始监控
 */
bool KeyMachine_RegisterKey(uint8_t key_id, KeyCallback_t callback);

/**
 * 函数: KeyMachine_UnregisterKey
 * 功能: 注销按键
 * 参数: key_id - 按键ID
 * 返回: bool true-成功, false-失败
 * 说明: 注销后停止监控该按键
 */
bool KeyMachine_UnregisterKey(uint8_t key_id);

/**
 * 函数: KeyMachine_UpdateKeyState
 * 功能: 更新按键状态
 * 参数: 
 *   - key_id: 按键ID
 *   - is_pressed: 当前按下状态
 * 返回: 无
 * 说明: 由外部调用，更新按键物理状态
 */
void KeyMachine_UpdateKeyState(uint8_t key_id, bool is_pressed);

/**
 * 函数: KeyMachine_Process
 * 功能: 处理按键状态机
 * 参数: 无
 * 返回: 无
 * 说明: 在主循环中调用，处理所有按键状态机
 */
void KeyMachine_Process(void);

/**
 * 函数: KeyMachine_GetKeyState
 * 功能: 获取按键状态
 * 参数: key_id - 按键ID
 * 返回: KeyState_e 按键状态
 * 说明: 返回当前按键状态
 */
KeyState_e KeyMachine_GetKeyState(uint8_t key_id);

/**
 * 函数: KeyMachine_IsKeyPressed
 * 功能: 检查按键是否按下
 * 参数: key_id - 按键ID
 * 返回: bool true-按下, false-未按下
 * 说明: 检查按键当前按下状态
 */
bool KeyMachine_IsKeyPressed(uint8_t key_id);

#endif /* KEY_MACHINE_H */
