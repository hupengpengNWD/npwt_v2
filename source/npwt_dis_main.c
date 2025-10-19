
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
// __CONFIG(1,WDTEN_OFF & XINST_OFF );
// __CONFIG(2,OSC_INTOSCPLL);
// __CONFIG(3,DSWDTEN_OFF);
// __CONFIG(4,WPCFG_OFF);
//
//#pragma config WDTEN = ON,XINST = OFF
//#pragma config OSC = INTOSCPLL
//#pragma config DSWDTEN = OFF
//#pragma config WPCFG = OFF
//#pragma config CP0 = OFF 
//#pragma config WDTPS = 16384
//#pragma config IOL1WAY = OFF
//#pragma config WPDIS  = OFF

// lwz Éú³ÉÅäÖÃÎ»£¬²¢·ÅÈëÒ»¸öµ¥¶ÀµÄÎÄ¼þÖÐ
#include "../project/config_bits.h"

unsigned char    audio_flg;
unsigned short   audio_cnt,audio_basic;
unsigned short   audio_period;
unsigned char    err_codea,err_codeb;/*´íÎó×´Ì¬   lwz   err_codebÃ»ÓÐÈÎºÎÓÃ´¦£¬¿ÉÒÔÖ±½ÓºöÂÔ£¬ÖµÒ»Ö±Îª0 */
unsigned char    buz_flg,buz_flg1,buz_cnt;// lwz buz_flg´ú±íÊÇ·ñÐèÒª·äÃùÆ÷Ïì£»buz_flg1Îª0´ú±í·äÃùÆ÷ÕýÔÚÏì£¬1´ú±í²»ÏìÁË£»buz_cnt´ú±í¹Ø±ÕÏìÉùµÄµ¹¼ÆÊ±£»
unsigned short   bat_close_tim=0;
unsigned char    mute_flg=0;// lwz ¾²Òô±êÖ¾£¬0Îª²»¾²Òô£¬1Îª¾²Òô
unsigned short   mute_tim=0;

unsigned char  SPEAK_flg=0;
unsigned char  close_flg=0;
unsigned char   	bat_lev_bak=0;// lwz ¶Ôz1µÄ±¸·Ý
unsigned char   	z1=0;// lwz z1µÄ¸÷¸öÎ»´ú±íµÄº¬Òå£º0Î»-¡·ÊÇ·ñµÍÓÚ3.5V£»1Î»-¡·ÊÇ·ñ·¢ÉúÁËÈÎºÎ´íÎó£»3Î»-¡·ÊÇ·ñµÍÓÚ3.7V£»

unsigned char   	open_bum=0;
unsigned char   	bum_dly=0;
unsigned char   	bum_dly_flg=0;// lwz ±Ã²»ÐèÒª·ÅÆø£¬ÔòÎª0£¬ÐèÒª·ÅÆøÔòÎª1
unsigned short      dataREAD0,dataREAD1,dataREAD2,dataREAD3;

unsigned char SPK_STATE = 0;
volatile unsigned short TK_TIME = 0;//Ì«¿ÕÊ±¼ä
unsigned short    key_silent_flag = 0;

/*20ÃëÏìÒ»Éù*/
typedef enum _on{
	BEEGO,BEEONE,BEETWO,BEETHREE,BEEEND
}BEEPTWO;
BEEPTWO BEE_TWO=BEEGO;
#define spk_set()  SPEAK=1;SPK_STATE=1;// lwz ·äÃùÆ÷Ïì
#define spk_clr()  SPEAK=0;SPK_STATE=0;// lwz ·äÃùÆ÷¹Ø±Õ
unsigned short spk_selay = 0;// lwz ·äÃùÆ÷ÑÓÊ±
void spk_bee_server()
{
	if (0==SPK_STATE)// lwz È·±£¹Ø±Õ·äÃùÆ÷£¬²¢ÖØÖÃspk_selay¼ÆÊý
	{
			spk_selay = 0;
			spk_clr();
			return;
	}
	
	if (spk_selay++ > MAX_BEE_TIME)// lwz µ±SPK_STATE=1Ê±£¨¼´·äÃùÆ÷ÔÚÏì£©£¬ÇÒ¼ÆÊ±Æ÷³¬Ê±£¬Ôò¹Ø±Õ·äÃùÆ÷
	{
		spk_selay = 15;
		audio_period=0;
		BEE_TWO=BEETWO;
		spk_clr();
	}
}
void  SYS_DatIni(void)/*used*/
{
	mod_main_a =  MOD_SYS;
	mod_seta_cnt = UI_WORKMODE_SELECT;
	back_led_cnt=0;
	key_val=0;
	key_val_bak=0;
	lock_flg=0;
	err_codea=err_codeb=0;
	mod_tim_cnta=0;;
	buz_flg1=0;
	///////////////////////////////////////
	audio_period=AUDIO_PERIOD+400;// lwz ÈÃ·äÃùÆ÷¿ª»ú¾ÍÏì
	audio_cnt=0;
	audio_basic=0;
	audio_flg=LED_BAT_NORMAL;
	key_start_tim=0;
	bat_close_tim=0;
	mod_tim_cnta=0;
	mod_tim_cnta2=0;
	mod_jixa=0;
}

void Warn(void)/*used*/
{
	// lwz ¾¯±¨ÓÅÏÈ¼¶£º¹ÜÂ·¶ÂÈû < ÒºÎ»µ½ < Â©Æø < ´«¸ÐÆ÷´¥·¢ < Î´½ÓÒººÐ
	if (flager_a&ERRA_V)// lwz Èç¹ûÎ´½ÓÒººÐ
	{
		if (mod_main_a==MOD_WAT)// lwz Èç¹ûµ±Ç°´¦ÓÚµÈ´ýÖ¸ÁîÄ£Ê½£¬ÔòÈ·ÈÏ´íÎóÎª£ºÎ´½ÓºÐ
		{
			err_codea=ERR_CANISTER_NOT_CON;    /////////Î´½ÓºÐ
		}
		else// lwz ·ñÔòÈ·ÈÏ´íÎóÎª£º¶Â¹Ü
		{
			err_codea=ERR_PIPE_BLOCKED;    //////////¶Â¹Ü
		}
	}
	else if (flager_a&ERRA_S)// lwz Èç¹û½ÓÁËºÐ×Ó£¬²¢ÇÒ´«¸ÐÆ÷´¥·¢
	{
		if (mod_main_a==MOD_WAT)// lwz Èç¹û½ÓÁËºÐ×Ó£¬´«¸ÐÆ÷´¥·¢£¬²¢ÇÒµ±Ç°´¦ÓÚµÈ´ýÖ¸ÁîµÄ½×¶Î£¬ÔòÈÏÎª´«¸ÐÆ÷¹ÊÕÏ
		{
			err_codea=ERR_SENSOR_MALFUCTION;    ///////////´«¸ÐÆ÷¹ÊÕÏ
		}
		else
		{
			err_codea=ERR_CANISTER_FULL;    ///////////ÒºÎ»Âú¹ÊÕÏ
		}
		//err_codea=4;
	}
	else if (flager_a&ERRA_LQ)// lwz Èç¹û½ÓÁËºÐ×Ó£¬´«¸ÐÆ÷Ã»´¥·¢£¬²¢ÇÒÂ©Æø
	{
		err_codea=ERR_AIR_LEAKAGE;    //////////Â©Æø
	}
	else if (flager_a&ERRA_YW)// ÒºÎ»µ½
	{
		err_codea=ERR_CANISTER_REACHED;    //////////ÒºÎ»µ½
	}
	else if (flager_a&ERRB_DS)// lwz ¹ÜÂ·¶ÂÈû
	{
		err_codea=ERR_JAMED;    //////////IEC60601 ¶ÂÈû±¨¾¯
	}
	else if (flager_a&ERRB_TK)// lwz ¿ÕÏÐ5·ÖÖÓ
	{
		err_codea=ERR_DEV_IDLE;    //////////IEC60601 ¶ÂÈû±¨¾¯
	}
	
	else
	{
		err_codea=0;    ///////////ÎÞ¹ÊÕÏ
	}
	if (err_codea > 0)/*Èç¹ûÓÐ¹ÊÕÏ ÔòµÆÁÁ*/
	{
		back_led_cnt = 0;
	}
	spk_bee_server();// lwz ÉèÖÃBEE_TWO = BEETWO
}
unsigned short bee_delay = 0;
// lwz ´Ë´¦Îª¿ØÖÆÉùÒô²¥·ÅµÄÖ÷º¯Êý
void bee_three()
{
	//if (buz_flg)return;
	switch (BEE_TWO)
	{
	default:
		spk_clr();
		BEE_TWO = BEEGO;
		break;
	case BEEGO:
	{
		audio_period = 0;
		audio_basic = 0;
		spk_clr();
		BEE_TWO=BEEONE;
		break;
	}
	case BEEONE:/*½ÐÒ»Éù */
	{
		spk_set();
		if (audio_basic++ > AUDIO_TIM)
		{
			BEE_TWO=BEETWO;
			audio_period = 0;
		}

		break;
	}
	case BEETWO:/*Í£20Ãë */
	{
		spk_clr();
		if (audio_period++ > AUDIO_PERIOD)
		{
			
			BEE_TWO=BEETHREE;
		}
		break;
	}
	case BEETHREE:/* ÉèÖÃ²ÎÊý*/
	{
		audio_period = 0;
		audio_basic = 0;
		spk_clr();
		BEE_TWO=BEEONE;
		break;
	}

	}
}

#if 0
// lwz ÉùÒô²¥·Åº¯Êý£¬²ÎÊýcntÎªÉùÒô²¥·ÅµÄ´ÎÊý
void AUDIO_Sub(unsigned char cnt)/*used*/
{
	unsigned char  i;
	i=cnt;
	// lwz (SPEAK_flg==0)µÄÌõ¼þÊÇ°´¼üÃ»ÓÐ±»³¤Ê±¼ä°´ÏÂ
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
	// lwz (SPEAK_flg==0)µÄÌõ¼þÊÇ°´¼üÃ»ÓÐ±»³¤Ê±¼ä°´ÏÂ¡£·´ÎÊ£ºÕâÀï¸úKEY_MUTL²»Ã¬¶ÜÂð£¿ÕâÀïÓ¦¸ÃÊÇÔÚ³¤°´¾²Òô¼üÊ±£¬ÉèÖÃSPEAK_flgÎª1
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

		if (mute_flg==0)// lwz Èç¹ûµ±Ç°²»ÊÇ¾²Òô×´Ì¬£¬ÔòÇå³ý·äÃùÆ÷±êÖ¾
		{
			DISP_BuzClr(6,25);
		}
	}

	if (mute_flg)// lwz ¾²ÒôÄ£Ê½
	{
		if (close_flg)// lwz ³¤°´ÁË¾²Òô°´Å¥£¬½øÈëÁË¾²ÒôÄ£Ê½£¬Ôò²»ÔÙµ¹¼ÆÊ±È¡Ïû¾²Òô±êÖ¾
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
//		if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
//		{
//			mute_flg=0;
//			mute_tim=0;
//			DISP_BuzClr(6,25);
//		}
	}
	else
	{
		bee_three();// lwz Èç¹ûÃ»ÓÐ¾²Òô±êÖ¾£¬Ôò²¥·ÅÉùÒô
	}
	// else
	// {
	// if (audio_period++>=AUDIO_PERIOD)
	// {
	// audio_basic++;
	// if (audio_basic<AUDIO_TIM)/////////////////Õ¼¿Õ±È
	// {
	// SPEAK  =  1;
	// }
	// else if ( audio_basic<20 )/////////////////Ð¡ÖÜÆÚ
	// {
	// SPEAK  =  0;
	// SPEAK=0;
	// }
	// else
	// {
	// SPEAK  =  0;
	// audio_basic=0;
	// if (++audio_cnt>=i)
	// {
	// audio_period=0;
	// audio_cnt=0;
	// }
	// }
	// }
	// }
}
#endif

void AUDIO_Sub(unsigned char cnt)/*used*/
{
	unsigned char  i;
	i=cnt;
	// lwz (SPEAK_flg==0)µÄÌõ¼þÊÇ°´¼üÃ»ÓÐ±»³¤Ê±¼ä°´ÏÂ
	if ((key_val==KEY_MUT)&&(SPEAK_flg==0))
	{
		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		//close_flg=0;

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

		if (mute_flg)// lwz ¾²ÒôÄ£Ê½
		{
			if (close_flg)// lwz ³¤°´ÁË¾²Òô°´Å¥£¬½øÈëÁË¾²ÒôÄ£Ê½£¬Ôò²»ÔÙµ¹¼ÆÊ±È¡Ïû¾²Òô±êÖ¾
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
			bee_three();// lwz Èç¹ûÃ»ÓÐ¾²Òô±êÖ¾£¬Ôò²¥·ÅÉùÒô
		}
	}
	
	// lwz (SPEAK_flg==0)µÄÌõ¼þÊÇ°´¼üÃ»ÓÐ±»³¤Ê±¼ä°´ÏÂ¡£·´ÎÊ£ºÕâÀï¸úKEY_MUTL²»Ã¬¶ÜÂð£¿ÕâÀïÓ¦¸ÃÊÇÔÚ³¤°´¾²Òô¼üÊ±£¬ÉèÖÃSPEAK_flgÎª1
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
		if (mute_flg==0)// lwz Èç¹ûµ±Ç°²»ÊÇ¾²Òô×´Ì¬£¬ÔòÇå³ý·äÃùÆ÷±êÖ¾
		{
			DISP_BuzClr(6,25);
		}
	}

	if (mute_flg)// lwz ¾²ÒôÄ£Ê½
	{
		if (close_flg)// lwz ³¤°´ÁË¾²Òô°´Å¥£¬½øÈëÁË¾²ÒôÄ£Ê½£¬Ôò²»ÔÙµ¹¼ÆÊ±È¡Ïû¾²Òô±êÖ¾
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
		bee_three();// lwz Èç¹ûÃ»ÓÐ¾²Òô±êÖ¾£¬Ôò²¥·ÅÉùÒô
	}
}

void BUZ_Cls(void);

// lwz µç³ØµçÁ¿¹ýµÍÊ±£¬Èí¹Ø»ú£»²¥·ÅÉùÒô£»
void AUDIO(void)/*used*/
{

	if ((bat_lev&0x0f)==LOWER_THAN_3_5V)
	{
		z1=z1|1;//lwz ÉèÖÃz1µÄµÚ0Î»
	}
	else
	{
		z1=z1&(~1);//lwz È¡Ïûz1µÄµÚ0Î»
	}


	if (err_codea)// Èç¹û·¢ÉúÁËÈÎºÎ´íÎó
	{
		z1=z1|2;//lwz ÉèÖÃz1µÄµÚ1Î»
	}
	else
	{
		z1=z1&(~2);//lwz È¡Ïûz1µÄµÚ1Î»
	}
	if ((bat_lev&0x0f)==1)// lwz µÍÓÚ3.7v
	{
		z1=z1|8;//lwz ÉèÖÃz1µÄµÚ3Î»
	}
	else
	{
		z1=z1&(~8);//lwz È¡Ïûz1µÄµÚ3Î»
	}

	if (z1>bat_lev_bak)// Èç¹ûz1ÐÂÉèÖÃÁËÄ³¸ö±êÖ¾Î»£¨¼´·¢ÉúÁËÄ³¸öÊÂ¼þ£©£¬ÔòÈ¡Ïû¾²Òô±êÊ¶
	{
		mute_flg=0;
	}

	/******************************************************/
	if ((bat_lev&0x0f)==LOWER_THAN_3_5V)////////////////////////////////////
	{////////////////////////////////////////////////////////Ç·Ñ¹±¨¾¯
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
			POWER_ON=0;// lwz Èç¹ûµçÑ¹Ð¡ÓÚ3.5V£¬ÇÒÔËÐÐ³¬¹ý500¸öÑ­»·£¬Ôò½øÐÐÈí¹Ø»ú
		}
	}

	else if (err_codea||err_codeb)
	{
		// lwz ³ýÁËÂ©ÆøµÄÍ¬Ê±³äµçÏß²åÉÏ»òÕßÂúµçÕâÖÖÇé¿ö²»±¨¾¯£¬ÆäËû´íÎóÂëÇé¿ö¶¼±¨¾¯¡£¼´´ó²¿·ÖÇé¿ö¶¼Òª±¨¾¯¡£
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
		if ((bat_lev&0x0f)==LOWER_BAT_WARN_3_6V)///////////////////////////////////µÍµçÁ¿±¨¾¯
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
	bat_lev_bak  =  z1;
	/******************************************************/
}

void BUZ_KeyCls(void)/*used*/
{
	buz_flg  =  0;
	buz_flg1 =  0;
	buz_cnt  =  0;
	spk_clr();
	TMR2     =  0;
	spk_clr();
}
void BUZ_Cls(void)/*used*/
{
	buz_flg  =  0;
	buz_flg1 =  1;
	buz_cnt  =  0;
	TMR2     =  0;
	spk_clr();
}

void AUDIO_Key(void)/*used*/
{
	if (key_val && key_silent_flag)// lwz Èç¹û°´¼üÓÐÖµ£¬Ôò¿ÉÒÔ²¥·ÅÉùÒô£» ¼ÓÉÏÁËkey_silent_flag±êÖ¾À´¾²Ä¬°´¼ü
	{
		buz_flg=1;
	}

	if (buz_flg)// lwz ´ú±íÐèÒª·äÃùÆ÷Ïì
	{
		if (buz_flg1==0)// lwz ÅÐ¶ÏÊÇ·ñÕýÔÚ²¥·ÅÉùÒô£¬Îª0´ú±íÕýÔÚ²¥·Å£¬1´ú±í²¥·ÅÍê±Ï
		{
			if (buz_cnt++>=BUZZER_TIME_CYCLE)
			{
				spk_clr();
				BUZ_Cls();// lwz ÉèÖÃÎªÉùÒôÒÑ¾­²¥·Å¹ý
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
		TK_TIME++;//5*60*1000/20=15000
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

void main(void)
{
	SYS_OSC_Ini();
  asm("clrwdt"); //Çå¿´ÃÅ¹·
  	// lwz ´ÓflashÖÐ¶ÁÈ¡Êý¾Ý£¬Ã¿´Î¶ÁÈ¡2¸ö×Ö½Ú£¬¼´Ò»¸öshortÀàÐÍ
	dataREAD1 = Flash_Read(addr);
	dataREAD2 = Flash_Read(addr+2);
	dataREAD3 = Flash_Read(addr+4);
	dataREAD0 = Flash_Read(addr+6);

	dataK1    = Flash_Read(addr+8);
	dataK2    = Flash_Read(addr+10);
	dataK3    = Flash_Read(addr+12);
	dataK4    = Flash_Read(addr+14);
	key_silent_flag  = Flash_Read(addr+16);
	language = (key_silent_flag & 0xFF);// lwz µÍ×Ö½ÚÎªÓïÑÔ±êÖ¾
#ifdef LOGO_TYPE_VR_CHINA
	if(language == 0xFF)// lwz Èç¹ûÊÇ¹úÄÚ°æ£¬ÉÕÂ¼ºóµÄ¿ª»úÄ¬ÈÏ¾ÍÊÇÓ¢ÎÄ
		language = 0xFE;
#endif
	if(language != 0xFE)// lwz Ä¬ÈÏÖ»ÓÐÓ¢ÎÄ¡¢ÖÐÎÄÁ½ÖÖÓïÑÔ£¬Èç¹û¶Áµ½µÄ²»ÊÇÖÐÎÄ£¬ÔòÖ±½ÓÉèÖÃÓ¢ÎÄ
		language = 0;
	key_silent_flag = (key_silent_flag & SILENT_FLAG_BITMASK);// lwz ¾²Ä¬±êÖ¾ÔÚµØÖ·Îªaddr+16µÄshortÀàÐÍµÄ¸ß×Ö½ÚµÄ×î¸ßÎ»
	// language=0;//Ä¬ÈÏÓ¢Óï
	mod_seta_prel  = Flash_Read(addr+18);// lwz ¶ÁÈ¡¼äÏ¶Ä£Ê½ÏÂµÄµÍÑ¹Öµ
	// lwz ³õÊ¼»¯È«¾ÖGPIO×´Ì¬
	SYS_IO_Ini();
	// lwz ³õÊ¼»¯¶¨Ê±Æ÷0
	SYS_TMR0_Ini();
	// lwz ³õÊ¼»¯¶¨Ê±Æ÷3
	SYS_TMR3_Ini();
	// lwz ³õÊ¼»¯ÏµÍ³ÖÐÒªÓÃµÄ¸÷¸öÈ«¾Ö±äÁ¿
	SYS_DatIni();
	// lwz adcÄ£Êý×ª»»¿ØÖÆ³õÊ¼»¯
	adc_init();
	// lwz ¼ÓÔØ¹¤×÷Ä£Ê½¡¢Ñ¹Á¦ÉÏÏÂÏÞµÈ²ÎÊý
	UART_Test();
	flager_a=0;
	mod_seta_prehh=0;
	bump_need_out_air_flg=0;
	BAT_WarnFir();// lwz ½«adc²É¼¯ËùµÃµÄÊý¾Ý×ª»»Îªµ±Ç°µÄµçÑ¹
	T3ON=1;// lwz Æô¶¯¶¨Ê±Æ÷3
	BEE_TWO=BEEGO;
	GIE =1;
	PEIE =1;
  asm("clrwdt"); //Çå¿´ÃÅ¹·
	clear_lqtimes();
	valueK  =  2.75f;
	while (1)
	{
	//	LATC|=0x10;
//		LATCbits.LATC4=1;
	  asm("clrwdt"); //Çå¿´ÃÅ¹·
		if (FLG_SYS_10MS)//Êµ¼Ê¸ÄÎªÁË20ms TMR0¶¨Ê±Æ÷ÖÐÉè¶¨ÁË
		{
			FLG_SYS_10MS = 0;
		//	ClrWdt();
			if (mod_main_a!=MOD_SYS)
			{
				KEY_Scan();// lwz °´¼üÉ¨Ãè
				AUDIO_Key();// lwz ¿ØÖÆÓë°´¼ü¶ÔÓ¦µÄÉùÒô
			}
#ifdef PUMP_IDLE_FLAG
			FIND_FREE();// lwz ¼ì²éµ±Ç°ÊÇ·ñ´¦ÓÚ¿ÕµÄ×´Ì¬£¬Çå³ýºÍÉèÖÃ¶ÔÓ¦µÄ×´Ì¬Î»
#endif
			MODE_ProA();// lwz ¸ù¾Ýµ±Ç°Ä£Ê½£¬À´¾ö¶¨´¦Àí·½Ê½
			MODE_Pro();// lwz ¿ØÖÆ°´¼ü¹Ø»ú¡¢ËøÆÁ¡¢ÓïÑÔÇÐ»»
			
			DISP_MainA();// lwz ¸ù¾Ýµ±Ç°Ä£Ê½£¬¿ØÖÆÃæ°åµÄÏÔÊ¾
			// lwz Èç¹ûµ±Ç°Ã»ÓÐÔÚÏµÍ³³õÊ¼»¯×´Ì¬ºÍ¿Õ×´Ì¬£¬ÔòÖ´ÐÐ±¨¾¯º¯Êý
			if ((mod_main_a!=MOD_SYS)||(mod_main_a==MOD_TK))
			{
				Warn();
				BAT_Warn();
				AUDIO();
			}

			if ((mod_main_a!=MOD_SYS)&&(mod_main_a!=MOD_OFF))/*Í³Í³¹Ø»úÊ±ºó²»ÏÔÊ¾µç³Ø*/
			{
				if (mod_main_a!=MOD_WAT)// lwz ³ýÁË³õÊ¼»¯¡¢µÈ´ýÖ¸Áî¡¢¹Ø»úÈýÖÖ×´Ì¬£¬ÆäËû×´Ì¬¶¼ÏÔÊ¾µç³ØÐÅÏ¢
				{
					DISP_Bat();
				}
			}

			/*********************¶Ô´óÆø·ÅÆø****************************/
			if (bump_need_out_air_flg&0xf0)// lwz Èç¹ûÒª´ò¿ªpwm£¬ÔòÉèÖÃVAL1£¬·ÅÆø100¸öÑ­»·£¬ÒÔ±ãÓÚ±ÃÆô¶¯
			{
				// open_bum=1;
				if (bum_dly_flg==0)
				{
					// if (bum_dly++>=20)
					if (bum_dly++>=50)
					{
						open_bum=1;// lwz ×¼±¸Æô¶¯±Ã
						// if (bum_dly>=30)//20160518
						//if (bum_dly>=100)
						if (bum_dly>=100)// lwz ·ÅÆø100¸öÑ­»·ºó£¬¹Ø·§1
						{
							bum_dly_flg=1;
							VAL1=0;// lwz ¹Ø±Õ·§1
							bum_dly=0;
							show_lq_times.lq_times++;// lwz ·ÅÆø´ÎÊý¼ÓÒ»
						}
					}
					else
					{
						//if(adc_ps00>79)//20160401:Ñ¹Á¦´óÓÚ80Æô¶¯ÎÌÎÌ½Ð
						//if(adc_ps00>200)//20160509:Ñ¹Á¦³¬ÁË¾Í´ò¿ª,¼õÉÙ·ÅÆøÔëÉù
							VAL1=1;// lwz ´ò¿ª·§1
					}
				}
			}
			else
			{
				open_bum=0;
				// VAL1=0;
				// VAL2=0;
				bum_dly_flg=0;
				VAL1=0;
				bum_dly=0;
			}

		}
	}
}




















