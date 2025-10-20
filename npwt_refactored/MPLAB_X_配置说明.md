# MPLAB X 项目配置说明

**新架构编译配置指南**  
**目标MCU**: PIC18F46J11  
**编译器**: XC8 v3.00  

---

## 📋 创建新项目步骤

### 1. 在MPLAB X中创建新项目

```
File → New Project
  → Microchip Embedded
    → Standalone Project
      → Device: PIC18F46J11
        → Tool: PICkit3 (或其他)
          → Compiler: XC8
            → Project Name: npwt_refactored
              → Location: [工程根目录]
```

### 2. 添加源文件

在项目窗口中，右键点击"源文件"：

**HAL层**（必需）:
```
HAL/Src/hal_gpio.c
HAL/Src/hal_adc.c
HAL/Src/hal_timer.c
```

**Drivers层**（必需）:
```
Drivers/Src/adc_driver.c
Drivers/Src/flash_driver.c (待实现)
```

**Middleware层**（必需）:
```
Middleware/Src/pressure_controller.c
Middleware/Src/battery_manager.c
Middleware/Src/fault_detector.c
```

**Application层**（必需）:
```
Application/Src/app_state_machine.c
```

**Core层**（必需）:
```
Core/Src/main.c
```

**字库文件**（从旧项目复制）:
```
../source/ziku/6X12.c
../source/ziku/8X16.c
../source/ziku/BIOS_ZK_DAW.c
../source/ziku/BIOS_ZK_X1.c
```

**LCD BIOS**（从旧项目复制）:
```
../source/BIOS_JLX1864G_139.c
```

### 3. 配置包含路径

右键项目 → Properties → XC8 Compiler → Preprocessing and messages

添加以下包含路径：
```
../npwt_refactored/Core/Inc
../npwt_refactored/HAL/Inc
../npwt_refactored/Drivers/Inc
../npwt_refactored/Middleware/Inc
../npwt_refactored/Application/Inc
../source/h
../source/ziku
```

### 4. 添加配置位文件

**方法1**: 创建新的config_bits.h
```c
// 复制 ../project/config_bits.h 到 Core/Inc/
```

**方法2**: 引用原有文件
```
在main.c中添加：
#include "../../../project/config_bits.h"
```

### 5. 编译器设置

**优化级别**: -O1 或 -O2  
**警告级别**: All warnings  
**其他选项**:
- [x] Enable all warnings
- [x] Treat warnings as errors (可选)

---

## 📝 快速配置方法

如果要快速测试新架构，可以复制现有项目：

```bash
# 1. 复制现有项目
cp -r project/npwt.X project/npwt_refactored.X

# 2. 修改 configurations.xml
# 将所有 ../../source/ 路径改为 ../../npwt_refactored/
```

---

## 🔧 Makefile配置（命令行编译）

如果想用命令行编译，创建简单的Makefile：

```makefile
# Makefile for npwt_refactored
CC = xc8
MCU = PIC18F46J11
CFLAGS = -O1 --warn=9

# 源文件
SOURCES = \
    Core/Src/main.c \
    HAL/Src/hal_gpio.c \
    HAL/Src/hal_adc.c \
    HAL/Src/hal_timer.c \
    Drivers/Src/adc_driver.c \
    Middleware/Src/pressure_controller.c \
    Middleware/Src/battery_manager.c \
    Middleware/Src/fault_detector.c \
    Application/Src/app_state_machine.c

# 包含路径
INCLUDES = \
    -I Core/Inc \
    -I HAL/Inc \
    -I Drivers/Inc \
    -I Middleware/Inc \
    -I Application/Inc

# 编译
all:
	$(CC) $(CFLAGS) $(INCLUDES) --chip=$(MCU) $(SOURCES) -o npwt_refactored.hex

clean:
	rm -f *.hex *.obj *.p1 *.d *.pre *.sym *.lst
```

---

## ⚠️ 注意事项

### 1. 缺少的模块

新架构目前未实现以下模块（需要从旧代码移植）：
- [ ] LCD驱动完整实现
- [ ] Flash驱动完整实现
- [ ] 按键扫描模块
- [ ] 报警管理器
- [ ] UI管理器
- [ ] 间歇模式详细逻辑

### 2. 依赖旧代码

新架构需要复用旧项目的：
- ✅ 字库文件（ziku/）
- ✅ LCD BIOS（BIOS_JLX1864G_139.c）
- ✅ 配置位（config_bits.h）

### 3. 头文件冲突

如果同时包含新旧架构的头文件，注意：
- 数据类型可能冲突（建议使用stdint.h的标准类型）
- 宏定义可能冲突（使用不同的前缀）

---

## 🚀 推荐使用方式

### 方案A：独立项目（推荐）

1. 创建新的MPLAB X项目
2. 只添加新架构的文件
3. 复用字库和配置位文件
4. 独立编译测试

**优势**: 干净独立，不影响旧代码

### 方案B：双配置

1. 在现有项目中添加新配置
2. 配置1：旧架构（default）
3. 配置2：新架构（refactored）
4. 可在两者间切换

**优势**: 一个项目，便于对比

### 方案C：仅作参考

1. 不编译新架构
2. 仅作为代码参考和学习
3. 逐步将设计思想应用到旧代码

**优势**: 风险最低

---

## 📖 配置示例

### configurations.xml 片段

```xml
<logicalFolder name="HeaderFiles" displayName="头文件" projectFiles="true">
  <!-- 新架构头文件 -->
  <itemPath>../../npwt_refactored/Core/Inc/system_config.h</itemPath>
  <itemPath>../../npwt_refactored/Core/Inc/system_types.h</itemPath>
  <itemPath>../../npwt_refactored/HAL/Inc/hal_gpio.h</itemPath>
  <itemPath>../../npwt_refactored/HAL/Inc/hal_adc.h</itemPath>
  <itemPath>../../npwt_refactored/HAL/Inc/hal_timer.h</itemPath>
  <!-- ... 更多头文件 -->
</logicalFolder>

<logicalFolder name="SourceFiles" displayName="源文件" projectFiles="true">
  <!-- 新架构源文件 -->
  <itemPath>../../npwt_refactored/Core/Src/main.c</itemPath>
  <itemPath>../../npwt_refactored/HAL/Src/hal_gpio.c</itemPath>
  <!-- ... 更多源文件 -->
</logicalFolder>

<sourceRootList>
  <Elem>../../npwt_refactored</Elem>
</sourceRootList>
```

---

## ✅ 配置检查清单

- [ ] 所有.c文件已添加到项目
- [ ] 所有.h文件已添加到项目
- [ ] 包含路径已正确配置
- [ ] config_bits.h已包含
- [ ] 字库文件已添加（如需LCD功能）
- [ ] 编译器设置正确（XC8，优化级别）
- [ ] 目标MCU正确（PIC18F46J11）
- [ ] 编程器已配置（PICkit3等）

---

## 🎯 后续工作

如需完整编译新架构，还需要：

1. **实现Flash驱动**
   ```c
   // Drivers/Src/flash_driver.c
   ```

2. **从旧代码移植LCD驱动**
   ```c
   // 创建 Drivers/Inc/lcd_driver.h
   // 创建 Drivers/Src/lcd_driver.c
   ```

3. **实现按键扫描**
   ```c
   // 创建 Drivers/Inc/key_driver.h
   // 创建 Drivers/Src/key_driver.c
   ```

4. **完善状态机逻辑**
   ```c
   // Application/Src/app_state_machine.c
   // 补充各状态的详细实现
   ```

---

**当前新架构可作为高质量参考，完整功能需要继续开发！** 📚

