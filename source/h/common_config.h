/* 
 * File:   common_config.h
 * Author: liwangzhi
 *
 * Created on April 6, 2021, 8:43 AM
 */

#ifndef COMMON_CONFIG_H
#define	COMMON_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

////////////////// 执行周期数说明 ////////////////////////////////////////////////////////////
///    执行周期数是指代码执行此语句的执行次数，每个周期的实际执行时间是不定的，依据当时状况///
///，执行时间有些许差异。后期如果可能的话，可以改为系统时间的计算方式。                                  					   ///
//////////////////////////////////////////////////////////////////////////////////////////////
#define CANCEL_LEAKAGE_FLAG_CYCLE 200 // 取消漏气标志的时间，单位：执行周期数
#define JUDGE_LEAKAGE_TIMEOUT 10800   // 判断漏气的超时时间，单位：执行周期数
#define JX_MODE_TIME_FACTOR_BY_LX 2
#define LEAKAGE_TIMEOUT_FOR_LIX 2200 // 连续模式下的漏气判定超时时间
#define LEAKAGE_TIMEOUT_FOR_JIX 4400 // 间歇模式下的漏气判定超时时间
#define SET_LEAKAGE_FLAG_CYCLE 2200   // 设置漏气标志的时间，单位：执行周期数
#define JUDGE_CANISTER_FULL_COUNT 4   // 判断液位满的次数
#define JUDGE_CANISTER_FULL_TIME 8000 // 判断液位满的判断时间，在此时间内，完成4次震荡。单位：执行周期数
#define MAX_BEE_TIME 10               // 蜂鸣器持续响的时间，单位：执行周期数
#define BELOW_3_5V_CYCLE 500          // 如果电压小于3.5V，且运行超过BELOW_3_5V_CYCLE个循环，则进行软关机，单位：执行周期数
#define LOWER_THAN_3_5V 5             // 自动关机值，低于3.5V
#define LOWER_BAT_WARN_3_6V 10        // 低电量报警值，低于3.6V
#define LOWER_THAN_3_7V 1             // 低于3.7V
#define LOWER_THAN_3_8V 2             // 低于3.8V
#define LOWER_THAN_4V 3               // 低于3.8V
#define BAT_FULL 4                    // 电池电量满
#define BAT_CHARGING 0                // 电池正在充电状态下
#define NO_KEY_PRESSED 0x3c           // 没有按键被按下时的键值
#define BACK_LED_OFF_TIMEOUT 1500     // 背光灭的超时时间
#define BUZZER_TIME_CYCLE 5           // 蜂鸣器响的时间，单位：执行周期数
#define TK_TIMEOUT_CYCLE 14999        // 空闲超时时间，计算：5*60*1000/20=15000
#define CANCEL_MUTEFLAG_TIMEOUT 15000 // 取消静音模式的倒计时
#define LED_LOW_THAN_3_5V_OR_ERR 10   // 当电量低于3.5V或者发生任何错误时，LED的显示方式：绿屏
#define LED_LOW_THAN_3_6V 1           // 低电量时，LED的显示方式：绿屏，并且界面显示低电量
#define LED_BAT_NORMAL 0              // 电量正常时，LED的显示方式：充电时黄屏，未充电时绿屏
#define UNLOCK_NPWT_KEYVAL 0x24       // 按下解锁键时的键值，即同时按下左右键
#define UNLOCK_NPWT_L_KEYVAL 0xa4     // 长时间按下解锁键时的键值，即同时按下左右键
#define LONG_PRESS_SWITCH_LANG 0x98   // 长时间按下上下键，用以切换语言
#define JUDGE_LONG_PRESS_TIME 100     // 判断按键被长时间按下的时间
#define GET_KEY_VAL PORTB&0x3c        // 获取按键的值，推测：这个寄存器存有按键的当前状态，未被按下时，按键处于高电平；按下后，按键处于低电平；
#define SILENT_FLAG_BITMASK 0x8000    // 静默标志的标志位掩码
#define SILENT_FLAG_SET_TIMEOUT 10000 // 静默标志设置的延时时间
#define JIX_CYCLES_PER_SECOND 50      // 间歇模式下，每秒对应的循环次数
#define LONGPRESS_MINITE_PER_STEP 5   // 长按时，设置时间的步距
#define LONG_PRESS_SET_PRESS_INTERVAL 20   // 设置压力时，长按按钮，连续更改压力的时间间隔
#define MAX_JIX_HIGHMODE_TIME 99      // 间歇高压下设置的高压最多工作时间
#define MAX_JIX_LOWMODE_TIME 99       // 间歇下设置的低压最多工作时间
#define MIN_JIX_HIGHMODE_TIME 1       // 间歇高压下设置的高压最少工作时间
#define MIN_JIX_LOWMODE_TIME 1        // 间歇下设置的低压最少工作时间
#define JUDGE_DS_TIME 180000ul        // 判断堵塞的超时时间
#define RECORDE_PRESSURE_INTERVAL 3000// 记录压力的间隔时间，此时间乘以7就是检测堵塞气压变化的时间
#define JUDGE_YWM_AFTER_PUMP_STOP 499 // 高压状态下停泵后，再经过多少时间，仍旧保持高压，则认为是液位满的一次震荡
#define UPDATE_UI_PRESSURE_TIME_1 50  // 更新界面显示的压力时间因素1，即，多久采集一次数据
#define UPDATE_UI_PRESSURE_TIME_2 8   // 更新界面显示的压力时间因素2，即，多久显示一次采集的数据
#define PRESS_KEY_BUZ_SHOWLQ_TIME 299 // 按下BUZ键，显示漏气的时间
#define MIN_PRESS_REPORT_DS 70        // 报告管路堵塞时，压力大于MIN_PRESS_REPORT_DS，则直接报告；
#define NO_REPORE_PRESS_FOR_DS 57     // 压力小于NO_REPORE_PRESS_FOR_DS时，不报管路堵塞。
#define DS_REPORT_STEADY_COUNTS 7     // 低压状态下，报告管路堵塞时，连续判定DS_REPORT_STEADY_COUNTS次，才会认为是管路堵塞
#define RECORDE_PRESSURE_INTERVAL_LOW 5000    // 低压条件下的堵塞压力记录时间
#define TOP_PRESSURE_MAX 320

enum EnumErr{
	ERR_NONE = 0,// 无错误
	ERR_CANISTER_NOT_CON,// 未接盒子
	ERR_CANISTER_REACHED,// 液位到
	ERR_AIR_LEAKAGE,// 漏气
	ERR_SENSOR_MALFUCTION,// 传感器故障
	ERR_PIPE_BLOCKED,// 管路堵塞
	ERR_CANISTER_FULL,// 液位满
	ERR_JAMED,// 堵塞。todo：两者之间有何区别
	ERR_DEV_IDLE,// 长时间空闲
};

enum EnumUiSelect{
	UI_WORKMODE_SELECT = 0,// 选择工作模式
	UI_MODE_SET_HI,// 间隙模式下，设置高压；或者连续模式下设置压力
	UI_JIX_MODE_SET_LO,// 间歇模式下设置低压
	UI_JIX_SET_HI_TIME,// 间歇模式下，设置高压时间
	UI_JIX_SET_LO_TIME,// 间歇模式下，设置低压时间
	UI_SET_PRESSURE,// 显示设置压力界面
};

// lwz 更改此值，可更换logo
//#define LOGO_TYPE_DEROYAL
// #define LOGO_TYPE_VR_CHINA
//#define LOGO_TYPE_VR_FORIEGN
//#define LOGO_TYPE_VR_MEDWIN

#define LANGUAGE_RUSSIA_ENGILISH 1 //英俄语言支持


// china版本的单独设置
#ifdef LANGUAGE_RUSSIA_ENGILISH
#define LOCK_FLAG_TIMEOUT 1500        // 锁屏的超时时间
#define DEFAULT_TARGET_PRESSURE 120   // 开机默认的目标压力
#define    CFG_HI           300       // 连续模式或间歇模式的高压工作的高压上限
#define    CFG_LOW          20        // 连续模式或间歇模式的高压工作的低压下限
#define JIX_LOW_MODE_HIGHPRESS 100    // 间歇模式的低压模式下的最高设置压力
#define JIX_LOW_MODE_LOWPRESS 10      // 间歇模式的低压模式下的最低设置压力
#define MMHG_PER_STEP 10  // 长按时，设置时间的步距
#define PUMP_IDLE_FLAG                // 若是定义了此标志，则显示pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*置位表示漏气不停泵*/
#endif

// droyal版本的单独设置
#ifdef LOGO_TYPE_DEROYAL
#define LOCK_FLAG_TIMEOUT 45000       // 锁屏的超时时间
#define DEFAULT_TARGET_PRESSURE 125   // 开机默认的目标压力
#define    CFG_HI           200       // 连续模式或间歇模式的高压工作的高压上限
#define    CFG_LOW          20        // 连续模式或间歇模式的高压工作的低压下限
#define JIX_LOW_MODE_HIGHPRESS 100    // 间歇模式的低压模式下的最高设置压力
#define JIX_LOW_MODE_LOWPRESS 10      // 间歇模式的低压模式下的最低设置压力
#define MMHG_PER_STEP 5   // 长按时，设置时间的步距
#define PUMP_IDLE_FLAG                // 若是定义了此标志，则显示pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*置位表示漏气不停泵*/
#endif

// china版本的单独设置
#ifdef LOGO_TYPE_VR_CHINA
#define LOCK_FLAG_TIMEOUT 1500        // 锁屏的超时时间
#define DEFAULT_TARGET_PRESSURE 120   // 开机默认的目标压力
#define    CFG_HI           300       // 连续模式或间歇模式的高压工作的高压上限
#define    CFG_LOW          20        // 连续模式或间歇模式的高压工作的低压下限
#define JIX_LOW_MODE_HIGHPRESS 100    // 间歇模式的低压模式下的最高设置压力
#define JIX_LOW_MODE_LOWPRESS 10      // 间歇模式的低压模式下的最低设置压力
#define MMHG_PER_STEP 10  // 长按时，设置时间的步距
#define PUMP_IDLE_FLAG                // 若是定义了此标志，则显示pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*置位表示漏气不停泵*/
#endif

// 国外版的单独设置
#ifdef LOGO_TYPE_VR_FORIEGN
#define LOCK_FLAG_TIMEOUT 1500        // 锁屏的超时时间
#define DEFAULT_TARGET_PRESSURE 125   // 开机默认的目标压力
#define    CFG_HI           200       // 连续模式或间歇模式的高压工作的高压上限
#define    CFG_LOW          20        // 连续模式或间歇模式的高压工作的低压下限
#define JIX_LOW_MODE_HIGHPRESS 100    // 间歇模式的低压模式下的最高设置压力
#define JIX_LOW_MODE_LOWPRESS 10      // 间歇模式的低压模式下的最低设置压力
#define MMHG_PER_STEP 5   // 长按时，设置时间的步距
#define PUMP_IDLE_FLAG                // 若是定义了此标志，则显示pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*置位表示漏气不停泵*/
#endif

// medway版的单独设置
#ifdef LOGO_TYPE_VR_MEDWIN
#define LOCK_FLAG_TIMEOUT 1500        // 锁屏的超时时间
#define DEFAULT_TARGET_PRESSURE 120   // 开机默认的目标压力
#define    CFG_HI           300       // 连续模式或间歇模式的高压工作的高压上限
#define    CFG_LOW          20        // 连续模式或间歇模式的高压工作的低压下限
#define JIX_LOW_MODE_HIGHPRESS 100    // 间歇模式的低压模式下的最高设置压力
#define JIX_LOW_MODE_LOWPRESS 20      // 间歇模式的低压模式下的最低设置压力
#define MMHG_PER_STEP 5   // 长按时，设置时间的步距
#define PUMP_IDLE_FLAG                // 若是定义了此标志，则显示pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*置位表示漏气不停泵*/
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_CONFIG_H */

