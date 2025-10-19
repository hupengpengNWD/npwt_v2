#include   "include.h"
#include  "BIOS_JLX1864G_139.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include   "npwt_dis_main.h"
#include   "npwt_dis_ofile_lcd_02.h"
#include   "npwt_con_over.h"
#include   "Flash.h"
unsigned short  key_cnt;
unsigned char   key_key=0;
unsigned char   set_a00=0;
unsigned char key_port,key_port_bak,key_val,key_val_bak,key_flg,key_flg_l;
unsigned char mod_main_a,mod_main_b,mod_main_baka,mod_jixa;

unsigned char remember_mod;
unsigned char mod_seta_cnt,mod_seta_wok;
unsigned short mod_seta_preh,mod_seta_preh_bak,mod_seta_prel,mod_seta_ont,mod_seta_oft;
unsigned short mod_tim_cnta,mod_tim_cnta2;

unsigned short back_led_cnt,lock_cnt;
unsigned char  lock_flg;

unsigned short  set_moda=0;
float   valueK  =  0;
unsigned short  dataK1,dataK2,dataK3,dataK4,dataK;

void KEY_Scan(void)
{
	key_port = GET_KEY_VAL;
	if (key_port == key_port_bak)
	{
		if (key_cnt++>=5)
		{
			if (key_port == NO_KEY_PRESSED)
			{
				
				key_flg_l=0;
				key_key=0;
				if (err_codea||err_codeb)
				{
					
				}
				else if (bat_lev==LOWER_BAT_WARN_3_6V)
				{
					
				}
				else if (back_led_cnt++>BACK_LED_OFF_TIMEOUT)
				{
					ClrBlk();    
				}

				if (lock_cnt++>LOCK_FLAG_TIMEOUT)
				{
					lock_flg=1;  
				}
				
				if (key_flg==0)
				{
					if (key_val_bak)
					{
						key_val = key_val_bak;
						buz_flg1=0;             
						
						key_val_bak = 0;        
					}
					else
					{
						key_val = key_val_bak;
						SPEAK_flg=0;
					}
				}
				else
				{
					key_flg=0;

					key_val = 0;
					key_val_bak=0;
				}
			}
			else
			{
				lock_cnt=0;
				back_led_cnt=0;
				SetBlk();
				key_val_bak  =  key_port;
				if (key_val==UNLOCK_NPWT_KEYVAL)
				{
					if (key_cnt>=5)
					{
						key_cnt = 5;
						key_val = key_val_bak+0x80;
						if (key_flg==1)
						{
							return;    
						}
						key_flg=1;
						buz_flg1=0;
					}

				}
				else
				{
					if (key_cnt>=JUDGE_LONG_PRESS_TIME)
					{
						key_cnt = JUDGE_LONG_PRESS_TIME;
						key_val = key_val_bak+0x80;
						if (key_flg==1)
						{
							return;    
						}
						key_flg=1;
						buz_flg1=0;
					}
				}
			}
		}
	}
	else
	{
		key_cnt=0;    
		key_val=0;
	}
	if(key_val>0)
	{
		flg2015=0;
		if(mod_main_a==MOD_TK)
		{
			DISP_ClrZero(0,6,0,125);
			all_flg=0;
			flg2015=0;
			flg_disp=0;
			mod_main_a=remember_mod;
			remember_mod=0;
			key_val=0;
			audio_flg=LED_BAT_NORMAL;
			SetBlk();
		}
		TK_TIME=0;
	}		
	key_port_bak  =  key_port;
}

static unsigned short calc_delta_i(unsigned short counts1,unsigned short counts2 )
{
	unsigned short rst = 0;
	if ( counts1 >= counts2 )
	{
		rst = (counts1 - counts2);
	}
	if (rst<10)
	{
		rst = 10;
	}
	return rst;
}
unsigned char   key_set_tim,key_start_tim;

unsigned char   tim5_flg=0;

unsigned char   ddfq=0;
unsigned short   ddfq_delay=0;

void get_xx_delta(unsigned short press)
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
#else
	delta = press/10;
#endif
	con_lo_delta = delta;
	con_hi_delta = delta;
	
}

static unsigned short key_mute_press_cnt = 0;
void  MODE_ProA(void)
{
	switch (mod_main_a)
	{
	case MOD_SYS:
	{
		err_codea=0;
		mod_tim_cnta2=0;
		mod_jixa=0;
		TK_TIME=0;

		if ((GET_KEY_VAL)==KEY_MUT && key_mute_press_cnt++ > 100 && key_mute_press_cnt < SILENT_FLAG_SET_TIMEOUT)
		{
			key_mute_press_cnt = SILENT_FLAG_SET_TIMEOUT;
			if(key_silent_flag)
			{
				key_silent_flag = 0;
			}
			else
			{
				key_silent_flag = SILENT_FLAG_BITMASK;
			}
		}
		if (mod_tim_cnta++>=(7000/MAIN_CLK))
		{
			mod_main_a = MOD_WAT;
			mod_tim_cnta = 0;
			key_val=key_val_bak=0;
			BUZ_KeyCls();
			lock_cnt=0;
			lock_flg=0;
			key_flg_l=1;
			DISP_Clear();
			adc_zero=adc_ps0;
			scan_dusai_time = 0;
			flager_a &=~ERRB_DS;
		}
		else if (mod_tim_cnta>=(5000/MAIN_CLK))
		{
			DISP_Ver();

		}
		else if (mod_tim_cnta>50)
		{
			if (mod_tim_cnta<52)
			{
				if (key_start_tim<30)
				{
					mod_tim_cnta=0;
					POWER_ON=0;
					key_start_tim=0;
					return;
				}
				else
				{
					SYS_IniLcd();
					POWER_ON=1;
					DRV_EN=1;
					VAL2 = 1;
				}
			}
			SetBlk();
			DISP_Sys100();

			if (mod_tim_cnta>62)
			{
				SPEAK  =  0;
			}
			else
			{
				SPEAK  =  1;
			}
		}
		else
		{
			if ((GET_KEY_VAL)==KEY_C)
			{
				key_start_tim++;
			}
			else
			{
				key_start_tim=0;
			}
		}
		break;
	}
	case MOD_WAT:
	{
		
		record_ds_turn=0;
		twenty_seconds=0;
		
		VAL2 = 0;
		mod_jixa = 0;
		scan_dusai_time = 0;
		flager_a &=~ERRB_DS;
		
		debug_air=0;
		if (err_codea!=0)
		{
			break;
		}
		if (lock_flg==1)
		{
			break;
		}
		if (key_val == KEY_C )
		{
			mod_main_a = mod_main_baka;
		}
		else if (key_val == KEY_UP)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;
		}
		break;
	}
	case MOD_LIX:
	{

		mod_seta_preh = mod_seta_preh_bak;
		mod_tim_cnta2=0;
		mod_jixa=0;
		mod_main_baka = MOD_LIX;

		if (lock_flg==1)
		{
			break;
		}

		if (key_val == KEY_OK )
		{
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;
			if (language)
			{

			}
			else
			{
				;
			}
		}
		break;
	}
	case MOD_JIX:
	{
		if (mod_jixa==0)
		{
			mod_seta_preh = mod_seta_preh_bak;
			if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)
			{
				mod_tim_cnta=0;
				mod_tim_cnta2++;
				if (mod_tim_cnta2>=mod_seta_ont*60)
				{
					record_ds_turn=0;
					twenty_seconds=0;

		record_ds_turn=0;
		flager_a &=~ERRB_DS;

					bbbbb=0;
					ddfq=1;
					ddfq_delay=0;
					mod_tim_cnta2=0;
					mod_jixa=1;
				}
			}
		}
		else if (mod_jixa==1)
		{
			mod_seta_preh=mod_seta_prel;
			
			if (adc_ps0>adc_zero)
				pwm_k1    =    (adc_ps0-adc_zero)/valueK;
			else
				pwm_k1=0;
			
			adc_ps00  =    (unsigned int)pwm_k1;
			
			if(ddfq==1)
			{
				if(ddfq_delay++ >= 12)
				{
					VAL2=0;
					if(ddfq_delay >=75)
					{
						get_xx_delta(mod_seta_preh);
						if(adc_ps00 <= (mod_seta_preh+con_hi_delta))
						{
							ddfq=2;
							mod_tim_cnta=0;
						}
						ddfq_delay=0;
					}
				}
				else
					VAL2=1;	

		record_ds_turn=0;
		flager_a &=~ERRB_DS;
			}
			else
			{
				if(ddfq_delay++ >= 499)
				{
					ddfq=0;
					if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)
					{
						mod_tim_cnta=0;
						mod_tim_cnta2++;
						if (mod_tim_cnta2>=mod_seta_oft*60)
						{
							record_ds_turn=0;
							twenty_seconds=0;
							mod_tim_cnta2=0;
							mod_jixa=0;
							mod_seta_preh = mod_seta_preh_bak;
						}
					}
					ddfq_delay=499;
				}
			}
		}
		mod_main_baka = MOD_JIX;

		if (lock_flg==1)
		{
			break;
		}

		if (key_val == KEY_OK )
		{
			
			mod_jixa=0;
			mod_tim_cnta=0;
			mod_tim_cnta2=0;
			
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;

			if (language)
			{

			}
			else
			{
				;
			}
		}
		break;
	}
	case MOD_ZXB:
	{
		if (mod_jixa==0)
		{
			mod_seta_preh = mod_seta_preh_bak;
			if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)
			{
				mod_tim_cnta=0;
				mod_tim_cnta2++;
				if (mod_tim_cnta2>=mod_seta_ont*60)
				{
					mod_tim_cnta2=0;
					mod_jixa=1;
				}
			}
		}
		else if (mod_jixa==1)
		{

			mod_seta_preh=mod_seta_prel;

			if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)
			{
				mod_tim_cnta=0;
				mod_tim_cnta2++;
				if (mod_tim_cnta2>=mod_seta_oft*60)
				{
					mod_tim_cnta2=0;
					mod_jixa=0;
					mod_seta_preh = mod_seta_preh_bak;
				}
			}
		}
		mod_main_baka = MOD_ZXB;

		if (lock_flg==1)
		{
			break;
		}

		if (mod_main_b==MOD_SET)
		{
			break;
		}
		if (key_val == KEY_OK )
		{
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;
		}
		break;
	}
	case MOD_ZHT:
	{
		record_ds_turn=0;
		twenty_seconds=0;
		PUMP = 0;
		debug_air=0;
		mod_jixa = 0;
		if (lock_flg==1)
		{
			break;
		}

		if (key_val == KEY_OK )
		{
			mod_main_a = mod_main_baka;
		}
		else if (key_val == KEY_UPL)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;

			if (language)
			{

			}
			else
			{
				;
			}
		}

		else if (key_val == 0x04)
		{
			tim5_flg =  ~tim5_flg;
		}
		scan_dusai_time = 0;
		flager_a &=~ERRB_DS;
		ddfq=0;
		break;;
	}
	case MOD_SET:
	{
		scan_dusai_time = 0;
		flager_a &=~ERRB_DS;
		debug_air=0;
		if (mod_seta_cnt == UI_WORKMODE_SELECT)
		{
			if (set_a00==0)
			{
				mod_seta_wok = MOD_LIX;
				if (key_val == KEY_UP)
				{
					set_a00=1;
					mod_seta_wok = MOD_JIX;
				}
				else if (key_val == KEY_DN)
				{
					set_a00=1;
					mod_seta_wok = MOD_JIX;
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt = UI_MODE_SET_HI;
					DISP_Clear();
				}
			}
			else if (set_a00==1)
			{
				mod_seta_wok = MOD_JIX;
				if (key_val == KEY_UP)
				{
					set_a00=0;
					mod_seta_wok = MOD_LIX;
				}
				else if (key_val == KEY_DN)
				{
					set_a00=0;
					mod_seta_wok = MOD_LIX;
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt = UI_MODE_SET_HI;
					DISP_Clear();
					if (language)
					{

					}
				}
			}
			else if (set_a00==2)
			{
				mod_seta_wok = MOD_ZXB;
				if (key_val == KEY_UP)
				{
					set_a00=0;
					DISP_Clr1();
					mod_seta_wok = MOD_LIX;
				}
				else if (key_val == KEY_DN)

				{
					set_a00=1;
					DISP_Clr1();
					mod_seta_wok = MOD_JIX;
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt = UI_MODE_SET_HI;
					DISP_Clear();
				}
			}
		}
		else if (mod_seta_cnt==UI_MODE_SET_HI)
		{
			if (mod_seta_wok == MOD_LIX)
			{
				if (key_val == KEY_UP)
				{
					mod_seta_preh=mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)
					{
						mod_seta_preh=CFG_LOW;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_DN)
				{
					mod_seta_preh=mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)
					{
						mod_seta_preh=CFG_HI;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_DNL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)
					{
						mod_seta_preh=CFG_LOW;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_UPL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)
					{
						mod_seta_preh=CFG_HI;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt=UI_WORKMODE_SELECT;
					mod_main_baka=mod_seta_wok;
					mod_main_a=MOD_ZHT;
					DISP_Clear();
					if (language)
					{

					}
					else
					{
						;
					}
				}
			}
			else
			{
				if (key_val == KEY_UP)
				{
					mod_seta_preh = mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)
						mod_seta_preh=CFG_LOW;
				}
				else if (key_val == KEY_DN)
				{
					mod_seta_preh = mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)
					{
						mod_seta_preh=CFG_HI;
					}
					
				}
				else if (key_val == KEY_DNL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)
					{
						mod_seta_preh=CFG_LOW;
					}

				}
				else if (key_val == KEY_UPL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)
					{
						mod_seta_preh=CFG_HI;
					}

				}
				else if (key_val == KEY_OK)
				{

					if (mod_seta_wok == MOD_JIX)
					{
						mod_seta_cnt=UI_JIX_MODE_SET_LO;
					}
					else
					{
						
						mod_seta_cnt=UI_JIX_SET_HI_TIME;
					}DISP_Clear();
					if (language)
					{

					}
					else
					{
						;
					}
				}
			}

			if (mod_seta_preh < mod_seta_prel+MMHG_PER_STEP)
			{
				mod_seta_prel=mod_seta_preh-MMHG_PER_STEP;
			}
			mod_seta_preh_bak=mod_seta_preh;
		}
		else if (mod_seta_cnt==UI_JIX_MODE_SET_LO)
		{

			if (key_val == KEY_UP)
			{
				mod_seta_prel = mod_seta_prel+MMHG_PER_STEP;
				if(mod_seta_prel > JIX_LOW_MODE_HIGHPRESS || mod_seta_prel > mod_seta_preh - 10)
					mod_seta_prel = JIX_LOW_MODE_LOWPRESS;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				
			}
			else if (key_val == KEY_DN)
			{

				mod_seta_prel = mod_seta_prel-MMHG_PER_STEP;
				
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				if(mod_seta_prel < JIX_LOW_MODE_LOWPRESS )
					mod_seta_prel = mod_seta_preh - 10;
				
			}
			else if (key_val == KEY_DNL)
			{

				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				
				key_set_tim=0;
				mod_seta_prel = mod_seta_prel-MMHG_PER_STEP;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;

			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				
				key_set_tim=0;
				mod_seta_prel = mod_seta_prel+MMHG_PER_STEP;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				
			}
			else if (key_val == KEY_OK)
			{
				if (language)
				{

					DISP_Clear();
				}
				else
				{
					DISP_Clear();
				}
				mod_seta_cnt=UI_JIX_SET_HI_TIME;
			}
			if (mod_seta_prel>JIX_LOW_MODE_HIGHPRESS)
			{
				mod_seta_prel = JIX_LOW_MODE_HIGHPRESS;
			}
			if (mod_seta_prel<JIX_LOW_MODE_LOWPRESS)
			{
				mod_seta_prel = JIX_LOW_MODE_LOWPRESS;
			}

		}
		else if (mod_seta_cnt==UI_JIX_SET_HI_TIME)
		{
			if (key_val == KEY_UP)
			{
				
				if (mod_seta_ont++>=MAX_JIX_HIGHMODE_TIME)
				{
					mod_seta_ont=MIN_JIX_HIGHMODE_TIME;
				}
				
			}
			else if (key_val == KEY_DN)
			{
				if (mod_seta_ont--<=MIN_JIX_HIGHMODE_TIME)
				{
					mod_seta_ont=MAX_JIX_HIGHMODE_TIME;
				}
				
			}
			else if (key_val == KEY_DNL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_ont -= LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_ont > MAX_JIX_HIGHMODE_TIME || mod_seta_ont < MIN_JIX_LOWMODE_TIME)
				{
					mod_seta_ont=MIN_JIX_HIGHMODE_TIME;
				}

			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_ont += LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_ont > MAX_JIX_HIGHMODE_TIME || mod_seta_ont < MIN_JIX_LOWMODE_TIME)
				{
					mod_seta_ont=MAX_JIX_HIGHMODE_TIME;
				}
				
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=UI_JIX_SET_LO_TIME;

					DISP_Clear();

			}

		}
		else if (mod_seta_cnt==UI_JIX_SET_LO_TIME)
		{
			if (key_val == KEY_UP)
			{
				if (mod_seta_oft++ >= MAX_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MIN_JIX_LOWMODE_TIME;
				}
				
			}
			else if (key_val == KEY_DN)
			{
				if (mod_seta_oft-- <= MIN_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MAX_JIX_LOWMODE_TIME;
				}
				
			}
			else if (key_val == KEY_DNL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_oft=mod_seta_oft-LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_oft < MIN_JIX_LOWMODE_TIME || mod_seta_oft > MAX_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MIN_JIX_LOWMODE_TIME;
				}
				
			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_oft=mod_seta_oft+LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_oft >= MAX_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MAX_JIX_LOWMODE_TIME;
				}
				
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=0;
				mod_main_baka=mod_seta_wok;
				mod_main_a=MOD_ZHT;
				mod_tim_cnta=0;
				cnt_cnta=0;
				cnt_cntaa=0;
				mod_tim_cnta2=0;
				mod_jixa=0;
				
			}
		}
		else if (mod_seta_cnt==UI_SET_PRESSURE)
		{

			if (key_val == KEY_UP)
			{
				set_moda=set_moda+1;

				DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{
				set_moda=set_moda-1;
				DISP_Clr3();
			}
			else if (key_val == KEY_DNL)
			{
				if (key_set_tim++<20)
				{
					return;
				}
				key_set_tim=0;

				set_moda=set_moda-10;
				DISP_Clr3();
			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<20)
				{
					return;
				}
				key_set_tim=0;
				set_moda=set_moda+10;
				DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=0;
				valueK  =  2.75f;
				mod_main_a=MOD_ZHT;

			}
		}

		break;;
	}
	case MOD_LANGUAGE:
	{

		if (key_val == KEY_OK )
		{				
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UP)
		{
			language++;
			if (language>6)
			{
				language = 6;
			}
		}
		else if (key_val == KEY_DN)
		{
				language--;
			if (language<1)
			{
				language = 0;
			}
		}
		else if(key_val==0x98)
		{
			DISP_LANGUAGE();
		}
		switch (language)
			{
			default:
				break;
				case 0:
				{
			DISP_set_sanjiao(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}
				case 1:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}
				case 2:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}

				case 3:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;			
				}
				case 4:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}
				case 5:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao(2,122);
			DISP_set_sanjiao0(4,122);
						break;			
				}
				case 6:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao(4,122);
						break;				
				}
		}
		break;		
	}
	case MOD_ERR:
	{
		break;
	}
	case MOD_OFF:
	{

		break;
	}
	case MOD_TK:
	{

		break;
	}
	
	default:
	{
		break;
	}
	}
}

unsigned short  close_tim;
unsigned char   mod_bak_bak;
void  MODE_Pro(void)
{
	if ( (mod_main_a != MOD_OFF )&&(mod_main_a != MOD_SYS ))
	{
		close_tim=0;
		if (lock_flg==1)
		{
			;
		}
		else
		{
			if (key_val == KEY_CL )
			{
				if (key_flg_l==1)
					return;
				key_flg_l  =  1;
				flg2015=0;
				DISP_Clear() ;
				mod_main_a = MOD_OFF;
				mod_main_b = MOD_OFF;
			}
		}
	}
	else if ( mod_main_a == MOD_OFF )
	{
		if ((bat_lev&0x0f)==LOWER_THAN_3_5V)
		{
			;
		}
		else
		{
			if (close_tim++>50)
			{
				ClrBlk();
				AUDIO_EN=0;
				Flash_Erase(addr);
				Write_One_Word(addr, mod_seta_preh_bak);
				Write_One_Word(addr+2, mod_seta_ont);
				Write_One_Word(addr+4, mod_seta_oft);
				Write_One_Word(addr+6, mod_main_baka);
				dataK  =  (unsigned int)(valueK*1000.0);
				dataK1 =   dataK/1000;
				dataK2 =   dataK%1000 /100;
				dataK3 =   (  ( dataK%1000 )%100   ) /10;
				dataK4 =   (  ( dataK%1000 )%100   )%10;
				Write_One_Word(addr+8, dataK1);
				Write_One_Word(addr+10,dataK2);
				Write_One_Word(addr+12,dataK3);
				Write_One_Word(addr+14,dataK4);
				key_silent_flag = (key_silent_flag | language);
				Write_One_Word(addr+16,key_silent_flag);
				Write_One_Word(addr+18,mod_seta_prel);
				POWER_ON=0;
			}
		}
	}

	if (key_val==UNLOCK_NPWT_L_KEYVAL)
	{
		lock_flg=0;
		lock_cnt=0;
	}
	if (key_val==UNLOCK_NPWT_KEYVAL)
	{
		lock_flg=0;
		lock_cnt=0;
	}

	if (lock_flg==0) 
	{
		if ( mod_main_a==MOD_WAT)
		{
			if (key_key==0)
			{

				if (key_val==LONG_PRESS_SWITCH_LANG)
				{
					key_key=1;

					if(0 == language)
					{
						language = 0xFE;
					}
					else
					{
						language = 0;
					}
					all_flg = 0;
					
					DISP_Clear();
					
				}

			}
		}

	}
	if (mod_main_a!=mod_bak_bak)
	{
		DISP_Clear();
	}
	mod_bak_bak=mod_main_a;
}

