#ifndef  _npwt_key_h__
#define  _npwt_key_h__
extern unsigned char   tim5_flg;
extern unsigned short  key_cnt;
extern unsigned char   key_key;
extern unsigned char   set_a00;

extern unsigned char key_port,key_port_bak,key_val,key_val_bak,key_flg,key_flg_l;
extern unsigned char mod_main_a,mod_main_b,mod_main_baka,mod_jixa;
extern unsigned char mod_seta_cnt,mod_seta_wok;
extern unsigned short mod_seta_preh,mod_seta_preh_bak,mod_seta_prel,mod_seta_ont,mod_seta_oft;
extern unsigned short mod_tim_cnta,mod_tim_cnta2;

extern unsigned char   mod_bak_bak;
extern unsigned short back_led_cnt,lock_cnt;
extern unsigned char  lock_flg;
extern  unsigned char   key_set_tim,key_start_tim;
extern unsigned char remember_mod;
extern unsigned short  set_moda;
extern float   valueK;
extern unsigned short  dataK1,dataK2,dataK3,dataK4,dataK;
extern unsigned char   ddfq;
extern void  MODE_ProA(void);
extern void KEY_Scan(void);
extern void  MODE_Pro(void);
#endif