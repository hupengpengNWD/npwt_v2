/**
  ******************************************************************************
  * @file:    pushpull.h
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   蜂鸣器控制组件头文件（基于lib_gpio_output架构）
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#ifndef PUSHPULL_H
#define PUSHPULL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/* GPIO 输出状态 */
typedef enum {
    PUSHPULL_STATE_LOW = 0,   // 低电平
    PUSHPULL_STATE_HIGH,      // 高电平
    PUSHPULL_STATE_COUNT      // 状态总数
} PushPullState_e;

typedef enum {
    PUSHPULL_MODE_MANUAL,     // 手动控制（高/低）
    PUSHPULL_MODE_SEQUENCE,   // 时序序列模式
} PushPullMode_e;

typedef enum {
    PUSHPULL_EVENT_STATE_CHANGE, // 状态切换
    PUSHPULL_EVENT_SEQUENCE_DONE // 序列完成
} PushPullEvent_e;

struct pushpull;

typedef void (*PushPullCallback_t)(struct pushpull*, PushPullEvent_e, void*);

// ==================== 硬件GPIO函数指针类型 ====================
typedef void (*PushPullWriteHighFunc_t)(void* gpio_drv_ptr);  /*!< 写高电平函数指针类型 */
typedef void (*PushPullWriteLowFunc_t)(void* gpio_drv_ptr);   /*!< 写低电平函数指针类型 */
typedef void (*PushPullToggleFunc_t)(void* gpio_drv_ptr);      /*!< 翻转引脚函数指针类型 */

#pragma pack(1)
typedef struct pushpull {
    PushPullMode_e mode;           /*!< 工作模式 */
    PushPullState_e state;         /*!< 当前状态 */
    uint32_t tick;                 /*!< 当前计时 */
    const uint16_t* seq_array;     /*!< 时序数组（毫秒） */
    uint32_t seq_length;           /*!< 数组长度 */
    uint32_t seq_index;            /*!< 当前索引 */
    uint32_t seq_count;            /*!< 剩余重复次数 */
    PushPullCallback_t callback;   /*!< 回调函数 */
    void* callback_arg;            /*!< 回调参数 */
    void* gpio_drv_ptr;            /*!< 指向底层GPIO硬件驱动的指针 */
    struct pushpull* next;         /*!< 链表指针 */
    
    // 驱动层函数指针接口
    PushPullWriteHighFunc_t write_high;   /*!< 写高电平函数指针 */ 
    PushPullWriteLowFunc_t write_low;     /*!< 写低电平函数指针 */
    PushPullToggleFunc_t toggle;          /*!< 翻转引脚函数指针 */

    void (*initialize)(struct pushpull*, const uint16_t*, uint32_t, uint32_t, void (*)(struct pushpull*, PushPullEvent_e, void*), void*);
    void (*configure)(struct pushpull*);
    void (*set_high)(struct pushpull*);
    void (*set_low)(struct pushpull*);
    void (*run_sequence)(struct pushpull*, PushPullState_e);
    void (*fsm)(struct pushpull*);
    void (*set_mode)(struct pushpull*, PushPullMode_e);
    void (*set_sequence)(struct pushpull*, const uint16_t*, uint32_t, uint32_t);
} PushPull_t, *PushPullPtr_t;
#pragma pack()

/**
 * @name      PushPull_Initialize
 * @brief     初始化蜂鸣器控制结构体
 * @param     ptr - 蜂鸣器控制指针
 * @param     seq_array - 时序数组（毫秒）
 * @param     seq_length - 数组长度
 * @param     seq_count - 重复次数
 * @param     callback - 事件回调函数
 * @param     arg - 回调参数
 * @retval    无
 */
void PushPull_Initialize(PushPullPtr_t ptr, 
                        const uint16_t* seq_array, 
                        uint32_t seq_length, 
                        uint32_t seq_count,
                        void (*callback)(struct pushpull*, PushPullEvent_e, void*), 
                        void* arg);

/**
 * @name      PushPull_Configure
 * @brief     配置蜂鸣器控制函数指针
 * @param     ptr - 蜂鸣器控制指针
 * @retval    无
 */
void PushPull_Configure(PushPullPtr_t ptr);

/**
 * @name      PushPull_RunSequence
 * @brief     执行序列中的指定状态
 * @param     ptr - 蜂鸣器控制指针
 * @param     state - 目标状态（高/低）
 * @retval    无
 */
void PushPull_RunSequence(PushPullPtr_t ptr, PushPullState_e state);

/**
 * @name      PushPull_FSM
 * @brief     时序序列状态机
 * @param     ptr - 蜂鸣器控制指针
 * @retval    无
 */
void PushPull_FSM(PushPullPtr_t ptr);

/**
 * @name      PushPull_SetMode
 * @brief     动态设置工作模式
 * @param     ptr - 蜂鸣器控制指针
 * @param     mode - 工作模式（手动/序列）
 * @retval    无
 */
void PushPull_SetMode(PushPullPtr_t ptr, PushPullMode_e mode);

/**
 * @name      PushPull_SetSequence
 * @brief     动态设置时序序列
 * @param     ptr - 蜂鸣器控制指针
 * @param     seq_array - 时序数组（毫秒）
 * @param     seq_length - 数组长度
 * @param     seq_count - 重复次数
 * @retval    无
 */
void PushPull_SetSequence(PushPullPtr_t ptr, 
                         const uint16_t* seq_array, 
                         uint32_t seq_length, 
                         uint32_t seq_count);

/**
 * @name      PushPull_SetDriverInterface
 * @brief     设置驱动层指针和函数指针
 * @param     ptr - 蜂鸣器控制库指针
 * @param     gpio_drv_ptr - 驱动层指针
 * @param     write_high_func - 写高电平函数指针
 * @param     write_low_func - 写低电平函数指针
 * @param     toggle_func - 翻转引脚函数指针
 * @retval    无
 */
void PushPull_SetDriverInterface(PushPullPtr_t ptr, 
                                void* gpio_drv_ptr,
                                PushPullWriteHighFunc_t write_high_func,
                                PushPullWriteLowFunc_t write_low_func,
                                PushPullToggleFunc_t toggle_func);

/**
 * @name      PushPull_Polling
 * @brief     蜂鸣器控制轮询函数
 * @param     arg - 蜂鸣器控制库指针
 * @retval    无
 */
void PushPull_Polling(void* arg);

#ifdef __cplusplus
}
#endif

#endif /* PUSHPULL_H */
