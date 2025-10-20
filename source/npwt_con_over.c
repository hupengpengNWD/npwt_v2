/****************************************************************************
 * 文件名: npwt_con_over.c
 * 功能: 压力综合控制模块
 * 
 * 主要功能:
 *   1. 压力双位控制算法（Bang-Bang Control）
 *   2. 动态阈值计算
 *   3. 泄漏检测
 *   4. 液位检测
 *   5. 堵塞检测
 *   6. 放气控制
 * 
 * 控制策略:
 *   - 当前压力 < 目标压力-10% → 开启气泵
 *   - 当前压力 > 目标压力+5~15% → 关闭气泵
 *   - 压力在范围内 → 保持状态并进行故障检测
 * 
 * 调用关系:
 *   被调用: PRESS_ConSubA() → STAT_conNewa()
 *   调用: OPEN_PwmA(), CLS_PwmA(), get_press_delta()
 ****************************************************************************/

#include  "include.h"
#include "npwt_con_over.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include "npwt_dis_main.h"
#include "npwt_con_ifile_adc.h"

unsigned short   fall_cnta0 ;

unsigned char    fall_stata ;

unsigned char    con_flg_falla ;
unsigned short   gao_cnt ;
unsigned char    good_check=0 ;
unsigned char    nggood_check=0 ;
unsigned short   BOX_FQ_TURNS ;

unsigned short   tim_tima=0 ;

unsigned short   adc_ps00;

unsigned char    det300 = 10;
unsigned char    det00 ;
unsigned short con_hi_delta = 0;
unsigned short con_lo_delta = 0;

/* 调试和记录相关 */
/* 调试和记录相关 */
unsigned short debug_times = 0;      // 调试计数器
unsigned short debug_thirtys = 0;    // 30秒调试计数
unsigned short debug_air=0;          // 调试用气压值
unsigned char  debug=0;              // 调试标志

/**
 * 函数: CONTR_fallaNew
 * 功能: 放气控制（三段式）
 * 说明: 
 *   状态0：开启放气阀VAL2，准备放气
 *   状态1：保持放气0.08~0.32秒
 *   状态2：关闭放气阀，等待压力稳定1秒后检测是否达到目标
 */
void CONTR_fallaNew(void)
{
	if (fall_stata==0)  // 状态0：准备放气
	{
		fall_stata=1;      // 转入状态1
		fall_cnta0=0;      // 清零计数器
		VAL2 = 1;          // 开启放气阀VAL2
		
		record_ds_turn=0;         // 清零堵塞检测记录
		flager_a &=~ERRB_DS;      // 清除堵塞标志（间歇模式下放气时不检测堵塞）
	}
	else if (fall_stata==1)  // 状态1：正在放气
	{
		fall_cnta0++;
		unsigned short fall_cnta0_count = VALVE2_DEGAS_SHORT_CYCLES;  // 默认短时放气
		
		if(mod_seta_preh <= PRESSURE_MIN_THRESHOLD)  // 低压时延长放气时间
			fall_cnta0_count = VALVE2_DEGAS_LONG_CYCLES;  // 长时放气
			
		if (fall_cnta0>fall_cnta0_count)  // 放气时间到
		{
			VAL2 = 0;      // 关闭放气阀
			fall_stata=2;  // 转入状态2：等待稳定
			fall_cnta0=0;
		}
		
		record_ds_turn=0;
		flager_a &=~ERRB_DS;  // 放气期间不检测堵塞
	}
	else if (fall_stata==2)  // 状态2：等待压力稳定
	{
		fall_cnta0++;
		if (fall_cnta0 > VALVE2_STABILIZE_CYCLES)  // 等待压力稳定
		{
			fall_stata=0;  // 返回状态0
			fall_cnta0=0;
			
			if (adc_ps00<(mod_seta_preh+con_hi_delta))  // 如果压力仍未达到目标
			{
				con_flg_falla=0;  // 清除补气标志：不需要继续补气了
				gao_cnt=0;        // 清零高压计数
			}
		}
		
		record_ds_turn=0;
		flager_a &=~ERRB_DS;  // 稳定期间不检测堵塞
	}
}

/**
 * 函数: get_press_delta
 * 功能: 计算动态压力阈值
 * 参数: press - 目标压力值（mmHg）
 * 
 * 说明:
 *   根据目标压力值动态计算控制死区，实现自适应控制
 *   - con_lo_delta: 下阈值增量（开始补气的偏差）
 *   - con_hi_delta: 上阈值增量（停止补气的偏差）
 * 
 * 控制范围示例:
 *   目标120mmHg → 补气范围：108~126 mmHg (±10%)
 *   目标200mmHg → 补气范围：180~210 mmHg (-10%/+5%)
 */
void get_press_delta(unsigned short press)
{
	unsigned short delta = 8;
	
#ifdef LOGO_TYPE_DEROYAL  // Deroyal版本：较小的死区
	if (press<20)  // 极低压：±2 mmHg
	{
		delta = 2;
		con_hi_delta = 2;
	}
	else if (press >= 80)  // 高压：±5~7.5%
	{
		delta = press/20;           // 5%
		con_hi_delta = delta*15/10; // 7.5%
	}
	else  // 中压：±10~17%
	{
		delta = press/10;           // 10%
		con_hi_delta = delta*17/10; // 17%
	}
	con_lo_delta = delta;
	con_hi_delta = delta+4;
	
#else  // 其他版本：较大的死区，减少补气次数
	con_lo_delta = press/10;  // 下阈值：目标压力的10%
	
	if (press > 200)  // 高压段：±5%
	{
		con_hi_delta = press/20;
	}
	else if (press >= 80)  // 中压段：±9~10%
	{
		con_hi_delta = press/20+4;  // 5% + 4mmHg
	}
	else  // 低压段：±10~12 mmHg
	{
		con_hi_delta = press/10+2;  // 10% + 2mmHg
	}
#endif
} 

/**
 * 函数: calc_delta_i
 * 功能: 计算两个值的差值
 * 参数: counts1 - 被减数, counts2 - 减数
 * 返回: 差值（最小值为5）
 * 说明: 用于计算压力控制的死区下限
 */
static unsigned short calc_delta_i(unsigned short counts1,unsigned short counts2 )
{
	unsigned short rst = 0;
	if ( counts1 >= counts2 )
		{rst = (counts1 - counts2);}
	if (rst<5)  // 确保最小死区为5mmHg
	{rst = 5;}
	return rst;
}

/******************** 压力控制状态变量 ********************/
unsigned char oppump_flg = 0;          // 气泵运行标志
unsigned short  np_good_times=0;       // 正常压力持续次数
unsigned char   overabc = 0;           // 泄漏超过10次标志：1=泄漏严重，需停止补气
unsigned short   val_cnt=0;            // 阀门控制计数器

/* 堵塞检测相关 */
unsigned long scan_dusai_time = 0;     // 堵塞检测时间累加器（单位：20ms）
unsigned char scan_turn=0;             // 扫描轮次计数
unsigned char scan_completed=0;        // 扫描完成标志
signed int    calc_result=0;           // 压力计算结果

/* 压力记录数组（用于堵塞检测） */
unsigned short record_ds[8];           // 记录最近8次的压力值
unsigned char  record_ds_turn=0;       // 当前记录位置（0~7循环）
unsigned short twenty_seconds=0;       // 记录间隔计时器

void STAT_conNewa(void)
{
	unsigned short temp=0;
	unsigned short run_time=0;
	if (mod_seta_preh==0)
	{
		CLS_PwmA();
		if (val_cnt++>1000)
		{
			VAL2 = 0;
			val_cnt=1100;
		}
		else
		{
			VAL2=1;
		}
		return;
	}
	else
	{
		get_press_delta(mod_seta_preh);
		val_cnt=0;
	}
	if (flager_a&ERRA_S)
	{
		np_good_times=0;
		VAL1 = 0 ;
		VAL2 = 0 ;
		
		CLS_PwmA();
	}
	else
	{
		temp = calc_delta_i(mod_seta_preh,con_lo_delta);
		unsigned short target_press = mod_seta_preh+con_hi_delta;
		if(target_press >= TOP_PRESSURE_MAX)
			target_press = TOP_PRESSURE_MAX-3;
		if (adc_ps00<=temp)
		{
			
			record_ds_turn=0;
			twenty_seconds=0;
			
			nggood_check++;
			if(nggood_check>49)
			{
				nggood_check=0;
				gao_cnt=0;
			}
			good_check=0;
			np_good_times=0;
			scan_dusai_time = 0;
			debug_times=0;
			debug_thirtys=0;
			flager_a &=~ERRB_DS;
			if (con_flg_falla==0)
			{
				BOX_FQ_TURNS++;
				if (BOX_FQ_TURNS>10)  
				
				{
					BOX_FQ_TURNS=0;
					
					if (mod_jixa!=1)
						VAL2 = 0 ;
					else
					{
						if(jx_current_phase==2)	
							VAL2 = 0 ;
					}

					if (flager_a&ERRA_LQ)
					{
					    
						if (load_perioda_up>JUDGE_LEAKAGE_TIMEOUT+1200  
						    || load_perioda_up>(JUDGE_LEAKAGE_TIMEOUT)*JX_MODE_TIME_FACTOR_BY_LX 
							)
#if IS_NO_LIQUID_ALARM_STOP==0
						{

							CLS_PwmA();
							overabc = 1;
						}
						else
#endif
						{

							OPEN_PwmA();
							overabc = 0;
						}
					}
					else
					{

						OPEN_PwmA();
						overabc = 0;
					}
				}
			}
		}

		else if (adc_ps00>=(target_press))
		{
			gao_cnt++;
			if (gao_cnt>999)
				gao_cnt=1000;
			
			run_time=mod_seta_preh/10+2;
			
			if(run_time < 15)
			{
				run_time=15;
			}
			
			if(mod_main_a == MOD_JIX)
			{
				run_time=run_time+4;
			}
			run_time=run_time*20;
			if(run_time<=20)
				run_time=20;
			if(run_time>=400)
				run_time=400;
			if (gao_cnt>run_time) 
			{
				CLS_PwmA();
				PUMP=0;
				open_bum=0;
			}
			if (gao_cnt>(run_time+JUDGE_YWM_AFTER_PUMP_STOP)) 
			{

 				if (con_flg_falla==0)
				{
					cnt_cnta++;	
					oppump_flg = 0;
					con_flg_falla=1;
					fall_stata=0;
					fall_cnta0=0;
					gao_cnt=0;
				} 	
				
			}
			record_ds_turn=0;
			twenty_seconds=0;
			good_check=0;
			np_good_times=0;
		}
		else
		{
			#if IS_NO_LIQUID_ALARM_STOP==0
			if (flager_a&ERRA_LQ)
			{
				if (load_perioda_up>JUDGE_LEAKAGE_TIMEOUT+1200
				|| load_perioda_up>(JUDGE_LEAKAGE_TIMEOUT)*JX_MODE_TIME_FACTOR_BY_LX 
				)
				{
					
					CLS_PwmA();
				}
			}
			#endif
			if (scan_dusai_time++>JUDGE_DS_TIME)
			{
				flager_a|=ERRB_DS;
			}
			
			good_check++;
			if(good_check>49)
			{
				nggood_check=0;
				good_check=0;
				gao_cnt=0;
			}
			
			twenty_seconds++;
			
			unsigned short record_pre_compare_interval;
			if(adc_ps00 >= MIN_PRESS_REPORT_DS)
			{
				
				if(mod_main_a == MOD_JIX)
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL*JX_MODE_TIME_FACTOR_BY_LX;
				}
				else
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL;
				}
			}
			else
			{
				
				if(mod_main_a == MOD_JIX)
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL_LOW*JX_MODE_TIME_FACTOR_BY_LX;
				}
				else
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL_LOW;
				}
			}
			if(twenty_seconds>=record_pre_compare_interval)
			{
				record_ds[record_ds_turn]=adc_ps00;
				record_ds_turn++;
				if(record_ds_turn>7)
				{
					if((record_ds[7] == record_ds[0])||((record_ds[7]+1) == record_ds[0]))
					{

						if(record_ds[7] >= MIN_PRESS_REPORT_DS)
						{
							flager_a|=ERRB_DS;
						}
						else if(record_ds[7] > NO_REPORE_PRESS_FOR_DS)
						{
							
							if((record_ds[0]+record_ds[1]+record_ds[2]+record_ds[3]+record_ds[4]+record_ds[5]+record_ds[6])/7 == record_ds[7])
							{
								flager_a|=ERRB_DS;
							}
						
}
					}
					else
					{
						flager_a &=~ERRB_DS;
					}
					
					record_ds[0]=record_ds[1];
					record_ds[1]=record_ds[2];
					record_ds[2]=record_ds[3];
					record_ds[3]=record_ds[4];
					record_ds[4]=record_ds[5];
					record_ds[5]=record_ds[6];
					record_ds[6]=record_ds[7];
					record_ds_turn=7;
				}
				twenty_seconds=0;
			}

		}
		if (con_flg_falla)
		{
			CONTR_fallaNew();
		}
	}
}

