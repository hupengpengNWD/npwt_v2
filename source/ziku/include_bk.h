/*****************************************
20%--------50mmhg
30%--------60mmhg
50%--------100mmhg
80%--------250--300mmhg
*****************************************/
#ifndef   INCLUDE_H
#define   INCLUDE_H

#include  <xc.h>
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
#define    MOD_SYS   0 /*³õÊ¼»¯*/
#define    MOD_WAT   1 /*µÈ´ýÖ¸Áî*/ 
#define    MOD_LIX   2 /*Á¬ÐøÔËÐÐ*/
#define    MOD_JIX   3 /*¼äÐªÔËÐÐ*/
#define    MOD_ZXB   10/*ÎÞ*/
#define    MOD_SET   4 /*Éè¶¨*/
#define    MOD_ZHT   5 /*ÔÝÍ£*/
#define    MOD_ERR   6 /*´íÎó*/
#define    MOD_OFF   7 /*¹Ø»ú*/
#define    KEY_C     0x38  
#define    KEY_OK    0x38  ///////0x38  
#define    KEY_UP    0x34  ///////0x0b
#define    KEY_DN    0x2c  ///////0x07
#define    KEY_CL    0xb8  
#define    KEY_OKL   0xb8
#define    KEY_UPL   0xb4
#define    KEY_DNL   0xac
#define    KEY_MUT   0x1c  
#define    KEY_MUTL  0x9c 

/*****************************************
	   Î»¶¨ÒåÇø
******************************************/

#define    ERRA_LQ     0x01  ////////Â©Æø
#define    ERRA_V      0x02  ////////Î´½ÓÒººÐ
#define    ERRA_YW     0x04  ////////ÒºÎ»µ½
#define    ERRA_S      0x08  ////////´«¸ÐÆ÷
#define    ERRB_DS     0x10  //¹ÜÂ·¶ÂÈû
#define    ERRB_V      0x20  
#define    ERRB_YW     0x40  
#define    ERRB_S      0x80 

#define    BAT_GOOD    PORTCbits.RC0
#define    BAT_CHARGE  PORTCbits.RC1
#define    GRE         LATCbits.LATC4
#define    YEL         LATCbits.LATC5
#define    DRV_EN      LATCbits.LATC3
#define    VAL1        LATBbits.LATB0    /////////start  Ð­Öú±ÃÆô¶¯µÄÐ¹Æø·§
#define    VAL2        LATBbits.LATB1    /////////open   ¸ßÑ¹·ÅÆøµÄÐ¹Æø·§
#define    SPEAK       LATAbits.LATA3
#define    PUMP        LATCbits.LATC7
#define    POWER_ON    LATCbits.LATC2
#define    AUDIO_EN    LATAbits.LATA1

#define  AUDIO_EN  LATAbits.LATA1
#define  AUDIO_G2  LATAbits.LATA2
#define  AUDIO_G1  LATAbits.LATA3
#define  AUDIO_G0  LATAbits.LATA5
#define  AUDIO_CNT     3      ///////////////Ãù½Ð´ÎÊý
#define  AUDIO_TIM     10     ///////////////ÖÜÆÚ20*50--1Ãë
#define  AUDIO_PERIOD  1000   ////////////////20Ãë

#define    T3ON        T3CONbits.TMR3ON
#define    TX2IF       PIR3bits.4
#define    RC2IF       PIR3bits.5

#define    JLX12864G_ON    0xaf
#define    JLX12864G_OFF   0xae

//#define    CFG_HI           200
//#define    CFG_LOW          20
#define    CFG_JIANX_LOW         0
#define    PWM_PERO         10   // 20MS--50HZ;100MS---10Hz

/***************************************************************************************/
#define ADC_CNT  3                ////////////////////////////////////////////adc×ª»»´ÎÊý
#define ADC_CHN  3                  ////////////////////////////////////////////adcÍ¨µÀÊý
/////////////////////////////////////////////////////////////////////////////////////////

#define addr 0xa000	    //¶¨ÒåÒªÐ´ÈëµÄÆðÊ¼µØÖ·  db00

#endif

