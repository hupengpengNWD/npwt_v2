/****************************************************************************
 * 文件名: fault_detector.c
 * 功能: 故障检测器实现
 * 
 * 说明: 
 *   实现泄漏、堵塞、液位满等故障的检测算法
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/fault_detector.h"
#include "../../Drivers/Inc/adc_driver.h"
#include "../../Core/Inc/system_config.h"
#include "../../Core/Inc/mcu_config.h"

/* 故障检测阈值 */
#define LEAKAGE_TIMEOUT_CYCLES      3000    // 泄漏检测超时：60秒
#define BLOCKAGE_TIMEOUT_CYCLES     180000  // 堵塞检测超时：1小时
#define BLOCKAGE_PRESSURE_THRESHOLD 8       // 堵塞判定：8次压力相同
#define LIQUID_LEVEL_THRESHOLD      150     // 液位满阈值

/**
 * 函数: FaultDetector_Init
 * 功能: 初始化故障检测器
 */
void FaultDetector_Init(FaultData_t *data)
{
	if (data != NULL) {
		data->active_error = ERROR_NONE;
		data->leakage_detected = false;
		data->blockage_detected = false;
		data->liquid_full = false;
		data->leakage_timer = 0;
		data->blockage_timer = 0;
		data->history_index = 0;
		
		/* 清空压力历史记录 */
		for (uint8_t i = 0; i < 8; i++) {
			data->pressure_history[i] = 0;
		}
	}
}

/**
 * 函数: FaultDetector_CheckLeakage
 * 功能: 检测泄漏故障
 * 
 * 算法：
 *   如果气泵持续补气超过LEAKAGE_TIMEOUT_CYCLES，判定为泄漏
 */
bool FaultDetector_CheckLeakage(FaultData_t *data, const PumpData_t *pump)
{
	if (pump->is_enabled)
	{
		/* 气泵正在工作，累加计时器 */
		if (data->leakage_timer++ > LEAKAGE_TIMEOUT_CYCLES)
		{
			data->leakage_detected = true;
			data->active_error = ERROR_LEAKAGE;
			return true;
		}
	}
	else
	{
		/* 气泵停止，清除计时器 */
		data->leakage_timer = 0;
	}
	
	return false;
}

/**
 * 函数: FaultDetector_CheckBlockage
 * 功能: 检测堵塞故障
 * 
 * 算法：
 *   记录最近8次压力值，如果完全相同，判定为堵塞
 */
bool FaultDetector_CheckBlockage(FaultData_t *data, const PressureData_t *pressure)
{
	/* 定期记录压力（每60秒一次） */
	static uint16_t record_interval = 0;
	
	if (record_interval++ >= 3000)  // 60秒
	{
		record_interval = 0;
		
		/* 记录当前压力 */
		data->pressure_history[data->history_index] = pressure->current_pressure;
		data->history_index = (data->history_index + 1) % 8;
		
		/* 检查是否所有记录值相同（只有记录满8次后才检查） */
		if (data->blockage_timer > 3000 * 8)
		{
			bool all_same = true;
			uint16_t first_value = data->pressure_history[0];
			
			for (uint8_t i = 1; i < 8; i++)
			{
				if (data->pressure_history[i] != first_value)
				{
					all_same = false;
					break;
				}
			}
			
			if (all_same && first_value > 0)
			{
				data->blockage_detected = true;
				data->active_error = ERROR_BLOCKAGE;
				return true;
			}
		}
	}
	
	data->blockage_timer++;
	return false;
}

/**
 * 函数: FaultDetector_CheckLiquidFull
 * 功能: 检测液位满
 */
bool FaultDetector_CheckLiquidFull(FaultData_t *data)
{
	uint16_t liquid_level = ADC_ReadLiquid();
	
	if (liquid_level < LIQUID_LEVEL_THRESHOLD)
	{
		data->liquid_full = true;
		data->active_error = ERROR_LIQUID_FULL;
		return true;
	}
	
	return false;
}

/**
 * 函数: FaultDetector_Update
 * 功能: 故障检测主循环
 */
void FaultDetector_Update(FaultData_t *data, 
                         const PressureData_t *pressure, 
                         const PumpData_t *pump)
{
	/* 检测各类故障（按优先级） */
	if (FaultDetector_CheckLiquidFull(data)) return;
	if (FaultDetector_CheckLeakage(data, pump)) return;
	if (FaultDetector_CheckBlockage(data, pressure)) return;
	
	/* 无故障 */
	data->active_error = ERROR_NONE;
}

/**
 * 函数: FaultDetector_GetActiveError
 * 功能: 获取当前激活的故障
 */
ErrorCode_e FaultDetector_GetActiveError(const FaultData_t *data)
{
	return data->active_error;
}

/**
 * 函数: FaultDetector_ClearError
 * 功能: 清除指定故障
 */
void FaultDetector_ClearError(FaultData_t *data, ErrorCode_e error)
{
	if (data != NULL && data->active_error == error)
	{
		data->active_error = ERROR_NONE;
		
		/* 清除对应的标志 */
		switch (error)
		{
			case ERROR_LEAKAGE:
				data->leakage_detected = false;
				data->leakage_timer = 0;
				break;
			case ERROR_BLOCKAGE:
				data->blockage_detected = false;
				data->blockage_timer = 0;
				break;
			case ERROR_LIQUID_FULL:
				data->liquid_full = false;
				break;
			default:
				break;
		}
	}
}

/**
 * 函数: FaultDetector_Check
 * 功能: 故障检测（主循环调用）
 * 说明: 简化的检测接口，用于main.c调用
 */
void FaultDetector_Check(FaultDetector_t *fault, PressureControl_t *pressure)
{
	if (fault != NULL && pressure != NULL) {
		static uint16_t check_timer = 0;
		
		/* 检测液位满 */
		uint16_t liquid_level = ADC_ReadLiquid();
		if (liquid_level > 800)  // 阈值
		{
			fault->liquid_full = true;
			fault->current_error = ERROR_LIQUID_FULL;
			return;
		}
		else
		{
			fault->liquid_full = false;
		}
		
		/* 检测压力相关故障（每1秒检测一次） */
		if (check_timer++ >= 50)  // 1秒
		{
			check_timer = 0;
			
		if (pressure->control_enabled)
		{
			/* 使用 int32_t 避免溢出（uint16_t 相减可能超出 int16_t 范围） */
			int32_t error = (int32_t)pressure->target_pressure - (int32_t)pressure->current_pressure;
				
				/* 检测过压 */
				if (error < -20)
				{
					fault->overpressure = true;
					fault->current_error = ERROR_OVERPRESSURE;
					return;
				}
				
				/* 检测泄漏：压力偏差持续较大 */
				static uint8_t leakage_count = 0;
				if (error > 15 && error < 40)
				{
					leakage_count++;
					if (leakage_count > 10)  // 10秒
					{
						fault->leakage_detected = true;
						fault->current_error = ERROR_LEAKAGE;
						return;
					}
				}
				else
				{
					leakage_count = 0;
					fault->leakage_detected = false;
				}
				
				/* 检测堵塞：压力偏差持续很大 */
				static uint8_t blockage_count = 0;
				if (error > 40)
				{
					blockage_count++;
					if (blockage_count > 30)  // 30秒
					{
						fault->blockage_detected = true;
						fault->current_error = ERROR_BLOCKAGE;
						return;
					}
				}
				else
				{
					blockage_count = 0;
					fault->blockage_detected = false;
				}
			}
		}
		
		/* 无故障 */
		fault->current_error = ERROR_NONE;
	}
}

