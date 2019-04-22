#ifndef LCD1602_H
#define LCD1602_H

void delay(unsigned int uiCount);	//延时函数
void LCD1602_CheckBusy(void);	//液晶忙检测
void LCD1602_WriteInformation(unsigned char ucData,bit bComOrData);	//在液晶上写数据或者写命令，0为命令，1为数据
void LCD1602_Init(void);	//液晶初始化
void LCD1602_MoveToPosition(unsigned char x,unsigned char y);	//液晶的坐标移动到指定位置
void LCD1602_DisplayOneCharOnAddr(unsigned char x,unsigned char y,unsigned char ucData);	//在液晶指定位置显示字符
void LCD1602_DisplayString(unsigned char *ucStr);	//在液晶上显示字符串
#endif  