/****************************************************************
                      LCD12864ͷ�ļ�

ʵ�ֹ��ܣ�LCD12864�Ŀ�����ʾ
*****************************************************************/
#ifndef _LCD12864_H_
#define _LCD12864_H_
#include <reg52.h> 
#include<intrins.h>  

#define uchar unsigned char
#define uint  unsigned int
/************************LCD12864���Ŷ���************************/
#define LCD P2	  //�������ݿ�D0~D7
sbit RS  =P3^5;	  //����/����ѡ�� ����
sbit RW  =P3^4;	  //��/дѡ�� ����
sbit E   =P3^3;	  //ʹ���ź� ����

/************************LCD12864��������************************/
void LCD12864_delay(uint x);							//LCD12864��ʱ
void write_com(uchar com);								//LCD12864д����
void write_data(uchar dat);							    //LCD12864д����
void LCD12864_display_char(uchar x,uchar y,uchar dat);  //�ڵ�y�У�x+1����ʾ�ַ�
void LCD12864_display_string(uchar x,uchar y,uchar *s);	//�ڵ�y�У�x+1�п�ʼ��ʾ�ַ���
void LCD12864_init(void);								//LCD12864��ʼ������
/*******************LCD12864�̶���ʾ��������*********************/
//uchar code ASCII[] =  {'0','1','2','3','4','5','6','7','8','9'}; //��ʾ���ַ�����

/********************************************************
��void LCD12864_delay(uint x)
LCD12864��ʱ����
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
����uchar Lcd_CheckBusy(void)
LCD12864��æ����
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
����void write_com(uchar com)
LCD12864д����
�ο��ֲ�comΪLCD12864ָ��
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
LCD12864д����
datΪ��д����
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
����void LCD12864_display_pox(uchar x,uchar y)
LCD12864��ʾ�ַ�
********************************************************/
void LCD12864_display_pox(uchar x,uchar y)
{
	uchar add;		//�洢��ʾλ��
	if(y==1)				  //�ڵ�1����ʾ
		add=0x80+x;
	else
		if(y==2)			  //�ڵ�2����ʾ
			add=0x90+x;
		else
			if(y==3)		  //�ڵ�3����ʾ
				add=0x88+x;
			else
				if(y==4)	  //�ڵ�4����ʾ
					add=0x98+x;
	write_com(add);	 //��д��ʾ��ַ
}
/********************************************************
����void LCD12864_display_char(uchar x,uchar y,uchar dat)
LCD12864��ʾ�ַ�
�ں�����x��0~7����������y��1~4����ʾ�ַ�dat
********************************************************/
void LCD12864_display_char(uchar x,uchar y,uchar dat)
{
	LCD12864_display_pox(x,y);	 //��д��ʾ��ַ
	write_data(dat); //��д��ʾ����
}

/********************************************************
����void LCD12864_display_string(uchar x,uchar y,uchar *s)
LCD12864��ʾ�ַ�
�ں�����x��0~7����������y��1~4����ʾ�ַ���*s
********************************************************/
void LCD12864_display_string(uchar x,uchar y,uchar *s)
{ 
	LCD12864_display_pox(x,y);	 //��д��ʾ��ַ
	while(*s!='\0')
	{ 
		write_data(*s);
		s++;
		LCD12864_delay(1);
	}                                           
} 

/********************************************************
����void LCD12864_init(void)
LCD12864��ʼ��
********************************************************/
void LCD12864_init(void)
{
	write_com(0x30);       //ѡ�����ָ�
	write_com(0x30);       //ѡ��8bit������
	write_com(0x0c);       //����ʾ(���αꡢ������)
	write_com(0x01);       //�����ʾ�������趨��ַָ��Ϊ00H
	write_com(0x06);       //ָ�������ϵĶ�ȡ��д��ʱ���趨�α���ƶ�����ָ����ʾ����λ�������������1λ�ƶ� 
}

#endif