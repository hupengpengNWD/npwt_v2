
#ifndef   INCLUDE_H
#define   INCLUDE_H

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
#define    LIAN   216
#define    XU     240
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
#define    YE     288     
#define    WZ     360     
#define    DAO    504+48
#define    WEIJ   504+72
#define    JIE    504+96
#define    HE     504+120
#define    LOU    144      
#define    QI     216      
#define    CHUAN  504+192
#define    GAN    504+216
#define    QII    504+240 
#define    DU     504+264
#define    GUAN   504+288
#define    MAN    432      
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
#define    MOD_SYS   0 
#define    MOD_WAT   1  
#define    MOD_LIX   2 
#define    MOD_JIX   3 
#define    MOD_ZXB   10
#define    MOD_SET   4 
#define    MOD_ZHT   5 
#define    MOD_ERR   6 
#define    MOD_OFF   7 
#define    MOD_TK   8 

#define    MOD_LANGUAGE   9 

#define    LANG_ENGLISH  0 
#define    LANG_CHINA  1 
#define    LANG_DUTCH   2 
#define    LANG_FRENCH  3 
#define    LANG_GERMAN  4 
#define    LANG_PORTUGUESE  5 
#define    LANG_SPANISH  6 

#define    KEY_C     0x38  
#define    KEY_OK    0x38  
#define    KEY_UP    0x34  
#define    KEY_DN    0x2c  
#define    KEY_CL    0xb8  
#define    KEY_OKL   0xb8  
#define    KEY_UPL   0xb4  
#define    KEY_DNL   0xac  
#define    KEY_MUT   0x1c  
#define    KEY_MUTL  0x9c  
#define   FALSE   0
#define  TRUE     1

#define    ERRA_LQ     0x01  
#define    ERRA_V      0x02  
#define    ERRA_YW     0x04  
#define    ERRA_S      0x08  
#define    ERRB_DS     0x10  
#define    ERRB_TK     0x20  
#define    ERRB_YW     0x40  
#define    ERRB_S      0x80  

#define    BAT_GOOD    PORTCbits.RC0
#define    BAT_CHARGE  PORTCbits.RC1
#define    GRE         LATCbits.LATC4
#define    YEL         LATCbits.LATC5 
#define    DRV_EN      LATCbits.LATC3 
#define    VAL1        LATBbits.LATB0    
#define    VAL2        LATBbits.LATB1    
#define    SPEAK       LATAbits.LATA3
#define    PUMP        LATCbits.LATC7
#define    POWER_ON    LATCbits.LATC2
#define    AUDIO_EN    LATAbits.LATA1

#define  AUDIO_EN  LATAbits.LATA1
#define  AUDIO_G2  LATAbits.LATA2
#define  AUDIO_G1  LATAbits.LATA3
#define  AUDIO_G0  LATAbits.LATA5
#define  AUDIO_CNT     3      
#define  AUDIO_TIM     10     

#define  AUDIO_PERIOD  250   

#define    T3ON        T3CONbits.TMR3ON
#define    TX2IF       PIR3bits.4
#define    RC2IF       PIR3bits.5

#define    JLX12864G_ON    0xaf
#define    JLX12864G_OFF   0xae

#define    CFG_JIANX_LOW         0
#define    PWM_PERO         10   

#define ADC_CNT  3                
#define ADC_CHN  3                  

#define addr 0xa000	    

#include "common_config.h"

#endif

