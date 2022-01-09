/*************************************************************
                      矩阵键盘头文件
 
实现功能：矩阵键盘的控制

***************************************************************/
#ifndef _KEY_H_
#define _KEY_H_
#include<reg52.h>
#define uchar unsigned char
#define uint unsigned int

/*****************矩阵键盘引脚定义*******************/
#define Key P1

bit key_f;//有按键按下标志
/*****************矩阵键盘函数声明*******************/
uchar jiema(unsigned char key);	//解码函数，输入按键编码，返回按键位置
void  key_delay();				//延时函数
uchar keycan(void);				//按键查询函数，返回矩阵键盘位置
/********************************************************
函数uchar jiema(unsigned char key)
转换按键码为1~16的数字
返回按下的按键位置
*******************************************************/
uchar jiema(uchar key)
{
   uchar n;
   switch(key)
   {
	   case 0x11: n= 13; break;
	   case 0x21: n= 0; break;
	   case 0x41: n= 14; break;
	   case 0x81: n= 15; break;
	   case 0x12: n= 7; break;
	   case 0x22: n= 8; break;
	   case 0x42: n= 9; break;
	   case 0x82: n= 12; break;
	   case 0x14: n= 4; break;
	   case 0x24: n= 5; break;
	   case 0x44: n= 6; break;
	   case 0x84: n= 11; break;
	   case 0x18: n= 1; break;
	   case 0x28: n= 2; break;
	   case 0x48: n= 3; break;
	   case 0x88: n= 10; break;
	   default: break;
   }
   key_f=1;
   return n;
}

/********************************************************
函数void key_delay()
延时函数
********************************************************/
void key_delay()                   //延时子程序
{
	uchar n, m;
	for (n = 100; n > 0; n--)
		for (m = 300; m > 0; m--);    
} 

/********************************************************
函数uchar keycan(void)
进行按键扫描
返回按键值，=0时表示没有按键按下
********************************************************/ 
uchar keycan(void)
{
	uchar rcode, ccode;
	Key = 0xF0;      // 发全0行扫描码，列线输入
	if((Key&0xF0) != 0xF0) // 若有键按下
	{
		key_delay();// 延时去抖动 
		if((Key&0xF0) != 0xF0)
		{  
			rcode = 0xFE;         // 逐行扫描初值
		    while((rcode&0x10) != 0)
		    {
		        Key = rcode;         // 输出行扫描码
			    if((Key&0xF0) != 0xF0) // 本行有键按下
			    {
				       ccode = (Key&0xF0)|0x0F;
				       do{;}
					   while((Key&0xF0) != 0xF0); //等待键释放
				       return jiema((~rcode) + (~ccode)); // 返回键编码
			    }
			    else
			    	rcode = (rcode<<1)|0x01; // 行扫描码左移一位
			}
		}
	}  
	return 0; // 无键按下，返回值为0
}
#endif