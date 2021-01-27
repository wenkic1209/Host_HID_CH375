/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : segled.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-11-09         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _segled_H
#define _segled_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void Seg_Init(void);            //数码管初始化
void SegScan_Timer_ms(void);    //HC595刷数码管
void Seg_print(char *fmt, ...); //写入数据


/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

