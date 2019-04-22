/**************************
			 《说明》
Is_F4 给手机发送信息的标志位 ，发送内容未确定。已写

各个房间发送过来的数据已确定，可接收、可显示。

可以给各个 “房间” 发送从 “操场” 接收到的温湿度数据。
 

*****************************/

#include<reg52.h>
#include"string.h"
#include <ctype.h>
#define uchar unsigned char
#define uint unsigned int

extern void print(uchar *s);
extern void lcdinit(void);
extern void write(bit start, uchar ddata);
void Dis_play();
/********************烟雾、蜂鸣器，CO**************************/
sbit YWZ=P1^1;
sbit YWC=P1^3;
sbit CO=P1^2;
sbit Bee=P1^5;
/*******************按键强制翻页********************/
sbit k2=P0^0;
sbit k1=P0^1;
sbit k4=P0^2;
sbit k3=P0^3;
/********************一大堆标志位***********************/
bit	B_TX1_Busy;	//发送忙标志	
static  bit   room1_flag=0,room2_flag=0,send_flag=0,time_flag=0,disf=0,Is_F1=0,Is_F2=0,Is_F3=0,Is_F4=0,display_flag=0,PM_F=0;
static uchar  RX1_Cnt=0,time=0,s_room;	//接收计数
static uchar ID1,ID2,ID3,ID4,ID5;  
uchar oldroom=1;
void Delay2(unsigned long cnt)
{
	long i;
 	for(i=0;i<cnt*10;i++);
}

/*输出字符串*/
void Tranfer(uchar *s)
{

	ES=0;
	TR0=0;  
   while(*s!='\0')
   {
     SBUF=toascii(*s);
	 while(!TI);
	 TI=0;
     s++;
	 Delay2(1);
   }
   ES=1; 
   TR0=1;
}
/*输出数组*/
void Tranfer_f(uchar *s,uchar Lenth)
{
	 ES=0;
	 TR0=0; 
   while(Lenth!=0)
   {
     SBUF=toascii(*s);	 
	 while(!TI);
	 TI=0;
	 s++;
	 Lenth--;
	 Delay2(1);
   }
   ES=1;  
   TR0=1;
}


#define	  RX1_Lenth		32			//串口接收缓冲长度
/*************	本地变量声明	**************/
uchar	idata RX1_Buffer[RX1_Lenth];	//接收缓冲
uchar	idata RX_Buffer1[RX1_Lenth];	//接收缓冲
uchar	idata RX_Buffer2[RX1_Lenth];	//接收缓冲
uchar	idata RX_Buffer3[RX1_Lenth];	//接收缓冲
uchar	idata RX_Buffer4[RX1_Lenth];	//接收缓冲
uchar	idata RX_Buffer5[RX1_Lenth];	//接收缓冲
uchar   PM[8],YW2[8],YW3[8];
bit YW2_F=0,YW3_F=0;


/**********************设置wifi************************/
void set_wifi()
{	
    Bee=0;
   Delay2(1000);
   write(0,0x80);
   print("系统初始化.");

   Delay2(500);  
   Tranfer("AT+CWMODE=2\r\n"); //可发送可接收
   Delay2(1000); 

   write(0,0x80);
   print("系统初始化..");

   Delay2(50);     
   Tranfer("AT+CWSAP=\"ESP8266\",\"123456789\",11,3\r\n");
   Delay2(2000);

   write(0,0x80);
   print("系统初始化...");

   Delay2(50);
   Tranfer("AT+RST\r\n");
   Delay2(1000);

   write(0,0x80);
   print("系统初始化.");

   Delay2(50);		
   Tranfer("AT+CIPMUX=1\r\n");
   Delay2(1000);

   write(0,0x80);
   print("系统初始化..");
    
   Delay2(8); 
   Tranfer("AT+CIPSERVER=1,5000\r\n");
   Delay2(1000);

   write(0,0x80);
   print("             ");
}

/****************发送数据*****************/

void Udp_send()
{
  TR0=0;ES=0;
 if(send_flag)
 { 
  
  if(Is_F1)
  {
	  send_flag=0;
	  Tranfer("AT+CIPSEND=");
	  SBUF=ID1; Delay2(1);
	  SBUF=0X2c;Delay2(1);
	  SBUF=0X31;Delay2(1);
	  SBUF=0X30;Delay2(1);
	  SBUF=0X0D;Delay2(1);
	  SBUF=0X0A;Delay2(1);
	
	  SBUF=0X52;Delay2(1);
	  SBUF=0X31;Delay2(1);
	  Tranfer_f(RX_Buffer5,8);
	  Delay2(5);
	  if(PM_F)
	  {
		  Tranfer("AT+CIPSEND=");
		  SBUF=ID1; Delay2(1);
		  SBUF=0X2c;Delay2(1);
		  SBUF=0X31;Delay2(1);
		  SBUF=0X30;Delay2(1);
		  SBUF=0X0D;Delay2(1);
		  SBUF=0X0A;Delay2(1);
		
		  SBUF=0X52;Delay2(1);
		  SBUF=0X31;Delay2(1);
		  Tranfer_f(PM,8);
		  Delay2(5);
	  }
  }	
if(Is_F2)
  {
	
	  send_flag=0;
	  Tranfer("AT+CIPSEND=");
	  SBUF=ID2;Delay2(1);
	  SBUF=0X2c;Delay2(1);
	  SBUF=0X31;Delay2(1);
	  SBUF=0X30;Delay2(1);
	  SBUF=0X0D;Delay2(1);
	  SBUF=0X0A;Delay2(1);
	
	  SBUF=0X52;Delay2(1);
	  SBUF=0X32;Delay2(1);
	  Tranfer_f(RX_Buffer5,8);
	  Delay2(5);
	  if(PM_F)
	  {
		  Tranfer("AT+CIPSEND=");
		  SBUF=ID2; Delay2(1);
		  SBUF=0X2c;Delay2(1);
		  SBUF=0X31;Delay2(1);
		  SBUF=0X30;Delay2(1);
		  SBUF=0X0D;Delay2(1);
		  SBUF=0X0A;Delay2(1);
		
		  SBUF=0X52;Delay2(1);
		  SBUF=0X31;Delay2(1);
		  Tranfer_f(PM,8);
		  Delay2(5);
	  }
  }	
if(Is_F3)
  {

	  send_flag=0;
	  Tranfer("AT+CIPSEND=");
	  SBUF=ID3;Delay2(1);
	  SBUF=0X2c;Delay2(1);
	  SBUF=0X31;Delay2(1);
	  SBUF=0X30;Delay2(1);
	  SBUF=0X0D;Delay2(1);
	  SBUF=0X0A;Delay2(1);
	
	  SBUF=0X52;Delay2(1);
	  SBUF=0X33;Delay2(1);
	  Tranfer_f(RX_Buffer5,8);
	  Delay2(5);
	  if(PM_F)
	  {
		  Tranfer("AT+CIPSEND=");
		  SBUF=ID3; Delay2(1);
		  SBUF=0X2c;Delay2(1);
		  SBUF=0X31;Delay2(1);
		  SBUF=0X30;Delay2(1);
		  SBUF=0X0D;Delay2(1);
		  SBUF=0X0A;Delay2(1);		
		  SBUF=0X52;Delay2(1);
		  SBUF=0X31;Delay2(1);
		  Tranfer_f(PM,8);
		  Delay2(5);
	  }
  }	
  if(Is_F4)//手机  要测试
  {
	  
		  Tranfer("AT+CIPSEND=");
		  SBUF=ID4; Delay2(1);
		  SBUF=0X2c;Delay2(1);
		  SBUF=0X31;Delay2(1);
		  SBUF=0X30;Delay2(1);
		  SBUF=0X0D;Delay2(1);
		  SBUF=0X0A;Delay2(1);
		  if(oldroom==1)
		  {
		    Tranfer("R400700001");
		     Delay2(5);
	 	  }
		  if(oldroom==2)
		  {
		  	Tranfer("R401600001");
		     Delay2(5);
		  }
		 if(oldroom==3)
		  {
		  	Tranfer("R400700000");
		     Delay2(5);
		  }
  }	
  
 }
 ES=1;
 TR0=1;
}

/******************系统初始化*********************/
void SysInit()
{
    lcdinit();    
    SCON=0X50;			//设置为工作方式1
	TMOD=0X21;			//设置计数器工作方式1	定时器0
	TH1=0XFD;		    //计数器初始值设置，注意波特率是9600
	TL1=0XFD;	
	TH0=0X3C;	        //50ms中断一次
	TL0=0XB0;					
	TR1=1;					    //打开计数器
    set_wifi();	
  	ES=1;						//打开接收中断
	EA=1;	
	ET0=1;
	TR0=1;	
}

/********************显示函数****************************/
void Dis_play()
{
 uchar j;
  if(display_flag)
	{
	  	if(s_room==1)
		{
		  write(0,0x80);
		  print("    操场      ");
		  write(0,0x90);
		  print("温度： ");
		  for(j=0;j<4;j++)
		  write(1,RX_Buffer5[j]);
		  write(0,0x88);
		  print("湿度： ");	  
		  for(j=4;j<8;j++)
		  write(1,RX_Buffer5[j]);	 
		  write(0,0x98);
		  print("烟雾："); 
		  write(1,' ');
		  write(1,' ');
		  print("PM：");
		  for(j=3;j<8;j++)
		  write(1,RX_Buffer5[j]);
		}
		if(s_room==2)
		{ 
		  write(0,0x80);
		  print("    房间①    ");
		  write(0,0x90);
		  print("温度： ");
		  for(j=0;j<4;j++)
		  write(1,RX_Buffer1[j]);
		  write(0,0x88);	  
		  print("湿度： ");
		  for(j=4;j<8;j++)
		  write(1,RX_Buffer1[j]);
		  write(0,0x98);
		  print("烟雾：");	  
		 }
		 if(s_room==3)
		{ 
		  write(0,0x80);
		  print("    房间②    ");
		  write(0,0x90);
		  print("温度： ");
		  for(j=0;j<4;j++)
		  write(1,RX_Buffer2[j]);
		  write(0,0x88);	  
		  print("湿度： ");
		  for(j=4;j<8;j++)
		  write(1,RX_Buffer2[j]);
		  write(0,0x98);
		  print("烟雾：");
		  write(1,YW2[5]);	  
		 }  
		 if(s_room==4)
		{ 
		  write(0,0x80);
		  print("    房间③    ");
		  write(0,0x90);
		  print("温度： ");
		  for(j=0;j<4;j++)
		  write(1,RX_Buffer3[j]);
		  write(0,0x88);	  
		  print("湿度： ");
		  for(j=4;j<8;j++)
		  write(1,RX_Buffer3[j]);
		  write(0,0x98);
		  print("烟雾：");
		  write(1,YW3[5]);	  
		 }   
	 }
}
void KeyBoard()
{
 if(k1==0)//跳到下一个房间
 {
   s_room++;
   if(s_room>4)s_room=2;
   while(!k1);
 }
// if(k4==0)	 //跳到操场
// {
//   s_room=1;
//    while(!k4);
// }
}


/************************定时器程序**************************************/

void  time0() interrupt 1
{
    TH0=0X3C;	        //50ms中断一次
	TL0=0XB0;
	 time++;
   if(time==100)  //1S刷新一次页面
	 {
	   time=0;
	   oldroom++;
	   s_room++;
	   if(s_room>4)
	   {
	       
	       s_room=1;
	   }
	   if(oldroom>3)
	   {
	     oldroom=1;
	   }
	 }
	 Dis_play();	  //50ms刷新一次
}

/*******************串口中断程序*******************************/
void Usart() interrupt 4
{
uchar m=0,i=0;

TR0=0;
if(RI)
{
     RI=0;	 
     RX1_Buffer[RX1_Cnt] = SBUF;		//保存一个字节
		if(RX1_Buffer[0]==0x2b)	 /*接收0~10十个字节的数据	RX1_Cnt最大32    2b=+	*/
		{
			RX1_Cnt++;
		
		}
		else
		{
			RX1_Cnt=0;
	
		}
  if(RX1_Cnt>=21)
		{
		  display_flag=1;
		   /*+IPD 这些值应该是APP上设置好的！！！！所以这边直接使用）*/
	if(RX1_Buffer[0]==0x2b&&RX1_Buffer[1]==0x49&&RX1_Buffer[2]==0x50&&RX1_Buffer[3]==0x44)
		{
			 /* 是哪个从机发来的信息 */
			 
			if(RX1_Buffer[11]==0x31)//房间1
						{ 
		
						  for(m=12;m<20;m++)
						  {
						     RX_Buffer1[i]=RX1_Buffer[m];
							 i++;
						  }	 
						     i=0;
							 ID1=RX1_Buffer[5];
							 Is_F1=1;	
						}
					
						
			if(RX1_Buffer[11]==0x32)//房间2
						{
						if(RX1_Buffer[12]==0X59&&RX1_Buffer[13]==0X57&&RX1_Buffer[14]==0X43&&RX1_Buffer[15]==0X47)
						 {
						 	for(m=12;m<20;m++)
							  {
							     YW2[i]=RX1_Buffer[m];
								 i++;	YW2_F=1;
							  }
							  i=0;
						 }
						 else
						 {
							  for(m=12;m<20;m++)
							  {
							     RX_Buffer2[i]=RX1_Buffer[m];
								 i++;
							  }
						 } 	
							 ID2=RX1_Buffer[5];
							 Is_F2=1;
						}
					
						
			if(RX1_Buffer[11]==0x33)//房间3
						{
						 if(RX1_Buffer[12]==0X59&&RX1_Buffer[13]==0X57&&RX1_Buffer[14]==0X43&&RX1_Buffer[15]==0X47)
						 {
						 	for(m=12;m<20;m++)
							  {
							     YW3[i]=RX1_Buffer[m];
								 i++;	YW3_F=1;
							  }
							  i=0;
						 }
						 else
						 {
						  for(m=12;m<20;m++)
						  {
						     RX_Buffer3[i]=RX1_Buffer[m];
							 i++;
						  }	
						     i=0;
						  }
							 ID3=RX1_Buffer[5];	
							 Is_F3=1;
						}
			if(RX1_Buffer[11]==0x34)//	手机
						{
//						  for(m=12;m<20;m++)
//						  {
//						     RX_Buffer4[i]=RX1_Buffer[m];
//								   i++;
//		
//						  }
//						  i=0;
							 ID4=RX1_Buffer[5];
							 Is_F4=1;	 
						}
	if(RX1_Buffer[11]==0x35)//操场  外界温湿度
			{
				if(RX1_Buffer[12]==0X50&&RX1_Buffer[13]==0X4D&&RX1_Buffer[14]==0X41)//接收PMA校验码
					{
						for(m=12;m<20;m++)
							{
								 PM[i]= RX1_Buffer[m];
								  i++;
							}
								i=0;
								PM_F=1;
							 }
							 else
							  {	 
							    for(m=12;m<20;m++)
								{
							      RX_Buffer5[i]=RX1_Buffer[m];
								  i++;
								}
								i=0;
							  }	
							 ID5=RX1_Buffer[5];
							 disf=1;
							 send_flag=1;						
					    } 
			 	RX1_Cnt=0;
	 }
	else
	{
     RX1_Cnt=0;
	} 
 }
 }
  TR0=1;  
 }
/***********************主程序*********************************/
void main()
{ 
	SysInit();
	while(1)
 {
  KeyBoard();
	Udp_send();
	if(display_flag==0)
	{
	    write(0,0x80);
		  print("没有接收数据");
		  Delay2(15);
	}
  if(YWZ||YWC||CO)
	{
	  Bee=0;
	}
	 else
	 {
	   Bee=1;
	 }
 }
}

