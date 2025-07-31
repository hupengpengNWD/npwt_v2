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
unsigned char  pwm_cnt2=0;// lwz 开泵时，发送开泵命令的次数。根据设定压力，开泵命令次数会有不同

volatile unsigned long   Timeone_sum = 0 ;  //全局计数
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
** 函数名称 ：TimeGet
** 功能说明 ：返回指定定时器溢出时间
** 设计说明 ：无
** 输入接口 ： 
** 输出接口 ：数值
** 说明       ：无
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
** 函数名称 ：TimeOutSet
** 功能说明 ：设置指定定时器溢出时间
** 设计说明 ：无
** 输入接口 ：timer--指定的定时器变量，timeout--溢出时间
** 输出接口 ：无
** 说明       ：无
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
** 函数名称 ：TimeOutSet
** 功能说明 ：判断定时器是否溢出
** 设计说明 ：无
** 输入接口 ：timer--指定的定时器变量
** 输出接口 ：IsTimeOut --是否溢出 1==溢出 0==非溢出
** 说明       ：无
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
		PIR2 = PIR2&(~0x02);             //clear interrupt flag
		TMR3H =0xfc ;////////////////////////0xff;        //clear timer
		TMR3L =0x17; ////////////////////////0x30;
		ADC_Ps90();// 压力采样
		if(os_cnt++==4)
		{
			os_cnt=0;
			// adc_ps0 = adc_buf[(ADC_CNT/2)];
			// adc_bat = adc_buf[(ADC_CNT/2+2*ADC_CNT)];
			PRESS_ConA();// lwz 获取当前压力，并根据压力的值做出不同的处理
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
		if(open_bum)// 开泵
		{
			pwm_cnt1++;
            oppump_flg =1;
			if(pwm_cnt1<=pwm_cnt2)// 多开几次泵
			{
				if(mod_jixa!=1)// lwz 如果不在低压状态，则开泵
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
					if((mod_jixa==1)&&(ddfq==0))// lwz 如果在低压状态，且没有放气，则开泵
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
			else if(pwm_cnt1<PWM_PERO)
				PUMP=0;
			else 
				pwm_cnt1=0;
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
		FLG_SYS_10MS = 1;      // lwz 如果泵没有开启，进入主循环的执行函数
		// 20ms       
		TMR0H = 0xd8;                    //clear timer
		TMR0L = 0xef;                    //clear timer,.
	}
}


//----------------------------------------------------------------------------
