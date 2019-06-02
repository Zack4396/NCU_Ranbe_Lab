#include"1602.h"
/***************延时函数*************/

void delay1ms(uint c)   //误差 0us
{
    uchar a,b;
    for(a=c;a>0;a--)  
        for(b=110;b>0;b--); 	
}

/*************lcd初始化函数**********/
void lcd_init()
{
	lcd_write_command(0x38);
	lcd_write_command(0x0c);
	lcd_write_command(0x06);
	lcd_write_command(0x01);	
}

/*以下读写操作按照时序图来写*/

/************写指令***************/
void lcd_write_command(uchar dat)
{
	EN=0;
	RS=0;
	RW=0;
	
	lcd_db=dat;   
	delay1ms(1);	 
	EN=1;
	delay1ms(5);
	EN=0;	
}
/************写数据***************/
void lcd_write_data(uchar dat)
{
	EN=0;
	RS=1;
	RW=0;
	
	lcd_db=dat;   
	delay1ms(1);	 
	EN=1;
	delay1ms(5);
	EN=0;	
}

/************写字符串*************/

/***p：字符串显示的初始位置*******/
/***s：你所定义的字符串数组名称***/
/***low：字符串长度***************/
void lcd_print(uchar L,uchar p,uchar *s,uint low) 
{  
  uint num;  

	if(L==0)
	{
		lcd_write_command(0x80+p);
	}
	else
	{
		lcd_write_command(0xC0+p);	
	} 
	for(num=0;num<low;num++)     
	{     
	 lcd_write_data(s[num]);   
	 delay1ms(1);     
	}  
}

void lcd_sum(uchar L,uchar p,uchar *s,uint low)
{

	uint num;  
	if(L==0)
	{
		lcd_write_command(0x80+p);
	}
	else
	{
		lcd_write_command(0xC0+p);	
	}   
	for(num=0;num<low;num++)     
	{   
		if(s[num]==17) 
	 		lcd_write_data(' ');
		else 
			lcd_write_data(s[num]+'0');	  	    
		delay1ms(1);     
	}

}
/*********测忙/读指令函数 **********/
/*此函数可加，可不加：视读写操作的延时长短来定*/
/********
void lcd_check_busy()
{
	do
	{	EN=0;
		RS=0;
		RW=1;
		
		lcd_db=0xff;   
		delay1ms(1);	 
		EN=1;
		delay1ms(5);
		EN=0;	
	}while(lcd_db7==1);
}
*******/


