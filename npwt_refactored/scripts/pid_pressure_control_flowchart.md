# PID建立负压算法流程图

## 算法概述

本算法使用PID控制器建立和维持负压，通过PWM控制泵电机和电磁阀来实现精确的压力控制。

## 主要参数

- **PID参数**: Kp=1.0, Ki=0.12, Kd=0.00
- **采样周期**: 10ms
- **输出范围**: -100 ~ +100（正值抽气，负值泄气）
- **死区**: 目标值 ~ 目标值+5mmHg
- **目标压力范围**: 20-300mmHg

## 流程图（Mermaid格式）

```mermaid
flowchart TD
    Start([启动压力控制]) --> Init1[限制目标压力范围<br/>20-300mmHg]
    Init1 --> Init2[应用偏移量调整目标]
    Init2 --> Init3[重置PID控制器]
    Init3 --> Init4[记录建立负压开始时间]
    Init4 --> Init5[设置泵工作原因为BUILDING]
    Init5 --> Init6[重置所有报警标志]
    Init6 --> Loop{控制已启用?}
    
    Loop -->|是| GetPressure[获取当前压力值]
    GetPressure --> CalcError[计算误差<br/>error = target - current]
    CalcError --> CalcAbsError[计算误差绝对值]
    CalcAbsError --> CheckHold{是否在保持状态?}
    
    CheckHold -->|是| CalcReengage[计算重新抽气阈值<br/>target - 5mmHg]
    CalcReengage --> CheckReengage{压力 <= 阈值?}
    CheckReengage -->|是| ExitHold[退出保持状态<br/>重置PID<br/>设置MAINTAINING]
    CheckReengage -->|否| CheckBlockage[检测管路堵塞报警]
    CheckBlockage --> CheckOverpressure[检测过压报警]
    CheckOverpressure --> Return1[返回]
    ExitHold --> CheckDeadband
    
    CheckHold -->|否| CheckDeadband{达到死区?<br/>error <= 0 且<br/>current >= target+5mmHg}
    
    CheckDeadband -->|是| ResetPID[重置PID<br/>停止泵电机]
    ResetPID --> EnterHold[进入保持状态]
    EnterHold --> FirstHold{首次进入?}
    FirstHold -->|是| StartBlockage[启动管路堵塞报警检测]
    StartBlockage --> CheckOver1[检查过压条件1<br/>压力 > target+15mmHg]
    CheckOver1 --> CheckOver2[检查过压条件2<br/>建立时间 < 阈值]
    CheckOver2 --> ClearLeak[清除泄漏报警检测]
    FirstHold -->|否| ClearLeak
    ClearLeak --> Return2[返回]
    
    CheckDeadband -->|否| ClearHold[清除保持状态标志]
    ClearHold --> LeakCheck[泄漏报警超时检测]
    LeakCheck --> LeakTimeout{超时且压力<br/>在1-15mmHg?}
    LeakTimeout -->|是| LeakCount{连续5次<br/>在区间内?}
    LeakCount -->|是| TriggerLeak[触发泄漏报警]
    LeakCount -->|否| PIDCalc
    LeakTimeout -->|否| PIDCalc
    
    PIDCalc[执行PID计算] --> PIDDetails[PID算法:<br/>P = Kp × error<br/>I += Ki × error × dt<br/>D = Kd × (error - prev_error) / dt<br/>output = P + I + D<br/>限幅: -100 ~ +100]
    PIDDetails --> SaveOutput[保存PID输出]
    SaveOutput --> BlockageCheck[管路堵塞报警检测]
    BlockageCheck --> SelectDuty[根据误差选择<br/>动态最小占空比]
    SelectDuty --> ApplyOutput[应用PID输出]
    
    ApplyOutput --> CheckOutput{PID输出 > 0?}
    CheckOutput -->|是| Normalize[归一化输出到0~1]
    Normalize --> CalcDuty[计算PWM占空比<br/>= 最小占空比 + 剩余占空比 × 归一化值]
    CalcDuty --> SetPWM[设置PWM占空比]
    SetPWM --> StartPWM[启动PWM]
    StartPWM --> CloseValve[关闭阀门1和2]
    CloseValve --> Wait1[等待10ms]
    
    CheckOutput -->|否| StopPWM[停止PWM<br/>占空比 = 0]
    StopPWM --> CheckRelease{负输出幅度<br/>>= 阈值?}
    CheckRelease -->|是| OpenValve[打开阀门1和2]
    CheckRelease -->|否| CloseValve2[关闭阀门1和2]
    OpenValve --> Wait1
    CloseValve2 --> Wait1
    
    Wait1 --> Loop
    Return1 --> Wait1
    Return2 --> Wait1
    TriggerLeak --> PIDCalc
    
    Loop -->|否| Stop([停止])
    
    style Start fill:#90EE90
    style Stop fill:#FFB6C1
    style PIDCalc fill:#87CEEB
    style CheckDeadband fill:#FFD700
    style EnterHold fill:#DDA0DD
```

## 详细步骤说明

### 1. 初始化阶段（AppPressure_StartControl）

1. **限制目标压力范围**: 将用户输入的目标压力限制在20-300mmHg范围内
2. **应用偏移量**: 根据系统特性调整目标压力（内部目标 = 用户目标 + 偏移量）
3. **重置PID**: 清除PID的积分项和上一次误差
4. **记录开始时间**: 记录建立负压的开始时间，用于液位满报警检测
5. **设置工作原因**: 标记为`PUMP_REASON_BUILDING`（建立负压）
6. **重置报警标志**: 清除所有报警相关标志

### 2. 主循环（AppPressure_Process，每10ms执行一次）

#### Step1: 控制启用检查
- 如果控制未启用，直接返回

#### Step2: 获取当前压力
- 读取经过滤波和零点校准的压力值（mmHg）

#### Step3: 安全保护（已注释）
- ADC读取失败检测
- 过压保护（>320mmHg）

#### Step4: 计算误差
- `error = target - current_pressure`
- `abs_error = |error|`
- 正值表示需要继续抽气，负值表示压力已超出目标

#### Step4.5: 保持状态检查
- 如果在保持状态且压力下降到重新抽气阈值以下，退出保持状态并重新抽气

#### Step5: 死区判断
- **条件**: `error <= 0` 且 `current_pressure >= target + 5mmHg`
- **动作**:
  - 重置PID
  - 停止泵电机（output = 0）
  - 进入保持状态
  - 首次进入时检查过压报警（条件1和条件2）

#### Step5.5: 泄漏报警检测
- 如果未达到目标压力，检查是否超时
- 超时后检查压力是否在1-15mmHg区间内
- 连续5次在区间内则触发泄漏报警

#### Step6: PID计算
- 调用`PID_Update()`计算控制输出
- 输出范围：-100 ~ +100

#### Step6.5: 管路堵塞报警检测
- 检查PID输出是否大于阈值（说明有负压补充）
- 如果超时且没有负压补充，触发堵塞报警

#### Step7: 应用输出
- 根据误差选择动态最小占空比
- 调用`PressureControl_ApplyOutput()`驱动执行器

### 3. PID算法（PID_Update）

1. **比例项**: `P = Kp × error`
2. **积分项**: `I += Ki × error × dt`（带限幅，防止积分饱和）
3. **微分项**: `D = Kd × (error - prev_error) / dt`（首次更新跳过）
4. **输出**: `output = P + I + D`（限幅到-100 ~ +100）

### 4. 输出应用（PressureControl_ApplyOutput）

#### 正输出（需要抽气）
1. 归一化输出到0~1范围
2. 计算PWM占空比 = 最小占空比 + 剩余占空比 × 归一化值
3. 设置PWM占空比并启动PWM
4. 关闭阀门1和2（保持管路密闭）

#### 非正输出（需要泄气或保压）
1. 停止PWM（占空比 = 0）
2. 如果负输出幅度 >= 阈值：打开阀门1和2（泄气）
3. 否则：关闭阀门1和2（保压）

## 关键特性

1. **动态最小占空比**: 根据误差大小动态调整最小PWM占空比，避免泵进入"嗡嗡不抽"区间
2. **死区控制**: 达到目标+5mmHg时停止泵，防止超调
3. **保持状态**: 压力稳定后进入保持状态，减少能耗
4. **自动恢复**: 保持状态下压力下降时自动重新抽气
5. **多重报警**: 集成泄漏报警、过压报警、堵塞报警等多种安全检测

## 相关文件

- `npwt_refactored/Application/Src/app_pressure.c`: 主控制逻辑
- `npwt_refactored/Middleware/Src/pid.c`: PID算法实现
- `npwt_refactored/Core/Inc/system_config.h`: 系统配置参数

