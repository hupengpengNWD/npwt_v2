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
// #include "EEP.h" 
/*
压力传感器：0-----105
            200---309
            100---207
            50----155
            400---516
            500---604
            550---670
            540---660
*/

unsigned char   ccnt1;
/*********************************
3.5v   275
3.6v   285
3.7v   295
3.8v   305
4.0v   315
充电中 CHARGE=0
充满： CHARGE=1
断开： CHARGE=1

***************RUI*****************/
// #define    BAT4          315     ///////4.0v
// #define    BAT3          305     ///////3.8v
// #define    BAT2          295     ///////3.7v
// #define    BAT1          285     ///////3.6v
// #define    BAT0          275     ///////3.5v
/*************softlee*******************/
// #define    BAT4          305     ///////4.0v
// #define    BAT3          290     ///////3.8v
// #define    BAT2          283     ///////3.7v
// #define    BAT1          275     ///////3.6v
// #define    BAT0          267     ///////3.5v
/*************ma 20160506日修改 防止电池显示3格工作后黑屏，让用户产生有充电的紧迫感*******************/
//将上面的3个增大
//下面的2个不变，工作时长不变
#define    BAT4          319     ///////4.15v
#define    BAT3          304     ///////3.95v
#define    BAT2          296     ///////3.85v
#define    BAT1          277     ///////3.6v
#define    BAT0          269     ///////3.5v
/********************************/



#define    WJ_LEV        150     ///////////////////////////////////////////未接液盒值
////#define    LQ_FIR    1100    ///////////////////////////////////////////漏气初值
#define    YEW_SES       15  
#define    LQ_SES        10

unsigned short     LQ_FIR=0;
 
unsigned short  cntcnt=0;
extern  unsigned char   overabc;

// lwz 检测漏气并判断液位满
void LEVEL_WarnA(void)/*used*/
{
	if(mod_main_a == MOD_LIX)
	{
		LQ_FIR=LEAKAGE_TIMEOUT_FOR_LIX;/*漏气2分钟*/
	}
	else if(mod_main_a == MOD_JIX)
	{
		LQ_FIR=LEAKAGE_TIMEOUT_FOR_JIX;/*漏气2分钟*/
	}
	else
	{
		LQ_FIR=SET_LEAKAGE_FLAG_CYCLE;/*漏气2分钟*/
	}
		// LQ_FIR=1100;/*漏气1分钟*/  //20160331
	/**********************************************************************/
	if(mod_seta_preh==0)
	{
		;
	}
	else
	{
		if(open_bum==FALSE)//泵无启动
		{
			ccnt1=0;
			if(overabc)//漏气的时间已经达到了10分钏，将overabc=1;
			{;}
			else// lwz 如果一段时间内没有漏气10分钟以上的标志，则清除漏气标志
			{
				if(cntcnt++>=CANCEL_LEAKAGE_FLAG_CYCLE)
				{
					cntcnt=0;
					flager_a = flager_a&(~ERRA_LQ);/*漏气标志*/
					load_perioda_up=0;/*漏气时间清零*/
				}
			}
		}
		else//泵有启动
		{
			cntcnt=0;
			if(ccnt1++>=10)
			{
				ccnt1=0;
				#if IS_NO_LIQUID_ALARM_STOP == 0
			//	if (adc_ps00<(mod_seta_preh-con_lo_delta))//20160506 日MA ADD
			//	{
				if(load_perioda_up++>JUDGE_LEAKAGE_TIMEOUT)////////////////////10分钟到
				{
                    load_perioda_up=JUDGE_LEAKAGE_TIMEOUT+2200;
                }///////////////停泵  
			//	}
				else if(load_perioda_up>LQ_FIR)/////////////////1分钟时间
				{
					flager_a = flager_a|ERRA_LQ;// lwz 在SET_LEAKAGE_FLAG_CYCLE周期后设置漏气标志
				}
				else
					{flager_a = flager_a&(~ERRA_LQ);}// lwz 在SET_LEAKAGE_FLAG_CYCLE周期内取消漏气标志
				#else

				if(load_perioda_up++>LQ_FIR)/////////////////1分钟时间
					{flager_a = flager_a|ERRA_LQ;}
				else
					{flager_a = flager_a&(~ERRA_LQ);}

				#endif			

			}
		}
	}
	// if(cnt_cntaa++>=4000)/////20秒
 	// if(cnt_cntaa++>=6000)/////30秒    20160401
	// {
		// if(cnt_cnta>4)//20秒内有4次震荡
			// flager_a = flager_a|ERRA_S;//////////液位满
	    // cnt_cnta=0;
		// cnt_cntaa=0;
	// } 
	
	if(cnt_cnta > 0)
	{
		cnt_cntaa++;
        //高压的时候，液满震荡的时间长，YUKI20160606拿了一台200mmh液位满无法判断
		// if(cnt_cntaa<=6000)//20秒内有4次震荡   20160420
		if(cnt_cntaa<=JUDGE_CANISTER_FULL_TIME)//20秒内有4次震荡   20160608 30秒报5次很紧张，改为40秒比较保险 200mmHG的时候0气压冲上去较慢
		{
			if(cnt_cnta > JUDGE_CANISTER_FULL_COUNT)// lwz 震荡次数大于4，则开始判断液位满
			{
				if(mod_main_a!=MOD_JIX)// lwz 如果不是间歇模式，则判断为液位满
				{
					flager_a = flager_a|ERRA_S;//////////液位满
				}
				else// lwz 如果是间歇模式，则在以下泵开启的工作状态下，可以判断为液位满
				{
					if (mod_jixa==0)// lwz 并且当前处于间歇模式的高压状态，则判断为液位满
					{
						flager_a = flager_a|ERRA_S;//////////液位满
					}
					if (mod_jixa==1)// lwz 并且当前处于间歇模式的低压状态，则判断为液位满
					{
						if(ddfq==0)// lwz 并且当前不处于放气状态，则判断为液位满
							flager_a = flager_a|ERRA_S;//////////液位满
					}
				}
				// lwz 清零计数
				cnt_cnta=0;
				cnt_cntaa=0;
			}
		}
		else
		{// lwz 30秒后，计时器清零
			cnt_cnta=0;
			cnt_cntaa=0;
		}
	}
	else
		cnt_cntaa=0;
	
	
	
	/**********************************************************************/
}

unsigned short  cntx=0;
unsigned char   bat_sas=0,bat_sas_bak=0;
// lwz 将adc采集所得的数据转换为当前的电压
void BAT_WarnFir(void)/*used 开机时候监测电量*/
{
	// if(adc_bat<BAT0)
		// {bat_lev=5;}
	// else if(adc_bat<BAT1)
		// {bat_lev=10;}
    // else if(adc_bat<BAT2)
		// {bat_lev=1;}
	// else if(adc_bat<BAT3)
		// {bat_lev=2;}
	// else if(adc_bat<BAT4)
		// {bat_lev=3;}
	// else
	        // {bat_lev=4;}
    bat_lev=0;
	bat_sas=bat_sas_bak=0;
	
}
// lwz 电池电量的计算
void BAT_Warn(void)/*used 电池状态*/
{
	if (adc_bat<BAT0)
	{
		bat_sas=LOWER_THAN_3_5V;// lwz 电量空格
	}
	else if (adc_bat<BAT1)
	{
		bat_sas=LOWER_BAT_WARN_3_6V;// lwz 电量一格
	}
	else if (adc_bat<BAT2)
	{
		bat_sas=LOWER_THAN_3_7V;// lwz 电量一格
	}
	else if (adc_bat<BAT3)
	{
		bat_sas=LOWER_THAN_3_8V;// lwz 电量二格
	}
	else if (adc_bat<BAT4)
	{
		bat_sas=LOWER_THAN_4V;// lwz 电量三格
	}
	else
	{
		bat_sas=BAT_FULL;// lwz 电量四满格
	}

	if (bat_sas==bat_sas_bak)
	{
		// if (cntx++>250)//020160511 edit ma
		// if (cntx++>25)
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
	// lwz 如果处于充电状态，则设置第12位；否则清零第12位；
	if (BAT_CHARGE==BAT_CHARGING)
	{
		bat_lev=bat_lev|0x80;    /////////////////充电中，将第7位置位
	}
	else
	{
		bat_lev=bat_lev&0x7f;// 不在充电中，将第7位清空
	}
}


void Warn1(void)
{
	BAT_Warn();	
}

