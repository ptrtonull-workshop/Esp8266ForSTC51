#include <STC12C5A60S2.h>
#include "stdio.h"
#include <LCD12864.h>

sbit LCD12864_RS  =  P1^5;	 //RS控制引脚
sbit LCD12864_RW  =  P1^6;	 //RW控制引脚
sbit LCD12864_EN  =  P1^7;	 //EN控制引脚
sbit LCD12864_PSB  =  P1^3;	  //模式选择引脚，ST7920控制器，1为8位并行接口，0为串行接口
#define LCDPORT P0		//数据引脚

/******************************************************************************
函数名称：LCD12864_WriteInfomation
函数功能：向LCD12864写入命令或者数据
入口参数：ucData-要写入液晶的数据或者命令的内容
		  bComOrData-命令或者数据的标志位选择，0或者1，其中
		  	1：写入的是数据
			0：写入的是命令
返回值：无
备注：无
*******************************************************************************/
void LCD12864_WriteInfomation(unsigned char ucData,bit bComOrData)
{
	LCD12864_CheckBusy();	//忙检测
	LCD12864_RW = 0;	//拉低RW
	LCD12864_RS = bComOrData;	//根据标志位判断写入的是命令还是数据
	delay(15);				  //延时，等待操作
	LCDPORT = ucData;		  //将数据送至数据端口
	LCD12864_EN = 1;		  //使能信号
	delay(15);				  //延时
	LCD12864_EN = 0;		//按照时序来操作
	delay(15);
}

/******************************************************************************
函数名称：LCD12864_Init
函数功能：LCD12864液晶初始化
入口参数：无
返回值：无
备注：无
*******************************************************************************/
void LCD12864_Init(void)
{
	delay(40);//延时
	LCD12864_PSB = 1;	//8位并口工作模式
	delay(15);//延时


	LCD12864_WriteInfomation(0x30,0); //基本指令集
	delay(15);
	LCD12864_WriteInfomation(0x08,0);  //显示设置
	delay(15);
	LCD12864_WriteInfomation(0x10,0);	//光标设置
	delay(15);
	LCD12864_WriteInfomation(0x0c,0);	//游标设置
	delay(15);
	LCD12864_WriteInfomation(0x01,0);  //清屏
	delay(15);
	LCD12864_WriteInfomation(0x06,0);	//进入点设定
	delay(15);
		
}

/******************************************************************************
函数名称：LCD12864_CheckBusy
函数功能：忙检测
入口参数：无
返回值：无
备注：使用变量i做计时，避免液晶在死循环处停滞。
*******************************************************************************/
void LCD12864_CheckBusy(void)
{
	unsigned char i = 250;	//局部变量
	LCD12864_RS = 0;	  //拉低
	LCD12864_RW = 1;	  //拉高
	LCD12864_EN = 1;	  //使能
	while((i > 0) && (P0 & 0x80))i--;  //判断忙标志位
	LCD12864_EN = 0;	  //释放
}

/******************************************************************************
函数名称：LCD12864_DisplayOneLine
函数功能：显示一行汉字（8个汉字或者16个英文字符）
入口参数：position-要显示的行的首地址，可选值0x80,0x88,0x90,0x98,其中：
			0x80:液晶的第一行；
			0x88:液晶的第三行；
			0x90:液晶的第二行；
			0x98:液晶的第四行。
		  p-要显示的内容的首地址。
返回值：无
备注：无
*******************************************************************************/
void LCD12864_DisplayOneLine(unsigned char position,unsigned char *p)
{
	 unsigned char i;
	 LCD12864_WriteInfomation(position,0);//写入要显示文字的行的首地址
	 delay(15);

	 for(i = 0;i<16;i++)		//依次执行写入操作
	 {
	 	 LCD12864_WriteInfomation(*p,1);
		 p++;
	 }
}
/******************************************************************************
函数名称：LCD12864_set_pos
函数功能：设定显示位置
入口参数：X-行数，Y-列数
返回值：无
备注：无
*******************************************************************************/
void LCD12864_set_pos(unsigned char X,unsigned char Y)
{
	 unsigned char pos;
	 if(X==1)
		 X=0x80;
	 else if(X==2)
		 X=0x90;
	 else if(X==3)
		 X=0x88;
	 else if(X==4)
		 X=0x98;
	 
	 pos=X+Y;
	 LCD12864_WriteInfomation(pos,0);
}
/*****************************************
函数名称：LCD_disp_list_char
函数功能：在指定位置显示内容
入口参数：X-行数，Y-列数，DData-显示缓冲地址
返回值：无
备注：无
*****************************************/
void LCD_disp_list_char(unsigned char X, unsigned char Y, char *DData)
{
    unsigned char ListLength;
    
    ListLength = 0;
    LCD12864_set_pos(X,Y);
		delay(15);
    while(DData[ListLength]!=0)
	{
		LCD12864_WriteInfomation(DData[ListLength],1);
        ListLength++;
        Y++;
        if(Y==16){
        	Y = 0;
        	X++;
        	LCD12864_set_pos(X,Y);
        }
    }
    //输出一行后发现结尾字符为英文字母，补一个空格
    if(ListLength%2){
		LCD12864_WriteInfomation(0,1);
    }
    return;
}
/******************************************************************************
函数名称：delay
函数功能：延时函数
入口参数：uiCount-延时参数
返回值：无
备注：无
*******************************************************************************/
void delay(unsigned int n)
{
	unsigned int i,j;
	for(i=0;i<n;i++)
		for(j=0;j<100;j++);
}