按
设定参数
开始治疗
运行中
停机中
连续模式
间歇模式
模式设定
切换
压力设置
高压
低压
间歇模式时间设置
运行时间
停止时间
按设定参数开始治疗中机连续模式间歇设切换高压低压时间运行停止


static void AppUI_Display_SYS(void)
{
#define SW 0
    if (g_ui_context.sys_show_logo) {
        // 显示开机Logo（由display模块处理）
        Display_ShowStartupInterface();
    } else {
        // 显示"npwt"和版本号
        Display_Clear();
        // 16x32字体需要4页（32像素），屏幕总共8页（0-7）
        // Y=2时，page_hw=4，占用硬件页4,5,6,7（完整显示）
        // 6x12字体需要2页（12像素），Y=5时，page_hw=1，占用硬件页1,2（不重叠）
#if SW
        static char text_buffer[32] = {0};
        Display_ShowString(40, 0, AppLanguage_GetTextConverted(TEXT_ID_MODE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MODE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT);
#else        
        Display_ShowString(46, 0, "npwt", DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT);
//        Display_ShowString(1, 5, "Vcare1000-300se.1.01", DISPLAY_FONT_6X12, DISPLAY_ALIGN_LEFT);
#endif
    }
}

如果Display_SendCharData函数中的语句是if (height == 64) 时
当#define SW 0时:
   “NPWP”从第2行开始显示，并且所有字符的下半部分不会显示
当#define SW 1时:、
    ”参数设定“从0行开始显示，并且显示正常

如果Display_SendCharData函数中的语句是if (height == 32) 时
当#define SW 0时:
   “NPWP”没有显示，将语句
   Display_ShowString(46, 0, "npwt", DISPLAY_FONT_16X32, DISPLAY_ALIGN_LEFT)
   中的第二个参数2改成0后显示正常
当#define SW 1时:、
    ”参数设定“没有显示，将语句
    Display_ShowString(40, 0, AppLanguage_GetTextConverted(TEXT_ID_MODE, text_buffer, sizeof(text_buffer)), AppLanguage_GetFontForText(TEXT_ID_MODE, DISPLAY_FONT_8X16), DISPLAY_ALIGN_LEFT)
    中第二个参数从0改成2后从第二行开始显示”参数设定“，但是同时会在”参数设定“字符下面多显示四个字符“数开定参”

