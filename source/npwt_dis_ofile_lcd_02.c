#include  "include.h"
#include  "sys_cpu.h"
#include  "BIOS_JLX1864G_139.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include   "npwt_dis_main.h"
#include   "npwt_dis_ofile_lcd_02.h"
/*
Ĭ��Ӣ��/���   0
Ĭ������/Ӣ�   FF
*/
unsigned char        language=0;
#define      det   32
unsigned char   bat_cnt,bat_cnt1;
unsigned char   led_cnt=0;
unsigned short  run_tim_a;
unsigned short  disp_presa;
unsigned short  disp_ssa;
unsigned char   disp_set_flg=0;
unsigned short  dis_cnta;
unsigned char   add91200=0;
unsigned short  add91201=0;
unsigned short  adc_temp00=0;// lwz ����������ĵ�ǰ��ѹ��ֵ
unsigned short  addhpp = 0;
unsigned short  addhpp2 = 0;

#if LANGUAGE_RUSSIA_ENGILISH
const unsigned char rus_model[] = {0x90, 0xA5, 0xA6, 0xC2, 0xAC, 0x00};
const unsigned char rus_switch[] = {0x82, 0xBB, 0xA1, 0xAE, 0xB0, 0x00};
const unsigned char rus_continuous[] = {0x8F, 0xAE, 0xB1, 0xB2, 0xAE, 0xBF, 0xAD,0xAD,0xBB,0xA9,0x00};
const unsigned char rus_intermittent[] = {0x8F, 0xA5, 0xB0, 0xA5, 0xAC, 0xA5, 0xAD, 0xAD, 0xBB, 0xA9, 0x00};

const unsigned char rus_liquid_full[] = {0x85,0xAC,0xAA,0xAE,0xB1, 0xB2,0xBC,0x20,0xA7,0xA0,0xAF,0xAE,0xAB,0xAD,0xA5,0xAD,0xA0,0x00};//Һλ������
const unsigned char rus_liquid_full1[] = {0x85,0xAC,0xAA,0xAE,0xB1, 0xB2,0xBC,0x00};//Һλ������
const unsigned char rus_liquid_full2[] = {0xA7,0xA0,0xAF,0xAE,0xAB,0xAD,0xA5,0xAD,0xA0,0x00};//Һλ������

const unsigned char rus_leak_Alarm[] = {0x93,0xB2,0xA5,0xB7,0xAA,0xA0,0x20,0xA2,0xAE,0xA7,0xA4,0xB3,0xB5,0xA0,0x00};//й©����
const unsigned char rus_leak_Alarm1[] = {0x93,0xB2,0xA5,0xB7,0xAA,0xA0,0x00};//й©����
const unsigned char rus_leak_Alarm2[] = {0xA2,0xAE,0xA7,0xA4,0xB3,0xB5,0xC4,0x00};//й©����

const unsigned char rus_battery_low[] = {0x8D,0xA8,0xA7,0xAA,0xA8,0xA9,0x20,0xA7,0xA0,0xB0,0xBF,0xA4,0x00};//�͵������� 

const unsigned char rus_blockage_Alarm[] = {0x91, 0xA8,0xA3,0xAD,0xA0,0xAB,0x20,0xA7,0xA0,0xB1,0xAE,0xB0,0xA0,0x00};//�������� 
const unsigned char rus_blockage_Alarm1[] = {0x91, 0xA8,0xA3,0xAD,0xA0,0xAB,0x00};//�������� 
const unsigned char rus_blockage_Alarm2[] = {0xA7,0xA0,0xB1,0xAE,0xB0,0xA0,0x00};//�������� 
const unsigned char rus_pump_idle[] = {0x81,0xA5,0xA7,0xA4,0xA5,0xA9,0xB1,0xB2, 0xA2,0xA8,0xA5,0x00};//���б��� 

const unsigned char rus_therapy_on[] = {0x8B, 0xA5 ,0xB7 ,0xA5 ,0xAD ,0xA8 ,0xA5 ,0x20, 0x82 ,0x8A ,0x8B ,0x00};  
const unsigned char rus_therapy_off[] = {0x8B, 0xA5 ,0xB7 ,0xA5 ,0xAD ,0xA8 ,0xA5 ,0x20, 0x82 ,0x9B ,0x8A ,0x8B ,0x00};
const unsigned char rus_psressure[] = {0x84 ,0xA0 ,0xA2 ,0xAB ,0xA5 ,0xAD ,0xA8 ,0xA5 ,0x00};  
const unsigned char rus_time_HP[]  = {0x82 ,0xC1 ,0xA4 ,0xA0 ,0xA2 ,0xAB ,0xC1 ,0xC3, 0x00};
const unsigned char rus_time_LP[] =  {0x8D ,0xC1 ,0xA4 ,0xA0 ,0xA2 ,0xAB ,0xC1 ,0xC3, 0x00};
const unsigned char rus_time[] = {0x82 ,0xB0 ,0xA5 ,0xAC ,0xBF ,0x00};
const unsigned char rus_set_pressure[] = {0x93 ,0xA1 ,0xB2 ,0xA0 ,0xAD ,0xAE ,0xB2 ,0xA8 ,0xB2 ,0xB6 ,0x20 ,0xA4 ,0xA0 ,0xB2 ,0xAB ,0xA5 ,0xAD ,0xA8 ,0xA5 ,0x00};
const unsigned char rus_Settings[] = {0x8D ,0xA0 ,0xB1 ,0xB2 ,0xB0 ,0xAE ,0xA9 ,0xAA ,0xA8 ,0x00};
const unsigned char rus_Therapy[] = {0x8B, 0xA5 ,0xB7 ,0xA5 ,0xAD ,0xA8 ,0xA5 ,0x00};
const unsigned char rus_Therapy2[] = {0xC0,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x8B, 0xA5 ,0xB7 ,0xA5 ,0xAD ,0xA8 ,0xA5 ,0x00};
const unsigned char rus_min[] = {0xAC, 0xA8 ,0xAD ,0x00};
const unsigned char rus_mmgh[] = {0xAC, 0xAC ,0xC1 ,0xB0,0xB2,0xC1,0xB1,0xB2,0xC1};
const unsigned char rus_pre_HP[]  = {0x82 ,0xC1 ,0xA4 ,0xA0 ,0xA2 ,0xAB ,0xC1 ,0xC3, 0x00};
const unsigned char rus_pre_LP[] = {0x8D ,0xC1 ,0xA4 ,0xA0 ,0xA2 ,0xAB ,0xC1 ,0xC3, 0x00};
#endif

/******************************************************************/
// lwz ��ʾLED��ɫ����
void DISP_Led(void)/*used*/
{
	YEL    =  0;
	ClrBlk();
	GRE    =  1;
	if (led_cnt++>=50)
	{
		led_cnt=0;
		GRE    =  1;////~GRE;
	}
}


void DISP_Bat(void)/*used*/
{
	unsigned char   i,j;
	bat_cnt++;
	i=(bat_lev&0x0f);
	j=(bat_lev&0xc0);
	if (j==0x80)
	{/////////////////////////////////�����δ��
		if (audio_flg==LED_BAT_NORMAL)
		{
			GRE=0;
			YEL=1;
		}
		else if (audio_flg==LED_LOW_THAN_3_6V)
		{
			DISP_Led();// lwz ��ʾ��������
		}
		else if (audio_flg==LED_LOW_THAN_3_5V_OR_ERR)
		{
			DISP_Led();// lwz ��ʾ��������
		}

		if (bat_cnt<25)
		{
			DISP_Bat000(6,102);
		}
		else if (bat_cnt<50)
		{
			DISP_Bat001(6,102);
		}
		else if (bat_cnt<75)
		{
			DISP_Bat002(6,102);
		}
		else if (bat_cnt<100)
		{
			DISP_Bat003(6,102);
		}
		else if (bat_cnt<125)
		{
			DISP_Bat004(6,102);
		}
		else
		{
			bat_cnt=0;
		}
	}

	else
	{/////////////////////////////////δ��������
		if (audio_flg==LED_BAT_NORMAL)
		{
			GRE=0;
			YEL=0;
		}
		else if (audio_flg==LED_LOW_THAN_3_6V)
		{
			DISP_Led();
		}
		else if (audio_flg==LED_LOW_THAN_3_5V_OR_ERR)
		{
			DISP_Led();
		}

		switch (i)
		{
		case 10://///////////////////
			if (bat_cnt1++<50)
			{
				//DISP_ClrA4();DISP_ClrB4();
				DISP_Bat00Clr(6,102);
			}
			else if (bat_cnt1++<200)
			{
				DISP_Bat000(6,102);
			}
			else
			{
				bat_cnt1=0;
			}
			//SetLedRed(void)
			break;
		case 1://///////////////////
			DISP_Bat001(6,102);
			break;
		case 2://///////////////////
			DISP_Bat002(6,102);
			break;
		case 3://///////////////////
			DISP_Bat003(6,102);
			break;
		case 4://///////////////////
			// SetLedYel();
			DISP_Bat004(6,102);
			break;
		case 5://///////////////////
			//bat_flg=10;
			break;
		default://///////////////////
			//  DISP_Bat03(6,102);
			break;
		}
	}
}

// lwz ���ݵ�ǰ�Ĵ����룬����Ļ����ʾ��Ӧ�Ĵ�������
void DISP_Erra(void)/*used*/
{
 unsigned char bbb=0;
	if (disp_ssa++<50)
	{
       bbb=1;
		DISP_ClrZero(0,6,0,125);
	}
	else if (disp_ssa++<200)
	{
		if ((mod_main_a==MOD_SYS)||(mod_main_a==MOD_OFF))
		{
			return;
		}
		switch (err_codea)
		{

		case ERR_CANISTER_FULL:////////////////////////Һλ��
			if (language)
			{
#if LANGUAGE_RUSSIA_ENGILISH 
                DISP_8X16ascii((char *)"Canister Full",3,13);
#else
                DISP_yewm(3,85);
#endif                
                
			}
			else
			{
#if LANGUAGE_RUSSIA_ENGILISH
//                DISP_8X16ascii((char *)rus_liquid_full,3,13); 
                  DISP_8X16ascii((char *)rus_liquid_full1,1,46); 
                  DISP_8X16ascii((char *)rus_liquid_full2,3,46); 
#else
                DISP_8X16ascii((char *)"Canister Full",3,13);
#endif 
			}
			break;

		case ERR_AIR_LEAKAGE://©��
			if (language)
			{
#if LANGUAGE_RUSSIA_ENGILISH 
               DISP_8X16ascii((char *)"Leak  Alarm",3,24);
#else
               DISP_louq(3,85);
#endif 
			}
			else
			{
#if LANGUAGE_RUSSIA_ENGILISH 
//                DISP_8X16ascii((char *)rus_leak_Alarm,3,24);	
                DISP_8X16ascii((char *)rus_leak_Alarm1,1,46);
                DISP_8X16ascii((char *)rus_leak_Alarm2,3,46);
#else
                DISP_8X16ascii((char *)"Leak  Alarm",3,24);
#endif
			}

			break;
		case ERR_JAMED://��������
		{
			if (language)
			{
#if LANGUAGE_RUSSIA_ENGILISH    
                DISP_8X16ascii_block((char *)"Blockage  Alarm",3,24);  //20160707  YUKI ASK CHANGED TO BLOCKAGE ALARM
#else
                DISP_block(3,85);
#endif
				
			}
			else
			{
#if LANGUAGE_RUSSIA_ENGILISH 
//                DISP_8X16ascii((char *)rus_blockage_Alarm,3,24);
                DISP_8X16ascii((char *)rus_blockage_Alarm1,1,46);
                DISP_8X16ascii((char *)rus_blockage_Alarm2,3,46);
#else
                DISP_8X16ascii_block((char *)"Blockage  Alarm",3,24);  //20160707  YUKI ASK CHANGED TO BLOCKAGE ALARM
#endif				
			}
		}
		break;
		
		case ERR_DEV_IDLE://���б���  ����5����������
		{
#if LANGUAGE_RUSSIA_ENGILISH    
            
        if (language)
        {
            DISP_8X16ascii((char *)"Pump Idle",3,24);
        }
        else
        {
            DISP_8X16ascii((char *)rus_pump_idle,3,40);
        }
		
#else
        DISP_8X16ascii((char *)"Pump Idle",3,24);
#endif                
		}
		break;

		default:
			if (audio_flg==LED_LOW_THAN_3_6V)
			{
				if (language)
				{
#if LANGUAGE_RUSSIA_ENGILISH 
                    DISP_8X16ascii((char *)"Battery  Low",3,20);
#else
                    DISP_didl(3,85);
#endif                    				
				}
				else
				{
#if LANGUAGE_RUSSIA_ENGILISH  
                    DISP_8X16ascii((char *)rus_battery_low,3,20);
#else
                    DISP_8X16ascii((char *)"Battery  Low",3,20);
#endif 				
				}
			}

			break;
		}
	}
	else
	{
		disp_ssa=0;
		bat_cnt1=0;
	}

}

static unsigned short value_buf[10];
static unsigned char i =0;
static unsigned short delay_count = 0;
static unsigned short disp_filter(unsigned short pdata)/*�¼ҵ�*/
{

	value_buf[i++] = pdata;
	unsigned char count= 0;
	unsigned short sum = 0;
	if ( i == 10 )i = 0;
	for ( count=0;count<10;count++)sum += value_buf[count];
	sum=(unsigned short)(sum/10);
	if (sum>998)sum=999;
	if (sum<1)sum=0;
	return (unsigned short)sum;
}
#define  addx  2
#define DIS_COUNT   5
unsigned char  err_codeabak=0;




void  clear_lqtimes(void)
{
	show_lq_times.lq_times=0;
	show_lq_times.show_times=0;
	show_lq_times.step=0;
	show_lq_times.buz_key_pressed_times=0;
}
	unsigned read_key=0;
	
void show_lq(void)
{
   
	if(show_lq_times.step==0)//wait to check lwz ���BUZ�����µ�״̬
	{
		read_key = PORTB;// lwz ��ȡ�����Ĵ�����ֵ
		read_key=~read_key;// lwz ȡ��
		// read_key=read_key&0b00010000;
		// if(read_key==0b00010000)
		
		read_key=read_key&0b00100000;// lwz �鿴��5λ�Ƿ���λ
		if(read_key==0b00100000)// lwz �����5λ��λ����ʼ��ʱ
		{
			// if(show_lq_times.buz_key_pressed_times++ >499)
			if(show_lq_times.buz_key_pressed_times++ >PRESS_KEY_BUZ_SHOWLQ_TIME)
				show_lq_times.step=1;
		}
		else
			show_lq_times.buz_key_pressed_times=0;
	}
	if(show_lq_times.step==1)//show,wait to exit lwz ��ʾ�����Ĵ���
	{
		read_key = PORTB;
		read_key=~read_key;
		// read_key=read_key&0b00010000;
		// if(read_key==0b00010000)
		read_key=read_key&0b00100000;
		if(read_key==0b00100000)
		{
			// if(show_lq_times.show_times++<250)
				DISP_Dig12_16(0,100,show_lq_times.lq_times);
			// else
			// {
				// DISP_Dig12_16(0,100,0);
				// show_lq_times.step=0;
			// }
		}
		else
		{
			DISP_Clear();
			show_lq_times.step=0;
			show_lq_times.show_times=9;
		}
	}
}
////////////////////////////////////////////////////////////


// lwz ��ʾ��������ģʽ�µĽ���
void DISP_LixA(void)/*used*/
{
	disp_presa = mod_seta_preh;

	if (err_codea==err_codeabak)
	{
		;
	}
	else
	{
		DISP_Clear();
	}
    
    if (audio_flg==LED_LOW_THAN_3_6V)
    {
        DISP_Clear22();
    }

	// 情况1：有错误时显示错误信息
	if (err_codea!=0)
	{
		DISP_Erra();
	}
	// 情况2：没有错误且电池低电量时显示电池低电量信息
	else if (audio_flg==LED_LOW_THAN_3_6V)
	{
        addhpp = 1;
		// 确保清除屏幕，避免重叠
		DISP_Clear();
		if (language)
		{
#if LANGUAGE_RUSSIA_ENGILISH 
            DISP_8X16ascii((char *)"Battery  Low",3,20);
#else
            DISP_didl(3,85);
#endif                    				
		}
		else
		{
#if LANGUAGE_RUSSIA_ENGILISH  
            DISP_8X16ascii((char *)rus_battery_low,3,34);
#else
            DISP_8X16ascii((char *)"Battery  Low",3,20);
#endif 
		}
	}
	// 情况3：没有错误且电池电量正常时显示正常界面
	else
	{
        if(addhpp == 1)
        {
            DISP_Clear22();
            addhpp = 0;
        }

		show_lq();

		DISP_lx(0,17);
		DISP_Fu1(0,31);
		DISP_Dig12_16(0,37,disp_presa);
		
#if LANGUAGE_RUSSIA_ENGILISH 
		if(language)
		{
			DISP_ChaBasic(M,0,55+addx);
			DISP_ChaBasic(M,0,61+addx);
			DISP_ChaBasic(H,0,67+addx);
			DISP_ChaBasic(G,0,73+addx);
		}
		else
		{
			DISP_ChaBasic2015(0xAC,0,60);
			DISP_ChaBasic2015(0xAC,0,67);
			DISP_ChaBasic2015(0xC1,0,74);
			DISP_ChaBasic2015(0xB0,0,81);
			DISP_ChaBasic2015(0xB2,0,88);
			DISP_ChaBasic2015(0xC1,0,95);
			DISP_ChaBasic2015(0xB1,0,102);
			DISP_ChaBasic2015(0xB2,0,109);
			DISP_ChaBasic2015(0xC1,0,116);
		}

#else
		DISP_ChaBasic(M,0,55+addx);
		DISP_ChaBasic(M,0,61+addx);
		DISP_ChaBasic(H,0,67+addx);
		DISP_ChaBasic(G,0,73+addx);
#endif    
		
		


		if (add91200++>UPDATE_UI_PRESSURE_TIME_1)
		{
			add91200=0;
			add91201=adc_temp00;
		}
		if (++delay_count>UPDATE_UI_PRESSURE_TIME_2)// lwz 延迟更新压力值
		{
			delay_count = 0;
			run_tim_a = disp_filter(add91201);
		}

#if (DISP_TRUE_DATA ==0)
		// 非实际采集数据，显示处理后的数据
		if (dis_cnta++>DIS_COUNT/2)
		{
			dis_cnta=0;
			val_val  =  (run_tim_a/DIS_COUNT)*DIS_COUNT;

		}

		if ((val_val>mod_seta_preh+DIS_COUNT)||(val_val>(mod_seta_preh-con_hi_delta)))
		{
			val_val=mod_seta_preh;
		}
#else
		val_val = run_tim_a;			
#endif
		
		
#if LANGUAGE_RUSSIA_ENGILISH 
		if(language)
		{
			DISP_Dig18_40(4,30);
			DISP_Fu(2,12);
			DISP_ChaBasic2('m',4,76);
			DISP_ChaBasic2('m',4,83);
			DISP_ChaBasic2('H',4,90);
			DISP_ChaBasic2('g',4,97);
		}
		else
		{
			DISP_Dig18_40(4,20);
			DISP_Fu(2,6);
			DISP_ChaBasic2015(0xAC,4,66);
			DISP_ChaBasic2015(0xAC,4,73);
			DISP_ChaBasic2015(0xC1,4,80);
			DISP_ChaBasic2015(0xB0,4,87);
			DISP_ChaBasic2015(0xB2,4,94);
			DISP_ChaBasic2015(0xC1,4,101);
			DISP_ChaBasic2015(0xB1,4,108);
			DISP_ChaBasic2015(0xB2,4,115);
			DISP_ChaBasic2015(0xC1,4,122);             
		}
	   
#elif
		DISP_Dig18_40(4,30);
		DISP_Fu(2,12);
		DISP_ChaBasic2('m',4,76);
		DISP_ChaBasic2('m',4,83);
		DISP_ChaBasic2('H',4,90);
		DISP_ChaBasic2('g',4,97);
#endif        



		if (language)
		{
#if LANGUAGE_RUSSIA_ENGILISH 
		   DISP_cha7s((char *)"Therapy  On",6,2); 
#else
		   DISP_run2015(6,45);
#endif            
			
		}
		else
		{
#if LANGUAGE_RUSSIA_ENGILISH 
			DISP_8X16ascii((char*)rus_therapy_on,6,2);
#else
			DISP_cha7s((char *)"Therapy  On",6,2);
#endif            			
		}
	}

	err_codeabak=err_codea;

}





unsigned short bbbbb=0;
// lwz ��ʾ��Ϲ���ģʽ�µĽ���
void DISP_JixA(void)/*used*/
{
	if (mod_jixa==0)
	{
		disp_presa = mod_seta_preh;
	}
	else
	{
		disp_presa = mod_seta_prel;
		// run_tim_a=0;
	}

	if (err_codea==err_codeabak)
	{
		;
	}
	else
	{
		DISP_Clear();
	}
    
    if (audio_flg==LED_LOW_THAN_3_6V)
    {
        DISP_Clear22();
        addhpp2 = 1;
    }
	// 情况1：有错误时显示错误信息
	if (err_codea!=0)
	{
		DISP_Erra();
	}
	// 情况2：没有错误且电池低电量时显示电池低电量信息
	else if (audio_flg==LED_LOW_THAN_3_6V)
	{
		// 确保清除屏幕，避免重叠
		DISP_Clear();
		
		if (language)
		{
#if LANGUAGE_RUSSIA_ENGILISH 
            DISP_8X16ascii((char *)"Battery  Low",3,20);
#else
            DISP_didl(3,85);
#endif                    				
		}
		else
		{
#if LANGUAGE_RUSSIA_ENGILISH  
            DISP_8X16ascii((char *)rus_battery_low,3,34);
#else
            DISP_8X16ascii((char *)"Battery  Low",3,20);
#endif 
		}
	}
	// 情况3：没有错误且电池电量正常时显示正常界面
	else
	{
        if(addhpp2 == 1)
        {
            addhpp2 = 0;
            DISP_Clear22();
        }
		show_lq();
		
		DISP_jx(0,17);
		DISP_Fu1(0,31);
		DISP_Dig12_16(0,37,disp_presa);
		
#if LANGUAGE_RUSSIA_ENGILISH 
		
		if(language)
		{
			DISP_ChaBasic(M,0,55+addx);
			DISP_ChaBasic(M,0,61+addx);
			DISP_ChaBasic(H,0,67+addx);
			DISP_ChaBasic(G,0,73+addx);
			DISP_Dig1(2,80,mod_seta_ont);// lwz 显示间歇模式下的高压时间
			DISP_ChaBasic2('m',2,106);
			DISP_ChaBasic2('i',2,113);
			DISP_ChaBasic2('n',2,120);

			DISP_Dig1(4,80,mod_seta_oft);// lwz 显示间歇模式下的低压时间
			DISP_ChaBasic2('m',4,106);
			DISP_ChaBasic2('i',4,113);
			DISP_ChaBasic2('n',4,120);
		}
		else
		{
			DISP_ChaBasic2015(0xAC,0,60);
			DISP_ChaBasic2015(0xAC,0,67);
			DISP_ChaBasic2015(0xC1,0,74);
			DISP_ChaBasic2015(0xB0,0,81);
			DISP_ChaBasic2015(0xB2,0,88);
			DISP_ChaBasic2015(0xC1,0,95);
			DISP_ChaBasic2015(0xB1,0,102);
			DISP_ChaBasic2015(0xB2,0,109);
			DISP_ChaBasic2015(0xC1,0,116);
			DISP_Dig1(2,80,mod_seta_ont);
			DISP_8X16ascii((char *)rus_min,2,106);
			DISP_Dig1(4,80,mod_seta_oft);
			DISP_8X16ascii((char *)rus_min,4,106);
		}
		

#else
		DISP_ChaBasic(M,0,55+addx);
		DISP_ChaBasic(M,0,61+addx);
		DISP_ChaBasic(H,0,67+addx);
		DISP_ChaBasic(G,0,73+addx);

		DISP_Dig1(2,80,mod_seta_ont);// lwz 显示间歇模式下的高压时间
		DISP_ChaBasic2('m',2,106);
		DISP_ChaBasic2('i',2,113);
		DISP_ChaBasic2('n',2,120);

		DISP_Dig1(4,80,mod_seta_oft);// lwz 显示间歇模式下的低压时间
		DISP_ChaBasic2('m',4,106);
		DISP_ChaBasic2('i',4,113);
		DISP_ChaBasic2('n',4,120);
#endif

		if (add91200++>UPDATE_UI_PRESSURE_TIME_1)
		{
			add91200=0;
			add91201=adc_temp00;
		}
		if (++delay_count>UPDATE_UI_PRESSURE_TIME_2)
		{
			delay_count = 0;
			run_tim_a = disp_filter(add91201);
		}
		if((mod_jixa!=1)||(ddfq!=1))
		{
			if (dis_cnta++>DIS_COUNT/2)
			{
				dis_cnta=0;
				val_val  =  (run_tim_a/DIS_COUNT)*DIS_COUNT;

			}
			if (val_val>=mod_seta_preh+DIS_COUNT)
			{
				val_val=mod_seta_preh;
			}
		}
		else
		{
			if(ddfq==1)
			{
				if(bbbbb==0)
					val_val=adc_ps00;
				 bbbbb++;
				 if(bbbbb>49)
					bbbbb=0;
			}
		}
			
		DISP_Dig18_40(4,30);
		DISP_Fu(2,12);
		if (language)
		{
#if LANGUAGE_RUSSIA_ENGILISH 
            DISP_cha7s((char *)"Therapy  On",6,2);
#else
            DISP_run2015(6,45);
#endif		
		}
		else
		{
#if LANGUAGE_RUSSIA_ENGILISH 
            DISP_8X16ascii((char *)rus_therapy_on,6,2);
#else
            DISP_cha7s((char *)"Therapy  On",6,2);
#endif            
			
		}
	}
	err_codeabak=err_codea;

}
    
    
    
    
unsigned char  flg_disp=0;

unsigned char  flg2015=0;

void DISP_SetA_new(void)/*used*/
{
	if (mod_seta_cnt == UI_WORKMODE_SELECT)
	{
		if (flg2015==0)
		{
			flg2015=1;
			if (language)
			{
#if LANGUAGE_RUSSIA_ENGILISH 
                DISP_key2015(0,70);
				DISP_8X16ascii((char *)"Mode",0,5);
				DISP_8X16ascii((char *)"Switch",0,80);
				DISP_8X16ascii((char *)"Continuous",2,8);
				DISP_8X16ascii((char *)"Intermittent",4,5);
#else
                DISP_mod_set2015();
#endif                
				
			}
			else
			{
#if LANGUAGE_RUSSIA_ENGILISH 
                DISP_key2015(0,70);
                DISP_8X16ascii((char *)rus_model,0,5);   
                DISP_8X16ascii((char *)rus_switch,0,80);   
                DISP_8X16ascii((char *)rus_continuous,2,8);   
                DISP_8X16ascii((char *)rus_intermittent,4,5); 
#else
                DISP_key2015(0,70);
				DISP_8X16ascii((char *)"Mode",0,5);
				DISP_8X16ascii((char *)"Switch",0,80);
				DISP_8X16ascii((char *)"Continuous",2,8);
				DISP_8X16ascii((char *)"Intermittent",4,5);
#endif                 

			}

		}

		if (mod_seta_wok == MOD_LIX)
		{

			DISP_select(2,116);
			DISP_select0(4,116);

		}
		else if (mod_seta_wok == MOD_JIX)
		{

			DISP_select(4,116);
			DISP_select0(2,116);
		}

	}
	else if (mod_seta_cnt==UI_MODE_SET_HI)//1 -----------------------------------���ø�ѹ
	{
		val_val = mod_seta_preh;
		if (mod_seta_wok == MOD_LIX)
		{
            
            if (language)
            {
#if LANGUAGE_RUSSIA_ENGILISH
                DISP_Fu(3,32);
                DISP_Dig15_32(4, 48);
                DISP_8X16ascii((char *)"Pressure",0,36);
#else
                DISP_pset(1,77);
#endif             
            }
            else
            {
#if LANGUAGE_RUSSIA_ENGILISH
                DISP_Fu(3,11);
                val_val = mod_seta_preh;
                if (mod_seta_preh<100)
                {
                    DISP_Dig15_32(4, 26);
                }
                else
                {
                    DISP_Dig15_32(4, 26);
                }
                DISP_8X16ascii((char *)rus_psressure,0,46);
#else
                DISP_8X16ascii((char *)"Pressure",0,36);
#endif           
            }
			
		}
		else//��Ъģʽ�ĸ�ѹ����
		{
            if (language)
            {
#if LANGUAGE_RUSSIA_ENGILISH                
                if(flg2015==0)
                {
                    flg2015=1;
                    DISP_8X16ascii((char *)"Pressure",0,36);
                    DISP_8X16ascii((char *)"HP Set : -",2,6);
                    DISP_8X16ascii((char *)"LP Set : -",4,6);                
                }
#else
				if(flg2015==0)
				{
					flg2015=1;
					DISP_press_set2016();
				}
				DISP_8X16ascii((char *)"-",2,55+6);
				DISP_8X16ascii((char *)"-",4,55+6);            
#endif

            }
            else
            {
                
#if LANGUAGE_RUSSIA_ENGILISH
                DISP_Fu(3,32);
                DISP_8X16ascii((char *)rus_psressure,0,42);
                DISP_8X16ascii((char *)rus_pre_HP,2,3);
				DISP_8X16ascii((char *)rus_pre_LP,4,3);
#else
                DISP_8X16ascii((char *)"Pressure",0,36);
                DISP_8X16ascii((char *)"HP Set : -",2,6);
                DISP_8X16ascii((char *)"LP Set : -",4,6);
#endif                
            }
            
            
#if LANGUAGE_RUSSIA_ENGILISH
            if(language)
            {
                disp_set_flg=1;
                DISP_Dig14_16(2, 65+8,mod_seta_preh);
                disp_set_flg=0;
                DISP_Dig14_16(4, 65+8,mod_seta_prel);
            }
            else
            {
                disp_set_flg=1;
                DISP_Dig14_16(2, 53,mod_seta_preh);
                disp_set_flg=0;
                DISP_Dig14_16(4, 53,mod_seta_prel);
            }
            
#else            
            disp_set_flg=1;
            DISP_Dig14_16(2, 65+8,mod_seta_preh);
            disp_set_flg=0;
            DISP_Dig14_16(4, 65+8,mod_seta_prel);
#endif  
            
            
#if LANGUAGE_RUSSIA_ENGILISH            
            if (language)
            {
                DISP_ChaBasic2('m',2,90+8);
                DISP_ChaBasic2('m',2,97+8);
                DISP_ChaBasic2('H',2,104+8);
                DISP_ChaBasic2('g',2,111+8);
            }
            else
            {
                DISP_ChaBasic2015(0xAC,2,78);
                DISP_ChaBasic2015(0xAC,2,84);
                DISP_ChaBasic2015(0xC1,2,90);
                DISP_ChaBasic2015(0xB0,2,96);
                DISP_ChaBasic2015(0xB2,2,102);
                DISP_ChaBasic2015(0xC1,2,108);
                DISP_ChaBasic2015(0xB1,2,114);
                DISP_ChaBasic2015(0xB2,2,120);
                DISP_ChaBasic2015(0xC1,2,126);
            }
#else
            DISP_ChaBasic2('m',2,90+8);
            DISP_ChaBasic2('m',2,97+8);
            DISP_ChaBasic2('H',2,104+8);
            DISP_ChaBasic2('g',2,111+8);
#endif            

		}
        
		disp_set_flg=0;
#if LANGUAGE_RUSSIA_ENGILISH         
        if(language)
        {
            DISP_ChaBasic2('m',4,90+8);
            DISP_ChaBasic2('m',4,97+8);
            DISP_ChaBasic2('H',4,104+8);
            DISP_ChaBasic2('g',4,111+8);
        }
        else
        {
            DISP_ChaBasic2015(0xAC,4,78);
            DISP_ChaBasic2015(0xAC,4,84);
            DISP_ChaBasic2015(0xC1,4,90);
            DISP_ChaBasic2015(0xB0,4,96);
            DISP_ChaBasic2015(0xB2,4,102);
            DISP_ChaBasic2015(0xC1,4,108);
            DISP_ChaBasic2015(0xB1,4,114);
            DISP_ChaBasic2015(0xB2,4,120);
            DISP_ChaBasic2015(0xC1,4,126);
        }  
#else
        DISP_ChaBasic2('m',4,90+8);
        DISP_ChaBasic2('m',4,97+8);
        DISP_ChaBasic2('H',4,104+8);
        DISP_ChaBasic2('g',4,111+8);
#endif     	
        
	}
	else if (mod_seta_cnt==UI_JIX_MODE_SET_LO)// 2 -----------------------------���õ�ѹ
	{
		flg2015=0;		
		val_val = mod_seta_preh;
		if (mod_seta_wok == MOD_LIX)
		{
#if LANGUAGE_RUSSIA_ENGILISH 
			DISP_Fu(3,32);
			DISP_Dig15_32(4, 48);
            
			if (language)
			{
				DISP_8X16ascii((char *)"Pressure",0,36);

			}
			else
			{
				DISP_8X16ascii((char *)rus_psressure,0,42);
			}            
#else            
			DISP_Fu(3,32);
			DISP_Dig15_32(4, 48);
            
			if (language)
			{
				DISP_pset(1,77);

			}
			else
			{
				DISP_8X16ascii((char *)"Pressure",0,36);
			}
#endif             
		}
		else//��Ъģʽ
		{
#if LANGUAGE_RUSSIA_ENGILISH
			if (language)
			{			
				DISP_8X16ascii((char *)"Pressure",0,36);
				DISP_8X16ascii((char *)"HP Set : -",2,6);
				DISP_8X16ascii((char *)"LP Set : -",4,6);
			}
			else
			{
                DISP_8X16ascii((char *)rus_psressure,0,42);
                DISP_8X16ascii((char *)rus_pre_HP,2,3);
                DISP_8X16ascii((char *)rus_pre_LP,4,3);
			}            
#else            
			if (language)
			{
				
				if((flg_disp++) < 5)
                {
					DISP_press_set2016();
                }
				else
                {
					flg_disp=5;
                }
				

				DISP_8X16ascii((char *)"-",2,55+6);
				DISP_8X16ascii((char *)"-",4,55+6);

			}
			else
			{
				DISP_8X16ascii((char *)"Pressure",0,36);
				DISP_8X16ascii((char *)"HP Set : -",2,6);
				DISP_8X16ascii((char *)"LP Set : -",4,6);
			}
#endif
#if LANGUAGE_RUSSIA_ENGILISH 
            if(language)
            {            
                DISP_Dig14_16(2, 65+8,mod_seta_preh);
                disp_set_flg=1;
                DISP_Dig14_16(4, 65+8,mod_seta_prel);
                disp_set_flg=0;
            }
            else
            {            
                DISP_Dig14_16(2, 53,mod_seta_preh);
                disp_set_flg=1;
                DISP_Dig14_16(4, 53,mod_seta_prel);
                disp_set_flg=0;
            }            

#else            
			DISP_Dig14_16(2, 65+8,mod_seta_preh);
			disp_set_flg=1;
			DISP_Dig14_16(4, 65+8,mod_seta_prel);
			disp_set_flg=0;
#endif            

#if LANGUAGE_RUSSIA_ENGILISH 
        if(language)
        {
            DISP_ChaBasic2('m',2,90+8);
			DISP_ChaBasic2('m',2,97+8);
			DISP_ChaBasic2('H',2,104+8);
			DISP_ChaBasic2('g',2,111+8);
        }
        else
        {
            DISP_ChaBasic2015(0xAC,2,78);
			DISP_ChaBasic2015(0xAC,2,84);
			DISP_ChaBasic2015(0xC1,2,90);
			DISP_ChaBasic2015(0xB0,2,96);
			DISP_ChaBasic2015(0xB2,2,102);
			DISP_ChaBasic2015(0xC1,2,108);
			DISP_ChaBasic2015(0xB1,2,114);
			DISP_ChaBasic2015(0xB2,2,120);
			DISP_ChaBasic2015(0xC1,2,126);
        }
#else
            DISP_ChaBasic2('m',2,90+8);
			DISP_ChaBasic2('m',2,97+8);
			DISP_ChaBasic2('H',2,104+8);
			DISP_ChaBasic2('g',2,111+8);
#endif 
            

		}
#if LANGUAGE_RUSSIA_ENGILISH 
        disp_set_flg=0;
        if(language)
        {
            DISP_ChaBasic2('m',4,90+8);
            DISP_ChaBasic2('m',4,97+8);
            DISP_ChaBasic2('H',4,104+8);
            DISP_ChaBasic2('g',4,111+8);
        }
        else
        {
            DISP_ChaBasic2015(0xAC,4,78);
            DISP_ChaBasic2015(0xAC,4,84);
            DISP_ChaBasic2015(0xC1,4,90);
            DISP_ChaBasic2015(0xB0,4,96);
            DISP_ChaBasic2015(0xB2,4,102);
            DISP_ChaBasic2015(0xC1,4,108);
            DISP_ChaBasic2015(0xB1,4,114);
            DISP_ChaBasic2015(0xB2,4,120);
            DISP_ChaBasic2015(0xC1,4,126);
        }
#else        
		disp_set_flg=0;
        DISP_ChaBasic2('m',4,90+8);
		DISP_ChaBasic2('m',4,97+8);
		DISP_ChaBasic2('H',4,104+8);
		DISP_ChaBasic2('g',4,111+8);
#endif
        
          
	}

	else if (mod_seta_cnt==UI_JIX_SET_HI_TIME)// 3 ���ø�ѹʱ��
	{
#if LANGUAGE_RUSSIA_ENGILISH  
		if (language)
		{
			DISP_8X16ascii((char *)"Intermittent",0,16);
			DISP_8X16ascii((char *)"HP Time :",2,6);
			DISP_8X16ascii((char *)"LP Time :",4,6);
		}
		else
		{
			DISP_8X16ascii((char *)rus_time,0,50);
			DISP_8X16ascii((char *)rus_time_HP,2,20);
			DISP_8X16ascii((char *)rus_time_LP,4,20);

		}        
#else        
		if (language)
		{
			if((flg_disp++) < 5)
				DISP_time_set2015();
			else
				flg_disp=5;
		}
		else
		{
			DISP_8X16ascii((char *)"Intermittent",0,16);
			DISP_8X16ascii((char *)"HP Time :",2,6);
			DISP_8X16ascii((char *)"LP Time :",4,6);

		}
#endif        

		disp_set_flg=1;
		DISP_Dig14_16(2, 75,mod_seta_ont);
		disp_set_flg=0;
        
        if(language)
        {
            DISP_8X16ascii((char *)"min",2,101);
        }
        else
        {
            DISP_8X16ascii((char *)rus_min,2,101);
        }
        
        
		
        
		DISP_Dig14_16(4, 75,mod_seta_oft);
        
        if(language)
        {
            DISP_8X16ascii((char *)"min",4,101);
        }    
        else
        {
            DISP_8X16ascii((char *)rus_min,4,101);
        }
        
	}
	else if (mod_seta_cnt==UI_JIX_SET_LO_TIME)// 4 -----------------------------���õ�ѹʱ��
	{
		flg2015=0;
        
#if LANGUAGE_RUSSIA_ENGILISH 
		if (language)
		{
			DISP_8X16ascii((char *)"Intermittent",0,16);
			DISP_8X16ascii((char *)"HP Time :",2,6);
			DISP_8X16ascii((char *)"LP Time :",4,6);
		}
		else
		{
			DISP_8X16ascii((char *)rus_time,0,50);
			DISP_8X16ascii((char *)rus_time_HP,2,20);
			DISP_8X16ascii((char *)rus_time_LP,4,20);	

		}

		disp_set_flg=0;
		DISP_Dig14_16(2, 75,mod_seta_ont);
		disp_set_flg=0;
        
        if(language){
			DISP_8X16ascii((char *)"min",2,101);
		}else{
			DISP_8X16ascii((char *)rus_min,2,101);
		}
        

		
        disp_set_flg=1;
		DISP_Dig14_16(4, 75,mod_seta_oft);
		disp_set_flg=0;
        
        
		if(language){
			DISP_8X16ascii((char *)"min",4,101);
		}else{
			DISP_8X16ascii((char *)rus_min,4,101);
		}     
#elif
		if (language)
		{
			if((flg_disp++) < 5)
				DISP_time_set2015();
			else
				flg_disp=5;
		}
		else
		{
			DISP_8X16ascii((char *)"Intermittent",0,16);
			DISP_8X16ascii((char *)"HP Time :",2,6);
			DISP_8X16ascii((char *)"LP Time :",4,6);

		}

		disp_set_flg=0;
		DISP_Dig14_16(2, 75,mod_seta_ont);
		disp_set_flg=0;
		DISP_8X16ascii((char *)"min",2,101);// lwz ����Ǽ�Ъģʽ�µķ���
		disp_set_flg=1;
		DISP_Dig14_16(4, 75,mod_seta_oft);
		disp_set_flg=0;
		DISP_8X16ascii((char *)"min",4,101);// lwz ����Ǽ�Ъģʽ�µķ���        
#endif        
	}


	else if (mod_seta_cnt==UI_SET_PRESSURE)
	{
		DISP_Fu(2,32);
        
#if LANGUAGE_RUSSIA_ENGILISH 
        if (language)
		{
			DISP_ChaBasic2('S',0,24);
			DISP_ChaBasic2('e',0,31);
			DISP_ChaBasic2('t',0,38);
			DISP_ChaBasic2('P',0,48);
			DISP_ChaBasic2('r',0,55);
			DISP_ChaBasic2('e',0,62);
			DISP_ChaBasic2('s',0,69);
			DISP_ChaBasic2('s',0,76);
			DISP_ChaBasic2('u',0,83);
			DISP_ChaBasic2('r',0,90);
			DISP_ChaBasic2('e',0,97);

		}
		else
		{
			DISP_ChaBasic2(0x93,0,24);
			DISP_ChaBasic2(0xB1,0,31);
			DISP_ChaBasic2(0xB2,0,38);
			DISP_ChaBasic2(0xA0,0,48);
			DISP_ChaBasic2(0xAD,0,55);
			DISP_ChaBasic2(0xAE,0,62);
			DISP_ChaBasic2(0xB2,0,69);
			DISP_ChaBasic2(0xA8,0,76);
			DISP_ChaBasic2(0xB2,0,83);
			DISP_ChaBasic2(0xB6,0,90);
			DISP_ChaBasic2(0xA4,0,97);

		}        
#elif
        if (language)
		{
			DISP_pset(0,77);

		}
		else
		{
			DISP_ChaBasic2('S',0,24);
			DISP_ChaBasic2('e',0,31);
			DISP_ChaBasic2('t',0,38);
			DISP_ChaBasic2('P',0,48);
			DISP_ChaBasic2('r',0,55);
			DISP_ChaBasic2('e',0,62);
			DISP_ChaBasic2('s',0,69);
			DISP_ChaBasic2('s',0,76);
			DISP_ChaBasic2('u',0,83);
			DISP_ChaBasic2('r',0,90);
			DISP_ChaBasic2('e',0,97);

		}
#endif        

		val_val = set_moda;
		if (mod_seta_preh<100)
		{
			DISP_Dig15_32(4, 48);
		}
		else
		{
			DISP_Dig15_32(4, 46);
		}
        
#if LANGUAGE_RUSSIA_ENGILISH 
        if(language)
        {
            DISP_ChaBasic2('m',4,90);
            DISP_ChaBasic2('m',4,97);
            DISP_ChaBasic2('H',4,104);
            DISP_ChaBasic2('g',4,111);
        }
        else
        {
            DISP_ChaBasic2015(0xAC,4,78);
            DISP_ChaBasic2015(0xAC,4,84);
            DISP_ChaBasic2015(0xC1,4,90);
            DISP_ChaBasic2015(0xB0,4,96);
            DISP_ChaBasic2015(0xB2,4,102);
            DISP_ChaBasic2015(0xC1,4,108);
            DISP_ChaBasic2015(0xB1,4,114);
            DISP_ChaBasic2015(0xB2,4,120);
            DISP_ChaBasic2015(0xC1,4,126);
        }

#else
        DISP_ChaBasic2('m',4,90);
		DISP_ChaBasic2('m',4,97);
		DISP_ChaBasic2('H',4,104);
		DISP_ChaBasic2('g',4,111);
#endif
		disp_set_flg=0;

	}
}




// lwz �����Ƿ���ʾ����־�;�����־
void DISP_Info(void)/*used*/
{

	if (mute_flg)
	{
		DISP_ClrZero(6,8,0,50);
		DISP_Buz(6,25);
	}
	else
	{
		//DISP_ClrZero(6,7,0,50);DISP_BuzClr(6,25);


	}

	if (lock_flg)// lwz ��ʾ��
	{
		DISP_Lock(6,115);
	}
	else
	{
		DISP_LockClr(6,115);
	}
}

extern  float valueK;
extern  void DISP_yn(unsigned char startx,unsigned char starty);
extern  void DISP_yn0(unsigned char startx,unsigned char starty);

unsigned char  all_flg=0;
// lwz ����������ʾ�߼�
void  DISP_MainA(void)/*used*/
{
	float i;
	if (adc_ps0>adc_zero)
	{
		i=(float)(adc_ps0-adc_zero)/valueK;
	}
	else
	{
		i=0;
	}

	adc_temp00=(unsigned short)i;
#if (DISP_TRUE_DATA ==0)

	if (tim5_flg==0)// lwz ����Բɼ��õ�����ֵ������һ���̶ȵ�����
	{
		if (adc_temp00>=(mod_seta_preh-DIS_COUNT))// lwz ����ɼ��õ�����ֵ�ȡ��趨ֵ��5������adc_temp00��ֵΪ�趨ֵ
		{
			adc_temp00   =   mod_seta_preh;
		}
		else
		{
			adc_temp00   =   (adc_temp00/DIS_COUNT)*(DIS_COUNT+1);// lwz �ɼ��õ�����ֵ�ϸ�20%
		}
	}
	if (adc_temp00<DIS_COUNT)// lwz ����ɼ�ֵ��С����ֱ������Ϊ0
	{
		adc_temp00=0;
	}
	

#endif

	switch (mod_main_a)
	{
	case MOD_SYS:
	{
		break;
	}
	case MOD_WAT:// lwz ��ʾ�ȴ�ָ��״̬�µĽ����߼�
	{
		if (all_flg==0)
		{
			all_flg=1;
			if (language)
			{
#if LANGUAGE_RUSSIA_ENGILISH 
                DISP_key2015(2,20);
				DISP_8X16ascii((char *)"Settings",2,50);
				DISP_8X16ascii((char *)" |       Therapy",4,10);    
#else
                DISP_ask();
#endif
				
			}
			else
			{
#if LANGUAGE_RUSSIA_ENGILISH 
                DISP_key2015(2,20);
				DISP_8X16ascii((char *)rus_Settings,2,50);		
				DISP_8X16ascii((char *)rus_Therapy2,4,14);
#else
                DISP_key2015(2,20);
				DISP_8X16ascii((char *)"Settings",2,50);
				DISP_8X16ascii((char *)" |       Therapy",4,10);
#endif

			}
		}
        
#if LANGUAGE_RUSSIA_ENGILISH 
		if (mod_main_baka==MOD_LIX)// lwz �������ģʽ������ģʽ
		{
			if(language){
				DISP_lx(0,17);
				DISP_Fu1(0,28);
				DISP_Dig12_16(0,34,mod_seta_preh);
				DISP_ChaBasic(M,0,52);
				DISP_ChaBasic(M,0,58);
				DISP_ChaBasic(H,0,64);
				DISP_ChaBasic(G,0,70);
			}else{           
				DISP_lx(0,17);
				DISP_Fu1(0,31);
				DISP_Dig12_16(0,37,mod_seta_preh);     
                DISP_ChaBasic2015(0xAC,0,60);
				DISP_ChaBasic2015(0xAC,0,66);
				DISP_ChaBasic2015(0xC1,0,72);
				DISP_ChaBasic2015(0xB0,0,78);
				DISP_ChaBasic2015(0xB2,0,84);
				DISP_ChaBasic2015(0xC1,0,90);
				DISP_ChaBasic2015(0xB1,0,96);
				DISP_ChaBasic2015(0xB2,0,102);
				DISP_ChaBasic2015(0xC1,0,108);
			}

		}
		else// lwz �������ģʽ�Ǽ��ģʽ
		{
			if(language){
                DISP_jx(0,17);
                DISP_Fu1(0,28);
                DISP_Dig12_16(0,33,mod_seta_preh);
				DISP_ChaBasic(M,0,52);
				DISP_ChaBasic(M,0,58);
				DISP_ChaBasic(H,0,64);
				DISP_ChaBasic(G,0,70);
			}else{
				DISP_jx(0,17);
				DISP_Fu1(0,31);
				DISP_Dig12_16(0,37,mod_seta_preh);     
                DISP_ChaBasic2015(0xAC,0,60);
				DISP_ChaBasic2015(0xAC,0,66);
				DISP_ChaBasic2015(0xC1,0,72);
				DISP_ChaBasic2015(0xB0,0,78);
				DISP_ChaBasic2015(0xB2,0,84);
				DISP_ChaBasic2015(0xC1,0,90);
				DISP_ChaBasic2015(0xB1,0,96);
				DISP_ChaBasic2015(0xB2,0,102);
				DISP_ChaBasic2015(0xC1,0,108);
			}

		}
#else        
		if (mod_main_baka==MOD_LIX)// lwz �������ģʽ������ģʽ
		{
			DISP_lx(0,17);
			DISP_Fu1(0,28);
			DISP_Dig12_16(0,34,mod_seta_preh);
			
			DISP_ChaBasic(M,0,55+addx);
			DISP_ChaBasic(M,0,61+addx);
			DISP_ChaBasic(H,0,67+addx);
			DISP_ChaBasic(G,0,73+addx);

		}
		else// lwz �������ģʽ�Ǽ��ģʽ
		{
			DISP_jx(0,17);
			DISP_Fu1(0,28);
			DISP_Dig12_16(0,33,mod_seta_preh);

			DISP_ChaBasic(M,0,55+addx);
			DISP_ChaBasic(M,0,61+addx);
			DISP_ChaBasic(H,0,67+addx);
			DISP_ChaBasic(G,0,73+addx);

		}
#endif
        
        

		if (lock_flg)// lwz ��ʾ��
		{
			DISP_Lock(6,108);
		}
		else
		{
			DISP_LockClr(6,108);
		}
		break;
	}

	case MOD_LIX:
	{

		DISP_LixA();
		DISP_Info();

		break;
	}
	case MOD_JIX:
	{

		DISP_JixA();
		DISP_Info();

		break;
	}

	case MOD_ZHT:
	{
		disp_presa = mod_seta_preh;// lwz 暂停阶段，显示当前设定的压力值，而不是实时压力值
		
#if LANGUAGE_RUSSIA_ENGILISH 
		if (mod_main_baka==MOD_LIX)// lwz 如果前一个工作模式是连续模式，则显示相应的界面
		{
			DISP_lx(0,17);
			DISP_Fu1(0,31);
			DISP_Dig12_16(0,37,disp_presa);

			//hpp
			if(language){
				DISP_ChaBasic(M,0,55);
				DISP_ChaBasic(M,0,61);
				DISP_ChaBasic(H,0,67);
				DISP_ChaBasic(G,0,73);
			}else{
				DISP_ChaBasic2015(0xAC,0,60);
				DISP_ChaBasic2015(0xAC,0,67);
				DISP_ChaBasic2015(0xC1,0,74);
				DISP_ChaBasic2015(0xB0,0,81);
				DISP_ChaBasic2015(0xB2,0,88);
				DISP_ChaBasic2015(0xC1,0,95);
				DISP_ChaBasic2015(0xB1,0,102);
				DISP_ChaBasic2015(0xB2,0,109);
				DISP_ChaBasic2015(0xC1,0,116);
			}
		}
		else if (mod_main_baka==MOD_JIX)// lwz 如果前一个工作模式是间歇工作模式，则显示相应的界面
		{
			DISP_jx(0,17);
			DISP_Fu1(0,31);
			DISP_Dig12_16(0,37,disp_presa);

			//hpp
			if(language){
				DISP_ChaBasic(M,0,55);
				DISP_ChaBasic(M,0,61);
				DISP_ChaBasic(H,0,67);
				DISP_ChaBasic(G,0,73);
			}else{
				DISP_ChaBasic2015(0xAC,0,60);
				DISP_ChaBasic2015(0xAC,0,67);
				DISP_ChaBasic2015(0xC1,0,74);
				DISP_ChaBasic2015(0xB0,0,81);
				DISP_ChaBasic2015(0xB2,0,88);
				DISP_ChaBasic2015(0xC1,0,95);
				DISP_ChaBasic2015(0xB1,0,102);
				DISP_ChaBasic2015(0xB2,0,109);
				DISP_ChaBasic2015(0xC1,0,116);
			}	
		}        
#else        
		if (mod_main_baka==MOD_LIX)// lwz 如果前一个工作模式是连续模式，则显示相应的界面
		{
			DISP_lx(0,17);
			DISP_Fu1(0,31);// lwz 显示单位
			DISP_Dig12_16(0,37,disp_presa);// lwz 显示压力值
			DISP_ChaBasic(M,0,55+addx);
			DISP_ChaBasic(M,0,61+addx);
			DISP_ChaBasic(H,0,67+addx);
			DISP_ChaBasic(G,0,73+addx);
		}
		else if (mod_main_baka==MOD_JIX)// lwz 如果前一个工作模式是间歇工作模式，则显示相应的界面
		{
			DISP_jx(0,17);
			DISP_Fu1(0,31);
			DISP_Dig12_16(0,37,disp_presa);
			DISP_ChaBasic(M,0,55+addx);
			DISP_ChaBasic(M,0,61+addx);
			DISP_ChaBasic(H,0,67+addx);
			DISP_ChaBasic(G,0,73+addx);
		}
#endif

		DISP_Info();

		if (mute_flg==0)
		{
			if (language)
			{
#if LANGUAGE_RUSSIA_ENGILISH 
                DISP_8X16ascii((char *)"Therapy  Off",3,20);
				DISP_ChaBasic2015('|',6,4);
				DISP_cha7s((char *)" Therapy",6,16);
#else
                DISP_stop2015(3,92);
				DISP_press2015(6,44);
#endif                

			}
			else
			{
#if LANGUAGE_RUSSIA_ENGILISH 
                DISP_8X16ascii((char *)rus_therapy_off,3,20);
				DISP_ChaBasic2015('|',6,4);
				DISP_8X16ascii((char *)rus_Therapy,6,16);
#else
                DISP_8X16ascii((char *)"Therapy  Off",3,20);
				DISP_ChaBasic2015('|',6,4);
				DISP_cha7s((char *)" Therapy",6,16);
#endif 

			}
		}
		else
		{
			;
		}
		break;
	}
	case MOD_SET:
	{
		DISP_SetA_new();// lwz ��ʾ���ý���
		break;;
	}
	case MOD_ERR:
	{
		;
		break;
	}

	case MOD_OFF:
	{
		if ((bat_lev&0x0f)==LOWER_THAN_3_5V)
		{
			if (language)
			{
				DISP_BatWarnE();//hpp
			}
			else
			{
				DISP_BatWarnC();//����
			}
		}
		else
		{
			DISP_Clear() ;
			BIOS_JLX12864_TRANS_CMD(JLX12864G_OFF);
		}
		break;
	}
	case MOD_TK:
	{
		DISP_Erra();
		err_codeabak=err_codea;
		break;
	}
	
	
	default:
	{
		break;
	}
	}
}






