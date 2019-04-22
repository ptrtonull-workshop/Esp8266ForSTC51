#include<reg52.h>

sbit CS  =  P2^0;	//片选 高电平有效 单片LCD使用时可固定高电平	 rs
sbit SID  = P2^1;	//数据	rw
sbit SCLK = P2^2;	//时钟	 e
sbit RESET = P2^3;	//LCD复位，LCD模块自带复位电路。可不接

void print(unsigned char *s);
void write(bit start, unsigned char ddata);
void sendbyte(unsigned char bbyte);
void delaynms(unsigned int di);
void lcdinit(void);
//void lcdtest(void);


void print(unsigned char *s)
{
  
   while(*s!='\0')
   {
      write(1,*s);
	  s++;
	  delaynms(10);
   }
  

}

void lcdinit(void)	//初始化LCD 
{
  delaynms(10); //启动等待，等LCM讲入工作状态
  	 //串口驱动模式
  RESET=0; delaynms(1); RESET=1; // 复位LCD
  CS=1;
  write(0,0x30);  //8 位介面，基本指令集
  write(0,0x0c);  //显示打开，光标关，反白关
  write(0,0x01);  //清屏，将DDRAM的地址计数器归零  
}

void write(bit start, unsigned char ddata) //写指令或数据
{
  unsigned char start_data,Hdata,Ldata;
  if(start==0) start_data=0xf8;	 //写指令
    else       start_data=0xfa;  //写数据
  
  Hdata=ddata&0xf0;		  //取高四位
  Ldata=(ddata<<4)&0xf0;  //取低四位
  sendbyte(start_data);	  //发送起始信号
  delaynms(5); //延时是必须的
  sendbyte(Hdata);	      //发送高四位
  delaynms(1);  //延时是必须的
  sendbyte(Ldata);		  //发送低四位
  delaynms(1);  //延时是必须的
}

void sendbyte(unsigned char bbyte) //发送一个字节
{
 unsigned char i;
 for(i=0;i<8;i++)
   {
   SID=bbyte&0x80; //取出最高位
   SCLK=1;
   SCLK=0;
   bbyte<<=1; //左移
   }  
}

void delaynms(unsigned int di) //延时
{
 unsigned int da,db;
 for(da=0;da<di;da++)
   for(db=0;db<10;db++);
}



 