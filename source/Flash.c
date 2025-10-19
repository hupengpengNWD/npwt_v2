
/****************************************************************************
 * 文件名: Flash.c
 * 功能: Flash非易失性存储器读写驱动
 * 
 * 主要功能:
 *   1. Flash擦除（按64字节块擦除）
 *   2. Flash写入（每次2字节）
 *   3. Flash读取（每次2字节）
 * 
 * 存储数据（起始地址0xa000）:
 *   addr+0:  配置数据1
 *   addr+2:  配置数据2
 *   addr+4:  配置数据3
 *   addr+6:  配置数据0
 *   addr+8:  压力校准系数K1
 *   addr+10: 压力校准系数K2
 *   addr+12: 压力校准系数K3
 *   addr+14: 压力校准系数K4
 *   addr+16: 静音标志（高字节）+ 语言选择（低字节）
 *   addr+18: 上次设置的目标压力值
 * 
 * 调用关系:
 *   被调用: main()初始化时读取，UART_Test()时写入
 ****************************************************************************/

#include   "include.h"

void Write_Cycle(void)
{

	WREN = 1;		

	CARRY = 0;
	if(GIE) 
		CARRY = 1;
	GIE = 0;  		

	EECON2 = 0X55;
	EECON2 = 0XAA;

	WR = 1; 		
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	while(WR) ;		
	WREN = 0;		

	if(CARRY) 
		GIE = 0;
}

void Flash_Erase(unsigned long address)
{
	TBLPTRL = ((address) & 0xFF);
	TBLPTRH = (((address) >> 8) & 0xFF);
	TBLPTRU = (((address) >> 8) >> 8);

	FREE = 1;  		
	Write_Cycle();
}

void Write_One_Word(unsigned long address,unsigned int data)
{

	TBLPTRL = ((address) & 0xFF);
		TBLPTRH = (((address) >> 8) & 0xFF);
		TBLPTRU = (((address) >> 8) >> 8);

	TABLAT = data>>8;
  	asm("\tTBLWT*+");
	TABLAT = data;   
  	asm("\tTBLWT*");  	

	FREE = 0;  		
	WPROG=1;
	Write_Cycle();
}

unsigned int Flash_Read(unsigned long address)
{
	unsigned int temp;
		TBLPTRL = ((address) & 0xFF);
		TBLPTRH = (((address) >> 8) & 0xFF);
		TBLPTRU = (((address) >> 8) >> 8);
	asm("\tTBLRD*+");
	temp=TABLAT;
	temp<<=8;
	asm("\tTBLRD*+");
	temp|=TABLAT;
		return temp;
}

