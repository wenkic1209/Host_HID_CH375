/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : tm_SPI.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-10-21         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _tm_stm32_SPI_H
#define _tm_stm32_SPI_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

//=============================================
//*********************************************
//            模版
//*********************************************
//=============================================
//管脚元素
typedef struct _GPIO
{
   GPIO_TypeDef * GPIO;
   uint16_t       Pin ;  
}TM_GPIO;

//SPI管脚
typedef struct _SPI_Handle
{
   TM_GPIO CS  ;
   TM_GPIO CLK ;
   TM_GPIO MOSI;
   TM_GPIO MISO;
}TM_SPI_TypeDef;

void TM_SPI_InitPins(TM_SPI_TypeDef SPI);        //初始SPI管脚
u8  TM_SPI_RW(TM_SPI_TypeDef SPI,u8 Wdat);       //SPI读写
void TM_SPI_SendMulti(TM_SPI_TypeDef SPI, uint8_t* dataOut, uint8_t* dataIn, uint32_t count) ;
void TM_SPI_WriteMulti(TM_SPI_TypeDef SPI, uint8_t* dataOut, uint32_t count)                 ;
void TM_SPI_ReadMulti(TM_SPI_TypeDef SPI, uint8_t* dataIn, uint8_t dummy, uint32_t count)    ;

//=============================================
//*********************************************
//            应用
//*********************************************
//=============================================

extern TM_SPI_TypeDef NRF24L;




/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

