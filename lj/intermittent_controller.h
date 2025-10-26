/****************************************************************************
 * 文件名: intermittent_controller.h
 * 功能: 间歇模式控制器
 * 
 * 说明: 
 *   实现间歇模式的3阶段控制
 *   - 阶段0：高压阶段（抽气到目标压力）
 *   - 阶段1：低压阶段（降压到低压值）
 *   - 阶段2：停顿阶段（停止工作一段时间）
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef INTERMITTENT_CONTROLLER_H
#define INTERMITTENT_CONTROLLER_H

#include "../../Core/Inc/system_types.h"

/**
 * 函数: IntermittentController_Init
 * 功能: 初始化间歇模式控制器
 */
void IntermittentController_Init(IntermittentControl_t *ctrl);

/**
 * 函数: IntermittentController_Start
 * 功能: 启动间歇模式
 */
void IntermittentController_Start(IntermittentControl_t *ctrl);

/**
 * 函数: IntermittentController_Stop
 * 功能: 停止间歇模式
 */
void IntermittentController_Stop(IntermittentControl_t *ctrl);

/**
 * 函数: IntermittentController_Update
 * 功能: 更新间歇模式（主循环调用）
 * 说明: 处理3阶段切换逻辑
 */
void IntermittentController_Update(IntermittentControl_t *ctrl, 
                                   PressureControl_t *pressure);

/**
 * 函数: IntermittentController_GetPhase
 * 功能: 获取当前阶段
 */
IntermittentPhase_e IntermittentController_GetPhase(const IntermittentControl_t *ctrl);

#endif /* INTERMITTENT_CONTROLLER_H */

