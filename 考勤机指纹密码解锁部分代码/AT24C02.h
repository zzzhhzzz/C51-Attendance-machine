/*************************************************************
                      AT24C02头文件

实现功能：AT24C02的控制
***************************************************************/
#ifndef _AT24C02_H_ 
#define _AT24C02_H_
#include<reg52.h>
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int

/*********************AT24C02引脚定义*********************/ 
sbit sda=P0^1;
sbit scl=P0^2;

/*********************AT24C02函数定义*********************/
void  AT24C02_start();							   //开始信号
void  AT24C02_stop();							   //停止信号
void  AT24C02_respons();						   //应答信号
void  AT24C02_write(uchar date);				   //写数据
uchar AT24C02_read();							   //读数据
void  AT24C02_write_date(uchar address,uchar date);//向address地址写输dete数据
uchar AT24C02_read_date(uchar address);			   //向address读出数据并返回
extern void LCD12864_delay(uint x);
/*********************************************************
函数void AT24C02_start() 
AT24C02开始信号
*********************************************************/
void AT24C02_start()
{
	sda=1;
	_nop_();
	_nop_();
	scl=1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	sda=0;
	_nop_();
	_nop_();
}

/*********************************************************
函数void AT24C02_stop() 
函数作用:AT24C02停止信号
*********************************************************/
void AT24C02_stop()
{
	sda=0;
	_nop_();
	_nop_();
	scl=1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	sda=1;
	_nop_();
	_nop_();
}

/*********************************************************
函void AT24C02_respons() 
函数AT24C02应答信号
*********************************************************/
void AT24C02_respons()
{
	uchar i;
	scl=1;
	_nop_();
	_nop_();
	while((sda=1)&&(i<250))
		i++;
	scl=0;
	_nop_();
	_nop_();
}

/*********************************************************
函数void AT24C02_write(uchar date)
AT24C02写一个字节数据
*********************************************************/
void AT24C02_write(uchar date) 
{
	 uchar i;
	 scl=0;
	 for(i=0;i<8;i++)
	 {
		 sda = (bit)(date&0x80);
		 _nop_();		 
		 scl=1;
		 _nop_();
		 _nop_();
		 scl=0;
		 date<<=1;
	 }
}

/*********************************************************
函数uchar AT24C02_read()
AT24C02读一个字节数据
*********************************************************/
uchar AT24C02_read()
{
	uchar i,k;
	for(i=0;i<8;i++)
	{
		scl=1;		
		k=(k<<1)|sda;
		scl=0;
	}
	return k;
}

/*********************************************************
函数void AT24C02_write_date(uchar address,uchar date）
AT24C02写数据
将数据date写入到address地址里
*********************************************************/
void AT24C02_write_date(uchar address,uchar date)
{
	AT24C02_start();		//开始信号
	AT24C02_write(0xa0); 	//写时候。写入AT24C02器件地址，由于器件引脚A2，A1，A0都是接地，所以器件地址为0xa0
	AT24C02_respons();		//应答信号
	AT24C02_write(address); //写入将要存放数据的地址
	AT24C02_respons();		//应答信号
	AT24C02_write(date);    //写入数据
	AT24C02_respons();		//应答信号
	AT24C02_stop();			//停止信号
	LCD12864_delay(2);
}

/*********************************************************
函数uchar AT24C02_read_date(uchar address)
AT24C02读数据
向address地址里读出一个数据，并返回
*********************************************************/
uchar AT24C02_read_date(uchar address)
{
	uchar date; 
	AT24C02_start();			   //开始信号
	AT24C02_write(0xa0); 		   //写时候。写入AT24C02器件地址，由于器件引脚A2，A1，A0都是接地，所以器件地址为0xa0
	AT24C02_respons();			   //应答信号
	AT24C02_write(address);//写入将要读取数据的地址
	AT24C02_respons();			   //应答信号
	AT24C02_start();			   //开始信号
	AT24C02_write(0xa1);		   //读时候。写入AT24C02器件地址，由于器件引脚A2，A1，A0都是接地，所以器件地址为0xa1
	AT24C02_respons();			   //应答信号
	date=AT24C02_read(); 		   //读取数据
	AT24C02_stop();
	LCD12864_delay(2);
	return date;				   //返回数据
}
#endif