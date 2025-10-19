#include   "include.h"
#include   "npwt_dis_main.h"

/*
hpp: PWM频率修改到10kHz时，本文件中需要修改的地方：

1. pwm_cnt2变量类型修改:
   - 在npwt_dis_sys_ini_00.c中将unsigned char pwm_cnt2改为float pwm_cnt2

2. pwm_cnt2赋值逻辑修改 (第134-156行):
   - pwm_cnt2=4; 改为 pwm_cnt2=0.4;  // 40%占空比
   - pwm_cnt2=5; 改为 pwm_cnt2=0.5;  // 50%占空比  
   - pwm_cnt2=6; 改为 pwm_cnt2=0.6;  // 60%占空比

3. PWM控制逻辑修改:
   - 需要修改PWM控制逻辑以支持小数占空比
   - 可能需要使用累加器来实现小数占空比控制
*/
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"

unsigned short	 load_perioda_up;// lwz �ж�©����ʱ��
unsigned short   cnt_cnta,cnt_cntaa;

unsigned char    bump_need_out_air_flg;


unsigned short   mod_seta_prehh;
float    pwm_k1;// lwz ��ͬ��adc_ps00������ǰѹ��ֵ
unsigned short   fq_cnt1=0;
// lwz ����
void OPEN_PwmA(void)
{
	bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
}
// lwz �ر�
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
 // lwz ���©�����ж�Һλ�������㵱ǰѹ��ֵ�����������������
void PRESS_ConSubA(void)
{
	unsigned short i;

	LEVEL_WarnA();// lwz ���©�����ж�Һλ��

	mod_seta_prehh=mod_seta_preh;
	if (adc_ps0>adc_zero)
	{
		pwm_k1    =    (adc_ps0-adc_zero)/valueK;// lwz ���㵱ǰ����ѹֵ = ��ǰ��������ѹ��ֵ ��ȥ ����ѹ �Ĳ�ٳ���valueK
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
	//20160601  ��0.4��Ĥ����ѹ��������̫�󣬿պĲĺ���40mmHG�²�����©�����ָ��ϲ���
	if (mod_seta_preh<=20)//20160630��ǰ��3����Щ�����������������4
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
	STAT_conNewa();// lwz ���ݵ�ǰ��ѹ�������жϣ�����ѹ����С������Һλ����©�������
}

// ��ȡ��ǰѹ���������ݵ�ǰѹ��������Ӧ
void PRESS_ConA(void)/*used*/
{
	switch (mod_main_a)
	{
	case MOD_SYS:
	{
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz ©��ʱ������
		break;
	}
	case MOD_WAT:
	{
		VAL1=0;
		VAL2=0;
		//flager_a = 0;     //flager_a = 0;      20160330ȥ��
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		load_perioda_up=0;/// lwz ©��ʱ������
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
		//flager_a = 0;     //flager_a = 0;      20160330ȥ��
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz ©��ʱ������
		CLS_PwmA();
		break;;
	}
	case MOD_SET:
	{
		VAL1=0;
		VAL2=0;
		//flager_a = 0;     //flager_a = 0;      20160330ȥ��
		flager_a &=~ERRA_LQ;
		flager_a &=~ERRA_V;
		flager_a &=~ERRA_YW;
		flager_a &=~ERRA_S;
		flager_a &=~ERRB_DS;
		flager_a &=~ERRB_YW;
		flager_a &=~ERRB_S;
		cnt_cnta=cnt_cntaa=0;
		load_perioda_up=0;/// lwz ©��ʱ������
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

