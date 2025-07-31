#include 	"sys_cpu.h"
#include	 "include.h"
#include	 "BIOS_JLX1864G_139.h"
#include	 "zk_all.h"

extern unsigned char disp_set_flg;
/***********************************************************************/
void BIOS_JLX12864_TRANS_CMD(int data)   /*used*/ 
{ 
	CS=0;  RS=0; 
	RD=1;  WR=0; 
	P1=data;  
	RD=0;  
	CS=1;  
}  


void BIOS_JLX12864_TRANS_DAT(int data) /*used*/
{ 
	CS=0;  RS=1; 
	RD=1;  WR=0;  
	P1=data; 
	CS=1;  RD=0; 
}  

void BIOS_JLX12864_DELAY(int i)/*used*/
{  
	int j,k; 
	for(j=0;j<i;j++) 
		for(k=0;k<10;k++);
}  

/******************LCD 初始化*********************************************/ 
void SYS_IniLcd(void)/*used*/
{     
	RES=0;              /*低电平复位*/    
	BIOS_JLX12864_DELAY(200);    
	RES=1;     /*复位完毕*/   
	BIOS_JLX12864_DELAY(200);          
	BIOS_JLX12864_TRANS_CMD(JLX12864G_RES);   /*软复位*/ 
	BIOS_JLX12864_DELAY(50);  
	
	
	BIOS_JLX12864_TRANS_CMD(0xa2);   /*1/9 偏压比（bias）*/ 
	
	BIOS_JLX12864_TRANS_CMD(0xa1);  /*列扫描顺序：从左到右*/ 
	
	BIOS_JLX12864_TRANS_CMD(0xc0);  /*行扫描顺序：从上到下*/ 
	
	BIOS_JLX12864_TRANS_CMD(0x2c);   /*升压步聚 1*/ 
	BIOS_JLX12864_DELAY(5);  
	BIOS_JLX12864_TRANS_CMD(0x2e);   /*升压步聚 2*/ 
	BIOS_JLX12864_DELAY(5); 
	BIOS_JLX12864_TRANS_CMD(0x2f);   /*升压步聚 3*/
	BIOS_JLX12864_DELAY(50);  
	
	
	BIOS_JLX12864_TRANS_CMD(0x25);   /*粗调对比度，可设置范围 0x20～0x27   23*/ 
	BIOS_JLX12864_TRANS_CMD(0x81);   /*微调对比度80*/ 
	//BIOS_JLX12864_TRANS_CMD(0x22); /*微调对比度的值，可设置范围 0x00～0x3f   1a*/ 

	BIOS_JLX12864_TRANS_CMD(0x0c); 
	
	
	BIOS_JLX12864_DELAY(10); 
	
	BIOS_JLX12864_TRANS_CMD(0xac);
	BIOS_JLX12864_TRANS_CMD(0x00);
	BIOS_JLX12864_TRANS_CMD(0x40);
	
	//BIOS_JLX12864_DELAY(200); 
	
	BIOS_JLX12864_TRANS_CMD(JLX12864G_ON);    /*开显示*/ 
	//BIOS_JLX12864_TRANS_CMD(0xa7);          /*反显开启*/ 
} 

void DISP_DigBasic(unsigned char dat,unsigned char startx,unsigned char starty)
{
	unsigned char i,j,k,l,m,n;
	unsigned short  x,y;
        m = dat*12;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+6);l++)   
	{
		x=arry_dig[l];
		x=x|(arry_dig[l+6]<<8);
		x=x<<3;
		
		x=x&0xff;
		BIOS_JLX12864_TRANS_DAT(x);
	}
	
	
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 		
        for(l=m+6;l<(m+12);l++)   
	{
		x=arry_dig[l];
		x=(x<<8)|arry_dig[l-6];
		x=x<<3;
		y=x>>8;
		//x=x>>8;
	    	
	       // y=y&0x00ff;
		
                BIOS_JLX12864_TRANS_DAT(y);
	}
}

unsigned short  val_val=0;// lwz 正式显示在界面上的数值

void DISP_DigBasic32(unsigned short dat,unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = dat*45;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+15);l++)   
		{BIOS_JLX12864_TRANS_DAT(arry_dig22[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+15;l<(m+30);l++)   
		{BIOS_JLX12864_TRANS_DAT(arry_dig22[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+2);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+30;l<(m+45);l++)   
		{BIOS_JLX12864_TRANS_DAT(arry_dig22[l]);}
}

void DISP_ChaBasic(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	
	unsigned char i,j,k,l,m,n;
	unsigned short x;
        m = dat;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+6);l++)   
	{
		x=arry_char[l];
		x=x|(arry_char[l+6]<<8);
		x=x>>3;
		
		x=x&0xff;
		BIOS_JLX12864_TRANS_DAT(x);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+6;l<(m+12);l++)   
	{
		x=arry_char[l];
		x=x<<8|(arry_char[l-6]);
		x=x>>3;
		
		x=x>>8;
		BIOS_JLX12864_TRANS_DAT(x);
	}
}



//void DISP_BatWarnC(void) /*used*/
//{ 
//	unsigned int i,j,l;   
//	l=0;       
//	for(i=0;i<8;i++)        
//	{          
//		CS=0; 
//		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
//		BIOS_JLX12864_TRANS_CMD(0x10);  
//		BIOS_JLX12864_TRANS_CMD(0x00); 
//		for(j=0;j<128;j++)   
//		{
//                  
//                   BIOS_JLX12864_TRANS_DAT(draw_batwarn[l]);
//                   l++;
//        	}         
//	} 
//}



//垂直翻转的效果
//void DISP_BatWarnC(void) /*used*/
//{ 
//	unsigned int i,j,l;   
//	unsigned char corrected_data;
//	l=0;       
//	for(i=0;i<8;i++)        
//	{          
//		CS=0; 
//		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
//		BIOS_JLX12864_TRANS_CMD(0x10);  
//		BIOS_JLX12864_TRANS_CMD(0x00); 
//		for(j=0;j<128;j++)   
//		{
//			// 先做垂直翻转（位序翻转），再做水平翻转（列序翻转）
//			unsigned char original_data = draw_batwarn[127-j + i*128];
//			
//			// 位序翻转：解决上下分割问题
//			corrected_data = 0;
//			if(original_data & 0x01) corrected_data |= 0x80;
//			if(original_data & 0x02) corrected_data |= 0x40;
//			if(original_data & 0x04) corrected_data |= 0x20;
//			if(original_data & 0x08) corrected_data |= 0x10;
//			if(original_data & 0x10) corrected_data |= 0x08;
//			if(original_data & 0x20) corrected_data |= 0x04;
//			if(original_data & 0x40) corrected_data |= 0x02;
//			if(original_data & 0x80) corrected_data |= 0x01;
//			
//			BIOS_JLX12864_TRANS_DAT(corrected_data);
//			l++;
//		}         
//	} 
//}

//分割了，上半部分在上，下半部分在下
//void DISP_BatWarnC(void) /*used*/
//{ 
//	unsigned int i,j,l;   
//	unsigned char corrected_data, final_data;
//	l=0;       
//	for(i=0;i<8;i++)        
//	{          
//		CS=0; 
//		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
//		BIOS_JLX12864_TRANS_CMD(0x10);  
//		BIOS_JLX12864_TRANS_CMD(0x00); 
//		for(j=0;j<128;j++)   
//		{
//			// 先做垂直翻转（位序翻转），再做水平翻转（列序翻转）
//			unsigned char original_data = draw_batwarn[127-j + i*128];
//			
//			// 第一次位序翻转：解决上下分割问题
//			corrected_data = 0;
//			if(original_data & 0x01) corrected_data |= 0x80;
//			if(original_data & 0x02) corrected_data |= 0x40;
//			if(original_data & 0x04) corrected_data |= 0x20;
//			if(original_data & 0x08) corrected_data |= 0x10;
//			if(original_data & 0x10) corrected_data |= 0x08;
//			if(original_data & 0x20) corrected_data |= 0x04;
//			if(original_data & 0x40) corrected_data |= 0x02;
//			if(original_data & 0x80) corrected_data |= 0x01;
//			
//			// 第二次位序翻转：解决垂直翻转问题
//			final_data = 0;
//			if(corrected_data & 0x01) final_data |= 0x80;
//			if(corrected_data & 0x02) final_data |= 0x40;
//			if(corrected_data & 0x04) final_data |= 0x20;
//			if(corrected_data & 0x08) final_data |= 0x10;
//			if(corrected_data & 0x10) final_data |= 0x08;
//			if(corrected_data & 0x20) final_data |= 0x04;
//			if(corrected_data & 0x40) final_data |= 0x02;
//			if(corrected_data & 0x80) final_data |= 0x01;
//			
//			BIOS_JLX12864_TRANS_DAT(final_data);
//			l++;
//		}         
//	} 
//}






//void DISP_BatWarnC(void) /*used*/
//{ 
//	unsigned int i,j,l;   
//	l=0;       
//	for(i=0;i<8;i++)        
//	{          
//		CS=0; 
//		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
//		BIOS_JLX12864_TRANS_CMD(0x10);  
//		BIOS_JLX12864_TRANS_CMD(0x00); 
//		for(j=0;j<128;j++)   
//		{
//			// 只做列序翻转，不做位序翻转
//			BIOS_JLX12864_TRANS_DAT(draw_batwarn[127-j + i*128]);
//			l++;
//		}         
//	} 
//}

//void DISP_BatWarnC(void) /*used*/
//{ 
//	unsigned int i,j,l;   
//	unsigned char corrected_data;
//	l=0;       
//	for(i=0;i<8;i++)        
//	{          
//		CS=0; 
//		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
//		BIOS_JLX12864_TRANS_CMD(0x10);  
//		BIOS_JLX12864_TRANS_CMD(0x00); 
//		for(j=0;j<128;j++)   
//		{
//			// 先做位序翻转解决分割问题，再做列序翻转解决水平翻转
//			unsigned char original_data = draw_batwarn[l];
//			
//			// 位序翻转：解决分割问题
//			corrected_data = 0;
//			if(original_data & 0x01) corrected_data |= 0x80;
//			if(original_data & 0x02) corrected_data |= 0x40;
//			if(original_data & 0x04) corrected_data |= 0x20;
//			if(original_data & 0x08) corrected_data |= 0x10;
//			if(original_data & 0x10) corrected_data |= 0x08;
//			if(original_data & 0x20) corrected_data |= 0x04;
//			if(original_data & 0x40) corrected_data |= 0x02;
//			if(original_data & 0x80) corrected_data |= 0x01;
//			
//			// 列序翻转：解决水平翻转
//			BIOS_JLX12864_TRANS_DAT(corrected_data);
//			l++;
//		}         
//	} 
//}

void DISP_BatWarnC(void) /*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
			// 调整行序解决分割问题，同时做列序翻转解决水平翻转
			BIOS_JLX12864_TRANS_DAT(draw_batwarn[127-j + (7-i)*128]);
			l++;
		}         
	} 
}



void DISP_BatWarnE(void) /*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                  
                   BIOS_JLX12864_TRANS_DAT(draw_batwarnE[l]);
                   l++;
        	}         
	} 
}





//===============clear all dot martrics============= 
void DISP_Clear(void) /*used*/
{ 
	unsigned char i,j;          
	for(i=0;i<9;i++)        
	{          
		CS=0;   
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<132;j++)   
			{BIOS_JLX12864_TRANS_DAT(0x00);}         
	} 
}  

void DISP_Clear22(void) /*used*/
{ 
	unsigned char i,j;          
	for(i=2;i<9;i++)        
	{          
		CS=0;   
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<132;j++)   
			{BIOS_JLX12864_TRANS_DAT(0x00);}         
	} 
} 

void DISP_ClrZero(unsigned char x,unsigned char x1,unsigned char y,unsigned char y1)/*used*/
{
	unsigned char  i,l,m;	
        for(i=x;i<x1;i++)
        {
		BIOS_JLX12864_TRANS_CMD(0xb0+7-x);  
		m = (128-y1)>>4;
		BIOS_JLX12864_TRANS_CMD(0x10+m);  
		m = (128-y1)&0x0f;
		BIOS_JLX12864_TRANS_CMD(0x00+m); 
                x++;
	        for(l=y;l<y1;l++)   
			{BIOS_JLX12864_TRANS_DAT(0);}
	}
}

void DISP_Bat00Clr(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+22);l++)   
	{
		BIOS_JLX12864_TRANS_DAT(0);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+22;l<(m+44);l++)   
        {
 	
		BIOS_JLX12864_TRANS_DAT(0);
	}
}

void QQQQQQQQQQQQQQDISP_Bat01(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+22);l++)   
	{
	        temp=draw_bat01[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+22;l<(m+44);l++)   
        {
		temp=draw_bat01[l];
	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}

void DISP_Bat000(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+24);l++)   
	{
	        temp=bat000[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+24;l<(m+48);l++)   
        {
		temp=bat000[l];
	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}
void DISP_Bat001(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+24);l++)   
	{
	        temp=bat001[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+24;l<(m+48);l++)   
        {
		temp=bat001[l];
	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}
void DISP_Bat002(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+24);l++)   
	{
	        temp=bat002[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+24;l<(m+48);l++)   
        {
		temp=bat002[l];
	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}
void DISP_Bat003(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+24);l++)   
	{
	        temp=bat003[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+24;l<(m+48);l++)   
        {
		temp=bat003[l];
	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}
void DISP_Bat004(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+24);l++)   
	{
	        temp=bat004[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+24;l<(m+48);l++)   
        {
		temp=bat004[l];
	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}


void DISP_Buz(unsigned char startx,unsigned char starty)/*used*/
{
    unsigned char i,j,k,n,temp;
	unsigned short m,l;
    m = 0;        
    i = (6-startx);
    n = (116-starty);
    j = n>>4;
    k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
    for(l=m;l<(m+16);l++)   
	{
	    temp=draw_buz[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
    for(l=m+16;l<(m+32);l++)   
    {
		temp=draw_buz[l];	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}

void DISP_BuzClr(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
    for(l=m;l<(m+16);l++)   
	{

		BIOS_JLX12864_TRANS_DAT(0);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
    for(l=m+16;l<(m+32);l++)   
    {
      	
		BIOS_JLX12864_TRANS_DAT(0);
	}
	
       
}

void DISP_Lock(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+9);l++)   
	{
	        temp=lock2015[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+9;l<(m+18);l++)   
        {
		temp=lock2015[l];	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}
void DISP_LockClr(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+9);l++)   
	{
		BIOS_JLX12864_TRANS_DAT(0);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
    for(l=m+9;l<(m+18);l++)   
    {    	
		BIOS_JLX12864_TRANS_DAT(0);
	}
}

void DISP_lx(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
		unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+30);l++)   
	{
	        temp=draw_lx[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
		BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+30;l<(m+60);l++)   
        {
		temp=draw_lx[l];	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
}




void DISP_jx(unsigned char startx,unsigned char starty)/*used*/
{
        unsigned char i,j,k,n,temp;
	unsigned short m,l;
        m = 0;        
        i = (6-startx);
        n = (116-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+31);l++)   
	{
	        temp=draw_jx[l];
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+31;l<(m+62);l++)   
        {
		temp=draw_jx[l];	        	
		BIOS_JLX12864_TRANS_DAT(temp);
	}
	
       
}
/*负号*/
void DISP_Fu(unsigned char startx,unsigned char starty)/*used*/
{
	
	unsigned char i,j,k,l,n;
      //  m = dat;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=0;l<10;l++)   
		{BIOS_JLX12864_TRANS_DAT(0x03);}	
}
/*负号另外一种*/
void DISP_Fu1(unsigned char startx,unsigned char starty)/*used*/
{
	
	unsigned char i,j,k,l,n;
      //  m = dat;        
        i = (7-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=0;l<6;l++)   
		{BIOS_JLX12864_TRANS_DAT(0x01);}	
}



unsigned char flgdis=0;


void DISP_Dig15_32(unsigned char startx, unsigned char starty)/*used*/
{
        unsigned char i,j,dig2,dig1,dig0;
        dig2=val_val/100;
        dig1=(val_val%100)/10;
        dig0=(val_val%100)%10;
        i = startx;
        j = starty;
        if(dig2!=0)
        {
        	flgdis=0;
        	DISP_DigBasic32(dig2,i,j);
		DISP_DigBasic32(dig1,i,j+15);
		DISP_DigBasic32(dig0,i,j+30);
	}
	else if(dig1!=0)
        {
                if(flgdis!=1)
                {
        	 	flgdis=1;
        	 	DISP_Clr2();
        		DISP_Clr3();
				DISP_Clr4();
	        }
                DISP_DigBasic32(dig1,i,j+8);
                DISP_DigBasic32(dig0,i,j+23);
	}
	else                            																																	
        {                               																																	
        	if(flgdis!=2)                    																																		
        	{                                																																		
        	 	flgdis=2;                																																		
        	 	DISP_Clr2();             																																		
        		DISP_Clr3();             																																		
				DISP_Clr4();     																																		
		}                        																																		
		DISP_DigBasic32(dig0,i,j+18);																																		
        }                               																																	
}



void DISP_DigBasic40(unsigned short dat,unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = dat*72;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+18);l++)   
		{BIOS_JLX12864_TRANS_DAT(arry_dig40[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+18;l<(m+36);l++)   
		{BIOS_JLX12864_TRANS_DAT(arry_dig40[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+2);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+36;l<(m+54);l++)   
		{BIOS_JLX12864_TRANS_DAT(arry_dig40[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+3);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+54;l<(m+72);l++)   
		{BIOS_JLX12864_TRANS_DAT(arry_dig40[l]);}
}


void DISP_Dig18_40(unsigned char startx, unsigned char starty)/*used*/
{
        unsigned char i,j,dig2,dig1,dig0;
        dig2=val_val/100;
        dig1=(val_val%100)/10;
        dig0=(val_val%100)%10;
        i = startx;
        j = starty;
        if(dig2!=0)
        {
        	flgdis=0;
        	DISP_DigBasic40(dig2,i,j);
		    DISP_DigBasic40(dig1,i,j+18);
		    DISP_DigBasic40(dig0,i,j+36);
	    }
	    else if(dig1!=0)
            {
            if(flgdis!=1)
            {
        	 	flgdis=1;
        	 	DISP_Clr2();
        		DISP_Clr3();
				DISP_Clr4();
			}
		        DISP_DigBasic40(dig1,i,j+9);
		        DISP_DigBasic40(dig0,i,j+27);
	    }
	    else
            {
        	if(flgdis!=2)
        	{
        	 	flgdis=2;
        	 	DISP_Clr2();
        		DISP_Clr3();
			DISP_Clr4();
		}
		DISP_DigBasic40(dig0,i,j+27);
	    }
}
/*压力设置*/
void DISP_pset(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+48);l++)   
		{BIOS_JLX12864_TRANS_DAT(pset[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+48;l<(m+96);l++)   
		{BIOS_JLX12864_TRANS_DAT(pset[l]);}
	
}
/*低压设置设置*/
void DISP_pset_low(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+48);l++)   
		{BIOS_JLX12864_TRANS_DAT(pset_l[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+48;l<(m+96);l++)   
		{BIOS_JLX12864_TRANS_DAT(pset_l[l]);}
	
}


void DISP_DigBasic0(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,l,m,n;
        m = (9-dat)*12;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+6);l++)   
	{
	    	if(disp_set_flg)
        	{BIOS_JLX12864_TRANS_DAT(0);}
		else
		{BIOS_JLX12864_TRANS_DAT(0);}	
		
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+6;l<(m+12);l++)   
	{
		if(disp_set_flg)
        	{BIOS_JLX12864_TRANS_DAT(0);}
		else
		{BIOS_JLX12864_TRANS_DAT(0);}	
	}
}



void DISP_Dig12_16(unsigned char startx, unsigned char starty,unsigned short disp_val)/*used*/
{
        unsigned char i,j,dig2,dig1,dig0;
        dig2=disp_val/100;
        dig1=(disp_val%100)/10;
        dig0=(disp_val%100)%10;
        i = startx;
        j = starty;
        if(dig2!=0)
        {
        	DISP_DigBasic(dig2,i,j);
		DISP_DigBasic(dig1,i,j+6);
		DISP_DigBasic(dig0,i,j+12);
	}
	else if(dig1!=0)
        {
        	DISP_DigBasic0(0,i,j);
		DISP_DigBasic(dig1,i,j+6);
		DISP_DigBasic(dig0,i,j+12);
	}
	else
        {
        	DISP_DigBasic0(0,i,j);
		DISP_DigBasic0(0,i,j+6);
		DISP_DigBasic(dig0,i,j+12);
	}
}

void DISP_DigBasics12(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,l,m,n;
        m = dat*16;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+8);l++)   
		{
		   	if(disp_set_flg)
	        	{BIOS_JLX12864_TRANS_DAT(~arry_digs12[l]);}
			else
			{BIOS_JLX12864_TRANS_DAT(arry_digs12[l]);}	
			
		}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+8;l<(m+16);l++)   
		{
			 if(disp_set_flg)
	        		{BIOS_JLX12864_TRANS_DAT(~arry_digs12[l]);}
			else
				{BIOS_JLX12864_TRANS_DAT(arry_digs12[l]);}	
		}
}



void DISP_DigBasic00(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,l,m,n;
        m = (9-dat)*12;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+8);l++)   
		{
		    if(disp_set_flg)
	        		{BIOS_JLX12864_TRANS_DAT(0);}
		    else
			{BIOS_JLX12864_TRANS_DAT(0);}	
			
		}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+8;l<(m+16);l++)   
		{
			 if(disp_set_flg)
	        	{BIOS_JLX12864_TRANS_DAT(0);}
				else
				{BIOS_JLX12864_TRANS_DAT(0);}	
		}
}

void DISP_Dig14_16(unsigned char startx, unsigned char starty,unsigned short disp_val)/*used*/
{
        unsigned char i,j,dig2,dig1,dig0;
        dig2=disp_val/100;
        dig1=(disp_val%100)/10;
        dig0=(disp_val%100)%10;
        i = startx;
        j = starty;
        if(dig2!=0)
        {
        DISP_DigBasics12(dig2,i,j);
		DISP_DigBasics12(dig1,i,j+8);
		DISP_DigBasics12(dig0,i,j+16);
	}
	else if(dig1!=0)
        {
        DISP_DigBasic00(0,i,j);
		DISP_DigBasics12(dig1,i,j+8);
		DISP_DigBasics12(dig0,i,j+16);
	}
	else
        {
        	DISP_DigBasic00(0,i,j);
		DISP_DigBasic00(0,i,j+8);
		DISP_DigBasics12(dig0,i,j+16);
	}
}

#if LANGUAGE_RUSSIA_ENGILISH
void DISP_Sys100(void)/*used*/
{
	unsigned int i,j,l;
	l=0;
	for (i=7;i>0;i--)
	{
		CS=0;

		BIOS_JLX12864_TRANS_CMD(0xb0+i);
		BIOS_JLX12864_TRANS_CMD(0x10);
		BIOS_JLX12864_TRANS_CMD(0x00);
		for (j=0;j<128;j++)
		{
            l=(7-i)*128;
			BIOS_JLX12864_TRANS_DAT(arry_desk[l+127-j]);    //arry_desk
			l++;
		}
	}
    CS=0;
	BIOS_JLX12864_TRANS_CMD(0xb0+0);
	BIOS_JLX12864_TRANS_CMD(0x10);
	BIOS_JLX12864_TRANS_CMD(0x00);
    for (j=0;j<128;j++)
	{
         
		BIOS_JLX12864_TRANS_DAT(0x00);    //arry_desk

	}
}
#else
void DISP_Sys100(void)/*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
  
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                   BIOS_JLX12864_TRANS_DAT(arry_desk[l]);    //arry_desk
                   l++;
                }         
	} 
} 

#endif



void DISP_Ver(void)/*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0;  
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                   BIOS_JLX12864_TRANS_DAT(arry_ver[l]);
                   l++;
                }         
	} 
} 


void DISP_ask(void)/*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                   BIOS_JLX12864_TRANS_DAT(ask[l]);
                   l++;
                }         
	} 
} 
extern const  char set_language[];
void DISP_LANGUAGE(void)/*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                   BIOS_JLX12864_TRANS_DAT(set_language[l]);
                   l++;
                }         
	} 
} 

extern const char sanjiao[];
void DISP_set_sanjiao(unsigned char x,unsigned y)/*used*/
{ 
	unsigned char i,j,k,l,n;
      //  m = dat;        
        i = (6-x);
        n = (122-y);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=0;l<7;l++)   
		{BIOS_JLX12864_TRANS_DAT(sanjiao[l]);}	
} 
void DISP_set_sanjiao0(unsigned char x,unsigned y)/*used*/
{ 
	unsigned char i,j,k,l,n;
      //  m = dat;        
        i = (6-x);
        n = (122-y);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=0;l<7;l++)   
		{BIOS_JLX12864_TRANS_DAT(0);}	
} 



void DISP_press_set2016(void)/*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                   BIOS_JLX12864_TRANS_DAT(press_set2016[l]);
                   l++;
                }         
	} 
} 


void DISP_mod_set2015(void)/*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                   BIOS_JLX12864_TRANS_DAT(mod_set2015[l]);
                   l++;
                }         
	} 
} 


void DISP_time_set2015(void)/*used*/
{ 
	unsigned int i,j,l;   
	l=0;       
	for(i=0;i<8;i++)        
	{          
		CS=0; 
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10);  
		BIOS_JLX12864_TRANS_CMD(0x00); 
		for(j=0;j<128;j++)   
		{
                   BIOS_JLX12864_TRANS_DAT(tim_set2015[l]);
                   l++;
                }         
	} 
} 

void DISP_run2015(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+46);l++)   
		{BIOS_JLX12864_TRANS_DAT(run2015[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+46;l<(m+92);l++) 
    	{BIOS_JLX12864_TRANS_DAT(run2015[l]);}
}



void DISP_press2015(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+46);l++)   
		{BIOS_JLX12864_TRANS_DAT(press2015[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+46;l<(m+92);l++) 
    	{BIOS_JLX12864_TRANS_DAT(press2015[l]);}
    	
    	
}


void DISP_stop2015(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+58);l++)   
		{BIOS_JLX12864_TRANS_DAT(stop2015[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+58;l<(m+116);l++) 
    	{BIOS_JLX12864_TRANS_DAT(stop2015[l]);}
    	
    	
}

/*打钩*/
void DISP_select(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+16);l++)   
		{BIOS_JLX12864_TRANS_DAT(select[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+16;l<(m+32);l++) 
    	{BIOS_JLX12864_TRANS_DAT(select[l]);}
    	
    	
}



void DISP_select0(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+16);l++)   
		{BIOS_JLX12864_TRANS_DAT(0);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+16;l<(m+32);l++) 
    	{BIOS_JLX12864_TRANS_DAT(0);}
    	
    	
}


/*管路堵塞*/
void DISP_block(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+54);l++)   
		{BIOS_JLX12864_TRANS_DAT(block[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+54;l<(m+108);l++) 
    	{BIOS_JLX12864_TRANS_DAT(block[l]);}
    	
    	
}


/*漏气*/
void DISP_louq(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+54);l++)   
		{BIOS_JLX12864_TRANS_DAT(louq[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+54;l<(m+108);l++) 
    	{BIOS_JLX12864_TRANS_DAT(louq[l]);}
    	
    	
}


void DISP_didl(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+54);l++)   
		{BIOS_JLX12864_TRANS_DAT(didl[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+54;l<(m+108);l++) 
    	{BIOS_JLX12864_TRANS_DAT(didl[l]);}
    	
    	
}


void DISP_yewm(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = 0;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+54);l++)   
		{BIOS_JLX12864_TRANS_DAT(yewm[l]);}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
         for(l=m+54;l<(m+108);l++) 
    	{BIOS_JLX12864_TRANS_DAT(yewm[l]);}
    	
    	
}



/*字符*/
#if LANGUAGE_RUSSIA_ENGILISH
void DISP_ChaBasic2(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,n;
	unsigned short l,m;
	const unsigned char *font_ptr;
	
	// 字符类型判断
	if (dat >= 0x20 && dat <= 0x7F) {
		// ASCII字符 (0x20-0x7F) - 保持原有逻辑
		m = dat;
		if (m>90)    //////small (97-122)
		{
			m=(m-97)*14;
			font_ptr = arry_char2;
		}
		else if (m>40) /////big (65-90)
		{
			m=(m-65)*14;
			font_ptr = arry_char22;
		}
		else
		{
			// 其他ASCII字符，显示空白
			i = (6-startx);
			n = (122-starty);
			j = n>>4;
			k = n&0x0f;
			BIOS_JLX12864_TRANS_CMD(0xb0+i);
			BIOS_JLX12864_TRANS_CMD(0x10+j);
			BIOS_JLX12864_TRANS_CMD(0x00+k);
			for (l=m;l<(m+2);l++)
			{
				BIOS_JLX12864_TRANS_DAT(0);
			}
			BIOS_JLX12864_TRANS_CMD(0xb0+i+1);
			BIOS_JLX12864_TRANS_CMD(0x10+j);
			BIOS_JLX12864_TRANS_CMD(0x00+k);
			for (l=m+2;l<(m+4);l++)
			{
				BIOS_JLX12864_TRANS_DAT(0);
			}
			return;
		}
	} else if (dat >= 0x80 && dat <= 0xFF) {
		// 俄罗斯字符 (0x80-0xFF) - 使用与ASCII相同的逻辑
		m = (dat - 0x80) * 14;  // 14字节/字符，与ASCII一致
		font_ptr = CYRILLIC;
	} else {
		// 其他字符，显示空白
		i = (6-startx);
		n = (122-starty);
		j = n>>4;
		k = n&0x0f;
		BIOS_JLX12864_TRANS_CMD(0xb0+i);
		BIOS_JLX12864_TRANS_CMD(0x10+j);
		BIOS_JLX12864_TRANS_CMD(0x00+k);
		for (l=m;l<(m+2);l++)
		{
			BIOS_JLX12864_TRANS_DAT(0);
		}
		BIOS_JLX12864_TRANS_CMD(0xb0+i+1);
		BIOS_JLX12864_TRANS_CMD(0x10+j);
		BIOS_JLX12864_TRANS_CMD(0x00+k);
		for (l=m+2;l<(m+4);l++)
		{
			BIOS_JLX12864_TRANS_DAT(0);
		}
		return;
	}
	
	// 显示字符 - 使用与原始函数完全相同的逻辑
	i = (6-startx);
	n = (122-starty);
	j = n>>4;
	k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);
	BIOS_JLX12864_TRANS_CMD(0x10+j);
	BIOS_JLX12864_TRANS_CMD(0x00+k);
	for (l=m;l<(m+7);l++)
	{
		BIOS_JLX12864_TRANS_DAT(font_ptr[l]);
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);
	BIOS_JLX12864_TRANS_CMD(0x10+j);
	BIOS_JLX12864_TRANS_CMD(0x00+k);
	for (l=m+7;l<(m+14);l++)
	{
		BIOS_JLX12864_TRANS_DAT(font_ptr[l]);
	}
}
#else
void DISP_ChaBasic2(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	
	unsigned char i,j,k,n;
	unsigned short l,m;
        m = dat;   
        if(m>90)     //////small
	{
		m=(m-97)*14;
	        i = (6-startx);
	        n = (122-starty);
	        j = n>>4;
	        k = n&0x0f;
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	        for(l=m;l<(m+7);l++)   
			{BIOS_JLX12864_TRANS_DAT(arry_char2[l]);}
		BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	        for(l=m+7;l<(m+14);l++)   
			{BIOS_JLX12864_TRANS_DAT(arry_char2[l]);}
	}
	else if(m>40)  /////big
	{
		m=(m-65)*14;
	        i = (6-startx);
	        n = (122-starty);
	        j = n>>4;
	        k = n&0x0f;
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	        for(l=m;l<(m+7);l++)   
			{BIOS_JLX12864_TRANS_DAT(arry_char22[l]);}
		BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	        for(l=m+7;l<(m+14);l++)   
			{BIOS_JLX12864_TRANS_DAT(arry_char22[l]);}
	}
	else
	{
		//m=(m-65)*14;
	        i = (6-startx);
	        n = (122-starty);
	        j = n>>4;
	        k = n&0x0f;
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	        for(l=m;l<(m+2);l++)   
			{BIOS_JLX12864_TRANS_DAT(0);}
		BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	        for(l=m+2;l<(m+4);l++)   
			{BIOS_JLX12864_TRANS_DAT(0);}

	}
	
}
#endif






void DISP_cha7s(char *s,unsigned char x,unsigned char y)/*used*/
{
	unsigned char  z;
	z = y;

	while(*s) 
	{
	         
		 DISP_ChaBasic2(*s,x,z);
		 
		 s = s+1;
		 if(*s==32)
		 {z=z+2;}
		 else
		 {z = z+7;}
       
	}


}




void DISP_block_new(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	
		unsigned char i,j,k,n;
		unsigned short l,m;
	        m = dat-32;   
		m=m*16;
	        i = (6-startx);
	        n = (135-starty);
	        j = n>>4;
	        k = n&0x0f;
		
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
		if(dat==32)
		{
			for(l=0;l<3;l++)   
				{BIOS_JLX12864_TRANS_DAT(0);}
		}
		else
		{
		 	for(l=m+15;l>(m+7);l--)   
				{BIOS_JLX12864_TRANS_DAT(ASCII[l]);}
	        }
		BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	       
	        if(dat==32)
		{
			for(l=0;l<3;l++)   
				{BIOS_JLX12864_TRANS_DAT(0);}
		}
		else
		{
			for(l=m+7;l>(m-1);l--)   
				{BIOS_JLX12864_TRANS_DAT(ASCII[l]);}
	        }
}



#if LANGUAGE_RUSSIA_ENGILISH
 void DISP_ChaBasic2015(unsigned char dat, unsigned char startx, unsigned char starty)
{
    unsigned char i, j, k, n;
    unsigned short l, m;
    const unsigned char *font_ptr;

    // 字符编码判断
    if (dat >= 0x20 && dat <= 0x7F) {
        // ASCII字符 (0x20-0x7F)
        m = (dat - 0x20) * 16;
        font_ptr = &ASCII[m];
    } else if (dat >= 0x80 && dat <= 0xFF) {
        // 俄罗斯字符 (0x80-0xFF) - ANSI编码
        m = (dat - 0x80) * 16;
        font_ptr = &CYRILLIC[m];
    } else {
        // 默认显示空格
        m = 0;
        font_ptr = &ASCII[0];
    }

    i = (6 - startx);
    n = (122 - starty);
    j = n >> 4;
    k = n & 0x0F;

    if(m==0x00){
        BIOS_JLX12864_TRANS_CMD(0xb0 + i);
        BIOS_JLX12864_TRANS_CMD(0x10 + j);
        BIOS_JLX12864_TRANS_CMD(0x00 + k);
        for (l = 0; l < 2; l++) {
            BIOS_JLX12864_TRANS_DAT(0);
        }     
        
        BIOS_JLX12864_TRANS_CMD(0xb0 + i + 1);
        BIOS_JLX12864_TRANS_CMD(0x10 + j);
        BIOS_JLX12864_TRANS_CMD(0x00 + k);
        for (l = 0; l < 2; l++) {
            BIOS_JLX12864_TRANS_DAT(0);
        } 
        
    }else{
         // 显示上半部分8像素 - 使用与DISP_Dig0相同的逻辑
        BIOS_JLX12864_TRANS_CMD(0xb0 + i);
        BIOS_JLX12864_TRANS_CMD(0x10 + j);
        BIOS_JLX12864_TRANS_CMD(0x00 + k);
        for (l = m+15; l > (m+7); l--) {
            BIOS_JLX12864_TRANS_DAT(font_ptr[l-m]);
        }

        // 显示下半部分8像素 - 使用与DISP_Dig0相同的逻辑
        BIOS_JLX12864_TRANS_CMD(0xb0 + i + 1);
        BIOS_JLX12864_TRANS_CMD(0x10 + j);
        BIOS_JLX12864_TRANS_CMD(0x00 + k);
        for (l = m+7; l > (m-1); l--) {
            BIOS_JLX12864_TRANS_DAT(font_ptr[l-m]);
    }
    }

}
#else
void DISP_ChaBasic2015(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	
		unsigned char i,j,k,n;
		unsigned short l,m;
	        m = dat-32;   
		m=m*16;
	        i = (6-startx);
	        n = (122-starty);
	        j = n>>4;
	        k = n&0x0f;
		
		BIOS_JLX12864_TRANS_CMD(0xb0+i);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
		if(dat==32)
		{
			for(l=0;l<3;l++)   
				{BIOS_JLX12864_TRANS_DAT(0);}
		}
		else
		{
		 	for(l=m+15;l>(m+7);l--)   
				{BIOS_JLX12864_TRANS_DAT(ASCII[l]);}
	        }
		BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
		BIOS_JLX12864_TRANS_CMD(0x10+j);  
		BIOS_JLX12864_TRANS_CMD(0x00+k); 
	       
	        if(dat==32)
		{
			for(l=0;l<3;l++)   
				{BIOS_JLX12864_TRANS_DAT(0);}
		}
		else
		{
			for(l=m+7;l>(m-1);l--)   
				{BIOS_JLX12864_TRANS_DAT(ASCII[l]);}
	        }
}
#endif

#if LANGUAGE_RUSSIA_ENGILISH
extern unsigned char   language;
void DISP_8X16ascii(char *s,unsigned char x,unsigned char y)/*used*/
{
	unsigned char  z;
	z = y;

	while (*s)
	{

		DISP_ChaBasic2015(*s,x,z);

		s = s+1;
		if (*s==32)
		{
            if(language){
                z=z+3; 
            }else{
                z=z+2;
            }
			
		}
        else if( *s==166 || *s==196)
        {
            if(language){
                z = z+8;
            }else{
                z = z+5;
            }
        }
        else if(*s==194){
            
            if(language){
                z = z+8;
            }else{
                z = z+8;
            }        
        }
		else
		{
            if(language){
                z = z+8;
            }else{
                z = z+6;
            }
			
		}

	}


}
#else
void DISP_8X16ascii(char *s,unsigned char x,unsigned char y)/*used*/
{
	unsigned char  z;
	z = y;

	while(*s) 
	{
	         
		 DISP_ChaBasic2015(*s,x,z);
		 
		 s = s+1;
		 if(*s==32)
		 {z=z+3;}
		 else
		 {z = z+8;}
       
	}
}
#endif




void DISP_8X16ascii_block(char *s,unsigned char x,unsigned char y)/*used*/
{
	unsigned char  z;
	z = y;

	while(*s) 
	{
	         
		 DISP_block_new(*s,x,z);
		 
		 s = s+1;
		 if(*s==32)
		 {z=z+3;}
		 else
		 {z = z+8;}
       
	}
}

void DISP_key2015(unsigned char startx,unsigned char starty)/*used*/
{
	unsigned char i,j,k,l,m=0,n;
      ///m = dat*16;  
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m;l<(m+16);l++)   
	{
		BIOS_JLX12864_TRANS_DAT(key2015[l]);
	}	
			
	
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+16;l<(m+32);l++)   
	{
		BIOS_JLX12864_TRANS_DAT(key2015[l]);
	}
}


void DISP_Dig0(unsigned char dat,unsigned char startx,unsigned char starty)/*used*/
{
	unsigned short i,j,k,l,n;
	unsigned short m;
        m = (dat+16)*16;        
        i = (6-startx);
        n = (122-starty);
        j = n>>4;
        k = n&0x0f;
	BIOS_JLX12864_TRANS_CMD(0xb0+i);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+15;l>(m+7);l--)   
	{
	   	if(disp_set_flg)
        		{BIOS_JLX12864_TRANS_DAT(~ASCII[l]);}
		else
			{BIOS_JLX12864_TRANS_DAT(ASCII[l]);}		
	}
	BIOS_JLX12864_TRANS_CMD(0xb0+i+1);  
	BIOS_JLX12864_TRANS_CMD(0x10+j);  
	BIOS_JLX12864_TRANS_CMD(0x00+k); 
        for(l=m+7;l>(m-1);l--)   
	{
		if(disp_set_flg)
        		{BIOS_JLX12864_TRANS_DAT(~ASCII[l]);}
		else
			{BIOS_JLX12864_TRANS_DAT(ASCII[l]);}	
	}
}



void DISP_Dig1(unsigned char startx, unsigned char starty,unsigned short disp_val)/*used*/
{
        unsigned char i,j,dig2,dig1,dig0;
        dig2=disp_val/100;
        dig1=(disp_val%100)/10;
        dig0=(disp_val%100)%10;
        i = startx;
        j = starty;
        if(dig2!=0)
        {
        	DISP_Dig0(dig2,i,j);
		DISP_Dig0(dig1,i,j+8);
		DISP_Dig0(dig0,i,j+16);
	}
	else if(dig1!=0)
        {
        	DISP_DigBasic00(0,i,j);
		DISP_Dig0(dig1,i,j+8);
		DISP_Dig0(dig0,i,j+16);
	}
	else
        {
        	DISP_DigBasic00(0,i,j);
		DISP_DigBasic00(0,i,j+8);
		DISP_Dig0(dig0,i,j+16);
	}
}