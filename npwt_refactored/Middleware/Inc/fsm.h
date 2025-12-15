 /****************************************************************************
  * 文件名: fsm.h
  * 功能: 
  * 作者: 韦睿医疗
  * 说明:
  *
  * 创建日期: 2025-11-07
  ****************************************************************************/
#ifndef LIB_FINITE_STATE_MACHINE_H
#define LIB_FINITE_STATE_MACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "queue.h"

/* 配置参数 */
#define FSM_QUEUE_LENGTH 8
#define FSM_MAX_INSTANCES 2

/* FSM 实例 ID */
typedef enum {
    FSM_ID_0 = 0,
    FSM_ID_1,
    FSM_ID_COUNT
} et_fsm_id;

/* 事件结构体 */
typedef struct {
    uint8_t event_type;          /* 事件类型 */
    uint8_t event_user;          /* 用户自定义 */
    uint8_t event_data[2];       /* 事件数据 */
} st_fsm_event, *st_fsm_event_ptr;

/* 前向声明 */
struct fsm;

/* 状态转换表项 */
typedef struct {
    uint8_t current_state;       /* 当前状态 */
    uint8_t trigger_event;       /* 触发事件 */
    void (*action_func)(void*, st_fsm_event); /* 动作函数 */
    uint8_t next_state;          /* 下一状态 */
} st_fsm_transition, *st_fsm_transition_ptr;

/* FSM 结构体 */
typedef struct fsm {
    et_fsm_id fsm_id;                    /* FSM ID */
    const st_fsm_transition* trans_table; /* 状态转换表 */
    uint8_t trans_size;                  /* 转换表大小 */
    uint8_t current_state;               /* 当前状态 */
    uint8_t last_state;                  /* 历史状态 */
    st_queue_ptr event_course_queue;     /* 事件队列1 */
    st_queue_ptr event_trigger_queue;    /* 事件队列2 */
    void* user_arg;                      /* 用户参数 */
    
    /* 函数指针 */
    void (*configure)(struct fsm*);
    void (*initialize)(struct fsm*);
    void (*poll)(struct fsm*, st_fsm_event);
    void (*transit)(struct fsm*, uint8_t);
} st_fsm, *st_fsm_ptr;

//#pragma pack()

/* 函数声明 */
uint8_t fsm_create(st_fsm_ptr ptr);
void fsm_poll(st_fsm_ptr ptr, st_fsm_event event);
void fsm_initialize(st_fsm_ptr ptr);
void fsm_configure(st_fsm_ptr ptr);

#ifdef __cplusplus
}
#endif

#endif /* LIB_FINITE_STATE_MACHINE_H */

