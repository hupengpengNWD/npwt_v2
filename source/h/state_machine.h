/****************************************************************************
 * 文件名: state_machine.h
 * 功能: 状态机框架定义
 * 
 * 说明: 
 *   使用表驱动方式实现状态机，使状态转换逻辑更清晰
 *   每个状态都有独立的处理函数，便于维护和测试
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

/****************************************************************************
 * 工作模式状态枚举（更清晰的定义）
 ****************************************************************************/
typedef enum {
	MODE_SYSTEM_INIT = 0,        // 系统初始化模式
	MODE_WAIT_COMMAND = 1,       // 等待指令模式
	MODE_CONTINUOUS = 2,         // 连续工作模式
	MODE_INTERMITTENT = 3,       // 间歇工作模式
	MODE_SETTING = 4,            // 参数设定模式
	MODE_PAUSE = 5,              // 暂停模式
	MODE_ERROR = 6,              // 错误模式
	MODE_POWER_OFF = 7,          // 关机模式
	MODE_IDLE_TIMEOUT = 8,       // 空闲超时模式
	MODE_LANGUAGE_SELECT = 9,    // 语言选择模式
	MODE_STANDBY = 10,           // 待机模式
	MODE_MAX                     // 模式总数
} WorkMode_e;

/****************************************************************************
 * 放气状态机枚举
 ****************************************************************************/
typedef enum {
	DEGAS_STATE_IDLE = 0,        // 空闲状态
	DEGAS_STATE_OPENING,         // 正在开阀放气
	DEGAS_STATE_STABILIZING,     // 等待压力稳定
	DEGAS_STATE_MAX
} DegasState_e;

/****************************************************************************
 * 蜂鸣器状态机枚举
 ****************************************************************************/
typedef enum {
	BEEP_STATE_IDLE = 0,         // 空闲
	BEEP_STATE_FIRST_BEEP,       // 第一次响
	BEEP_STATE_PAUSE,            // 停顿
	BEEP_STATE_SECOND_BEEP,      // 第二次响
	BEEP_STATE_END               // 结束
} BeepState_e;

/****************************************************************************
 * 间歇模式阶段枚举
 ****************************************************************************/
typedef enum {
	JX_PHASE_HIGH_PRESSURE = 0,  // 高压阶段
	JX_PHASE_LOW_PRESSURE = 1,   // 低压阶段
	JX_PHASE_PAUSE = 2,          // 停顿阶段
	JX_PHASE_MAX
} JxPhase_e;

/****************************************************************************
 * 状态处理函数指针类型
 ****************************************************************************/
typedef void (*StateHandler_t)(void);

/****************************************************************************
 * 状态机表项结构
 ****************************************************************************/
typedef struct {
	unsigned char state;          // 状态编号
	StateHandler_t handler;       // 状态处理函数
	const char *name;             // 状态名称（用于调试）
} StateTableEntry_t;

/****************************************************************************
 * 状态机控制结构
 ****************************************************************************/
typedef struct {
	unsigned char current_state;  // 当前状态
	unsigned char previous_state; // 前一个状态
	const StateTableEntry_t *table;  // 状态表指针
	unsigned char table_size;     // 状态表大小
} StateMachine_t;

/****************************************************************************
 * 状态机操作函数
 ****************************************************************************/

/**
 * 函数: StateMachine_Init
 * 功能: 初始化状态机
 * 参数: sm - 状态机指针
 *       table - 状态表
 *       size - 状态表大小
 *       init_state - 初始状态
 */
void StateMachine_Init(StateMachine_t *sm, 
                      const StateTableEntry_t *table,
                      unsigned char size,
                      unsigned char init_state);

/**
 * 函数: StateMachine_SetState
 * 功能: 切换状态
 * 参数: sm - 状态机指针
 *       new_state - 新状态
 */
void StateMachine_SetState(StateMachine_t *sm, unsigned char new_state);

/**
 * 函数: StateMachine_Run
 * 功能: 运行状态机（调用当前状态的处理函数）
 * 参数: sm - 状态机指针
 */
void StateMachine_Run(StateMachine_t *sm);

/**
 * 函数: StateMachine_GetCurrentState
 * 功能: 获取当前状态
 * 参数: sm - 状态机指针
 * 返回: 当前状态编号
 */
unsigned char StateMachine_GetCurrentState(const StateMachine_t *sm);

#endif /* STATE_MACHINE_H */

