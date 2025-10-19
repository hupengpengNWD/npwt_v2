#ifndef __npwt_con_main_h_
#define __npwt_con_main_h_
/*********************************
3.5v   275
3.6v   285
3.7v   295
3.8v   305
4.0v   315
³äµçÖÐ CHARGE=0
³äÂú£º CHARGE=1
¶Ï¿ª£º CHARGE=1

********************************/
#define    BAT4          315     ///////4.0v
#define    BAT3          305     ///////3.8v
#define    BAT2          295     ///////3.7v
#define    BAT1          285     ///////3.6v
#define    BAT0          275     ///////3.5v
/********************************/
#define    WJ_LEV        150     ///////////////////////////////////////////Î´½ÓÒººÐÖµ
////#define    LQ_FIR    1100    ///////////////////////////////////////////Â©Æø³õÖµ
#define    YEW_SES       15  
#define    LQ_SES        10
extern unsigned char   bat_sas;
extern void LEVEL_WarnA(void)/*used*/;
extern void BAT_WarnFir(void)/*used*/;
extern void  DISP_MainA(void)/*used*/;
extern void BAT_Warn(void)/*used*/;

#endif
