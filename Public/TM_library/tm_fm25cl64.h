/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : tm_stm32_fm25cl64.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-15         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _tm_fm25cl64_H
#define _tm_fm25cl64_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void FM25CL_SPI_Init(void);
u8 fm25cl64_Text(u32 WAddr, u8 Dat);
/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

