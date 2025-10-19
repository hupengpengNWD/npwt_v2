#include  "include.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_sys_ini_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_dis_main.h"
#include   "npwt_dis_ifile_key_00.h"
#include   "npwt_con_over.h"
unsigned char FLG_SYS_10MS;	
unsigned char  os_cnt=0;
unsigned char  pwm_cnt1=0;
unsigned char  pwm_cnt2=0;
// hpp: 当PWM频率提高到10kHz时，需要改为 float pwm_cnt2=0.0;

volatile unsigned long   Timeone_sum = 0 ;  
unsigned char TIMER_FOUR=0;
void SYS_OSC_Ini(void)/*used*/
{
	OSCCON  =  0b01110000  ;//////////INTOSCPLL(8M*4PLL=32M)
        OSCTUNE =  OSCTUNE|0x40;
 	while( !(OSCCON&0x08) )  ;         //////////WAIT FLAGER
}

void SYS_IO_Ini(void)/*used*/
{
	TRISA  =  0b00110111  ;
	LATA  =  0  ;
	PORTA  =  0  ;
	TRISB  =  0b11111100  ;
	PORTB  =  0  ;
	LATB  =  0  ;
	TRISC  =  0b00000011  ;
	PORTC  =  0  ;
	LATC  =  0  ;
        //  ODCON2 =  0x02;
	LATD  =  0  ;
	TRISD  =  0  ;
	PORTD  =  0  ;
	LATE  =  0  ;
	TRISE  =  0  ;
	PORTE  =  0  ;
	ANCON0  =  0xff;
	ANCON1  =  0xff;
}
void SYS_TMR0_Ini(void)/*used*/
{
	INTCON = 0x20;                //disable global and enable TMR0 interrupt
	INTCON2 = 0x80;               //TMR0 high priority
	TMR0H = 0xd8;                 //clear timer
	TMR0L = 0xef;                 //clear timer
	T0CON = 0x83;                 //set up timer0 - prescaler 1:16
}

void SYS_TMR3_Ini(void)/*used*/
{
	T3CON    =  0x34;////////////pre 1:2   behind 1:4 ////0x01: 
	// hpp: 要修改PWM频率到10kHz，将T3CON改为0x30 (pre 1:1 behind 1:1)
	PIE2  =  PIE2|0x02;//////////enable tmr3 int

}

void SYS_ADCini(void)/*used*/
{
	ADCON0  =  0x01;
	ADCON1  =  0xbe;///0xa6;0xbe
    ANCON0  =  0x01;   
}









/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** �������� ��TimeGet
** ����˵�� ������ָ����ʱ�����ʱ��
** ���˵�� ����
** ����ӿ� �� 
** ����ӿ� ����ֵ
** ˵��       ����
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------
*/

unsigned long T1_TimeGet()
{
	return  Timeone_sum ;
}

/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** �������� ��TimeOutSet
** ����˵�� ������ָ����ʱ�����ʱ��
** ���˵�� ����
** ����ӿ� ��timer--ָ���Ķ�ʱ��������timeout--���ʱ��
** ����ӿ� ����
** ˵��       ����
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------
*/

void TimeOutSet(TIMER	*timer,	unsigned long 	timeout)
{
	timer->RecTickVal = T1_TimeGet();
	timer->TimeOutVal = T1_TimeGet() + timeout;
	timer->IsTimeOut = FALSE;
}	

/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** �������� ��TimeOutSet
** ����˵�� ���ж϶�ʱ���Ƿ����
** ���˵�� ����
** ����ӿ� ��timer--ָ���Ķ�ʱ������
** ����ӿ� ��IsTimeOut --�Ƿ���� 1==��� 0==�����
** ˵��       ����
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------
*/

unsigned char IsTimeOut(	TIMER 	*timer)
{
	if(FALSE==timer->IsTimeOut)
	{
		if((long)(T1_TimeGet() - timer->TimeOutVal) >= 0)
		{
			timer->IsTimeOut = TRUE;
		}
	}		

	return timer->IsTimeOut;
}



unsigned short   adc_ps00;
unsigned char tttt=0;
unsigned int  psps[30];
unsigned short  psps_turn=0;


unsigned char run_arrived=0;
 void __interrupt() Isr(void)
 { 
    if (PIR2&0x02)
    {   //////////////////1ms    
		PIR2 = PIR2&(~0x02);             
		TMR3H =0xfc ;////////////////////////0xff;        
		TMR3L =0x17; ////////////////////////0x30;
		// hpp: 要修改PWM频率到10kHz，将TMR3H改为0xff，TMR3L改为0x9c (0.1ms中断)
		ADC_Ps90();
		if(os_cnt++==4)
		{
			os_cnt=0;
			// adc_ps0 = adc_buf[(ADC_CNT/2)];
			// adc_bat = adc_buf[(ADC_CNT/2+2*ADC_CNT)];
			PRESS_ConA();
		}
		TIMER_FOUR++;
		if(TIMER_FOUR>4)
		{
			if( Timeone_sum<= 0xfffffffe)
				Timeone_sum++;
			else
				Timeone_sum = 0;
			TIMER_FOUR=0;
		}
		if(open_bum)//
		{
			pwm_cnt1++;
            oppump_flg =1;
			// hpp: 修改为100%占空比输出，无论压力值如何都全功率输出
			if(pwm_cnt1<=PWM_PERO)  // 整个PWM周期都输出高电平，实现100%占空比
			{
				if(mod_jixa!=1)// lwz ڵѹ״̬򿪱
				{
					// if (adc_ps00>=(mod_seta_preh+con_hi_delta+5))//20160506 ma add
					// {
						// run_arrived=1;
						// PUMP=0;
					// }
					// if ((adc_ps00<(mod_seta_preh-con_lo_delta))||(run_arrived==0))//20160506 ma add
					// {
						// run_arrived=0;
						PUMP=1;
					//}	
				}	
				else
				{
					if((mod_jixa==1)&&(ddfq==0))// lwz ڵѹ״̬ûз򿪱
					{
						// if (adc_ps00>=(mod_seta_preh+con_hi_delta+5))//20160506 ma add
						// {
							// run_arrived=1;
							// PUMP=0;
						// }
						// if ((adc_ps00<(mod_seta_preh-con_lo_delta))||(run_arrived==0))//20160506 ma add
						// {
							// run_arrived=0;
							PUMP=1;
						// }	
					}	
				}
			}
			else 
				pwm_cnt1=0;  // 重置计数器，开始下一个PWM周期
		}
		else
		{

			PUMP=0;
			pwm_cnt1=0;
		}
	}
	else if(INTCONbits.TMR0IF)
	{                                    //check for TMR0 overflow
		INTCONbits.TMR0IF = 0;           //clear interrupt flag
		FLG_SYS_10MS = 1;      // lwz �����û�п�����������ѭ����ִ�к���
		// 20ms       
		TMR0H = 0xd8;                    //clear timer
		TMR0L = 0xef;                    //clear timer,.
	}
}


//----------------------------------------------------------------------------

/*
hpp: PWM频率修改到10kHz的完整修改方案：

1. TMR3配置修改 (第54行):
   - 将 T3CON = 0x34 改为 T3CON = 0x30
   - 预分频器从1:2改为1:1，后分频器从1:4改为1:1

2. TMR3重载值修改 (第152-153行):
   - 将 TMR3H = 0xfc 改为 TMR3H = 0xff
   - 将 TMR3L = 0x17 改为 TMR3L = 0x9c
   - 中断频率从1ms提高到0.1ms (10kHz)

3. PWM周期修改 (include.h第161行):
   - 将 PWM_PERO = 10 改为 PWM_PERO = 1
   - 1个中断周期为一个PWM周期

4. 占空比控制修改:
   - pwm_cnt2变量类型需要从unsigned char改为float
   - 原来pwm_cnt2=4表示40%占空比，现在需要pwm_cnt2=0.4

5. 频率计算验证:
   - 修改后PWM频率 = 10kHz / 1 = 10kHz

- 提高PWM频率会增加CPU负载
- 需要确保泵和驱动电路支持10kHz工作频率
- 占空比控制逻辑需要相应调整

hpp: PWM占空比修改方案 (第178行):
- 已将 pwm_cnt1<=pwm_cnt2 改为 pwm_cnt1<=PWM_PERO
- 实现100%占空比输出，无论压力值如何都全功率输出
- 移除了低占空比输出逻辑，简化了控制流程
*/
