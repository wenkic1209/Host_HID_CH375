/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : Key.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-13         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _Key_H
#define _Key_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

void Key_FilterScan(void);
void Key_defaults(void);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

