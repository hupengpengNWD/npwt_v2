/****************************************************************************
 * 文件名: state_machine.c
 * 功能: 状态机框架实现
 * 
 * 说明: 
 *   提供通用的状态机运行框架
 *   支持表驱动的状态转换
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#include "include.h"
#include "state_machine.h"

/**
 * 函数: StateMachine_Init
 * 功能: 初始化状态机
 */
void StateMachine_Init(StateMachine_t *sm, 
                      const StateTableEntry_t *table,
                      unsigned char size,
                      unsigned char init_state)
{
	if (sm == NULL || table == NULL)
		return;
	
	sm->current_state = init_state;
	sm->previous_state = init_state;
	sm->table = table;
	sm->table_size = size;
}

/**
 * 函数: StateMachine_SetState
 * 功能: 切换状态
 */
void StateMachine_SetState(StateMachine_t *sm, unsigned char new_state)
{
	if (sm == NULL)
		return;
	
	if (new_state >= sm->table_size)
		return;  // 状态编号越界保护
	
	sm->previous_state = sm->current_state;
	sm->current_state = new_state;
}

/**
 * 函数: StateMachine_Run
 * 功能: 运行状态机
 */
void StateMachine_Run(StateMachine_t *sm)
{
	if (sm == NULL || sm->table == NULL)
		return;
	
	// 查找当前状态的处理函数
	for (unsigned char i = 0; i < sm->table_size; i++)
	{
		if (sm->table[i].state == sm->current_state)
		{
			if (sm->table[i].handler != NULL)
			{
				sm->table[i].handler();  // 执行状态处理函数
			}
			return;
		}
	}
}

/**
 * 函数: StateMachine_GetCurrentState
 * 功能: 获取当前状态
 */
unsigned char StateMachine_GetCurrentState(const StateMachine_t *sm)
{
	if (sm == NULL)
		return 0;
	return sm->current_state;
}

