#include   "include.h"
#include   "npwt_dis_main.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"

unsigned short	 load_perioda_up;// lwz 判断漏气的时间
unsigned short   cnt_cnta,cnt_cntaa;

unsigned char    bump_need_out_air_flg;


unsigned short   mod_seta_prehh;
float    pwm_k1;// lwz 等同于adc_ps00，即当前压力值
unsigned short   fq_cnt1=0;
// lwz 开泵
void OPEN_PwmA(void)
{
	bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
}
// lwz 关泵
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
 // lwz 检测漏气并判断液位满，计算当前压力值，决定开泵命令次数
void PRESS_ConSubA(void)
{
	unsigned short i;

	LEVEL_WarnA();// lwz 检测漏气并判断液位满

	mod_seta_prehh=mod_seta_preh;
	if (adc_ps0>adc_zero)
	{
		pwm_k1    =    (adc_ps0-adc_zero)/valueK;// lwz 计算当前的气压值 = 当前传感器的压力值 减去 大气压 的差，再除以valueK
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
	//20160601  用0.4的膜，低压力，阻力太大，空耗材盒在40mmHG下不报警漏气，恢复老参数
	if (mod_seta_preh<=20)//20160630日前用3，有些电机启动不良，改用4
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
	STAT_conNewa();// lwz 根据当前的压力做出判断，处理压力过小、过大、液位满、漏气等情况
}

// 获取当前压力，并根据当前压力做出反应
void PRESS_ConA(void)/*used*/
{
	switch (mod_main_a)
	{
	case MOD_SYS:
	{
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz 漏气时间清零
		break;
	}
	case MOD_WAT:
	{
		VAL1=0;
		VAL2=0;
		//flager_a = 0;     //flager_a = 0;      20160330去除
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		load_perioda_up=0;/// lwz 漏气时间清零
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
		//flager_a = 0;     //flager_a = 0;      20160330去除
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz 漏气时间清零
		CLS_PwmA();
		break;;
	}
	case MOD_SET:
	{
		VAL1=0;
		VAL2=0;
		//flager_a = 0;     //flager_a = 0;      20160330去除
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz 漏气时间清零
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

