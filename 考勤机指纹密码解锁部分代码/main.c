/*************************************************************
                         智能云考勤机指纹密码锁部分
												 
完成情况：指纹解锁和密码解锁完成，人脸识别部分还在进行
          在自学和别人的帮助下完成了一个简单pcb板的设计和焊接，实现了指纹密码部分的功能


部件部分说明：由于单独实现指纹密码部分功能对单片机的性能要求不高，也为了方便代码在面包板上进
              行简单调试，故采用了一开始学习所用的stc12c5a16s2单片机。（由于最终在pcb上使用的是
              89c52，故最终程序所采用的时序根据c52和16s2的速度不同而做出了调整）

              抬杆系统改用继电器hk4100f跟电磁锁
  
              存储芯片采用的AT24C02
							
							键盘采用了4*4的独立按键，考虑到4*4功能够用且正好使用8个接口更加方便，故并未采用4*5排键
                      

应用视频已经完成（放在了压缩包里），但最终放到b站上的视频还在制作过程中，会尽快完成并上传！

***************************************************************/
#include<reg52.h>			//头文件
#include<LCD12864.h>
#include<AT24C02.h>
#include<KEY.h>
#include<AS608.h>
#include<T2.h>

#define uchar unsigned char	//宏定义
#define uint unsigned int
#define ulong unsigned long

/************************引脚定义************************/
sbit buzz=P3^7; //蜂鸣器
sbit led=P0^0;  //LED灯

/************************变量定义************************/
#define MAX 20   //指纹最大数量
#define ID_MAX 3 //输入ID最大位数

uchar mode=0;    //0正常解锁，1管理界面，2录入指纹模式，3删除指纹模式，4修改密码

uchar mode_num=0;//管理界面选择项

uint ID=0;	 	 //存储输入编号ID
uchar n=0;	     //输入的位数
bit zhiwen_f=0;	 //1=标记进入指纹采集

uchar a;		 //存储按键值		 
uchar num;	 	 //记录输入的密码位数
bit   in_f;		 //输入密码的标志
bit   reset_f;	 //重设密码的标志
bit   input_f;	 //六位密码输完的标志
uchar reset_num; //标记修改密码阶段，=1输入旧密码，=2输入新密码，=3再次输入新密码

ulong low_mima=0;//系统密码
ulong in_mima=0; //暂存输入的密码
ulong new_mima=0;//暂存输入新密码

uchar temp[5]="    \0";//存储显示内容  
/********************************************************
函数void delayms(uint ms)
毫秒延时函数
********************************************************/ 
void delayms(uint ms)
{
	unsigned char i=100,j;
	for(;ms;ms--)
	{
		while(--i)
		{
			j=10;
			while(--j);
		}
	}
}
/********************************************************
函数void didi(uchar i)
蜂鸣器鸣叫
i为鸣叫次数
********************************************************/ 
void didi(uchar i)
{
	uchar j;
	for(j=0;j<i;j++)//循环i次，LED、蜂鸣器鸣叫几次
	{
		led=0;		//开启LED、蜂鸣器
		buzz=0;
		delayms(20);
		led=1;		//关闭LED、蜂鸣器
		buzz=1;
		delayms(15);
	}
}
/********************************************************
函数void diaplay_mode0()
显示密码锁主界面
********************************************************/
void diaplay_mode0()
{
	LCD12864_display_string(1,1,"指纹密码锁");
	LCD12864_display_string(0,2,"编号: ");	
	LCD12864_display_string(0,3,"状态: ");
}
/********************************************************
函数void xunzhiwen()
查找指纹
********************************************************/
void xunzhiwen()
{
	searchnum=search(); //搜索指纹
	if((searchnum>=1&&searchnum<=MAX)||searchnum==255)//最多MAX个指纹
	{	  
		if(searchnum>=1&&searchnum<=MAX)      //指纹识别成功
		{
			temp[0]=searchnum%1000/100+0x30;
			temp[1]=searchnum%100/10+0x30;
			temp[2]=searchnum%10+0x30;
			temp[3]=' ';
			LCD12864_display_string(3,2,temp);//显示指纹编号
			LCD12864_display_string(3,3,"指纹解锁");
			LCD12864_display_string(1,4,"            ");
			in_mima=0;						  //清除以输入的密码
			num=0;							  //清除以输入的密码位数
			input_f=0;						  //清除输入密码标志
			jdq=0;							  //继电器吸合，打开电磁锁
			T2_num=0;						  //开启重新开始计时
			TR2=1;
		}
		else                                  //不正确的指纹
		{
			LCD12864_display_string(3,3,"无效指纹");
			didi(3);						  //报警三次
		}
		delay1ms(500);						  //延时后自动清除显示内容
		LCD12864_display_string(3,2,"    ");
		LCD12864_display_string(3,3,"        ");
	}								  		
}

/********************************************************
函数void cunchu()
存储指纹
********************************************************/
void cunchu()
{
	LCD12864_display_string(0,3,"状态: ");

	if(ID>0&&ID<=MAX)  //输入的存储指纹位置正确
	{
		if(enroll()==1)//采集两次，生成1个指纹模板成功
		{
			if(savefingure(ID)==1)//保存指纹成功
			{
				LCD12864_display_string(3,3,"录入成功");
			} 
			else				  //保存指纹失败
				LCD12864_display_string(3,3,"存储失败");	
		}
		else		              //采集指纹失败
			LCD12864_display_string(3,3,"采集失败");	
	}
	else			              //输入指纹编号错误
		LCD12864_display_string(3,3,"编号错误");

	n=0;	   //清除输入编号位数
	ID=0;	   //清除输入编号
	zhiwen_f=0;//关闭指纹采集
}
/********************************************************
函数void shanchu()
删除指纹
********************************************************/
void shanchu()
{
	LCD12864_display_string(0,3,"状态: ");
	if(ID>0&&ID<=MAX)		  //输入的存储指纹位置正确
	{
		if(dellfingure(ID)==1)//删除指纹成功
		{
			LCD12864_display_string(3,3,"删除成功");
		}
		else				  //删除指纹失败
			LCD12864_display_string(3,3,"删除失败");	
	}
	else
	{
		if(ID==999)			  //如果输入的是999表示清空指纹
		{
			if(Clear_All()==1)//清空指纹成功
				LCD12864_display_string(3,3,"清空指纹");
			else			  //清空指纹失败
				LCD12864_display_string(3,3,"清空失败");	
		}
		else				  //输入的是其它错误编号
			LCD12864_display_string(3,3,"编号错误");
	}
		
	n=0;	   //清除输入编号位数
	ID=0;	   //清除输入编号
	zhiwen_f=0;//关闭指纹采集						  		
}

/********************************************************
函void display_guanli()
显示管理界面
********************************************************/
void display_guanli()
{
	LCD12864_display_string(0,1,"  管理系统界面  ");
	LCD12864_display_string(0,2,"    录入指纹    ");
	LCD12864_display_string(0,3,"    删除指纹    ");
	LCD12864_display_string(0,4,"    修改密码    ");
	switch(mode_num)		
	{
	   case 0: LCD12864_display_string(0,2," >> "); break;
	   case 1: LCD12864_display_string(0,3," >> "); break;
	   case 2: LCD12864_display_string(0,4," >> "); break;
	   default: break;
	}
}
/*********************************************************
函数void chuli()
对所按下的按键进行处理
*********************************************************/
void chuli()
{
	if(key_f==1)	    //先判断是否有按键按下
	{	
		if((mode==2||mode==3)&&a<10)//如果输入ID状态，并且按下的是数字键
		{
			if(n<ID_MAX)//判断是否为输入状态，并且输入未满最大整数位数，才可以继续输入
			{
				if(n==0)//判断为编号第1位		    
				{	
					LCD12864_display_string(3,2,"        ");
					LCD12864_display_string(0,3,"              ");				
				} 
				LCD12864_display_char(3+n,2,a+0x30);//显示出输入的数字  ASCII[a]

				ID=ID*10+a;//编号计算	  
				n++;	   //编号输入位数+1
			}					   					
		}

		if((mode==0||mode==4)&&input_f==0&&a<10)//如果在in_f=1（输入密码状态），并且按下的是数字键
		{			
			if(in_f==1||mode==4)
			{	
				if(num==0)                      //清除显示后、显示输入新密码
					LCD12864_display_string(1,3,"           ");	
				if(reset_f==1&&reset_num>1)		//在重置密码时候输入新密码阶段
				    LCD12864_display_char(num+1,2,a+0x30);//显示输入的数字
				else							//其它情况下输入密码显示*
					LCD12864_display_char(num+1,2,'*');   //显示“*”
			}
			else
				LCD12864_display_char(num+1,4,'*');//显示“*”
			in_mima=in_mima*10+a;//读入密码	
			num++;			     //密码输入次数加一			
			if(num==6)		     //六位密码输入完成								   	   				   							
				input_f=1;	     //标记六位密码输入完成	   					
		}

		if(a==10)				 //进入管理界面
		{
			if(mode==0&&in_f==0) //输入密码
			{
				num=0;			 //清0 输入密码位数
				in_f=1;			 //标记输入密码
				in_mima=0;		 //清0 输入的密码
				input_f=0;       //清除密码输入完成标志
				write_com(0x01); //清除显示
				LCD12864_display_string(0,1,"请输入管理密码: ");//显示输入密码
			}
		}
		if(a==15)                //返回上一级菜单
		{
			if(mode>1)			 //在管理操作中
			{	 
				reset_f=0;
				reset_num=0;
				write_com(0x01); //清除显示
				mode=1;		     //回到管理选择界面
			}
			else				 //非管理界面
			{
				if(mode!=0||in_f==1)//如果在输入密码阶段
				{
					write_com(0x01);//清除显示
					mode=0;
					diaplay_mode0();//显示主界面
					in_f=0;
					num=0;
					input_f=0;		//清除密码输入完成标志
				}
			}
			
			if(jdq==0)				//如果电磁锁打开，按下该按键，手动关门
			{
				jdq=1;				//关闭继电器
				T2_num=0;			//清除计时
				TR2=0;				//关闭定时器
			}			   
		}
				
		if(a==11)      //选择上一项
		{
			if(mode==1)//选择上一项管理	   
			{
				if(mode_num==0)
					mode_num=2;	
				else
					mode_num--;	
			}
		}
		if(a==12)      //选择下一项
		{
			if(mode==1)//选择下一项管理
			{
				if(mode_num>=2)
					mode_num=0;	
				else
					mode_num++;		
			}	
		}
		if(a==13)      //退格
		{					 					 
			 if(mode==0||mode==4)//在输入密码状态
			 {
				if(num!=0)		 //当前已经有输入了	 
				{			 
					num--;       //输入密码个数减1
					if(in_f==1||mode==4)
						LCD12864_display_char(num+1,2,' ');	
					else
						LCD12864_display_char(num+1,4,' ');	
				
					in_mima=in_mima/10;	  
					input_f=0;
				}
			 }
			 if(mode==2||mode==3)//在ID编号输入状态
			 {
			 	if(n!=0)		 //当前已经有输入了
				{
					LCD12864_display_char(2+n,2,' ');
					n--;
					ID=ID/10;
					zhiwen_f=0;
				}
			 }
		}

		if(a==14)//确定
		{
			if(mode==1)		    //确定选择管理操作
			{	
				write_com(0x01);//清除显示
				mode=mode_num+2;
				n=0;
				ID=0;
				zhiwen_f=0; 
				input_f=0;   //清除确定标志
				num=0;
				in_mima=0;
				new_mima=0;
				switch(mode_num)		
				{
				   case 0:  	
								LCD12864_display_string(0,1,">>  录入指纹");
								LCD12864_display_string(0,2,"编号: ");
								break;
				   case 1:  	
								LCD12864_display_string(0,1,">>  删除指纹");
								LCD12864_display_string(0,2,"编号: ");
								break;
				   case 2:  	
								reset_f=1;
								reset_num=1;
								LCD12864_display_string(0,1,"请输入旧密码: "); 
								break;	 
				   default: break;
				}	
			}
			else 
			{
				if(input_f==1)     //密码输入完成确定
				{
					if(reset_f==1) //修改密码区间
					{
						if(reset_num==3)//再次输入新密码
						{	
							if(new_mima==in_mima)//两次输入的新密码一致
							{	
								low_mima=new_mima;		 
								AT24C02_write_date(200,low_mima/10000);	   //存储密码
								AT24C02_write_date(201,low_mima%10000/100);
								AT24C02_write_date(202,low_mima%100);
								write_com(0x01);
								LCD12864_display_string(1,1,"密码修改成功");
								LCD12864_display_string(1,3,"请妥善保管好");
							}
							else
							{
								LCD12864_display_string(1,3,"密码修改失败");
							}
							reset_num=0;
							reset_f=0;	
						} 
						if(reset_num==2)//输入新密码
						{	
							write_com(0x01);
							LCD12864_display_string(0,1,"请再次输入新密码");		
							new_mima=in_mima;
							reset_num++;	
						} 
						if(reset_num==1)//输入旧密码
						{
							if(low_mima==in_mima)//输入的密码和旧密码一致
							{	
								write_com(0x01);
								reset_num++;
								LCD12864_display_string(0,1,"请输入新密码: ");//显示输入新密码
							}
							else
							{
								LCD12864_display_string(2,3,"密码错误");	//显示密码错误
								reset_num=1;
								LCD12864_display_string(1,2,"            ");	
							}
						}
					}
					if(in_f==1)		   //进入管理输入密码时候
					{
						if(low_mima==in_mima)//输入的密码正确
						{						
							mode=1;	   //标记进入管理界面
							write_com(0x01);
							in_f=0;
						}
						else
						{
							LCD12864_display_string(2,3,"密码错误");	//显示密码错误
							LCD12864_display_string(1,2,"            ");	
						}
					} 

					if(mode==0&&in_f==0)	 //密码解锁
					{
						if(low_mima==in_mima)//输入的密码正确
						{					
							LCD12864_display_string(3,3,"解锁成功");
							jdq=0;			 //继电器吸合，电磁锁打开
							T2_num=0;		 //清0 计数
							TR2=1;			 //开启定时器2
						}
						else
						{
							LCD12864_display_string(3,3,"密码错误");	//显示密码错误
							didi(3);
						}

						LCD12864_display_string(1,4,"            ");
						delay1ms(500);
						LCD12864_display_string(3,3,"        ");	
					}
					in_mima=0;
					num=0;		//清除密码计数。重新计算输入的密码个数
					input_f=0;  //清除确定标志
				}

				if((mode==2||mode==3)&&n==ID_MAX)//注册或删除指纹时编号输入完成后开启指纹检测
				{
					zhiwen_f=1;	//标记进入指纹采集	
				}
			}			
		}						
		key_f=0;        //清除按键标志	
	}
}
/*********************************************************
函数void main()
主函数
*********************************************************/
void main()
{
	uchar i;

	LCD12864_init();	   //初始化LCD12864	
	UartInit();			   //串口初始化
	T2_init();			   //定时器2初始化
	low_mima=AT24C02_read_date(200);			 //读取存储在AT24C02的密码
	low_mima=low_mima*100+AT24C02_read_date(201);
	low_mima=low_mima*100+AT24C02_read_date(202);

	diaplay_mode0();       //显示初始界面
	while(1)
	{
		i++;
		a=keycan();		   //矩阵键盘检测
		chuli();		   //矩阵键盘处理
		
		if(mode==1)
			display_guanli();		  //显示管理界面
		else
			if(mode==2)				  //录入指纹
			{
				if(zhiwen_f==1)
					cunchu();		  //录入指纹
			}
			else
				if(mode==3)			  //注销指纹
				{
					if(zhiwen_f==1)
						shanchu();	  //注销指纹		
				}
				else				
					if(mode==0&&in_f==0)//正常解锁界面
					{
						if(i>=150)
						{
							i=0;
							xunzhiwen();//指纹解锁
						}
					}
	}
}