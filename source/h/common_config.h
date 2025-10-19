/* 
 * File:   common_config.h
 * Author: liwangzhi
 *
 * Created on April 6, 2021, 8:43 AM
 */

#ifndef COMMON_CONFIG_H
#define	COMMON_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

////////////////// Ö´ÐÐÖÜÆÚÊýËµÃ÷ ////////////////////////////////////////////////////////////
///    Ö´ÐÐÖÜÆÚÊýÊÇÖ¸´úÂëÖ´ÐÐ´ËÓï¾äµÄÖ´ÐÐ´ÎÊý£¬Ã¿¸öÖÜÆÚµÄÊµ¼ÊÖ´ÐÐÊ±¼äÊÇ²»¶¨µÄ£¬ÒÀ¾Ýµ±Ê±×´¿ö///
///£¬Ö´ÐÐÊ±¼äÓÐÐ©Ðí²îÒì¡£ºóÆÚÈç¹û¿ÉÄÜµÄ»°£¬¿ÉÒÔ¸ÄÎªÏµÍ³Ê±¼äµÄ¼ÆËã·½Ê½¡£                                  					   ///
//////////////////////////////////////////////////////////////////////////////////////////////
#define CANCEL_LEAKAGE_FLAG_CYCLE 200 // È¡ÏûÂ©Æø±êÖ¾µÄÊ±¼ä£¬µ¥Î»£ºÖ´ÐÐÖÜÆÚÊý
#define JUDGE_LEAKAGE_TIMEOUT 10800   // ÅÐ¶ÏÂ©ÆøµÄ³¬Ê±Ê±¼ä£¬µ¥Î»£ºÖ´ÐÐÖÜÆÚÊý
#define JX_MODE_TIME_FACTOR_BY_LX 2
#define LEAKAGE_TIMEOUT_FOR_LIX 2200 // Á¬ÐøÄ£Ê½ÏÂµÄÂ©ÆøÅÐ¶¨³¬Ê±Ê±¼ä
#define LEAKAGE_TIMEOUT_FOR_JIX 4400 // ¼äÐªÄ£Ê½ÏÂµÄÂ©ÆøÅÐ¶¨³¬Ê±Ê±¼ä
#define SET_LEAKAGE_FLAG_CYCLE 2200   // ÉèÖÃÂ©Æø±êÖ¾µÄÊ±¼ä£¬µ¥Î»£ºÖ´ÐÐÖÜÆÚÊý
#define JUDGE_CANISTER_FULL_COUNT 4   // ÅÐ¶ÏÒºÎ»ÂúµÄ´ÎÊý
#define JUDGE_CANISTER_FULL_TIME 8000 // ÅÐ¶ÏÒºÎ»ÂúµÄÅÐ¶ÏÊ±¼ä£¬ÔÚ´ËÊ±¼äÄÚ£¬Íê³É4´ÎÕðµ´¡£µ¥Î»£ºÖ´ÐÐÖÜÆÚÊý
#define MAX_BEE_TIME 10               // ·äÃùÆ÷³ÖÐøÏìµÄÊ±¼ä£¬µ¥Î»£ºÖ´ÐÐÖÜÆÚÊý
#define BELOW_3_5V_CYCLE 500          // Èç¹ûµçÑ¹Ð¡ÓÚ3.5V£¬ÇÒÔËÐÐ³¬¹ýBELOW_3_5V_CYCLE¸öÑ­»·£¬Ôò½øÐÐÈí¹Ø»ú£¬µ¥Î»£ºÖ´ÐÐÖÜÆÚÊý
#define LOWER_THAN_3_5V 5             // ×Ô¶¯¹Ø»úÖµ£¬µÍÓÚ3.5V
#define LOWER_BAT_WARN_3_6V 10        // µÍµçÁ¿±¨¾¯Öµ£¬µÍÓÚ3.6V
#define LOWER_THAN_3_7V 1             // µÍÓÚ3.7V
#define LOWER_THAN_3_8V 2             // µÍÓÚ3.8V
#define LOWER_THAN_4V 3               // µÍÓÚ3.8V
#define BAT_FULL 4                    // µç³ØµçÁ¿Âú
#define BAT_CHARGING 0                // µç³ØÕýÔÚ³äµç×´Ì¬ÏÂ
#define NO_KEY_PRESSED 0x3c           // Ã»ÓÐ°´¼ü±»°´ÏÂÊ±µÄ¼üÖµ
#define BACK_LED_OFF_TIMEOUT 1500     // ±³¹âÃðµÄ³¬Ê±Ê±¼ä
#define BUZZER_TIME_CYCLE 5           // ·äÃùÆ÷ÏìµÄÊ±¼ä£¬µ¥Î»£ºÖ´ÐÐÖÜÆÚÊý
#define TK_TIMEOUT_CYCLE 14999        // ¿ÕÏÐ³¬Ê±Ê±¼ä£¬¼ÆËã£º5*60*1000/20=15000
#define CANCEL_MUTEFLAG_TIMEOUT 15000 // È¡Ïû¾²ÒôÄ£Ê½µÄµ¹¼ÆÊ±
#define LED_LOW_THAN_3_5V_OR_ERR 10   // µ±µçÁ¿µÍÓÚ3.5V»òÕß·¢ÉúÈÎºÎ´íÎóÊ±£¬LEDµÄÏÔÊ¾·½Ê½£ºÂÌÆÁ
#define LED_LOW_THAN_3_6V 1           // µÍµçÁ¿Ê±£¬LEDµÄÏÔÊ¾·½Ê½£ºÂÌÆÁ£¬²¢ÇÒ½çÃæÏÔÊ¾µÍµçÁ¿
#define LED_BAT_NORMAL 0              // µçÁ¿Õý³£Ê±£¬LEDµÄÏÔÊ¾·½Ê½£º³äµçÊ±»ÆÆÁ£¬Î´³äµçÊ±ÂÌÆÁ
#define UNLOCK_NPWT_KEYVAL 0x24       // °´ÏÂ½âËø¼üÊ±µÄ¼üÖµ£¬¼´Í¬Ê±°´ÏÂ×óÓÒ¼ü
#define UNLOCK_NPWT_L_KEYVAL 0xa4     // ³¤Ê±¼ä°´ÏÂ½âËø¼üÊ±µÄ¼üÖµ£¬¼´Í¬Ê±°´ÏÂ×óÓÒ¼ü
#define LONG_PRESS_SWITCH_LANG 0x98   // ³¤Ê±¼ä°´ÏÂÉÏÏÂ¼ü£¬ÓÃÒÔÇÐ»»ÓïÑÔ
#define JUDGE_LONG_PRESS_TIME 100     // ÅÐ¶Ï°´¼ü±»³¤Ê±¼ä°´ÏÂµÄÊ±¼ä
#define GET_KEY_VAL PORTB&0x3c        // »ñÈ¡°´¼üµÄÖµ£¬ÍÆ²â£ºÕâ¸ö¼Ä´æÆ÷´æÓÐ°´¼üµÄµ±Ç°×´Ì¬£¬Î´±»°´ÏÂÊ±£¬°´¼ü´¦ÓÚ¸ßµçÆ½£»°´ÏÂºó£¬°´¼ü´¦ÓÚµÍµçÆ½£»
#define SILENT_FLAG_BITMASK 0x8000    // ¾²Ä¬±êÖ¾µÄ±êÖ¾Î»ÑÚÂë
#define SILENT_FLAG_SET_TIMEOUT 10000 // ¾²Ä¬±êÖ¾ÉèÖÃµÄÑÓÊ±Ê±¼ä
#define JIX_CYCLES_PER_SECOND 50      // ¼äÐªÄ£Ê½ÏÂ£¬Ã¿Ãë¶ÔÓ¦µÄÑ­»·´ÎÊý
#define LONGPRESS_MINITE_PER_STEP 5   // ³¤°´Ê±£¬ÉèÖÃÊ±¼äµÄ²½¾à
#define LONG_PRESS_SET_PRESS_INTERVAL 20   // ÉèÖÃÑ¹Á¦Ê±£¬³¤°´°´Å¥£¬Á¬Ðø¸ü¸ÄÑ¹Á¦µÄÊ±¼ä¼ä¸ô
#define MAX_JIX_HIGHMODE_TIME 99      // ¼äÐª¸ßÑ¹ÏÂÉèÖÃµÄ¸ßÑ¹×î¶à¹¤×÷Ê±¼ä
#define MAX_JIX_LOWMODE_TIME 99       // ¼äÐªÏÂÉèÖÃµÄµÍÑ¹×î¶à¹¤×÷Ê±¼ä
#define MIN_JIX_HIGHMODE_TIME 1       // ¼äÐª¸ßÑ¹ÏÂÉèÖÃµÄ¸ßÑ¹×îÉÙ¹¤×÷Ê±¼ä
#define MIN_JIX_LOWMODE_TIME 1        // ¼äÐªÏÂÉèÖÃµÄµÍÑ¹×îÉÙ¹¤×÷Ê±¼ä
#define JUDGE_DS_TIME 180000ul        // ÅÐ¶Ï¶ÂÈûµÄ³¬Ê±Ê±¼ä
#define RECORDE_PRESSURE_INTERVAL 3000// ¼ÇÂ¼Ñ¹Á¦µÄ¼ä¸ôÊ±¼ä£¬´ËÊ±¼ä³ËÒÔ7¾ÍÊÇ¼ì²â¶ÂÈûÆøÑ¹±ä»¯µÄÊ±¼ä
#define JUDGE_YWM_AFTER_PUMP_STOP 499 // ¸ßÑ¹×´Ì¬ÏÂÍ£±Ãºó£¬ÔÙ¾­¹ý¶àÉÙÊ±¼ä£¬ÈÔ¾É±£³Ö¸ßÑ¹£¬ÔòÈÏÎªÊÇÒºÎ»ÂúµÄÒ»´ÎÕðµ´
#define UPDATE_UI_PRESSURE_TIME_1 50  // ¸üÐÂ½çÃæÏÔÊ¾µÄÑ¹Á¦Ê±¼äÒòËØ1£¬¼´£¬¶à¾Ã²É¼¯Ò»´ÎÊý¾Ý
#define UPDATE_UI_PRESSURE_TIME_2 8   // ¸üÐÂ½çÃæÏÔÊ¾µÄÑ¹Á¦Ê±¼äÒòËØ2£¬¼´£¬¶à¾ÃÏÔÊ¾Ò»´Î²É¼¯µÄÊý¾Ý
#define PRESS_KEY_BUZ_SHOWLQ_TIME 299 // °´ÏÂBUZ¼ü£¬ÏÔÊ¾Â©ÆøµÄÊ±¼ä
#define MIN_PRESS_REPORT_DS 70        // ±¨¸æ¹ÜÂ·¶ÂÈûÊ±£¬Ñ¹Á¦´óÓÚMIN_PRESS_REPORT_DS£¬ÔòÖ±½Ó±¨¸æ£»
#define NO_REPORE_PRESS_FOR_DS 57     // Ñ¹Á¦Ð¡ÓÚNO_REPORE_PRESS_FOR_DSÊ±£¬²»±¨¹ÜÂ·¶ÂÈû¡£
#define DS_REPORT_STEADY_COUNTS 7     // µÍÑ¹×´Ì¬ÏÂ£¬±¨¸æ¹ÜÂ·¶ÂÈûÊ±£¬Á¬ÐøÅÐ¶¨DS_REPORT_STEADY_COUNTS´Î£¬²Å»áÈÏÎªÊÇ¹ÜÂ·¶ÂÈû
#define RECORDE_PRESSURE_INTERVAL_LOW 5000    // µÍÑ¹Ìõ¼þÏÂµÄ¶ÂÈûÑ¹Á¦¼ÇÂ¼Ê±¼ä
#define TOP_PRESSURE_MAX 320

enum EnumErr{
	ERR_NONE = 0,// ÎÞ´íÎó
	ERR_CANISTER_NOT_CON,// Î´½ÓºÐ×Ó
	ERR_CANISTER_REACHED,// ÒºÎ»µ½
	ERR_AIR_LEAKAGE,// Â©Æø
	ERR_SENSOR_MALFUCTION,// ´«¸ÐÆ÷¹ÊÕÏ
	ERR_PIPE_BLOCKED,// ¹ÜÂ·¶ÂÈû
	ERR_CANISTER_FULL,// ÒºÎ»Âú
	ERR_JAMED,// ¶ÂÈû¡£todo£ºÁ½ÕßÖ®¼äÓÐºÎÇø±ð
	ERR_DEV_IDLE,// ³¤Ê±¼ä¿ÕÏÐ
};

enum EnumUiSelect{
	UI_WORKMODE_SELECT = 0,// Ñ¡Ôñ¹¤×÷Ä£Ê½
	UI_MODE_SET_HI,// ¼äÏ¶Ä£Ê½ÏÂ£¬ÉèÖÃ¸ßÑ¹£»»òÕßÁ¬ÐøÄ£Ê½ÏÂÉèÖÃÑ¹Á¦
	UI_JIX_MODE_SET_LO,// ¼äÐªÄ£Ê½ÏÂÉèÖÃµÍÑ¹
	UI_JIX_SET_HI_TIME,// ¼äÐªÄ£Ê½ÏÂ£¬ÉèÖÃ¸ßÑ¹Ê±¼ä
	UI_JIX_SET_LO_TIME,// ¼äÐªÄ£Ê½ÏÂ£¬ÉèÖÃµÍÑ¹Ê±¼ä
	UI_SET_PRESSURE,// ÏÔÊ¾ÉèÖÃÑ¹Á¦½çÃæ
};

// lwz ¸ü¸Ä´ËÖµ£¬¿É¸ü»»logo
//#define LOGO_TYPE_DEROYAL
// #define LOGO_TYPE_VR_CHINA
//#define LOGO_TYPE_VR_FORIEGN
//#define LOGO_TYPE_VR_MEDWIN

#define LANGUAGE_RUSSIA_ENGILISH 1 //Ó¢¶íÓïÑÔÖ§³Ö


// china°æ±¾µÄµ¥¶ÀÉèÖÃ
#ifdef LANGUAGE_RUSSIA_ENGILISH
#define LOCK_FLAG_TIMEOUT 1500        // ËøÆÁµÄ³¬Ê±Ê±¼ä
#define DEFAULT_TARGET_PRESSURE 120   // ¿ª»úÄ¬ÈÏµÄÄ¿±êÑ¹Á¦
#define    CFG_HI           300       // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄ¸ßÑ¹ÉÏÏÞ
#define    CFG_LOW          20        // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄµÍÑ¹ÏÂÏÞ
#define JIX_LOW_MODE_HIGHPRESS 100    // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×î¸ßÉèÖÃÑ¹Á¦
#define JIX_LOW_MODE_LOWPRESS 10      // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×îµÍÉèÖÃÑ¹Á¦
#define MMHG_PER_STEP 10  // ³¤°´Ê±£¬ÉèÖÃÊ±¼äµÄ²½¾à
#define PUMP_IDLE_FLAG                // ÈôÊÇ¶¨ÒåÁË´Ë±êÖ¾£¬ÔòÏÔÊ¾pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*ÖÃÎ»±íÊ¾Â©Æø²»Í£±Ã*/
#endif

// droyal°æ±¾µÄµ¥¶ÀÉèÖÃ
#ifdef LOGO_TYPE_DEROYAL
#define LOCK_FLAG_TIMEOUT 45000       // ËøÆÁµÄ³¬Ê±Ê±¼ä
#define DEFAULT_TARGET_PRESSURE 125   // ¿ª»úÄ¬ÈÏµÄÄ¿±êÑ¹Á¦
#define    CFG_HI           200       // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄ¸ßÑ¹ÉÏÏÞ
#define    CFG_LOW          20        // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄµÍÑ¹ÏÂÏÞ
#define JIX_LOW_MODE_HIGHPRESS 100    // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×î¸ßÉèÖÃÑ¹Á¦
#define JIX_LOW_MODE_LOWPRESS 10      // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×îµÍÉèÖÃÑ¹Á¦
#define MMHG_PER_STEP 5   // ³¤°´Ê±£¬ÉèÖÃÊ±¼äµÄ²½¾à
#define PUMP_IDLE_FLAG                // ÈôÊÇ¶¨ÒåÁË´Ë±êÖ¾£¬ÔòÏÔÊ¾pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*ÖÃÎ»±íÊ¾Â©Æø²»Í£±Ã*/
#endif

// china°æ±¾µÄµ¥¶ÀÉèÖÃ
#ifdef LOGO_TYPE_VR_CHINA
#define LOCK_FLAG_TIMEOUT 1500        // ËøÆÁµÄ³¬Ê±Ê±¼ä
#define DEFAULT_TARGET_PRESSURE 120   // ¿ª»úÄ¬ÈÏµÄÄ¿±êÑ¹Á¦
#define    CFG_HI           300       // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄ¸ßÑ¹ÉÏÏÞ
#define    CFG_LOW          20        // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄµÍÑ¹ÏÂÏÞ
#define JIX_LOW_MODE_HIGHPRESS 100    // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×î¸ßÉèÖÃÑ¹Á¦
#define JIX_LOW_MODE_LOWPRESS 10      // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×îµÍÉèÖÃÑ¹Á¦
#define MMHG_PER_STEP 10  // ³¤°´Ê±£¬ÉèÖÃÊ±¼äµÄ²½¾à
#define PUMP_IDLE_FLAG                // ÈôÊÇ¶¨ÒåÁË´Ë±êÖ¾£¬ÔòÏÔÊ¾pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*ÖÃÎ»±íÊ¾Â©Æø²»Í£±Ã*/
#endif

// ¹úÍâ°æµÄµ¥¶ÀÉèÖÃ
#ifdef LOGO_TYPE_VR_FORIEGN
#define LOCK_FLAG_TIMEOUT 1500        // ËøÆÁµÄ³¬Ê±Ê±¼ä
#define DEFAULT_TARGET_PRESSURE 125   // ¿ª»úÄ¬ÈÏµÄÄ¿±êÑ¹Á¦
#define    CFG_HI           200       // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄ¸ßÑ¹ÉÏÏÞ
#define    CFG_LOW          20        // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄµÍÑ¹ÏÂÏÞ
#define JIX_LOW_MODE_HIGHPRESS 100    // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×î¸ßÉèÖÃÑ¹Á¦
#define JIX_LOW_MODE_LOWPRESS 10      // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×îµÍÉèÖÃÑ¹Á¦
#define MMHG_PER_STEP 5   // ³¤°´Ê±£¬ÉèÖÃÊ±¼äµÄ²½¾à
#define PUMP_IDLE_FLAG                // ÈôÊÇ¶¨ÒåÁË´Ë±êÖ¾£¬ÔòÏÔÊ¾pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*ÖÃÎ»±íÊ¾Â©Æø²»Í£±Ã*/
#endif

// medway°æµÄµ¥¶ÀÉèÖÃ
#ifdef LOGO_TYPE_VR_MEDWIN
#define LOCK_FLAG_TIMEOUT 1500        // ËøÆÁµÄ³¬Ê±Ê±¼ä
#define DEFAULT_TARGET_PRESSURE 120   // ¿ª»úÄ¬ÈÏµÄÄ¿±êÑ¹Á¦
#define    CFG_HI           300       // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄ¸ßÑ¹ÉÏÏÞ
#define    CFG_LOW          20        // Á¬ÐøÄ£Ê½»ò¼äÐªÄ£Ê½µÄ¸ßÑ¹¹¤×÷µÄµÍÑ¹ÏÂÏÞ
#define JIX_LOW_MODE_HIGHPRESS 100    // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×î¸ßÉèÖÃÑ¹Á¦
#define JIX_LOW_MODE_LOWPRESS 20      // ¼äÐªÄ£Ê½µÄµÍÑ¹Ä£Ê½ÏÂµÄ×îµÍÉèÖÃÑ¹Á¦
#define MMHG_PER_STEP 5   // ³¤°´Ê±£¬ÉèÖÃÊ±¼äµÄ²½¾à
#define PUMP_IDLE_FLAG                // ÈôÊÇ¶¨ÒåÁË´Ë±êÖ¾£¬ÔòÏÔÊ¾pump idle
#define IS_NO_LIQUID_ALARM_STOP    0   /*ÖÃÎ»±íÊ¾Â©Æø²»Í£±Ã*/
#endif


#ifdef	__cplusplus
}
#endif

#endif	/* COMMON_CONFIG_H */

