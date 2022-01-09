/*************************************************************
                         �����ƿ��ڻ�ָ������������
												 
��������ָ�ƽ��������������ɣ�����ʶ�𲿷ֻ��ڽ���
          ����ѧ�ͱ��˵İ����������һ����pcb�����ƺͺ��ӣ�ʵ����ָ�����벿�ֵĹ���


��������˵�������ڵ���ʵ��ָ�����벿�ֹ��ܶԵ�Ƭ��������Ҫ�󲻸ߣ�ҲΪ�˷��������������Ͻ�
              �м򵥵��ԣ��ʲ�����һ��ʼѧϰ���õ�stc12c5a16s2��Ƭ����������������pcb��ʹ�õ���
              89c52�������ճ��������õ�ʱ�����c52��16s2���ٶȲ�ͬ�������˵�����

              ̧��ϵͳ���ü̵���hk4100f�������
  
              �洢оƬ���õ�AT24C02
							
							���̲�����4*4�Ķ������������ǵ�4*4���ܹ���������ʹ��8���ӿڸ��ӷ��㣬�ʲ�δ����4*5�ż�
                      

Ӧ����Ƶ�Ѿ���ɣ�������ѹ������������շŵ�bվ�ϵ���Ƶ�������������У��ᾡ����ɲ��ϴ���

***************************************************************/
#include<reg52.h>			//ͷ�ļ�
#include<LCD12864.h>
#include<AT24C02.h>
#include<KEY.h>
#include<AS608.h>
#include<T2.h>

#define uchar unsigned char	//�궨��
#define uint unsigned int
#define ulong unsigned long

/************************���Ŷ���************************/
sbit buzz=P3^7; //������
sbit led=P0^0;  //LED��

/************************��������************************/
#define MAX 20   //ָ���������
#define ID_MAX 3 //����ID���λ��

uchar mode=0;    //0����������1������棬2¼��ָ��ģʽ��3ɾ��ָ��ģʽ��4�޸�����

uchar mode_num=0;//�������ѡ����

uint ID=0;	 	 //�洢������ID
uchar n=0;	     //�����λ��
bit zhiwen_f=0;	 //1=��ǽ���ָ�Ʋɼ�

uchar a;		 //�洢����ֵ		 
uchar num;	 	 //��¼���������λ��
bit   in_f;		 //��������ı�־
bit   reset_f;	 //��������ı�־
bit   input_f;	 //��λ��������ı�־
uchar reset_num; //����޸�����׶Σ�=1��������룬=2���������룬=3�ٴ�����������

ulong low_mima=0;//ϵͳ����
ulong in_mima=0; //�ݴ����������
ulong new_mima=0;//�ݴ�����������

uchar temp[5]="    \0";//�洢��ʾ����  
/********************************************************
����void delayms(uint ms)
������ʱ����
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
����void didi(uchar i)
����������
iΪ���д���
********************************************************/ 
void didi(uchar i)
{
	uchar j;
	for(j=0;j<i;j++)//ѭ��i�Σ�LED�����������м���
	{
		led=0;		//����LED��������
		buzz=0;
		delayms(20);
		led=1;		//�ر�LED��������
		buzz=1;
		delayms(15);
	}
}
/********************************************************
����void diaplay_mode0()
��ʾ������������
********************************************************/
void diaplay_mode0()
{
	LCD12864_display_string(1,1,"ָ��������");
	LCD12864_display_string(0,2,"���: ");	
	LCD12864_display_string(0,3,"״̬: ");
}
/********************************************************
����void xunzhiwen()
����ָ��
********************************************************/
void xunzhiwen()
{
	searchnum=search(); //����ָ��
	if((searchnum>=1&&searchnum<=MAX)||searchnum==255)//���MAX��ָ��
	{	  
		if(searchnum>=1&&searchnum<=MAX)      //ָ��ʶ��ɹ�
		{
			temp[0]=searchnum%1000/100+0x30;
			temp[1]=searchnum%100/10+0x30;
			temp[2]=searchnum%10+0x30;
			temp[3]=' ';
			LCD12864_display_string(3,2,temp);//��ʾָ�Ʊ��
			LCD12864_display_string(3,3,"ָ�ƽ���");
			LCD12864_display_string(1,4,"            ");
			in_mima=0;						  //��������������
			num=0;							  //��������������λ��
			input_f=0;						  //������������־
			jdq=0;							  //�̵������ϣ��򿪵����
			T2_num=0;						  //�������¿�ʼ��ʱ
			TR2=1;
		}
		else                                  //����ȷ��ָ��
		{
			LCD12864_display_string(3,3,"��Чָ��");
			didi(3);						  //��������
		}
		delay1ms(500);						  //��ʱ���Զ������ʾ����
		LCD12864_display_string(3,2,"    ");
		LCD12864_display_string(3,3,"        ");
	}								  		
}

/********************************************************
����void cunchu()
�洢ָ��
********************************************************/
void cunchu()
{
	LCD12864_display_string(0,3,"״̬: ");

	if(ID>0&&ID<=MAX)  //����Ĵ洢ָ��λ����ȷ
	{
		if(enroll()==1)//�ɼ����Σ�����1��ָ��ģ��ɹ�
		{
			if(savefingure(ID)==1)//����ָ�Ƴɹ�
			{
				LCD12864_display_string(3,3,"¼��ɹ�");
			} 
			else				  //����ָ��ʧ��
				LCD12864_display_string(3,3,"�洢ʧ��");	
		}
		else		              //�ɼ�ָ��ʧ��
			LCD12864_display_string(3,3,"�ɼ�ʧ��");	
	}
	else			              //����ָ�Ʊ�Ŵ���
		LCD12864_display_string(3,3,"��Ŵ���");

	n=0;	   //���������λ��
	ID=0;	   //���������
	zhiwen_f=0;//�ر�ָ�Ʋɼ�
}
/********************************************************
����void shanchu()
ɾ��ָ��
********************************************************/
void shanchu()
{
	LCD12864_display_string(0,3,"״̬: ");
	if(ID>0&&ID<=MAX)		  //����Ĵ洢ָ��λ����ȷ
	{
		if(dellfingure(ID)==1)//ɾ��ָ�Ƴɹ�
		{
			LCD12864_display_string(3,3,"ɾ���ɹ�");
		}
		else				  //ɾ��ָ��ʧ��
			LCD12864_display_string(3,3,"ɾ��ʧ��");	
	}
	else
	{
		if(ID==999)			  //����������999��ʾ���ָ��
		{
			if(Clear_All()==1)//���ָ�Ƴɹ�
				LCD12864_display_string(3,3,"���ָ��");
			else			  //���ָ��ʧ��
				LCD12864_display_string(3,3,"���ʧ��");	
		}
		else				  //�����������������
			LCD12864_display_string(3,3,"��Ŵ���");
	}
		
	n=0;	   //���������λ��
	ID=0;	   //���������
	zhiwen_f=0;//�ر�ָ�Ʋɼ�						  		
}

/********************************************************
��void display_guanli()
��ʾ�������
********************************************************/
void display_guanli()
{
	LCD12864_display_string(0,1,"  ����ϵͳ����  ");
	LCD12864_display_string(0,2,"    ¼��ָ��    ");
	LCD12864_display_string(0,3,"    ɾ��ָ��    ");
	LCD12864_display_string(0,4,"    �޸�����    ");
	switch(mode_num)		
	{
	   case 0: LCD12864_display_string(0,2," >> "); break;
	   case 1: LCD12864_display_string(0,3," >> "); break;
	   case 2: LCD12864_display_string(0,4," >> "); break;
	   default: break;
	}
}
/*********************************************************
����void chuli()
�������µİ������д���
*********************************************************/
void chuli()
{
	if(key_f==1)	    //���ж��Ƿ��а�������
	{	
		if((mode==2||mode==3)&&a<10)//�������ID״̬�����Ұ��µ������ּ�
		{
			if(n<ID_MAX)//�ж��Ƿ�Ϊ����״̬����������δ���������λ�����ſ��Լ�������
			{
				if(n==0)//�ж�Ϊ��ŵ�1λ		    
				{	
					LCD12864_display_string(3,2,"        ");
					LCD12864_display_string(0,3,"              ");				
				} 
				LCD12864_display_char(3+n,2,a+0x30);//��ʾ�����������  ASCII[a]

				ID=ID*10+a;//��ż���	  
				n++;	   //�������λ��+1
			}					   					
		}

		if((mode==0||mode==4)&&input_f==0&&a<10)//�����in_f=1����������״̬�������Ұ��µ������ּ�
		{			
			if(in_f==1||mode==4)
			{	
				if(num==0)                      //�����ʾ����ʾ����������
					LCD12864_display_string(1,3,"           ");	
				if(reset_f==1&&reset_num>1)		//����������ʱ������������׶�
				    LCD12864_display_char(num+1,2,a+0x30);//��ʾ���������
				else							//�������������������ʾ*
					LCD12864_display_char(num+1,2,'*');   //��ʾ��*��
			}
			else
				LCD12864_display_char(num+1,4,'*');//��ʾ��*��
			in_mima=in_mima*10+a;//��������	
			num++;			     //�������������һ			
			if(num==6)		     //��λ�����������								   	   				   							
				input_f=1;	     //�����λ�����������	   					
		}

		if(a==10)				 //����������
		{
			if(mode==0&&in_f==0) //��������
			{
				num=0;			 //��0 ��������λ��
				in_f=1;			 //�����������
				in_mima=0;		 //��0 ���������
				input_f=0;       //�������������ɱ�־
				write_com(0x01); //�����ʾ
				LCD12864_display_string(0,1,"�������������: ");//��ʾ��������
			}
		}
		if(a==15)                //������һ���˵�
		{
			if(mode>1)			 //�ڹ��������
			{	 
				reset_f=0;
				reset_num=0;
				write_com(0x01); //�����ʾ
				mode=1;		     //�ص�����ѡ�����
			}
			else				 //�ǹ������
			{
				if(mode!=0||in_f==1)//�������������׶�
				{
					write_com(0x01);//�����ʾ
					mode=0;
					diaplay_mode0();//��ʾ������
					in_f=0;
					num=0;
					input_f=0;		//�������������ɱ�־
				}
			}
			
			if(jdq==0)				//���������򿪣����¸ð������ֶ�����
			{
				jdq=1;				//�رռ̵���
				T2_num=0;			//�����ʱ
				TR2=0;				//�رն�ʱ��
			}			   
		}
				
		if(a==11)      //ѡ����һ��
		{
			if(mode==1)//ѡ����һ�����	   
			{
				if(mode_num==0)
					mode_num=2;	
				else
					mode_num--;	
			}
		}
		if(a==12)      //ѡ����һ��
		{
			if(mode==1)//ѡ����һ�����
			{
				if(mode_num>=2)
					mode_num=0;	
				else
					mode_num++;		
			}	
		}
		if(a==13)      //�˸�
		{					 					 
			 if(mode==0||mode==4)//����������״̬
			 {
				if(num!=0)		 //��ǰ�Ѿ���������	 
				{			 
					num--;       //�������������1
					if(in_f==1||mode==4)
						LCD12864_display_char(num+1,2,' ');	
					else
						LCD12864_display_char(num+1,4,' ');	
				
					in_mima=in_mima/10;	  
					input_f=0;
				}
			 }
			 if(mode==2||mode==3)//��ID�������״̬
			 {
			 	if(n!=0)		 //��ǰ�Ѿ���������
				{
					LCD12864_display_char(2+n,2,' ');
					n--;
					ID=ID/10;
					zhiwen_f=0;
				}
			 }
		}

		if(a==14)//ȷ��
		{
			if(mode==1)		    //ȷ��ѡ��������
			{	
				write_com(0x01);//�����ʾ
				mode=mode_num+2;
				n=0;
				ID=0;
				zhiwen_f=0; 
				input_f=0;   //���ȷ����־
				num=0;
				in_mima=0;
				new_mima=0;
				switch(mode_num)		
				{
				   case 0:  	
								LCD12864_display_string(0,1,">>  ¼��ָ��");
								LCD12864_display_string(0,2,"���: ");
								break;
				   case 1:  	
								LCD12864_display_string(0,1,">>  ɾ��ָ��");
								LCD12864_display_string(0,2,"���: ");
								break;
				   case 2:  	
								reset_f=1;
								reset_num=1;
								LCD12864_display_string(0,1,"�����������: "); 
								break;	 
				   default: break;
				}	
			}
			else 
			{
				if(input_f==1)     //�����������ȷ��
				{
					if(reset_f==1) //�޸���������
					{
						if(reset_num==3)//�ٴ�����������
						{	
							if(new_mima==in_mima)//���������������һ��
							{	
								low_mima=new_mima;		 
								AT24C02_write_date(200,low_mima/10000);	   //�洢����
								AT24C02_write_date(201,low_mima%10000/100);
								AT24C02_write_date(202,low_mima%100);
								write_com(0x01);
								LCD12864_display_string(1,1,"�����޸ĳɹ�");
								LCD12864_display_string(1,3,"�����Ʊ��ܺ�");
							}
							else
							{
								LCD12864_display_string(1,3,"�����޸�ʧ��");
							}
							reset_num=0;
							reset_f=0;	
						} 
						if(reset_num==2)//����������
						{	
							write_com(0x01);
							LCD12864_display_string(0,1,"���ٴ�����������");		
							new_mima=in_mima;
							reset_num++;	
						} 
						if(reset_num==1)//���������
						{
							if(low_mima==in_mima)//���������;�����һ��
							{	
								write_com(0x01);
								reset_num++;
								LCD12864_display_string(0,1,"������������: ");//��ʾ����������
							}
							else
							{
								LCD12864_display_string(2,3,"�������");	//��ʾ�������
								reset_num=1;
								LCD12864_display_string(1,2,"            ");	
							}
						}
					}
					if(in_f==1)		   //���������������ʱ��
					{
						if(low_mima==in_mima)//�����������ȷ
						{						
							mode=1;	   //��ǽ���������
							write_com(0x01);
							in_f=0;
						}
						else
						{
							LCD12864_display_string(2,3,"�������");	//��ʾ�������
							LCD12864_display_string(1,2,"            ");	
						}
					} 

					if(mode==0&&in_f==0)	 //�������
					{
						if(low_mima==in_mima)//�����������ȷ
						{					
							LCD12864_display_string(3,3,"�����ɹ�");
							jdq=0;			 //�̵������ϣ��������
							T2_num=0;		 //��0 ����
							TR2=1;			 //������ʱ��2
						}
						else
						{
							LCD12864_display_string(3,3,"�������");	//��ʾ�������
							didi(3);
						}

						LCD12864_display_string(1,4,"            ");
						delay1ms(500);
						LCD12864_display_string(3,3,"        ");	
					}
					in_mima=0;
					num=0;		//���������������¼���������������
					input_f=0;  //���ȷ����־
				}

				if((mode==2||mode==3)&&n==ID_MAX)//ע���ɾ��ָ��ʱ���������ɺ���ָ�Ƽ��
				{
					zhiwen_f=1;	//��ǽ���ָ�Ʋɼ�	
				}
			}			
		}						
		key_f=0;        //���������־	
	}
}
/*********************************************************
����void main()
������
*********************************************************/
void main()
{
	uchar i;

	LCD12864_init();	   //��ʼ��LCD12864	
	UartInit();			   //���ڳ�ʼ��
	T2_init();			   //��ʱ��2��ʼ��
	low_mima=AT24C02_read_date(200);			 //��ȡ�洢��AT24C02������
	low_mima=low_mima*100+AT24C02_read_date(201);
	low_mima=low_mima*100+AT24C02_read_date(202);

	diaplay_mode0();       //��ʾ��ʼ����
	while(1)
	{
		i++;
		a=keycan();		   //������̼��
		chuli();		   //������̴���
		
		if(mode==1)
			display_guanli();		  //��ʾ�������
		else
			if(mode==2)				  //¼��ָ��
			{
				if(zhiwen_f==1)
					cunchu();		  //¼��ָ��
			}
			else
				if(mode==3)			  //ע��ָ��
				{
					if(zhiwen_f==1)
						shanchu();	  //ע��ָ��		
				}
				else				
					if(mode==0&&in_f==0)//������������
					{
						if(i>=150)
						{
							i=0;
							xunzhiwen();//ָ�ƽ���
						}
					}
	}
}