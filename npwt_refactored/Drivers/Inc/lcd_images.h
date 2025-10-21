/**
 * @file    lcd_images.h
 * @brief   LCD图片数据 - Logo和图标
 * @date    2025-10-20
 * 
 * 包含内容：
 *   - 开机Logo图片（128x64）
 *   - 版本信息图片（128x64）
 *   - 电池图标5个（24x16，0格-4格电量）
 */

#ifndef LCD_IMAGES_H
#define LCD_IMAGES_H

#include <stdint.h>

/*===========================================================================
 * Logo和版本信息图片
 *===========================================================================*/

/**
 * @brief 开机Logo图片数据
 * @note  尺寸：128x64像素，8页，每页128字节，共1024字节
 */
extern const uint8_t LOGO_STARTUP_IMAGE[1024];

/**
 * @brief 版本信息图片数据
 * @note  尺寸：128x64像素，8页，每页128字节，共1024字节
 */
extern const uint8_t LOGO_VERSION_IMAGE[1024];

/*===========================================================================
 * 电池图标
 *===========================================================================*/

#define BATTERY_ICON_WIDTH   24   // 电池图标宽度（像素）
#define BATTERY_ICON_HEIGHT  16   // 电池图标高度（像素）
#define BATTERY_ICON_SIZE    48   // 电池图标大小（字节）= 24 * 16 / 8 = 48

/**
 * @brief 电池图标数据（5个等级）
 * @note  每个图标尺寸：24x16像素，2页，每页24字节，共48字节
 */
extern const uint8_t BATTERY_ICON_0[BATTERY_ICON_SIZE];  // 空电池（0%）
extern const uint8_t BATTERY_ICON_1[BATTERY_ICON_SIZE];  // 1格电（25%）
extern const uint8_t BATTERY_ICON_2[BATTERY_ICON_SIZE];  // 2格电（50%）
extern const uint8_t BATTERY_ICON_3[BATTERY_ICON_SIZE];  // 3格电（75%）
extern const uint8_t BATTERY_ICON_4[BATTERY_ICON_SIZE];  // 4格电（100%）

/*===========================================================================
 * 辅助函数
 *===========================================================================*/

/**
 * @brief 根据电量百分比获取电池图标指针
 * @param battery_percent 电量百分比（0-100）
 * @return 对应的电池图标数据指针
 */
const uint8_t* LCD_GetBatteryIcon(uint8_t battery_percent);

#endif // LCD_IMAGES_H

