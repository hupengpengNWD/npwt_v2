#include  "include.h"
#include "npwt_con_over.h"
#include  "npwt_con_ofile_load_00.h"
#include  "npwt_dis_ifile_key_00.h"
#include   "npwt_dis_sys_ini_00.h"
#include   "npwt_dis_sys_uart_00.h"
#include "npwt_dis_main.h"
#include "npwt_con_ifile_adc.h"

unsigned short   fall_cnta0 ;

unsigned char    fall_stata ;

unsigned char    con_flg_falla ;// lwz 1代表耗材盒需要放气、0代表耗材盒不用放气
unsigned short   gao_cnt ;
unsigned char    good_check=0 ;
unsigned char    nggood_check=0 ;
unsigned short   BOX_FQ_TURNS ;

unsigned short   tim_tima=0 ;

unsigned short   adc_ps00;// lwz 当前的压力值
// unsigned char    det300 = 0;
//20160505日修改，减少液位满误报警，过冲值
// unsigned char    det300 = 20;


//20160506日修改，减少液位满误报警，过冲值
// unsigned char    det300 = 15;
//20160603 日修改，陈巧林反应20mmhg的时候液位满不报警
unsigned char    det300 = 10;
unsigned char    det00 ;
unsigned short con_hi_delta = 0;// 高压步距
unsigned short con_lo_delta = 0;// 低压步距

unsigned short debug_times = 0;
unsigned short debug_thirtys = 0;
unsigned short debug_air=0;
unsigned char  debug=0;
void CONTR_fallaNew(void)
{
	if (fall_stata==0)
	{
		fall_stata=1;
		fall_cnta0=0;
		VAL2 = 1 ;
		//20160725 修改在间隙模式下，报管路堵塞后，进入低压模式不消除报警
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// 只有当前气压大于设定压力，后才有con_flg_falla=1；
	}
	else if (fall_stata==1)
	{
		fall_cnta0++;
		unsigned short fall_cnta0_count = 4;
		if(mod_seta_preh <=50)// 低压泄气时间延长
			fall_cnta0_count = 16;
		if (fall_cnta0>fall_cnta0_count) 
		{//////////////////////0.1
			VAL2 = 0 ;
			fall_stata=2;
			fall_cnta0=0;
		}
		//20160725 修改在间隙模式下，报管路堵塞后，进入低压模式不消除报警
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// 只有当前气压大于设定压力，后才有con_flg_falla=1；
	}
	else if (fall_stata==2)
	{
		fall_cnta0++;
		if (fall_cnta0>50)  //////30
		{//////////////////////0.1秒
			fall_stata=0;
			fall_cnta0=0;
			if (adc_ps00<(mod_seta_preh+con_hi_delta))// lwz 泄气达到目标
			{
				// if(debug_air!=0)
				// {
					// mod_seta_preh=debug_air;
					// debug_air=0;
				// }	
				con_flg_falla=0;//盒子不用放气
				gao_cnt=0;
			}
		}
		//20160725 修改在间隙模式下，报管路堵塞后，进入低压模式不消除报警
		record_ds_turn=0;
		flager_a &=~ERRB_DS;// 只有当前气压大于设定压力，后才有con_flg_falla=1；
		
	}
}

void get_press_delta(unsigned short press)
{
	unsigned short delta = 8;
	// lwz 更改：除了Deroyal版意外，其他的版本将浮动阈值扩大，尽量减少泵的启动次数
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
	con_lo_delta = delta;
	con_hi_delta = delta+4;
#else
	con_lo_delta = press/10;// lwz 非deroyal版，10%的补气判定条件
	// lwz 对于充气停止条件，做出调整
	if (press > 200)
	{
		con_hi_delta = press/20;
	}
	else if (press >= 80)
	{
		con_hi_delta = press/20+4;
	}
	else
	{
		con_hi_delta = press/10+2;
	}
#endif
	// 20160526 正15，负10
} 




/*----------------16位有符号数计算差值-----------------------------------------*/
static unsigned short calc_delta_i(unsigned short counts1,unsigned short counts2 )
{
	unsigned short rst = 0;
	if ( counts1 >= counts2 )
		{rst = (counts1 - counts2);}
	if (rst<5)
	{rst = 5;
	}
		return rst;
}
unsigned char oppump_flg = 0;
unsigned short  np_good_times=0;
unsigned char   overabc = 0;// lwz 为1，表示漏气超过10分钟
unsigned short   val_cnt=0;
unsigned long scan_dusai_time = 0;// lwz 判断堵塞时间，在补气和切换主模式的时候，这个标志会清零
unsigned char xxturn=0;
unsigned char xxok=0;
signed int result=0;

unsigned short record_ds[8];
unsigned char  record_ds_turn=0;// 当前的数组索引
unsigned short twenty_seconds=0;
// lwz 根据当前的压力做出判断，处理压力过小、过大、液位满、漏气等情况
void STAT_conNewa(void)/*used*/
{
	unsigned short temp=0;
    unsigned short run_time=0;
	if (mod_seta_preh==0)/*如果设定的压力是0就不要玩*/
	{
		CLS_PwmA();
		if (val_cnt++>1000)
		{
			VAL2 = 0;
			val_cnt=1100;
		}
		else
		{
			VAL2=1;
		}
		return;
	}
	else
	{
		get_press_delta(mod_seta_preh);
		val_cnt=0;
	}
	if (flager_a&ERRA_S)/*液位满*/
	{
		np_good_times=0;
		VAL1 = 0 ;
		VAL2 = 0 ;
		//bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
		CLS_PwmA();//////////////////////液位满需要取消后才能使用
	}
	else
	{
		temp = calc_delta_i(mod_seta_preh,con_lo_delta);
		unsigned short target_press = mod_seta_preh+con_hi_delta;
		if(target_press >= TOP_PRESSURE_MAX)
			target_press = TOP_PRESSURE_MAX-3;
		if (adc_ps00<=temp)//压力小于底限处理
		{
			
			record_ds_turn=0;
			twenty_seconds=0;
			
			nggood_check++;
			if(nggood_check>49)
			{
				nggood_check=0;
				gao_cnt=0;
			}
			good_check=0;
			np_good_times=0;
			scan_dusai_time = 0;
			debug_times=0;
			debug_thirtys=0;
			flager_a &=~ERRB_DS;// 只有当前气压大于设定压力，后才有con_flg_falla=1；
			if (con_flg_falla==0)//盒子不需要放气
			{
				BOX_FQ_TURNS++;
				if (BOX_FQ_TURNS>10)/*泄气的状态没有完成前不能开泵:40ms*/  
				//if (BOX_FQ_TURNS>99)/*泄气的状态没有完成前不能开泵:500ms*/   //20160509
				{
					BOX_FQ_TURNS=0;
					
					if (mod_jixa!=1)
						VAL2 = 0 ;//盒子放气阀关闭
					else
					{
						if(ddfq==2)	
							VAL2 = 0 ;//盒子放气阀关闭
					}
					// if(debug_air!=0)
					// {
						// mod_seta_preh=debug_air;
						// debug_air=0;
					// }
					if (flager_a&ERRA_LQ)//如果确认已经有了漏气标志
					{
					    // lwz 九梅反馈间歇模式的漏气时间不足，需单独针对间歇模式来调整，原时间：1分12秒
						if (load_perioda_up>JUDGE_LEAKAGE_TIMEOUT+1200  // 连续模式下，漏气超过10分钟
						    || load_perioda_up>(JUDGE_LEAKAGE_TIMEOUT)*JX_MODE_TIME_FACTOR_BY_LX // 间歇模式下漏气超过10分钟
							)/*漏气超10分钟*/
#if IS_NO_LIQUID_ALARM_STOP==0
						{
							//泵上的阀VA1不要放气
							//bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
							CLS_PwmA();// lwz 如果发生了漏气，则停泵
							overabc = 1;
						}
						else
#endif
						{
							//泵上的阀VA1要放气一欠,准备打开泵充气
							//bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
							OPEN_PwmA();
							overabc = 0;
						}
					}
					else
					{
						//泵上的阀VA1要放气一欠,准备打开泵充气
						//bump_need_out_air_flg=bump_need_out_air_flg|0xf0;
						OPEN_PwmA();
						overabc = 0;
					}
				}
			}
		}
		//else if (adc_ps00>=(mod_seta_preh+con_hi_delta))//压力大于上限处理

		//else if	(((adc_ps00>=mod_seta_preh+17))||((mod_seta_preh>=200)&&(adc_ps00>=mod_seta_preh+12))||((mod_seta_preh<=100)&&(adc_ps00>=mod_seta_preh+7)))
		else if (adc_ps00>=(target_press))//压力大于上限处理
		{
			gao_cnt++;
			if (gao_cnt>999)
				gao_cnt=1000;
			// lwz 根据当前设定的压力来判断需要等待高压的时间，目标压力越高，等待的时间就越长，等待时间=目标压力*2
            run_time=mod_seta_preh/10+2;
			// lwz 对于小于等于50mmhg，泵可以延迟启动，以确保泵不会多次启动
			if(run_time < 15)
			{
				run_time=15;
			}
			// lwz 测试中发现，间歇模式下，保压时间不够，在这里补偿一点
			if(mod_main_a == MOD_JIX)
			{
				run_time=run_time+4;
			}
			run_time=run_time*20;
			if(run_time<=20)
				run_time=20;
			if(run_time>=400)
				run_time=400;
            if (gao_cnt>run_time) //20160606 lwz 时间到，关泵
			{
				CLS_PwmA();
				PUMP=0;
				open_bum=0;
			}
            if (gao_cnt>(run_time+JUDGE_YWM_AFTER_PUMP_STOP)) 
			{
/*  				if (oppump_flg)//防止液位满误报警
				{
					cnt_cnta++;	//20秒内4次震荡累计
					oppump_flg = 0;
					con_flg_falla=1;
					fall_stata=0;
					fall_cnta0=0;
					gao_cnt=0;
				} 	 */
	// lwz 如果高压计数大于499，并且泵在上一次震荡累计之后开过，即气压：adc_ps00<=(mod_seta_preh-10)条件被执行过
 				if (con_flg_falla==0)//防止液位满误报警 lwz 当前不在液位满的放气阶段
				{
					cnt_cnta++;	//20秒内4次震荡累计
					oppump_flg = 0;
					con_flg_falla=1;
					fall_stata=0;// lwz 放气开VAL2标志
					fall_cnta0=0;
					gao_cnt=0;
				} 	
				
			}
			record_ds_turn=0;
			twenty_seconds=0;
			good_check=0;
			np_good_times=0;
		}
		else//压力大于底限小于上限
		{
			#if IS_NO_LIQUID_ALARM_STOP==0
			if (flager_a&ERRA_LQ)
			{
				if (load_perioda_up>JUDGE_LEAKAGE_TIMEOUT+1200
				|| load_perioda_up>(JUDGE_LEAKAGE_TIMEOUT)*JX_MODE_TIME_FACTOR_BY_LX // 间歇模式下漏气超过10分钟
				)
				{
					//bump_need_out_air_flg=bump_need_out_air_flg&0x0f;
					CLS_PwmA();//不要开泵上的放气阀 	lwz 关泵
				}
			}
			#endif
			if (scan_dusai_time++>JUDGE_DS_TIME)/*如果持续15分钟内没有补气、放气表示有堵塞*/
			{
				flager_a|=ERRB_DS;
			}
			
            good_check++;
			if(good_check>49)
			{
				nggood_check=0;
				good_check=0;
				gao_cnt=0;
			}
			//20160524 add mafacai///////////
			twenty_seconds++;
			// lwz 低压状态下，
			unsigned short record_pre_compare_interval;
			if(adc_ps00 >= MIN_PRESS_REPORT_DS)
			{
				// lwz 间歇模式的管路堵塞报警周期计算时应该加倍，这样得到的实际时间才能准确
				if(mod_main_a == MOD_JIX)
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL*JX_MODE_TIME_FACTOR_BY_LX;
				}
				else
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL;
				}
			}
			else
			{
				// lwz 间歇模式的管路堵塞报警周期计算时应该加倍，这样得到的实际时间才能准确
				if(mod_main_a == MOD_JIX)
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL_LOW*JX_MODE_TIME_FACTOR_BY_LX;
				}
				else
				{
					record_pre_compare_interval = RECORDE_PRESSURE_INTERVAL_LOW;
				}
			}
			if(twenty_seconds>=record_pre_compare_interval)//180000ul/15/3  每隔15秒查检一次，
			{
				record_ds[record_ds_turn]=adc_ps00;
				record_ds_turn++;
				if(record_ds_turn>7)
				{
					if((record_ds[7] == record_ds[0])||((record_ds[7]+1) == record_ds[0]))
					{
						// lwz 这部分代码的目的是为了提高在低压状态下判定管路堵塞的门槛
						// lwz 如果大于此值，则直接设置为管路堵塞
						if(record_ds[7] >= MIN_PRESS_REPORT_DS)
						{
							flager_a|=ERRB_DS;// lwz 压力前后没有变化，设置堵塞标志
						}
						else if(record_ds[7] > NO_REPORE_PRESS_FOR_DS)
						{
							// lwz 如果大于NO_REPORE_PRESS_FOR_DS，则计算数组平均值，如果等于record_ds[7]，才判定为管路堵塞
							if((record_ds[0]+record_ds[1]+record_ds[2]+record_ds[3]+record_ds[4]+record_ds[5]+record_ds[6])/7 == record_ds[7])
							{
								flager_a|=ERRB_DS;// lwz 压力前后没有变化，设置堵塞标志
							}
						
}// lwz 小于NO_REPORE_PRESS_FOR_DS，则不报管路堵塞
					}
					else
					{
						flager_a &=~ERRB_DS;// 只有当前气压大于设定压力，后才有con_flg_falla=1；
					}
					
					record_ds[0]=record_ds[1];
					record_ds[1]=record_ds[2];
					record_ds[2]=record_ds[3];
					record_ds[3]=record_ds[4];
					record_ds[4]=record_ds[5];
					record_ds[5]=record_ds[6];
					record_ds[6]=record_ds[7];
					record_ds_turn=7;
				}
				twenty_seconds=0;
			}
			//////////////////////////////////
				
		}
		if (con_flg_falla)//当前压力大于设定压力+上限，给盒子放气一小会儿，执行完后con_flg_falla=0；
		{
			CONTR_fallaNew();
		}
	}
}

























