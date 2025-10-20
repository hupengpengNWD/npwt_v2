#include   "include.h"	
#include "system_manager.h"
#include "global_compat.h"
#include "hardware_abstraction.h"
#include   "npwt_dis_main.h"

extern unsigned char    audio_flg;
void SetBlk(void)
	{
		if(audio_flg==LED_BAT_NORMAL)
		{
			LATC6 = 1;
		}
		
	}
