
/****************************************************************************
 * 文件名: npwt_dis_main.c
 * 功能: NPWT负压伤口治疗仪 - 主程序
 * 
 * 主要功能:
 *   1. 程序入口main()函数
 *   2. 系统初始化流程
 *   3. 主循环控制（20ms周期）
 *   4. 故障检测与声音报警
 *   5. 电池电量监控
 *   6. 系统工作模式管理
 * 
 * 调用关系:
 *   - 初始化模块: SYS_OSC_Ini(), SYS_IO_Ini(), SYS_TMR0_Ini() 等
 *   - 控制模块: MODE_ProA(), MODE_Pro(), PRESS_ConA()
 *   - 显示模块: DISP_MainA(), DISP_Bat()
 *   - 输入模块: KEY_Scan(), ADC_Ps90()
 *   - 报警模块: Warn(), AUDIO(), AUDIO_Key()
 * 
 * 版本: v1.0 俄语/英语版本
 ****************************************************************************/

#include  "include.h"
#include  "Flash.h"
#include "BIOS_JLX1864G_139.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include   "npwt_dis_main.h"
#include   "npwt_con_main.h"
#include   "npwt_dis_ofile_lcd_02.h"
#include  "adc.h"
#include  "system_manager.h"
#include  "global_compat.h"
#include  "hardware_abstraction.h"

#include "../project/config_bits.h"

/****************************************************************************
 * 【架构重构说明】
 * 
 * 本文件已迁移至新架构：
 * - 全局变量已移至 system_manager.c 中的结构体
 * - 通过 global_compat.h 提供的宏进行兼容访问
 * - 硬件操作已更换为 hardware_abstraction.h 中的HAL函数
 * - 底层实现已改变，但对本文件代码透明
 ****************************************************************************/

/* 蜂鸣器状态枚举 */
typedef enum _on{
	BEEGO,      // 蜂鸣器初始状态
	BEEONE,     // 第一次响
	BEETWO,     // 停顿
	BEETHREE,   // 第二次响
	BEEEND      // 结束
}BEEPTWO;
BEEPTWO BEE_TWO=BEEGO;

// 扬声器控制宏（使用HAL层）
#define spk_set()  HAL_Buzzer_On();SPK_STATE=1;  // 开启扬声器
#define spk_clr()  HAL_Buzzer_Off();SPK_STATE=0;  // 关闭扬声器

/**
 * 函数: spk_bee_server
 * 功能: 扬声器服务函数，控制蜂鸣器的超时保护
 * 说明: 防止蜂鸣器一直响，超过MAX_BEE_TIME后自动关闭
 */
void spk_bee_server()
{
	if (0==SPK_STATE)  // 如果扬声器已关闭
	{
		spk_selay = 0;
		spk_clr();
		return;
	}
	
	if (spk_selay++ > MAX_BEE_TIME)  // 超时保护：响铃时间过长则强制关闭
	{
		spk_selay = 15;
		audio_period=0;
		BEE_TWO=BEETWO;
		spk_clr();
	}
}

/**
 * 函数: SYS_DatIni
 * 功能: 系统数据初始化
 * 说明: 初始化所有全局变量和系统状态到默认值
 */
void  SYS_DatIni(void)
{
	mod_main_a =  MOD_SYS;               // 设置初始模式为系统初始化模式
	mod_seta_cnt = UI_WORKMODE_SELECT;   // UI设置计数器：工作模式选择
	back_led_cnt=0;                      // 背光LED计数器清零
	key_val=0;                           // 按键值清零
	key_val_bak=0;                       // 按键备份值清零
	lock_flg=0;                          // 锁定标志清零
	err_codea=err_codeb=0;               // 错误码清零
	mod_tim_cnta=0;                      // 模式计时器清零
	buz_flg1=0;                          // 蜂鸣器状态清零
	
	audio_period=AUDIO_PERIOD+400;       // 设置声音周期
	audio_cnt=0;                         // 声音计数清零
	audio_basic=0;                       // 声音基础计数清零
	audio_flg=LED_BAT_NORMAL;            // 声音标志设为正常
	key_start_tim=0;                     // 按键启动计时器清零
	bat_close_tim=0;                     // 电池关机计时器清零
	mod_tim_cnta2=0;                     // 模式计时器2清零
	mod_jixa=0;                          // 间歇模式状态清零
}

/**
 * 函数: Warn
 * 功能: 故障报警优先级判断
 * 说明: 根据故障类型优先级设置错误码
 *       优先级（从高到低）：电路堵塞 > 液位警告 > 泄漏 > 传感器故障 > 未接液瓶
 */
void Warn(void)
{
	// 根据故障标志位判断具体错误类型，优先级从高到低
	
	if (flager_a&ERRA_V)  // 标志位：未接液瓶或管路堵塞
	{
		if (mod_main_a==MOD_WAT)  // 如果在等待指令模式，判定为未接液瓶
		{
			err_codea=ERR_CANISTER_NOT_CON;  // 错误：未连接收集罐
		}
		else  // 否则判定为管路堵塞
		{
			err_codea=ERR_PIPE_BLOCKED;  // 错误：管路堵塞
		}
	}
	else if (flager_a&ERRA_S)  // 标志位：传感器故障或液位满
	{
		if (mod_main_a==MOD_WAT)  // 等待模式下判定为传感器故障
		{
			err_codea=ERR_SENSOR_MALFUCTION;  // 错误：传感器故障
		}
		else  // 工作模式下判定为液位满
		{
			err_codea=ERR_CANISTER_FULL;  // 错误：液位满
		}
	}
	else if (flager_a&ERRA_LQ)  // 标志位：泄漏
	{
		err_codea=ERR_AIR_LEAKAGE;  // 错误：空气泄漏
	}
	else if (flager_a&ERRA_YW)  // 标志位：液位警告
	{
		err_codea=ERR_CANISTER_REACHED;  // 错误：液位达到警戒线
	}
	else if (flager_a&ERRB_DS)  // 标志位：电路堵塞
	{
		err_codea=ERR_JAMED;  // 错误：堵塞（IEC60601标准）
	}
	else if (flager_a&ERRB_TK)  // 标志位：空闲超时
	{
		err_codea=ERR_DEV_IDLE;  // 错误：设备空闲（5分钟无操作）
	}
	else
	{
		err_codea=0;  // 无故障
	}
	
	if (err_codea > 0)  // 如果有故障，重置背光计数器
	{
		back_led_cnt = 0;
	}
	
	spk_bee_server();  // 调用蜂鸣器服务函数
}
unsigned short bee_delay = 0;  // 蜂鸣器延迟计数

/**
 * 函数: bee_three
 * 功能: 三段式蜂鸣器控制
 * 说明: 控制蜂鸣器按照"响-停-响"的模式工作
 */
void bee_three()
{
	// 状态机控制蜂鸣器的响铃模式
	switch (BEE_TWO)
	{
	default:
		spk_clr();
		BEE_TWO = BEEGO;
		break;
		
	case BEEGO:  // 初始状态：准备第一次响
	{
		audio_period = 0;
		audio_basic = 0;
		spk_clr();
		BEE_TWO=BEEONE;
		break;
	}
	
	case BEEONE:  // 第一次响：持续AUDIO_TIM个周期
	{
		spk_set();
		if (audio_basic++ > AUDIO_TIM)
		{
			BEE_TWO=BEETWO;  // 转入停顿状态
			audio_period = 0;
		}
		break;
	}
	
	case BEETWO:  // 停顿状态：持续AUDIO_PERIOD个周期
	{
		spk_clr();
		if (audio_period++ > AUDIO_PERIOD)
		{
			BEE_TWO=BEETHREE;  // 转入第二次响或循环
		}
		break;
	}
	
	case BEETHREE:  // 准备循环或结束
	{
		audio_period = 0;
		audio_basic = 0;
		spk_clr();
		BEE_TWO=BEEONE;  // 重新开始循环
		break;
	}
	}
}

#if 0

void AUDIO_Sub(unsigned char cnt)
{
	unsigned char  i;
	i=cnt;
	
	if ((key_val==KEY_MUT)&&(SPEAK_flg==0))
	{
		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		close_flg=0;

		if (mute_flg==0)
		{
			DISP_BuzClr(6,25);
		}
	}
	
	if ((key_val==KEY_MUTL)&&(SPEAK_flg==0))
	{

		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		if (mute_flg)
		{
			close_flg=1;
		}
		else
		{
			close_flg=0;
		}

		if (mute_flg==0)
		{
			DISP_BuzClr(6,25);
		}
	}

	if (mute_flg)
	{
		if (close_flg)
		{
		if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
		{
			mute_flg=0;
			mute_tim=0;
			DISP_BuzClr(6,25);
			bee_three();
		}
			return;
		}

	}
	else
	{
		bee_three();
	}

}
#endif

void AUDIO_Sub(unsigned char cnt)
{
	unsigned char  i;
	i=cnt;
	
	if ((key_val==KEY_MUT)&&(SPEAK_flg==0))
	{
		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;

		if (mute_flg)
		{
			close_flg=1;
		}
		else
		{
			close_flg=0;
		}

		if (mute_flg==0)
		{
			DISP_BuzClr(6,25);
		}

		if (mute_flg)
		{
			if (close_flg)
			{
				if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
				{
					mute_flg=0;
					mute_tim=0;
					DISP_BuzClr(6,25);
					bee_three();
				}
				return;
			}
		}
		else
		{
			bee_three();
		}
	}

	if ((key_val==KEY_MUTL)&&(SPEAK_flg==0))
	{

		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		if (mute_flg)
		{
			close_flg=1;
		}
		else
		{
			close_flg=0;
		}
		if (mute_flg==0)
		{
			DISP_BuzClr(6,25);
		}
	}

	if (mute_flg)
	{
		if (close_flg)
		{
			if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
			{
			  mute_flg=0;
			  mute_tim=0;
			  DISP_BuzClr(6,25);
			  bee_three();
			}
			return;
		}
	}
	else
	{
		bee_three();
	}
}

void BUZ_Cls(void);

void AUDIO(void)
{

	if ((bat_lev&0x0f)==LOWER_THAN_3_5V)
	{
		battery_status_flags=battery_status_flags|1;
	}
	else
	{
		battery_status_flags=battery_status_flags&(~1);
	}

	if (err_codea)
	{
		battery_status_flags=battery_status_flags|2;
	}
	else
	{
		battery_status_flags=battery_status_flags&(~2);
	}
	if ((bat_lev&0x0f)==1)
	{
		battery_status_flags=battery_status_flags|8;
	}
	else
	{
		battery_status_flags=battery_status_flags&(~8);
	}

	if (battery_status_flags>bat_lev_bak)
	{
		mute_flg=0;
	}

	if ((bat_lev&0x0f)==LOWER_THAN_3_5V)
	{
		mod_main_a  =  MOD_OFF;
		mod_main_b  =  MOD_OFF;
		audio_flg=LED_LOW_THAN_3_5V_OR_ERR;
		DISP_Led();
		if (bat_close_tim++<BELOW_3_5V_CYCLE)
		{
			AUDIO_Sub(1);
		}
		else
		{
			HAL_Power_Release();  // 释放电源（关机）
		}
	}

	else if (err_codea||err_codeb)
	{
		
		if (!((err_codea == ERR_AIR_LEAKAGE)&&((BAT_CHARGING ==BAT_CHARGE)||(bat_sas == BAT_FULL)&&(overabc == 1))))
		{
			AUDIO_Sub(1);
		}
		audio_flg=LED_LOW_THAN_3_5V_OR_ERR;
	}
	else
	{
		if (mod_main_a == MOD_OFF)
			return;
		if ((bat_lev&0x0f)==LOWER_BAT_WARN_3_6V)
		{
			AUDIO_Sub(1);
			audio_flg=LED_LOW_THAN_3_6V;
			DISP_Led();
		}
		else
		{
			audio_flg=LED_BAT_NORMAL;
			mute_flg=0;
			mute_tim=0;
			GRE    =  0;
		}
	}
	bat_lev_bak = battery_status_flags;  // 备份当前状态
	
}

void BUZ_KeyCls(void)
{
	buz_flg  =  0;
	buz_flg1 =  0;
	buz_cnt  =  0;
	spk_clr();
	TMR2     =  0;
	spk_clr();
}
void BUZ_Cls(void)
{
	buz_flg  =  0;
	buz_flg1 =  1;
	buz_cnt  =  0;
	TMR2     =  0;
	spk_clr();
}

void AUDIO_Key(void)
{
	if (key_val && key_silent_flag)
	{
		buz_flg=1;
	}

	if (buz_flg)
	{
		if (buz_flg1==0)
		{
			if (buz_cnt++>=BUZZER_TIME_CYCLE)
			{
				spk_clr();
				BUZ_Cls();
			}
			else
			{
				spk_set();
			}
		}
	}
}

void FIND_FREE(void)
{
	if((mod_main_a==MOD_WAT)||(mod_main_a==MOD_SET)||(mod_main_a==MOD_ZHT)||(mod_main_a==MOD_TK))
	{
		TK_TIME++;
		if(TK_TIME>TK_TIMEOUT_CYCLE)
		{
			TK_TIME=TK_TIMEOUT_CYCLE+1;
			flager_a|=ERRB_TK;
			if(remember_mod==0)
				remember_mod=mod_main_a;
			mod_main_a=MOD_TK;
		}
		else
			flager_a&=~ERRB_TK;
	}
	else
	{
		TK_TIME=0;
		flager_a&=~ERRB_TK;
	}	
}

/****************************************************************************
 * 函数: main
 * 功能: 程序主入口
 * 
 * 初始化流程:
 *   1. 振荡器配置（内部32MHz）
 *   2. 从Flash读取用户配置数据
 *   3. 语言选择初始化
 *   4. GPIO、定时器、ADC初始化
 *   5. 使能中断
 *   6. 进入主循环
 * 
 * 主循环（20ms周期）:
 *   - 按键扫描与处理
 *   - 模式切换控制
 *   - LCD显示更新
 *   - 故障检测与报警
 *   - 电池电量监控
 *   - 气泵和电磁阀控制
 ****************************************************************************/
void main(void)
{
	/* ========== 第1步：系统时钟初始化 ========== */
	SYS_OSC_Ini();         // 配置振荡器：内部8MHz×4PLL=32MHz
	HAL_Watchdog_Clear();  // 清除看门狗
	
	/* ========== 【新架构】初始化系统管理器 ========== */
	System_Init();
	
	/* ========== 第2步：从Flash读取配置参数 ========== */
	// Flash地址0xa000开始存储用户配置数据，每次读取2字节
	dataREAD1 = Flash_Read(addr);      // 读取配置数据1
	dataREAD2 = Flash_Read(addr+2);    // 读取配置数据2
	dataREAD3 = Flash_Read(addr+4);    // 读取配置数据3
	dataREAD0 = Flash_Read(addr+6);    // 读取配置数据0
	
	// 读取压力校准系数K1-K4
	dataK1    = Flash_Read(addr+8);
	dataK2    = Flash_Read(addr+10);
	dataK3    = Flash_Read(addr+12);
	dataK4    = Flash_Read(addr+14);
	
	/* ========== 第3步：语言和静音配置 ========== */
	key_silent_flag  = Flash_Read(addr+16);  // 读取静音标志（高字节）和语言（低字节）
	language = (key_silent_flag & 0xFF);     // 提取语言选择：低字节
	
#ifdef LOGO_TYPE_VR_CHINA  // 国内版本
	if(language == 0xFF)       // 如果是新设备（未配置）
		language = 0xFE;       // 默认设为中文
#endif
	if(language != 0xFE)       // 其他版本默认英语
		language = 0;
		
	key_silent_flag = (key_silent_flag & SILENT_FLAG_BITMASK);  // 提取静音标志：高位
	
	mod_seta_prel  = Flash_Read(addr+18);  // 读取上次设置的目标压力值
	
	/* ========== 第4步：硬件初始化 ========== */
	SYS_IO_Ini();      // GPIO端口初始化
	SYS_TMR0_Ini();    // 定时器0初始化（20ms主循环）
	SYS_TMR3_Ini();    // 定时器3初始化（UART和PWM）
	SYS_DatIni();      // 系统数据初始化
	adc_init();        // ADC模块初始化
	
	/* ========== 第5步：UART测试与参数初始化 ========== */
	UART_Test();                   // UART参数加载测试
	flager_a=0;                    // 清除所有故障标志
	mod_seta_prehh=0;              // 目标压力缓存清零
	bump_need_out_air_flg=0;       // 排气标志清零
	BAT_WarnFir();                 // 电池电量首次检测
	valueK  =  2.75f;              // 压力校准系数K值
	
	/* ========== 第6步：使能中断和蜂鸣器 ========== */
	T3ON=1;            // 启动定时器3
	BEE_TWO=BEEGO;     // 蜂鸣器状态机初始化
	GIE =1;            // 使能全局中断
	PEIE =1;           // 使能外设中断
	asm("clrwdt");     // 清除看门狗
	clear_lqtimes();   // 清除泄漏次数记录
	
	/* ========== 主循环：20ms周期 ========== */
	while (1)  // 主循环：永久运行
	{
		asm("clrwdt");  // 喂狗：防止看门狗复位
		
		if (FLG_SYS_10MS)  // 每20ms执行一次（由定时器0中断设置）
		{
			FLG_SYS_10MS = 0;  // 清除标志
			
			/* --- 任务1：输入处理（按键扫描） --- */
			if (mod_main_a!=MOD_SYS)  // 非初始化模式时才扫描按键
			{
				KEY_Scan();       // 按键扫描
				AUDIO_Key();      // 按键音处理
			}
			
#ifdef PUMP_IDLE_FLAG
			/* --- 任务2：空闲检测 --- */
			FIND_FREE();  // 检测设备是否空闲超过5分钟
#endif
			
			/* --- 任务3：模式控制 --- */
			MODE_ProA();  // 模式处理A：根据当前模式执行相应逻辑
			MODE_Pro();   // 模式处理：按键切换和关机控制
			
			/* --- 任务4：显示更新 --- */
			DISP_MainA();  // 根据当前模式刷新LCD显示
			
			/* --- 任务5：故障检测与报警 --- */
			if ((mod_main_a!=MOD_SYS)||(mod_main_a==MOD_TK))
			{
				Warn();       // 故障检测与分类
				BAT_Warn();   // 电池电量检测
				AUDIO();      // 声音报警处理
			}
			
			/* --- 任务6：电池电量显示 --- */
			if ((mod_main_a!=MOD_SYS)&&(mod_main_a!=MOD_OFF))  // 非初始化且非关机模式
			{
				if (mod_main_a!=MOD_WAT)  // 非等待模式时显示电池
				{
					DISP_Bat();  // 显示电池电量
				}
			}

			/* --- 任务7：排气控制（泄漏补偿） --- */
			if (bump_need_out_air_flg & PUMP_ENABLE_MASK)  // 如果需要通过PWM控制排气
			{
				if (bum_dly_flg==0)  // 排气延迟标志为0时才执行
				{
					if (bum_dly++ >= VALVE1_OPEN_DELAY_CYCLES)  // 延迟1秒后开始排气
					{
						open_bum=1;  // 允许气泵工作
						
						if (bum_dly >= VALVE1_CLOSE_DELAY_CYCLES)  // 延迟2秒后关闭排气阀
						{
							bum_dly_flg=1;         // 设置排气完成标志
							HAL_Valve1_Close();    // 关闭电磁阀1（排气阀）
							bum_dly=0;             // 清零延迟计数器
							show_lq_times.lq_times++;  // 泄漏次数+1
						}
					}
					else  // 延迟期间保持阀门开启
					{
						HAL_Valve1_Open();  // 开启电磁阀1
					}
				}
			}
			else  // 不需要排气时
			{
				open_bum=0;      // 禁止气泵工作
				bum_dly_flg=0;   // 清除排气标志
				HAL_Valve1_Close();  // 关闭电磁阀1
				bum_dly=0;       // 清零延迟计数
			}
		}  // if (FLG_SYS_10MS) 结束
	}  // while(1) 结束
}  // main() 结束

