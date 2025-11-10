/**
  ******************************************************************************
  * @file:    lib_finite_state_machine.c
  * @author:  hupengepng
  * @date:    
  * @email:   hupengpengHPP@outlook.com
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fsm.h"

/* 全局变量 */
static uint8_t fsm_event_queue_store[FSM_ID_COUNT][sizeof(st_fsm_event) * FSM_QUEUE_LENGTH];
static uint8_t fsm_event_queue_store2[FSM_ID_COUNT][sizeof(st_fsm_event) * FSM_QUEUE_LENGTH];
static st_queue fsm_event_queue[FSM_ID_COUNT];
static st_queue fsm_event_queue2[FSM_ID_COUNT];

extern  const st_fsm_transition g_ui_transition_table[11];
/**
  * @name     fsm_transit
  * @brief    状态转换函数
  * @param    ptr: FSM 指针
  * @param    state: 目标状态
  * @return   None
  * @remark   内部函数，执行状态转换
  */
static void fsm_transit(st_fsm_ptr ptr, uint8_t state) {
    ptr->last_state = ptr->current_state;
    ptr->current_state = state;
}

/**
  * @name     fsm_poll
  * @brief    FSM 事件轮询处理
  * @param    ptr: FSM 指针
  * @param    event: 事件
  * @return   None
  * @remark   处理事件并执行状态转换
  */
void fsm_poll(st_fsm_ptr ptr, st_fsm_event event) {
    void (*action_func)(void*, st_fsm_event) = NULL;
    uint8_t next_state = 0;
    uint8_t found = 0;
    uint8_t trigger_event = 0;
    uint8_t trans_table_current_state = 0;
    
    /* 遍历状态转换表 */
    for (uint8_t i = 0; i < ptr->trans_size; i++) {
        
        trigger_event = g_ui_transition_table[i].trigger_event;
        trans_table_current_state = g_ui_transition_table[i].current_state;
                
//        if (event.event_type == ptr->trans_table[i].trigger_event && ptr->current_state == ptr->trans_table[i].current_state) {
          if (event.event_type == trigger_event && ptr->current_state == trans_table_current_state) {  
            found = 1;
//            action_func = ptr->trans_table[i].action_func;
//            next_state = ptr->trans_table[i].next_state;
            action_func = g_ui_transition_table[i].action_func;
            next_state = g_ui_transition_table[i].next_state;
            break;
        }
    }
    
    /* 执行状态转换 */
    if (found) {
        if (action_func != NULL) {
            action_func(ptr->user_arg, event); /* 执行相应动作 */
        }
        fsm_transit(ptr, next_state); /* 状态转换 */
    }
}

/**
  * @name     fsm_initialize
  * @brief    初始化 FSM 结构体
  * @param    ptr: FSM 指针
  * @return   None
  * @remark   初始化事件队列和用户参数
  */
void fsm_initialize(st_fsm_ptr ptr) {
//    ptr->user_arg = ptr;
    
    /* 初始化事件队列1 */
    ptr->event_course_queue->initialize(
        ptr->event_course_queue,
        FSM_QUEUE_LENGTH,
        sizeof(st_fsm_event),
        fsm_event_queue_store[ptr->fsm_id]
    );
    
    /* 初始化事件队列2 */
    ptr->event_trigger_queue->initialize(
        ptr->event_trigger_queue,
        FSM_QUEUE_LENGTH,
        sizeof(st_fsm_event),
        fsm_event_queue_store2[ptr->fsm_id]
    );
}

/**
  * @name     fsm_configure
  * @brief    配置 FSM 函数指针
  * @param    ptr: FSM 指针
  * @return   None
  * @remark   设置所有函数指针
  */
void fsm_configure(st_fsm_ptr ptr) {
    ptr->initialize = fsm_initialize;
    ptr->poll = fsm_poll;
    ptr->transit = fsm_transit;
    
    ptr->event_course_queue->configure(ptr->event_course_queue);
    ptr->event_trigger_queue->configure(ptr->event_trigger_queue);
}

/**
  * @name     fsm_create
  * @brief    创建 FSM 实例
  * @param    ptr: FSM 指针
  * @return   uint8_t: 0 成功, 1 失败
  * @remark   创建并配置 FSM 实例
  */
uint8_t fsm_create(st_fsm_ptr ptr) {
    if (ptr->fsm_id >= FSM_ID_COUNT) {
        return 1;
    }
    
    ptr->configure = fsm_configure;
    ptr->event_course_queue = &fsm_event_queue[ptr->fsm_id];
    ptr->event_trigger_queue = &fsm_event_queue2[ptr->fsm_id];
    
    lib_queue_create(ptr->event_course_queue);
    lib_queue_create(ptr->event_trigger_queue);
    
    return 0;
}

