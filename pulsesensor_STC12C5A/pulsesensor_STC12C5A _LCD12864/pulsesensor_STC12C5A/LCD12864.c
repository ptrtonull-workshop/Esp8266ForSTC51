#include <STC12C5A60S2.h>
#include "stdio.h"
#include <LCD12864.h>

sbit LCD12864_RS  =  P1^5;	 //RS��������
sbit LCD12864_RW  =  P1^6;	 //RW��������
sbit LCD12864_EN  =  P1^7;	 //EN��������
sbit LCD12864_PSB  =  P1^3;	  //ģʽѡ�����ţ�ST7920��������1Ϊ8λ���нӿڣ�0Ϊ���нӿ�
#define LCDPORT P0		//��������

/******************************************************************************
�������ƣ�LCD12864_WriteInfomation
�������ܣ���LCD12864д�������������
��ڲ�����ucData-Ҫд��Һ�������ݻ������������
		  bComOrData-����������ݵı�־λѡ��0����1������
		  	1��д���������
			0��д���������
����ֵ����
��ע����
*******************************************************************************/
void LCD12864_WriteInfomation(unsigned char ucData,bit bComOrData)
{
	LCD12864_CheckBusy();	//æ���
	LCD12864_RW = 0;	//����RW
	LCD12864_RS = bComOrData;	//���ݱ�־λ�ж�д��������������
	delay(15);				  //��ʱ���ȴ�����
	LCDPORT = ucData;		  //�������������ݶ˿�
	LCD12864_EN = 1;		  //ʹ���ź�
	delay(15);				  //��ʱ
	LCD12864_EN = 0;		//����ʱ��������
	delay(15);
}

/******************************************************************************
�������ƣ�LCD12864_Init
�������ܣ�LCD12864Һ����ʼ��
��ڲ�������
����ֵ����
��ע����
*******************************************************************************/
void LCD12864_Init(void)
{
	delay(40);//��ʱ
	LCD12864_PSB = 1;	//8λ���ڹ���ģʽ
	delay(15);//��ʱ


	LCD12864_WriteInfomation(0x30,0); //����ָ�
	delay(15);
	LCD12864_WriteInfomation(0x08,0);  //��ʾ����
	delay(15);
	LCD12864_WriteInfomation(0x10,0);	//�������
	delay(15);
	LCD12864_WriteInfomation(0x0c,0);	//�α�����
	delay(15);
	LCD12864_WriteInfomation(0x01,0);  //����
	delay(15);
	LCD12864_WriteInfomation(0x06,0);	//������趨
	delay(15);
		
}

/******************************************************************************
�������ƣ�LCD12864_CheckBusy
�������ܣ�æ���
��ڲ�������
����ֵ����
��ע��ʹ�ñ���i����ʱ������Һ������ѭ����ͣ�͡�
*******************************************************************************/
void LCD12864_CheckBusy(void)
{
	unsigned char i = 250;	//�ֲ�����
	LCD12864_RS = 0;	  //����
	LCD12864_RW = 1;	  //����
	LCD12864_EN = 1;	  //ʹ��
	while((i > 0) && (P0 & 0x80))i--;  //�ж�æ��־λ
	LCD12864_EN = 0;	  //�ͷ�
}

/******************************************************************************
�������ƣ�LCD12864_DisplayOneLine
�������ܣ���ʾһ�к��֣�8�����ֻ���16��Ӣ���ַ���
��ڲ�����position-Ҫ��ʾ���е��׵�ַ����ѡֵ0x80,0x88,0x90,0x98,���У�
			0x80:Һ���ĵ�һ�У�
			0x88:Һ���ĵ����У�
			0x90:Һ���ĵڶ��У�
			0x98:Һ���ĵ����С�
		  p-Ҫ��ʾ�����ݵ��׵�ַ��
����ֵ����
��ע����
*******************************************************************************/
void LCD12864_DisplayOneLine(unsigned char position,unsigned char *p)
{
	 unsigned char i;
	 LCD12864_WriteInfomation(position,0);//д��Ҫ��ʾ���ֵ��е��׵�ַ
	 delay(15);

	 for(i = 0;i<16;i++)		//����ִ��д�����
	 {
	 	 LCD12864_WriteInfomation(*p,1);
		 p++;
	 }
}
/******************************************************************************
�������ƣ�LCD12864_set_pos
�������ܣ��趨��ʾλ��
��ڲ�����X-������Y-����
����ֵ����
��ע����
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
�������ƣ�LCD_disp_list_char
�������ܣ���ָ��λ����ʾ����
��ڲ�����X-������Y-������DData-��ʾ�����ַ
����ֵ����
��ע����
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
    //���һ�к��ֽ�β�ַ�ΪӢ����ĸ����һ���ո�
    if(ListLength%2){
		LCD12864_WriteInfomation(0,1);
    }
    return;
}
/******************************************************************************
�������ƣ�delay
�������ܣ���ʱ����
��ڲ�����uiCount-��ʱ����
����ֵ����
��ע����
*******************************************************************************/
void delay(unsigned int n)
{
	unsigned int i,j;
	for(i=0;i<n;i++)
		for(j=0;j<100;j++);
}