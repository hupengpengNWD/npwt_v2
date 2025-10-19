#ifndef __npwt_con_over_h_
#define __npwt_con_over_h_
extern void get_press_delta(unsigned short press);
extern unsigned short   fall_cnta0 ;
extern unsigned char    fall_stata ;
extern unsigned long scan_dusai_time;
extern unsigned char    con_flg_falla ;
extern unsigned short   gao_cnt ;
extern unsigned short   gao_cnt1 ;
extern 	unsigned short con_hi_delta;
extern 	unsigned short con_lo_delta;
extern unsigned short   tim_tima ;
unsigned char oppump_flg;
extern unsigned short   adc_ps00;
extern unsigned char    det300 ;
extern unsigned char    det00 ;
extern unsigned short debug_times;
extern unsigned short debug_air;
extern unsigned char  debug;

extern unsigned short record_ds[8];
extern unsigned char  record_ds_turn;
extern unsigned short twenty_seconds;
extern unsigned char   overabc;

extern void STAT_conNewa(void);

#endif

