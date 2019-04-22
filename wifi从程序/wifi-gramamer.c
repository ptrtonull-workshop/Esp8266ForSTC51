/***************************

先把wifi模块设在flash置  AT+CWMODE=1   AT+CWJAP="ESP8266","123456789" 

修改波特率9600

***************************/
#include<reg52.h>
#include"string.h"
#include <ctype.h>
#define uchar unsigned char
#define uint unsigned int
#define	RX1_Lenth	16			//串口接收缓冲长度
uchar	RX1_Cnt=0;	                //接收计数
bit flag=0;
void Tranfer(uchar *s)
{
	 ES=0; 
   while(*s!='\0')
   {
     SBUF= toascii(*s);
		 while(!TI);
	   TI=0;
     s++;
   }
   ES=1;
}						 
void Tranfer_f(uchar *s,uchar Lenth)
{
	 ES=0; 
   while(Lenth--!=0)
   {
     SBUF= toascii(*s++);
		 while(!TI);
	   TI=0;
   }
   ES=1;
}
uchar idata RX1_Buffer[RX1_Lenth]=0;	//接收缓冲

void Delay2(unsigned long cnt)
{
	long i;
 	for(i=0;i<cnt*10;i++);
}
/* 配置wifi */
void set_wifi()
{
	Delay2(500);
  Tranfer("AT+RST\r\n");	
	Delay2(1000);
	Tranfer("AT+CIPMUX=0\r\n");	 //单链接
	Delay2(500);
  Tranfer("AT+CIPSTART=\"TCP\",\"192.168.4.1\",5000\r\n");
	Delay2(1000);
  Tranfer("AT+CIPMODE=1\r\n");	 //透传模式
	Delay2(500);
	Tranfer("AT+CIPSEND=10\r\n");
	Delay2(500);
}
void main()
{
  SCON=0X50;			//设置为工作方式1
	TMOD=0X20;			//设置计数器工作方式1
	TH1=0XFD;		    //计数器初始值设置，注意波特率是9600
	TL1=0XFD;	
	TR1=1;						    //打开计数器
	set_wifi();
	ES=1;						//打开接收中断
	EA=1;					//打开总中断
	while(1)
	{
	  /*一直在采集信息，采集到信息 发送数据*/
	  /*  实验用灯显示   */
		Delay2(500);
	  Tranfer("AT+CIPSEND=11\r\n");
		Delay2(200);
	  Tranfer("R432.240.1E");   //	 给主机发送十一个字节的数据
		Delay2(500);
	  if(flag==1)
    {  
	    Tranfer_f(RX1_Buffer,11);	//	 发送十一个字节数据给上位机	 第二位ID
		  flag=0;
	  }
	}
}
void Usart() interrupt 4
{   
	if(RI)
	{
	   RI=0;
	   RX1_Buffer[RX1_Cnt]=SBUF;	//接收缓冲   接收完数据显示
	   if(RX1_Buffer[0]=='T')	// T
	   {
				RX1_Cnt++;
	   }
		 else
		 {  
				RX1_Cnt=0;
		 }
		 if(RX1_Cnt>=10)
		 {
				RX1_Cnt=0;
				flag=1;	   
		}   
	}
}