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
Ñ¹Á¦´«¸ÐÆ÷£º0-----105
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
³äµçÖÐ CHARGE=0
³äÂú£º CHARGE=1
¶Ï¿ª£º CHARGE=1

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
/*************ma 20160506ÈÕÐÞ¸Ä ·ÀÖ¹µç³ØÏÔÊ¾3¸ñ¹¤×÷ºóºÚÆÁ£¬ÈÃÓÃ»§²úÉúÓÐ³äµçµÄ½ôÆÈ¸Ð*******************/
//½«ÉÏÃæµÄ3¸öÔö´ó
//ÏÂÃæµÄ2¸ö²»±ä£¬¹¤×÷Ê±³¤²»±ä
#define    BAT4          319     ///////4.15v
#define    BAT3          304     ///////3.95v
#define    BAT2          296     ///////3.85v
#define    BAT1          277     ///////3.6v
#define    BAT0          269     ///////3.5v
/********************************/



#define    WJ_LEV        150     ///////////////////////////////////////////Î´½ÓÒººÐÖµ
////#define    LQ_FIR    1100    ///////////////////////////////////////////Â©Æø³õÖµ
#define    YEW_SES       15  
#define    LQ_SES        10

unsigned short     LQ_FIR=0;
 
unsigned short  cntcnt=0;
extern  unsigned char   overabc;

// lwz ¼ì²âÂ©Æø²¢ÅÐ¶ÏÒºÎ»Âú
void LEVEL_WarnA(void)/*used*/
{
	if(mod_main_a == MOD_LIX)
	{
		LQ_FIR=LEAKAGE_TIMEOUT_FOR_LIX;/*Â©Æø2·ÖÖÓ*/
	}
	else if(mod_main_a == MOD_JIX)
	{
		LQ_FIR=LEAKAGE_TIMEOUT_FOR_JIX;/*Â©Æø2·ÖÖÓ*/
	}
	else
	{
		LQ_FIR=SET_LEAKAGE_FLAG_CYCLE;/*Â©Æø2·ÖÖÓ*/
	}
		// LQ_FIR=1100;/*Â©Æø1·ÖÖÓ*/  //20160331
	/**********************************************************************/
	if(mod_seta_preh==0)
	{
		;
	}
	else
	{
		if(open_bum==FALSE)//±ÃÎÞÆô¶¯
		{
			ccnt1=0;
			if(overabc)//Â©ÆøµÄÊ±¼äÒÑ¾­´ïµ½ÁË10·ÖîË£¬½«overabc=1;
			{;}
			else// lwz Èç¹ûÒ»¶ÎÊ±¼äÄÚÃ»ÓÐÂ©Æø10·ÖÖÓÒÔÉÏµÄ±êÖ¾£¬ÔòÇå³ýÂ©Æø±êÖ¾
			{
				if(cntcnt++>=CANCEL_LEAKAGE_FLAG_CYCLE)
				{
					cntcnt=0;
					flager_a = flager_a&(~ERRA_LQ);/*Â©Æø±êÖ¾*/
					load_perioda_up=0;/*Â©ÆøÊ±¼äÇåÁã*/
				}
			}
		}
		else//±ÃÓÐÆô¶¯
		{
			cntcnt=0;
			if(ccnt1++>=10)
			{
				ccnt1=0;
				#if IS_NO_LIQUID_ALARM_STOP == 0
			//	if (adc_ps00<(mod_seta_preh-con_lo_delta))//20160506 ÈÕMA ADD
			//	{
				if(load_perioda_up++>JUDGE_LEAKAGE_TIMEOUT)////////////////////10·ÖÖÓµ½
				{
					load_perioda_up=JUDGE_LEAKAGE_TIMEOUT+2200;
				}///////////////Í£±Ã  
			//	}
				else if(load_perioda_up>LQ_FIR)/////////////////1·ÖÖÓÊ±¼ä
				{
					flager_a = flager_a|ERRA_LQ;// lwz ÔÚSET_LEAKAGE_FLAG_CYCLEÖÜÆÚºóÉèÖÃÂ©Æø±êÖ¾
				}
				else
					{flager_a = flager_a&(~ERRA_LQ);}// lwz ÔÚSET_LEAKAGE_FLAG_CYCLEÖÜÆÚÄÚÈ¡ÏûÂ©Æø±êÖ¾
				#else

				if(load_perioda_up++>LQ_FIR)/////////////////1·ÖÖÓÊ±¼ä
					{flager_a = flager_a|ERRA_LQ;}
				else
					{flager_a = flager_a&(~ERRA_LQ);}

				#endif			

			}
		}
	}
	// if(cnt_cntaa++>=4000)/////20Ãë
 	// if(cnt_cntaa++>=6000)/////30Ãë    20160401
	// {
		// if(cnt_cnta>4)//20ÃëÄÚÓÐ4´ÎÕðµ´
			// flager_a = flager_a|ERRA_S;//////////ÒºÎ»Âú
	    // cnt_cnta=0;
		// cnt_cntaa=0;
	// } 
	
	if(cnt_cnta > 0)
	{
		cnt_cntaa++;
		//¸ßÑ¹µÄÊ±ºò£¬ÒºÂúÕðµ´µÄÊ±¼ä³¤£¬YUKI20160606ÄÃÁËÒ»Ì¨200mmhÒºÎ»ÂúÎÞ·¨ÅÐ¶Ï
		// if(cnt_cntaa<=6000)//20ÃëÄÚÓÐ4´ÎÕðµ´   20160420
		if(cnt_cntaa<=JUDGE_CANISTER_FULL_TIME)//20ÃëÄÚÓÐ4´ÎÕðµ´   20160608 30Ãë±¨5´ÎºÜ½ôÕÅ£¬¸ÄÎª40Ãë±È½Ï±£ÏÕ 200mmHGµÄÊ±ºò0ÆøÑ¹³åÉÏÈ¥½ÏÂý
		{
			if(cnt_cnta > JUDGE_CANISTER_FULL_COUNT)// lwz Õðµ´´ÎÊý´óÓÚ4£¬Ôò¿ªÊ¼ÅÐ¶ÏÒºÎ»Âú
			{
				if(mod_main_a!=MOD_JIX)// lwz Èç¹û²»ÊÇ¼äÐªÄ£Ê½£¬ÔòÅÐ¶ÏÎªÒºÎ»Âú
				{
					flager_a = flager_a|ERRA_S;//////////ÒºÎ»Âú
				}
				else// lwz Èç¹ûÊÇ¼äÐªÄ£Ê½£¬ÔòÔÚÒÔÏÂ±Ã¿ªÆôµÄ¹¤×÷×´Ì¬ÏÂ£¬¿ÉÒÔÅÐ¶ÏÎªÒºÎ»Âú
				{
					if (mod_jixa==0)// lwz ²¢ÇÒµ±Ç°´¦ÓÚ¼äÐªÄ£Ê½µÄ¸ßÑ¹×´Ì¬£¬ÔòÅÐ¶ÏÎªÒºÎ»Âú
					{
						flager_a = flager_a|ERRA_S;//////////ÒºÎ»Âú
					}
					if (mod_jixa==1)// lwz ²¢ÇÒµ±Ç°´¦ÓÚ¼äÐªÄ£Ê½µÄµÍÑ¹×´Ì¬£¬ÔòÅÐ¶ÏÎªÒºÎ»Âú
					{
						if(ddfq==0)// lwz ²¢ÇÒµ±Ç°²»´¦ÓÚ·ÅÆø×´Ì¬£¬ÔòÅÐ¶ÏÎªÒºÎ»Âú
							flager_a = flager_a|ERRA_S;//////////ÒºÎ»Âú
					}
				}
				// lwz ÇåÁã¼ÆÊý
				cnt_cnta=0;
				cnt_cntaa=0;
			}
		}
		else
		{// lwz 30Ãëºó£¬¼ÆÊ±Æ÷ÇåÁã
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
// lwz ½«adc²É¼¯ËùµÃµÄÊý¾Ý×ª»»Îªµ±Ç°µÄµçÑ¹
void BAT_WarnFir(void)/*used ¿ª»úÊ±ºò¼à²âµçÁ¿*/
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
// lwz µç³ØµçÁ¿µÄ¼ÆËã
void BAT_Warn(void)/*used µç³Ø×´Ì¬*/
{
	if (adc_bat<BAT0)
	{
		bat_sas=LOWER_THAN_3_5V;// lwz µçÁ¿¿Õ¸ñ
	}
	else if (adc_bat<BAT1)
	{
		bat_sas=LOWER_BAT_WARN_3_6V;// lwz µçÁ¿Ò»¸ñ
	}
	else if (adc_bat<BAT2)
	{
		bat_sas=LOWER_THAN_3_7V;// lwz µçÁ¿Ò»¸ñ
	}
	else if (adc_bat<BAT3)
	{
		bat_sas=LOWER_THAN_3_8V;// lwz µçÁ¿¶þ¸ñ
	}
	else if (adc_bat<BAT4)
	{
		bat_sas=LOWER_THAN_4V;// lwz µçÁ¿Èý¸ñ
	}
	else
	{
		bat_sas=BAT_FULL;// lwz µçÁ¿ËÄÂú¸ñ
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
	// lwz Èç¹û´¦ÓÚ³äµç×´Ì¬£¬ÔòÉèÖÃµÚ12Î»£»·ñÔòÇåÁãµÚ12Î»£»
	if (BAT_CHARGE==BAT_CHARGING)
	{
		bat_lev=bat_lev|0x80;    /////////////////³äµçÖÐ£¬½«µÚ7Î»ÖÃÎ»
	}
	else
	{
		bat_lev=bat_lev&0x7f;// ²»ÔÚ³äµçÖÐ£¬½«µÚ7Î»Çå¿Õ
	}
}


void Warn1(void)
{
	BAT_Warn();	
}

