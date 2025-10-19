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

unsigned char    con_flg_falla ;// lwz 1´ú±íºÄ²ÄºÐÐèÒª·ÅÆø¡¢0´ú±íºÄ²ÄºÐ²»ÓÃ·ÅÆø
unsigned short   gao_cnt ;
unsigned char    good_check=0 ;
unsigned char    nggood_check=0 ;
unsigned short   BOX_FQ_TURNS ;

unsigned short   tim_tima=0 ;

unsigned short   adc_ps00;// lwz µ±Ç°µÄÑ¹Á¦Öµ
// unsigned char    det300 = 0;
//20160505ÈÕÐÞ¸Ä£¬¼õÉÙÒºÎ»ÂúÎó±¨¾¯£¬¹ý³åÖµ
// unsigned char    det300 = 20;


//20160506ÈÕÐÞ¸Ä£¬¼õÉÙÒºÎ»ÂúÎó±¨¾¯£¬¹ý³åÖµ
// unsigned char    det300 = 15;
//20160603 ÈÕÐÞ¸Ä£¬³ÂÇÉÁÖ·´Ó¦20mmhgµÄÊ±ºòÒºÎ»Âú²»±¨¾¯
unsigned char    det300 = 10;
unsigned char    det00 ;
unsigned short con_hi_delta = 0;// ¸ßÑ¹²½¾à
unsigned short con_lo_delta = 0;// µÍÑ¹²½¾à

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
		//20160725 ÐÞ¸ÄÔÚ¼äÏ¶Ä£Ê½ÏÂ£¬±¨¹ÜÂ·¶ÂÈûºó£¬½øÈëµÍÑ¹Ä£Ê½²»Ïû³ý±¨¾¯
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// Ö»ÓÐµ±Ç°ÆøÑ¹´óÓÚÉè¶¨Ñ¹Á¦£¬ºó²ÅÓÐcon_flg_falla=1£»
	}
	else if (fall_stata==1)
	{
		fall_cnta0++;
		unsigned short fall_cnta0_count = 4;
		if(mod_seta_preh <=50)// µÍÑ¹Ð¹ÆøÊ±¼äÑÓ³¤
			fall_cnta0_count = 16;
		if (fall_cnta0>fall_cnta0_count) 
		{//////////////////////0.1
			VAL2 = 0 ;
			fall_stata=2;
			fall_cnta0=0;
		}
		//20160725 ÐÞ¸ÄÔÚ¼äÏ¶Ä£Ê½ÏÂ£¬±¨¹ÜÂ·¶ÂÈûºó£¬½øÈëµÍÑ¹Ä£Ê½²»Ïû³ý±¨¾¯
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// Ö»ÓÐµ±Ç°ÆøÑ¹´óÓÚÉè¶¨Ñ¹Á¦£¬ºó²ÅÓÐcon_flg_falla=1£»
	}
	else if (fall_stata==2)
	{
		fall_cnta0++;
		if (fall_cnta0>50)  //////30
		{//////////////////////0.1Ãë
			fall_stata=0;
			fall_cnta0=0;
			if (adc_ps00<(mod_seta_preh+con_hi_delta))// lwz Ð¹Æø´ïµ½Ä¿±ê
			{
				// if(debug_air!=0)
				// {
					// mod_seta_preh=debug_air;
					// debug_air=0;
				// }	
				con_flg_falla=0;//ºÐ×Ó²»ÓÃ·ÅÆø
				gao_cnt=0;
			}
		}
		//20160725 ÐÞ¸ÄÔÚ¼äÏ¶Ä£Ê½ÏÂ£¬±¨¹ÜÂ·¶ÂÈûºó£¬½øÈëµÍÑ¹Ä£Ê½²»Ïû³ý±¨¾¯
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// Ö»ÓÐµ±Ç°ÆøÑ¹´óÓÚÉè¶¨Ñ¹Á¦£¬ºó²ÅÓÐcon_flg_falla=1£»
		
	}
}

void get_press_delta(unsigned short press)
{
	unsigned short delta = 8;
	// lwz ¸ü¸Ä£º³ýÁËDeroyal°æÒâÍâ£¬ÆäËûµÄ°æ±¾½«¸¡¶¯ãÐÖµÀ©´ó£¬¾¡Á¿¼õÉÙ±ÃµÄÆô¶¯´ÎÊý
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
	con_lo_delta = press/10;// lwz ·Çderoyal°æ£¬10%µÄ²¹ÆøÅÐ¶¨Ìõ¼þ
	// lwz ¶ÔÓÚ³äÆøÍ£Ö¹Ìõ¼þ£¬×ö³öµ÷Õû
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
	// 20160526 Õý15£¬¸º10
} 




/*----------------16Î»ÓÐ·ûºÅÊý¼ÆËã²îÖµ-----------------------------------------*/
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
unsigned char   overabc = 0;// lwz Îª1£¬±íÊ¾Â©Æø³¬¹ý10·ÖÖÓ
unsigned short   val_cnt=0;
unsigned long scan_dusai_time = 0;// lwz ÅÐ¶Ï¶ÂÈûÊ±¼ä£¬ÔÚ²¹ÆøºÍÇÐ»»Ö÷Ä£Ê½µÄÊ±ºò£¬Õâ¸ö±êÖ¾»áÇåÁã
unsigned char xxturn=0;
unsigned char xxok=0;
signed int result=0;

unsigned short record_ds[8];
unsigned char  record_ds_turn=0;// µ±Ç°µÄÊý×éË÷Òý
unsigned short twenty_seconds=0;
// lwz ¸ù¾Ýµ±Ç°µÄÑ¹Á¦×ö³öÅÐ¶Ï£¬´¦ÀíÑ¹Á¦¹ýÐ¡¡¢¹ý´ó¡¢ÒºÎ»Âú¡¢Â©ÆøµÈÇé¿ö
void STAT_conNewa(void)/*used*/
{
	unsigned short temp=0;
	unsigned short run_time=0;
	if (mod_seta_preh==0)/*Èç¹ûÉè¶¨µÄÑ¹Á¦ÊÇ0¾Í²»ÒªÍæ*/
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
	if (flager_a&ERRA_S)/*ÒºÎ»Âú*/
	{
		np_good_times=0;
		VAL1 = 0 ;
		VAL2 = 0 ;
		//bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
		CLS_PwmA();//////////////////////ÒºÎ»ÂúÐèÒªÈ¡Ïûºó²ÅÄÜÊ¹ÓÃ
	}
	else
	{
		temp = calc_delta_i(mod_seta_preh,con_lo_delta);
		unsigned short target_press = mod_seta_preh+con_hi_delta;
		if(target_press >= TOP_PRESSURE_MAX)
			target_press = TOP_PRESSURE_MAX-3;
		if (adc_ps00<=temp)//Ñ¹Á¦Ð¡ÓÚµ×ÏÞ´¦Àí
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
			flager_a &=~ERRB_DS;// Ö»ÓÐµ±Ç°ÆøÑ¹´óÓÚÉè¶¨Ñ¹Á¦£¬ºó²ÅÓÐcon_flg_falla=1£»
			if (con_flg_falla==0)//ºÐ×Ó²»ÐèÒª·ÅÆø
			{
				BOX_FQ_TURNS++;
				if (BOX_FQ_TURNS>10)/*Ð¹ÆøµÄ×´Ì¬Ã»ÓÐÍê³ÉÇ°²»ÄÜ¿ª±Ã:40ms*/  
				//if (BOX_FQ_TURNS>99)/*Ð¹ÆøµÄ×´Ì¬Ã»ÓÐÍê³ÉÇ°²»ÄÜ¿ª±Ã:500ms*/   //20160509
				{
					BOX_FQ_TURNS=0;
					
					if (mod_jixa!=1)
						VAL2 = 0 ;//ºÐ×Ó·ÅÆø·§¹Ø±Õ
					else
					{
						if(ddfq==2)	
							VAL2 = 0 ;//ºÐ×Ó·ÅÆø·§¹Ø±Õ
					}
					// if(debug_air!=0)
					// {
						// mod_seta_preh=debug_air;
						// debug_air=0;
					// }
					if (flager_a&ERRA_LQ)//Èç¹ûÈ·ÈÏÒÑ¾­ÓÐÁËÂ©Æø±êÖ¾
					{
					    // lwz ¾ÅÃ··´À¡¼äÐªÄ£Ê½µÄÂ©ÆøÊ±¼ä²»×ã£¬Ðèµ¥¶ÀÕë¶Ô¼äÐªÄ£Ê½À´µ÷Õû£¬Ô­Ê±¼ä£º1·Ö12Ãë
						if (load_perioda_up>JUDGE_LEAKAGE_TIMEOUT+1200  // Á¬ÐøÄ£Ê½ÏÂ£¬Â©Æø³¬¹ý10·ÖÖÓ
						    || load_perioda_up>(JUDGE_LEAKAGE_TIMEOUT)*JX_MODE_TIME_FACTOR_BY_LX // ¼äÐªÄ£Ê½ÏÂÂ©Æø³¬¹ý10·ÖÖÓ
							)/*Â©Æø³¬10·ÖÖÓ*/
#if IS_NO_LIQUID_ALARM_STOP==0
						{
							//±ÃÉÏµÄ·§VA1²»Òª·ÅÆø
							//bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
							CLS_PwmA();// lwz Èç¹û·¢ÉúÁËÂ©Æø£¬ÔòÍ£±Ã
							overabc = 1;
						}
						else
#endif
						{
							//±ÃÉÏµÄ·§VA1Òª·ÅÆøÒ»Ç·,×¼±¸´ò¿ª±Ã³äÆø
							//bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
							OPEN_PwmA();
							overabc = 0;
						}
					}
					else
					{
						//±ÃÉÏµÄ·§VA1Òª·ÅÆøÒ»Ç·,×¼±¸´ò¿ª±Ã³äÆø
						//bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
						OPEN_PwmA();
						overabc = 0;
					}
				}
			}
		}
		//else if (adc_ps00>=(mod_seta_preh+con_hi_delta))//Ñ¹Á¦´óÓÚÉÏÏÞ´¦Àí

		//else if	(((adc_ps00>=mod_seta_preh+17))||((mod_seta_preh>=200)&&(adc_ps00>=mod_seta_preh+12))||((mod_seta_preh<=100)&&(adc_ps00>=mod_seta_preh+7)))
		else if (adc_ps00>=(target_press))//Ñ¹Á¦´óÓÚÉÏÏÞ´¦Àí
		{
			gao_cnt++;
			if (gao_cnt>999)
				gao_cnt=1000;
			// lwz ¸ù¾Ýµ±Ç°Éè¶¨µÄÑ¹Á¦À´ÅÐ¶ÏÐèÒªµÈ´ý¸ßÑ¹µÄÊ±¼ä£¬Ä¿±êÑ¹Á¦Ô½¸ß£¬µÈ´ýµÄÊ±¼ä¾ÍÔ½³¤£¬µÈ´ýÊ±¼ä=Ä¿±êÑ¹Á¦*2
			run_time=mod_seta_preh/10+2;
			// lwz ¶ÔÓÚÐ¡ÓÚµÈÓÚ50mmhg£¬±Ã¿ÉÒÔÑÓ³ÙÆô¶¯£¬ÒÔÈ·±£±Ã²»»á¶à´ÎÆô¶¯
			if(run_time < 15)
			{
				run_time=15;
			}
			// lwz ²âÊÔÖÐ·¢ÏÖ£¬¼äÐªÄ£Ê½ÏÂ£¬±£Ñ¹Ê±¼ä²»¹»£¬ÔÚÕâÀï²¹³¥Ò»µã
			if(mod_main_a == MOD_JIX)
			{
				run_time=run_time+4;
			}
			run_time=run_time*20;
			if(run_time<=20)
				run_time=20;
			if(run_time>=400)
				run_time=400;
			if (gao_cnt>run_time) //20160606 lwz Ê±¼äµ½£¬¹Ø±Ã
			{
				CLS_PwmA();
				PUMP=0;
				open_bum=0;
			}
			if (gao_cnt>(run_time+JUDGE_YWM_AFTER_PUMP_STOP)) 
			{
/*  				if (oppump_flg)//·ÀÖ¹ÒºÎ»ÂúÎó±¨¾¯
				{
					cnt_cnta++;	//20ÃëÄÚ4´ÎÕðµ´ÀÛ¼Æ
					oppump_flg = 0;
					con_flg_falla=1;
					fall_stata=0;
					fall_cnta0=0;
					gao_cnt=0;
				} 	 */
	// lwz Èç¹û¸ßÑ¹¼ÆÊý´óÓÚ499£¬²¢ÇÒ±ÃÔÚÉÏÒ»´ÎÕðµ´ÀÛ¼ÆÖ®ºó¿ª¹ý£¬¼´ÆøÑ¹£ºadc_ps00<=(mod_seta_preh-10)Ìõ¼þ±»Ö´ÐÐ¹ý
 				if (con_flg_falla==0)//·ÀÖ¹ÒºÎ»ÂúÎó±¨¾¯ lwz µ±Ç°²»ÔÚÒºÎ»ÂúµÄ·ÅÆø½×¶Î
				{
					cnt_cnta++;	//20ÃëÄÚ4´ÎÕðµ´ÀÛ¼Æ
					oppump_flg = 0;
					con_flg_falla=1;
					fall_stata=0;// lwz ·ÅÆø¿ªVAL2±êÖ¾
					fall_cnta0=0;
					gao_cnt=0;
				} 	
				
			}
			record_ds_turn=0;
			twenty_seconds=0;
			good_check=0;
			np_good_times=0;
		}
		else//Ñ¹Á¦´óÓÚµ×ÏÞÐ¡ÓÚÉÏÏÞ
		{
			#if IS_NO_LIQUID_ALARM_STOP==0
			if (flager_a&ERRA_LQ)
			{
				if (load_perioda_up>JUDGE_LEAKAGE_TIMEOUT+1200
				|| load_perioda_up>(JUDGE_LEAKAGE_TIMEOUT)*JX_MODE_TIME_FACTOR_BY_LX // ¼äÐªÄ£Ê½ÏÂÂ©Æø³¬¹ý10·ÖÖÓ
				)
				{
					//bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
					CLS_PwmA();//²»Òª¿ª±ÃÉÏµÄ·ÅÆø·§ 	lwz ¹Ø±Ã
				}
			}
			#endif
			if (scan_dusai_time++>JUDGE_DS_TIME)/*Èç¹û³ÖÐø15·ÖÖÓÄÚÃ»ÓÐ²¹Æø¡¢·ÅÆø±íÊ¾ÓÐ¶ÂÈû*/
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
			//20160524 add mafacai///////////
			twenty_seconds++;
			// lwz µÍÑ¹×´Ì¬ÏÂ£¬
			unsigned short record_pre_compare_interval;
			if(adc_ps00 >= MIN_PRESS_REPORT_DS)
			{
				// lwz ¼äÐªÄ£Ê½µÄ¹ÜÂ·¶ÂÈû±¨¾¯ÖÜÆÚ¼ÆËãÊ±Ó¦¸Ã¼Ó±¶£¬ÕâÑùµÃµ½µÄÊµ¼ÊÊ±¼ä²ÅÄÜ×¼È·
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
				// lwz ¼äÐªÄ£Ê½µÄ¹ÜÂ·¶ÂÈû±¨¾¯ÖÜÆÚ¼ÆËãÊ±Ó¦¸Ã¼Ó±¶£¬ÕâÑùµÃµ½µÄÊµ¼ÊÊ±¼ä²ÅÄÜ×¼È·
				if(mod_main_a == MOD_JIX)
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL_LOW*JX_MODE_TIME_FACTOR_BY_LX;
				}
				else
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL_LOW;
				}
			}
			if(twenty_seconds>=record_pre_compare_interval)//180000ul/15/3  Ã¿¸ô15Ãë²é¼ìÒ»´Î£¬
			{
				record_ds[record_ds_turn]=adc_ps00;
				record_ds_turn++;
				if(record_ds_turn>7)
				{
					if((record_ds[7] == record_ds[0])||((record_ds[7]+1) == record_ds[0]))
					{
						// lwz Õâ²¿·Ö´úÂëµÄÄ¿µÄÊÇÎªÁËÌá¸ßÔÚµÍÑ¹×´Ì¬ÏÂÅÐ¶¨¹ÜÂ·¶ÂÈûµÄÃÅ¼÷
						// lwz Èç¹û´óÓÚ´ËÖµ£¬ÔòÖ±½ÓÉèÖÃÎª¹ÜÂ·¶ÂÈû
						if(record_ds[7] >= MIN_PRESS_REPORT_DS)
						{
							flager_a|=ERRB_DS;// lwz Ñ¹Á¦Ç°ºóÃ»ÓÐ±ä»¯£¬ÉèÖÃ¶ÂÈû±êÖ¾
						}
						else if(record_ds[7] > NO_REPORE_PRESS_FOR_DS)
						{
							// lwz Èç¹û´óÓÚNO_REPORE_PRESS_FOR_DS£¬Ôò¼ÆËãÊý×éÆ½¾ùÖµ£¬Èç¹ûµÈÓÚrecord_ds[7]£¬²ÅÅÐ¶¨Îª¹ÜÂ·¶ÂÈû
							if((record_ds[0]+record_ds[1]+record_ds[2]+record_ds[3]+record_ds[4]+record_ds[5]+record_ds[6])/7 == record_ds[7])
							{
								flager_a|=ERRB_DS;// lwz Ñ¹Á¦Ç°ºóÃ»ÓÐ±ä»¯£¬ÉèÖÃ¶ÂÈû±êÖ¾
							}
						
}// lwz Ð¡ÓÚNO_REPORE_PRESS_FOR_DS£¬Ôò²»±¨¹ÜÂ·¶ÂÈû
					}
					else
					{
						flager_a &=~ERRB_DS;// Ö»ÓÐµ±Ç°ÆøÑ¹´óÓÚÉè¶¨Ñ¹Á¦£¬ºó²ÅÓÐcon_flg_falla=1£»
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
			//////////////////////////////////
				
		}
		if (con_flg_falla)//µ±Ç°Ñ¹Á¦´óÓÚÉè¶¨Ñ¹Á¦+ÉÏÏÞ£¬¸øºÐ×Ó·ÅÆøÒ»Ð¡»á¶ù£¬Ö´ÐÐÍêºócon_flg_falla=0£»
		{
			CONTR_fallaNew();
		}
	}
}

























