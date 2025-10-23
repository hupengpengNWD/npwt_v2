/**
  ******************************************************************************
  * @file:    key_machine.c
  * @author:  Refactored from lib_gpio_input
  * @date:    2025-01-22
  * @brief:   按键状态机模块实现 - 基于lib_gpio_input重构
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#include "key_machine.h"
#include "../../Middleware/Inc/soft_timer.h"
#include <stddef.h>
#include <stdlib.h>

// ==================== 默认配置 ====================
const KeyConfig_t KEY_CONFIG_DEFAULT = {
    .debounce_ms = 20,          // 20ms去抖
    .short_press_ms = 50,       // 50ms短按
    .long_press_ms = 1000,      // 1秒长按
    .ultra_long_press_ms = 3000, // 3秒超长按
    .trigger = KEY_TRIGGER_LOW, // 低电平触发
    .event_mask = KEY_EVENT_MASK_ALL // 所有事件
};

// ==================== 全局按键管理器 ====================
static KeyManager_t g_key_manager = {0};

// ==================== 内部函数声明 ====================
static void KeyMachine_InternalFSM(KeyMachinePtr_t ptr);
static void KeyMachine_InternalSetTrigger(KeyMachinePtr_t ptr, KeyTrigger_e trigger);
static void KeyMachine_InternalSetDebounceMs(KeyMachinePtr_t ptr, uint32_t debounce_ms);
static void KeyMachine_InternalSetEventMask(KeyMachinePtr_t ptr, uint32_t event_mask);
static uint8_t KeyMachine_InternalGetLevel(KeyMachinePtr_t ptr);
static KeyState_e KeyMachine_InternalGetState(KeyMachinePtr_t ptr);
static void KeyMachine_InternalInitialize(KeyMachinePtr_t ptr, const KeyConfig_t* config, KeyCallback_t callback, void* arg);
static void KeyMachine_InternalConfigure(KeyMachinePtr_t ptr);

// ==================== 按键状态机实现 ====================

/**
  * @name     KeyMachine_FSM
  * @brief    按键状态机处理
  * @param    ptr: 按键状态机指针
  * @retval   None
  * @remark   处理去抖、短按、长按、超长按、释放事件
  */
void KeyMachine_FSM(KeyMachinePtr_t ptr) {
    if (ptr == NULL || ptr->gpio_drv_ptr == NULL || ptr->read_level == NULL) {
        return;
    }
    
    // 使用驱动层函数指针读取电平
    uint8_t read_level = ptr->read_level(ptr->gpio_drv_ptr);
    
    // 去抖处理：检查当前读取的电平是否与上次记录的电平不同
    if (read_level != ptr->current_level) {
        ptr->debounce_count++;
        
        // 检查去抖计数是否达到去抖时间所需的轮询次数
        // 假设系统10ms轮询一次，所以去抖时间除以10
        if (ptr->debounce_count >= (ptr->config.debounce_ms / 10)) {
            // 去抖时间到达，确认电平变化有效，更新当前电平
            ptr->current_level = read_level;
            ptr->debounce_count = 0;
        }
    } else {
        // 如果电平未变化，重置去抖计数
        ptr->debounce_count = 0;
    }

    /* 判断当前电平是否为触发状态 */
    bool is_triggered = (ptr->current_level == (ptr->trigger == KEY_TRIGGER_LOW ? 0 : 1));
    
    switch (ptr->state) {
        case KEY_STATE_IDLE:
            // 如果检测到触发状态，进入短按状态
            if (is_triggered) {
                ptr->press_time_ms = 0; // 重置按下时间计数
                ptr->state = KEY_STATE_SHORT;
                
                // 触发按下事件
                if (ptr->callback && (ptr->event_mask & KEY_EVENT_MASK_PRESS)) {
                    ptr->callback(ptr, KEY_MACHINE_EVENT_PRESS, ptr->callback_arg);
                }
            }
            break;

        case KEY_STATE_SHORT:
            // 累积按下时间，每次调用增加10ms（假设10ms轮询）
            ptr->press_time_ms += 10;
            
            // 如果引脚不再触发（释放）
            if (!is_triggered) {
                ptr->state = KEY_STATE_IDLE;
                
                if (ptr->callback) {
                    // 根据按下时间判断事件类型
                    if (ptr->press_time_ms < ptr->config.short_press_ms) {
                        // 短按事件
                        if (ptr->event_mask & KEY_EVENT_MASK_SHORT_PRESS) {
                            ptr->callback(ptr, KEY_MACHINE_EVENT_SHORT_PRESS, ptr->callback_arg);
                        }
                    } else {
                        // 单击事件（短按后释放）
                        if (ptr->event_mask & KEY_EVENT_MASK_CLICK) {
                            ptr->callback(ptr, KEY_MACHINE_EVENT_CLICK, ptr->callback_arg);
                        }
                    }
                    
                    // 释放事件
                    if (ptr->event_mask & KEY_EVENT_MASK_RELEASE) {
                        ptr->callback(ptr, KEY_MACHINE_EVENT_RELEASE, ptr->callback_arg);
                    }
                }
            } else if (ptr->press_time_ms >= ptr->config.ultra_long_press_ms) {
                // 如果按下时间达到或超过超长按阈值
                ptr->state = KEY_STATE_ULTRA_LONG;
                if (ptr->callback && (ptr->event_mask & KEY_EVENT_MASK_ULTRA_LONG_PRESS)) {
                    ptr->callback(ptr, KEY_MACHINE_EVENT_ULTRA_LONG_PRESS, ptr->callback_arg);
                }
            } else if (ptr->press_time_ms >= ptr->config.long_press_ms) {
                // 如果按下时间达到或超过长按阈值
                ptr->state = KEY_STATE_LONG;
                if (ptr->callback && (ptr->event_mask & KEY_EVENT_MASK_LONG_PRESS)) {
                    ptr->callback(ptr, KEY_MACHINE_EVENT_LONG_PRESS, ptr->callback_arg);
                }
            }
            break;

        case KEY_STATE_LONG:
            // 长按状态，继续累积时间
            ptr->press_time_ms += 10;
            
            // 如果引脚不再触发（释放）
            if (!is_triggered) {
                ptr->state = KEY_STATE_IDLE;
                if (ptr->callback && (ptr->event_mask & KEY_EVENT_MASK_RELEASE)) {
                    ptr->callback(ptr, KEY_MACHINE_EVENT_RELEASE, ptr->callback_arg);
                }
            } else if (ptr->press_time_ms >= ptr->config.ultra_long_press_ms) {
                // 升级到超长按状态
                ptr->state = KEY_STATE_ULTRA_LONG;
                if (ptr->callback && (ptr->event_mask & KEY_EVENT_MASK_ULTRA_LONG_PRESS)) {
                    ptr->callback(ptr, KEY_MACHINE_EVENT_ULTRA_LONG_PRESS, ptr->callback_arg);
                }
            }
            break;

        case KEY_STATE_ULTRA_LONG:
            // 超长按状态
            if (!is_triggered) {
                ptr->state = KEY_STATE_IDLE;
                if (ptr->callback && (ptr->event_mask & KEY_EVENT_MASK_RELEASE)) {
                    ptr->callback(ptr, KEY_MACHINE_EVENT_RELEASE, ptr->callback_arg);
                }
            }
            break;

        default:
            // 未知状态，强制切换到空闲状态
            ptr->state = KEY_STATE_IDLE;
            break;
    }
}

/**
  * @name     KeyMachine_SetTrigger
  * @brief    动态设置触发电平
  * @param    ptr: 按键状态机指针
  * @param    trigger: 触发电平
  * @retval   None
  */
void KeyMachine_SetTrigger(KeyMachinePtr_t ptr, KeyTrigger_e trigger) {
    if (ptr == NULL) return;
    
    ptr->trigger = trigger;
    ptr->config.trigger = trigger;
    
    if (ptr->gpio_drv_ptr != NULL && ptr->read_level != NULL) {
        ptr->current_level = ptr->read_level(ptr->gpio_drv_ptr);
    }
}

/**
  * @name     KeyMachine_SetDebounceMs
  * @brief    动态设置去抖时间
  * @param    ptr: 按键状态机指针
  * @param    debounce_ms: 去抖时间（毫秒）
  * @retval   None
  */
void KeyMachine_SetDebounceMs(KeyMachinePtr_t ptr, uint32_t debounce_ms) {
    if (ptr == NULL) return;
    
    ptr->config.debounce_ms = debounce_ms > 0 ? debounce_ms : 20;
    ptr->debounce_count = 0;
}

/**
  * @name     KeyMachine_SetEventMask
  * @brief    动态设置事件掩码
  * @param    ptr: 按键状态机指针
  * @param    event_mask: 事件掩码
  * @retval   None
  */
void KeyMachine_SetEventMask(KeyMachinePtr_t ptr, uint32_t event_mask) {
    if (ptr == NULL) return;
    
    ptr->event_mask = event_mask;
}

/**
  * @name     KeyMachine_GetLevel
  * @brief    获取当前按键电平
  * @param    ptr: 按键状态机指针
  * @retval   uint8_t: 当前电平（0 或 1）
  */
uint8_t KeyMachine_GetLevel(KeyMachinePtr_t ptr) {
    if (ptr == NULL || ptr->gpio_drv_ptr == NULL || ptr->read_level == NULL) {
        return 0;
    }
    
    return ptr->read_level(ptr->gpio_drv_ptr);
}

/**
  * @name     KeyMachine_GetState
  * @brief    获取当前按键状态
  * @param    ptr: 按键状态机指针
  * @retval   KeyState_e: 当前状态
  */
KeyState_e KeyMachine_GetState(KeyMachinePtr_t ptr) {
    if (ptr == NULL) {
        return KEY_STATE_IDLE;
    }
    
    return ptr->state;
}

/**
  * @name     KeyMachine_Initialize
  * @brief    初始化按键状态机
  * @param    ptr: 按键状态机指针
  * @param    config: 按键配置
  * @param    callback: 事件回调函数
  * @param    arg: 回调参数
  * @retval   None
  */
void KeyMachine_Initialize(KeyMachinePtr_t ptr, 
                          const KeyConfig_t* config,
                          KeyCallback_t callback, 
                          void* arg) {
    if (ptr == NULL) return;
    
    // 使用默认配置或用户配置
    if (config != NULL) {
        ptr->config = *config;
    } else {
        ptr->config = KEY_CONFIG_DEFAULT;
    }
    
    ptr->trigger = ptr->config.trigger;
    ptr->event_mask = ptr->config.event_mask;
    ptr->state = KEY_STATE_IDLE;
    ptr->current_level = (ptr->gpio_drv_ptr && ptr->read_level) ? ptr->read_level(ptr->gpio_drv_ptr) : 0;
    ptr->last_level = ptr->current_level;
    ptr->debounce_count = 0;
    ptr->press_time_ms = 0;
    ptr->callback = callback;
    ptr->callback_arg = arg;
}

/**
  * @name     KeyMachine_Configure
  * @brief    配置按键状态机函数指针
  * @param    ptr: 按键状态机指针
  * @retval   None
  */
void KeyMachine_Configure(KeyMachinePtr_t ptr) {
    if (ptr == NULL) return;

    // 设置为默认值
    ptr->debounce_count = 0;
    ptr->press_time_ms = 0;
    ptr->state = KEY_STATE_IDLE;
    ptr->config = KEY_CONFIG_DEFAULT;
    ptr->trigger = ptr->config.trigger;
    ptr->event_mask = ptr->config.event_mask;
    ptr->callback = NULL;
    ptr->callback_arg = NULL;
}

/**
 * @brief 设置驱动层指针和函数指针
 * @param ptr 按键状态机指针
 * @param gpio_drv_ptr 驱动层指针
 * @param read_level_func 读取电平函数指针
 */
void KeyMachine_SetDriverInterface(KeyMachinePtr_t ptr, 
                                   void* gpio_drv_ptr,
                                   KeyReadLevelFunc_t read_level_func) {
    if (ptr == NULL) return;
    
    ptr->gpio_drv_ptr = gpio_drv_ptr;
    ptr->read_level = read_level_func;
    
    // 设置内部函数指针
    ptr->configure = KeyMachine_InternalConfigure;
    ptr->initialize = KeyMachine_InternalInitialize;
    ptr->fsm = KeyMachine_InternalFSM;
    ptr->set_trigger = KeyMachine_InternalSetTrigger;
    ptr->set_debounce_ms = KeyMachine_InternalSetDebounceMs;
    ptr->set_event_mask = KeyMachine_InternalSetEventMask;
    ptr->get_level = KeyMachine_InternalGetLevel;
    ptr->get_state = KeyMachine_InternalGetState;
}

// ==================== 内部函数实现 ====================
static void KeyMachine_InternalFSM(KeyMachinePtr_t ptr) {
    KeyMachine_FSM(ptr);
}

static void KeyMachine_InternalSetTrigger(KeyMachinePtr_t ptr, KeyTrigger_e trigger) {
    KeyMachine_SetTrigger(ptr, trigger);
}

static void KeyMachine_InternalSetDebounceMs(KeyMachinePtr_t ptr, uint32_t debounce_ms) {
    KeyMachine_SetDebounceMs(ptr, debounce_ms);
}

static void KeyMachine_InternalSetEventMask(KeyMachinePtr_t ptr, uint32_t event_mask) {
    KeyMachine_SetEventMask(ptr, event_mask);
}

static uint8_t KeyMachine_InternalGetLevel(KeyMachinePtr_t ptr) {
    return KeyMachine_GetLevel(ptr);
}

static KeyState_e KeyMachine_InternalGetState(KeyMachinePtr_t ptr) {
    return KeyMachine_GetState(ptr);
}

static void KeyMachine_InternalInitialize(KeyMachinePtr_t ptr, const KeyConfig_t* config, KeyCallback_t callback, void* arg) {
    KeyMachine_Initialize(ptr, config, callback, arg);
}

static void KeyMachine_InternalConfigure(KeyMachinePtr_t ptr) {
    KeyMachine_Configure(ptr);
}

// ==================== 全局按键管理器实现 ====================

/**
 * @brief 初始化全局按键管理器
 */
void KeyManager_Init(void) {
    g_key_manager.head = NULL;
    g_key_manager.count = 0;
    g_key_manager.tick_ms = 0;
}

/**
 * @brief 处理所有按键状态机
 */
void KeyManager_Process(void) {
    KeyMachinePtr_t ptr = g_key_manager.head;
    
    while (ptr != NULL) {
        KeyMachine_FSM(ptr);
        ptr = ptr->next;
    }
}

/**
 * @brief 创建新的按键状态机（支持静态实例）
 * @param config 按键配置
 * @param callback 回调函数
 * @param arg 回调参数
 * @return 按键状态机指针
 * @note 此函数现在支持静态实例，调用者需要提供预分配的实例
 */
KeyMachinePtr_t KeyManager_CreateKey(const KeyConfig_t* config, 
                                     KeyCallback_t callback, 
                                     void* arg) {
    // 注意：此函数现在主要用于兼容性
    // 实际使用中应该直接使用 KeyMachine_Initialize 和静态实例
    return NULL; // 返回NULL表示不支持动态分配
}

/**
 * @brief 销毁按键状态机（支持静态实例）
 * @param ptr 按键状态机指针
 * @note 对于静态实例，此函数只从链表中移除，不释放内存
 */
void KeyManager_DestroyKey(KeyMachinePtr_t ptr) {
    if (ptr == NULL) return;
    
    // 从链表中移除
    if (g_key_manager.head == ptr) {
        g_key_manager.head = ptr->next;
    } else {
        KeyMachinePtr_t prev = g_key_manager.head;
        while (prev != NULL && prev->next != ptr) {
            prev = prev->next;
        }
        if (prev != NULL) {
            prev->next = ptr->next;
        }
    }
    
    g_key_manager.count--;
    
    // 注意：对于静态实例，不需要调用free()
    // 只有动态分配的实例才需要释放内存
}

/**
 * @brief 注册静态按键实例到管理器
 * @param ptr 静态按键实例指针
 */
void KeyManager_RegisterStaticKey(KeyMachinePtr_t ptr) {
    if (ptr == NULL) return;
    
    // 添加到链表头部
    ptr->next = g_key_manager.head;
    g_key_manager.head = ptr;
    g_key_manager.count++;
}

/**
 * @brief 更新系统滴答
 * @param tick_ms 系统滴答（毫秒）
 */
void KeyManager_UpdateTick(uint32_t tick_ms) {
    g_key_manager.tick_ms = tick_ms;
}

/*---End of File----------------------------------------------------*/