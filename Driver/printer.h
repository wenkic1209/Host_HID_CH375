/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : printer.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-11         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _printer_H
#define _printer_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f10x.h"
enum {
    WAIT_FALL = 0,
    WAIT_OK,
    WAIT_BUSY
};
void Printer(void);
void Print_OutTimeTiming(void);
 u8 PrintfPlaySingle(void);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

