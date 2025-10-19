#include  "include.h"	
#include  "sys_cpu.h"
#include  "BIOS_JLX1864G_139.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include   "npwt_dis_main.h"
#include   "npwt_dis_ofile_lcd_02.h"

unsigned char   ccnt1;

#define    BAT4          319     
#define    BAT3          304     
#define    BAT2          296     
#define    BAT1          277     
#define    BAT0          269     

#define    WJ_LEV        150     

#define    YEW_SES       15  
#define    LQ_SES        10

unsigned short     LQ_FIR=0;
 
unsigned short  cntcnt=0;
extern  unsigned char   overabc;

void LEVEL_WarnA(void)
{
	if(mod_main_a == MOD_LIX)
	{
		LQ_FIR=LEAKAGE_TIMEOUT_FOR_LIX;
	}
	else if(mod_main_a == MOD_JIX)
	{
		LQ_FIR=LEAKAGE_TIMEOUT_FOR_JIX;
	}
	else
	{
		LQ_FIR=SET_LEAKAGE_FLAG_CYCLE;
	}

	if(mod_seta_preh==0)
	{
		;
	}
	else
	{
		if(open_bum==FALSE)
		{
			ccnt1=0;
			if(overabc)
			{;}
			else
			{
				if(cntcnt++>=CANCEL_LEAKAGE_FLAG_CYCLE)
				{
					cntcnt=0;
					flager_a = flager_a&(~ERRA_LQ);
					load_perioda_up=0;
				}
			}
		}
		else
		{
			cntcnt=0;
			if(ccnt1++>=10)
			{
				ccnt1=0;
				#if IS_NO_LIQUID_ALARM_STOP == 0

				if(load_perioda_up++>JUDGE_LEAKAGE_TIMEOUT)
				{
					load_perioda_up=JUDGE_LEAKAGE_TIMEOUT+2200;
				}
			
				else if(load_perioda_up>LQ_FIR)
				{
					flager_a = flager_a|ERRA_LQ;
				}
				else
					{flager_a = flager_a&(~ERRA_LQ);}
				#else

				if(load_perioda_up++>LQ_FIR)
					{flager_a = flager_a|ERRA_LQ;}
				else
					{flager_a = flager_a&(~ERRA_LQ);}

				#endif			

			}
		}
	}

	if(cnt_cnta > 0)
	{
		cnt_cntaa++;

		if(cnt_cntaa<=JUDGE_CANISTER_FULL_TIME)
		{
			if(cnt_cnta > JUDGE_CANISTER_FULL_COUNT)
			{
				if(mod_main_a!=MOD_JIX)
				{
					flager_a = flager_a|ERRA_S;
				}
				else
				{
					if (mod_jixa==0)
					{
						flager_a = flager_a|ERRA_S;
					}
					if (mod_jixa==1)
					{
						if(ddfq==0)
							flager_a = flager_a|ERRA_S;
					}
				}
				
				cnt_cnta=0;
				cnt_cntaa=0;
			}
		}
		else
		{
			cnt_cnta=0;
			cnt_cntaa=0;
		}
	}
	else
		cnt_cntaa=0;

}

unsigned short  cntx=0;
unsigned char   bat_sas=0,bat_sas_bak=0;

void BAT_WarnFir(void)
{

	bat_lev=0;
	bat_sas=bat_sas_bak=0;
	
}

void BAT_Warn(void)
{
	if (adc_bat<BAT0)
	{
		bat_sas=LOWER_THAN_3_5V;
	}
	else if (adc_bat<BAT1)
	{
		bat_sas=LOWER_BAT_WARN_3_6V;
	}
	else if (adc_bat<BAT2)
	{
		bat_sas=LOWER_THAN_3_7V;
	}
	else if (adc_bat<BAT3)
	{
		bat_sas=LOWER_THAN_3_8V;
	}
	else if (adc_bat<BAT4)
	{
		bat_sas=LOWER_THAN_4V;
	}
	else
	{
		bat_sas=BAT_FULL;
	}

	if (bat_sas==bat_sas_bak)
	{

		if (cntx++>49)
		{
			bat_lev=bat_sas;
		}
	}
	else
	{
		cntx=0;
	}

	bat_sas_bak = bat_sas;
	
	if (BAT_CHARGE==BAT_CHARGING)
	{
		bat_lev=bat_lev|0x80;    
	}
	else
	{
		bat_lev=bat_lev&0x7f;
	}
}

void Warn1(void)
{
	BAT_Warn();	
}

