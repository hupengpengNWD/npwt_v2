/****************************************************************************
 * 文件名: pwm.h
 * 功能: PWM波形生成模块（中间件层）
 * 作者: 韦睿医疗
 * 说明: 
 *   基于Timer3的1ms中断回调实现PWM波形生成
 *   通过GPIO翻转控制输出PWM信号
 *   参考老版本工程的PWM实现方式
 * 
 * 实现原理：
 *   - PWM周期：10ms（100Hz，与老版本工程一致）
 *   - 占空比：0-1000对应0-100%（0=0%, 1000=100%）
 *   - 在1ms定时器回调中维护计数器，控制GPIO翻转
 *   - 使用HAL_Pump_Enable/HAL_Pump_Stop控制气泵GPIO
 * 
 * 创建日期: 2025-10-22
 ****************************************************************************/

#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <stdbool.h>

/****************************************************************************
 * 常量定义
 ****************************************************************************/

#define PWM_PERIOD_MS             10      // PWM周期：10ms（100Hz）
#define PWM_DUTY_MAX              1000   // 最大占空比值（对应100%）
#define PWM_DUTY_MIN              0       // 最小占空比值（对应0%）

/****************************************************************************
 * PWM管理接口函数
 ****************************************************************************/

/**
 * @name      PWM_Init
 * @brief     初始化PWM模块
 * @param     无
 * @retval    无
 * @note      注册Timer3的1ms回调，初始化内部变量
 */
void PWM_Init(void);

/**
 * @name      PWM_Start
 * @brief     启动PWM输出
 * @param     无
 * @retval    无
 * @note      开始PWM波形生成，计数器从0开始
 */
void PWM_Start(void);

/**
 * @name      PWM_Stop
 * @brief     停止PWM输出
 * @param     无
 * @retval    无
 * @note      停止PWM波形生成，GPIO输出低电平
 */
void PWM_Stop(void);

/**
 * @name      PWM_SetDuty
 * @brief     设置PWM占空比
 * @param     duty - 占空比值（0-1000，对应0%-100%）
 * @retval    无
 * @note      duty=0: 0%占空比，duty=1000: 100%占空比
 *            duty=500: 50%占空比
 */
void PWM_SetDuty(uint16_t duty);

/**
 * @name      PWM_GetDuty
 * @brief     获取当前PWM占空比
 * @param     无
 * @retval    当前占空比值（0-1000）
 */
uint16_t PWM_GetDuty(void);

/**
 * @name      PWM_IsRunning
 * @brief     查询PWM是否正在运行
 * @param     无
 * @retval    true=运行中, false=已停止
 */
bool PWM_IsRunning(void);

#endif /* PWM_H */

