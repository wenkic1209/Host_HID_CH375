/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : tm_stm32_DMA.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-12-28         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _tm_stm32_DMA_H
#define _tm_stm32_DMA_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f10x.h"

enum _DataDir{
 dir_IN = 0,
 dir_OUT
};
enum _DatSize{
 Dat_BYTE = 0,
 Dat_HALFWORD,
 Dat_WORD
};

//DMA_CHx:DMA通道CHx
//cpar :外设地址
//cmar :存储器地址
//dir  :数据方向
//Size :字节大小
//cndtr:数据传输量 
void TM_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 PeriphBaseaddr,u32 MemoryBaseAddr,enum _DataDir dir, enum _DatSize Size, u16 DatSize);
//DMAx_Channelx_IRQn :中断号
//PreemptionPriority :抢占优先级
//SubPriority        :从优先级
void TM_DMA_NVIC_Configuration(IRQn_Type DMAx_Channelx_IRQn,u8 PreemptionPriority,u8 SubPriority);
//通道,数量,状态
void TM_DMA_SetCountAndState(DMA_Channel_TypeDef*DMA_CHx,u16 SendSize,FunctionalState NewState);
//开启一次DMA传输
void TM_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 SendSize);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

