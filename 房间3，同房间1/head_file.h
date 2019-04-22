#ifndef _head_file_h_
#define _head_file_h_
/***************************
WIFI就接单片机串口

温湿度数据口接P1.0
****************************/
#define uchar unsigned char
#define uint unsigned int

/*WIFI声明函数*/
void Delay2(unsigned long cnt);
void Tranfer(uchar *s);
void set_wifi(void);




/*12864声明函数*/
void print(unsigned char *s);
void write(bit start, unsigned char ddata);
void sendbyte(unsigned char bbyte);
void delaynms(unsigned int di);
void lcdinit(void);
sbit CS  =  P2^0;	//片选 高电平有效 单片LCD使用时可固定高电平	 rs
sbit SID  = P2^1;	//数据	rw
sbit SCLK = P2^2;	//时钟	 e
sbit RESET = P2^3;	//LCD复位，LCD模块自带复位电路。可不接
void Display(uchar,uchar,uchar,uchar,uchar,uchar);
void SendData(uchar,uchar,uchar,uchar,uchar,uchar);
/*DHT声明函数*/
void  Delay(uint j);
void  Delay_10us(void);
void  COM(void);
void  RH(void);
sbit  P2_0  = P1^1 ;//接温、湿度传感器 的单线引脚
static bit RHT_flag;
#endif



