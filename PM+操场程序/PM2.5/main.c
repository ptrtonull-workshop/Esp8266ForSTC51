
//头文件
#include "STC12c5A.h"
#include "1602.h"
#include "2402.h"

#define uchar unsigned char
#define uint  unsigned int


//定义按键

sbit Key_Up_   = P3^4;
sbit Key_Down  = P3^5;
//定义风扇
sbit fs = P2^0;


//定义蜂鸣器LED报警

sbit DUST_Warn = P3^3;
sbit  LED1 = P2^3;
sbit  LED2 = P2^4;
//定义标识
uchar FlagStart = 0;  
sbit  LED = P3^2;


//全局变量定义，
const char ADC =   0x00;	    //P1^0
	           
uint Counter;
uint DUST_SET;	            //固体颗粒的阈值
float   DUST_Value; 

uchar ADC_Get[10]={0}; 	  //定义AD采样数组
uchar num=0;

//函数声明

void Data_Init();
void Timer0_Init();
void Port_Init();
void ADC_Init();
uchar GetADVal(uchar);
void KeyProcess(void);



//数据初始化
void Data_Init()
{
   Counter = 0;
   FlagStart=0;
   DUST_Value=0;
   DUST_Warn=1;
   LED=0;
   fs=1;
}

//DS---185页  定时器初始化 定时10ms
void Timer0_Init()
{
	
	//AUXR=0x80; //Time work in 1T mode      
	TMOD = 1;      
	TL0 = (65536-30000)/256;     
	TH0 = (65536-30000)%256;    
	TR0 = 1;
	ET0 = 1;   
	EA = 1;     
}

//定时器0中断
void Timer0_ISR (void) interrupt 1 using 0
{
  uint i,j;	
	TL0 = (65536-30000)/256;     
	TH0 = (65536-30000)%256;   
    	LED=1;					 //开启传感器的ＬＥＤ
	
     for (j=0;j<222;j++);  //0.28ms  //延时0.28ms
	
	ADC_Get[num]=GetADVal(ADC);	  //开启ADC采集
	num++;
	if(num>9)
	{
	  FlagStart=1;
	  num=0;
	  TR0 = 0;   //先关闭定时器0
	  EA = 0; 
	}						   //采集10次，关闭定时器0，进行数据处理
//	for (j=0;j<25;j++);
							   //关闭传感器LED
	LED=0;

}

//端口功能设置
void Port_Init()
{
   P1M0 = 0x03;  //DS--88页IO口功能设置，设为开漏11 
   P1M1 = 0x03;  //   
}

void ADC_Init()  //DS-282页
{
   
   P1ASF = 0x03;   //开启通道0,1 
   ADC_RES = 0;              
   ADC_CONTR=ADC_POWER|ADC_SPEEDLL; //每次转换需要420个时钟周期	         
   delay_ms(10);

}


//存入设定值  将设定值存储在24C02中
void Save_Setting(void)
{
   uchar Save;
   Save =DUST_SET/256;
   IIC_Write(0x00, Save);
   Save =DUST_SET%256;
   delay_ms(10);
   IIC_Write(0x01, Save);	     
   
}

//载入设定值  从24C02中读取设定值
void Load_Setting()
{
   
   DUST_SET=0;
   DUST_SET = IIC_Read(0x00);
   DUST_SET*=256;
   delay_ms(10);
   DUST_SET+= IIC_Read(0x01);
   if ((DUST_SET>=760)||(DUST_SET<=0)) DUST_SET =100;
}

//按键处理程序
void KeyProcess(void)
{
   uchar Temp;
   uint i,j;
   Temp=P3;
   Temp&=0xf0;
   if(Temp!=0xf0)
    {
    	 
	 //延时
	 //for (i=0;i<500;i++)
      //for (j=0;j<254;j++);
	 Temp=P3;
      Temp&=0xf0;
    	 if(Temp!=0xf0)
	 {
	    switch(Temp)
	   {
	   case 0xE0:  DUST_SET++;   break;		  //	 设定值+
	   case 0XD0:  DUST_SET--;   break;		  //	 设定值-
	   default: break;
	
	   }
	   //值限定
	  
	   
	    if (DUST_SET>=760)
	        DUST_SET =760;
	    if (DUST_SET<=1)
	        DUST_SET =0;
	   Save_Setting();

	 }
	 
   }
}
void main()
{
   uchar read; 
   uint DUST;
   EA = 0;          //总中断关闭	     
   Data_Init();     //数据初始化
   Port_Init();     //端口初始化
   IIC_Init();

   LCD_Init(); 	//液晶初始化
   LCD_Clear();//清屏 
   LCD_Write_String(4,0,"Welcome");
   LCD_Write_String(0,1,"PM2.5  Detector");
   delay_ms(2000);
  
   LCD_Clear();//清屏
   delay_ms(10); 
   LCD_Write_String(0,0,"PM2.5:     mg/m3");
   LCD_Write_String(0,1,"Alarm:     mg/m3");
   
   //载入设定值
   Load_Setting();
   
   ADC_Init();        //ADC初始化
   delay_ms(10);
   Timer0_Init();   //定时器0初始化
   num=0;	
   DUST=0;
   FlagStart=0;
   
   
   while(1)
   {
   	 if(FlagStart==1)       //10次采集完成
	 {
	  //////////////////////////////
	  DUST=Error_Correct(ADC_Get,10);		//求取10次AD采样的值
	  DUST_Value=(DUST/256.0)*5000;		//转化成电压值MV
	  DUST_Value=DUST_Value*0.172-99.9; //DUST_Value=((DUST)*38)/4-202;//	//固体悬浮颗粒浓度计算 Y=0.172*X-0.0999      X--采样电压V
	  if(DUST_Value<0)	      DUST_Value=0;
	  if(DUST_Value>760)	 DUST_Value=760;	//限位
	  DUST=(uint)DUST_Value;					 //
	  
	  LCD_Show4(6,0,DUST);
	  LCD_Show4(6,1,DUST_SET);			     //显示结果

	  if(DUST>DUST_SET)
	  { 
	  	DUST_Warn=0;
		fs=0;
		LED1=1;
		LED2=0;  
		}      //超过阈值，报警
	  else
	 { DUST_Warn=1;
	 	fs=1;
	  	LED1=0;
	  	LED2=1;
		}

	  
	  KeyProcess();
	  delay_ms(100);
	  TL0 = (65536-30000)/256;     
	  TH0 = (65536-30000)%256; 
	  TR0 = 1;   //开启定时器0
	  EA = 1;
	  FlagStart=0;
	 }
	 
	 
   }
   
}