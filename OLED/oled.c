 
//#include "delay.h"
#include "stdlib.h"

//basic <XXX.h>
#include "oled.h"
#include "oledfont.h"  	

//OLED Display buffer
u8 OLED_GRAM[144][8];	

//Color reversal
void OLED_ColorTurn(u8 i){
	
	if(i==0){
		OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
	}
	if(i==1){
		OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
	}
}

//Rotate 180 degrees
void OLED_DisplayTurn(u8 i){
	
	if(i==0){
		OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
		OLED_WR_Byte(0xA1,OLED_CMD);
	}
	if(i==1){
		OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
		OLED_WR_Byte(0xA0,OLED_CMD);
	}
}

//I2C start
void I2C_Start(void){
	
	OLED_SDIN_Set();
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SCLK_Clr();
}

//I2C stop
void I2C_Stop(void){
	
	OLED_SCLK_Set();
	OLED_SDIN_Clr();
	OLED_SDIN_Set();
}

//I2C waitAck
void I2C_WaitAck(void){
	
	OLED_SCLK_Set();
	OLED_SCLK_Clr();
}

//write 1 byte
void Send_Byte(u8 dat){
	
	u8 i;
	
	for(i=0;i<8;i++){
		
		OLED_SCLK_Clr();//将时钟信号设置为低电平
		if(dat&0x80){//将dat的8位从最高位依次写入
		
			OLED_SDIN_Set();
		}else{
			
			OLED_SDIN_Clr();
		}
		OLED_SCLK_Set();//将时钟信号设置为高电平
		OLED_SCLK_Clr();//将时钟信号设置为低电平
		dat<<=1;
	}
}

//send 1 byte
//write 1 byte to SSD1306
//mode: 0 - CMD  1 - DATA
void OLED_WR_Byte(u8 dat,u8 mode){
	
	I2C_Start();
	Send_Byte(0x78);
	I2C_WaitAck();
	if(mode){Send_Byte(0x40);}
		else{Send_Byte(0x00);}
	I2C_WaitAck();
	Send_Byte(dat);
	I2C_WaitAck();
	I2C_Stop();
}

//Turn on OLED 
void OLED_DisPlay_On(void){
	
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//Turn off OLED 
void OLED_DisPlay_Off(void){
	
	OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
	OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
	OLED_WR_Byte(0xAF,OLED_CMD);//关闭屏幕
}

//Refresh OLED
void OLED_Refresh(void){
	
	u8 i,n;
	
	for(i=0;i<8;i++){
		
		OLED_WR_Byte(0xb0+i,OLED_CMD); //set the row start address
		OLED_WR_Byte(0x00,OLED_CMD);   //set the low column start address
		OLED_WR_Byte(0x10,OLED_CMD);   //set the high column start address
		for(n=0;n<128;n++)
			OLED_WR_Byte(OLED_GRAM[n][i],OLED_DATA);
	}
}
//Clean OLED Display buffer 
void OLED_Clear(void){

	u8 i,n;
	
	for(i=0;i<8;i++)
		for(n=0;n<128;n++)
			 OLED_GRAM[n][i]=0;	//Clean the buffer
	OLED_Refresh();				//Refresh OLED
}

//Draw point
//x:0~127
//y:0~63
//t: 0 - inverse 1 - normal
void OLED_DrawPoint(u8 x,u8 y,u8 t){
	
	u8 i,m,n;
	
	i=y/8;
	m=y%8;
	n=1<<m;
	t?(OLED_GRAM[x][i]|=n):(OLED_GRAM[x][i]&=~n);
}

//Draw line
//x:0~128
//y:0~64
//t: 0 - inverse 1 - normal
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 t){

	u8 i,k,k1,k2;
	
	if((x2>128)||(y2>64)||(x1>x2)||(y1>y2))	return;
	
	if(x1==x2)			//vertical curve	
		for(i=0;i<(y2-y1);i++)
			OLED_DrawPoint(x1,y1+i,t);
		
	else if(y1==y2)		//transverse line
	
		for(i=0;i<(x2-x1);i++)
			OLED_DrawPoint(x1+i,y1,t);
		
	else{      			//bias line 

		k1=y2-y1;
		k2=x2-x1;
		k=k1*10/k2;
		for(i=0;i<(x2-x1);i++)
			OLED_DrawPoint(x1+i,y1+i*k/10,t);
	}
}

//x,y	: circle X,Y
//r		: circle R
//t: 0 - inverse 1 - normal
void OLED_DrawCircle(u8 x,u8 y,u8 r,u8 t){

	int a, b,num;
	
	a = 0;
    	b = r;
	while(2 * b * b >= r * r){
		OLED_DrawPoint(x + a, y - b,t);
        	OLED_DrawPoint(x - a, y - b,t);
        	OLED_DrawPoint(x - a, y + b,t);
        	OLED_DrawPoint(x + a, y + b,t);
 
        	OLED_DrawPoint(x + b, y + a,t);
       		OLED_DrawPoint(x + b, y - a,t);
        	OLED_DrawPoint(x - b, y - a,t);
        	OLED_DrawPoint(x - b, y + a,t);
        
        	a++;
        	num = (a * a + b * b) - r*r;	//Calculate the distance from the center of the circle
        	if(num > 0){
            		b--;
            		a--;
       		}
    	}
}

//Display char 
//x:0~127
//y:0~63
//size:font 12/16/24
//t: 0 - inverse 1 - normal
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1,u8 t){

	u8 i,m,temp,size2,chr1;
	u8 y0=y;
	
	size2=(size1/8+((size1%8)?1:0))*(size1/2);  //得到字体一个字符对应点阵集所占的字节数
	chr1=chr-' ';  //计算偏移后的值
	for(i=0;i<size2;i++){
		
		if(size1==12)
			temp=asc2_1206[chr1][i];	//12*6
		else if(size1==16)
			temp=asc2_1608[chr1][i];	//16*8
		else if(size1==24)
			temp=asc2_2412[chr1][i];	//24*12
		else 
			return ;
		
		for(m=0;m<8;m++){
			
			(temp & 0x80) ? OLED_DrawPoint(x, y, t) : OLED_DrawPoint(x, y, !t);
			temp<<=1;
			y++;
			if((y-y0)==size1){
			
				y=y0;
				x++;
				break;
			}
		}
	}
	//OLED_Refresh();
}


//Display String
//x,y:Start XY  
//size1:font size
//*chr: the string's start address 
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode){
					
	u16 x0=x;
	u16 y0=y;

	while(*str!=0){	//String end 

		if(x>(x0+width-size/2)){	//line feed
			
			y+=size;
			x=x0;	   
		}							    
		if(y>(y0+height-size))	break;     
		if(*str==13){
			
			y+=size;
			x=x0;
			str++; 
		}else 
			OLED_ShowChar(x,y,*str,size,mode);
		str++; 
		x+=size/2; 
	}
}		 		 

void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1,u8 t){
	
	while((*chr>=' ')&&(*chr<='~')){
	//judge it is or not Legal char

		OLED_ShowChar(x,y,*chr,size1,t);
		x+=size1/2;
		if(x>128-size1){
			x=0;
			y+=2;
		}	
		chr++;
	}
}

//m^n
u32 OLED_Pow(u8 m,u8 n){
	
	u32 result=1;
	
	while(n--)
	  result*=m;
  
	return result;
}

////显示2个数字
////x,y :起点坐标	 
////len :数字的位数
////size:字体大小
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 t){
	
	u8 k,temp;
	
	for(k=0;k<len;k++){
		temp=(num/OLED_Pow(10,len-k-1))%10;
		if(temp==0)
			OLED_ShowChar(x+(size1/2)*k,y,'0',size1,t);
		else 
			OLED_ShowChar(x+(size1/2)*k,y,temp+'0',size1,t);
	}
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 t){

	u8 i,m,n=0,temp,chr1;
	u8 x0=x,y0=y;
	u8 size3=size1/8;

	while(size3--){

		chr1=num*size1/8+n;
		n++;
			for(i=0;i<size1;i++)
			{
				if(size1==16)
						{temp=Hzk1[chr1][i];}//调用16*16字体
				else if(size1==24)
						{temp=Hzk2[chr1][i];}//调用24*24字体
				else if(size1==32)       
						{temp=Hzk3[chr1][i];}//调用32*32字体
				else if(size1==64)
						{temp=Hzk4[chr1][i];}//调用64*64字体
				else return;
							
						for(m=0;m<8;m++)
							{
//								if(temp&0x01)
//								{OLED_DrawPoint(x,y,t);}
								(temp & 0x01) ? OLED_DrawPoint(x, y, !t) : OLED_DrawPoint(x, y, t);
								temp>>=1;
								y++;
							}
							x++;
							if((x-x0)==size1)
							{x=x0;y0=y0+8;}
							y=y0;
			 }
	//			OLED_Refresh();
	}
}

//num 显示汉字的个数
//space 每一遍显示的间隔
void OLED_ScrollDisplay(u8 num,u8 space,u8 k){

	u8 i,n,t=0,m=0,r;

	while(1){

		if(m==0)
		{
	    OLED_ShowChinese(128,24,t,16,k); //写入一个汉字保存在OLED_GRAM[][]数组中
			t++;
		}
		if(t==num)
			{
				for(r=0;r<16*space;r++)      //显示间隔
				 {
					for(i=0;i<144;i++)
						{
							for(n=0;n<8;n++)
							{
								OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
							}
						}
           OLED_Refresh();
				 }
        t=0;
      }
		m++;
		if(m==16){m=0;}
		for(i=0;i<144;i++)   //实现左移
		{
			for(n=0;n<8;n++)
			{
				OLED_GRAM[i-1][n]=OLED_GRAM[i][n];
			}
		}
		OLED_Refresh();
	}
}

//配置写入数据的起始位置
void OLED_WR_BP(u8 x,u8 y)
{
	OLED_WR_Byte(0xb0+y,OLED_CMD);//设置行起始地址
	OLED_WR_Byte(((x&0xf0)>>4)|0x10,OLED_CMD);
	OLED_WR_Byte((x&0x0f)|0x01,OLED_CMD);
}

//x0,y0：起点坐标
//x1,y1：终点坐标
//BMP[]：要写入的图片数组
void OLED_ShowPicture(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	u32 j=0;
	u8 x=0,y=0;
	if(y%8==0)y=0;
	else y+=1;
	for(y=y0;y<y1;y++)
	 {
		 OLED_WR_BP(x0,y);
		 for(x=x0;x<x1;x++)
		 {
			 OLED_WR_Byte(BMP[j],OLED_DATA);
			 j++;
     }
	 }
}
void OLED_Fill(u8 x0, u8 y0, u8 x1, u8 y1, u8 t)
{
	u8 x, y;
	for (x = x0; x < x1; x++)
	{
		for (y = y0; y < y1; y++)
		{
			OLED_DrawPoint(x, y, t);
		}
	}
//	OLED_Refresh();
}
void OLED_DrawRectangle(u8 x0, u8 y0, u8 x1, u8 y1, u8 t)
{
	 u8 y_0, y_1, x_0, x_1, i;
	if (y0 > y1)	//??y0?y1???
	{
		y_0 = y1;
		y_1 = y0;
	}
	else
	{
		y_0 = y0;
		y_1 = y1;
	}
	
	if (x0 > x1)	//??x0?x1???
	{
		x_0 = x1;
		x_1 = x0;
	}
	else
	{
		x_0 = x0;
		x_1 = x1;
	}
	
	for (i = x_0 ; i <= x_1; i++)	//????
	{
		OLED_DrawPoint(i, y0, t);
		OLED_DrawPoint(i, y1, t);
	}
	
	for (i = y_0 ; i <= y_1; i++)	//????
	{
		OLED_DrawPoint(x0, i, t);
		OLED_DrawPoint(x1, i, t);
	}
	OLED_Refresh();//????
}

//OLED的初始化
void OLED_Init(void)
{
//	GPIO_InitTypeDef  GPIO_InitStructure;
// 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能A端口时钟
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7;	 
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
// 	GPIO_Init(GPIOA, &GPIO_InitStructure);	  //初始化GPIOD3,6
// 	GPIO_SetBits(GPIOA,GPIO_Pin_5|GPIO_Pin_7|GPIO_Pin_4);	
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能A端口时钟
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8;	 
// 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
// 	GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOD3,6
// 	GPIO_SetBits(GPIOB,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_8);	
	
//	OLED_RST_Set();
	HAL_Delay(100);
//	OLED_RST_Clr();//复位
	HAL_Delay(200);
//	OLED_RST_Set();
//	OLED_DC_Clr();
	
	OLED_WR_Byte(0xAE,OLED_CMD);//--turn off oled panel
	OLED_WR_Byte(0x00,OLED_CMD);//---set low column address
	OLED_WR_Byte(0x10,OLED_CMD);//---set high column address
	OLED_WR_Byte(0x40,OLED_CMD);//--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81,OLED_CMD);//--set contrast control register
	OLED_WR_Byte(0xCF,OLED_CMD);// Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1,OLED_CMD);//--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
	OLED_WR_Byte(0xC8,OLED_CMD);//Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
	OLED_WR_Byte(0xA6,OLED_CMD);//--set normal display
	OLED_WR_Byte(0xA8,OLED_CMD);//--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3f,OLED_CMD);//--1/64 duty
	OLED_WR_Byte(0xD3,OLED_CMD);//-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00,OLED_CMD);//-not offset
	OLED_WR_Byte(0xd5,OLED_CMD);//--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80,OLED_CMD);//--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9,OLED_CMD);//--set pre-charge period
	OLED_WR_Byte(0xF1,OLED_CMD);//Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA,OLED_CMD);//--set com pins hardware configuration
	OLED_WR_Byte(0x12,OLED_CMD);
	OLED_WR_Byte(0xDB,OLED_CMD);//--set vcomh
	OLED_WR_Byte(0x40,OLED_CMD);//Set VCOM Deselect Level
	OLED_WR_Byte(0x20,OLED_CMD);//-Set Page Addressing Mode (0x00/0x01/0x02)
	OLED_WR_Byte(0x02,OLED_CMD);//
	OLED_WR_Byte(0x8D,OLED_CMD);//--set Charge Pump enable/disable
	OLED_WR_Byte(0x14,OLED_CMD);//--set(0x10) disable
	OLED_WR_Byte(0xA4,OLED_CMD);// Disable Entire Display On (0xa4/0xa5)
	OLED_WR_Byte(0xA6,OLED_CMD);// Disable Inverse Display On (0xa6/a7) 
	OLED_WR_Byte(0xAF,OLED_CMD);
	OLED_Clear();
}
