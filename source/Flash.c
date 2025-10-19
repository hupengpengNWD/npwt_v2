/**************************************************************************************************
**Project Name  : Flash_READ_WRITE
**Created by    : Abbott Lin
**Created Date  : 2014.11.21
**Version:      : V1.0
**Target        : PIC18F46J11
**OSC           : 8MHZ
**HardWare      : 
**Function      : Flash¶ÁÐ´²Á³ýº¯Êý
**Notice	: ¿ÉÊ¹ÓÃMPLAB SIM½øÐÐ·ÂÕæ,²é¿´ÔËÐÐÐ§¹û
**************************************************************************************************/
#include   "include.h"


//Ð´ÖÜÆÚ
void Write_Cycle(void)/*used*/
{

	WREN = 1;		

	CARRY = 0;
	if(GIE) 
		CARRY = 1;
	GIE = 0;  		

	EECON2 = 0X55;
	EECON2 = 0XAA;

	WR = 1; 		//1 Æô¶¯¶Á/Ð´ÖÜÆÚ;0 Ð´ÖÜÆÚÍê³É
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();
	while(WR) ;		//µÈ´ýÐ´ÖÜÆÚÍê³É
	WREN = 0;		

	if(CARRY) 
		GIE = 0;
}



//²Á³ýº¯Êý,Ã¿´Î²Á³ý1024¸ö×Ö½Ú
void Flash_Erase(unsigned long address)/*used*/
{
	TBLPTRL = ((address) & 0xFF);
	TBLPTRH = (((address) >> 8) & 0xFF);
	TBLPTRU = (((address) >> 8) >> 8);

	FREE = 1;  		
	Write_Cycle();
}

//Ð´º¯Êý,Ã¿´ÎÐ´ÈëFLASH 1¸ö×Ö
void Write_One_Word(unsigned long address,unsigned int data)/*used*/
{

	TBLPTRL = ((address) & 0xFF);
		TBLPTRH = (((address) >> 8) & 0xFF);
		TBLPTRU = (((address) >> 8) >> 8);

	TABLAT = data>>8;//¸ßÎ»
  	asm("\tTBLWT*+");
	TABLAT = data;   //µÍÎ»
  	asm("\tTBLWT*");  	

	FREE = 0;  		
	WPROG=1;
	Write_Cycle();
}

//¶Áº¯Êý£¬Ã¿´Î¶ÁÒ»¸ö×Ö
unsigned int Flash_Read(unsigned long address)/*used*/
{
	unsigned int temp;
		TBLPTRL = ((address) & 0xFF);
		TBLPTRH = (((address) >> 8) & 0xFF);
		TBLPTRU = (((address) >> 8) >> 8);
	asm("\tTBLRD*+");
	temp=TABLAT;//¸ßÎ»
	temp<<=8;
	asm("\tTBLRD*+");
	temp|=TABLAT;//µÍÎ»		
		return temp;
}


