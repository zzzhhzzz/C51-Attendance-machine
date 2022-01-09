/****************************************************************
                      LCD12864头文件

实现功能：LCD12864的控制显示
*****************************************************************/
#ifndef _LCD12864_H_
#define _LCD12864_H_
#include <reg52.h> 
#include<intrins.h>  

#define uchar unsigned char
#define uint  unsigned int
/************************LCD12864引脚定义************************/
#define LCD P2	  //并行数据口D0~D7
sbit RS  =P3^5;	  //数据/命令选择 引脚
sbit RW  =P3^4;	  //读/写选择 引脚
sbit E   =P3^3;	  //使能信号 引脚

/************************LCD12864函数定义************************/
void LCD12864_delay(uint x);							//LCD12864延时
void write_com(uchar com);								//LCD12864写命令
void write_data(uchar dat);							    //LCD12864写数据
void LCD12864_display_char(uchar x,uchar y,uchar dat);  //在第y行，x+1列显示字符
void LCD12864_display_string(uchar x,uchar y,uchar *s);	//在第y行，x+1列开始显示字符串
void LCD12864_init(void);								//LCD12864初始化函数
/*******************LCD12864固定显示变量定义*********************/
//uchar code ASCII[] =  {'0','1','2','3','4','5','6','7','8','9'}; //显示的字符数组

/********************************************************
函void LCD12864_delay(uint x)
LCD12864延时函数
********************************************************/
void LCD12864_delay(uint x)
{
	uint j,i;
	for(j=0;j<x;j++)
	{
		for(i=0;i<120;i++);
	}
}
/********************************************************
函数uchar Lcd_CheckBusy(void)
LCD12864读忙函数
********************************************************/
uchar Lcd_CheckBusy(void)
{
    unsigned char Busy;
	LCD=0xff;
    RS=0;
    RW=1;
    E=1;
    _nop_(); 
    Busy=LCD&0x80;
    E=0;
    return Busy;
}
/********************************************************
函数void write_com(uchar com)
LCD12864写命令
参考手册com为LCD12864指令
********************************************************/
void write_com(uchar com)
{
	while(Lcd_CheckBusy());
   	RS=0;
   	RW=0;
   	E=0;
   	_nop_();  
	_nop_();
   	LCD=com;
   	_nop_(); 
	_nop_();
   	E=1;
   	_nop_();  
	_nop_();
   	E=0;
}

/********************************************************
void write_data(uchar dat)
LCD12864写数据
dat为所写内容
********************************************************/
void write_data(uchar dat)
{
	while(Lcd_CheckBusy());
	RS=1;
	RW=0;
	E=0;
	_nop_();  
	_nop_(); 
	LCD=dat;
	E=1;
	_nop_();
	_nop_();
	E=0;
}
/********************************************************
函数void LCD12864_display_pox(uchar x,uchar y)
LCD12864显示字符
********************************************************/
void LCD12864_display_pox(uchar x,uchar y)
{
	uchar add;		//存储显示位置
	if(y==1)				  //在第1行显示
		add=0x80+x;
	else
		if(y==2)			  //在第2行显示
			add=0x90+x;
		else
			if(y==3)		  //在第3行显示
				add=0x88+x;
			else
				if(y==4)	  //在第4行显示
					add=0x98+x;
	write_com(add);	 //先写显示地址
}
/********************************************************
函数void LCD12864_display_char(uchar x,uchar y,uchar dat)
LCD12864显示字符
在横坐标x（0~7），纵坐标y（1~4）显示字符dat
********************************************************/
void LCD12864_display_char(uchar x,uchar y,uchar dat)
{
	LCD12864_display_pox(x,y);	 //先写显示地址
	write_data(dat); //在写显示内容
}

/********************************************************
函数void LCD12864_display_string(uchar x,uchar y,uchar *s)
LCD12864显示字符
在横坐标x（0~7），纵坐标y（1~4）显示字符串*s
********************************************************/
void LCD12864_display_string(uchar x,uchar y,uchar *s)
{ 
	LCD12864_display_pox(x,y);	 //先写显示地址
	while(*s!='\0')
	{ 
		write_data(*s);
		s++;
		LCD12864_delay(1);
	}                                           
} 

/********************************************************
函数void LCD12864_init(void)
LCD12864初始化
********************************************************/
void LCD12864_init(void)
{
	write_com(0x30);       //选择基本指令集
	write_com(0x30);       //选择8bit数据流
	write_com(0x0c);       //开显示(无游标、不反白)
	write_com(0x01);       //清除显示，并且设定地址指针为00H
	write_com(0x06);       //指定在资料的读取及写入时，设定游标的移动方向及指定显示的移位，光标从右向左加1位移动 
}

#endif