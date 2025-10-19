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
// hpp: å½PWMé¢çæé«å°10kHzæ¶ï¼éè¦æ¹ä¸º float pwm_cnt2=0.0;

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
	// hpp: è¦ä¿®æ¹PWMé¢çå°10kHzï¼å°T3CONæ¹ä¸º0x30 (pre 1:1 behind 1:1)
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
** ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½TimeGet
** ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö¸ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½
** ï¿½ï¿½ï¿½Ëµï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
** ï¿½ï¿½ï¿½ï¿½Ó¿ï¿½ ï¿½ï¿½ 
** ï¿½ï¿½ï¿½ï¿½Ó¿ï¿½ ï¿½ï¿½ï¿½ï¿½Öµ
** Ëµï¿½ï¿½       ï¿½ï¿½ï¿½ï¿½
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
** ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½TimeOutSet
** ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö¸ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½
** ï¿½ï¿½ï¿½Ëµï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
** ï¿½ï¿½ï¿½ï¿½Ó¿ï¿½ ï¿½ï¿½timer--Ö¸ï¿½ï¿½ï¿½Ä¶ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½timeout--ï¿½ï¿½ï¿½Ê±ï¿½ï¿½
** ï¿½ï¿½ï¿½ï¿½Ó¿ï¿½ ï¿½ï¿½ï¿½ï¿½
** Ëµï¿½ï¿½       ï¿½ï¿½ï¿½ï¿½
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
** ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½TimeOutSet
** ï¿½ï¿½ï¿½ï¿½Ëµï¿½ï¿½ ï¿½ï¿½ï¿½Ð¶Ï¶ï¿½Ê±ï¿½ï¿½ï¿½Ç·ï¿½ï¿½ï¿½ï¿½
** ï¿½ï¿½ï¿½Ëµï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
** ï¿½ï¿½ï¿½ï¿½Ó¿ï¿½ ï¿½ï¿½timer--Ö¸ï¿½ï¿½ï¿½Ä¶ï¿½Ê±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
** ï¿½ï¿½ï¿½ï¿½Ó¿ï¿½ ï¿½ï¿½IsTimeOut --ï¿½Ç·ï¿½ï¿½ï¿½ï¿½ 1==ï¿½ï¿½ï¿½ 0==ï¿½ï¿½ï¿½ï¿½ï¿½
** Ëµï¿½ï¿½       ï¿½ï¿½ï¿½ï¿½
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
		// hpp: è¦ä¿®æ¹PWMé¢çå°10kHzï¼å°TMR3Hæ¹ä¸º0xffï¼TMR3Læ¹ä¸º0x9c (0.1msä¸­æ­)
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
			// hpp: ä¿®æ¹ä¸º100%å ç©ºæ¯è¾åºï¼æ è®ºååå¼å¦ä½é½å¨åçè¾åº
			if(pwm_cnt1<=PWM_PERO)  // æ´ä¸ªPWMå¨æé½è¾åºé«çµå¹³ï¼å®ç°100%å ç©ºæ¯
			{
				if(mod_jixa!=1)// lwz ÚµÑ¹×´Ì¬ò¿ª±
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
					if((mod_jixa==1)&&(ddfq==0))// lwz ÚµÑ¹×´Ì¬Ã»Ð·ò¿ª±
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
				pwm_cnt1=0;  // éç½®è®¡æ°å¨ï¼å¼å§ä¸ä¸ä¸ªPWMå¨æ
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
		FLG_SYS_10MS = 1;      // lwz ï¿½ï¿½ï¿½ï¿½ï¿½Ã»ï¿½Ð¿ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñ­ï¿½ï¿½ï¿½ï¿½Ö´ï¿½Ðºï¿½ï¿½ï¿½
		// 20ms       
		TMR0H = 0xd8;                    //clear timer
		TMR0L = 0xef;                    //clear timer,.
	}
}


//----------------------------------------------------------------------------

/*
hpp: PWMé¢çä¿®æ¹å°10kHzçå®æ´ä¿®æ¹æ¹æ¡ï¼

1. TMR3éç½®ä¿®æ¹ (ç¬¬54è¡):
   - å° T3CON = 0x34 æ¹ä¸º T3CON = 0x30
   - é¢åé¢å¨ä»1:2æ¹ä¸º1:1ï¼ååé¢å¨ä»1:4æ¹ä¸º1:1

2. TMR3éè½½å¼ä¿®æ¹ (ç¬¬152-153è¡):
   - å° TMR3H = 0xfc æ¹ä¸º TMR3H = 0xff
   - å° TMR3L = 0x17 æ¹ä¸º TMR3L = 0x9c
   - ä¸­æ­é¢çä»1msæé«å°0.1ms (10kHz)

3. PWMå¨æä¿®æ¹ (include.hç¬¬161è¡):
   - å° PWM_PERO = 10 æ¹ä¸º PWM_PERO = 1
   - 1ä¸ªä¸­æ­å¨æä¸ºä¸ä¸ªPWMå¨æ

4. å ç©ºæ¯æ§å¶ä¿®æ¹:
   - pwm_cnt2åéç±»åéè¦ä»unsigned charæ¹ä¸ºfloat
   - åæ¥pwm_cnt2=4è¡¨ç¤º40%å ç©ºæ¯ï¼ç°å¨éè¦pwm_cnt2=0.4

5. é¢çè®¡ç®éªè¯:
   - ä¿®æ¹åPWMé¢ç = 10kHz / 1 = 10kHz

- æé«PWMé¢çä¼å¢å CPUè´è½½
- éè¦ç¡®ä¿æ³µåé©±å¨çµè·¯æ¯æ10kHzå·¥ä½é¢ç
- å ç©ºæ¯æ§å¶é»è¾éè¦ç¸åºè°æ´

hpp: PWMå ç©ºæ¯ä¿®æ¹æ¹æ¡ (ç¬¬178è¡):
- å·²å° pwm_cnt1<=pwm_cnt2 æ¹ä¸º pwm_cnt1<=PWM_PERO
- å®ç°100%å ç©ºæ¯è¾åºï¼æ è®ºååå¼å¦ä½é½å¨åçè¾åº
- ç§»é¤äºä½å ç©ºæ¯è¾åºé»è¾ï¼ç®åäºæ§å¶æµç¨
*/
