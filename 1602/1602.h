#ifndef _1602_H_
#define _1602_H_

#include<reg51.h>

//--重定义关键词--//

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

//--pin口定义--//
#define lcd_db  P0
//sbit lcd_db7=P0^7; /*这里我就没有加入测忙函数*/

sbit RS=P2^0;
sbit RW=P2^1;
sbit EN=P2^2; 

//---函数声明---//
void lcd_init();
void lcd_write_command(uchar dat);
void lcd_write_data(uchar dat);
void delay1ms(uint c);

void lcd_print(uchar L,uchar p,uchar *s,uint low);
void lcd_sum(uchar L,uchar p,uchar *s,uint low);
#endif
