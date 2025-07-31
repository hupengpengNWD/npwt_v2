#ifndef __npwt_con_main_h_
#define __npwt_con_main_h_
/*********************************
3.5v   275
3.6v   285
3.7v   295
3.8v   305
4.0v   315
充电中 CHARGE=0
充满： CHARGE=1
断开： CHARGE=1

********************************/
#define    BAT4          315     ///////4.0v
#define    BAT3          305     ///////3.8v
#define    BAT2          295     ///////3.7v
#define    BAT1          285     ///////3.6v
#define    BAT0          275     ///////3.5v
/********************************/
#define    WJ_LEV        150     ///////////////////////////////////////////未接液盒值
////#define    LQ_FIR    1100    ///////////////////////////////////////////漏气初值
#define    YEW_SES       15  
#define    LQ_SES        10
extern unsigned char   bat_sas;
extern void LEVEL_WarnA(void)/*used*/;
extern void BAT_WarnFir(void)/*used*/;
extern void  DISP_MainA(void)/*used*/;
extern void BAT_Warn(void)/*used*/;

#endif
