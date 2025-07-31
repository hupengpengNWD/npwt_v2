#ifndef _bios_jlx1864g_139_h_
#define _bios_jlx1864g_139_h_

#define    CS     LATEbits.LATE2
#define    RES    LATAbits.LATA6
#define    RS     LATAbits.LATA7
#define    RD     LATEbits.LATE0
#define    WR     LATEbits.LATE1
#define    P1     LATD
#define   JLX12864G_ON    0xaf
#define   JLX12864G_OFF   0xae
#define   JLX12864G_RES   0xe2


#define DISP_ClrA() {DISP_ClrZero(0,8,0,64);}
	
#define DISP_ClrA1() {DISP_ClrZero(0,2,0,64);}
	
#define DISP_ClrA2()	{DISP_ClrZero(2,4,0,64);}

#define DISP_ClrA3()	{DISP_ClrZero(4,6,0,64);}

#define DISP_ClrA4()	{DISP_ClrZero(6,8,0,64);}	

#define DISP_ClrB()	{DISP_ClrZero(0,8,65,128);}

#define DISP_ClrB1()	{DISP_ClrZero(0,2,65,128);}

#define DISP_ClrB2()	{DISP_ClrZero(2,4,65,128);}

#define DISP_ClrB3()	{DISP_ClrZero(4,6,65,128);}

#define DISP_ClrB4()	{DISP_ClrZero(6,8,64,128);}

#define DISP_Clr1()/*used*/	{DISP_ClrZero(0,2,0,128);}

#define DISP_Clr2()/*used*/	{DISP_ClrZero(2,4,0,128);}

#define DISP_Clr3()/*used*/	{DISP_ClrZero(4,6,0,128);}

#define DISP_Clr4()/*used*/	{DISP_ClrZero(6,8,0,128);}

#define  DISP_ClrD2()	{DISP_ClrZero(4,8,46,80);}

#define  DISP_ClrD1()	{DISP_ClrZero(4,8,46,80);}

extern unsigned short  val_val;

extern void BIOS_JLX12864_TRANS_CMD(int data);   /*used*/ 

extern void BIOS_JLX12864_TRANS_DAT(int data); /*used*/

extern void BIOS_JLX12864_DELAY(int i);/*used*/

extern void SYS_IniLcd(void);/*used*/

extern void DISP_DigBasic(unsigned char dat,unsigned char startx,unsigned char starty);

extern void DISP_DigBasic32(unsigned short dat,unsigned char startx,unsigned char starty);/*used*/

// extern void DISP_Dig12_166(unsigned char startx, unsigned char starty,unsigned short disp_val);

extern void DISP_ChaBasic(unsigned char dat,unsigned char startx,unsigned char starty);/*used*/


extern void DISP_BatWarnC(void); /*used*/

extern void DISP_BatWarnE(void); /*used*/

extern void DISP_Clear(void); /*used*/
extern void DISP_Clear22(void); /*used*/

extern void DISP_ClrZero(unsigned char x,unsigned char x1,unsigned char y,unsigned char y1);/*used*/

//extern void DISP_Bat00(unsigned char startx,unsigned char starty);

extern void DISP_Bat00Clr(unsigned char startx,unsigned char starty);/*used*/

extern void QQQQQQQQQQQQQQDISP_Bat01(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Bat000(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Bat001(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Bat002(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Bat003(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Bat004(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Buz(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_BuzClr(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Lock(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_LockClr(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_lx(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_jx(unsigned char startx,unsigned char starty);/*used*/

	
extern void DISP_Fu(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Fu1(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Dig15_32(unsigned char startx, unsigned char starty);/*used*/

extern void DISP_DigBasic40(unsigned short dat,unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Dig18_40(unsigned char startx, unsigned char starty);/*used*/

extern void DISP_pset(unsigned char startx,unsigned char starty);/*used*/
extern void DISP_pset_low(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_DigBasic0(unsigned char dat,unsigned char startx,unsigned char starty);

extern void DISP_Dig12_16(unsigned char startx, unsigned char starty,unsigned short disp_val);/*used*/

extern void DISP_DigBasics12(unsigned char dat,unsigned char startx,unsigned char starty);/*used*/

extern void DISP_DigBasic00(unsigned char dat,unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Dig14_16(unsigned char startx, unsigned char starty,unsigned short disp_val);/*used*/


extern void DISP_Sys100(void);/*used*/

extern void DISP_Ver(void);/*used*/

extern void DISP_ask(void);/*used*/
extern void DISP_LANGUAGE(void);/*used*/
extern void DISP_set_sanjiao(unsigned char x,unsigned y);
extern void DISP_set_sanjiao0(unsigned char x,unsigned y);
extern void DISP_mod_set2015(void);/*used*/
extern void DISP_press_set2016(void);/*used*/

extern void DISP_time_set2015(void);/*used*/

extern void DISP_run2015(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_press2015(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_stop2015(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_select(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_select0(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_block(unsigned char startx,unsigned char starty);/*used*/
extern void DISP_louq(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_didl(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_yewm(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_ChaBasic2(unsigned char dat,unsigned char startx,unsigned char starty);/*used*/

extern void DISP_8X16ascii_block(char *s,unsigned char x,unsigned char y);/*used*/

extern void DISP_cha7s(char *s,unsigned char x,unsigned char y);/*used*/

extern void DISP_ChaBasic2015(unsigned char dat,unsigned char startx,unsigned char starty);/*used*/

extern void DISP_8X16ascii( char *s,unsigned char x,unsigned char y);/*used*/

extern void DISP_key2015(unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Dig0(unsigned char dat,unsigned char startx,unsigned char starty);/*used*/

extern void DISP_block_new(unsigned char dat,unsigned char startx,unsigned char starty);/*used*/

extern void DISP_Dig1(unsigned char startx, unsigned char starty,unsigned short disp_val);/*used*/
#endif