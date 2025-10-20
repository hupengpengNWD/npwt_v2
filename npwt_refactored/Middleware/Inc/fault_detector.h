/****************************************************************************
 * 文件名: fault_detector.h
 * 功能: 故障检测器
 * 
 * 说明: 
 *   检测系统运行中的各种故障
 *   包括泄漏、堵塞、液位满等
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef FAULT_DETECTOR_H
#define FAULT_DETECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include "../../Core/Inc/system_types.h"

/****************************************************************************
 * 故障检测器函数
 ****************************************************************************/

/**
 * 函数: FaultDetector_Init
 * 功能: 初始化故障检测器
 */
void FaultDetector_Init(FaultData_t *data);

/**
 * 函数: FaultDetector_Update
 * 功能: 更新故障检测（主循环调用）
 * 参数: data - 故障数据
 *       pressure - 压力数据
 *       pump - 气泵数据
 */
void FaultDetector_Update(FaultData_t *data, 
                         const PressureData_t *pressure, 
                         const PumpData_t *pump);

/**
 * 函数: FaultDetector_CheckLeakage
 * 功能: 检测泄漏故障
 * 说明: 如果气泵持续补气时间过长，判定为泄漏
 * 返回: true=检测到泄漏
 */
bool FaultDetector_CheckLeakage(FaultData_t *data, const PumpData_t *pump);

/**
 * 函数: FaultDetector_CheckBlockage
 * 功能: 检测堵塞故障
 * 说明: 如果压力长时间不变，判定为堵塞
 * 返回: true=检测到堵塞
 */
bool FaultDetector_CheckBlockage(FaultData_t *data, const PressureData_t *pressure);

/**
 * 函数: FaultDetector_CheckLiquidFull
 * 功能: 检测液位满
 * 返回: true=液位满
 */
bool FaultDetector_CheckLiquidFull(FaultData_t *data);

/**
 * 函数: FaultDetector_GetActiveError
 * 功能: 获取当前激活的故障
 * 返回: 错误代码
 */
ErrorCode_e FaultDetector_GetActiveError(const FaultData_t *data);

/**
 * 函数: FaultDetector_ClearError
 * 功能: 清除指定故障
 */
void FaultDetector_ClearError(FaultData_t *data, ErrorCode_e error);

#endif /* FAULT_DETECTOR_H */

