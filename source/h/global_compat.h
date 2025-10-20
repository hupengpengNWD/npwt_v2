/****************************************************************************
 * 文件名: global_compat.h
 * 功能: 全局变量兼容层（宏定义映射）
 * 
 * 说明: 
 *   这是新架构的兼容层，用于平滑过渡
 *   旧代码使用的全局变量名被映射为新架构的结构体成员
 *   
 *   【重要】：
 *   1. 必须在system_manager.c之后include此文件
 *   2. 所有源文件都应include此文件以使用兼容层
 *   3. 底层已经是结构体访问，但对上层代码透明
 * 
 * 创建日期: 2025-10-19
 ****************************************************************************/

#ifndef GLOBAL_COMPAT_H
#define GLOBAL_COMPAT_H

#include "system_manager.h"

/* 声明全局结构体变量（定义在system_manager.c） */
extern AudioControl_t     g_audio;
extern BatteryManager_t   g_battery;
extern PressureControl_t  g_pressure;
extern PumpValveControl_t g_pump;
extern FaultDetector_t    g_fault;
extern SystemState_t      g_system;
extern KeyControl_t       g_key;
extern DisplayControl_t   g_display;
extern FlashConfig_t      g_flash_cfg;
extern SystemFlags_t      g_flags;

/****************************************************************************
 * 音频控制兼容层
 ****************************************************************************/
#define audio_flg       (g_audio.audio_flg)
#define audio_cnt       (g_audio.audio_cnt)
#define audio_basic     (g_audio.audio_basic)
#define audio_period    (g_audio.audio_period)
#define buz_flg         (g_audio.buz_flg)
#define buz_flg1        (g_audio.buz_flg1)
#define buz_cnt         (g_audio.buz_cnt)
#define mute_flg        (g_audio.mute_flg)
#define mute_tim        (g_audio.mute_tim)
#define SPEAK_flg       (g_audio.SPEAK_flg)
#define close_flg       (g_audio.close_flg)
#define SPK_STATE       (g_audio.SPK_STATE)
#define spk_selay       (g_audio.spk_selay)

/****************************************************************************
 * 电池管理兼容层
 ****************************************************************************/
#define bat_lev         (g_battery.bat_lev)
#define bat_lev_bak     (g_battery.bat_lev_bak)
#define bat_sas         (g_battery.bat_sas)
#define bat_sas_bak     (g_battery.bat_sas_bak)
#define bat_close_tim   (g_battery.bat_close_tim)
#define battery_status_flags (g_battery.battery_status_flags)
#define adc_bat         (g_battery.adc_bat)

/****************************************************************************
 * 压力控制兼容层
 ****************************************************************************/
#define mod_seta_preh   (g_pressure.mod_seta_preh)
#define mod_seta_prehh  (g_pressure.mod_seta_prehh)
#define mod_seta_prel   (g_pressure.mod_seta_prel)
#define adc_ps00        (g_pressure.adc_ps00)
#define adc_ps0         (g_pressure.adc_ps0)
#define adc_zero        (g_pressure.adc_zero)
#define valueK          (g_pressure.valueK)
#define con_hi_delta    (g_pressure.con_hi_delta)
#define con_lo_delta    (g_pressure.con_lo_delta)

/****************************************************************************
 * 气泵阀门兼容层
 ****************************************************************************/
#define open_bum        (g_pump.open_bum)
#define bum_dly         (g_pump.bum_dly)
#define bum_dly_flg     (g_pump.bum_dly_flg)
#define bump_need_out_air_flg (g_pump.bump_need_out_air_flg)
#define oppump_flg      (g_pump.oppump_flg)
#define pwm_cnt1        (g_pump.pwm_cnt1)
#define pwm_cnt2        (g_pump.pwm_cnt2)

/****************************************************************************
 * 故障检测兼容层
 ****************************************************************************/
#define err_codea       (g_fault.err_codea)
#define err_codeb       (g_fault.err_codeb)
#define flager_a        (g_fault.flager_a)
#define load_perioda_up (g_fault.load_perioda_up)
#define cnt_cnta        (g_fault.cnt_cnta)
#define cnt_cntaa       (g_fault.cnt_cntaa)
#define leak_cancel_counter (g_fault.leak_cancel_counter)
#define scan_dusai_time (g_fault.scan_dusai_time)
#define record_ds       (g_fault.record_ds)
#define record_ds_turn  (g_fault.record_ds_turn)
#define twenty_seconds  (g_fault.twenty_seconds)

/****************************************************************************
 * 系统状态兼容层
 ****************************************************************************/
#define mod_main_a      (g_system.mod_main_a)
#define mod_main_b      (g_system.mod_main_b)
#define mod_seta_cnt    (g_system.mod_seta_cnt)
#define mod_jixa        (g_system.mod_jixa)
#define jx_current_phase (g_system.jx_current_phase)
#define jx_phase_delay  (g_system.jx_phase_delay)
#define mod_tim_cnta    (g_system.mod_tim_cnta)
#define mod_tim_cnta2   (g_system.mod_tim_cnta2)
#define lock_flg        (g_system.lock_flg)
#define lock_cnt        (g_system.lock_cnt)
#define TK_TIME         (g_system.TK_TIME)
#define remember_mod    (g_system.remember_mod)

/****************************************************************************
 * 按键控制兼容层
 ****************************************************************************/
#define key_val         (g_key.key_val)
#define key_val_bak     (g_key.key_val_bak)
#define key_flg_l       (g_key.key_flg_l)
#define key_cnt         (g_key.key_cnt)
#define key_set_tim     (g_key.key_set_tim)
#define key_start_tim   (g_key.key_start_tim)
#define key_silent_flag (g_key.key_silent_flag)

/****************************************************************************
 * 显示控制兼容层
 ****************************************************************************/
#define back_led_cnt    (g_display.back_led_cnt)
#define language        (g_display.language)

/****************************************************************************
 * Flash配置兼容层
 ****************************************************************************/
#define dataREAD0       (g_flash_cfg.dataREAD0)
#define dataREAD1       (g_flash_cfg.dataREAD1)
#define dataREAD2       (g_flash_cfg.dataREAD2)
#define dataREAD3       (g_flash_cfg.dataREAD3)
#define dataK1          (g_flash_cfg.dataK1)
#define dataK2          (g_flash_cfg.dataK2)
#define dataK3          (g_flash_cfg.dataK3)
#define dataK4          (g_flash_cfg.dataK4)

/****************************************************************************
 * 系统标志兼容层
 ****************************************************************************/
#define FLG_SYS_10MS    (g_flags.FLG_SYS_10MS)
#define overabc         (g_flags.overabc)
#define con_flg_falla   (g_flags.con_flg_falla)
#define fall_stata      (g_flags.fall_stata)
#define fall_cnta0      (g_flags.fall_cnta0)
#define gao_cnt         (g_flags.gao_cnt)
#define BOX_FQ_TURNS    (g_flags.BOX_FQ_TURNS)

#endif /* GLOBAL_COMPAT_H */

