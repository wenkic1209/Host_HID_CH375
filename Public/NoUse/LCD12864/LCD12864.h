/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : LCD12864.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-11-14         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _LCD12864_H
#define _LCD12864_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

extern const u8  LCD_icon[2][32];

void LCD_Init( void );
void LCD_Clear( void );
void LCD_Printf( u8 y, u8 x, char *fmt, ... );             //����д�뻺����
void LcdUpDataDisplay(void);                               //���������Ե���ʾ��
void LCD_Display16x32Number2Bit(u8 y,u8 x,u8 num,u8 Over); //��ʾ16x32������
void LCD_xLineOver(u8 line);                       //�з���
void LCD_SetPictureOver(u8 y,u8 x,u8 Over,u8 len); //����λ������
void LCD_OverUpdate(void);                         //��������ˢ��
void LCD_ClearAllOver(void);                       //ȫ���������λ��
void LCD_Display16x16Picture(u8 y,u8 x,u8 Over ,u8 *dat); //16x16��Ԫ��ͼ���


/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

