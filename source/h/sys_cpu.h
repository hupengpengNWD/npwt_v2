#ifndef  _sys_cpu_h_
#define  _sys_cpu_h_

typedef  unsigned char  BOOLEAN;
typedef  unsigned char  INT8U;                    
typedef  signed   char  INT8S;                    
typedef  unsigned short INT16U;                   
typedef  signed   short INT16S;                   

typedef  float          FP32;                     

#define DISP_TRUE_DATA    0   

#define SetLedRed()	{LATB  =  LATB&0xef; LATB  =  LATB|0x20;}

#define  ClrBlk()	{LATC  =  LATC&0xbf;}

extern void SetBlk(void);

 #endif

	 