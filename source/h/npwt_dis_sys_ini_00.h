#ifndef _npwt_dis_sys_ini_00_
#define _npwt_dis_sys_ini_00_

typedef struct	_TIMER
{
	unsigned long	TimeOutVal; 			
	unsigned long	RecTickVal;			  	
	unsigned char	IsTimeOut;				
} TIMER;

extern volatile unsigned long   Timeone_sum;  
extern unsigned char FLG_SYS_10MS;	
extern unsigned char  os_cnt;
extern unsigned char  pwm_cnt1;
extern unsigned char  pwm_cnt2;

extern unsigned char TIMER_FOUR;
extern  unsigned short  psps_turn;
extern unsigned int psps[30];
extern void SYS_OSC_Ini(void);

extern void SYS_IO_Ini(void);

extern void SYS_TMR0_Ini(void);

extern void SYS_TMR3_Ini(void);

extern void SYS_ADCini(void);

extern unsigned long T1_TimeGet();
extern void TimeOutSet(TIMER *timer,unsigned long timeout);

extern unsigned char IsTimeOut(	TIMER 	*timer);
#endif