/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : MasterSPI.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-12-21         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _MasterSPI_H
#define _MasterSPI_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "tm_library.h"
void SPI_MasterCommunication_Init(void);
u16 MasterSPI_SendByte(uint16_t dat);
void MasterSPI_SetSPI_CS(u8 Dat);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

