#ifndef _npwt_dis_ofile_lcd_02_
#define _npwt_dis_ofile_lcd_02_

extern unsigned char        language;
extern unsigned char   bat_cnt,bat_cnt1;
extern unsigned char   led_cnt;
extern unsigned short  run_tim_a;
extern unsigned short  disp_presa;
extern unsigned short  disp_ssa;
extern unsigned char   disp_set_flg;
extern unsigned short  dis_cnta;
extern unsigned char   add91200;
extern unsigned short  add91201;
extern unsigned char   zht_flg;
extern unsigned short  adc_temp00;
extern unsigned char  all_flg;
extern unsigned char  flg2015;
extern unsigned char  flg_disp;
//extern struct show_lq_times;


///20160510 add////////////////////////////////////////////////////////////
struct 
{
	unsigned short lq_times;// lwz 放气次数
	unsigned char show_times;
	unsigned char step;
	unsigned long buz_key_pressed_times;// lwz 按下BUZ键的时间
}show_lq_times;


extern  unsigned short bbbbb;
extern void DISP_Led(void)/*used*/;
extern void  clear_lqtimes(void);
extern void DISP_Bat(void)/*used*/;


#endif

