/****************************************************************************
 * 文件名: pwm.c
 * 功能: PWM波形生成模块实现（中间件层）
 * 作者: 韦睿医疗
 * 说明: 
 *   基于Timer3的1ms中断回调实现PWM波形生成
 *   通过GPIO翻转控制输出PWM信号
 *   参考老版本工程的PWM实现方式
 * 
 * 创建日期: 2025-10-29
 ****************************************************************************/

#include "../Inc/pwm.h"
#include "../../HAL/Inc/hal_gpio.h"
#include "../../HAL/Inc/hal_timer.h"

/****************************************************************************
 * 内部变量
 ****************************************************************************/

/* PWM状态 */
static bool g_pwm_enabled = false;           // PWM是否启用

/* PWM占空比（0-1000，对应0%-100%） */
static uint16_t g_pwm_duty = 0;              // 当前占空比

/* PWM周期计数器（0-9，对应10ms周期） */
static uint8_t g_pwm_counter = 0;            // 当前周期内的计数器

/****************************************************************************
 * 内部函数声明
 ****************************************************************************/

/**
 * @name      PWM_ProcessCallback
 * @brief     PWM处理回调函数（在Timer3的1ms中断中调用）
 * @param     无
 * @retval    无
 * @note      在HAL_Timer3_RegisterCallback_1ms中注册
 */
static void PWM_ProcessCallback(void);

/****************************************************************************
 * 函数实现
 ****************************************************************************/

/**
 * @name      PWM_Init
 * @brief     初始化PWM模块
 * @note      注册Timer3的1ms回调，初始化内部变量
 */
void PWM_Init(void)
{
    /* 初始化变量 */
    g_pwm_enabled = false;
    g_pwm_duty = 0;
    g_pwm_counter = 0;
    
    /* 注册Timer3的1ms回调函数 */
    HAL_Timer3_RegisterCallback_1ms(PWM_ProcessCallback);
    
    /* 确保GPIO初始为低电平（气泵停止） */
    HAL_Pump_Stop();
}

/**
 * @name      PWM_Start
 * @brief     启动PWM输出
 * @note      开始PWM波形生成，计数器从0开始
 */
void PWM_Start(void)
{
    g_pwm_enabled = true;
    g_pwm_counter = 0;  // 重置计数器，从新周期开始
    
    /* 如果占空比>0，立即输出高电平 */
    if (g_pwm_duty > 0) {
        HAL_Pump_Start();
    } else {
        HAL_Pump_Stop();
    }
}

/**
 * @name      PWM_Stop
 * @brief     停止PWM输出
 * @note      停止PWM波形生成，GPIO输出低电平
 */
void PWM_Stop(void)
{
    g_pwm_enabled = false;
    g_pwm_counter = 0;
    
    /* 强制输出低电平（气泵停止） */
    HAL_Pump_Stop();
}

/**
 * @name      PWM_SetDuty
 * @brief     设置PWM占空比
 * @param     duty - 占空比值（0-1000，对应0%-100%）
 * @note      duty=0: 0%占空比，duty=1000: 100%占空比
 *            duty=500: 50%占空比
 */
void PWM_SetDuty(uint16_t duty)
{
    /* 边界检查 */
    if (duty > PWM_DUTY_MAX) {
        duty = PWM_DUTY_MAX;
    }
    
    g_pwm_duty = duty;
    
    /* 如果PWM正在运行，立即更新输出状态 */
    if (g_pwm_enabled) {
        /* 计算当前计数器对应的输出状态 */
        uint8_t threshold = (uint8_t)((duty * PWM_PERIOD_MS) / PWM_DUTY_MAX);
        
        if (g_pwm_counter < threshold) {
            HAL_Pump_Start();
        } else {
            HAL_Pump_Stop();
        }
    }
}

/**
 * @name      PWM_GetDuty
 * @brief     获取当前PWM占空比
 * @retval    当前占空比值（0-1000）
 */
uint16_t PWM_GetDuty(void)
{
    return g_pwm_duty;
}

/**
 * @name      PWM_IsRunning
 * @brief     查询PWM是否正在运行
 * @retval    true=运行中, false=已停止
 */
bool PWM_IsRunning(void)
{
    return g_pwm_enabled;
}

/****************************************************************************
 * 内部函数实现
 ****************************************************************************/

/**
 * @name      PWM_ProcessCallback
 * @brief     PWM处理回调函数（在Timer3的1ms中断中调用）
 * @note      每1ms调用一次，维护PWM周期和占空比
 * 
 * 算法：
 *   - PWM周期：10ms（计数器0-9）
 *   - 占空比阈值：threshold = (duty * 10) / 1000
 *   - 如果 counter < threshold，输出高电平
 *   - 否则输出低电平
 * 
 * 示例：
 *   - duty=500（50%）：threshold=5，counter<5时高电平
 *   - duty=1000（100%）：threshold=10，始终高电平
 *   - duty=0（0%）：threshold=0，始终低电平
 */
static void PWM_ProcessCallback(void)
{
    /* 如果PWM未启用，直接返回 */
    if (!g_pwm_enabled) {
        return;
    }
    
    /* 计算占空比阈值（0-10） */
    /* 公式：threshold = (duty * PWM_PERIOD_MS) / PWM_DUTY_MAX */
    uint8_t threshold = (uint8_t)((g_pwm_duty * PWM_PERIOD_MS) / PWM_DUTY_MAX);
    
    /* 根据当前计数器和阈值决定输出状态 */
    if (g_pwm_counter < threshold) {
        /* 在高电平期间 */
        HAL_Pump_Start();
    } else {
        /* 在低电平期间 */
        HAL_Pump_Stop();
    }
    
    /* 更新计数器 */
    g_pwm_counter++;
    
    /* 如果计数器达到周期长度，重置计数器（开始新的周期） */
    if (g_pwm_counter >= PWM_PERIOD_MS) {
        g_pwm_counter = 0;
    }
}

