#ifndef LCD12864_H
#define LCD12864_H

void LCD12864_Init(void);		  //LCD12864初始化函数
void LCD12864_WriteInfomation(unsigned char ucData,bit bComOrData);	   //向LCD12864写入数据，bComOrData为1时写入的是数据，0时写入的是命令
void LCD12864_CheckBusy(void);		//忙检测函数
void LCD12864_DisplayOneLine(unsigned char ucPos,unsigned char *ucStr);	  //向LCD12864写入一行文字
void LCD12864_set_pos(unsigned char X,unsigned char Y);
void LCD_disp_list_char(unsigned char X, unsigned char Y, char *DData);
void delay(unsigned int uiCount);
#endif  