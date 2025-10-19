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
#define    MOD_SYS   0 /*��ʼ��*/
#define    MOD_WAT   1 /*�ȴ�ָ��*/ 
#define    MOD_LIX   2 /*��������*/
#define    MOD_JIX   3 /*��Ъ����*/
#define    MOD_ZXB   10/*��*/
#define    MOD_SET   4 /*�趨*/
#define    MOD_ZHT   5 /*��ͣ*/
#define    MOD_ERR   6 /*����*/
#define    MOD_OFF   7 /*�ػ�*/
#define    MOD_TK   8 /*̫��*/

#define    MOD_LANGUAGE   9 /*�����л�*/

#define    LANG_ENGLISH  0 /*Ӣ��*/
#define    LANG_CHINA  1 /*����*/
#define    LANG_DUTCH   2 /*������*/
#define    LANG_FRENCH  3 /*����*/
#define    LANG_GERMAN  4 /*����*/
#define    LANG_PORTUGUESE  5 /*������*/
#define    LANG_SPANISH  6 /*������*/



#define    KEY_C     0x38  // lwz F�� KEY_C��KEY_OKָ����ͬһ��
#define    KEY_OK    0x38  // lwz F�� KEY_C��KEY_OKָ����ͬһ��
#define    KEY_UP    0x34  ///////0x0b lwz ���ͷ
#define    KEY_DN    0x2c  ///////0x07 lwz �Ҽ�ͷ
#define    KEY_CL    0xb8  // lwz F������
#define    KEY_OKL   0xb8  // lwz F������
#define    KEY_UPL   0xb4  // lwz �������
#define    KEY_DNL   0xac  // lwz �Ҽ�ͷ����
#define    KEY_MUT   0x1c  // lwz ������
#define    KEY_MUTL  0x9c  // lwz ����������
#define   FALSE   0
#define  TRUE     1
/*****************************************
	   λ������
******************************************/

#define    ERRA_LQ     0x01  ////////©��
#define    ERRA_V      0x02  ////////δ��Һ��
#define    ERRA_YW     0x04  ////////Һλ�� lwz �˴���û���õ�
#define    ERRA_S      0x08  ////////������
#define    ERRB_DS     0x10  //��·����
#define    ERRB_TK     0x20  // ����5����Ҫ����
#define    ERRB_YW     0x40  // lwz Һλ��
#define    ERRB_S      0x80  // lwz ����������

#define    BAT_GOOD    PORTCbits.RC0
#define    BAT_CHARGE  PORTCbits.RC1
#define    GRE         LATCbits.LATC4
#define    YEL         LATCbits.LATC5 // lwz ��Ļ����ɫLED
#define    DRV_EN      LATCbits.LATC3 // lwz ��Ļ�Ļ�ɫLED
#define    VAL1        LATBbits.LATB0    /////////start ��1�������ù����ķ�
#define    VAL2        LATBbits.LATB1    /////////open  ��2������Һλ����ʱ��й���ķ�
#define    SPEAK       LATAbits.LATA3
#define    PUMP        LATCbits.LATC7
#define    POWER_ON    LATCbits.LATC2
#define    AUDIO_EN    LATAbits.LATA1

#define  AUDIO_EN  LATAbits.LATA1
#define  AUDIO_G2  LATAbits.LATA2
#define  AUDIO_G1  LATAbits.LATA3
#define  AUDIO_G0  LATAbits.LATA5
#define  AUDIO_CNT     3      ///////////////���д���
#define  AUDIO_TIM     10     ///////////////����20*50--1��
//#define  AUDIO_PERIOD  1000   ////////////////20��


#define  AUDIO_PERIOD  250   ////////////////5��

#define    T3ON        T3CONbits.TMR3ON
#define    TX2IF       PIR3bits.4
#define    RC2IF       PIR3bits.5

#define    JLX12864G_ON    0xaf
#define    JLX12864G_OFF   0xae

//#define    CFG_HI           200
//#define    CFG_LOW          20
#define    CFG_JIANX_LOW         0
#define    PWM_PERO         10   // 20MS--50HZ;100MS---10Hz
// hpp: 要修改PWM频率到10kHz，将PWM_PERO改为1 (1个中断周期为一个PWM周期)

/***************************************************************************************/
#define ADC_CNT  3                ////////////////////////////////////////////adcת������
#define ADC_CHN  3                  ////////////////////////////////////////////adcͨ����
/////////////////////////////////////////////////////////////////////////////////////////

#define addr 0xa000	    //����Ҫд�����ʼ��ַ  db00

//extern unsigned short 

#include "common_config.h"// �����˳����е�һЩȫ�ֺ꣬�������ó���İ汾�Ͳ���

#endif

