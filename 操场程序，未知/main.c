
//头文件
#include "STC12c5A.h"
#include<head_file.h>

#define FOSC 11059200L		//系统时钟



bit  FlagStart,DUST_Warn;



//全局变量定义，
const char ADC =   0x00;	    //P1^0
	           
uint Counter;
uint DUST_SET;	            //固体颗粒的阈值
float   DUST_Value; 

uchar ADC_Get[10]={0}; 	  //定义AD采样数组
uchar num=0;

//函数声明
void delay_us(uint t);
void delay_ms(uint t);
void Data_Init();
void Timer0_Init();
void Port_Init();
void ADC_Init();
uchar GetADVal(uchar);

void UART_init(void); 


void delay_us(uint t)
{   
 	  uint T;
	  T=t;
	  while(T!=0)
	  T--;
}

void delay_ms(uint t)
{
     
	uint T;
	T=t;
	while(T--)
	{
		
	     
		delay_us(245);
		delay_us(245);
	     delay_us(245);
		delay_us(245);
	}
}


void UART_init(void)
{
   PCON &= 0x7f;  //波特率不倍速
   SCON = 0x50;  //8位数据，可变波特率
   BRT = 0xFD;    //独立波特率产生器初值
   AUXR |= 0x04;  //时钟设置为1T模式
   AUXR |= 0x01;  //选择独立波特率产生器
   AUXR |= 0x10;  //启动波特率产生
}



//进行AD转换，得到当前8位AD值
uchar GetADVal(uchar CH)
{
   
   ADC_CONTR=ADC_POWER|ADC_SPEEDLL|CH|ADC_START;
   //delay_ms(1);        
   while(!(ADC_CONTR&ADC_FLAG)); //等待ADC转换结束
   ADC_CONTR &= ~ADC_FLAG;       //关闭ADC     
   return ADC_RES;
}


//数据初始化
void Data_Init()
{
   Counter = 0;
   FlagStart=0;
   DUST_Value=0;
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
	EA  = 1;     
}

//定时器0中断
void Timer0_ISR (void) interrupt 1 using 0
{
    uint j;	
	TL0 = (65536-30000)/256;     
	TH0 = (65536-30000)%256;   
  
	
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


}

//端口功能设置
void Port_Init()
{
   P1M0 = 0x03;  //DS--88页IO口功能设置，设为开漏11 
   P1M1 = 0x03;  //   
}

void ADC_Init()  //DS-282页
{
   
   P1ASF = 0x01;   //开启通道0,1 
   ADC_RES = 0;              
   ADC_CONTR=ADC_POWER|ADC_SPEEDLL; //每次转换需要420个时钟周期	         
   delay_ms(10);

}

//中值滤波
//算法：先进行排序，然后将数组的中间值作为当前值返回。
uchar Error_Correct(uchar *str,uchar num) 
{
   unsigned char i=0;
   unsigned char j=0;
   uchar Temp=0;
   
   //排序
   for(i=0;i<num-1;i++)
     {
	  for(j=i+1;j<num;j++)
	   {
	     if(str[i]<str[j])
	   	{
		   Temp=str[i];
		   str[i]=str[j];
		   str[j]=Temp;
		
		}
	   
	   }
	}
    //去除误差，取中间值
    return str[num/2];

}
//存入设定值  将设定值存储在24C02中



void main()
{
   uint DUST=0;
   EA = 0;          //总中断关闭
   	     
   Data_Init();     //数据初始化
   set_wifi();
   Port_Init();     //端口初始化
   UART_init();
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

	  delay_ms(100);
	  FlagStart=0;

     }
	    RH();
	  SendData(DUST);    
	 
  }
   
}