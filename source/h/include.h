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
#define    MOD_SYS   0 /*初始化*/
#define    MOD_WAT   1 /*等待指令*/ 
#define    MOD_LIX   2 /*连续运行*/
#define    MOD_JIX   3 /*间歇运行*/
#define    MOD_ZXB   10/*无*/
#define    MOD_SET   4 /*设定*/
#define    MOD_ZHT   5 /*暂停*/
#define    MOD_ERR   6 /*错误*/
#define    MOD_OFF   7 /*关机*/
#define    MOD_TK   8 /*太空*/

#define    MOD_LANGUAGE   9 /*语言切换*/

#define    LANG_ENGLISH  0 /*英语*/
#define    LANG_CHINA  1 /*中文*/
#define    LANG_DUTCH   2 /*荷兰语*/
#define    LANG_FRENCH  3 /*法语*/
#define    LANG_GERMAN  4 /*德语*/
#define    LANG_PORTUGUESE  5 /*葡萄牙*/
#define    LANG_SPANISH  6 /*西班牙*/



#define    KEY_C     0x38  // lwz F键 KEY_C和KEY_OK指的是同一个
#define    KEY_OK    0x38  // lwz F键 KEY_C和KEY_OK指的是同一个
#define    KEY_UP    0x34  ///////0x0b lwz 左箭头
#define    KEY_DN    0x2c  ///////0x07 lwz 右箭头
#define    KEY_CL    0xb8  // lwz F键长按
#define    KEY_OKL   0xb8  // lwz F键长按
#define    KEY_UPL   0xb4  // lwz 左键长按
#define    KEY_DNL   0xac  // lwz 右箭头长按
#define    KEY_MUT   0x1c  // lwz 静音键
#define    KEY_MUTL  0x9c  // lwz 静音键长按
#define   FALSE   0
#define  TRUE     1
/*****************************************
	   位定义区
******************************************/

#define    ERRA_LQ     0x01  ////////漏气
#define    ERRA_V      0x02  ////////未接液盒
#define    ERRA_YW     0x04  ////////液位到 lwz 此错误没有用到
#define    ERRA_S      0x08  ////////传感器
#define    ERRB_DS     0x10  //管路堵塞
#define    ERRB_TK     0x20  // 空闲5分钟要报警
#define    ERRB_YW     0x40  // lwz 液位满
#define    ERRB_S      0x80  // lwz 传感器触发

#define    BAT_GOOD    PORTCbits.RC0
#define    BAT_CHARGE  PORTCbits.RC1
#define    GRE         LATCbits.LATC4
#define    YEL         LATCbits.LATC5 // lwz 屏幕的绿色LED
#define    DRV_EN      LATCbits.LATC3 // lwz 屏幕的黄色LED
#define    VAL1        LATBbits.LATB0    /////////start 阀1，辅助泵工作的阀
#define    VAL2        LATBbits.LATB1    /////////open  阀2，用于液位满的时候泄气的阀
#define    SPEAK       LATAbits.LATA3
#define    PUMP        LATCbits.LATC7
#define    POWER_ON    LATCbits.LATC2
#define    AUDIO_EN    LATAbits.LATA1

#define  AUDIO_EN  LATAbits.LATA1
#define  AUDIO_G2  LATAbits.LATA2
#define  AUDIO_G1  LATAbits.LATA3
#define  AUDIO_G0  LATAbits.LATA5
#define  AUDIO_CNT     3      ///////////////鸣叫次数
#define  AUDIO_TIM     10     ///////////////周期20*50--1秒
//#define  AUDIO_PERIOD  1000   ////////////////20秒


#define  AUDIO_PERIOD  250   ////////////////5秒

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
#define ADC_CNT  3                ////////////////////////////////////////////adc转换次数
#define ADC_CHN  3                  ////////////////////////////////////////////adc通道数
/////////////////////////////////////////////////////////////////////////////////////////

#define addr 0xa000	    //定义要写入的起始地址  db00

//extern unsigned short 

#include "common_config.h"// 定义了程序中的一些全局宏，用以配置程序的版本和参数

#endif

