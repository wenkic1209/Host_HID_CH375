  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : SalveSPI.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-12-21         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "./SalveSPI/SalveSPI.h"
#include "Public.h"
//管脚元素
struct SalveGPIO
{
   GPIO_TypeDef * GPIO;
   uint16_t       Pin ;  
};
//SPI管脚
typedef struct SalveSPI
{
   struct SalveGPIO CS  ;
   struct SalveGPIO CLK ;
   struct SalveGPIO MOSI;
   struct SalveGPIO MISO;
}SalveSPI;
SalveSPI SalveSPI_1={
  .CS   =  {GPIOA,GPIO_Pin_4},
  .CLK  =  {GPIOA,GPIO_Pin_5},
  .MISO =  {GPIOA,GPIO_Pin_7},
  .MOSI =  {GPIOA,GPIO_Pin_6},
};
////外部中断管脚初始化========/*管脚信息          ,抢占优先级                , 响应优先级*/
//static void SalveSPI_ExitGPIO_Init(struct SalveSPI* SPI,uint8_t PreemptionPriority, uint8_t SubPriority);

void SalveSPI_GPIO_Init(SPI_TypeDef* SPIx,struct SalveSPI* SPI,uint8_t PreemptionPriority, uint8_t SubPriority)
{
    SPI_InitTypeDef  SPI_InitStructure ;
    NVIC_InitTypeDef NVIC_InitStructure;
    
//    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    if(SPIx == SPI1){
        RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1, ENABLE);
    }else if(SPIx == SPI2){
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE);
    }
    #if (defined(STM32F10X_CL))
    else if(SPIx == SPI3){
        RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI3, ENABLE);    
    }
    #endif
    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_IN_FLOATING ,GPIO_Speed_50MHz); //输出
    TM_GPIO_Init(SPI->CLK.GPIO ,SPI->CLK.Pin ,GPIO_Mode_IN_FLOATING ,GPIO_Speed_50MHz); //输出 
    TM_GPIO_Init(SPI->MISO.GPIO,SPI->MISO.Pin,GPIO_Mode_AF_PP       ,GPIO_Speed_50MHz); //输出    
    TM_GPIO_Init(SPI->MOSI.GPIO,SPI->MOSI.Pin,GPIO_Mode_IN_FLOATING ,GPIO_Speed_50MHz); //输入
    
//    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //输出
//    TM_GPIO_Init(SPI->CLK.GPIO ,SPI->CLK.Pin ,GPIO_Mode_AF_PP ,GPIO_Speed_50MHz); //输出 
//    TM_GPIO_Init(SPI->MISO.GPIO,SPI->MISO.Pin,GPIO_Mode_AF_PP ,GPIO_Speed_50MHz); //输出    
//    TM_GPIO_Init(SPI->MOSI.GPIO,SPI->MOSI.Pin,GPIO_Mode_IPU   ,GPIO_Speed_50MHz); //输入
    TM_GPIO_SetPinHigh(SPI->CS.GPIO  ,SPI->CS.Pin  );                             //初始片选电平
    TM_GPIO_SetPinHigh(SPI->CLK.GPIO ,SPI->CLK.Pin );                               
    TM_GPIO_SetPinHigh(SPI->MISO.GPIO,SPI->MISO.Pin);                             
    TM_GPIO_SetPinHigh(SPI->MOSI.GPIO,SPI->MOSI.Pin);                              
    
//    SalveSPI_ExitGPIO_Init(SPI,0,1);                  //片选管脚设置为中断
    
    

    
    if(SPIx == SPI1){
        NVIC_InitStructure.NVIC_IRQChannel = SPI1_IRQn;
    }else if(SPIx == SPI2){
        NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
    }
    #if (defined(STM32F10X_CL))
    else if(SPIx == SPI3){
        NVIC_InitStructure.NVIC_IRQChannel = SPI3_IRQn;   
    }
    #endif
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority  ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = SubPriority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    
    //SPI初始化
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //设置SPI的数据收发模式
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Slave  ;                  //设置SPI主从模式 SPI_Mode_Master
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b ;                  //设置SPI的数据宽度大小
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low    ;                  //设置串行时钟空闲时保持电平状态
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //设置数据捕获时的时钟边沿
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Hard    ;                  //指定NSS信号(即CS)由硬件控制，还是软件控制SPI_NSS_Soft
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //定义波特率预分频值
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //指定数据传输从MSB位还是LSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //定义用于CRC值计算的多项式

    SPI_Init(SPIx, &SPI_InitStructure);
	//开启接收中断
//	SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
    SPI_Cmd(SPIx, ENABLE);  //使能SPI设备
}

//const uint8_t EXIT_Channel[16]={EXTI0_IRQn    ,EXTI1_IRQn    ,EXTI2_IRQn    ,EXTI3_IRQn     ,EXTI4_IRQn    ,  
//                                EXTI9_5_IRQn  , EXTI9_5_IRQn , EXTI9_5_IRQn , EXTI9_5_IRQn  , EXTI9_5_IRQn ,  
//                                EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,   
//                                EXTI15_10_IRQn };

////外部中断管脚初始化========/*管脚信息          ,抢占优先级                , 响应优先级*/
//static void SalveSPI_ExitGPIO_Init(struct SalveSPI* SPI,uint8_t PreemptionPriority, uint8_t SubPriority)
//{
//    EXTI_InitTypeDef EXTI_InitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;
//    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_IPU,GPIO_Speed_50MHz); //输出
//    //外部中断 配置
//    EXTI_InitStructure.EXTI_Line    = SPI->CS.Pin  ;       //外部中断线
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //中断状态
//    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt; //选择中断
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
//    EXTI_Init(&EXTI_InitStructure);
//	//中断优先级配置
//	NVIC_InitStructure.NVIC_IRQChannel    = EXIT_Channel[TM_GPIO_GetPinSource(SPI->CS.Pin  )];//通过管脚查询中断号列表
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority  ;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = SubPriority;
//	NVIC_Init(&NVIC_InitStructure);
//	//把相应IO连接到外部中断线
//    uint8_t GPIO_Port = TM_GPIO_GetPortSource(SPI->CS.GPIO  ); //获取到端口位置
//	GPIO_EXTILineConfig(GPIO_Port,GPIO_PinSource12);
//}

//void EXTI1_IRQHandler(void)
//{
//   if(EXTI_GetITStatus(EXTI_Line1) != RESET)
//	 {
//	    EXTI_ClearITPendingBit(EXTI_Line1); // 清除中断
//        while(TM_GPIO_GetInputPinValue(SalveSPI_1.CS.GPIO  ,SalveSPI_1.CS.Pin  )==0)
//        {
//        
//        
//        }
//	 }
//}

#define SALVESPIx  SPI1
void SPI_SalveCommunication_Init(void)
{
     SalveSPI_GPIO_Init(SPI1,&SalveSPI_1,0,3);
}

//    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); //等待发送完成
//    SPI_I2S_SendData(SPI1, WRDat); //发送数据
//    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0); //等待接受完成
//    return (SPI_I2S_ReceiveData(SPI1)); //返回数据


//SPI中断服务函数
void SPI1_IRQHandler(void)
{
	uint16_t data;
	//接收中断
	if(SPI_I2S_GetITStatus(SALVESPIx, SPI_I2S_IT_RXNE))
	{
		//接收主机发来的数据
		data = SPI_I2S_ReceiveData(SALVESPIx);
		//SPI_I2S_SendData(SPIX,data);
		SPI1->DR = data;
        que_Write(&SalveSPI_Rque,data);
		//不用清除中断标志位，读取数据时就清过了
		//而且没有办法用软件清除RXNE标志，不信去看手册
	}
}
 
/********************************End of File************************************/

