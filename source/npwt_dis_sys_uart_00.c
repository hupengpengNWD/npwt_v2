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
	// lwz 从flash中读取的6到7字节的数据赋值给mod_main_baka
	// lwz 初步推断，这个代表工作模式
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
		// lwz 根据工作模式的不同，设置set_a00的值
    	if(mod_main_baka==MOD_LIX)
    		{set_a00=0;}
    	else if(mod_main_baka==MOD_JIX)
    		{set_a00=1;}
        else if(mod_main_baka==MOD_ZXB)
    		{set_a00=2;}
 
 
        // mod_seta_preh = i; 
        // mod_seta_preh_bak = i; 
		// lwz 从flash中读取的0到1字节的数据赋值给mod_seta_preh
        mod_seta_preh=mod_seta_preh_bak=dataREAD1;
		// lwz 判断是否超限，若超出上限，设为默认值120
        if(mod_seta_preh>CFG_HI)
        	{mod_seta_preh=mod_seta_preh_bak=DEFAULT_TARGET_PRESSURE;}
        
      
        // mod_seta_prel = 0; 
		// lwz 若mod_seta_preh小于0，则将mod_seta_prel设为40
        if(mod_seta_prel>mod_seta_preh)
        	{ mod_seta_prel = 40; }
		// lwz 从flash中读取的2到3字节的数据赋值给mod_seta_ont
        mod_seta_ont  = dataREAD2;
        if(mod_seta_ont>99)
        	{ mod_seta_ont = 5; }
		// lwz 从flash中读取的4到5字节的数据赋值给mod_seta_oft
        mod_seta_oft  = dataREAD3;
        if(mod_seta_oft>99)
        	{ mod_seta_oft = 2; }

        //mod_seta_preh=mod_seta_preh_bak=120;
        // mod_seta_prel = 0; 
        //  mod_seta_ont = 5;       
        //  mod_seta_oft = 2; 
}





























































































































