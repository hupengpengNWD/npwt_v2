/**************************************************************************************************
**Project Name  : Flash_READ_WRITE
**Created by    : Abbott Lin
**Created Date  : 2014.11.21
**Version:      : V1.0
**Target        : PIC18F46J11
**OSC           : 8MHZ
**HardWare      : 
**Function      : Flash读写擦除函数
**Notice	: 可使用MPLAB SIM进行仿真,查看运行效果
**************************************************************************************************/
#include   "include.h"


//写周期
void Write_Cycle(void)/*used*/
{

	WREN = 1;		

	CARRY = 0;
	if(GIE) 
		CARRY = 1;
	GIE = 0;  		

	EECON2 = 0X55;
	EECON2 = 0XAA;

	WR = 1; 		//1 启动读/写周期;0 写周期完成
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	while(WR) ;		//等待写周期完成
	WREN = 0;		

	if(CARRY) 
		GIE = 0;
}



//擦除函数,每次擦除1024个字节
void Flash_Erase(unsigned long address)/*used*/
{
    TBLPTRL = ((address) & 0xFF);
    TBLPTRH = (((address) >> 8) & 0xFF);
    TBLPTRU = (((address) >> 8) >> 8);

	FREE = 1;  		
	Write_Cycle();
}

//写函数,每次写入FLASH 1个字
void Write_One_Word(unsigned long address,unsigned int data)/*used*/
{

	TBLPTRL = ((address) & 0xFF);
        TBLPTRH = (((address) >> 8) & 0xFF);
        TBLPTRU = (((address) >> 8) >> 8);

	TABLAT = data>>8;//高位
  	asm("\tTBLWT*+");
	TABLAT = data;   //低位
  	asm("\tTBLWT*");  	

	FREE = 0;  		
	WPROG=1;
	Write_Cycle();
}

//读函数，每次读一个字
unsigned int Flash_Read(unsigned long address)/*used*/
{
	unsigned int temp;
    	TBLPTRL = ((address) & 0xFF);
    	TBLPTRH = (((address) >> 8) & 0xFF);
    	TBLPTRU = (((address) >> 8) >> 8);
	asm("\tTBLRD*+");
	temp=TABLAT;//高位
	temp<<=8;
	asm("\tTBLRD*+");
	temp|=TABLAT;//低位		
        return temp;
}


