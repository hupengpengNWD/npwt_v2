#ifndef  _sys_cpu_h_
#define  _sys_cpu_h_

typedef  unsigned char  BOOLEAN;
typedef  unsigned char  INT8U;                    /* Unsigned  8 bit quantity                           */
typedef  signed   char  INT8S;                    /* Signed    8 bit quantity                           */
typedef  unsigned short INT16U;                   /* Unsigned 16 bit quantity                           */
typedef  signed   short INT16S;                   /* Signed   16 bit quantity                           */
///typedef  unsigned int   INT16U;                   /* Unsigned 32 bit quantity                           */
///typedef  signed   int   INT16S;                   /* Signed   32 bit quantity                           */
typedef  float          FP32;                     /* Single precision floating point                    */
///typedef  double         FP32;                     /* Double precision floating point                    */

#define DISP_TRUE_DATA    0   /*ÏÔÊ¾Êµ¼ÊÊý¾Ý*/

#define SetLedRed()	{LATB  =  LATB&0xef; LATB  =  LATB|0x20;}

//#define  ClrLedRed()	{LATB  =  LATB&0xdf;}

//#define  SetLedYel()	{LATB  =  LATB&0xdf; LATB  =  LATB|0x10;}

//#define  ClrLedYel()	{LATB  =  LATB&0xef;}

#define  ClrBlk()	{LATC  =  LATC&0xbf;}

extern void SetBlk(void);

 #endif
	 

	 
	 