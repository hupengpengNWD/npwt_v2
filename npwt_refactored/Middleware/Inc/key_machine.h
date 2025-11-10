/**
  ******************************************************************************
  * @file:    key_machine.h
  * @author:  hupengpeng
  * @date:    2025-01-22
  * @brief:   按键状态机模块 - 基于lib_gpio_input重构
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#ifndef KEY_MACHINE_H
#define KEY_MACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "../../HAL/Inc/hal_gpio.h"

/* 按键触发电平 */
typedef enum {
    KEY_TRIGGER_LOW = 0,   // 低电平触发
    KEY_TRIGGER_HIGH,      // 高电平触发
    KEY_TRIGGER_BOTH,      // 双边沿触发
    KEY_TRIGGER_COUNT      // 触发电平总数
} KeyTrigger_e;

/* 按键状态 */
typedef enum {
    KEY_STATE_IDLE = 0,    // 未触发
    KEY_STATE_SHORT,       // 短按
    KEY_STATE_LONG,        // 长按
    KEY_STATE_ULTRA_LONG,  // 超长按
    KEY_STATE_COUNT        // 状态总数
} KeyState_e;
  
/* 按键事件 */
typedef enum {
    KEY_MACHINE_EVENT_NONE = 0,        // 无事件
    KEY_MACHINE_EVENT_LONG_PRESS = 4,  // 长按事件：按键按下时间达到 long_press_ms（默认1000ms），用于开机、特殊功能触发
    KEY_MACHINE_EVENT_ULTRA_LONG_PRESS = 8, // 超长按事件：按键按下时间达到 ultra_long_press_ms（默认3000ms），用于关机、紧急功能
    KEY_MACHINE_EVENT_LONG_PRESS_RELEASE = 16,    // 长按释放事件：长按后释放时触发
    KEY_MACHINE_EVENT_ULTRA_LONG_PRESS_RELEASE = 32  // 超长按释放事件：超长按后释放时触发
} KeyMachineEvent_e;

/* 按键事件掩码 */
typedef enum {
    KEY_EVENT_MASK_NONE        = 0x00,
    KEY_EVENT_MASK_LONG_PRESS  = 0x04,
    KEY_EVENT_MASK_ULTRA_LONG_PRESS = 0x08,
    KEY_EVENT_MASK_LONG_PRESS_RELEASE = 0x10,
    KEY_EVENT_MASK_ULTRA_LONG_PRESS_RELEASE = 0x20,
    KEY_EVENT_MASK_ALL         = 0x3C  // 0x04 | 0x08 | 0x10 | 0x20
} KeyEventMask_e;

/* 按键配置参数 */
typedef struct {
    uint32_t debounce_ms;      // 去抖时间（毫秒）
    uint32_t short_press_ms;   // 短按阈值（毫秒）
    uint32_t long_press_ms;    // 长按阈值（毫秒）
    uint32_t ultra_long_press_ms; // 超长按阈值（毫秒）
    KeyTrigger_e trigger;       // 触发电平
    uint32_t event_mask;       // 事件掩码
} KeyConfig_t;

struct key_machine;

typedef void (*KeyCallback_t)(struct key_machine*, KeyMachineEvent_e, void*);

// ==================== 硬件GPIO读取函数指针类型 ====================
typedef uint8_t (*KeyReadLevelFunc_t)(void* gpio_drv_ptr);  /*!< 读取电平函数指针类型 */

#pragma pack(1)
typedef struct key_machine {
    // 状态机相关
    KeyState_e state;           // 当前状态
    uint8_t current_level;      // 当前电平
    uint8_t last_level;         // 上次电平
    uint32_t debounce_count;    // 去抖计数
    uint32_t press_time_ms;     // 按下时间（毫秒）
    KeyTrigger_e trigger;       // 触发电平
    uint32_t event_mask;        // 事件掩码
    
    // 配置参数
    KeyConfig_t config;         // 按键配置
    
    // 回调相关
    KeyCallback_t callback;     // 回调函数
    void* callback_arg;         // 回调参数
    
    // 硬件接口
    void* gpio_drv_ptr;         // 指向底层GPIO硬件驱动的指针
    KeyReadLevelFunc_t read_level; // 读取电平函数指针
    
    // 链表指针
    struct key_machine* next;   // 链表指针

    // 函数指针接口
    void (*initialize)(struct key_machine*, const KeyConfig_t*, KeyCallback_t, void*);
    void (*configure)(struct key_machine*);
    void (*fsm)(struct key_machine*);
    void (*set_trigger)(struct key_machine*, KeyTrigger_e);
    void (*set_debounce_ms)(struct key_machine*, uint32_t);
    void (*set_event_mask)(struct key_machine*, uint32_t);
    uint8_t (*get_level)(struct key_machine*);
    KeyState_e (*get_state)(struct key_machine*);
    
} KeyMachine_t, *KeyMachinePtr_t;
#pragma pack()

// ==================== 全局按键管理器 ====================
typedef struct {
    KeyMachinePtr_t head;       // 按键链表头
    uint32_t count;             // 按键数量
    uint32_t tick_ms;           // 系统滴答（毫秒）
} KeyManager_t;

// ==================== 基本功能函数 ====================
void KeyMachine_FSM(KeyMachinePtr_t ptr);
void KeyMachine_SetTrigger(KeyMachinePtr_t ptr, KeyTrigger_e trigger);
void KeyMachine_SetDebounceMs(KeyMachinePtr_t ptr, uint32_t debounce_ms);
void KeyMachine_SetEventMask(KeyMachinePtr_t ptr, uint32_t event_mask);
uint8_t KeyMachine_GetLevel(KeyMachinePtr_t ptr);
KeyState_e KeyMachine_GetState(KeyMachinePtr_t ptr);
void KeyMachine_Initialize(KeyMachinePtr_t ptr, 
                          const KeyConfig_t* config,
                          KeyCallback_t callback, 
                          void* arg);
void KeyMachine_Configure(KeyMachinePtr_t ptr);

/**
 * @brief 设置驱动层指针和函数指针
 * @param ptr 按键状态机指针
 * @param gpio_drv_ptr 驱动层指针
 * @param read_level_func 读取电平函数指针
 */
void KeyMachine_SetDriverInterface(KeyMachinePtr_t ptr, 
                                   void* gpio_drv_ptr,
                                   KeyReadLevelFunc_t read_level_func);

// ==================== 全局按键管理器函数 ====================
void KeyManager_Init(void);
void KeyManager_Process(void);
KeyMachinePtr_t KeyManager_CreateKey(const KeyConfig_t* config, 
                                     KeyCallback_t callback, 
                                     void* arg);
void KeyManager_DestroyKey(KeyMachinePtr_t ptr);
void KeyManager_RegisterStaticKey(KeyMachinePtr_t ptr);
void KeyManager_UpdateTick(uint32_t tick_ms);

// ==================== 默认配置 ====================
extern const KeyConfig_t KEY_CONFIG_DEFAULT;

#ifdef __cplusplus
}
#endif

#endif /* KEY_MACHINE_H */