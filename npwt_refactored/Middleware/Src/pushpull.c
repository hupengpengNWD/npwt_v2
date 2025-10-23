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
    if (ptr == NULL || ptr->state == state) {
        return;
    }

    switch (state) {
        case PUSHPULL_STATE_LOW:
            if (ptr->write_low && ptr->gpio_drv_ptr) {
                ptr->write_low(ptr->gpio_drv_ptr);
                ptr->state = PUSHPULL_STATE_LOW;
                if (ptr->callback) {
                    ptr->callback(ptr, PUSHPULL_EVENT_STATE_CHANGE, ptr->callback_arg);
                }
            }
            break;
        case PUSHPULL_STATE_HIGH:
            if (ptr->write_high && ptr->gpio_drv_ptr) {
                ptr->write_high(ptr->gpio_drv_ptr);
                ptr->state = PUSHPULL_STATE_HIGH;
                if (ptr->callback) {
                    ptr->callback(ptr, PUSHPULL_EVENT_STATE_CHANGE, ptr->callback_arg);
                }
            }
            break;
        default:
            break;
    }
}

/**
 * @name      PushPull_FSM
 * @brief     时序序列状态机（支持一维和二维数组）
 * @param     ptr - 蜂鸣器控制指针
 * @retval    无
 * @remark   
 */
void PushPull_FSM(PushPullPtr_t ptr) {
    if (ptr == NULL || ptr->run_sequence == NULL) {
        return;
    }
    
    // 优先使用二维数组模式
    if (ptr->seq_2d_array != NULL && ptr->seq_2d_count > 0) {
        // 二维数组模式
        if (ptr->seq_2d_index >= ptr->seq_2d_count) {
            return; // 超出范围
        }
        
        const uint16_t* current_seq = ptr->seq_2d_array[ptr->seq_2d_index];
        if (current_seq == NULL) {
            return;
        }
        
        // 计算当前序列长度（假设每行都是2个元素：开、关）
        uint32_t current_seq_length = 2;
        
        if (ptr->tick >= current_seq[ptr->seq_index]) {
            ptr->tick = 0;
            ptr->run_sequence(ptr, (ptr->seq_index % 2) ? PUSHPULL_STATE_HIGH : PUSHPULL_STATE_LOW);
            ptr->seq_index++;
            if (ptr->seq_index >= current_seq_length) {
                ptr->seq_index = 0;
                // 当前行完成，检查是否需要重复
                if (ptr->seq_2d_repeat_count > 0) {
                    ptr->seq_2d_repeat_count--;
                    if (ptr->seq_2d_repeat_count == 0) {
                        // 当前行重复完成，切换到下一行
                        ptr->seq_2d_index++;
                        ptr->seq_2d_repeat_count = 1; // 重置重复次数
                        if (ptr->seq_2d_index >= ptr->seq_2d_count) {
                            // 所有行完成，重新开始
                            ptr->seq_2d_index = 0;
                        }
                    }
                } else {
                    // seq_2d_repeat_count = 0 表示无限循环，不自动切换模式
                    // 模式切换需要通过外部调用 PushPull_Set2DSequence 来实现
                }
            }
        } else {
            ptr->tick++;
        }
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
        ptr->tick = 0;
    }
    // 注意：对于 PUSHPULL_MODE_SEQUENCE 模式，不重置执行状态
    // 因为 PushPull_SetSequence() 和 PushPull_Set2DSequence() 已经正确设置了状态
}


/**
 * @name      PushPull_Set2DSequence
 * @brief     动态设置二维时序序列
 * @param     ptr - 蜂鸣器控制指针
 * @param     seq_2d_array - 二维时序数组指针
 * @param     seq_2d_count - 二维数组行数
 * @param     seq_2d_repeat_count - 每行重复次数
 * @retval    无
 * @remark   
 */
void PushPull_Set2DSequence(PushPullPtr_t ptr, 
                           const uint16_t** seq_2d_array, 
                           uint32_t seq_2d_count, 
                           uint32_t seq_2d_repeat_count) {
    ptr->seq_index = 0;
    ptr->tick = 0;
    ptr->mode = PUSHPULL_MODE_SEQUENCE;
    ptr->seq_2d_array = seq_2d_array;
    ptr->seq_2d_count = seq_2d_count;
    ptr->seq_2d_index = 0;
    ptr->seq_2d_repeat_count = seq_2d_repeat_count;
}

/**
 * @name      PushPull_Initialize
 * @brief     初始化蜂鸣器控制结构体
 * @param     ptr - 蜂鸣器控制指针
 * @param     callback - 事件回调函数
 * @param     arg - 回调参数
 * @retval    无
 * @remark   
 */
void PushPull_Initialize(PushPullPtr_t ptr, 
                        void (*callback)(struct pushpull*, PushPullEvent_e, void*), 
                        void* arg) {
    if (ptr == NULL) {
        return;
    }
    
    ptr->seq_index = 0;
    ptr->tick = 0;
    ptr->mode = PUSHPULL_MODE_MANUAL;
    ptr->state = PUSHPULL_STATE_LOW;
    ptr->callback = callback;
    ptr->callback_arg = arg;
    ptr->next = NULL;

    // 初始化二维数组字段
    ptr->seq_2d_array = NULL;
    ptr->seq_2d_count = 0;
    ptr->seq_2d_index = 0;
    ptr->seq_2d_repeat_count = 0;

    // 初始化函数指针
    PushPull_Configure(ptr);
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
    ptr->set_2d_sequence = PushPull_Set2DSequence;
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
