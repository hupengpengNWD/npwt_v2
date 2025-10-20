/****************************************************************************
 * 文件名: npwt_con_ofile_load_00.c
 * 功能: 负载控制模块（气泵和电磁阀控制）
 * 
 * 主要功能:
 *   1. 压力计算：ADC值 → mmHg
 *   2. PWM占空比查表（根据目标压力）
 *   3. 泄漏检测
 *   4. 液位检测
 *   5. 电磁阀控制（VAL1排气阀、VAL2放气阀）
 * 
 * 控制参数查找表:
 *   mot_sys[]: 根据目标压力查找det00控制参数
 *              用于调整不同压力段的响应速度
 * 
 * 调用关系:
 *   主调用: PRESS_ConA() → PRESS_ConSubA() → STAT_conNewa()
 *   被调用: 定时器3中断每100ms调用一次PRESS_ConA()
 ****************************************************************************/

#include   "include.h"
#include "system_manager.h"
#include "global_compat.h"
#include "hardware_abstraction.h"
#include   "npwt_dis_main.h"

#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"

unsigned short	 load_perioda_up;
unsigned short   cnt_cnta,cnt_cntaa;

unsigned char    bump_need_out_air_flg;

unsigned short   mod_seta_prehh;
float    pwm_k1;
unsigned short   fq_cnt1=0;          // 放气计数器

/**
 * 函数: OPEN_PwmA
 * 功能: 开启PWM控制（允许气泵工作）
 * 说明: 设置排气需求标志的高4位
 */
void OPEN_PwmA(void)
{
	bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
}

/**
 * 函数: CLS_PwmA
 * 功能: 关闭PWM控制（禁止气泵工作）
 * 说明: 清除排气需求标志的高4位
 */
void CLS_PwmA(void)
{
	bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
}

const  char mot_sys[]={
	0,  0,
	25, 25, 25, 25, 25, 25, 30, 25, 25,         

	25, 25, 20, 20, 25, 25, 25, 25, 25,  20,    

	20, 20, 20, 20, 15, 15, 15, 15, 15,  15     

};

void PRESS_ConSubA(void)
{
	unsigned short i;

	LEVEL_WarnA();

	mod_seta_prehh=mod_seta_preh;
	if (adc_ps0>adc_zero)
	{
		pwm_k1    =    (adc_ps0-adc_zero)/valueK;
	}
	else
	{
		pwm_k1=0;
	}
	adc_ps00  =    (unsigned int)pwm_k1;
	if (adc_ps00>TOP_PRESSURE_MAX)
	{
		adc_ps00=TOP_PRESSURE_MAX;
	}
	pwm_k1    =    (mod_seta_preh-50.0)*0.6+10;

	if (mod_seta_preh<=20)
	{
		pwm_cnt2=4;    
	}
	else if (mod_seta_preh<70)        
	{
		pwm_cnt2  =    4;    
	}

	else if (mod_seta_preh<100)       
	{
		pwm_cnt2  =    4;    
	}

	else if (mod_seta_preh<=290)
	{
		pwm_cnt2  =    5;    
	}
	else
	{
		pwm_cnt2  =    6;    
	} 	

	i       =     mod_seta_preh/10;
	det00   =     mot_sys[i];
	STAT_conNewa();
}

void PRESS_ConA(void)
{
	switch (mod_main_a)
	{
	case MOD_SYS:
	{
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;
		break;
	}
	case MOD_WAT:
	{
		HAL_Valve1_Close();
		HAL_Valve2_Close();
		
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		load_perioda_up=0;
		break;
	}
	case MOD_LIX:
	{
		PRESS_ConSubA();
		break;
	}
	case MOD_JIX:
	{  
		if((jx_current_phase!=1)&&(jx_current_phase!=2))
		
			PRESS_ConSubA();
		else
			
		break;
	}
	case MOD_ZXB:
	{
		PRESS_ConSubA();
		break;
	}
	case MOD_ZHT:
	{
		HAL_Valve1_Close();
		HAL_Valve2_Close();
		
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;
		CLS_PwmA();
		break;;
	}
	case MOD_SET:
	{
		HAL_Valve1_Close();
		HAL_Valve2_Close();
		
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;
		CLS_PwmA();
		break;;
	}
	case MOD_ERR:
	{
		break;
	}
	case MOD_OFF:
	{
		HAL_Valve1_Close();
		HAL_Valve2_Close();
		CLS_PwmA();
		break;
	}
	default:
	{
		break;
	}
	}
}

