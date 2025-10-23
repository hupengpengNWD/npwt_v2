/****************************************************************************
 * 文件名: key_machine.c
 * 功能: 按键状态机组件实现
 * 
 * 说明: 
 *   基于软件定时器实现按键状态机
 *   支持按键消抖、长按检测、多按键处理
 *   提供统一的按键事件接口
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#include "key_machine.h"
#include "../Inc/soft_timer.h"
#include <stddef.h>

/****************************************************************************
 * 全局变量
 ****************************************************************************/
static Key_t g_keys[KEY_MAX_COUNT];
static bool g_initialized = false;

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/
static Key_t* KeyMachine_GetKey(uint8_t key_id);
static void KeyMachine_ProcessKey(Key_t* key);

/****************************************************************************
 * API函数实现
 ****************************************************************************/

/**
 * 函数: KeyMachine_Init
 * 功能: 初始化按键状态机模块
 */
void KeyMachine_Init(void)
{
    if (g_initialized) {
        return;
    }
    
    // 初始化所有按键
    for (uint8_t i = 0; i < KEY_MAX_COUNT; i++) {
        g_keys[i].key_id = i + 1;
        g_keys[i].state = KEY_STATE_IDLE;
        g_keys[i].is_pressed = false;
        g_keys[i].last_pressed = false;
        g_keys[i].press_time = 0;
        g_keys[i].release_time = 0;
        g_keys[i].callback = NULL;
        g_keys[i].is_used = false;
    }
    
    g_initialized = true;
}

/**
 * 函数: KeyMachine_RegisterKey
 * 功能: 注册按键
 */
bool KeyMachine_RegisterKey(uint8_t key_id, KeyCallback_t callback)
{
    if (!g_initialized || key_id == 0 || key_id > KEY_MAX_COUNT) {
        return false;
    }
    
    Key_t* key = &g_keys[key_id - 1];
    if (key->is_used) {
        return false;  // 按键已被注册
    }
    
    key->callback = callback;
    key->is_used = true;
    key->state = KEY_STATE_IDLE;
    key->is_pressed = false;
    key->last_pressed = false;
    key->press_time = 0;
    key->release_time = 0;
    
    return true;
}

/**
 * 函数: KeyMachine_UnregisterKey
 * 功能: 注销按键
 */
bool KeyMachine_UnregisterKey(uint8_t key_id)
{
    if (!g_initialized || key_id == 0 || key_id > KEY_MAX_COUNT) {
        return false;
    }
    
    Key_t* key = &g_keys[key_id - 1];
    if (!key->is_used) {
        return false;  // 按键未被注册
    }
    
    key->is_used = false;
    key->callback = NULL;
    key->state = KEY_STATE_IDLE;
    
    return true;
}

/**
 * 函数: KeyMachine_UpdateKeyState
 * 功能: 更新按键状态
 */
void KeyMachine_UpdateKeyState(uint8_t key_id, bool is_pressed)
{
    if (!g_initialized || key_id == 0 || key_id > KEY_MAX_COUNT) {
        return;
    }
    
    Key_t* key = &g_keys[key_id - 1];
    if (!key->is_used) {
        return;
    }
    
    key->last_pressed = key->is_pressed;
    key->is_pressed = is_pressed;
}

/**
 * 函数: KeyMachine_Process
 * 功能: 处理按键状态机
 */
void KeyMachine_Process(void)
{
    if (!g_initialized) {
        return;
    }
    
    // 处理所有已注册的按键
    for (uint8_t i = 0; i < KEY_MAX_COUNT; i++) {
        if (g_keys[i].is_used) {
            KeyMachine_ProcessKey(&g_keys[i]);
        }
    }
}

/**
 * 函数: KeyMachine_GetKeyState
 * 功能: 获取按键状态
 */
KeyState_e KeyMachine_GetKeyState(uint8_t key_id)
{
    if (!g_initialized || key_id == 0 || key_id > KEY_MAX_COUNT) {
        return KEY_STATE_IDLE;
    }
    
    Key_t* key = &g_keys[key_id - 1];
    if (!key->is_used) {
        return KEY_STATE_IDLE;
    }
    
    return key->state;
}

/**
 * 函数: KeyMachine_IsKeyPressed
 * 功能: 检查按键是否按下
 */
bool KeyMachine_IsKeyPressed(uint8_t key_id)
{
    if (!g_initialized || key_id == 0 || key_id > KEY_MAX_COUNT) {
        return false;
    }
    
    Key_t* key = &g_keys[key_id - 1];
    if (!key->is_used) {
        return false;
    }
    
    return key->is_pressed;
}

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * 函数: KeyMachine_GetKey
 * 功能: 获取按键对象
 */
static Key_t* KeyMachine_GetKey(uint8_t key_id)
{
    if (key_id == 0 || key_id > KEY_MAX_COUNT) {
        return NULL;
    }
    
    return &g_keys[key_id - 1];
}

/**
 * 函数: KeyMachine_ProcessKey
 * 功能: 处理单个按键状态机
 */
static void KeyMachine_ProcessKey(Key_t* key)
{
    if (key == NULL || !key->is_used) {
        return;
    }
    
    uint32_t current_time = SoftTimer_GetTickCount();
    
    switch (key->state) {
        case KEY_STATE_IDLE:
        {
            // 检测按键按下
            if (key->is_pressed && !key->last_pressed) {
                key->press_time = current_time;
                key->state = KEY_STATE_PRESSED;
                
                // 调用按下回调
                if (key->callback != NULL) {
                    key->callback(key->key_id, KEY_MACHINE_EVENT_PRESS);
                }
            }
            break;
        }
        
        case KEY_STATE_PRESSED:
        {
            // 检测按键释放
            if (!key->is_pressed && key->last_pressed) {
                key->release_time = current_time;
                key->state = KEY_STATE_RELEASED;
                
                // 调用释放回调
                if (key->callback != NULL) {
                    key->callback(key->key_id, KEY_MACHINE_EVENT_RELEASE);
                }
            }
            // 检测超长按（3秒）
            else if (key->is_pressed && (current_time - key->press_time) >= (KEY_ULTRA_LONG_PRESS_MS / SOFT_TIMER_TICK_MS)) {
                key->state = KEY_STATE_HELD;
                
                // 调用超长按回调
                if (key->callback != NULL) {
                    key->callback(key->key_id, KEY_MACHINE_EVENT_ULTRA_LONG_PRESS);
                }
            }
            // 检测长按（1秒）
            else if (key->is_pressed && (current_time - key->press_time) >= (KEY_LONG_PRESS_MS / SOFT_TIMER_TICK_MS)) {
                key->state = KEY_STATE_HELD;
                
                // 调用长按回调
                if (key->callback != NULL) {
                    key->callback(key->key_id, KEY_MACHINE_EVENT_LONG_PRESS);
                }
            }
            break;
        }
        
        case KEY_STATE_HELD:
        {
            // 检测按键释放
            if (!key->is_pressed && key->last_pressed) {
                key->release_time = current_time;
                key->state = KEY_STATE_RELEASED;
                
                // 调用释放回调
                if (key->callback != NULL) {
                    key->callback(key->key_id, KEY_MACHINE_EVENT_RELEASE);
                }
            }
            break;
        }
        
        case KEY_STATE_RELEASED:
        {
            // 检测单击（短按后释放）
            if (!key->is_pressed && (current_time - key->release_time) >= (KEY_DEBOUNCE_MS / SOFT_TIMER_TICK_MS)) {
                // 判断是否为单击（按下时间小于长按时间）
                if ((key->release_time - key->press_time) < (KEY_LONG_PRESS_MS / SOFT_TIMER_TICK_MS)) {
                    if (key->callback != NULL) {
                        key->callback(key->key_id, KEY_MACHINE_EVENT_CLICK);
                    }
                }
                
                key->state = KEY_STATE_IDLE;
            }
            break;
        }
        
        default:
            key->state = KEY_STATE_IDLE;
            break;
    }
}
