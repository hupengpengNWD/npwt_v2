/****************************************************************************
 * 文件名: ARCHITECTURE_EXAMPLE.c
 * 功能: 新架构使用示例（不参与编译，仅作参考）
 * 
 * 说明: 
 *   展示如何使用新架构框架开发代码
 *   对比新旧实现方式的差异
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#include "include.h"
#include "system_types.h"
#include "system_manager.h"
#include "hardware_abstraction.h"
#include "state_machine.h"

/****************************************************************************
 * 示例1：使用系统管理器替代全局变量
 ****************************************************************************/

/* ========== 旧方式（不推荐） ========== */
void old_set_pressure_BAD(void)
{
	// 直接访问全局变量
	mod_seta_preh = 120;
	adc_ps00 = 100;
	con_hi_delta = 12;
	con_lo_delta = 12;
}

/* ========== 新方式（推荐） ========== */
void new_set_pressure_GOOD(void)
{
	// 通过系统管理器接口
	System_SetTargetPressure(120);
	System_SetCurrentPressure(100);
	System_SetPressureDelta(12, 12);
	
	// 或者直接访问结构体
	PressureControl_t *pressure = System_GetPressure();
	pressure->mod_seta_preh = 120;
	pressure->adc_ps00 = 100;
}

/****************************************************************************
 * 示例2：使用硬件抽象层
 ****************************************************************************/

/* ========== 旧方式（不推荐） ========== */
void old_control_pump_BAD(void)
{
	// 直接操作寄存器
	PUMP = 1;
	VAL1 = 0;
	VAL2 = 1;
	SPEAK = 1;
}

/* ========== 新方式（推荐） ========== */
void new_control_pump_GOOD(void)
{
	// 使用HAL接口
	HAL_Pump_Start();
	HAL_Valve1_Close();
	HAL_Valve2_Open();
	HAL_Buzzer_On();
}

// 更高层的封装
void PumpControl_Start_Pumping(void)
{
	HAL_Valve1_Close();   // 1. 先关闭排气阀
	HAL_Valve2_Close();   // 2. 关闭放气阀
	HAL_Pump_Start();     // 3. 启动气泵
	System_EnablePump();  // 4. 设置软件标志
}

void PumpControl_Stop_Pumping(void)
{
	HAL_Pump_Stop();       // 1. 停止气泵
	System_DisablePump();  // 2. 清除软件标志
}

/****************************************************************************
 * 示例3：使用状态机框架
 ****************************************************************************/

/* ========== 旧方式（switch-case嵌套，500+行） ========== */
void old_mode_control_BAD(void)
{
	switch (mod_main_a)
	{
		case MOD_SYS:
		{
			// 100行代码...
			if (某条件)
				mod_main_a = MOD_WAT;
			break;
		}
		case MOD_WAT:
		{
			// 80行代码...
			break;
		}
		// ...更多case
	}
}

/* ========== 新方式（表驱动，清晰简洁） ========== */

// 定义状态处理函数（每个函数职责单一）
static void handle_mode_system_init(void)
{
	SystemState_t *sys = System_GetState();
	
	// 显示开机画面
	if (sys->mod_tim_cnta < INIT_BOOT_SCREEN_TIME)
	{
		// 检查启动按键
		if (System_GetKeyValue() != KEY_C)
		{
			sys->key_start_tim = 0;
		}
		else
		{
			sys->key_start_tim++;
		}
	}
	// 初始化硬件
	else if (sys->mod_tim_cnta < INIT_VERSION_DISPLAY_TIME)
	{
		if (sys->mod_tim_cnta == INIT_BOOT_SCREEN_TIME + 2)
		{
			// 检查启动时间
			if (sys->key_start_tim < KEY_START_MIN_HOLD_TIME)
			{
				// 启动时间不足，关机
				HAL_Power_Release();
				return;
			}
			else
			{
				// 初始化完成
				HAL_Power_Hold();
				// 初始化LCD等
			}
		}
		// 显示版本号
	}
	// 转入等待模式
	else if (sys->mod_tim_cnta >= INIT_TOTAL_TIME)
	{
		System_SetMode(MODE_WAIT_COMMAND);
		sys->mod_tim_cnta = 0;
	}
	
	sys->mod_tim_cnta++;
}

static void handle_mode_wait_command(void)
{
	// 等待用户操作的代码...
}

static void handle_mode_continuous(void)
{
	// 连续模式的代码...
}

// 状态表定义
const StateTableEntry_t work_mode_table[] = {
	{MODE_SYSTEM_INIT,  handle_mode_system_init,  "系统初始化"},
	{MODE_WAIT_COMMAND, handle_mode_wait_command, "等待指令"},
	{MODE_CONTINUOUS,   handle_mode_continuous,   "连续模式"},
	// ...
};

// 状态机实例
static StateMachine_t main_state_machine;

// 初始化（在main函数中调用）
void ModeControl_Init(void)
{
	StateMachine_Init(&main_state_machine,
	                 work_mode_table,
	                 sizeof(work_mode_table)/sizeof(work_mode_table[0]),
	                 MODE_SYSTEM_INIT);
}

// 运行（在主循环中调用）
void ModeControl_Run(void)
{
	StateMachine_Run(&main_state_machine);  // 只需一行！
}

/****************************************************************************
 * 示例4：模块化设计
 ****************************************************************************/

/* ========== 旧方式：所有代码混在一起 ========== */
void old_audio_control_BAD(void)
{
	// 直接访问大量全局变量
	if (err_codea)
	{
		audio_flg = LED_LOW_THAN_3_5V_OR_ERR;
		if (mute_flg)
		{
			if (mute_tim++ > CANCEL_MUTEFLAG_TIMEOUT)
			{
				mute_flg = 0;
				// ...
			}
		}
		else
		{
			// 播放声音...
		}
	}
}

/* ========== 新方式：模块化 + 接口化 ========== */

// audio_manager.c（新模块）
void AudioManager_Update(void)
{
	AudioControl_t *audio = System_GetAudio();
	unsigned char error = System_GetErrorCode();
	
	if (error)
	{
		audio->audio_flg = LED_LOW_THAN_3_5V_OR_ERR;
		
		if (audio->mute_flg)
		{
			AudioManager_HandleMute(audio);
		}
		else
		{
			AudioManager_PlayAlert(audio);
		}
	}
}

static void AudioManager_HandleMute(AudioControl_t *audio)
{
	if (audio->mute_tim++ > CANCEL_MUTEFLAG_TIMEOUT)
	{
		audio->mute_flg = 0;
		audio->mute_tim = 0;
	}
}

static void AudioManager_PlayAlert(AudioControl_t *audio)
{
	// 播放报警声音的逻辑
	HAL_Buzzer_On();
	// ...
}

/****************************************************************************
 * 示例5：压力控制模块化
 ****************************************************************************/

// pressure_controller.c（新模块）

/**
 * 函数: PressureController_Update
 * 功能: 压力控制主循环（双位控制算法）
 */
void PressureController_Update(void)
{
	PressureControl_t *press = System_GetPressure();
	
	// 计算动态阈值
	PressureController_CalculateDelta(press->mod_seta_preh);
	
	// 获取下限压力
	unsigned short lower_limit = press->mod_seta_preh - press->con_lo_delta;
	unsigned short upper_limit = press->mod_seta_preh + press->con_hi_delta;
	
	// 双位控制判断
	if (press->adc_ps00 <= lower_limit)
	{
		// 压力过低，开启补气
		PressureController_StartPumping();
	}
	else if (press->adc_ps00 >= upper_limit)
	{
		// 压力过高，停止补气
		PressureController_StopPumping();
	}
	else
	{
		// 压力正常，进行故障检测
		FaultDetector_CheckBlockage();
		FaultDetector_CheckLeakage();
	}
}

static void PressureController_CalculateDelta(unsigned short target)
{
	PressureControl_t *press = System_GetPressure();
	
	// 下阈值：10%
	press->con_lo_delta = target / 10;
	
	// 上阈值：根据压力段调整
	if (target > 200)
		press->con_hi_delta = target / 20;        // 5%
	else if (target >= 80)
		press->con_hi_delta = target / 20 + 4;   // 5% + 4mmHg
	else
		press->con_hi_delta = target / 10 + 2;   // 10% + 2mmHg
}

static void PressureController_StartPumping(void)
{
	HAL_Valve2_Close();          // 关闭放气阀
	HAL_Pump_Start();            // 启动气泵
	System_EnablePump();         // 设置软件标志
}

static void PressureController_StopPumping(void)
{
	HAL_Pump_Stop();             // 停止气泵
	System_DisablePump();        // 清除软件标志
}

/****************************************************************************
 * 示例6：故障检测模块化
 ****************************************************************************/

// fault_detector.c（新模块）

void FaultDetector_CheckLeakage(void)
{
	FaultDetector_t *fault = System_GetFault();
	PumpValveControl_t *pump = System_GetPump();
	
	if (pump->open_bum == FALSE)  // 不在补气状态
	{
		// 清除泄漏标志的逻辑
		if (fault->leak_cancel_counter++ >= LEAK_CANCEL_TIME_CYCLES)
		{
			System_ClearFaultFlag(ERRA_LQ);
			fault->load_perioda_up = 0;
		}
	}
	else  // 正在补气
	{
		// 检测泄漏的逻辑
		if (fault->load_perioda_up > SET_LEAKAGE_FLAG_CYCLE)
		{
			System_SetFaultFlag(ERRA_LQ);  // 设置泄漏标志
		}
	}
}

void FaultDetector_CheckBlockage(void)
{
	FaultDetector_t *fault = System_GetFault();
	
	// 检查压力记录数组
	if (fault->record_ds_turn > BLOCKAGE_STEADY_COUNT)
	{
		// 判断最近8次压力是否相同
		if (fault->record_ds[7] == fault->record_ds[0])
		{
			System_SetFaultFlag(ERRB_DS);  // 设置堵塞标志
		}
	}
}

/****************************************************************************
 * 示例7：完整的main()函数重构
 ****************************************************************************/

void main_NEW_ARCHITECTURE(void)
{
	/* ========== 初始化 ========== */
	SYS_OSC_Ini();          // 硬件时钟
	HAL_Watchdog_Clear();   // 清除看门狗
	System_Init();          // 系统管理器初始化
	
	// 从Flash加载配置
	FlashConfig_t *cfg = System_GetFlashConfig();
	cfg->dataREAD1 = Flash_Read(addr);
	cfg->dataK1 = Flash_Read(addr+8);
	// ...
	
	// 设置系统参数
	System_SetLanguage(cfg->dataREAD1 & 0xFF);
	System_SetTargetPressure(cfg->dataREAD2);
	
	// 硬件初始化
	SYS_IO_Ini();
	SYS_TMR0_Ini();
	SYS_TMR3_Ini();
	adc_init();
	
	// 状态机初始化
	ModeControl_Init();
	
	// 使能中断
	GIE = 1;
	PEIE = 1;
	
	/* ========== 主循环 ========== */
	while (1)
	{
		HAL_Watchdog_Clear();
		
		if (System_GetFlags()->FLG_SYS_10MS)  // 20ms周期
		{
			System_GetFlags()->FLG_SYS_10MS = 0;
			
			// 运行状态机
			ModeControl_Run();
			
			// 更新各个管理器
			AudioManager_Update();
			BatteryManager_Update();
			PressureController_Update();
			FaultDetector_Update();
			
			// 显示更新
			DisplayManager_Update();
		}
	}
}

/****************************************************************************
 * 代码质量对比
 ****************************************************************************/

/* 
【可读性对比】
旧：PUMP = 1;                    → 不清楚是启动还是停止
新：HAL_Pump_Start();            → 一目了然

旧：mod_seta_preh = 120;         → 不知道这是什么变量
新：System_SetTargetPressure(120); → 清楚是设置目标压力

【可维护性对比】
旧：在10个文件中直接修改 bat_lev
新：只能通过 System_SetBatteryLevel() 修改，便于追踪

【可测试性对比】
旧：无法单独测试某个功能（全局变量耦合）
新：可以mock System_GetBattery() 进行单元测试

【可移植性对比】
旧：更换硬件需要修改所有 PUMP=1 的地方
新：只需修改 HAL_Pump_Start() 的实现

【Bug定位对比】
旧：bat_lev被意外修改，不知道是哪里改的
新：只能通过接口修改，可在接口处打断点追踪
*/

