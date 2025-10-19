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
unsigned char key_port,key_port_bak,key_val,key_val_bak,key_flg,key_flg_l;// lwz key_flg±íÊ¾ÊÇ·ñ´¦ÓÚ³¤°´×´Ì¬
unsigned char mod_main_a,mod_main_b,mod_main_baka,mod_jixa;// mod_jixaÎª¼äÐªÄ£Ê½ÏÂµÄ¸ßµÍÑ¹×´Ì¬£¬0Îª¸ßÑ¹£¬1ÎªµÍÑ¹£»mod_main_bakaÎªflashÖÐµÄ±¸·Ý¹¤×÷Ä£Ê½£»


unsigned char remember_mod;
unsigned char mod_seta_cnt,mod_seta_wok;// lwz mod_seta_cntÎª1´ú±í¼äÐªÄ£Ê½¸ßÑ¹Á¦ÉèÖÃ»òÕßÁ¬ÐøÄ£Ê½Ñ¹Á¦ÉèÖÃ£¬Îª2´ú±í¼äÐªÄ£Ê½µÍÑ¹Á¦ÉèÖÃ
unsigned short mod_seta_preh,mod_seta_preh_bak,mod_seta_prel,mod_seta_ont,mod_seta_oft;// lwz ¼äÐªÄ£Ê½ÏÂ£ºmod_seta_ont¸ßÑ¹µÄ·ÖÖÓ£»mod_seta_oftµÍÑ¹µÄ·ÖÖÓ
unsigned short mod_tim_cnta,mod_tim_cnta2;

unsigned short back_led_cnt,lock_cnt;
unsigned char  lock_flg;



unsigned short  set_moda=0;
float   valueK  =  0;
unsigned short  dataK1,dataK2,dataK3,dataK4,dataK;
// lwz É¨Ãè°´¼üµÄ×´Ì¬£¬ÅÐ¶Ï°´¼üÊÇ·ñÊôÓÚ³¤°´£¬°´¼ü°´ÏÂ¾Í¿ª±³¹â£¬ÎÞ°´¼üÔò¹Ø±³¹â£¬ÅÐ¶Ï°´¼üËø
void KEY_Scan(void)/*used*/
{
	key_port = GET_KEY_VAL;
	if (key_port == key_port_bak)// lwz Èç¹ûÇ°ºóÁ½´ÎµÄ°´¼ü×´Ì¬Ò»ÖÂ
	{
		if (key_cnt++>=5)// lwz ÀÛ¼Æ¼ÆÊý´óÓÚ5´Î£¬Ôò½øÒ»²½ÅÐ¶Ï
		{
			if (key_port == NO_KEY_PRESSED)////////////////////////////////no key
			{
				//key_flg  =  0;/////////////////////////////°´¼ü±êÊ¶ÇåÁã
				key_flg_l=0;
				key_key=0;
				if (err_codea||err_codeb)// lwz Èç¹û·¢ÉúÈÎºÎ´íÎó£¬²»×ö´¦Àí
				{
					//SetBlk();
				}
				else if (bat_lev==LOWER_BAT_WARN_3_6V)// lwz Èç¹ûµ±Ç°µç³ØµçÑ¹3.6V£¬¼´´¦ÓÚµÍÑ¹×´Ì¬£¬²»×ö´¦Àí
				{
					//SetBlk();
				}
				else if (back_led_cnt++>BACK_LED_OFF_TIMEOUT)
				{
					ClrBlk();    /////////////////////////30Ãë±³¹âÃð
				}

				/////////////×Ô¶¯ÉÏËø////////////////////////////////////////////////////////
				// if (lock_cnt++>1500)
				// {
					// lock_flg=1;  //////////////Ã»ÓÐ°´¼ü30Ãëºó¹Ø±³¹âËø°´¼ü
				// }
				
				
				//20160719 EDIT BY MA
				if (lock_cnt++>LOCK_FLAG_TIMEOUT)
				{
					lock_flg=1;  //////////////Ã»ÓÐ°´¼ü30Ãëºó¹Ø±³¹âËø°´¼ü
				}
				////////////////////////////////////////////////////////////////////////////
				if (key_flg==0)
				{///////////////////////////////////////////ÎÞ³¤°´°´¼ü 
					if (key_val_bak)
					{
						key_val = key_val_bak;
						buz_flg1=0;             ///////////////¶Ì°´¼üBUZ¿ªÆô
						///////////////¼üÖµ»ñÈ¡
						key_val_bak = 0;        ///////////////Çå³ý±¸·ÝÖµ
					}
					else
					{
						key_val = key_val_bak;
						SPEAK_flg=0;
					}
				}
				else
				{// lwz Èç¹ûÊÇ¸Õ¸ÕÇÐÈëÎÞ³¤°´°´¼ü×´Ì¬
					key_flg=0;
					//if(key_val&0x80)
					//{BUZ_KeyCls();}/////////////Ö´ÐÐ¹ý³£°´¼ü¹Ø±ÕBUZ
					key_val = 0;////////////////////////Î´Ö´ÐÐ¹ý¼ÌÐøBUZ
					key_val_bak=0;//////////////////////Çå³ý¼üÖµ
				}
			}
			else// lwz ÈÎºÎ°´¼ü±»°´ÏÂ
			{
				lock_cnt=0;// lwz Ëø¼ÆÊýÇåÁã
				back_led_cnt=0;// lwz LED¼ÆÊýÇåÁã
				SetBlk();// lwz ¿ªÆô±³¹â
				key_val_bak  =  key_port;// lwz ±¸·Ýµ±Ç°µÄ¼üÖµ
				if (key_val==UNLOCK_NPWT_KEYVAL)// lwz 0x24Õâ¸ö°´¼üÄ¬ÈÏÎª³¤Ê±¼ä°´ÏÂ£¨ºÜÈÝÒ×´ïµ½£©
				{
					if (key_cnt>=5)
					{
						key_cnt = 5;
						key_val = key_val_bak+0x80;
						if (key_flg==1)
						{
							return;    /////////////////////////////
						}
						key_flg=1;
						buz_flg1=0;///////////////////////////³¤°´¼üBUZ¿ªÆô
					}

				}
				else
				{
					if (key_cnt>=JUDGE_LONG_PRESS_TIME)// lwz ÆäËû°´¼ü£¬Èç¹û¼ÆÊý´óÓÚ100£¬²ÅÈÏÎª³¤Ê±¼ä°´ÏÂ´Ë¼ü
					{
						key_cnt = JUDGE_LONG_PRESS_TIME;
						key_val = key_val_bak+0x80;// lwz ÉèÖÃÎª³¤Ê±¼ä°´ÏÂ
						if (key_flg==1)
						{
							return;    /////////////////////////////
						}
						key_flg=1;
						buz_flg1=0;///////////////////////////³¤°´¼üBUZ¿ªÆô
					}
				}
			}
		}
	}
	else
	{// lwz °´¼üËø¼ÆÊýÇåÁã
		key_cnt=0;    ///////////////////////////////////////////////clear plus
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
/*----------------16Î»ÓÐ·ûºÅÊý¼ÆËã²îÖµ-----------------------------------------*/
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
unsigned char   key_set_tim,key_start_tim;// lwz ¿ªÊ¼Éè±¸µ¹¼ÆÊ±

unsigned char   tim5_flg=0;



unsigned char   ddfq=0;// lwz ¼äÐªÄ£Ê½ÏÂµÄµ±Ç°µÄ·ÅÆø×´Ì¬£º0²»·ÅÆø£»1·ÅÆø£»2ÊÇµÍÑ¹¹¤×÷Ä£Ê½ÏÂ·ÅÆøÒÑ¾­Íê³É£¬²»ÔÙ¼ÌÐø·ÅÆø
unsigned short   ddfq_delay=0;// lwz ·ÅÆøµÄÑÓ³ÙÊ±¼ä


// lwz ¸ù¾Ýµ±Ç°µÄÑ¹Á¦£¬¾ö¶¨µ±Ç°µÄ¸ß²½¾àºÍµÍ²½¾à
void get_xx_delta(unsigned short press)
{
	unsigned short delta = 8;
	//// lwz ¸ü¸Ä£º³ýÁËDeroyal°æÒâÍâ£¬ÆäËûµÄ°æ±¾½«¸¡¶¯ãÐÖµÀ©´ó£¬¾¡Á¿¼õÉÙ±ÃµÄÆô¶¯´ÎÊý
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
void  MODE_ProA(void)/*used*/
{
	switch (mod_main_a)
	{
	case MOD_SYS:////////////////////////////////////////////////system test
	{////////////////////////////////////////////////////////////
		err_codea=0;
		mod_tim_cnta2=0;
		mod_jixa=0;
		TK_TIME=0;
		// lwz Ìí¼Ó´Ë´úÂë£¬Èç¹ûÔÚÏµÍ³³õÊ¼»¯½×¶ÎµãÁË¾²Òô°´Å¥£¬ÔòÇÐ»»¾²Òô±êÖ¾¡£
		// lwz key_mute_press_cntµÄÄ¿µÄÊÇÈ·±£¾²Ä¬±êÖ¾Ö»ÇÐ»»Ò»´Î
		// lwz PORTBµØÖ·µÄÖµ£¬Ó¦¸Ã¾ÍÊÇµ±Ç°±»°´ÏÂµÄ°´¼üÖµ
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
		if (mod_tim_cnta++>=(7000/MAIN_CLK))// lwz ÏÔÊ¾Ö÷½çÃæ
		{
			mod_main_a = MOD_WAT;
			mod_tim_cnta = 0;
			key_val=key_val_bak=0;
			BUZ_KeyCls();
			lock_cnt=0;
			lock_flg=0;
			key_flg_l=1;
			DISP_Clear();
			adc_zero=adc_ps0;// lwz ²É¼¯µ±Ç°´óÆøÑ¹£¬ÒòÎªÕâ¸öÊ±ºò·§2ÔÚ·ÅÆø£¬ËùÒÔÓ¦¸ÃÊÇ´óÆøÑ¹
			scan_dusai_time = 0;
			flager_a &=~ERRB_DS;
		}
		else if (mod_tim_cnta>=(5000/MAIN_CLK))// lwz ÏÔÊ¾°æ±¾ºÅ
		{
			DISP_Ver();
			// DISP_8X16ascii("Intermittent",1,13);
			// mod_tim_cnta--;
		}
		else if (mod_tim_cnta>50)// lwz ¿ª»ú£¬ÏÔÊ¾¿ª»úlogo
		{
			if (mod_tim_cnta<52)
			{
				if (key_start_tim<30)// lwz Èç¹û°´ÏÂ¿ª»ú¼üµÄÊ±¼ä¹ýÓÚ¶Ì£¬È¡Ïû¿ª»ú
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
					VAL2 = 1;// lwz ´ò¿ª·§2½øÐÐ·ÅÆø
				}
			}
			SetBlk();
			DISP_Sys100();
			// DISP_Clear();
			// DISP_BatWarnE() ;
			if (mod_tim_cnta>62)// lwz ¿ª»ú¶ÌÔÝµÄÏìÒ»Éù
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
			if ((GET_KEY_VAL)==KEY_C)// lwz °´ÏÂKEY_C
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
		if (err_codea!=0)//////////////´Ë×´Ì¬ÏÂµÄ¹ÊÕÏÐèÒªÖØÐÂ¿ª»ú
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

		//PUMP = 1;
		mod_seta_preh = mod_seta_preh_bak;
		mod_tim_cnta2=0;
		mod_jixa=0;
		mod_main_baka = MOD_LIX;

		if (lock_flg==1)
		{
			break;
		}

		if (key_val == KEY_OK )// lwz ÔÚÁ¬ÐøÄ£Ê½ÏÂ°´ÏÂOK£¬Ôò½øÈëÔÝÍ£Ä£Ê½
		{
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)// lwz ÔÚÁ¬ÐøÄ£Ê½ÏÂ³¤°´×ó¼ýÍ·£¬Ôò½øÈëÉèÖÃÄ£Ê½
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;
			if (language)
			{
//				DISP_mod_set2015();
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
		if (mod_jixa==0)// lwz ¼äÐªÄ£Ê½ÏÂµÄ¸ßÑ¹×´Ì¬
		{
			mod_seta_preh = mod_seta_preh_bak;
			if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)// lwz Ã¿50¸öÑ­»·½øÈëÒ»´Î¸Ã·ÖÖ§£¬¼´50Ñ­»·µÈÓÚ1ÃëÖÓ
			{
				mod_tim_cnta=0;
				mod_tim_cnta2++;
				if (mod_tim_cnta2>=mod_seta_ont*60)// ¼äÐªÄ£Ê½ÏÂµÄ¸ßÑ¹¹¤×÷Ê±¼äÊÇ·ñ´ïµ½
				{
					record_ds_turn=0;
					twenty_seconds=0;
					


		//20160725 ÐÞ¸ÄÔÚ¼äÏ¶Ä£Ê½ÏÂ£¬±¨¹ÜÂ·¶ÂÈûºó£¬½øÈëµÍÑ¹Ä£Ê½²»Ïû³ý±¨¾¯
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// Ö»ÓÐµ±Ç°ÆøÑ¹´óÓÚÉè¶¨Ñ¹Á¦£¬ºó²ÅÓÐcon_flg_falla=1£»

					bbbbb=0;
					ddfq=1;
					ddfq_delay=0;
					mod_tim_cnta2=0;
					mod_jixa=1;// lwz ÇÐ»»µ½µÍÑ¹¹¤×÷×´Ì¬
				}
			}
		}
		else if (mod_jixa==1)// lwz ¼äÐªÄ£Ê½ÏÂµÄµÍÑ¹×´Ì¬
		{
			mod_seta_preh=mod_seta_prel;// lwz ÉèÖÃÆøÑ¹µÄÄ¿±êÖµÎ´µÍÑ¹
			
			if (adc_ps0>adc_zero)
				pwm_k1    =    (adc_ps0-adc_zero)/valueK;
			else
				pwm_k1=0;
			
			adc_ps00  =    (unsigned int)pwm_k1;
			
			if(ddfq==1)// Èç¹ûÐèÒªÔÚµÍÑ¹×´Ì¬ÏÂ·ÅÆø
			{
				if(ddfq_delay++ >= 12)// lwz ÔÚ12µ½75Ö®¼äÊôÓÚ·ÅÆø½×¶Î
				{
					VAL2=0;
					if(ddfq_delay >=75)
					{
						get_xx_delta(mod_seta_preh);
						if(adc_ps00 <= (mod_seta_preh+con_hi_delta))//if(adc_ps00 <= (mod_seta_preh+con_hi_delta+2))
						{
							ddfq=2;// lwz µÍÑ¹¹¤×÷Ä£Ê½ÏÂ·ÅÆøÒÑ¾­Íê³É£¬²»ÔÙ¼ÌÐø·ÅÆø£¬¹Ø±ÕVAL2
							mod_tim_cnta=0;
						}
						ddfq_delay=0;
					}
				}
				else
					VAL2=1;	


		//20160725 ÐÞ¸ÄÔÚ¼äÏ¶Ä£Ê½ÏÂ£¬±¨¹ÜÂ·¶ÂÈûºó£¬½øÈëµÍÑ¹Ä£Ê½²»Ïû³ý±¨¾¯
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// Ö»ÓÐµ±Ç°ÆøÑ¹´óÓÚÉè¶¨Ñ¹Á¦£¬ºó²ÅÓÐcon_flg_falla=1£»			
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
						if (mod_tim_cnta2>=mod_seta_oft*60)// ¼äÐªÄ£Ê½ÏÂµÄµÍÑ¹¹¤×÷Ê±¼äÊÇ·ñ´ïµ½
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

		if (key_val == KEY_OK )// lwz ¼äÐªÄ£Ê½ÏÂ£¬°´ÏÂOK£¬Ôò½øÈëÔÝÍ£Ä£Ê½
		{
			////20160420ÈÕÐÞ¸Ä£¬¼äÏ¶½øÈëÔÝÍ£Ä£Ê½£¬´Ó¸ßµÄ¿ªÊ¼½øÀ´
			mod_jixa=0;
			mod_tim_cnta=0;
			mod_tim_cnta2=0;
			/////////////////////////////////////////////////////
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)// lwz ¼äÐªÄ£Ê½ÏÂ£¬³¤°´×ó¼ýÍ·£¬Ôò½øÈëÉèÖÃÄ£Ê½
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;

			if (language)
			{
//				DISP_mod_set2015();
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
//				DISP_mod_set2015();
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
		if (mod_seta_cnt == UI_WORKMODE_SELECT)// lwz mod_seta_cntÎª0´ú±íµ±Ç°½çÃæÎªÑ¡ÔñÄ£Ê½
		{
			if (set_a00==0)// lwz µ±Ç°ÔÚ½çÃæÉÏÑ¡ÔñµÄÊÇÁ¬ÐøÄ£Ê½
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
			else if (set_a00==1)// lwz µ±Ç°ÔÚ½çÃæÉÏÑ¡ÔñµÄÊÇ¼äÐªÄ£Ê½
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
//						DISP_press_set2016();
					}
				}
			}
			else if (set_a00==2)// lwz µ±Ç°ÔÚ½çÃæÉÏÑ¡ÔñµÄÊÇÎÞÄ£Ê½
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
		else if (mod_seta_cnt==UI_MODE_SET_HI)// lwz mod_seta_cntÎª1´ú±í¼äÐªÄ£Ê½¸ßÑ¹Á¦ÉèÖÃ»òÕßÁ¬ÐøÄ£Ê½Ñ¹Á¦ÉèÖÃ
		{
			if (mod_seta_wok == MOD_LIX)// lwz Á¬ÐøÄ£Ê½ÏÂÉèÖÃÑ¹Á¦
			{
				if (key_val == KEY_UP)
				{
					mod_seta_preh=mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)// lwz Ñ¹Á¦³¬¹ýÉÏÏÞÔò·­×ª
					{
						mod_seta_preh=CFG_LOW;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_DN)
				{
					mod_seta_preh=mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)// lwz Ñ¹Á¦³¬¹ýÏÂÏÞÔò·­×ª
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
					mod_seta_cnt=UI_WORKMODE_SELECT;////////////////////////20151201ÐÞ¸Ä////////////////////
					mod_main_baka=mod_seta_wok;
					mod_main_a=MOD_ZHT;
					DISP_Clear();
					if (language)
					{
//						DISP_press_set2016();
					}
					else
					{
						;
					}
				}
			}
			else//¼äÏ¶Ä£Ê½Éè¸ßÑ¹Á¦ 
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
					// DISP_Clr3();
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

					// DISP_Clr3();
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

					// DISP_Clr3();
				}
				else if (key_val == KEY_OK)
				{

					// if(mod_seta_wok == MOD_ZXB)
					if (mod_seta_wok == MOD_JIX)
					{
						mod_seta_cnt=UI_JIX_MODE_SET_LO;
					}
					else
					{
						// mod_seta_prel=0;
						mod_seta_cnt=UI_JIX_SET_HI_TIME;
					}DISP_Clear();
					if (language)
					{
//						DISP_press_set2016();
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
		else if (mod_seta_cnt==UI_JIX_MODE_SET_LO)//¼äÏ¶Ä£Ê½ÉèµÍÑ¹Á¦
		{

			if (key_val == KEY_UP)
			{
				mod_seta_prel = mod_seta_prel+MMHG_PER_STEP;
				if(mod_seta_prel > JIX_LOW_MODE_HIGHPRESS || mod_seta_prel > mod_seta_preh - 10)
					mod_seta_prel = JIX_LOW_MODE_LOWPRESS;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				// DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{

				mod_seta_prel = mod_seta_prel-MMHG_PER_STEP;
				//20160606 add  Èç¹ûÑ¹Á¦Ð¡ÓÚ10£¬ÔòÎª¸ßÑ¹-5£¬ÊµÏÖÊý×Ö·´×ª
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				if(mod_seta_prel < JIX_LOW_MODE_LOWPRESS )
					mod_seta_prel = mod_seta_preh - 10;
				// DISP_Clr3();
			}
			else if (key_val == KEY_DNL)
			{

				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				//mod_seta_prel = mod_seta_prel-10;
				key_set_tim=0;
				mod_seta_prel = mod_seta_prel-MMHG_PER_STEP;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				//20160606 add  Èç¹ûÑ¹Á¦Ð¡ÓÚ10£¬ÔòÎª¸ßÑ¹-5£¬ÊµÏÖÊý×Ö·´×ª
				// DISP_Clr3();
			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				//mod_seta_prel = mod_seta_prel+10;
				key_set_tim=0;
				mod_seta_prel = mod_seta_prel+MMHG_PER_STEP;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				// DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				if (language)
				{
//					DISP_time_set2015();
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
		else if (mod_seta_cnt==UI_JIX_SET_HI_TIME)//¼äÏ¶Ä£Ê½³äONÊ±¼ä
		{
			if (key_val == KEY_UP)
			{
				//mod_seta_ont = mod_seta_ont+10;
				if (mod_seta_ont++>=MAX_JIX_HIGHMODE_TIME)
				{
					mod_seta_ont=MIN_JIX_HIGHMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{
				if (mod_seta_ont--<=MIN_JIX_HIGHMODE_TIME)
				{
					mod_seta_ont=MAX_JIX_HIGHMODE_TIME;
				}
				//DISP_Clr3();
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

				//DISP_Clr3();
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
				//DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=UI_JIX_SET_LO_TIME;
//				if (language==0)
//				{
					DISP_Clear();
//				}
			}

		}
		else if (mod_seta_cnt==UI_JIX_SET_LO_TIME)//¼äÏ¶Ä£Ê½ÉèOFFÊ±¼ä
		{
			if (key_val == KEY_UP)
			{
				if (mod_seta_oft++ >= MAX_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MIN_JIX_LOWMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{
				if (mod_seta_oft-- <= MIN_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MAX_JIX_LOWMODE_TIME;
				}
				//DISP_Clr3();
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
				//DISP_Clr3();
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
				//DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=0;
				mod_main_baka=mod_seta_wok;
				mod_main_a=MOD_ZHT;//½øÈëÔÝÍ£Ä£Ê½
				mod_tim_cnta=0;
				cnt_cnta=0;
				cnt_cntaa=0;
				mod_tim_cnta2=0;
				mod_jixa=0;
				// DISP_Clear();
			}
		}
		else if (mod_seta_cnt==UI_SET_PRESSURE)//Ã»ÓÐÓÃÉÏ
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
void  MODE_Pro(void)/*used*/
{
	if ( (mod_main_a != MOD_OFF )&&(mod_main_a != MOD_SYS ))
	{//////////////////////////////////////////////////////////////
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
		if ((bat_lev&0x0f)==LOWER_THAN_3_5V)// lwz µÍµçÑ¹ÏÂ£¬²»½øÐÐÕý³£¹Ø»ú²Ù×÷¡£ÍÆ²â£º¿ÉÄÜÊÇÎªÁË±ÜÃâÔÚÐ´FLASHÊ±¶Ïµã£¬´Ó¶ø»Ù»µÊý¾Ý
		{
			;
		}
		else
		{
			if (close_tim++>50)
			{
				ClrBlk();
				AUDIO_EN=0;
				Flash_Erase(addr);//²Á³ý1024¸ö×Ö½Ú,CPU ÔÚ²Á³ýÆÚ¼ä½«»áÍ£Ö¹¹¤×÷,ËùÒÔÎÞ·¨·ÂÕæ
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
				key_silent_flag = (key_silent_flag | language);// lwz ½«¾²Ä¬±êÖ¾±£´æµ½flashÖÐ
				Write_One_Word(addr+16,key_silent_flag);
				Write_One_Word(addr+18,mod_seta_prel);
				POWER_ON=0;
			}
		}
	}

	if (key_val==UNLOCK_NPWT_L_KEYVAL)// °´¼ü±»³¤°´£¬½âËø°´¼ü
	{
		lock_flg=0;
		lock_cnt=0;
	}
	if (key_val==UNLOCK_NPWT_KEYVAL)// °´¼ü±»³¤°´£¬½âËø°´¼ü
	{
		lock_flg=0;
		lock_cnt=0;
	}

	if (lock_flg==0) // lwz ·ÇËø¶¨×´Ì¬ Æô¶¯ºó½øÈëÔÝÍ£Ä£Ê½£¬°´ÁËPOWER+BUZ½øÐÐ»»ÓïÑÔ
	{
		if ( mod_main_a==MOD_WAT)
		{
			if (key_key==0)// lwz ÕâÀïÖ÷ÒªÊÇÎªÁËÅÐ¶Ï¡°ÔÚÇÐ»»ÓïÑÔÖ®Ç°£¬ÊÇ·ñÓÐÒ»¶ÎÎÞ°´¼ü°´ÏÂµÄ¿ÕÏ¶¡±£¬ÓÐ¿ÕÏ¶²ÅÔÊÐíÇÐ»»ÓïÑÔ
			{
//20160606 ½ûÖ¹ÓïÎÄÇÐ»»
// lwz ÓÉÓÚÕâ¸öÓïÑÔÇÐ»»¹¦ÄÜ²»ÍêÉÆ£¬ÉÐÓÐÐí¶àBUG£¬½ûµôÔ­À´µÄ¹¦ÄÜ
				if (key_val==LONG_PRESS_SWITCH_LANG)
				{
					key_key=1;
					// ÇÐ»»ÓïÑÔ
					//language=~language;
					if(0 == language)
					{
						language = 0xFE;
					}
					else
					{
						language = 0;
					}
					all_flg = 0;
					//mod_main_a=MOD_LANGUAGE;
					DISP_Clear();
					// DISP_LANGUAGE();
				}

			}
		}

	}
	if (mod_main_a!=mod_bak_bak)//Ä£Ê½ÇÐ»»Ç°ÏÈÇåÆÁ
	{
		DISP_Clear();
	}
	mod_bak_bak=mod_main_a;//±¸·Ýµ±Ç°Ä£Ê½
}





