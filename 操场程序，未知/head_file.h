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
void SendData(uint);



/*DHT声明函数*/
void  Delay(uint j);
void  Delay_10us(void);
void  COM(void);
void  RH(void);
sbit  P2_0  = P1^0 ;//接温、湿度传感器 的单线引脚
static bit RHT_flag;
#endif



