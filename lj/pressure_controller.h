/****************************************************************************
 * 文件名: pressure_controller.h
 * 功能: 压力控制器
 * 
 * 说明: 
 *   实现双位控制算法（Bang-Bang Control）
 *   自动调节气泵和阀门，维持目标压力
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef PRESSURE_CONTROLLER_H
#define PRESSURE_CONTROLLER_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_types.h"

/****************************************************************************
 * 压力控制器操作函数
 ****************************************************************************/

/**
 * 函数: PressureController_Init
 * 功能: 初始化压力控制器
 * 参数: data - 压力数据指针
 */
void PressureController_Init(PressureData_t *data);

/**
 * 函数: PressureController_SetTarget
 * 功能: 设置目标压力
 * 参数: data - 压力数据指针
 *       target - 目标压力（mmHg）
 */
void PressureController_SetTarget(PressureData_t *data, uint16_t target);

/**
 * 函数: PressureController_Update
 * 功能: 更新压力控制（主循环调用）
 * 参数: data - 压力数据指针
 *       pump - 气泵数据指针
 */
void PressureController_Update(PressureData_t *data, PumpData_t *pump);

/**
 * 函数: PressureController_CalculateThresholds
 * 功能: 计算动态阈值
 * 参数: data - 压力数据指针
 * 
 * 算法说明:
 *   下阈值 = 目标压力 - 10%
 *   上阈值 = 目标压力 + (5%~10%，根据压力段调整)
 */
void PressureController_CalculateThresholds(PressureData_t *data);

/**
 * 函数: PressureController_IsInRange
 * 功能: 判断压力是否在正常范围
 * 返回: true=在范围内, false=超出范围
 */
bool PressureController_IsInRange(const PressureData_t *data);

#endif /* PRESSURE_CONTROLLER_H */

