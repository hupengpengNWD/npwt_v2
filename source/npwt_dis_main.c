
#include  "include.h"
#include  "Flash.h"
#include "BIOS_JLX1864G_139.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include  "npwt_con_ifile_adc.h"
#include   "npwt_con_over.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include   "npwt_dis_main.h"
#include   "npwt_con_main.h"
#include   "npwt_dis_ofile_lcd_02.h"
#include  "adc.h"
// __CONFIG(1,WDTEN_OFF & XINST_OFF );
// __CONFIG(2,OSC_INTOSCPLL);
// __CONFIG(3,DSWDTEN_OFF);
// __CONFIG(4,WPCFG_OFF);
//
//#pragma config WDTEN = ON,XINST = OFF
//#pragma config OSC = INTOSCPLL
//#pragma config DSWDTEN = OFF
//#pragma config WPCFG = OFF
//#pragma config CP0 = OFF 
//#pragma config WDTPS = 16384
//#pragma config IOL1WAY = OFF
//#pragma config WPDIS  = OFF

// lwz 生成配置位，并放入一个单独的文件中
#include "../project/config_bits.h"

unsigned char    audio_flg;
unsigned short   audio_cnt,audio_basic;
unsigned short   audio_period;
unsigned char    err_codea,err_codeb;/*错误状态   lwz   err_codeb没有任何用处，可以直接忽略，值一直为0 */
unsigned char    buz_flg,buz_flg1,buz_cnt;// lwz buz_flg代表是否需要蜂鸣器响；buz_flg1为0代表蜂鸣器正在响，1代表不响了；buz_cnt代表关闭响声的倒计时；
unsigned short   bat_close_tim=0;
unsigned char    mute_flg=0;// lwz 静音标志，0为不静音，1为静音
unsigned short   mute_tim=0;

unsigned char  SPEAK_flg=0;
unsigned char  close_flg=0;
unsigned char   	bat_lev_bak=0;// lwz 对z1的备份
unsigned char   	z1=0;// lwz z1的各个位代表的含义：0位-》是否低于3.5V；1位-》是否发生了任何错误；3位-》是否低于3.7V；

unsigned char   	open_bum=0;
unsigned char   	bum_dly=0;
unsigned char   	bum_dly_flg=0;// lwz 泵不需要放气，则为0，需要放气则为1
unsigned short      dataREAD0,dataREAD1,dataREAD2,dataREAD3;

unsigned char SPK_STATE = 0;
volatile unsigned short TK_TIME = 0;//太空时间
unsigned short    key_silent_flag = 0;

/*20秒响一声*/
typedef enum _on{
	BEEGO,BEEONE,BEETWO,BEETHREE,BEEEND
}BEEPTWO;
BEEPTWO BEE_TWO=BEEGO;
#define spk_set()  SPEAK=1;SPK_STATE=1;// lwz 蜂鸣器响
#define spk_clr()  SPEAK=0;SPK_STATE=0;// lwz 蜂鸣器关闭
unsigned short spk_selay = 0;// lwz 蜂鸣器延时
void spk_bee_server()
{
	if (0==SPK_STATE)// lwz 确保关闭蜂鸣器，并重置spk_selay计数
	{
			spk_selay = 0;
			spk_clr();
			return;
	}
	
	if (spk_selay++ > MAX_BEE_TIME)// lwz 当SPK_STATE=1时（即蜂鸣器在响），且计时器超时，则关闭蜂鸣器
	{
		spk_selay = 15;
		audio_period=0;
		BEE_TWO=BEETWO;
		spk_clr();
	}
}
void  SYS_DatIni(void)/*used*/
{
	mod_main_a =  MOD_SYS;
	mod_seta_cnt = UI_WORKMODE_SELECT;
	back_led_cnt=0;
	key_val=0;
	key_val_bak=0;
	lock_flg=0;
	err_codea=err_codeb=0;
	mod_tim_cnta=0;;
	buz_flg1=0;
	///////////////////////////////////////
	audio_period=AUDIO_PERIOD+400;// lwz 让蜂鸣器开机就响
	audio_cnt=0;
	audio_basic=0;
	audio_flg=LED_BAT_NORMAL;
	key_start_tim=0;
	bat_close_tim=0;
	mod_tim_cnta=0;
	mod_tim_cnta2=0;
	mod_jixa=0;
}

void Warn(void)/*used*/
{
	// lwz 警报优先级：管路堵塞 < 液位到 < 漏气 < 传感器触发 < 未接液盒
	if (flager_a&ERRA_V)// lwz 如果未接液盒
	{
		if (mod_main_a==MOD_WAT)// lwz 如果当前处于等待指令模式，则确认错误为：未接盒
		{
			err_codea=ERR_CANISTER_NOT_CON;    /////////未接盒
		}
		else// lwz 否则确认错误为：堵管
		{
			err_codea=ERR_PIPE_BLOCKED;    //////////堵管
		}
	}
	else if (flager_a&ERRA_S)// lwz 如果接了盒子，并且传感器触发
	{
		if (mod_main_a==MOD_WAT)// lwz 如果接了盒子，传感器触发，并且当前处于等待指令的阶段，则认为传感器故障
		{
			err_codea=ERR_SENSOR_MALFUCTION;    ///////////传感器故障
		}
		else
		{
			err_codea=ERR_CANISTER_FULL;    ///////////液位满故障
		}
		//err_codea=4;
	}
	else if (flager_a&ERRA_LQ)// lwz 如果接了盒子，传感器没触发，并且漏气
	{
		err_codea=ERR_AIR_LEAKAGE;    //////////漏气
	}
	else if (flager_a&ERRA_YW)// 液位到
	{
		err_codea=ERR_CANISTER_REACHED;    //////////液位到
	}
	else if (flager_a&ERRB_DS)// lwz 管路堵塞
	{
		err_codea=ERR_JAMED;    //////////IEC60601 堵塞报警
	}
	else if (flager_a&ERRB_TK)// lwz 空闲5分钟
	{
		err_codea=ERR_DEV_IDLE;    //////////IEC60601 堵塞报警
	}
	
	else
	{
		err_codea=0;    ///////////无故障
	}
	if (err_codea > 0)/*如果有故障 则灯亮*/
	{
		back_led_cnt = 0;
	}
	spk_bee_server();// lwz 设置BEE_TWO = BEETWO
}
unsigned short bee_delay = 0;
// lwz 此处为控制声音播放的主函数
void bee_three()
{
	//if (buz_flg)return;
	switch (BEE_TWO)
	{
	default:
		spk_clr();
		BEE_TWO = BEEGO;
		break;
	case BEEGO:
	{
		audio_period = 0;
		audio_basic = 0;
		spk_clr();
		BEE_TWO=BEEONE;
		break;
	}
	case BEEONE:/*叫一声 */
	{
		spk_set();
		if (audio_basic++ > AUDIO_TIM)
		{
			BEE_TWO=BEETWO;
			audio_period = 0;
		}

		break;
	}
	case BEETWO:/*停20秒 */
	{
		spk_clr();
		if (audio_period++ > AUDIO_PERIOD)
		{
            
			BEE_TWO=BEETHREE;
		}
		break;
	}
	case BEETHREE:/* 设置参数*/
	{
		audio_period = 0;
		audio_basic = 0;
		spk_clr();
		BEE_TWO=BEEONE;
		break;
	}

	}
}

#if 0
// lwz 声音播放函数，参数cnt为声音播放的次数
void AUDIO_Sub(unsigned char cnt)/*used*/
{
	unsigned char  i;
	i=cnt;
	// lwz (SPEAK_flg==0)的条件是按键没有被长时间按下
	if ((key_val==KEY_MUT)&&(SPEAK_flg==0))
	{
		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		close_flg=0;

		if (mute_flg==0)
		{
			DISP_BuzClr(6,25);
		}
	}
	// lwz (SPEAK_flg==0)的条件是按键没有被长时间按下。反问：这里跟KEY_MUTL不矛盾吗？这里应该是在长按静音键时，设置SPEAK_flg为1
	if ((key_val==KEY_MUTL)&&(SPEAK_flg==0))
	{

		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		if (mute_flg)
		{
			close_flg=1;
		}
		else
		{
			close_flg=0;
		}

		if (mute_flg==0)// lwz 如果当前不是静音状态，则清除蜂鸣器标志
		{
			DISP_BuzClr(6,25);
		}
	}

	if (mute_flg)// lwz 静音模式
	{
		if (close_flg)// lwz 长按了静音按钮，进入了静音模式，则不再倒计时取消静音标志
		{
        if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
		{
			mute_flg=0;
			mute_tim=0;
			DISP_BuzClr(6,25);
            bee_three();
		}
			return;
		}
//		if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
//		{
//			mute_flg=0;
//			mute_tim=0;
//			DISP_BuzClr(6,25);
//		}
	}
	else
	{
		bee_three();// lwz 如果没有静音标志，则播放声音
	}
	// else
	// {
	// if (audio_period++>=AUDIO_PERIOD)
	// {
	// audio_basic++;
	// if (audio_basic<AUDIO_TIM)/////////////////占空比
	// {
	// SPEAK  =  1;
	// }
	// else if ( audio_basic<20 )/////////////////小周期
	// {
	// SPEAK  =  0;
	// SPEAK=0;
	// }
	// else
	// {
	// SPEAK  =  0;
	// audio_basic=0;
	// if (++audio_cnt>=i)
	// {
	// audio_period=0;
	// audio_cnt=0;
	// }
	// }
	// }
	// }
}
#endif

void AUDIO_Sub(unsigned char cnt)/*used*/
{
	unsigned char  i;
	i=cnt;
	// lwz (SPEAK_flg==0)的条件是按键没有被长时间按下
	if ((key_val==KEY_MUT)&&(SPEAK_flg==0))
	{
		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		//close_flg=0;

        if (mute_flg)
		{
			close_flg=1;
		}
		else
		{
			close_flg=0;
		}

		if (mute_flg==0)
		{
			DISP_BuzClr(6,25);
		}

		if (mute_flg)// lwz 静音模式
		{
			if (close_flg)// lwz 长按了静音按钮，进入了静音模式，则不再倒计时取消静音标志
			{
				if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
				{
					mute_flg=0;
					mute_tim=0;
					DISP_BuzClr(6,25);
					bee_three();
				}
				return;
			}
		}
		else
		{
			bee_three();// lwz 如果没有静音标志，则播放声音
		}
	}
	
	// lwz (SPEAK_flg==0)的条件是按键没有被长时间按下。反问：这里跟KEY_MUTL不矛盾吗？这里应该是在长按静音键时，设置SPEAK_flg为1
	if ((key_val==KEY_MUTL)&&(SPEAK_flg==0))
	{

		SPEAK_flg=1;
		mute_flg=~mute_flg;
		mute_tim=0;
		if (mute_flg)
		{
			close_flg=1;
		}
		else
		{
			close_flg=0;
		}
		if (mute_flg==0)// lwz 如果当前不是静音状态，则清除蜂鸣器标志
		{
			DISP_BuzClr(6,25);
		}
	}

	if (mute_flg)// lwz 静音模式
	{
		if (close_flg)// lwz 长按了静音按钮，进入了静音模式，则不再倒计时取消静音标志
		{
            if (mute_tim++>CANCEL_MUTEFLAG_TIMEOUT)
            {
              mute_flg=0;
			  mute_tim=0;
			  DISP_BuzClr(6,25);
              bee_three();
            }
			return;
		}
	}
	else
	{
		bee_three();// lwz 如果没有静音标志，则播放声音
	}
}

void BUZ_Cls(void);

// lwz 电池电量过低时，软关机；播放声音；
void AUDIO(void)/*used*/
{

	if ((bat_lev&0x0f)==LOWER_THAN_3_5V)
	{
		z1=z1|1;//lwz 设置z1的第0位
	}
	else
	{
		z1=z1&(~1);//lwz 取消z1的第0位
	}


	if (err_codea)// 如果发生了任何错误
	{
		z1=z1|2;//lwz 设置z1的第1位
	}
	else
	{
		z1=z1&(~2);//lwz 取消z1的第1位
	}
	if ((bat_lev&0x0f)==1)// lwz 低于3.7v
	{
		z1=z1|8;//lwz 设置z1的第3位
	}
	else
	{
		z1=z1&(~8);//lwz 取消z1的第3位
	}

	if (z1>bat_lev_bak)// 如果z1新设置了某个标志位（即发生了某个事件），则取消静音标识
	{
		mute_flg=0;
	}

	/******************************************************/
	if ((bat_lev&0x0f)==LOWER_THAN_3_5V)////////////////////////////////////
	{////////////////////////////////////////////////////////欠压报警
		mod_main_a  =  MOD_OFF;
		mod_main_b  =  MOD_OFF;
		audio_flg=LED_LOW_THAN_3_5V_OR_ERR;
		DISP_Led();
		if (bat_close_tim++<BELOW_3_5V_CYCLE)
		{
			AUDIO_Sub(1);
		}
		else
		{
			POWER_ON=0;// lwz 如果电压小于3.5V，且运行超过500个循环，则进行软关机
		}
	}

	else if (err_codea||err_codeb)
	{
		// lwz 除了漏气的同时充电线插上或者满电这种情况不报警，其他错误码情况都报警。即大部分情况都要报警。
		if (!((err_codea == ERR_AIR_LEAKAGE)&&((BAT_CHARGING ==BAT_CHARGE)||(bat_sas == BAT_FULL)&&(overabc == 1))))
		{
			AUDIO_Sub(1);
		}
		audio_flg=LED_LOW_THAN_3_5V_OR_ERR;
	}
	else
	{
		if (mod_main_a == MOD_OFF)
			return;
		if ((bat_lev&0x0f)==LOWER_BAT_WARN_3_6V)///////////////////////////////////低电量报警
		{
			AUDIO_Sub(1);
			audio_flg=LED_LOW_THAN_3_6V;
			DISP_Led();
		}
		else
		{
			audio_flg=LED_BAT_NORMAL;
			mute_flg=0;
			mute_tim=0;
			GRE    =  0;
		}
	}
	bat_lev_bak  =  z1;
	/******************************************************/
}

void BUZ_KeyCls(void)/*used*/
{
	buz_flg  =  0;
	buz_flg1 =  0;
	buz_cnt  =  0;
	spk_clr();
	TMR2     =  0;
	spk_clr();
}
void BUZ_Cls(void)/*used*/
{
	buz_flg  =  0;
	buz_flg1 =  1;
	buz_cnt  =  0;
	TMR2     =  0;
	spk_clr();
}

void AUDIO_Key(void)/*used*/
{
	if (key_val && key_silent_flag)// lwz 如果按键有值，则可以播放声音； 加上了key_silent_flag标志来静默按键
	{
		buz_flg=1;
	}

	if (buz_flg)// lwz 代表需要蜂鸣器响
	{
		if (buz_flg1==0)// lwz 判断是否正在播放声音，为0代表正在播放，1代表播放完毕
		{
			if (buz_cnt++>=BUZZER_TIME_CYCLE)
			{
				spk_clr();
				BUZ_Cls();// lwz 设置为声音已经播放过
			}
			else
			{
				spk_set();
			}
		}
	}
}

void FIND_FREE(void)
{
	if((mod_main_a==MOD_WAT)||(mod_main_a==MOD_SET)||(mod_main_a==MOD_ZHT)||(mod_main_a==MOD_TK))
	{
		TK_TIME++;//5*60*1000/20=15000
		if(TK_TIME>TK_TIMEOUT_CYCLE)
		{
			TK_TIME=TK_TIMEOUT_CYCLE+1;
			flager_a|=ERRB_TK;
			if(remember_mod==0)
				remember_mod=mod_main_a;
			mod_main_a=MOD_TK;
		}
		else
			flager_a&=~ERRB_TK;
	}
	else
	{
		TK_TIME=0;
		flager_a&=~ERRB_TK;
	}	
}

void main(void)
{
	SYS_OSC_Ini();
  asm("clrwdt"); //清看门狗
  	// lwz 从flash中读取数据，每次读取2个字节，即一个short类型
	dataREAD1 = Flash_Read(addr);
	dataREAD2 = Flash_Read(addr+2);
	dataREAD3 = Flash_Read(addr+4);
	dataREAD0 = Flash_Read(addr+6);

	dataK1    = Flash_Read(addr+8);
	dataK2    = Flash_Read(addr+10);
	dataK3    = Flash_Read(addr+12);
	dataK4    = Flash_Read(addr+14);
	key_silent_flag  = Flash_Read(addr+16);
	language = (key_silent_flag & 0xFF);// lwz 低字节为语言标志
#ifdef LOGO_TYPE_VR_CHINA
	if(language == 0xFF)// lwz 如果是国内版，烧录后的开机默认就是英文
		language = 0xFE;
#endif
	if(language != 0xFE)// lwz 默认只有英文、中文两种语言，如果读到的不是中文，则直接设置英文
		language = 0;
	key_silent_flag = (key_silent_flag & SILENT_FLAG_BITMASK);// lwz 静默标志在地址为addr+16的short类型的高字节的最高位
	// language=0;//默认英语
	mod_seta_prel  = Flash_Read(addr+18);// lwz 读取间隙模式下的低压值
	// lwz 初始化全局GPIO状态
	SYS_IO_Ini();
	// lwz 初始化定时器0
	SYS_TMR0_Ini();
	// lwz 初始化定时器3
	SYS_TMR3_Ini();
	// lwz 初始化系统中要用的各个全局变量
	SYS_DatIni();
	// lwz adc模数转换控制初始化
	adc_init();
	// lwz 加载工作模式、压力上下限等参数
	UART_Test();
	flager_a=0;
	mod_seta_prehh=0;
	bump_need_out_air_flg=0;
	BAT_WarnFir();// lwz 将adc采集所得的数据转换为当前的电压
	T3ON=1;// lwz 启动定时器3
	BEE_TWO=BEEGO;
	GIE =1;
	PEIE =1;
  asm("clrwdt"); //清看门狗
	clear_lqtimes();
	valueK  =  2.75f;
	while (1)
	{
	//	LATC|=0x10;
//		LATCbits.LATC4=1;
	  asm("clrwdt"); //清看门狗
		if (FLG_SYS_10MS)//实际改为了20ms TMR0定时器中设定了
		{
			FLG_SYS_10MS = 0;
		//	ClrWdt();
			if (mod_main_a!=MOD_SYS)
			{
				KEY_Scan();// lwz 按键扫描
				AUDIO_Key();// lwz 控制与按键对应的声音
			}
#ifdef PUMP_IDLE_FLAG
			FIND_FREE();// lwz 检查当前是否处于空的状态，清除和设置对应的状态位
#endif
			MODE_ProA();// lwz 根据当前模式，来决定处理方式
			MODE_Pro();// lwz 控制按键关机、锁屏、语言切换
			
			DISP_MainA();// lwz 根据当前模式，控制面板的显示
			// lwz 如果当前没有在系统初始化状态和空状态，则执行报警函数
			if ((mod_main_a!=MOD_SYS)||(mod_main_a==MOD_TK))
			{
				Warn();
				BAT_Warn();
				AUDIO();
			}

			if ((mod_main_a!=MOD_SYS)&&(mod_main_a!=MOD_OFF))/*统统关机时后不显示电池*/
			{
				if (mod_main_a!=MOD_WAT)// lwz 除了初始化、等待指令、关机三种状态，其他状态都显示电池信息
				{
					DISP_Bat();
				}
			}

			/*********************对大气放气****************************/
			if (bump_need_out_air_flg&0xf0)// lwz 如果要打开pwm，则设置VAL1，放气100个循环，以便于泵启动
			{
				// open_bum=1;
				if (bum_dly_flg==0)
				{
					// if (bum_dly++>=20)
					if (bum_dly++>=50)
					{
						open_bum=1;// lwz 准备启动泵
						// if (bum_dly>=30)//20160518
						//if (bum_dly>=100)
						if (bum_dly>=100)// lwz 放气100个循环后，关阀1
						{
							bum_dly_flg=1;
							VAL1=0;// lwz 关闭阀1
							bum_dly=0;
							show_lq_times.lq_times++;// lwz 放气次数加一
						}
					}
					else
					{
						//if(adc_ps00>79)//20160401:压力大于80启动翁翁叫
						//if(adc_ps00>200)//20160509:压力超了就打开,减少放气噪声
							VAL1=1;// lwz 打开阀1
					}
				}
			}
			else
			{
				open_bum=0;
				// VAL1=0;
				// VAL2=0;
				bum_dly_flg=0;
				VAL1=0;
				bum_dly=0;
			}

		}
	}
}




















