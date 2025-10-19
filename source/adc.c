#include	 "include.h"
#include	 "sys_cpu.h"
#include  "npwt_con_main.h"

/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** 函数名称 #:adc_init
** 功能说明 #:初始化ADC相关的引脚。使对应的管脚进入AD工作模式
** 设计说 #:
** 输入接 #:无
** 输出接口 #:无
** 说明       #:无
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------
*/
 void adc_init(void)
{
	ANCON0bits.PCFG0 = 0;
	ANCON0bits.PCFG1 = 0;
	ANCON0bits.PCFG2 = 0;




	
	ADCON0  =  0x01;
	ADCON1  =  0xbe;
	ADCON0bits.ADON=1;
}

/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** 函数名称 ：adc_read
** 功能说明 ：读取制定通道的AD值
** 设计说明 ：
** 输入接口 ：channel ---AD通道编号
** 输出接口 ：ADC----读取的AD数据
** 说明       ：ADC0==电池电压 ADC3==液面检测  ADC8==负压检测 
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------
*/
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
	return(ADC);	// return 8 MSB of the result
}
/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** 函数名称 ：adc_filter
** 功能说明 ：对制定通道的AD值进行滤波抗干扰处理
** 设计说明 ：对读取的AD值去基值然后转换成负压值
** 输入接口 ：ch ---AD通道编号
** 输出接口 ：sum---读取的AD数据
** 说明       ：ADC0==电池电压  ADC8==负压检测 
*********************************************************************************************************
---------------------------------------------------------------------------------------------------------
*/
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

