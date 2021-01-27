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
void LCD_Printf( u8 y, u8 x, char *fmt, ... );             //数据写入缓冲区
void LcdUpDataDisplay(void);                               //缓冲区更显到显示区
void LCD_Display16x32Number2Bit(u8 y,u8 x,u8 num,u8 Over); //显示16x32的数字
void LCD_xLineOver(u8 line);                       //行反白
void LCD_SetPictureOver(u8 y,u8 x,u8 Over,u8 len); //反白位置设置
void LCD_OverUpdate(void);                         //反白数据刷新
void LCD_ClearAllOver(void);                       //全部清除反白位置
void LCD_Display16x16Picture(u8 y,u8 x,u8 Over ,u8 *dat); //16x16单元画图填充


/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

