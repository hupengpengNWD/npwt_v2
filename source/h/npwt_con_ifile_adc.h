#ifndef __npwt_con_ifile_adc_h_
#define __npwt_con_ifile_adc_h_

extern  unsigned char  adc_ch,adc_cnt;
extern  unsigned short adc_buf[ADC_CHN*ADC_CNT];
extern  unsigned short adc_ps0,adc_bat;
extern  unsigned short adc_zero;

extern void ADC_Sort(void)/*used*/;

extern void ADC_DatCal(void)/*used*/;

extern void ADC_Ps90(void)/*used*/;

#endif

