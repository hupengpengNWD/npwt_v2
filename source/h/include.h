/*****************************************
20%--------50mmhg
30%--------60mmhg
50%--------100mmhg
80%--------250--300mmhg
*****************************************/
#ifndef   INCLUDE_H
#define   INCLUDE_H

// #include  "xc.h"
#include	<htc.h>
#include   "sys_cpu.h"

#define    M      0
#define    I      12
#define    N      24
#define    H      36
#define    G      48
#define    L      60
#define    P      72
#define    T      84
#define    D      96
#define    O      108
#define    F      120
#define    V      132
#define    aa     144
#define    ee     156
#define    ll     168
#define    kk     180
#define    ii     192
#define    oo     204
#define    tt     216
#define    ww     228
#define    uu     240
#define    ZHI    0
#define    SHE    24
#define    DI     48
#define    GAO    72
#define    JIAN   96
#define    SHI    120
#define    LI     144
#define    YA     168
#define    XIE    192
#define    LIAN   216/////0
#define    XU     240/////72
#define    KUN    264
#define    SHAN   288
#define    WEI    312
#define    RUI    336
#define    YI     360
#define    LIAO   384
#define    KE     408
#define    JI     432
#define    ZAN    456
#define    TING   480  
#define    YE     288     ///////////////// 504
#define    WZ     360     ///////////////// 504+24
#define    DAO    504+48
#define    WEIJ   504+72
#define    JIE    504+96
#define    HE     504+120
#define    LOU    144      ////////////////504+144
#define    QI     216      ////////////////504+168
#define    CHUAN  504+192
#define    GAN    504+216
#define    QII    504+240 
#define    DU     504+264
#define    GUAN   504+288
#define    MAN    432      /////////////////504+312
#define    ZHEN   504+336
#define    XIAN   504+360
#define    BO     504+384
#define    ZHI1   504+408
#define    TING1  504+432
#define    XING   504+456
#define    YUN    504+480
#define    GONG     1008
#define    ZUO      1032

#define    MAIN_CLK  20
#define    MOD_SYS   0 /*ï¿½ï¿½Ê¼ï¿½ï¿½*/
#define    MOD_WAT   1 /*ï¿½È´ï¿½Ö¸ï¿½ï¿½*/ 
#define    MOD_LIX   2 /*ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½*/
#define    MOD_JIX   3 /*ï¿½ï¿½Ðªï¿½ï¿½ï¿½ï¿½*/
#define    MOD_ZXB   10/*ï¿½ï¿½*/
#define    MOD_SET   4 /*ï¿½è¶¨*/
#define    MOD_ZHT   5 /*ï¿½ï¿½Í£*/
#define    MOD_ERR   6 /*ï¿½ï¿½ï¿½ï¿½*/
#define    MOD_OFF   7 /*ï¿½Ø»ï¿½*/
#define    MOD_TK   8 /*Ì«ï¿½ï¿½*/

#define    MOD_LANGUAGE   9 /*ï¿½ï¿½ï¿½ï¿½ï¿½Ð»ï¿½*/

#define    LANG_ENGLISH  0 /*Ó¢ï¿½ï¿½*/
#define    LANG_CHINA  1 /*ï¿½ï¿½ï¿½ï¿½*/
#define    LANG_DUTCH   2 /*ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½*/
#define    LANG_FRENCH  3 /*ï¿½ï¿½ï¿½ï¿½*/
#define    LANG_GERMAN  4 /*ï¿½ï¿½ï¿½ï¿½*/
#define    LANG_PORTUGUESE  5 /*ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½*/
#define    LANG_SPANISH  6 /*ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½*/



#define    KEY_C     0x38  // lwz Fï¿½ï¿½ KEY_Cï¿½ï¿½KEY_OKÖ¸ï¿½ï¿½ï¿½ï¿½Í¬Ò»ï¿½ï¿½
#define    KEY_OK    0x38  // lwz Fï¿½ï¿½ KEY_Cï¿½ï¿½KEY_OKÖ¸ï¿½ï¿½ï¿½ï¿½Í¬Ò»ï¿½ï¿½
#define    KEY_UP    0x34  ///////0x0b lwz ï¿½ï¿½ï¿½Í·
#define    KEY_DN    0x2c  ///////0x07 lwz ï¿½Ò¼ï¿½Í·
#define    KEY_CL    0xb8  // lwz Fï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define    KEY_OKL   0xb8  // lwz Fï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define    KEY_UPL   0xb4  // lwz ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define    KEY_DNL   0xac  // lwz ï¿½Ò¼ï¿½Í·ï¿½ï¿½ï¿½ï¿½
#define    KEY_MUT   0x1c  // lwz ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define    KEY_MUTL  0x9c  // lwz ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define   FALSE   0
#define  TRUE     1
/*****************************************
	   Î»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
******************************************/

#define    ERRA_LQ     0x01  ////////Â©ï¿½ï¿½
#define    ERRA_V      0x02  ////////Î´ï¿½ï¿½Òºï¿½ï¿½
#define    ERRA_YW     0x04  ////////ÒºÎ»ï¿½ï¿½ lwz ï¿½Ë´ï¿½ï¿½ï¿½Ã»ï¿½ï¿½ï¿½Ãµï¿½
#define    ERRA_S      0x08  ////////ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define    ERRB_DS     0x10  //ï¿½ï¿½Â·ï¿½ï¿½ï¿½ï¿½
#define    ERRB_TK     0x20  // ï¿½ï¿½ï¿½ï¿½5ï¿½ï¿½ï¿½ï¿½Òªï¿½ï¿½ï¿½ï¿½
#define    ERRB_YW     0x40  // lwz ÒºÎ»ï¿½ï¿½
#define    ERRB_S      0x80  // lwz ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½

#define    BAT_GOOD    PORTCbits.RC0
#define    BAT_CHARGE  PORTCbits.RC1
#define    GRE         LATCbits.LATC4
#define    YEL         LATCbits.LATC5 // lwz ï¿½ï¿½Ä»ï¿½ï¿½ï¿½ï¿½É«LED
#define    DRV_EN      LATCbits.LATC3 // lwz ï¿½ï¿½Ä»ï¿½Ä»ï¿½É«LED
#define    VAL1        LATBbits.LATB0    /////////start ï¿½ï¿½1ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã¹ï¿½ï¿½ï¿½ï¿½Ä·ï¿½
#define    VAL2        LATBbits.LATB1    /////////open  ï¿½ï¿½2ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ÒºÎ»ï¿½ï¿½ï¿½ï¿½Ê±ï¿½ï¿½Ð¹ï¿½ï¿½ï¿½Ä·ï¿½
#define    SPEAK       LATAbits.LATA3
#define    PUMP        LATCbits.LATC7
#define    POWER_ON    LATCbits.LATC2
#define    AUDIO_EN    LATAbits.LATA1

#define  AUDIO_EN  LATAbits.LATA1
#define  AUDIO_G2  LATAbits.LATA2
#define  AUDIO_G1  LATAbits.LATA3
#define  AUDIO_G0  LATAbits.LATA5
#define  AUDIO_CNT     3      ///////////////ï¿½ï¿½ï¿½Ð´ï¿½ï¿½ï¿½
#define  AUDIO_TIM     10     ///////////////ï¿½ï¿½ï¿½ï¿½20*50--1ï¿½ï¿½
//#define  AUDIO_PERIOD  1000   ////////////////20ï¿½ï¿½


#define  AUDIO_PERIOD  250   ////////////////5ï¿½ï¿½

#define    T3ON        T3CONbits.TMR3ON
#define    TX2IF       PIR3bits.4
#define    RC2IF       PIR3bits.5

#define    JLX12864G_ON    0xaf
#define    JLX12864G_OFF   0xae

//#define    CFG_HI           200
//#define    CFG_LOW          20
#define    CFG_JIANX_LOW         0
#define    PWM_PERO         10   // 20MS--50HZ;100MS---10Hz
// hpp: è¦ä¿®æ¹PWMé¢çå°10kHzï¼å°PWM_PEROæ¹ä¸º1 (1ä¸ªä¸­æ­å¨æä¸ºä¸ä¸ªPWMå¨æ)

/***************************************************************************************/
#define ADC_CNT  3                ////////////////////////////////////////////adc×ªï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
#define ADC_CHN  3                  ////////////////////////////////////////////adcÍ¨ï¿½ï¿½ï¿½ï¿½
/////////////////////////////////////////////////////////////////////////////////////////

#define addr 0xa000	    //ï¿½ï¿½ï¿½ï¿½ÒªÐ´ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½Ö·  db00

//extern unsigned short 

#include "common_config.h"// ï¿½ï¿½ï¿½ï¿½ï¿½Ë³ï¿½ï¿½ï¿½ï¿½Ðµï¿½Ò»Ð©È«ï¿½Öºê£¬ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ã³ï¿½ï¿½ï¿½Ä°æ±¾ï¿½Í²ï¿½ï¿½ï¿½

#endif

