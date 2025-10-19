#include  "include.h"
#include  "sys_cpu.h"
#include  "adc.h"
#include  "npwt_con_main.h"
 unsigned char  adc_ch,adc_cnt;
 unsigned short adc_buf[ADC_CHN*ADC_CNT];
 unsigned short adc_ps0,adc_bat = BAT4;// lwz adc_ps0ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½Öµ
 unsigned short adc_zero;// lwz ï¿½ï¿½Ç°ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½Öµï¿½ï¿½ï¿½ï¿½Öµï¿½Ú¿ï¿½ï¿½ï¿½Ê±ï¿½Í½ï¿½ï¿½Ð²É¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Öµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Î¿ï¿½Öµï¿½ï¿½ï¿½ï¿½ï¿½ï¿½É¼ï¿½ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½Ð±ä»¯ï¿½ï¿½ï¿½ï¿½ï¿½Ü»áµ¼ï¿½Âµï¿½Ç°ï¿½ï¿½Ñ¹ï¿½Ä±ä»¯ï¿½ï¿½Ò²ï¿½ï¿½Ó°ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ÞµÄ¼ï¿½ï¿½ã£¬ï¿½ï¿½ï¿½ç£¬ï¿½ï¿½ï¿½ï¿½É¼ï¿½ï¿½ï¿½ï¿½Ä³ï¿½Ê¼ï¿½ï¿½Ñ¹ï¿½ï¿½ï¿½ï¿½143ï¿½ï¿½ï¿½ï¿½ï¿½Ð¿ï¿½ï¿½Ü»áµ¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ñ¹ï¿½ï²»ï¿½ï¿½320
/*

300mmHg  adc_ps0=841
108              363
202              623  
0                66
319              946
*/

void ADC_Sort(void)/*used*/
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

unsigned short  delay_adc = 249;
unsigned short  delay_P_adc = 249;
void ADC_DatCal(void)/*used*/
{
	// unsigned short bat_temp =adc_buf[(ADC_CNT/2+2*ADC_CNT)]; 

	// adc_ps0 = adc_buf[(ADC_CNT/2)];
	if (++delay_P_adc > 3)
	{
		adc_ps0 = adc_reader_press();
		delay_P_adc = 0;
	}
	// adc_bat = bat_scan_filter(bat_temp);
	if (++delay_adc > 250)
	{
		adc_bat = adc_reader_power();	
		delay_adc = 0;
	}
}


void ADC_Ps90(void)/*used*/
{
		// unsigned short i;
		// ADCON0 = ADCON0|0x02;///////start
		// while(ADCON0&0x02);
		// i= ADRESH&0x03;
		// i=(i<<8)+ADRESL;
		// adc_buf[ADC_CNT*adc_ch+adc_cnt]=i;
		// if(++adc_ch>=ADC_CHN)
		// {
			// adc_ch = 0;
			// if(++adc_cnt>=ADC_CNT)
			// {
				// adc_cnt=0;
				// ADC_Sort();
				ADC_DatCal();
			// }    
		// }
		// else
			// {adc_ch = 2;}
		// ADCON0 =( 1+(adc_ch<<2) );              
}




