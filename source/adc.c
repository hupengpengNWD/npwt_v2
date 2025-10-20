/****************************************************************************
 * 文件名: adc.c
 * 功能: ADC模数转换驱动模块
 * 
 * 主要功能:
 *   1. 多通道ADC采集
 *   2. 数字滤波（采样3次取平均）
 *   3. 采集通道：AN0（压力），AN1（电池），AN2（液位）
 * 
 * 采集频率: 每5ms采集一次（由定时器3中断触发）
 * 
 * 数据处理:
 *   - 压力传感器：adc_ps0（10位ADC值，对应0~320mmHg）
 *   - 电池电压：adc_bat（10位ADC值，对应3.5~4.2V）
 *   - 液位传感器：adc_yew（10位ADC值）
 * 
 * 调用关系:
 *   被调用: ADC_Ps90()由定时器3中断调用
 ****************************************************************************/

#include	 "include.h"
#include "system_manager.h"
#include "global_compat.h"
#include "hardware_abstraction.h"
#include	 "sys_cpu.h"
#include  "npwt_con_main.h"

 void adc_init(void)
{
	ANCON0bits.PCFG0 = 0;
	ANCON0bits.PCFG1 = 0;
	ANCON0bits.PCFG2 = 0;

	ADCON0  =  0x01;
	ADCON1  =  0xbe;
	ADCON0bits.ADON=1;
}

static unsigned short adc_read(unsigned char channel)
{
	unsigned short ADC=0;
	unsigned char temp=0;
	ADCON0bits.VCFG=0b0;
	ADCON0bits.CHS=channel&0xf;
	ADCON0bits.GO_DONE=1;
	while(ADCON0&0x02); 
	ADC=(unsigned short)(((unsigned short)ADRESL|((unsigned short)ADRESH<<8)))&0x3ff;
	if(ADC>1023)ADC=1023;
	if(ADC<1)ADC=0;
	ADCON0bits.GO=0;
	return(ADC);	
}

#define  COUNT_PS 3
static short value_press_buf[COUNT_PS];
static unsigned char PS_i=0;
static unsigned short adc_press_filter(unsigned char ch)
{
	unsigned char count;
	short sum=0;
	value_press_buf[PS_i++] = (short)(adc_read(ch));
	if ( PS_i == COUNT_PS )PS_i = 0;
	for ( count=0;count<COUNT_PS;count++)sum += value_press_buf[count]; 
	sum=(unsigned short)(sum/COUNT_PS);
		if(sum>1023)sum=1023;
		if(sum<1)sum=0;
   return (unsigned short)sum;
}

#define COUNT_PW 10
static short value_power_buf[COUNT_PW]={
	BAT4,BAT4,BAT4,BAT4,BAT4,
	BAT4,BAT4,BAT4,BAT4,BAT4,
};
static unsigned char Pw_i=0;
static unsigned short adc_power_filter(unsigned char ch)
{
	unsigned char count;
	short sum=0;
	value_power_buf[Pw_i++] = (short)(adc_read(ch));
	if ( Pw_i == COUNT_PW )Pw_i = 0;
	for ( count=0;count<COUNT_PW;count++)sum += value_power_buf[count]; 
	sum=(unsigned short)(sum/COUNT_PW);
		if(sum>1023)sum=1023;
		if(sum<1)sum=0;
   return (unsigned short)sum;
}

unsigned short  adc_reader_press(void)
{
	 unsigned short p = adc_press_filter(0);
	return p;
}

unsigned short  adc_reader_power(void)
{
	 unsigned short p = adc_power_filter(2);
	return p;
}

