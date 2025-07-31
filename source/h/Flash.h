#ifndef __flash_h__
#define __flash_h__

extern void Write_Cycle(void)/*used*/;

//擦除函数,每次擦除1024个字节
extern void Flash_Erase(unsigned long address)/*used*/;

//写函数,每次写入FLASH 1个字
extern void Write_One_Word(unsigned long address,unsigned int data)/*used*/;

//读函数，每次读一个字
extern unsigned int Flash_Read(unsigned long address)/*used*/;

#endif