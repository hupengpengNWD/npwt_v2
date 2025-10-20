#include  "include.h"
#include "system_manager.h"
#include "global_compat.h"
#include "hardware_abstraction.h"
#include  "sys_cpu.h"
#include  "adc.h"
#include  "npwt_con_main.h"
 unsigned char  adc_ch,adc_cnt;
 unsigned short adc_buf[ADC_CHN*ADC_CNT];
 unsigned short adc_ps0,adc_bat = BAT4;
 unsigned short adc_zero;

void ADC_Sort(void)
{
	unsigned char i,j,k,l,m;
	unsigned short tem1;
	for(i=0;i<ADC_CHN;i++)
	{
	        j= i*ADC_CNT;
	        l= j+ADC_CNT-1;
	        for(m=0;m<(ADC_CNT-1);m++)
	        {
				for(k=j;k<l;k++)
				{
					if(adc_buf[k]>adc_buf[k+1])
					{
						tem1=adc_buf[k+1];
						adc_buf[k+1]=adc_buf[k];
						adc_buf[k]=tem1;
				    }
				}
	            l--;
	        }	
	}	
}

static unsigned short bat_value_buf[10];
static unsigned char bat_i =0;
static unsigned short bat_scan_filter(unsigned short pdata)
{
	bat_value_buf[bat_i++] = pdata;
	unsigned char count= 0;
	unsigned short sum = 0;
	if ( bat_i == 10 )bat_i = 0;
	for ( count=0;count<10;count++)sum += bat_value_buf[count]; 
	sum=(unsigned short)(sum/10);
		if(sum>998)sum=999;
		if(sum<1)sum=0;
   return (unsigned short)sum;
}

/****************************************************************************
 * 【架构重构】
 * 局部变量改为static，避免全局污染
 ****************************************************************************/
static unsigned short delay_adc = 249;
static unsigned short delay_P_adc = 249;
void ADC_DatCal(void)
{

	if (++delay_P_adc > 3)
	{
		adc_ps0 = adc_reader_press();
		delay_P_adc = 0;
	}
	
	if (++delay_adc > 250)
	{
		adc_bat = adc_reader_power();	
		delay_adc = 0;
	}
}

void ADC_Ps90(void)
{

				ADC_DatCal();

}

