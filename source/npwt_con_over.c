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

unsigned short debug_times = 0;
unsigned short debug_thirtys = 0;
unsigned short debug_air=0;
unsigned char  debug=0;
void CONTR_fallaNew(void)
{
	if (fall_stata==0)
	{
		fall_stata=1;
		fall_cnta0=0;
		VAL2 = 1 ;
		
		record_ds_turn=0;
		flager_a &=~ERRB_DS;
	}
	else if (fall_stata==1)
	{
		fall_cnta0++;
		unsigned short fall_cnta0_count = 4;
		if(mod_seta_preh <=50)
			fall_cnta0_count = 16;
		if (fall_cnta0>fall_cnta0_count) 
		{
			VAL2 = 0 ;
			fall_stata=2;
			fall_cnta0=0;
		}
		
		record_ds_turn=0;
		flager_a &=~ERRB_DS;
	}
	else if (fall_stata==2)
	{
		fall_cnta0++;
		if (fall_cnta0>50)  
		{
			fall_stata=0;
			fall_cnta0=0;
			if (adc_ps00<(mod_seta_preh+con_hi_delta))
			{

				con_flg_falla=0;
				gao_cnt=0;
			}
		}
		
		record_ds_turn=0;
		flager_a &=~ERRB_DS;
		
	}
}

void get_press_delta(unsigned short press)
{
	unsigned short delta = 8;
	
#ifdef LOGO_TYPE_DEROYAL
	if (press<20)
	{
		delta = 2;
		con_hi_delta = 2;
	}
	else if (press >= 80)
	{
		delta = press/20;
		con_hi_delta = delta*15/10;
	}
	else
	{
		delta = press/10;
		con_hi_delta = delta*17/10;
	}
	con_lo_delta = delta;
	con_hi_delta = delta+4;
#else
	con_lo_delta = press/10;
	
	if (press > 200)
	{
		con_hi_delta = press/20;
	}
	else if (press >= 80)
	{
		con_hi_delta = press/20+4;
	}
	else
	{
		con_hi_delta = press/10+2;
	}
#endif
	
} 

static unsigned short calc_delta_i(unsigned short counts1,unsigned short counts2 )
{
	unsigned short rst = 0;
	if ( counts1 >= counts2 )
		{rst = (counts1 - counts2);}
	if (rst<5)
	{rst = 5;
	}
		return rst;
}
unsigned char oppump_flg = 0;
unsigned short  np_good_times=0;
unsigned char   overabc = 0;
unsigned short   val_cnt=0;
unsigned long scan_dusai_time = 0;
unsigned char xxturn=0;
unsigned char xxok=0;
signed int result=0;

unsigned short record_ds[8];
unsigned char  record_ds_turn=0;
unsigned short twenty_seconds=0;

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
						if(ddfq==2)	
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

