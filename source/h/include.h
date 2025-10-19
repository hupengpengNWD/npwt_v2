
/****************************************************************************
 * 文件名: include.h
 * 功能: 系统全局头文件
 * 
 * 包含内容:
 *   1. 编译器头文件（htc.h）
 *   2. 中文字库索引宏定义（用于LCD显示）
 *   3. 系统工作模式定义
 *   4. 多语言支持定义
 *   5. 按键值定义
 *   6. 错误标志位定义
 *   7. 硬件IO宏定义
 *   8. ADC通道配置
 *   9. Flash存储地址定义
 * 
 * 所有C文件都应包含此头文件
 ****************************************************************************/

#ifndef   INCLUDE_H
#define   INCLUDE_H

#include	<htc.h>
#include   "sys_cpu.h"

/****************************************************************************
 * 中文字库索引宏定义
 * 说明：这些宏定义用于在字库数组中快速定位中文字符
 *      每个字符占用12或24字节（取决于字体大小）
 ****************************************************************************/

/* 英文字母和符号偏移量（8×16字体） */
#define    M      0     // 字符'M'
#define    I      12    // 字符'I'
#define    N      24    // 字符'N'
#define    H      36    // 字符'H'
#define    G      48    // 字符'G'
#define    L      60    // 字符'L'
#define    P      72    // 字符'P'
#define    T      84    // 字符'T'
#define    D      96    // 字符'D'
#define    O      108   // 字符'O'
#define    F      120   // 字符'F'
#define    V      132   // 字符'V'
#define    aa     144   // 小写字母
#define    ee     156
#define    ll     168
#define    kk     180
#define    ii     192
#define    oo     204
#define    tt     216
#define    ww     228
#define    uu     240
/* 中文字符偏移量（汉字字库，每字24字节） */
#define    ZHI    0      // "置"
#define    SHE    24     // "设"
#define    DI     48     // "低"
#define    GAO    72     // "高"
#define    JIAN   96     // "间"
#define    SHI    120    // "时"
#define    LI     144    // "力"
#define    YA     168    // "压"
#define    XIE    192    // "泄"
#define    LIAN   216    // "连"
#define    XU     240    // "续"
#define    KUN    264    // "困"
#define    SHAN   288    // "闪"
#define    WEI    312    // "未"
#define    RUI    336    // "锐"
#define    YI     360    // "已"
#define    LIAO   384    // "疗"
#define    KE     408    // "可"
#define    JI     432    // "集"
#define    ZAN    456    // "暂"
#define    TING   480    // "停"
#define    YE     288    // "液"
#define    WZ     360    // "位"
#define    DAO    504+48  // "到"
#define    WEIJ   504+72  // "未"
#define    JIE    504+96  // "接"
#define    HE     504+120 // "和"
#define    LOU    144     // "漏"
#define    QI     216     // "气"
#define    CHUAN  504+192 // "传"
#define    GAN    504+216 // "感"
#define    QII    504+240 // "器"
#define    DU     504+264 // "堵"
#define    GUAN   504+288 // "管"
#define    MAN    432     // "满"
#define    ZHEN   504+336 // "诊"
#define    XIAN   504+360 // "显"
#define    BO     504+384 // "波"
#define    ZHI1   504+408 // "治"
#define    TING1  504+432 // "停"
#define    XING   504+456 // "行"
#define    YUN    504+480 // "运"
#define    GONG     1008  // "工"
#define    ZUO      1032  // "作"

/****************************************************************************
 * 系统配置宏定义
 ****************************************************************************/
#define    MAIN_CLK  20    // 主时钟频率（MHz）

/****************************************************************************
 * 系统工作模式定义
 * 说明：设备的7种主要工作模式
 ****************************************************************************/
#define    MOD_SYS   0     // 系统初始化模式
#define    MOD_WAT   1     // 等待指令模式（待机）
#define    MOD_LIX   2     // 连续工作模式（持续负压）
#define    MOD_JIX   3     // 间歇工作模式（高压-低压循环）
#define    MOD_ZXB   10    // 暂停模式（备用）
#define    MOD_SET   4     // 参数设定模式
#define    MOD_ZHT   5     // 暂停模式
#define    MOD_ERR   6     // 错误模式
#define    MOD_OFF   7     // 关机模式
#define    MOD_TK    8     // 空闲超时模式（5分钟无操作）

#define    MOD_LANGUAGE   9  // 语言切换模式 

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

