#include  "include.h"
#include "system_manager.h"
#include "global_compat.h"
#include "hardware_abstraction.h"
#include   "npwt_dis_main.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"

volatile unsigned char flager_a,bat_lev;

void UART_Test(void)
{
	flager_a=0;	
	bat_lev=0;

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
		
		if(mod_main_baka==MOD_LIX)
			{set_a00=0;}
		else if(mod_main_baka==MOD_JIX)
			{set_a00=1;}
		else if(mod_main_baka==MOD_ZXB)
			{set_a00=2;}

		mod_seta_preh=mod_seta_preh_bak=dataREAD1;
		
		if(mod_seta_preh>CFG_HI)
			{mod_seta_preh=mod_seta_preh_bak=DEFAULT_TARGET_PRESSURE;}

		if(mod_seta_prel>mod_seta_preh)
			{ mod_seta_prel = 40; }
		
		mod_seta_ont  = dataREAD2;
		if(mod_seta_ont>99)
			{ mod_seta_ont = 5; }
		
		mod_seta_oft  = dataREAD3;
		if(mod_seta_oft>99)
			{ mod_seta_oft = 2; }

}

