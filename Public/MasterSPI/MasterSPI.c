  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : MasterSPI.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-12-21         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "./MasterSPI/MasterSPI.h"
#include "tm_library.h"
#include "public.h"
//管脚元素
struct MasterGPIO
{
   GPIO_TypeDef * GPIO;
   uint16_t       Pin ;  
};
//SPI管脚
typedef struct MasterSPI
{
   struct MasterGPIO CS  ;
   struct MasterGPIO CLK ;
   struct MasterGPIO MOSI;
   struct MasterGPIO MISO;
}MasterSPI;

static MasterSPI MasterSPI_1={
  .CS   =  {GPIOA,GPIO_Pin_4},
  .CLK  =  {GPIOA,GPIO_Pin_5},
  .MISO =  {GPIOA,GPIO_Pin_6},
  .MOSI =  {GPIOA,GPIO_Pin_7},
};
static u16 SPI1_SendBuff[10]   ={0};
static u16 SPI1_ReviceBuff[10] ={0};
u8  Rev_Flag = 0;
/*
     SPI接收DMA中断
*/
void DMA1_Channel2_IRQHandler(void)
{
//    u16 Cnt = 0; 
    if(DMA_GetITStatus(DMA1_IT_TC2))
    {
        DMA_Cmd(DMA1_Channel2,DISABLE);              //关闭发送中断 
        DMA_ClearITPendingBit(DMA1_IT_TC2);          //清除全部中断标志           
//        Cnt = DMA_GetCurrDataCounter(DMA1_Channel2);
//        que_Write(&Tx1_que,SPI1_ReviceBuff[0]&0xFF);        
//        for(u16 i=0;i<Cnt;i++)
//        {
//            if(SPI1_ReviceBuff[i]&0x0100)
//            {
//               que_Write(&Tx1_que,SPI1_ReviceBuff[i]&0xFF);
//            }
//        }
        if(SPI1_ReviceBuff[0]&0x0100)
        {
           que_Write(&MasterSPI_Rque,SPI1_ReviceBuff[0]&0xFF);             
        }        
        Rev_Flag = 0;        
      
    }
}
/*
SPI发送DMA中断
*/
void DMA1_Channel3_IRQHandler(void)
{
//    u16 Cnt = 0;
    if(DMA_GetITStatus(DMA1_IT_TC3))
    {

//        while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==0);
//        //缓存数据
//        Cnt = DMA_GetCurrDataCounter(DMA1_Channel3);
////        que_Write(&Tx1_que,SPI1_ReviceBuff[0]&0xFF);
//        for(u16 i=0;i<Cnt;i++)
//        {
//            if(SPI1_ReviceBuff[i]&0x0100)
//            {
//               que_Write(&Tx1_que,SPI1_ReviceBuff[i]&0xFF);
//            }
//        }
        Rev_Flag = 0;
        DMA_Cmd(DMA1_Channel3,DISABLE);
        DMA_ClearITPendingBit(DMA1_IT_TC3); //清除全部中断标志        
    }
}



////外部中断管脚初始化========/*管脚信息          ,抢占优先级                , 响应优先级*/
//static void MasterSPI_ExitGPIO_Init(struct MasterSPI* SPI,uint8_t PreemptionPriority, uint8_t SubPriority);

void MasterSPI_GPIO_Init(SPI_TypeDef* SPIx,struct MasterSPI* SPI)
{
    SPI_InitTypeDef  SPI_InitStructure ;

    if(SPIx == SPI1){
    RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);
    }else if(SPIx == SPI2){
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE);
    }else if(SPIx == SPI3){
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI3, ENABLE);    
    }
    
    /**SPI1 GPIO Configuration  
    PA4   ------> SPI1_NSS   推挽式输出
    PA5   ------> SPI1_SCK   推挽式复用功能
    PA7   ------> SPI1_MOSI  推挽式复用功能
    PA6   ------> SPI1_MISO  输入上拉    
    */
    
    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_Out_PP     ,GPIO_Speed_50MHz); //输出 PA4  推挽式输出
    TM_GPIO_Init(SPI->CLK.GPIO ,SPI->CLK.Pin ,GPIO_Mode_AF_PP      ,GPIO_Speed_50MHz); //输出 PA5  推挽式复用功能
    TM_GPIO_Init(SPI->MOSI.GPIO,SPI->MOSI.Pin,GPIO_Mode_AF_PP      ,GPIO_Speed_50MHz); //输入 PA7  推挽式复用功能    
    TM_GPIO_Init(SPI->MISO.GPIO,SPI->MISO.Pin,GPIO_Mode_IPU        ,GPIO_Speed_50MHz); //输出 PA6  输入上拉    

    
    TM_GPIO_SetPinHigh(SPI->CS.GPIO  ,SPI->CS.Pin  );                             //初始片选电平
    TM_GPIO_SetPinHigh(SPI->CLK.GPIO ,SPI->CLK.Pin );                               
    TM_GPIO_SetPinHigh(SPI->MISO.GPIO,SPI->MISO.Pin);                             
    TM_GPIO_SetPinHigh(SPI->MOSI.GPIO,SPI->MOSI.Pin);                              
 
    //SPI初始化
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //设置SPI的数据收发模式
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;                   //设置SPI主从模式SPI_Mode_Slave 
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_16b;                 //设置SPI的数据宽度大小
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low    ;                  //设置串行时钟空闲时保持电平状态
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //设置数据捕获时的时钟边沿
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft    ;                  //指定NSS信号(即CS)由硬件控制，还是软件控制SPI_NSS_Soft
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //定义波特率预分频值
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //指定数据传输从MSB位还是LSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //定义用于CRC值计算的多项式

    SPI_Init(SPIx, &SPI_InitStructure);
    SPI_Cmd(SPIx, ENABLE);                                               //使能SPI设备
   

    TM_DMA_Config(DMA1_Channel3,(u32)&SPI1->DR,(u32)SPI1_SendBuff  ,dir_OUT,Dat_HALFWORD,1);
    TM_DMA_Config(DMA1_Channel2,(u32)&SPI1->DR,(u32)SPI1_ReviceBuff,dir_IN ,Dat_HALFWORD,1);
    DMA_Cmd(DMA1_Channel2,DISABLE);        
    DMA_Cmd(DMA1_Channel3,DISABLE);
    /* Enable SPI1 TX/RX request */
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx , ENABLE); 
    TM_DMA_NVIC_Configuration(DMA1_Channel2_IRQn,3,0); //开启DMA1_2的中断连接
    TM_DMA_NVIC_Configuration(DMA1_Channel3_IRQn,3,0); //开启DMA1_2的中断连接    
    
    DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);      //配置DMA中断打开
    DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);      //配置DMA中断打开      
}

//SPI主机模式初始化
void SPI_MasterCommunication_Init(void)
{
     MasterSPI_GPIO_Init(SPI1,&MasterSPI_1);
}

#include "public.h"
void MasterSPI_SetSPI_CS(u8 Dat)
{
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,Dat);
}
//SPI主机发送一个数据
u16 MasterSPI_SendByte(uint16_t dat)
{

    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);
    #if 0
    
    uint16_t Rdat = 0;    
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);    
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); //等待发送完成  
    SPI_I2S_SendData(SPI1,dat);      //发送数据 空     
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0); //等待接受完成  
    Rdat = SPI_I2S_ReceiveData(SPI1);  
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);    
    if(Rdat&0x0100)                            //判断是否有效
    {
      que_Write(&MasterSPI_Rque,Rdat&0x00FF);  //缓存队列
      return (1); //返回数据有效                 
    }else{
      return (0); //返回数据无效    
    }    
    #else
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);
    SPI1_SendBuff[0] = dat;
    Rev_Flag = 1;
    TM_DMA_SetCountAndState(DMA1_Channel3,1,ENABLE); //DMA发送使能
    while(Rev_Flag);                                 //等待完成
    Rev_Flag = 1;
    TM_DMA_SetCountAndState(DMA1_Channel2,1,ENABLE); //DMA接收使能
    while(Rev_Flag);                                 //等待完成
    
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);  
    return (0); //返回数据无效      
    #endif
} 
/********************************End of File************************************/

