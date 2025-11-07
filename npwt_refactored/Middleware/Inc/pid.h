 /****************************************************************************
  * 文件名: pid.h
  * 功能:   通用 PID 控制器接口（中间件层）
  *
  * 说明:
  *   - 采用离散化（位置式）PID 算法
  *   - 支持输出限幅、积分限幅、采样周期设置
  *   - 可用于泵电机、温度等需要闭环控制的场景
  *
  * 创建日期: 2025-11-07
  ****************************************************************************/

#ifndef PID_H
#define PID_H

#include <stdbool.h>

typedef struct {
    float kp;                 /* 比例系数，用于放大当前误差 */
    float ki;                 /* 积分系数（每秒），用于消除稳态误差 */
    float kd;                 /* 微分系数（每秒），用于抑制误差变化 */

    float sample_time;        /* 采样周期（秒），决定积分/微分的缩放 */

    float integral;           /* 当前积分项累计值 */
    float prev_error;         /* 上一次误差，用于计算微分 */
    bool first_update;        /* 首次更新标志，避免微分突变 */

    float output_min;         /* 控制输出下限 */
    float output_max;         /* 控制输出上限 */

    float integral_min;       /* 积分限幅下限，防止积分饱和 */
    float integral_max;       /* 积分限幅上限，防止积分饱和 */
} PIDController_t;

/**
 * @brief  初始化 PID 控制器
 * @param  pid          控制器实例指针
 * @param  kp/ki/kd     比例、积分、微分系数
 * @param  sample_time  采样周期 (秒)
 */
void PID_Init(PIDController_t* pid, float kp, float ki, float kd, float sample_time);

/**
 * @brief  运行时更新 PID 系数
 */
void PID_SetTunings(PIDController_t* pid, float kp, float ki, float kd);

/**
 * @brief  设置采样周期（影响积分/微分计算）
 */
void PID_SetSampleTime(PIDController_t* pid, float sample_time);

/**
 * @brief  设置输出限幅，防止控制量越界
 */
void PID_SetOutputLimits(PIDController_t* pid, float min_output, float max_output);

/**
 * @brief  设置积分限幅，抑制积分饱和
 */
void PID_SetIntegralLimits(PIDController_t* pid, float min_integral, float max_integral);

/**
 * @brief  重置内部状态（积分、微分记忆）
 */
void PID_Reset(PIDController_t* pid);

/**
 * @brief  计算 PID 输出
 * @param  setpoint    目标值
 * @param  measurement 当前测量值
 * @return 控制输出（已限幅）
 */
float PID_Update(PIDController_t* pid, float setpoint, float measurement);

#endif /* PID_H */
