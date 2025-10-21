/****************************************************************************
 * 文件名: pressure_controller.c
 * 功能: 压力控制器实现
 * 
 * 说明: 
 *   双位控制算法（Bang-Bang Control）
 *   - 压力低于下限：开启气泵补气
 *   - 压力高于上限：停止气泵
 *   - 压力在范围内：保持当前状态，进行故障检测
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/pressure_controller.h"
#include "../../Drivers/Inc/adc_driver.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../Core/Inc/system_config.h"
#include "../Inc/realtime_tasks.h"

/* 全局压力控制实例（供中断访问） */
static PressureControl_t *g_pressure_ctrl = NULL;

/**
 * 函数: PressureController_Init
 * 功能: 初始化压力控制器
 */
void PressureController_Init(PressureControl_t *ctrl)
{
	ctrl->target_pressure = PRESSURE_DEFAULT;
	ctrl->current_pressure = 0;
	ctrl->control_enabled = false;
	ctrl->calibration_k = 2.75f;
	
	/* 保存全局指针供中断使用 */
	g_pressure_ctrl = ctrl;
}

/**
 * 函数: PressureController_SetTarget
 * 功能: 设置目标压力
 */
void PressureController_SetTarget(PressureControl_t *ctrl, uint16_t target)
{
	if (target < PRESSURE_MIN) target = PRESSURE_MIN;
	if (target > PRESSURE_MAX) target = PRESSURE_MAX;
	
	ctrl->target_pressure = target;
}

/**
 * 函数: PressureController_Execute [已废弃]
 * 说明: 此函数已被 PressureController_Update() 替代
 *       保留实现仅供参考，不会被调用
 */
#if 0  // 已废弃，不再使用
void PressureController_Execute(void)
{
	if (g_pressure_ctrl == NULL || !g_pressure_ctrl->control_enabled) {
		return;
	}
	
	/* 读取最新ADC值并转换为压力 */
	uint16_t adc_value = ADC_GetLatestPressure();
	g_pressure_ctrl->current_pressure = ADC_ConvertToMmHg(
		adc_value, 
		0,  // 零点（应从系统配置读取）
		g_pressure_ctrl->calibration_k
	);
	
	/* 计算压力偏差（使用 int32_t 避免溢出） */
	int32_t error = (int32_t)g_pressure_ctrl->target_pressure - (int32_t)g_pressure_ctrl->current_pressure;
	
	/* 计算动态阈值（10%目标压力，最小5mmHg） */
	uint16_t threshold = g_pressure_ctrl->target_pressure / 10;
	if (threshold < 5) threshold = 5;
	
	/* 双位控制逻辑（将 threshold 转换为有符号以避免符号转换警告） */
	int32_t threshold_signed = (int32_t)threshold;
	
	if (error > threshold_signed) {
		/* 压力不足：关闭排气阀，开启气泵 */
		HAL_Valve2_Close();
		RealtimeTasks_SetPumpEnable(true);
		
		/* 根据压力段设置PWM占空比 */
		if (error > threshold_signed * 3) {
			RealtimeTasks_SetPWMDuty(8);  // 80%
		} else if (error > threshold_signed * 2) {
			RealtimeTasks_SetPWMDuty(6);  // 60%
		} else {
			RealtimeTasks_SetPWMDuty(4);  // 40%
		}
	}
	else if (error < -threshold_signed) {
		/* 压力过高：开启排气阀，关闭气泵 */
		HAL_Valve2_Open();
		RealtimeTasks_SetPumpEnable(false);
	}
	else {
		/* 压力在范围内：关闭阀门和气泵 */
		HAL_Valve2_Close();
		RealtimeTasks_SetPumpEnable(false);
	}
}
#endif  // 结束废弃代码

/**
 * 函数: PressureController_Update
 * 功能: 压力控制主循环
 * 
 * 控制逻辑：
 *   1. 读取当前压力
 *   2. 与阈值比较
 *   3. 控制气泵和阀门
 */
void PressureController_Update(PressureData_t *data, PumpData_t *pump)
{
	/* 读取当前压力 */
	uint16_t adc_raw = ADC_ReadPressure();
	data->current_pressure = ADC_ConvertToMmHg(adc_raw, data->adc_zero, data->calibration_k);
	
	/* 双位控制判断 */
	if (data->current_pressure <= data->lower_threshold)
	{
		/* 压力过低，开启补气 */
		HAL_Valve2_Close();      // 关闭放气阀
		HAL_Pump_Start();        // 启动气泵
		pump->is_enabled = true;
		data->is_stable = false;
	}
	else if (data->current_pressure >= data->upper_threshold)
	{
		/* 压力过高，停止补气 */
		HAL_Pump_Stop();         // 停止气泵
		pump->is_enabled = false;
		data->is_stable = false;
	}
	else
	{
		/* 压力在正常范围内 */
		data->is_stable = true;
		/* 保持当前气泵状态不变 */
	}
}

/**
 * 函数: PressureController_IsInRange
 * 功能: 判断压力是否在正常范围
 */
bool PressureController_IsInRange(const PressureData_t *data)
{
	return (data->current_pressure >= data->lower_threshold &&
	        data->current_pressure <= data->upper_threshold);
}

