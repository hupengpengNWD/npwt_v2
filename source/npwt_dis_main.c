
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

#include "../project/config_bits.h"

unsigned char    audio_flg;
unsigned short   audio_cnt,audio_basic;
unsigned short   audio_period;
unsigned char    err_codea,err_codeb;
unsigned char    buz_flg,buz_flg1,buz_cnt;
unsigned short   bat_close_tim=0;
unsigned char    mute_flg=0;
unsigned short   mute_tim=0;

unsigned char  SPEAK_flg=0;
unsigned char  close_flg=0;
unsigned char   	bat_lev_bak=0;
unsigned char   	z1=0;

unsigned char   	open_bum=0;
unsigned char   	bum_dly=0;
unsigned char   	bum_dly_flg=0;
unsigned short      dataREAD0,dataREAD1,dataREAD2,dataREAD3;

unsigned char SPK_STATE = 0;
volatile unsigned short TK_TIME = 0;
unsigned short    key_silent_flag = 0;

typedef enum _on{
	BEEGO,BEEONE,BEETWO,BEETHREE,BEEEND
}BEEPTWO;
BEEPTWO BEE_TWO=BEEGO;
#define spk_set()  SPEAK=1;SPK_STATE=1;
#define spk_clr()  SPEAK=0;SPK_STATE=0;
unsigned short spk_selay = 0;
void spk_bee_server()
{
	if (0==SPK_STATE)
	{
			spk_selay = 0;
			spk_clr();
			return;
	}
	
	if (spk_selay++ > MAX_BEE_TIME)
	{
		spk_selay = 15;
		audio_period=0;
		BEE_TWO=BEETWO;
		spk_clr();
	}
}
void  SYS_DatIni(void)
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
	
	audio_period=AUDIO_PERIOD+400;
	audio_cnt=0;
	audio_basic=0;
	audio_flg=LED_BAT_NORMAL;
	key_start_tim=0;
	bat_close_tim=0;
	mod_tim_cnta=0;
	mod_tim_cnta2=0;
	mod_jixa=0;
}

void Warn(void)
{
	
	if (flager_a&ERRA_V)
	{
		if (mod_main_a==MOD_WAT)
		{
			err_codea=ERR_CANISTER_NOT_CON;    
		}
		else
		{
			err_codea=ERR_PIPE_BLOCKED;    
		}
	}
	else if (flager_a&ERRA_S)
	{
		if (mod_main_a==MOD_WAT)
		{
			err_codea=ERR_SENSOR_MALFUCTION;    
		}
		else
		{
			err_codea=ERR_CANISTER_FULL;    
		}
		
	}
	else if (flager_a&ERRA_LQ)
	{
		err_codea=ERR_AIR_LEAKAGE;    
	}
	else if (flager_a&ERRA_YW)
	{
		err_codea=ERR_CANISTER_REACHED;    
	}
	else if (flager_a&ERRB_DS)
	{
		err_codea=ERR_JAMED;    
	}
	else if (flager_a&ERRB_TK)
	{
		err_codea=ERR_DEV_IDLE;    
	}
	
	else
	{
		err_codea=0;    
	}
	if (err_codea > 0)
	{
		back_led_cnt = 0;
	}
	spk_bee_server();
}
unsigned short bee_delay = 0;

void bee_three()
{
	
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
	case BEEONE:
	{
		spk_set();
		if (audio_basic++ > AUDIO_TIM)
		{
			BEE_TWO=BEETWO;
			audio_period = 0;
		}

		break;
	}
	case BEETWO:
	{
		spk_clr();
		if (audio_period++ > AUDIO_PERIOD)
		{
			
			BEE_TWO=BEETHREE;
		}
		break;
	}
	case BEETHREE:
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
		z1=z1|1;
	}
	else
	{
		z1=z1&(~1);
	}

	if (err_codea)
	{
		z1=z1|2;
	}
	else
	{
		z1=z1&(~2);
	}
	if ((bat_lev&0x0f)==1)
	{
		z1=z1|8;
	}
	else
	{
		z1=z1&(~8);
	}

	if (z1>bat_lev_bak)
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
			POWER_ON=0;
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
	bat_lev_bak  =  z1;
	
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

void main(void)
{
	SYS_OSC_Ini();
  asm("clrwdt"); 
  	
	dataREAD1 = Flash_Read(addr);
	dataREAD2 = Flash_Read(addr+2);
	dataREAD3 = Flash_Read(addr+4);
	dataREAD0 = Flash_Read(addr+6);

	dataK1    = Flash_Read(addr+8);
	dataK2    = Flash_Read(addr+10);
	dataK3    = Flash_Read(addr+12);
	dataK4    = Flash_Read(addr+14);
	key_silent_flag  = Flash_Read(addr+16);
	language = (key_silent_flag & 0xFF);
#ifdef LOGO_TYPE_VR_CHINA
	if(language == 0xFF)
		language = 0xFE;
#endif
	if(language != 0xFE)
		language = 0;
	key_silent_flag = (key_silent_flag & SILENT_FLAG_BITMASK);
	
	mod_seta_prel  = Flash_Read(addr+18);
	
	SYS_IO_Ini();
	
	SYS_TMR0_Ini();
	
	SYS_TMR3_Ini();
	
	SYS_DatIni();
	
	adc_init();
	
	UART_Test();
	flager_a=0;
	mod_seta_prehh=0;
	bump_need_out_air_flg=0;
	BAT_WarnFir();
	T3ON=1;
	BEE_TWO=BEEGO;
	GIE =1;
	PEIE =1;
  asm("clrwdt"); 
	clear_lqtimes();
	valueK  =  2.75f;
	while (1)
	{

	  asm("clrwdt"); 
		if (FLG_SYS_10MS)
		{
			FLG_SYS_10MS = 0;
		
			if (mod_main_a!=MOD_SYS)
			{
				KEY_Scan();
				AUDIO_Key();
			}
#ifdef PUMP_IDLE_FLAG
			FIND_FREE();
#endif
			MODE_ProA();
			MODE_Pro();
			
			DISP_MainA();
			
			if ((mod_main_a!=MOD_SYS)||(mod_main_a==MOD_TK))
			{
				Warn();
				BAT_Warn();
				AUDIO();
			}

			if ((mod_main_a!=MOD_SYS)&&(mod_main_a!=MOD_OFF))
			{
				if (mod_main_a!=MOD_WAT)
				{
					DISP_Bat();
				}
			}

			if (bump_need_out_air_flg&0xf0)
			{
				
				if (bum_dly_flg==0)
				{
					
					if (bum_dly++>=50)
					{
						open_bum=1;

						if (bum_dly>=100)
						{
							bum_dly_flg=1;
							VAL1=0;
							bum_dly=0;
							show_lq_times.lq_times++;
						}
					}
					else
					{

							VAL1=1;
					}
				}
			}
			else
			{
				open_bum=0;

				bum_dly_flg=0;
				VAL1=0;
				bum_dly=0;
			}

		}
	}
}

