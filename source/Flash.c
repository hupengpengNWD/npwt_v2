
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

