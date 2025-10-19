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

volatile unsigned long   Timeone_sum = 0 ;  
unsigned char TIMER_FOUR=0;
void SYS_OSC_Ini(void)
{
	OSCCON  =  0b01110000  ;
		OSCTUNE =  OSCTUNE|0x40;
 	while( !(OSCCON&0x08) )  ;         
}

void SYS_IO_Ini(void)
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
		
	LATD  =  0  ;
	TRISD  =  0  ;
	PORTD  =  0  ;
	LATE  =  0  ;
	TRISE  =  0  ;
	PORTE  =  0  ;
	ANCON0  =  0xff;
	ANCON1  =  0xff;
}
void SYS_TMR0_Ini(void)
{
	INTCON = 0x20;                
	INTCON2 = 0x80;               
	TMR0H = 0xd8;                 
	TMR0L = 0xef;                 
	T0CON = 0x83;                 
}

void SYS_TMR3_Ini(void)
{
	T3CON    =  0x34;
	
	PIE2  =  PIE2|0x02;

}

void SYS_ADCini(void)
{
	ADCON0  =  0x01;
	ADCON1  =  0xbe;
	ANCON0  =  0x01;   
}

unsigned long T1_TimeGet()
{
	return  Timeone_sum ;
}

void TimeOutSet(TIMER	*timer,	unsigned long 	timeout)
{
	timer->RecTickVal = T1_TimeGet();
	timer->TimeOutVal = T1_TimeGet() + timeout;
	timer->IsTimeOut = FALSE;
}	

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
	{   
		PIR2 = PIR2&(~0x02);             
		TMR3H =0xfc ;
		TMR3L =0x17; 
		
		ADC_Ps90();
		if(os_cnt++==4)
		{
			os_cnt=0;

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
		if(open_bum)
		{
			pwm_cnt1++;
			oppump_flg =1;
			
			if(pwm_cnt1<=PWM_PERO)  
			{
				if(mod_jixa!=1)
				{

						PUMP=1;
					
				}	
				else
				{
					if((mod_jixa==1)&&(ddfq==0))
					{

							PUMP=1;
						
					}	
				}
			}
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
	{                                    
		INTCONbits.TMR0IF = 0;           
		FLG_SYS_10MS = 1;      
		
		TMR0H = 0xd8;                    
		TMR0L = 0xef;                    
	}
}

