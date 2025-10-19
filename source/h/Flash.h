#ifndef __flash_h__
#define __flash_h__

extern void Write_Cycle(void)/*used*/;

//²Á³ýº¯Êý,Ã¿´Î²Á³ý1024¸ö×Ö½Ú
extern void Flash_Erase(unsigned long address)/*used*/;

//Ð´º¯Êý,Ã¿´ÎÐ´ÈëFLASH 1¸ö×Ö
extern void Write_One_Word(unsigned long address,unsigned int data)/*used*/;

//¶Áº¯Êý£¬Ã¿´Î¶ÁÒ»¸ö×Ö
extern unsigned int Flash_Read(unsigned long address)/*used*/;

#endif