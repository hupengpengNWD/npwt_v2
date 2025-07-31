#include   "include.h"
#include  "BIOS_JLX1864G_139.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include   "npwt_dis_main.h"
#include   "npwt_dis_ofile_lcd_02.h"
#include   "npwt_con_over.h"
#include   "Flash.h"
unsigned short  key_cnt;
unsigned char   key_key=0;
unsigned char   set_a00=0;
unsigned char key_port,key_port_bak,key_val,key_val_bak,key_flg,key_flg_l;// lwz key_flg表示是否处于长按状态
unsigned char mod_main_a,mod_main_b,mod_main_baka,mod_jixa;// mod_jixa为间歇模式下的高低压状态，0为高压，1为低压；mod_main_baka为flash中的备份工作模式；


unsigned char remember_mod;
unsigned char mod_seta_cnt,mod_seta_wok;// lwz mod_seta_cnt为1代表间歇模式高压力设置或者连续模式压力设置，为2代表间歇模式低压力设置
unsigned short mod_seta_preh,mod_seta_preh_bak,mod_seta_prel,mod_seta_ont,mod_seta_oft;// lwz 间歇模式下：mod_seta_ont高压的分钟；mod_seta_oft低压的分钟
unsigned short mod_tim_cnta,mod_tim_cnta2;

unsigned short back_led_cnt,lock_cnt;
unsigned char  lock_flg;



unsigned short  set_moda=0;
float   valueK  =  0;
unsigned short  dataK1,dataK2,dataK3,dataK4,dataK;
// lwz 扫描按键的状态，判断按键是否属于长按，按键按下就开背光，无按键则关背光，判断按键锁
void KEY_Scan(void)/*used*/
{
	key_port = GET_KEY_VAL;
	if (key_port == key_port_bak)// lwz 如果前后两次的按键状态一致
	{
		if (key_cnt++>=5)// lwz 累计计数大于5次，则进一步判断
		{
			if (key_port == NO_KEY_PRESSED)////////////////////////////////no key
			{
				//key_flg  =  0;/////////////////////////////按键标识清零
				key_flg_l=0;
				key_key=0;
				if (err_codea||err_codeb)// lwz 如果发生任何错误，不做处理
				{
					//SetBlk();
				}
				else if (bat_lev==LOWER_BAT_WARN_3_6V)// lwz 如果当前电池电压3.6V，即处于低压状态，不做处理
				{
					//SetBlk();
				}
				else if (back_led_cnt++>BACK_LED_OFF_TIMEOUT)
				{
					ClrBlk();    /////////////////////////30秒背光灭
				}

				/////////////自动上锁////////////////////////////////////////////////////////
				// if (lock_cnt++>1500)
				// {
					// lock_flg=1;  //////////////没有按键30秒后关背光锁按键
				// }
				
				
				//20160719 EDIT BY MA
				if (lock_cnt++>LOCK_FLAG_TIMEOUT)
				{
					lock_flg=1;  //////////////没有按键30秒后关背光锁按键
				}
				////////////////////////////////////////////////////////////////////////////
				if (key_flg==0)
				{///////////////////////////////////////////无长按按键 
					if (key_val_bak)
					{
						key_val = key_val_bak;
						buz_flg1=0;             ///////////////短按键BUZ开启
						///////////////键值获取
						key_val_bak = 0;        ///////////////清除备份值
					}
					else
					{
						key_val = key_val_bak;
						SPEAK_flg=0;
					}
				}
				else
				{// lwz 如果是刚刚切入无长按按键状态
					key_flg=0;
					//if(key_val&0x80)
					//{BUZ_KeyCls();}/////////////执行过常按键关闭BUZ
					key_val = 0;////////////////////////未执行过继续BUZ
					key_val_bak=0;//////////////////////清除键值
				}
			}
			else// lwz 任何按键被按下
			{
				lock_cnt=0;// lwz 锁计数清零
				back_led_cnt=0;// lwz LED计数清零
				SetBlk();// lwz 开启背光
				key_val_bak  =  key_port;// lwz 备份当前的键值
				if (key_val==UNLOCK_NPWT_KEYVAL)// lwz 0x24这个按键默认为长时间按下（很容易达到）
				{
					if (key_cnt>=5)
					{
						key_cnt = 5;
						key_val = key_val_bak+0x80;
						if (key_flg==1)
						{
							return;    /////////////////////////////
						}
						key_flg=1;
						buz_flg1=0;///////////////////////////长按键BUZ开启
					}

				}
				else
				{
					if (key_cnt>=JUDGE_LONG_PRESS_TIME)// lwz 其他按键，如果计数大于100，才认为长时间按下此键
					{
						key_cnt = JUDGE_LONG_PRESS_TIME;
						key_val = key_val_bak+0x80;// lwz 设置为长时间按下
						if (key_flg==1)
						{
							return;    /////////////////////////////
						}
						key_flg=1;
						buz_flg1=0;///////////////////////////长按键BUZ开启
					}
				}
			}
		}
	}
	else
	{// lwz 按键锁计数清零
		key_cnt=0;    ///////////////////////////////////////////////clear plus
		key_val=0;
	}
	if(key_val>0)
	{
        flg2015=0;
		if(mod_main_a==MOD_TK)
		{
			DISP_ClrZero(0,6,0,125);
			all_flg=0;
			flg2015=0;
			flg_disp=0;
			mod_main_a=remember_mod;
			remember_mod=0;
			key_val=0;
			audio_flg=LED_BAT_NORMAL;
			SetBlk();
		}
		TK_TIME=0;
	}		
	key_port_bak  =  key_port;
}
/*----------------16位有符号数计算差值-----------------------------------------*/
static unsigned short calc_delta_i(unsigned short counts1,unsigned short counts2 )
{
	unsigned short rst = 0;
	if ( counts1 >= counts2 )
	{
		rst = (counts1 - counts2);
	}
	if (rst<10)
	{
		rst = 10;
	}
	return rst;
}
unsigned char   key_set_tim,key_start_tim;// lwz 开始设备倒计时

unsigned char   tim5_flg=0;



unsigned char   ddfq=0;// lwz 间歇模式下的当前的放气状态：0不放气；1放气；2是低压工作模式下放气已经完成，不再继续放气
unsigned short   ddfq_delay=0;// lwz 放气的延迟时间


// lwz 根据当前的压力，决定当前的高步距和低步距
void get_xx_delta(unsigned short press)
{
	unsigned short delta = 8;
	//// lwz 更改：除了Deroyal版意外，其他的版本将浮动阈值扩大，尽量减少泵的启动次数
#ifdef LOGO_TYPE_DEROYAL
	if (press<20)
	{
		delta = 2;
		con_hi_delta = 2;
	}
	else if (press >= 80)
	{
		delta = press/20;
		con_hi_delta = delta*15/10;
	}
	else
	{
		delta = press/10;
		con_hi_delta = delta*17/10;
	}
#else
	delta = press/10;
#endif
	con_lo_delta = delta;
	con_hi_delta = delta;
	
}

static unsigned short key_mute_press_cnt = 0;
void  MODE_ProA(void)/*used*/
{
	switch (mod_main_a)
	{
	case MOD_SYS:////////////////////////////////////////////////system test
	{////////////////////////////////////////////////////////////
		err_codea=0;
		mod_tim_cnta2=0;
		mod_jixa=0;
		TK_TIME=0;
		// lwz 添加此代码，如果在系统初始化阶段点了静音按钮，则切换静音标志。
		// lwz key_mute_press_cnt的目的是确保静默标志只切换一次
		// lwz PORTB地址的值，应该就是当前被按下的按键值
		if ((GET_KEY_VAL)==KEY_MUT && key_mute_press_cnt++ > 100 && key_mute_press_cnt < SILENT_FLAG_SET_TIMEOUT)
		{
			key_mute_press_cnt = SILENT_FLAG_SET_TIMEOUT;
			if(key_silent_flag)
			{
				key_silent_flag = 0;
			}
			else
			{
				key_silent_flag = SILENT_FLAG_BITMASK;
			}
		}
		if (mod_tim_cnta++>=(7000/MAIN_CLK))// lwz 显示主界面
		{
			mod_main_a = MOD_WAT;
			mod_tim_cnta = 0;
			key_val=key_val_bak=0;
			BUZ_KeyCls();
			lock_cnt=0;
			lock_flg=0;
			key_flg_l=1;
			DISP_Clear();
			adc_zero=adc_ps0;// lwz 采集当前大气压，因为这个时候阀2在放气，所以应该是大气压
			scan_dusai_time = 0;
			flager_a &=~ERRB_DS;
		}
		else if (mod_tim_cnta>=(5000/MAIN_CLK))// lwz 显示版本号
		{
			DISP_Ver();
			// DISP_8X16ascii("Intermittent",1,13);
			// mod_tim_cnta--;
		}
		else if (mod_tim_cnta>50)// lwz 开机，显示开机logo
		{
			if (mod_tim_cnta<52)
			{
				if (key_start_tim<30)// lwz 如果按下开机键的时间过于短，取消开机
				{
					mod_tim_cnta=0;
					POWER_ON=0;
					key_start_tim=0;
					return;
				}
				else
				{
					SYS_IniLcd();
					POWER_ON=1;
					DRV_EN=1;
					VAL2 = 1;// lwz 打开阀2进行放气
				}
			}
			SetBlk();
			DISP_Sys100();
			// DISP_Clear();
			// DISP_BatWarnE() ;
			if (mod_tim_cnta>62)// lwz 开机短暂的响一声
			{
				SPEAK  =  0;
			}
			else
			{
				SPEAK  =  1;
			}
		}
		else
		{
			if ((GET_KEY_VAL)==KEY_C)// lwz 按下KEY_C
			{
				key_start_tim++;
			}
			else
			{
				key_start_tim=0;
			}
		}
		break;
	}
	case MOD_WAT:
	{
		
		record_ds_turn=0;
        twenty_seconds=0;
		
		VAL2 = 0;
		mod_jixa = 0;
		scan_dusai_time = 0;
		flager_a &=~ERRB_DS;
		
		debug_air=0;
		if (err_codea!=0)//////////////此状态下的故障需要重新开机
		{
			break;
		}
		if (lock_flg==1)
		{
			break;
		}
		if (key_val == KEY_C )
		{
			mod_main_a = mod_main_baka;
		}
		else if (key_val == KEY_UP)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;
		}
		break;
	}
	case MOD_LIX:
	{

		//PUMP = 1;
		mod_seta_preh = mod_seta_preh_bak;
		mod_tim_cnta2=0;
		mod_jixa=0;
		mod_main_baka = MOD_LIX;

		if (lock_flg==1)
		{
			break;
		}

		if (key_val == KEY_OK )// lwz 在连续模式下按下OK，则进入暂停模式
		{
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)// lwz 在连续模式下长按左箭头，则进入设置模式
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;
			if (language)
			{
//				DISP_mod_set2015();
			}
			else
			{
				;
			}
		}
		break;
	}
	case MOD_JIX:
	{
		if (mod_jixa==0)// lwz 间歇模式下的高压状态
		{
			mod_seta_preh = mod_seta_preh_bak;
			if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)// lwz 每50个循环进入一次该分支，即50循环等于1秒钟
			{
				mod_tim_cnta=0;
				mod_tim_cnta2++;
				if (mod_tim_cnta2>=mod_seta_ont*60)// 间歇模式下的高压工作时间是否达到
				{
					record_ds_turn=0;
					twenty_seconds=0;
					


		//20160725 修改在间隙模式下，报管路堵塞后，进入低压模式不消除报警
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// 只有当前气压大于设定压力，后才有con_flg_falla=1；

					bbbbb=0;
					ddfq=1;
					ddfq_delay=0;
					mod_tim_cnta2=0;
					mod_jixa=1;// lwz 切换到低压工作状态
				}
			}
		}
		else if (mod_jixa==1)// lwz 间歇模式下的低压状态
		{
			mod_seta_preh=mod_seta_prel;// lwz 设置气压的目标值未低压
			
			if (adc_ps0>adc_zero)
				pwm_k1    =    (adc_ps0-adc_zero)/valueK;
			else
				pwm_k1=0;
			
			adc_ps00  =    (unsigned int)pwm_k1;
			
			if(ddfq==1)// 如果需要在低压状态下放气
			{
				if(ddfq_delay++ >= 12)// lwz 在12到75之间属于放气阶段
				{
					VAL2=0;
					if(ddfq_delay >=75)
					{
						get_xx_delta(mod_seta_preh);
						if(adc_ps00 <= (mod_seta_preh+con_hi_delta))//if(adc_ps00 <= (mod_seta_preh+con_hi_delta+2))
						{
							ddfq=2;// lwz 低压工作模式下放气已经完成，不再继续放气，关闭VAL2
							mod_tim_cnta=0;
						}
						ddfq_delay=0;
					}
				}
				else
					VAL2=1;	


		//20160725 修改在间隙模式下，报管路堵塞后，进入低压模式不消除报警
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// 只有当前气压大于设定压力，后才有con_flg_falla=1；			
			}
			else
			{
				if(ddfq_delay++ >= 499)
				{
					ddfq=0;
					if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)
					{
						mod_tim_cnta=0;
						mod_tim_cnta2++;
						if (mod_tim_cnta2>=mod_seta_oft*60)// 间歇模式下的低压工作时间是否达到
						{
							record_ds_turn=0;
							twenty_seconds=0;
							mod_tim_cnta2=0;
							mod_jixa=0;
							mod_seta_preh = mod_seta_preh_bak;
						}
					}
					ddfq_delay=499;
				}
			}
		}
		mod_main_baka = MOD_JIX;

		if (lock_flg==1)
		{
			break;
		}

		if (key_val == KEY_OK )// lwz 间歇模式下，按下OK，则进入暂停模式
		{
			////20160420日修改，间隙进入暂停模式，从高的开始进来
			mod_jixa=0;
			mod_tim_cnta=0;
			mod_tim_cnta2=0;
			/////////////////////////////////////////////////////
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)// lwz 间歇模式下，长按左箭头，则进入设置模式
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;

			if (language)
			{
//				DISP_mod_set2015();
			}
			else
			{
				;
			}
		}
		break;
	}
	case MOD_ZXB:
	{
		if (mod_jixa==0)
		{
			mod_seta_preh = mod_seta_preh_bak;
			if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)
			{
				mod_tim_cnta=0;
				mod_tim_cnta2++;
				if (mod_tim_cnta2>=mod_seta_ont*60)
				{
					mod_tim_cnta2=0;
					mod_jixa=1;
				}
			}
		}
		else if (mod_jixa==1)
		{

			mod_seta_preh=mod_seta_prel;

			if (mod_tim_cnta++>=JIX_CYCLES_PER_SECOND)
			{
				mod_tim_cnta=0;
				mod_tim_cnta2++;
				if (mod_tim_cnta2>=mod_seta_oft*60)
				{
					mod_tim_cnta2=0;
					mod_jixa=0;
					mod_seta_preh = mod_seta_preh_bak;
				}
			}
		}
		mod_main_baka = MOD_ZXB;

		if (lock_flg==1)
		{
			break;
		}

		if (mod_main_b==MOD_SET)
		{
			break;
		}
		if (key_val == KEY_OK )
		{
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UPL)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;
		}
		break;
	}
	case MOD_ZHT:
	{
		record_ds_turn=0;
        twenty_seconds=0;
		PUMP = 0;
		debug_air=0;
		mod_jixa = 0;
		if (lock_flg==1)
		{
			break;
		}

		if (key_val == KEY_OK )
		{
			mod_main_a = mod_main_baka;
		}
		else if (key_val == KEY_UPL)
		{
			mod_main_a = MOD_SET;
			mod_seta_preh = mod_seta_preh_bak;

			if (language)
			{
//				DISP_mod_set2015();
			}
			else
			{
				;
			}
		}

		else if (key_val == 0x04)
		{
			tim5_flg =  ~tim5_flg;
		}
		scan_dusai_time = 0;
		flager_a &=~ERRB_DS;
		ddfq=0;
		break;;
	}
	case MOD_SET:
	{
		scan_dusai_time = 0;
		flager_a &=~ERRB_DS;
		debug_air=0;
		if (mod_seta_cnt == UI_WORKMODE_SELECT)// lwz mod_seta_cnt为0代表当前界面为选择模式
		{
			if (set_a00==0)// lwz 当前在界面上选择的是连续模式
			{
				mod_seta_wok = MOD_LIX;
				if (key_val == KEY_UP)
				{
					set_a00=1;
					mod_seta_wok = MOD_JIX;
				}
				else if (key_val == KEY_DN)
				{
					set_a00=1;
					mod_seta_wok = MOD_JIX;
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt = UI_MODE_SET_HI;
					DISP_Clear();
				}
			}
			else if (set_a00==1)// lwz 当前在界面上选择的是间歇模式
			{
				mod_seta_wok = MOD_JIX;
				if (key_val == KEY_UP)
				{
					set_a00=0;
					mod_seta_wok = MOD_LIX;
				}
				else if (key_val == KEY_DN)
				{
					set_a00=0;
					mod_seta_wok = MOD_LIX;
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt = UI_MODE_SET_HI;
					DISP_Clear();
					if (language)
					{
//						DISP_press_set2016();
					}
				}
			}
			else if (set_a00==2)// lwz 当前在界面上选择的是无模式
			{
				mod_seta_wok = MOD_ZXB;
				if (key_val == KEY_UP)
				{
					set_a00=0;
					DISP_Clr1();
					mod_seta_wok = MOD_LIX;
				}
				else if (key_val == KEY_DN)

				{
					set_a00=1;
					DISP_Clr1();
					mod_seta_wok = MOD_JIX;
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt = UI_MODE_SET_HI;
					DISP_Clear();
				}
			}
		}
		else if (mod_seta_cnt==UI_MODE_SET_HI)// lwz mod_seta_cnt为1代表间歇模式高压力设置或者连续模式压力设置
		{
			if (mod_seta_wok == MOD_LIX)// lwz 连续模式下设置压力
			{
				if (key_val == KEY_UP)
				{
					mod_seta_preh=mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)// lwz 压力超过上限则翻转
					{
						mod_seta_preh=CFG_LOW;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_DN)
				{
					mod_seta_preh=mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)// lwz 压力超过下限则翻转
					{
						mod_seta_preh=CFG_HI;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_DNL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)
					{
						mod_seta_preh=CFG_LOW;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_UPL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)
					{
						mod_seta_preh=CFG_HI;
					}
					DISP_Clr3();
				}
				else if (key_val == KEY_OK)
				{
					mod_seta_cnt=UI_WORKMODE_SELECT;////////////////////////20151201修改////////////////////
					mod_main_baka=mod_seta_wok;
					mod_main_a=MOD_ZHT;
					DISP_Clear();
					if (language)
					{
//						DISP_press_set2016();
					}
					else
					{
						;
					}
				}
			}
			else//间隙模式设高压力 
			{
				if (key_val == KEY_UP)
				{
					mod_seta_preh = mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)
						mod_seta_preh=CFG_LOW;
				}
				else if (key_val == KEY_DN)
				{
					mod_seta_preh = mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)
					{
						mod_seta_preh=CFG_HI;
					}
					// DISP_Clr3();
				}
				else if (key_val == KEY_DNL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh-MMHG_PER_STEP;
					if (mod_seta_preh<CFG_LOW)
					{
						mod_seta_preh=CFG_LOW;
					}

					// DISP_Clr3();
				}
				else if (key_val == KEY_UPL)
				{
					if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
					{
						return;
					}
					key_set_tim=0;
					mod_seta_preh = mod_seta_preh+MMHG_PER_STEP;
					if (mod_seta_preh>CFG_HI)
					{
						mod_seta_preh=CFG_HI;
					}

					// DISP_Clr3();
				}
				else if (key_val == KEY_OK)
				{

					// if(mod_seta_wok == MOD_ZXB)
					if (mod_seta_wok == MOD_JIX)
					{
						mod_seta_cnt=UI_JIX_MODE_SET_LO;
					}
					else
					{
						// mod_seta_prel=0;
						mod_seta_cnt=UI_JIX_SET_HI_TIME;
					}DISP_Clear();
					if (language)
					{
//						DISP_press_set2016();
					}
					else
					{
						;
					}
				}
			}

			if (mod_seta_preh < mod_seta_prel+MMHG_PER_STEP)
			{
				mod_seta_prel=mod_seta_preh-MMHG_PER_STEP;
			}
			mod_seta_preh_bak=mod_seta_preh;
		}
		else if (mod_seta_cnt==UI_JIX_MODE_SET_LO)//间隙模式设低压力
		{

			if (key_val == KEY_UP)
			{
				mod_seta_prel = mod_seta_prel+MMHG_PER_STEP;
                if(mod_seta_prel > JIX_LOW_MODE_HIGHPRESS || mod_seta_prel > mod_seta_preh - 10)
					mod_seta_prel = JIX_LOW_MODE_LOWPRESS;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				// DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{

				mod_seta_prel = mod_seta_prel-MMHG_PER_STEP;
                //20160606 add  如果压力小于10，则为高压-5，实现数字反转
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
                if(mod_seta_prel < JIX_LOW_MODE_LOWPRESS )
					mod_seta_prel = mod_seta_preh - 10;
				// DISP_Clr3();
			}
			else if (key_val == KEY_DNL)
			{

				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				//mod_seta_prel = mod_seta_prel-10;
				key_set_tim=0;
				mod_seta_prel = mod_seta_prel-MMHG_PER_STEP;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
                //20160606 add  如果压力小于10，则为高压-5，实现数字反转
				// DISP_Clr3();
			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				//mod_seta_prel = mod_seta_prel+10;
				key_set_tim=0;
				mod_seta_prel = mod_seta_prel+MMHG_PER_STEP;
				if(mod_seta_prel >= mod_seta_preh - 10 )
					mod_seta_prel = mod_seta_preh - 10;
				// DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				if (language)
				{
//					DISP_time_set2015();
                    DISP_Clear();
				}
				else
				{
					DISP_Clear();
				}
				mod_seta_cnt=UI_JIX_SET_HI_TIME;
			}
			if (mod_seta_prel>JIX_LOW_MODE_HIGHPRESS)
			{
				mod_seta_prel = JIX_LOW_MODE_HIGHPRESS;
			}
			if (mod_seta_prel<JIX_LOW_MODE_LOWPRESS)
			{
				mod_seta_prel = JIX_LOW_MODE_LOWPRESS;
			}

		}
		else if (mod_seta_cnt==UI_JIX_SET_HI_TIME)//间隙模式充ON时间
		{
			if (key_val == KEY_UP)
			{
				//mod_seta_ont = mod_seta_ont+10;
				if (mod_seta_ont++>=MAX_JIX_HIGHMODE_TIME)
				{
					mod_seta_ont=MIN_JIX_HIGHMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{
				if (mod_seta_ont--<=MIN_JIX_HIGHMODE_TIME)
				{
					mod_seta_ont=MAX_JIX_HIGHMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_DNL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_ont -= LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_ont > MAX_JIX_HIGHMODE_TIME || mod_seta_ont < MIN_JIX_LOWMODE_TIME)
				{
					mod_seta_ont=MIN_JIX_HIGHMODE_TIME;
				}

				//DISP_Clr3();
			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_ont += LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_ont > MAX_JIX_HIGHMODE_TIME || mod_seta_ont < MIN_JIX_LOWMODE_TIME)
				{
					mod_seta_ont=MAX_JIX_HIGHMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=UI_JIX_SET_LO_TIME;
//				if (language==0)
//				{
					DISP_Clear();
//				}
			}

		}
		else if (mod_seta_cnt==UI_JIX_SET_LO_TIME)//间隙模式设OFF时间
		{
			if (key_val == KEY_UP)
			{
				if (mod_seta_oft++ >= MAX_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MIN_JIX_LOWMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{
				if (mod_seta_oft-- <= MIN_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MAX_JIX_LOWMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_DNL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_oft=mod_seta_oft-LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_oft < MIN_JIX_LOWMODE_TIME || mod_seta_oft > MAX_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MIN_JIX_LOWMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<LONG_PRESS_SET_PRESS_INTERVAL)
				{
					return;
				}
				key_set_tim=0;
				mod_seta_oft=mod_seta_oft+LONGPRESS_MINITE_PER_STEP;
				if (mod_seta_oft >= MAX_JIX_LOWMODE_TIME)
				{
					mod_seta_oft=MAX_JIX_LOWMODE_TIME;
				}
				//DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=0;
				mod_main_baka=mod_seta_wok;
				mod_main_a=MOD_ZHT;//进入暂停模式
				mod_tim_cnta=0;
				cnt_cnta=0;
				cnt_cntaa=0;
				mod_tim_cnta2=0;
				mod_jixa=0;
				// DISP_Clear();
			}
		}
		else if (mod_seta_cnt==UI_SET_PRESSURE)//没有用上
		{

			if (key_val == KEY_UP)
			{
				set_moda=set_moda+1;

				DISP_Clr3();
			}
			else if (key_val == KEY_DN)
			{
				set_moda=set_moda-1;
				DISP_Clr3();
			}
			else if (key_val == KEY_DNL)
			{
				if (key_set_tim++<20)
				{
					return;
				}
				key_set_tim=0;

				set_moda=set_moda-10;
				DISP_Clr3();
			}
			else if (key_val == KEY_UPL)
			{
				if (key_set_tim++<20)
				{
					return;
				}
				key_set_tim=0;
				set_moda=set_moda+10;
				DISP_Clr3();
			}
			else if (key_val == KEY_OK)
			{
				mod_seta_cnt=0;
				valueK  =  2.75f;
				mod_main_a=MOD_ZHT;

			}
		}

		break;;
	}
	case MOD_LANGUAGE:
	{

		if (key_val == KEY_OK )
		{				
			mod_main_a = MOD_ZHT;
		}
		else if (key_val == KEY_UP)
		{
			language++;
			if (language>6)
			{
				language = 6;
			}
		}
		else if (key_val == KEY_DN)
		{
				language--;
			if (language<1)
			{
				language = 0;
			}
		}
		else if(key_val==0x98)
		{
			DISP_LANGUAGE();
		}
		switch (language)
			{
			default:
				break;
				case 0:
				{
			DISP_set_sanjiao(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}
				case 1:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}
				case 2:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}

				case 3:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;			
				}
				case 4:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao0(4,122);
						break;				
				}
				case 5:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao(2,122);
			DISP_set_sanjiao0(4,122);
						break;			
				}
				case 6:
				{
			DISP_set_sanjiao0(0,53);
			DISP_set_sanjiao0(2,53);
			DISP_set_sanjiao0(4,53);
			DISP_set_sanjiao0(6,53);
			DISP_set_sanjiao0(0,122);
			DISP_set_sanjiao0(2,122);
			DISP_set_sanjiao(4,122);
						break;				
				}
		}
		break;		
	}
	case MOD_ERR:
	{
		break;
	}
	case MOD_OFF:
	{

		break;
	}
	case MOD_TK:
	{

		break;
	}
	
	default:
	{
		break;
	}
	}
}




unsigned short  close_tim;
unsigned char   mod_bak_bak;
void  MODE_Pro(void)/*used*/
{
	if ( (mod_main_a != MOD_OFF )&&(mod_main_a != MOD_SYS ))
	{//////////////////////////////////////////////////////////////
		close_tim=0;
		if (lock_flg==1)
		{
			;
		}
		else
		{
			if (key_val == KEY_CL )
			{
				if (key_flg_l==1)
					return;
				key_flg_l  =  1;
				flg2015=0;
				DISP_Clear() ;
				mod_main_a = MOD_OFF;
				mod_main_b = MOD_OFF;
			}
		}
	}
	else if ( mod_main_a == MOD_OFF )
	{
		if ((bat_lev&0x0f)==LOWER_THAN_3_5V)// lwz 低电压下，不进行正常关机操作。推测：可能是为了避免在写FLASH时断点，从而毁坏数据
		{
			;
		}
		else
		{
			if (close_tim++>50)
			{
				ClrBlk();
				AUDIO_EN=0;
				Flash_Erase(addr);//擦除1024个字节,CPU 在擦除期间将会停止工作,所以无法仿真
				Write_One_Word(addr, mod_seta_preh_bak);
				Write_One_Word(addr+2, mod_seta_ont);
				Write_One_Word(addr+4, mod_seta_oft);
				Write_One_Word(addr+6, mod_main_baka);
				dataK  =  (unsigned int)(valueK*1000.0);
				dataK1 =   dataK/1000;
				dataK2 =   dataK%1000 /100;
				dataK3 =   (  ( dataK%1000 )%100   ) /10;
				dataK4 =   (  ( dataK%1000 )%100   )%10;
				Write_One_Word(addr+8, dataK1);
				Write_One_Word(addr+10,dataK2);
				Write_One_Word(addr+12,dataK3);
				Write_One_Word(addr+14,dataK4);
				key_silent_flag = (key_silent_flag | language);// lwz 将静默标志保存到flash中
				Write_One_Word(addr+16,key_silent_flag);
				Write_One_Word(addr+18,mod_seta_prel);
				POWER_ON=0;
			}
		}
	}

	if (key_val==UNLOCK_NPWT_L_KEYVAL)// 按键被长按，解锁按键
	{
		lock_flg=0;
		lock_cnt=0;
	}
	if (key_val==UNLOCK_NPWT_KEYVAL)// 按键被长按，解锁按键
	{
		lock_flg=0;
		lock_cnt=0;
	}

	if (lock_flg==0) // lwz 非锁定状态 启动后进入暂停模式，按了POWER+BUZ进行换语言
	{
		if ( mod_main_a==MOD_WAT)
		{
			if (key_key==0)// lwz 这里主要是为了判断“在切换语言之前，是否有一段无按键按下的空隙”，有空隙才允许切换语言
			{
//20160606 禁止语文切换
// lwz 由于这个语言切换功能不完善，尚有许多BUG，禁掉原来的功能
				if (key_val==LONG_PRESS_SWITCH_LANG)
				{
					key_key=1;
					// 切换语言
					//language=~language;
					if(0 == language)
					{
						language = 0xFE;
					}
					else
					{
						language = 0;
					}
                    all_flg = 0;
					//mod_main_a=MOD_LANGUAGE;
					DISP_Clear();
					// DISP_LANGUAGE();
				}

			}
		}

	}
	if (mod_main_a!=mod_bak_bak)//模式切换前先清屏
	{
		DISP_Clear();
	}
	mod_bak_bak=mod_main_a;//备份当前模式
}





