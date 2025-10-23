/**
  ******************************************************************************
  * @file:    pushpull.c
  * @author:  Assistant
  * @date:    2025-01-23
  * @brief:   蜂鸣器控制组件实现（基于lib_gpio_output架构）
  ******************************************************************************
  * @attention
  * 
  ******************************************************************************
  */

#include "pushpull.h"

// 定时器回调
void PushPull_Polling(void* arg) {
    PushPullPtr_t ptr = (PushPullPtr_t)arg;
    ptr->fsm(ptr);
}

/**
 * @name      PushPull_RunSequence
 * @brief     执行序列中的指定状态
 * @param     ptr - 蜂鸣器控制指针
 * @param     state - 目标状态（高/低）
 * @retval    无
 * @remark   
 */
void PushPull_RunSequence(PushPullPtr_t ptr, PushPullState_e state) {
    if (ptr->state == state) {
        return;
    }

    switch (state) {
        case PUSHPULL_STATE_LOW:
            if (ptr->write_low && ptr->gpio_drv_ptr) {
                ptr->write_low(ptr->gpio_drv_ptr);
                ptr->state = PUSHPULL_STATE_LOW;
                ptr->callback(ptr, PUSHPULL_EVENT_STATE_CHANGE, ptr->callback_arg);
            }
            break;
        case PUSHPULL_STATE_HIGH:
            if (ptr->write_high && ptr->gpio_drv_ptr) {
                ptr->write_high(ptr->gpio_drv_ptr);
                ptr->state = PUSHPULL_STATE_HIGH;
                ptr->callback(ptr, PUSHPULL_EVENT_STATE_CHANGE, ptr->callback_arg);
            }
            break;
        default:
            break;
    }
}

/**
 * @name      PushPull_FSM
 * @brief     时序序列状态机
 * @param     ptr - 蜂鸣器控制指针
 * @retval    无
 * @remark   
 */
void PushPull_FSM(PushPullPtr_t ptr) {
    if (ptr->tick >= ptr->seq_array[ptr->seq_index]) {
        ptr->tick = 0;
        ptr->run_sequence(ptr, (ptr->seq_index % 2) ? PUSHPULL_STATE_LOW : PUSHPULL_STATE_HIGH);
        ptr->seq_index++;
        if (ptr->seq_index >= ptr->seq_length) {
            ptr->seq_index = 0;
            ptr->seq_count--;
            if (ptr->seq_count == 0) {
                if (ptr->callback) {
                    ptr->callback(ptr, PUSHPULL_EVENT_SEQUENCE_DONE, ptr->callback_arg);
                }
            }
        }
    } else {
        ptr->tick++;
    }
}

/**
 * @name      PushPull_SetMode
 * @brief     动态设置工作模式
 * @param     ptr - 蜂鸣器控制指针
 * @param     mode - 工作模式（手动/序列）
 * @retval    无
 * @remark   
 */
void PushPull_SetMode(PushPullPtr_t ptr, PushPullMode_e mode) {
    ptr->mode = mode;
    if (mode == PUSHPULL_MODE_MANUAL) { 
        ptr->seq_index = 0;
        ptr->seq_count = 0;
        ptr->tick = 0;
    } else if (mode == PUSHPULL_MODE_SEQUENCE && 
               ptr->seq_array && 
               ptr->seq_length > 0 && 
               ptr->seq_count > 0) {
    }
}

/**
 * @name      PushPull_SetSequence
 * @brief     动态设置时序序列
 * @param     ptr - 蜂鸣器控制指针
 * @param     seq_array - 时序数组（毫秒）
 * @param     seq_length - 数组长度
 * @param     seq_count - 重复次数
 * @retval    无
 * @remark   
 */
void PushPull_SetSequence(PushPullPtr_t ptr, 
                         const uint16_t* seq_array, 
                         uint32_t seq_length, 
                         uint32_t seq_count) {
    ptr->seq_index = 0;
    ptr->tick = 0;
    ptr->mode = PUSHPULL_MODE_SEQUENCE;
    ptr->seq_array = seq_array;
    ptr->seq_length = seq_length;
    ptr->seq_count = seq_count;
}

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
 * @remark   
 */
void PushPull_Initialize(PushPullPtr_t ptr, 
                        const uint16_t* seq_array, 
                        uint32_t seq_length, 
                        uint32_t seq_count,
                        void (*callback)(struct pushpull*, PushPullEvent_e, void*), 
                        void* arg) {
    ptr->seq_index = 0;
    ptr->tick = 0;
    ptr->mode = seq_array && seq_length > 0 && seq_count > 0 ? PUSHPULL_MODE_SEQUENCE : PUSHPULL_MODE_MANUAL;
    ptr->seq_array = seq_array;
    ptr->seq_length = seq_length;
    ptr->seq_count = seq_count;
    ptr->state = PUSHPULL_STATE_LOW;
    ptr->callback = callback;
    ptr->callback_arg = arg;
    ptr->next = NULL;
}

/**
 * @name      PushPull_Configure
 * @brief     配置蜂鸣器控制函数指针
 * @param     ptr - 蜂鸣器控制指针
 * @retval    无
 * @remark   
 */
void PushPull_Configure(PushPullPtr_t ptr) {
    ptr->run_sequence = PushPull_RunSequence;
    ptr->fsm = PushPull_FSM;
    ptr->set_mode = PushPull_SetMode;
    ptr->set_sequence = PushPull_SetSequence;
}

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
                                PushPullToggleFunc_t toggle_func) {
    if (!ptr) return;
    
    ptr->configure = PushPull_Configure;
    ptr->initialize = PushPull_Initialize;
    ptr->gpio_drv_ptr = gpio_drv_ptr;
    ptr->write_high = write_high_func;
    ptr->write_low = write_low_func;
    ptr->toggle = toggle_func;
}

/*---End of File----------------------------------------------------*/
