/*************************************************************
                      AT24C02ͷ�ļ�

ʵ�ֹ��ܣ�AT24C02�Ŀ���
***************************************************************/
#ifndef _AT24C02_H_ 
#define _AT24C02_H_
#include<reg52.h>
#include<intrins.h>

#define uchar unsigned char
#define uint unsigned int

/*********************AT24C02���Ŷ���*********************/ 
sbit sda=P0^1;
sbit scl=P0^2;

/*********************AT24C02��������*********************/
void  AT24C02_start();							   //��ʼ�ź�
void  AT24C02_stop();							   //ֹͣ�ź�
void  AT24C02_respons();						   //Ӧ���ź�
void  AT24C02_write(uchar date);				   //д����
uchar AT24C02_read();							   //������
void  AT24C02_write_date(uchar address,uchar date);//��address��ַд��dete����
uchar AT24C02_read_date(uchar address);			   //��address�������ݲ�����
extern void LCD12864_delay(uint x);
/*********************************************************
����void AT24C02_start() 
AT24C02��ʼ�ź�
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
����void AT24C02_stop() 
��������:AT24C02ֹͣ�ź�
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
��void AT24C02_respons() 
����AT24C02Ӧ���ź�
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
����void AT24C02_write(uchar date)
AT24C02дһ���ֽ�����
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
����uchar AT24C02_read()
AT24C02��һ���ֽ�����
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
����void AT24C02_write_date(uchar address,uchar date��
AT24C02д����
������dateд�뵽address��ַ��
*********************************************************/
void AT24C02_write_date(uchar address,uchar date)
{
	AT24C02_start();		//��ʼ�ź�
	AT24C02_write(0xa0); 	//дʱ��д��AT24C02������ַ��������������A2��A1��A0���ǽӵأ�����������ַΪ0xa0
	AT24C02_respons();		//Ӧ���ź�
	AT24C02_write(address); //д�뽫Ҫ������ݵĵ�ַ
	AT24C02_respons();		//Ӧ���ź�
	AT24C02_write(date);    //д������
	AT24C02_respons();		//Ӧ���ź�
	AT24C02_stop();			//ֹͣ�ź�
	LCD12864_delay(2);
}

/*********************************************************
����uchar AT24C02_read_date(uchar address)
AT24C02������
��address��ַ�����һ�����ݣ�������
*********************************************************/
uchar AT24C02_read_date(uchar address)
{
	uchar date; 
	AT24C02_start();			   //��ʼ�ź�
	AT24C02_write(0xa0); 		   //дʱ��д��AT24C02������ַ��������������A2��A1��A0���ǽӵأ�����������ַΪ0xa0
	AT24C02_respons();			   //Ӧ���ź�
	AT24C02_write(address);//д�뽫Ҫ��ȡ���ݵĵ�ַ
	AT24C02_respons();			   //Ӧ���ź�
	AT24C02_start();			   //��ʼ�ź�
	AT24C02_write(0xa1);		   //��ʱ��д��AT24C02������ַ��������������A2��A1��A0���ǽӵأ�����������ַΪ0xa1
	AT24C02_respons();			   //Ӧ���ź�
	date=AT24C02_read(); 		   //��ȡ����
	AT24C02_stop();
	LCD12864_delay(2);
	return date;				   //��������
}
#endif