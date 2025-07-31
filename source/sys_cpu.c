#include   "include.h"	
#include   "npwt_dis_main.h"

extern unsigned char    audio_flg;
void SetBlk(void)/*used*/
	{
		if(audio_flg==LED_BAT_NORMAL)
		{
			LATC6 = 1;
		}
		
	}
