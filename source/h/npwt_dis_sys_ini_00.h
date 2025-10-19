#ifndef _npwt_dis_sys_ini_00_
#define _npwt_dis_sys_ini_00_


typedef struct	_TIMER
{
	unsigned long	TimeOutVal; 			//time out value
	unsigned long	RecTickVal;			  	//softtimer setting value
	unsigned char	IsTimeOut;				//time out flag
} TIMER;

extern volatile unsigned long   Timeone_sum;  //ȫ�ּ���
extern unsigned char FLG_SYS_10MS;	
extern unsigned char  os_cnt;
extern unsigned char  pwm_cnt1;
extern unsigned char  pwm_cnt2;
// hpp: 当PWM频率提高到10kHz时，需要改为 extern float pwm_cnt2;
//extern volatile unsigned long  ;  //ȫ�ּ���
extern unsigned char TIMER_FOUR;
extern  unsigned short  psps_turn;
extern unsigned int psps[30];
extern void SYS_OSC_Ini(void)/*used*/;

extern void SYS_IO_Ini(void)/*used*/;

extern void SYS_TMR0_Ini(void)/*used*/;

extern void SYS_TMR3_Ini(void)/*used*/;

extern void SYS_ADCini(void)/*used*/;

extern unsigned long T1_TimeGet();
extern void TimeOutSet(TIMER *timer,unsigned long timeout);

extern unsigned char IsTimeOut(	TIMER 	*timer);
#endif