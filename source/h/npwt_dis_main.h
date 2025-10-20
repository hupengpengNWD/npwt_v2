#ifndef __main__h__
#define __main__h__

extern unsigned char    audio_flg;
extern unsigned short   audio_cnt,audio_basic;
extern unsigned short   audio_period;
extern unsigned char    err_codea,err_codeb;
extern unsigned char    buz_flg,buz_flg1,buz_cnt;
extern unsigned short   bat_close_tim;
extern unsigned char    mute_flg;
extern unsigned short   mute_tim; 

extern unsigned char  SPEAK_flg; 
extern unsigned char  close_flg;
extern unsigned char   	bat_lev_bak;
extern unsigned char battery_status_flags;  // 电池状态标志位

extern unsigned char   	open_bum;
extern unsigned char   	bum_dly;
extern unsigned char   	bum_dly_flg;
extern unsigned short          dataREAD0,dataREAD1,dataREAD2,dataREAD3;
extern volatile unsigned short TK_TIME ;
extern unsigned short   key_silent_flag;
extern  void LEVEL_WarnA(void);

extern void BUZ_KeyCls(void);

#endif

