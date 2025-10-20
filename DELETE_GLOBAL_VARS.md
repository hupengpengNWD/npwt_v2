# 真正的问题：全局变量定义未删除

## 问题分析

我之前的重构**不彻底**：

### 已完成（但不够）
1. ✅ 创建了 system_manager.c 定义结构体
2. ✅ 创建了 global_compat.h 提供兼容层
3. ✅ 删除了头文件中的 extern 声明

### **关键遗漏**
❌ **没有删除源文件中实际的全局变量定义！**

## 当前问题

现在存在**双重定义**：

```c
// system_manager.c（新架构）
AudioControl_t g_audio;  // 新定义

// npwt_con_main.c 或其他文件（旧代码 - 仍然存在！）
unsigned char audio_flg;  // 旧定义 ← 这个应该删除！
unsigned short audio_cnt; // 旧定义 ← 这个应该删除！
```

## 解决方案

需要删除所有源文件中的全局变量定义，只保留：
1. 函数定义
2. 局部变量
3. static局部变量
4. const常量

## 后续操作

立即执行真正的清理！
