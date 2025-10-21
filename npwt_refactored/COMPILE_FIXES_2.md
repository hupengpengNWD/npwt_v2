# 编译错误修复清单（第2轮）

本轮修复了以下编译错误：

## 修改文件清单

1. **Application/Src/ui_enhancements.c**
   - 添加 `#include "../../Core/Inc/mcu_config.h"` 解决 NULL 未定义

2. **Application/Src/app_input.c**
   - 添加 `#include "../../Middleware/Inc/alarm_manager.h"` 解决 AlarmManager_Beep 未声明

3. **Application/Src/app_settings.c**
   - 添加 `#include "../../Drivers/Inc/flash_driver.h"`
   - 添加 `#include "../Inc/app_state_machine.h"`
   - 添加前向声明 `static void AppSettings_HandleKeyLongPress(...);`
   - 修复：`SETTING_INTERMITTENT_LOW_VALUE` → `SETTING_INTERMITTENT_LOW_PRESSURE`
   - 修复：`Flash_SaveSystemSettings(&sys->flash_config)` → `Flash_SaveSystemSettings(sys)`

4. **Application/Src/app_state_machine.c**
   - 添加 `#include "../../Drivers/Inc/flash_driver.h"`
   - 添加 `#include "../../Middleware/Inc/alarm_manager.h"`
   - 修复：所有 `state->mode` → `state->current_mode`
   - 修复：`state->fault.active_error` → `state->fault.current_error`
   - 修复：`pump->is_enabled` → `state->pump.is_enabled`

5. **Core/Src/main.c**
   - 修复：`Key_Init()` → `Key_Init(&g_key_data)`
   - 修复：`AlarmManager_Init()` → `AlarmManager_Init(&g_system.alarm)`
   - 修复：`void interrupt ISR(void)` → `void __interrupt() ISR(void)`

## 需要同步到 MPLAB X 的文件

请将以下5个文件从 `npwt_refactored/` 覆盖到 MPLAB X 工程：

1. Application/Src/ui_enhancements.c
2. Application/Src/app_input.c  
3. Application/Src/app_settings.c
4. Application/Src/app_state_machine.c
5. Core/Src/main.c

