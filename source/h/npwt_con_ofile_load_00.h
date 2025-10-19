#ifndef __npwt_con_ofile_load_00_h_
#define __npwt_con_ofile_load_00_h_

#define  LOW_DAT    20   
#define  HIGH_DAT   120  
#define  MOT_SES    160.0

#define  CON_SES    40     
#define  SES_50     10
#define  SES_200    40   
#define  SES_300    40   

#define  TIM_50     20  
#define  TIM_200    20
#define  TIM_300    10

extern unsigned short	 load_perioda_up;
extern unsigned short   cnt_cnta,cnt_cntaa;

extern unsigned char    bump_need_out_air_flg;

extern unsigned short   mod_seta_prehh;
extern float    pwm_k1;
extern unsigned short   fq_cnt1;

extern void OPEN_PwmA(void);

extern void CLS_PwmA(void);

extern void PRESS_ConA(void);

#endif