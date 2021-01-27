/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : Moter.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-12-11         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _Moter_H
#define _Moter_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void Moter_GPIOInit(void);
//左右电机控制
void LRMoterControl(u8 Switch,u8 dir);
//前后电机控制
void BFMoterControl(u8 Switch,u8 dir);
//上下电机控制
void UDMoterControl(u8 Switch,u8 dir);

void MainGameMoterControl(void) ;


/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

