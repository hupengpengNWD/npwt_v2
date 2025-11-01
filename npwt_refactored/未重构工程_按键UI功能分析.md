# 未重构工程 - 按键触发UI界面切换功能分析

## 1. 系统架构概览

### 1.1 主循环结构（20ms周期）
```c
主循环流程：
├─ KEY_Scan()          // 按键扫描（5次采样去抖）
├─ MODE_ProA()         // 模式处理A：根据当前模式执行相应逻辑
├─ MODE_Pro()          // 模式处理：按键切换和关机控制
└─ DISP_MainA()        // 显示更新：根据当前模式刷新LCD显示
```

### 1.2 核心全局变量
- `mod_main_a`: 当前系统主状态（0-8）
- `mod_main_b`: 辅助状态（设置模式子状态）
- `mod_main_baka`: 工作模式备份（连续/间歇）
- `key_val`: 当前按键值（0表示无按键）
- `lock_flg`: 锁定标志（锁定后按键无效）

## 2. 系统状态（mod_main_a）定义

| 状态宏 | 值 | 说明 | 显示函数 |
|--------|-----|------|----------|
| `MOD_SYS` | 0 | 系统初始化模式 | 开机Logo/版本信息 |
| `MOD_WAT` | 1 | 等待指令模式（待机） | 主菜单界面 |
| `MOD_LIX` | 2 | 连续工作模式 | `DISP_LixA()` |
| `MOD_JIX` | 3 | 间歇工作模式 | `DISP_JixA()` |
| `MOD_SET` | 4 | 参数设定模式 | 设置界面 |
| `MOD_ZHT` | 5 | 暂停模式 | 暂停界面 |
| `MOD_ZXB` | 10 | 暂停模式（备用） | - |
| `MOD_OFF` | 7 | 关机模式 | - |
| `MOD_TK` | 8 | 空闲超时模式（5分钟） | - |

## 3. 按键定义

| 按键宏 | 值 | 说明 | 长按标志 |
|--------|-----|------|----------|
| `KEY_C` / `KEY_OK` | 0x38 | 确认键 | `KEY_OKL` (0xb8) |
| `KEY_UP` | 0x34 | 上键 | `KEY_UPL` (0xb4) |
| `KEY_DN` | 0x2c | 下键 | `KEY_DNL` (0xac) |
| `KEY_MUT` | 0x1c | 静音键 | - |

## 4. 按键处理流程

### 4.1 按键扫描（KEY_Scan）
```c
功能：
1. 5次采样去抖（key_cnt >= 5）
2. 检测长按（key_cnt >= JUDGE_LONG_PRESS_TIME）
3. 长按按键值 = key_val_bak + 0x80
4. 按键释放后，将key_val_bak赋值给key_val
```

### 4.2 模式处理（MODE_ProA）

#### MOD_SYS（初始化模式）
```c
流程：
1. 检测静音键长按 -> 切换静音标志
2. 初始化倒计时（7000ms）
   - 0-50ms: 检测启动键（KEY_C）按下
   - 50-52ms: 初始化LCD，启动系统
   - 5000-7000ms: 显示版本信息
   - 7000ms: 切换到MOD_WAT
```

#### MOD_WAT（待机模式）
```c
功能：
- 显示主菜单界面
- 等待用户选择进入设置或开始治疗

按键响应：
- KEY_UPL (上键长按) -> MOD_SET（进入设置模式）
- 启动键 -> 根据mod_main_baka切换到工作模式
```

#### MOD_SET（设置模式）
```c
子状态（mod_seta_cnt）：
- UI_WORKMODE_SELECT: 工作模式选择（连续/间歇）
- UI_MODE_SET_HI: 高压设置
- UI_JIX_MODE_SET_LO: 间歇模式低压设置
- UI_JIX_SET_HI_TIME: 间歇模式高压时间设置
- UI_JIX_SET_LO_TIME: 间歇模式低压时间设置

按键响应：
- KEY_UP/KEY_DN: 切换选项或调整数值
- KEY_UPL/KEY_DNL: 长按快速调整
- KEY_OK: 确认并进入下一项
- KEY_UPL: 返回上级或退出设置 -> MOD_ZHT
```

#### MOD_ZHT（暂停模式）
```c
显示内容：
- 显示目标压力值
- 显示"Therapy Off | Therapy"

按键响应：
- KEY_OK -> 返回工作模式（mod_main_baka）
- KEY_UPL -> MOD_SET（进入设置）
- 0x04 -> 切换定时器标志
```

#### MOD_ZXB（工作模式 - 暂停）
```c
按键响应：
- KEY_OK -> MOD_ZHT（进入暂停界面）
- KEY_UPL -> MOD_SET（进入设置）
```

#### MOD_LIX / MOD_JIX（工作模式）
```c
功能：
- 实时显示压力值
- 显示运行时间
- 显示"Therapy On"

按键响应（在工作模式下）：
- 长按语言切换键 -> 切换语言（仅在MOD_WAT有效）
- 解锁键 -> 解除锁定
```

### 4.3 模式切换（MODE_Pro）
```c
功能：
1. 处理按键解锁（UNLOCK_NPWT_KEYVAL）
2. 语言切换（仅在MOD_WAT，长按特定键）
3. 检测状态变化 -> DISP_Clear()清屏

关键逻辑：
if (mod_main_a != mod_bak_bak) {
    DISP_Clear();  // 状态变化时清屏
}
```

## 5. 显示更新流程（DISP_MainA）

### 5.1 显示函数分配
```c
switch (mod_main_a) {
    case MOD_SYS:  // 不显示（由MODE_ProA处理）
        break;
    case MOD_WAT:  // 显示主菜单
        DISP_key2015();  // 显示"Settings | Therapy"
        DISP_lx() / DISP_jx();  // 显示工作模式图标
        DISP_Dig12_16();  // 显示目标压力
        break;
    case MOD_LIX:  // 连续模式
        DISP_LixA();  // 显示治疗界面
        DISP_Info();  // 显示信息
        break;
    case MOD_JIX:  // 间歇模式
        DISP_JixA();  // 显示治疗界面
        DISP_Info();  // 显示信息
        break;
    case MOD_ZHT:  // 暂停模式
        // 显示目标压力 + "Therapy Off | Therapy"
        break;
    case MOD_SET:  // 设置模式
        // 根据mod_seta_cnt显示不同设置界面
        break;
}
```

### 5.2 显示更新特点
- **状态驱动**：每种状态有对应的显示函数
- **变化检测**：状态变化时自动清屏
- **多语言支持**：根据`language`变量显示中英文/俄文
- **实时更新**：每20ms刷新一次显示

## 6. 状态转换关系图

```
MOD_SYS (初始化)
    │ (7000ms后)
    ↓
MOD_WAT (待机)
    │
    ├─ KEY_UPL → MOD_SET (设置)
    │              │
    │              ├─ KEY_OK → 进入设置子项
    │              └─ KEY_UPL → MOD_ZHT (退出设置)
    │
    └─ 启动键 → MOD_LIX/MOD_JIX (工作模式)
                    │
                    ├─ KEY_OK → MOD_ZHT (暂停)
                    │              │
                    │              ├─ KEY_OK → 返回工作模式
                    │              └─ KEY_UPL → MOD_SET
                    │
                    └─ KEY_UPL → MOD_SET (设置)

MOD_ZHT (暂停)
    ├─ KEY_OK → 返回工作模式
    └─ KEY_UPL → MOD_SET
```

## 7. 关键设计特点

### 7.1 状态管理模式
- **单一主状态变量**：`mod_main_a`作为状态机核心
- **Switch-Case实现**：每个状态独立处理逻辑
- **状态备份机制**：`mod_main_baka`保存工作模式

### 7.2 按键处理特点
- **去抖机制**：5次采样确认
- **长按检测**：自动生成长按事件（+0x80）
- **锁定保护**：`lock_flg`保护关键状态
- **按键释放**：按键释放后才生效

### 7.3 UI显示特点
- **状态驱动显示**：根据状态自动切换界面
- **自动清屏**：状态变化时清屏
- **条件显示**：根据语言、错误码等条件显示

### 7.4 存在的问题
1. **代码耦合度高**：按键处理和状态逻辑混合
2. **状态转换分散**：分布在多个函数中
3. **难以扩展**：新增状态需要修改多处代码
4. **可读性差**：大量switch-case嵌套
5. **测试困难**：状态转换逻辑难以单独测试

## 8. 重构建议

### 8.1 使用FSM组件重构的优势
1. **状态转换表化**：明确定义状态转换关系
2. **事件驱动**：按键作为事件统一处理
3. **解耦合**：按键处理、状态管理、显示更新分离
4. **易扩展**：新增状态只需添加转换表项
5. **易测试**：可以单独测试状态转换逻辑

### 8.2 重构方案
```
FSM实例1：主状态机（mod_main_a）
├─ 状态：MOD_SYS, MOD_WAT, MOD_LIX, MOD_JIX, MOD_SET, MOD_ZHT...
├─ 事件：KEY_OK, KEY_UPL, KEY_DNL, KEY_UPL, 启动事件, 超时事件...
└─ 动作：状态进入/退出时的初始化、清屏、显示更新等

FSM实例2：设置状态机（mod_seta_cnt）
├─ 状态：UI_WORKMODE_SELECT, UI_MODE_SET_HI...
├─ 事件：KEY_OK, KEY_UP, KEY_DN...
└─ 动作：参数调整、界面刷新等
```

## 9. 总结

未重构工程的核心功能是通过**全局状态变量**（mod_main_a）和**按键值**（key_val）的组合，在多个switch-case分支中实现状态转换和界面切换。这种方式虽然功能完整，但代码结构复杂、难以维护和扩展。

使用FSM组件重构后，可以将这些分散的状态转换逻辑**表格化**，使状态机逻辑更加清晰、易于理解和维护。
