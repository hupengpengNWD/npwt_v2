/****************************************************************************
 * 文件名: intermittent_controller.c
 * 功能: 间歇模式控制器实现
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#include "../Inc/intermittent_controller.h"
#include "../Inc/pressure_controller.h"
#include "../../Core/Inc/system_config.h"

/* 全局变量（供中断访问） */
IntermittentPhase_e g_intermittent_phase = INTERMITTENT_PHASE_HIGH;
bool g_is_intermittent_mode = false;

/**
 * 函数: IntermittentController_Init
 * 功能: 初始化间歇模式控制器
 */
void IntermittentController_Init(IntermittentControl_t *ctrl)
{
	ctrl->current_phase = INTERMITTENT_PHASE_HIGH;
	ctrl->high_pressure = 120;  // 默认高压120mmHg
	ctrl->low_pressure = 80;    // 默认低压80mmHg
	ctrl->high_time = 10;       // 默认高压10分钟
	ctrl->low_time = 2;         // 默认低压2分钟
	ctrl->stop_time = 3;        // 默认停顿3分钟
	ctrl->phase_timer = 0;
}

/**
 * 函数: IntermittentController_Start
 * 功能: 启动间歇模式
 */
void IntermittentController_Start(IntermittentControl_t *ctrl)
{
	ctrl->current_phase = INTERMITTENT_PHASE_HIGH;
	ctrl->phase_timer = 0;
	
	/* 更新全局变量供中断使用 */
	g_intermittent_phase = INTERMITTENT_PHASE_HIGH;
	g_is_intermittent_mode = true;
}

/**
 * 函数: IntermittentController_Stop
 * 功能: 停止间歇模式
 */
void IntermittentController_Stop(IntermittentControl_t *ctrl)
{
	ctrl->current_phase = INTERMITTENT_PHASE_HIGH;
	ctrl->phase_timer = 0;
	
	/* 清除全局变量 */
	g_is_intermittent_mode = false;
}

/**
 * 函数: IntermittentController_Update
 * 功能: 更新间歇模式（每秒调用一次）
 * 说明: 
 *   实现3阶段自动切换：
 *   高压 → 低压 → 停顿 → 高压（循环）
 */
void IntermittentController_Update(IntermittentControl_t *ctrl, 
                                   PressureControl_t *pressure)
{
	ctrl->phase_timer++;
	
	switch (ctrl->current_phase)
	{
		case INTERMITTENT_PHASE_HIGH:
			/* 高压阶段：抽气到高压目标值 */
			pressure->target_pressure = ctrl->high_pressure;
			pressure->control_enabled = true;
			g_intermittent_phase = INTERMITTENT_PHASE_HIGH;
			
			/* 达到时间后切换到低压阶段 */
			if (ctrl->phase_timer >= ctrl->high_time * 60)
			{
				ctrl->current_phase = INTERMITTENT_PHASE_LOW;
				ctrl->phase_timer = 0;
				g_intermittent_phase = INTERMITTENT_PHASE_LOW;
			}
			break;
			
		case INTERMITTENT_PHASE_LOW:
			/* 低压阶段：降压到低压目标值 */
			pressure->target_pressure = ctrl->low_pressure;
			pressure->control_enabled = true;
			g_intermittent_phase = INTERMITTENT_PHASE_LOW;
			
			/* 达到时间后切换到停顿阶段 */
			if (ctrl->phase_timer >= ctrl->low_time * 60)
			{
				ctrl->current_phase = INTERMITTENT_PHASE_STOP;
				ctrl->phase_timer = 0;
				g_intermittent_phase = INTERMITTENT_PHASE_STOP;
			}
			break;
			
		case INTERMITTENT_PHASE_STOP:
			/* 停顿阶段：停止抽气，保持压力 */
			pressure->control_enabled = false;
			g_intermittent_phase = INTERMITTENT_PHASE_STOP;
			
			/* 达到时间后切换回高压阶段 */
			if (ctrl->phase_timer >= ctrl->stop_time * 60)
			{
				ctrl->current_phase = INTERMITTENT_PHASE_HIGH;
				ctrl->phase_timer = 0;
				g_intermittent_phase = INTERMITTENT_PHASE_HIGH;
			}
			break;
	}
}

/**
 * 函数: IntermittentController_GetPhase
 * 功能: 获取当前阶段
 */
IntermittentPhase_e IntermittentController_GetPhase(const IntermittentControl_t *ctrl)
{
	return ctrl->current_phase;
}

