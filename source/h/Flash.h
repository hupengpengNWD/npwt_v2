#ifndef __flash_h__
#define __flash_h__

extern void Write_Cycle(void);

extern void Flash_Erase(unsigned long address);

extern void Write_One_Word(unsigned long address,unsigned int data);

extern unsigned int Flash_Read(unsigned long address);

#endif