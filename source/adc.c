#include	 "include.h"
#include	 "sys_cpu.h"
#include  "npwt_con_main.h"

/*
---------------------------------------------------------------------------------------------------------
*********************************************************************************************************
** º¯ÊýÃû³Æ #:adc_init
** ¹¦ÄÜËµÃ÷ #:³õÊ¼»¯ADCÏà¹ØµÄÒý½Å¡£Ê¹¶ÔÓ¦µÄ¹Ü½Å½øÈëAD¹¤×÷Ä£Ê½
** Éè¼ÆËµ #:
** ÊäÈë½Ó #:ÎÞ
** Êä³ö½Ó¿Ú #:ÎÞ
** ËµÃ÷       #:ÎÞ
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
** º¯ÊýÃû³Æ £ºadc_read
** ¹¦ÄÜËµÃ÷ £º¶ÁÈ¡ÖÆ¶¨Í¨µÀµÄADÖµ
** Éè¼ÆËµÃ÷ £º
** ÊäÈë½Ó¿Ú £ºchannel ---ADÍ¨µÀ±àºÅ
** Êä³ö½Ó¿Ú £ºADC----¶ÁÈ¡µÄADÊý¾Ý
** ËµÃ÷       £ºADC0==µç³ØµçÑ¹ ADC3==ÒºÃæ¼ì²â  ADC8==¸ºÑ¹¼ì²â 
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
** º¯ÊýÃû³Æ £ºadc_filter
** ¹¦ÄÜËµÃ÷ £º¶ÔÖÆ¶¨Í¨µÀµÄADÖµ½øÐÐÂË²¨¿¹¸ÉÈÅ´¦Àí
** Éè¼ÆËµÃ÷ £º¶Ô¶ÁÈ¡µÄADÖµÈ¥»ùÖµÈ»ºó×ª»»³É¸ºÑ¹Öµ
** ÊäÈë½Ó¿Ú £ºch ---ADÍ¨µÀ±àºÅ
** Êä³ö½Ó¿Ú £ºsum---¶ÁÈ¡µÄADÊý¾Ý
** ËµÃ÷       £ºADC0==µç³ØµçÑ¹  ADC8==¸ºÑ¹¼ì²â 
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

