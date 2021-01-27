  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : tm_stm32_DMA.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-12-28         
  * Description  :    
  * Function List:  
    1. ....
        <version>:       
          <staff>:
           <data>:
    <description>:  
    2. ...

  ******************************************************************************
  */
/********************************End of Head************************************/
#include "..\tm_library\tm_stm32_DMA.h"

DMA_InitTypeDef DMA_InitStructure;
 
u16 DMA1_MEM_LEN;//保存DMA每次数据传送的长度 	    
//DMA1的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_CHx:DMA通道CHx
//cpar :外设地址
//cmar :存储器地址
//dir  :数据方向
//Size :字节大小
//cndtr:数据传输量 
void TM_DMA_Config(DMA_Channel_TypeDef* DMA_CHx,u32 PeriphBaseaddr,u32 MemoryBaseAddr,enum _DataDir dir, enum _DatSize Size, u16 DatSize)
{
 	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
	
    DMA_DeInit(DMA_CHx);   //将DMA的通道1寄存器重设为缺省值
 
	DMA_InitStructure.DMA_PeripheralBaseAddr = PeriphBaseaddr;             //DMA外设基地址 
	DMA_InitStructure.DMA_MemoryBaseAddr     = MemoryBaseAddr;             //DMA内存基地址
	DMA_InitStructure.DMA_DIR                = (dir==1)?DMA_DIR_PeripheralDST:DMA_DIR_PeripheralSRC ;             //数据传输方向，从内存读取发送到外设
	DMA_InitStructure.DMA_BufferSize         = DatSize;                    //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;       //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = (Size==0)?DMA_PeripheralDataSize_Byte:     /* 8位*/ \
                                              ((Size==1)?DMA_PeripheralDataSize_HalfWord: /*16位*/ \
                                                         DMA_PeripheralDataSize_Word);    /*16位*/ //外设数据宽度
    
	DMA_InitStructure.DMA_MemoryDataSize     =  (Size==0)?DMA_MemoryDataSize_Byte:     /* 8位*/ \
                                               ((Size==1)?DMA_MemoryDataSize_HalfWord: /*16位*/ \
                                                          DMA_MemoryDataSize_Word);    /*16位*/    //内存数据宽度
	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;            //工作在正常模式
	DMA_InitStructure.DMA_Priority           = DMA_Priority_Medium;        //DMA通道 x拥有中优先级 
	DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
	DMA_Init(DMA_CHx, &DMA_InitStructure);                       //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器
	  	
} 
//DMAx_Channelx_IRQn :中断号
//PreemptionPriority :抢占优先级
//SubPriority        :从优先级
void TM_DMA_NVIC_Configuration(IRQn_Type DMAx_Channelx_IRQn,u8 PreemptionPriority,u8 SubPriority)
{
  NVIC_InitTypeDef NVIC_InitStructure;     /* Configure one bit for preemption priority */   
  NVIC_InitStructure.NVIC_IRQChannel = DMAx_Channelx_IRQn;     
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = SubPriority       ;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;          
  NVIC_Init(&NVIC_InitStructure);
}
//通道,数量,状态
void TM_DMA_SetCountAndState(DMA_Channel_TypeDef*DMA_CHx,u16 SendSize,FunctionalState NewState)
{     
 	DMA_SetCurrDataCounter(DMA_CHx,SendSize);  //DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, NewState);                //使能USART1 TX DMA1 所指示的通道 
}

//开启一次DMA传输
void TM_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 SendSize)
{ 
	DMA_Cmd(DMA_CHx, DISABLE );              //关闭USART1 TX DMA1 所指示的通道      
 	DMA_SetCurrDataCounter(DMA_CHx,SendSize);//DMA通道的DMA缓存的大小
 	DMA_Cmd(DMA_CHx, ENABLE);                //使能USART1 TX DMA1 所指示的通道 
}
 
/********************************End of File************************************/

