/****************************************************************************
 * 文件名: app_selftest.h
 * 功能: 应用层自检模块
 * 
 * 说明: 
 *   实现系统自检功能
 *   - 传感器测试
 *   - 气泵测试
 *   - 阀门测试
 * 
 * 创建日期: 2025-10-20
 ****************************************************************************/

#ifndef APP_SELFTEST_H
#define APP_SELFTEST_H

#include "../../Core/Inc/system_types.h"

/**
 * 函数: AppSelftest_Start
 * 功能: 启动自检
 */
void AppSelftest_Start(SystemState_t *sys);

/**
 * 函数: AppSelftest_Run
 * 功能: 运行自检（主循环调用）
 * 返回: true=自检完成, false=进行中
 */
bool AppSelftest_Run(SystemState_t *sys);

/**
 * 函数: AppSelftest_Stop
 * 功能: 停止自检
 */
void AppSelftest_Stop(SystemState_t *sys);


#endif /* APP_SELFTEST_H */

