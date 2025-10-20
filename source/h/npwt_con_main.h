/****************************************************************************
 * 文件名: npwt_con_main.h
 * 功能: 控制模块头文件
 * 
 * 说明: 
 *   【架构升级】
 *   原有的全局变量extern声明已移除
 *   现在通过 global_compat.h 提供的兼容层访问
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef __npwt_con_main_h_
#define __npwt_con_main_h_

/* 电池电压阈值定义（ADC值） */
#define    BAT4          315     // 电池电量4格
#define    BAT3          305     // 电池电量3格
#define    BAT2          295     // 电池电量2格
#define    BAT1          285     // 电池电量1格
#define    BAT0          275     // 电池电量0格（低电压）

#define    WJ_LEV        150     // 液位检测阈值

#define    YEW_SES       15      // 黄色LED闪烁速率
#define    LQ_SES        10      // 泄漏检测灵敏度

/* 函数声明 */
extern void LEVEL_WarnA(void);
extern void BAT_WarnFir(void);
extern void DISP_MainA(void);
extern void BAT_Warn(void);

#endif
