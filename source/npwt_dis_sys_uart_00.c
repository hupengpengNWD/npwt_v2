#include  "include.h"
#include   "npwt_dis_main.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"

volatile unsigned char flager_a,bat_lev;

void UART_Test(void)/*used*/
{
	flager_a=0;	
	bat_lev=0;
	// lwz ´ÓflashÖÐ¶ÁÈ¡µÄ6µ½7×Ö½ÚµÄÊý¾Ý¸³Öµ¸ømod_main_baka
	// lwz ³õ²½ÍÆ¶Ï£¬Õâ¸ö´ú±í¹¤×÷Ä£Ê½
		mod_main_baka =  dataREAD0; 
		mod_bak_bak   =  dataREAD0;	
		if(mod_main_baka!=MOD_LIX)
		{
			if(mod_main_baka!=MOD_JIX)
			{
				if(mod_main_baka!=MOD_ZXB)
					{mod_main_baka=MOD_LIX;}
			}
		}
		// lwz ¸ù¾Ý¹¤×÷Ä£Ê½µÄ²»Í¬£¬ÉèÖÃset_a00µÄÖµ
		if(mod_main_baka==MOD_LIX)
			{set_a00=0;}
		else if(mod_main_baka==MOD_JIX)
			{set_a00=1;}
		else if(mod_main_baka==MOD_ZXB)
			{set_a00=2;}
 
 
		// mod_seta_preh = i; 
		// mod_seta_preh_bak = i; 
		// lwz ´ÓflashÖÐ¶ÁÈ¡µÄ0µ½1×Ö½ÚµÄÊý¾Ý¸³Öµ¸ømod_seta_preh
		mod_seta_preh=mod_seta_preh_bak=dataREAD1;
		// lwz ÅÐ¶ÏÊÇ·ñ³¬ÏÞ£¬Èô³¬³öÉÏÏÞ£¬ÉèÎªÄ¬ÈÏÖµ120
		if(mod_seta_preh>CFG_HI)
			{mod_seta_preh=mod_seta_preh_bak=DEFAULT_TARGET_PRESSURE;}
		
	  
		// mod_seta_prel = 0; 
		// lwz Èômod_seta_prehÐ¡ÓÚ0£¬Ôò½«mod_seta_prelÉèÎª40
		if(mod_seta_prel>mod_seta_preh)
			{ mod_seta_prel = 40; }
		// lwz ´ÓflashÖÐ¶ÁÈ¡µÄ2µ½3×Ö½ÚµÄÊý¾Ý¸³Öµ¸ømod_seta_ont
		mod_seta_ont  = dataREAD2;
		if(mod_seta_ont>99)
			{ mod_seta_ont = 5; }
		// lwz ´ÓflashÖÐ¶ÁÈ¡µÄ4µ½5×Ö½ÚµÄÊý¾Ý¸³Öµ¸ømod_seta_oft
		mod_seta_oft  = dataREAD3;
		if(mod_seta_oft>99)
			{ mod_seta_oft = 2; }

		//mod_seta_preh=mod_seta_preh_bak=120;
		// mod_seta_prel = 0; 
		//  mod_seta_ont = 5;       
		//  mod_seta_oft = 2; 
}





























































































































