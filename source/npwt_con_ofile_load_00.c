#include   "include.h"
#include   "npwt_dis_main.h"

/*
hpp: PWMé¢çä¿®æ¹å°10kHzæ¶ï¼æ¬æä»¶ä¸­éè¦ä¿®æ¹çå°æ¹ï¼

1. pwm_cnt2åéç±»åä¿®æ¹:
   - å¨npwt_dis_sys_ini_00.cä¸­å°unsigned char pwm_cnt2æ¹ä¸ºfloat pwm_cnt2

2. pwm_cnt2èµå¼é»è¾ä¿®æ¹ (ç¬¬134-156è¡):
   - pwm_cnt2=4; æ¹ä¸º pwm_cnt2=0.4;  // 40%å ç©ºæ¯
   - pwm_cnt2=5; æ¹ä¸º pwm_cnt2=0.5;  // 50%å ç©ºæ¯  
   - pwm_cnt2=6; æ¹ä¸º pwm_cnt2=0.6;  // 60%å ç©ºæ¯

3. PWMæ§å¶é»è¾ä¿®æ¹:
   - éè¦ä¿®æ¹PWMæ§å¶é»è¾ä»¥æ¯æå°æ°å ç©ºæ¯
   - å¯è½éè¦ä½¿ç¨ç´¯å å¨æ¥å®ç°å°æ°å ç©ºæ¯æ§å¶
*/
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"

unsigned short	 load_perioda_up;// lwz ï¿½Ð¶ï¿½Â©ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½
unsigned short   cnt_cnta,cnt_cntaa;

unsigned char    bump_need_out_air_flg;


unsigned short   mod_seta_prehh;
float    pwm_k1;// lwz ï¿½ï¿½Í¬ï¿½ï¿½adc_ps00ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ç°Ñ¹ï¿½ï¿½Öµ
unsigned short   fq_cnt1=0;
// lwz ï¿½ï¿½ï¿½ï¿½
void OPEN_PwmA(void)
{
	bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
}
// lwz ï¿½Ø±ï¿½
void CLS_PwmA(void)/*used*/
{
	bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
}


const  char mot_sys[]={
	0,  0,
	25, 25, 25, 25, 25, 25, 30, 25, 25,         ////20---100
//20  30  40  50  60  70  80  90  100
	25, 25, 20, 20, 25, 25, 25, 25, 25,  20,    ////110---200
//110 120 130 140 150 160 170 180 190  200
	20, 20, 20, 20, 15, 15, 15, 15, 15,  15     ////210---300
//210 220 230 240 250 260 270 280 290  300

};

/* void PRESS_ConSubA(void)
{
	unsigned short i;

	LEVEL_WarnA();

	mod_seta_prehh=mod_seta_preh;
	if (adc_ps0>adc_zero)
	{
		pwm_k1    =    (adc_ps0-adc_zero)/valueK;
	}
	else
	{
		pwm_k1=0;
	}
	adc_ps00  =    (unsigned int)pwm_k1;
	if (adc_ps00>320)
	{
		adc_ps00=320;
	}
	pwm_k1    =    (mod_seta_preh-50.0)*0.6+10;
	if (mod_seta_preh<=20)
	{
		pwm_cnt2=4;    
	}
	else if (mod_seta_preh<70)      
	{
		pwm_cnt2  =    4;   
	}
	else if (mod_seta_preh<130)     
	{
		pwm_cnt2  =    4;   
	}
	else if (mod_seta_preh<=290)
	{
		pwm_cnt2  =    5;  
	}
	else
	{
		pwm_cnt2  =    6;  
	}

	i       =     mod_seta_preh/10;
	det00   =     mot_sys[i];
	STAT_conNewa();
} */
 // lwz ï¿½ï¿½ï¿½Â©ï¿½ï¿½ï¿½ï¿½ï¿½Ð¶ï¿½ÒºÎ»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ãµ±Ç°Ñ¹ï¿½ï¿½Öµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
void PRESS_ConSubA(void)
{
	unsigned short i;

	LEVEL_WarnA();// lwz ï¿½ï¿½ï¿½Â©ï¿½ï¿½ï¿½ï¿½ï¿½Ð¶ï¿½ÒºÎ»ï¿½ï¿½

	mod_seta_prehh=mod_seta_preh;
	if (adc_ps0>adc_zero)
	{
		pwm_k1    =    (adc_ps0-adc_zero)/valueK;// lwz ï¿½ï¿½ï¿½ãµ±Ç°ï¿½ï¿½ï¿½ï¿½Ñ¹Öµ = ï¿½ï¿½Ç°ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½Öµ ï¿½ï¿½È¥ ï¿½ï¿½ï¿½ï¿½Ñ¹ ï¿½Ä²î£¬ï¿½Ù³ï¿½ï¿½ï¿½valueK
	}
	else
	{
		pwm_k1=0;
	}
	adc_ps00  =    (unsigned int)pwm_k1;
	if (adc_ps00>TOP_PRESSURE_MAX)
	{
		adc_ps00=TOP_PRESSURE_MAX;
	}
	pwm_k1    =    (mod_seta_preh-50.0)*0.6+10;
/* 	if (mod_seta_preh<=20)
	{
		pwm_cnt2=4;    
	}
	else if (mod_seta_preh<70)      
	{
		pwm_cnt2  =   4;   
	}
	else if (mod_seta_preh<130)     
	{
		pwm_cnt2  =    4;   
	}
	else if (mod_seta_preh<=290)
	{
		pwm_cnt2  =    5;  
	}
	else
	{
		pwm_cnt2  =    6;  
	}
 */
//	pwm_cnt2  =    5;  
	//20160601  ï¿½ï¿½0.4ï¿½ï¿½Ä¤ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ì«ï¿½ó£¬¿ÕºÄ²Äºï¿½ï¿½ï¿½40mmHGï¿½Â²ï¿½ï¿½ï¿½ï¿½ï¿½Â©ï¿½ï¿½ï¿½ï¿½ï¿½Ö¸ï¿½ï¿½Ï²ï¿½ï¿½ï¿½
	if (mod_seta_preh<=20)//20160630ï¿½ï¿½Ç°ï¿½ï¿½3ï¿½ï¿½ï¿½ï¿½Ð©ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½4
	{
		pwm_cnt2=4;    //  3//2
	}
	else if (mod_seta_preh<70)        //  80
	{
		pwm_cnt2  =    4;    //  3
	}
//	else if (mod_seta_preh<130)       //  150
	else if (mod_seta_preh<100)       //  150
	{
		pwm_cnt2  =    4;    //  4
	}
	//  else if(mod_seta_preh<160)
	// 	{pwm_cnt2  =    5;}       //  4
	else if (mod_seta_preh<=290)
	{
		pwm_cnt2  =    5;    //  5
	}
	else
	{
		pwm_cnt2  =    6;    //
	} 	
	
	
	i       =     mod_seta_preh/10;
	det00   =     mot_sys[i];
	STAT_conNewa();// lwz ï¿½ï¿½ï¿½Ýµï¿½Ç°ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ð¶Ï£ï¿½ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ï¿½Ð¡ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ÒºÎ»ï¿½ï¿½ï¿½ï¿½Â©ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
}

// ï¿½ï¿½È¡ï¿½ï¿½Ç°Ñ¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ýµï¿½Ç°Ñ¹ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ó¦
void PRESS_ConA(void)/*used*/
{
	switch (mod_main_a)
	{
	case MOD_SYS:
	{
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz Â©ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		break;
	}
	case MOD_WAT:
	{
		VAL1=0;
		VAL2=0;
		//flager_a = 0;     //flager_a = 0;      20160330È¥ï¿½ï¿½
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		load_perioda_up=0;/// lwz Â©ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		break;
	}
	case MOD_LIX:
	{
		PRESS_ConSubA();
		break;
	}
	case MOD_JIX:
	{  
		if((ddfq!=1)&&(ddfq!=2))
		//if((ddfq==0)||((mod_jixa==1)&&(ddfq=0)))
			PRESS_ConSubA();
		else
			
		break;
	}
	case MOD_ZXB:
	{
		PRESS_ConSubA();
		break;
	}
	case MOD_ZHT:
	{
		VAL1=0;
		VAL2=0;
		//flager_a = 0;     //flager_a = 0;      20160330È¥ï¿½ï¿½
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz Â©ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		CLS_PwmA();
		break;;
	}
	case MOD_SET:
	{
		VAL1=0;
		VAL2=0;
		//flager_a = 0;     //flager_a = 0;      20160330È¥ï¿½ï¿½
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz Â©ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
		CLS_PwmA();
		break;;
	}
	case MOD_ERR:
	{
		break;
	}
	case MOD_OFF:
	{
		VAL1=0;
		VAL2=0;
		CLS_PwmA();
		break;
	}
	default:
	{
		break;
	}
	}
}

