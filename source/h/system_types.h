/****************************************************************************
 * 文件名: system_types.h
 * 功能: 系统数据结构定义
 * 
 * 说明: 
 *   将分散的全局变量封装到结构体中，提高代码组织性和可维护性
 *   减少全局变量数量，降低模块间耦合度
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef SYSTEM_TYPES_H
#define SYSTEM_TYPES_H

/****************************************************************************
 * 音频报警控制结构体
 ****************************************************************************/
typedef struct {
	unsigned char    audio_flg;          // 声音状态标志
	unsigned short   audio_cnt;          // 声音计数器
	unsigned short   audio_basic;        // 声音基础计数
	unsigned short   audio_period;       // 声音周期计数
	unsigned char    buz_flg;            // 蜂鸣器标志
	unsigned char    buz_flg1;           // 蜂鸣器状态
	unsigned char    buz_cnt;            // 蜂鸣器计数器
	unsigned char    mute_flg;           // 静音标志
	unsigned short   mute_tim;           // 静音计时器
	unsigned char    SPEAK_flg;          // 按键静音操作标志
	unsigned char    close_flg;          // 静音关闭标志
	unsigned char    SPK_STATE;          // 扬声器状态
	unsigned short   spk_selay;          // 扬声器延时计数器
} AudioControl_t;

/****************************************************************************
 * 电池管理结构体
 ****************************************************************************/
typedef struct {
	unsigned char    bat_lev;            // 当前电池电量等级（0-4格）
	unsigned char    bat_lev_bak;        // 电池电量备份值
	unsigned char    bat_sas;            // 电池采样状态
	unsigned char    bat_sas_bak;        // 电池采样备份
	unsigned short   bat_close_tim;      // 低电自动关机计时器
	unsigned char    battery_status_flags; // 电池综合状态位
	unsigned short   adc_bat;            // 电池ADC采样值
} BatteryManager_t;

/****************************************************************************
 * 压力控制结构体
 ****************************************************************************/
typedef struct {
	unsigned short   mod_seta_preh;      // 目标压力值（mmHg）
	unsigned short   mod_seta_prehh;     // 目标压力缓存
	unsigned short   mod_seta_prel;      // 上次设置的目标压力
	unsigned short   adc_ps00;           // 当前压力值（mmHg）
	unsigned short   adc_ps0;            // 压力ADC原始值
	unsigned short   adc_zero;           // 压力零点校准值
	float            valueK;             // 压力校准系数
	unsigned short   con_hi_delta;       // 上阈值增量
	unsigned short   con_lo_delta;       // 下阈值增量
} PressureControl_t;

/****************************************************************************
 * 气泵和阀门控制结构体
 ****************************************************************************/
typedef struct {
	unsigned char    open_bum;           // 气泵开启标志
	unsigned char    bum_dly;            // 泄气延迟计数器
	unsigned char    bum_dly_flg;        // 泄气延迟标志
	unsigned char    bump_need_out_air_flg; // 排气需求标志
	unsigned char    oppump_flg;         // 气泵运行标志
	unsigned char    pwm_cnt1;           // PWM计数器1
	unsigned char    pwm_cnt2;           // PWM占空比设定值
} PumpValveControl_t;

/****************************************************************************
 * 故障检测结构体
 ****************************************************************************/
typedef struct {
	unsigned char    err_codea;          // 错误代码A
	unsigned char    err_codeb;          // 错误代码B
	unsigned char    flager_a;           // 故障标志位寄存器
	unsigned short   load_perioda_up;    // 泄漏检测计时器
	unsigned short   cnt_cnta;           // 膨胀计数器
	unsigned short   cnt_cntaa;          // 膨胀时间累计器
	unsigned short   leak_cancel_counter; // 泄漏取消计数器
	unsigned long    scan_dusai_time;    // 堵塞检测时间累加器
	unsigned short   record_ds[8];       // 压力记录数组
	unsigned char    record_ds_turn;     // 压力记录位置
	unsigned short   twenty_seconds;     // 记录间隔计时器
} FaultDetector_t;

/****************************************************************************
 * 系统状态结构体
 ****************************************************************************/
typedef struct {
	unsigned char    mod_main_a;         // 当前工作模式
	unsigned char    mod_main_b;         // 备用模式
	unsigned char    mod_seta_cnt;       // UI设置计数器
	unsigned char    mod_jixa;           // 间歇模式子状态
	unsigned char    jx_current_phase;   // 间歇模式当前阶段
	unsigned short   jx_phase_delay;     // 间歇模式阶段延迟
	unsigned short   mod_tim_cnta;       // 模式计时器
	unsigned short   mod_tim_cnta2;      // 模式计时器2
	unsigned char    lock_flg;           // 锁定标志
	unsigned short   lock_cnt;           // 锁定计数器
	volatile unsigned short TK_TIME;     // 空闲超时计时器
	unsigned char    remember_mod;       // 记住的模式
} SystemState_t;

/****************************************************************************
 * 按键控制结构体
 ****************************************************************************/
typedef struct {
	unsigned char    key_val;            // 当前按键值
	unsigned char    key_val_bak;        // 按键备份值
	unsigned char    key_flg_l;          // 长按标志
	unsigned short   key_cnt;            // 按键计数器
	unsigned short   key_set_tim;        // 按键设置时间
	unsigned short   key_start_tim;      // 按键启动时间
	unsigned short   key_silent_flag;    // 按键静音配置标志
} KeyControl_t;

/****************************************************************************
 * 显示控制结构体
 ****************************************************************************/
typedef struct {
	unsigned short   back_led_cnt;       // 背光LED计数器
	unsigned char    language;           // 当前语言选择
} DisplayControl_t;

/****************************************************************************
 * Flash配置数据结构体
 ****************************************************************************/
typedef struct {
	unsigned short   dataREAD0;          // Flash读取数据0
	unsigned short   dataREAD1;          // Flash读取数据1
	unsigned short   dataREAD2;          // Flash读取数据2
	unsigned short   dataREAD3;          // Flash读取数据3
	unsigned short   dataK1;             // 压力校准系数K1
	unsigned short   dataK2;             // 压力校准系数K2
	unsigned short   dataK3;             // 压力校准系数K3
	unsigned short   dataK4;             // 压力校准系数K4
} FlashConfig_t;

/****************************************************************************
 * 系统控制标志结构体
 ****************************************************************************/
typedef struct {
	unsigned char    FLG_SYS_10MS;       // 主循环20ms标志
	unsigned char    overabc;            // 泄漏严重标志
	unsigned char    con_flg_falla;      // 补气控制标志
	unsigned char    fall_stata;         // 放气状态
	unsigned short   fall_cnta0;         // 放气计数器
	unsigned short   gao_cnt;            // 高压计数器
	unsigned short   BOX_FQ_TURNS;       // 补气状态持续计数
} SystemFlags_t;

/****************************************************************************
 * 定时器结构体（软件定时器）
 ****************************************************************************/
typedef struct {
	unsigned long    RecTickVal;         // 记录时刻值
	unsigned long    TimeOutVal;         // 超时时间点
	unsigned char    IsTimeOut;          // 超时标志
} TIMER;

#endif /* SYSTEM_TYPES_H */

