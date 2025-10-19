

#ifndef COMMON_CONFIG_H
#define	COMMON_CONFIG_H

#ifdef	__cplusplus
extern "C" {
#endif

#define CANCEL_LEAKAGE_FLAG_CYCLE 200 
#define JUDGE_LEAKAGE_TIMEOUT 10800   
#define JX_MODE_TIME_FACTOR_BY_LX 2
#define LEAKAGE_TIMEOUT_FOR_LIX 2200 
#define LEAKAGE_TIMEOUT_FOR_JIX 4400 
#define SET_LEAKAGE_FLAG_CYCLE 2200   
#define JUDGE_CANISTER_FULL_COUNT 4   
#define JUDGE_CANISTER_FULL_TIME 8000 
#define MAX_BEE_TIME 10               
#define BELOW_3_5V_CYCLE 500          
#define LOWER_THAN_3_5V 5             
#define LOWER_BAT_WARN_3_6V 10        
#define LOWER_THAN_3_7V 1             
#define LOWER_THAN_3_8V 2             
#define LOWER_THAN_4V 3               
#define BAT_FULL 4                    
#define BAT_CHARGING 0                
#define NO_KEY_PRESSED 0x3c           
#define BACK_LED_OFF_TIMEOUT 1500     
#define BUZZER_TIME_CYCLE 5           
#define TK_TIMEOUT_CYCLE 14999        
#define CANCEL_MUTEFLAG_TIMEOUT 15000 
#define LED_LOW_THAN_3_5V_OR_ERR 10   
#define LED_LOW_THAN_3_6V 1           
#define LED_BAT_NORMAL 0              
#define UNLOCK_NPWT_KEYVAL 0x24       
#define UNLOCK_NPWT_L_KEYVAL 0xa4     
#define LONG_PRESS_SWITCH_LANG 0x98   
#define JUDGE_LONG_PRESS_TIME 100     
#define GET_KEY_VAL PORTB&0x3c        
#define SILENT_FLAG_BITMASK 0x8000    
#define SILENT_FLAG_SET_TIMEOUT 10000 
#define JIX_CYCLES_PER_SECOND 50      
#define LONGPRESS_MINITE_PER_STEP 5   
#define LONG_PRESS_SET_PRESS_INTERVAL 20   
#define MAX_JIX_HIGHMODE_TIME 99      
#define MAX_JIX_LOWMODE_TIME 99       
#define MIN_JIX_HIGHMODE_TIME 1       
#define MIN_JIX_LOWMODE_TIME 1        
#define JUDGE_DS_TIME 180000ul        
#define RECORDE_PRESSURE_INTERVAL 3000
#define JUDGE_YWM_AFTER_PUMP_STOP 499 
#define UPDATE_UI_PRESSURE_TIME_1 50  
#define UPDATE_UI_PRESSURE_TIME_2 8   
#define PRESS_KEY_BUZ_SHOWLQ_TIME 299 
#define MIN_PRESS_REPORT_DS 70        
#define NO_REPORE_PRESS_FOR_DS 57     
#define DS_REPORT_STEADY_COUNTS 7     
#define RECORDE_PRESSURE_INTERVAL_LOW 5000    
#define TOP_PRESSURE_MAX 320

enum EnumErr{
	ERR_NONE = 0,
	ERR_CANISTER_NOT_CON,
	ERR_CANISTER_REACHED,
	ERR_AIR_LEAKAGE,
	ERR_SENSOR_MALFUCTION,
	ERR_PIPE_BLOCKED,
	ERR_CANISTER_FULL,
	ERR_JAMED,
	ERR_DEV_IDLE,
};

enum EnumUiSelect{
	UI_WORKMODE_SELECT = 0,
	UI_MODE_SET_HI,
	UI_JIX_MODE_SET_LO,
	UI_JIX_SET_HI_TIME,
	UI_JIX_SET_LO_TIME,
	UI_SET_PRESSURE,
};

#define LANGUAGE_RUSSIA_ENGILISH 1 

#ifdef LANGUAGE_RUSSIA_ENGILISH
#define LOCK_FLAG_TIMEOUT 1500        
#define DEFAULT_TARGET_PRESSURE 120   
#define    CFG_HI           300       
#define    CFG_LOW          20        
#define JIX_LOW_MODE_HIGHPRESS 100    
#define JIX_LOW_MODE_LOWPRESS 10      
#define MMHG_PER_STEP 10  
#define PUMP_IDLE_FLAG                
#define IS_NO_LIQUID_ALARM_STOP    0   
#endif

#ifdef LOGO_TYPE_DEROYAL
#define LOCK_FLAG_TIMEOUT 45000       
#define DEFAULT_TARGET_PRESSURE 125   
#define    CFG_HI           200       
#define    CFG_LOW          20        
#define JIX_LOW_MODE_HIGHPRESS 100    
#define JIX_LOW_MODE_LOWPRESS 10      
#define MMHG_PER_STEP 5   
#define PUMP_IDLE_FLAG                
#define IS_NO_LIQUID_ALARM_STOP    0   
#endif

#ifdef LOGO_TYPE_VR_CHINA
#define LOCK_FLAG_TIMEOUT 1500        
#define DEFAULT_TARGET_PRESSURE 120   
#define    CFG_HI           300       
#define    CFG_LOW          20        
#define JIX_LOW_MODE_HIGHPRESS 100    
#define JIX_LOW_MODE_LOWPRESS 10      
#define MMHG_PER_STEP 10  
#define PUMP_IDLE_FLAG                
#define IS_NO_LIQUID_ALARM_STOP    0   
#endif

#ifdef LOGO_TYPE_VR_FORIEGN
#define LOCK_FLAG_TIMEOUT 1500        
#define DEFAULT_TARGET_PRESSURE 125   
#define    CFG_HI           200       
#define    CFG_LOW          20        
#define JIX_LOW_MODE_HIGHPRESS 100    
#define JIX_LOW_MODE_LOWPRESS 10      
#define MMHG_PER_STEP 5   
#define PUMP_IDLE_FLAG                
#define IS_NO_LIQUID_ALARM_STOP    0   
#endif

#ifdef LOGO_TYPE_VR_MEDWIN
#define LOCK_FLAG_TIMEOUT 1500        
#define DEFAULT_TARGET_PRESSURE 120   
#define    CFG_HI           300       
#define    CFG_LOW          20        
#define JIX_LOW_MODE_HIGHPRESS 100    
#define JIX_LOW_MODE_LOWPRESS 20      
#define MMHG_PER_STEP 5   
#define PUMP_IDLE_FLAG                
#define IS_NO_LIQUID_ALARM_STOP    0   
#endif

#ifdef	__cplusplus
}
#endif

#endif	

