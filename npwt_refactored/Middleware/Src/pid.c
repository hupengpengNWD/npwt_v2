#include "../Inc/pid.h"

#include <stddef.h>

/**
 * @brief  浮点限幅工具函数
 * @note   由于后续多次使用，因此抽象成静态函数
 */
static float clampf(float value, float min_value, float max_value)
{
    if (value > max_value) {
        return max_value;
    }
    if (value < min_value) {
        return min_value;
    }
    return value;
}

/**
 * @brief PID 控制器初始化
 *
 * - 保存比例/积分/微分系数
 * - 设定采样周期（若<=0 则回退到 10ms）
 * - 清零积分项、历史误差等内部状态
 * - 默认设置输出与积分限幅，防止越界
 */
void PID_Init(PIDController_t* pid, float kp, float ki, float kd, float sample_time)
{
    if (pid == NULL) {
        return;
    }

    /* 保存 PID 参数 */
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;

    /* 避免采样周期为 0，默认回退到 10ms */
    pid->sample_time = (sample_time > 0.0f) ? sample_time : 0.01f;

    /* 清零内部状态 */
    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->first_update = true;

    /* 默认输出 / 积分限幅范围，可后续重新设置 */
    pid->output_min = -100.0f;
    pid->output_max = 100.0f;
    pid->integral_min = -100.0f;
    pid->integral_max = 100.0f;
}

/**
 * @brief 运行时更新 PID 系数
 *
 * 常用于在线调参或根据工况动态调整 PID 权重。
 */
void PID_SetTunings(PIDController_t* pid, float kp, float ki, float kd)
{
    if (pid == NULL) {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

/**
 * @brief 设置采样周期
 *
 * 修改采样周期会影响积分/微分项的缩放，需和外部调度周期保持一致。
 */
void PID_SetSampleTime(PIDController_t* pid, float sample_time)
{
    if (pid == NULL || sample_time <= 0.0f) {
        return;
    }

    pid->sample_time = sample_time;
}

/**
 * @brief 设置输出限幅
 *
 * 防止控制量超过执行器能力或造成振荡，必须保证 max > min。
 */
void PID_SetOutputLimits(PIDController_t* pid, float min_output, float max_output)
{
    if (pid == NULL || max_output <= min_output) {
        return;
    }

    pid->output_min = min_output;
    pid->output_max = max_output;
}

/**
 * @brief 设置积分项限幅
 *
 * 用于抑制积分饱和（wind-up），限制积分累积范围。
 */
void PID_SetIntegralLimits(PIDController_t* pid, float min_integral, float max_integral)
{
    if (pid == NULL || max_integral <= min_integral) {
        return;
    }

    pid->integral_min = min_integral;
    pid->integral_max = max_integral;
}

/**
 * @brief 重置 PID 内部状态
 *
 * 在停机、切换工况或误差较大时调用，清除积分、历史误差，防止旧状态影响新控制。
 */
void PID_Reset(PIDController_t* pid)
{
    if (pid == NULL) {
        return;
    }

    pid->integral = 0.0f;
    pid->prev_error = 0.0f;
    pid->first_update = true;
}

/**
 * @brief 计算一次 PID 输出
 *
 * @param pid         PID 控制器实例
 * @param setpoint    目标值
 * @param measurement 当前测量值
 * @return 限幅后的控制输出
 *
 * 算法步骤：
 * 1. 计算误差与比例项
 * 2. 积分项累加并限幅
 * 3. 微分项基于误差变化率，首次更新跳过
 * 4. 三项求和并按输出限幅裁剪
 */
float PID_Update(PIDController_t* pid, float setpoint, float measurement)
{
    if (pid == NULL) {
        return 0.0f;
    }

    /* 计算当前误差 */
    float error = setpoint - measurement;
    float proportional = pid->kp * error;

    /* 积分项：误差累加并做限幅，避免积分飘逸 */
    pid->integral += pid->ki * error * pid->sample_time;
    pid->integral = clampf(pid->integral, pid->integral_min, pid->integral_max);

    /* 微分项：首次更新不做微分，防止突变 */
    float derivative;
    if (pid->first_update) {
        derivative = 0.0f;
        pid->first_update = false;
    } else {
        derivative = pid->kd * (error - pid->prev_error) / pid->sample_time;
    }
    pid->prev_error = error;

    /* 计算最终输出并做限幅 */
    float output = proportional + pid->integral + derivative;
    output = clampf(output, pid->output_min, pid->output_max);

    return output;
}
