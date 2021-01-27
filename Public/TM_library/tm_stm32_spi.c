  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : tm_SPI.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-10-21         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include ".\tm_library\tm_stm32_spi.h"
#include ".\tm_library\tm_stm32_GPIO.h"
#include "stm32f10x.h"
//=============================================
//*********************************************
//            模版
//*********************************************
//=============================================
////管脚元素
//typedef struct _GPIO
//{
//   GPIO_TypeDef * GPIO;
//   uint16_t       Pin ;  
//}TM_GPIO;
////SPI管脚
//typedef struct _SPI_Handle
//{
//   TM_GPIO CS  ;
//   TM_GPIO CLK ;
//   TM_GPIO MOSI;
//   TM_GPIO MISO;
//}TM_SPI_TypeDef;

//void Delay(unsigned int s)
//{
//    unsigned int i;
//    for(i=0; i<s; i++);
//    for(i=0; i<s; i++);
//}
 


/********** 函数名: Delay ***********
【功能】：
【参数】：
【返回】：
【说明】：
***********  By(Wenkic) 2021-01-14  **/
void Delay(vu32 nCount)
{
  for(; nCount != 0; nCount--);
}

#define SimuSPI_CPOL   0 //SCLK==0 是空闲状态，所以有效状态是SCLK等于高电平
#define SimuSPI_CPHA   1 //数据采样是在第2个边沿，数据发送在第1个边沿
//模版SPI发送数据（无CS）
u8 TM_SPI_RW(TM_SPI_TypeDef SPI,u8 Wdat)
{
  u8 i=0;
  u8 dat = 0;
  for(i=0;i<8;i++)
  {
     #if (SimuSPI_CPHA ==0)
     GPIO_WriteBit(SPI.CLK.GPIO ,SPI.CLK.Pin,SimuSPI_CPOL); 
     {
        //写数据
        if(Wdat&0x80){
            GPIO_SetBits(SPI.MOSI.GPIO,SPI.MOSI.Pin);
        }else{ 
            GPIO_ResetBits(SPI.MOSI.GPIO,SPI.MOSI.Pin);
        }
        Wdat<<=1;      
        //读取数据
        dat <<=1; 
        if(SimuSPI_MOSI){
            dat|=0x01;
        }                
     }
     GPIO_WriteBit(SPI.CLK.GPIO ,SPI.CLK.Pin,!SimuSPI_CPOL);    
     #else
//     GPIO_WriteBit(SPI.CLK.GPIO ,SPI.CLK.Pin,(BitAction)SimuSPI_CPOL); //0
     {  //写数据
        if(Wdat&0x80){
            GPIO_SetBits(SPI.MOSI.GPIO,SPI.MOSI.Pin);
        }else{
            GPIO_ResetBits(SPI.MOSI.GPIO,SPI.MOSI.Pin);
        }
        Wdat<<=1;      
     }  
     GPIO_WriteBit(SPI.CLK.GPIO ,SPI.CLK.Pin,(BitAction)!SimuSPI_CPOL); //1     
     {  //读取数据 
        dat <<=1; 
        Delay(0xFF);
        if(GPIO_ReadInputDataBit(SPI.MISO.GPIO,SPI.MISO.Pin)){
            dat++;
        }
     }  
     GPIO_WriteBit(SPI.CLK.GPIO ,SPI.CLK.Pin,(BitAction)SimuSPI_CPOL); //0
     Delay(0xFF);
     #endif
  }
  return dat;
}
//模版SPI初始化管脚
void TM_SPI_InitPins(TM_SPI_TypeDef SPI)
{


    TM_GPIO_Init(NRF24L.CS.GPIO ,NRF24L.CS.Pin   ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //输出
    TM_GPIO_Init(NRF24L.CLK.GPIO,NRF24L.CLK.Pin  ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //输入   
    TM_GPIO_Init(NRF24L.MOSI.GPIO,NRF24L.MOSI.Pin,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //输出
    TM_GPIO_Init(NRF24L.MISO.GPIO,NRF24L.MISO.Pin,GPIO_Mode_IPU   ,GPIO_Speed_50MHz); //输入
    
    TM_GPIO_SetPinValue(NRF24L.CS.GPIO  ,NRF24L.CS.Pin  ,1);
    TM_GPIO_SetPinValue(NRF24L.CLK.GPIO ,NRF24L.CLK.Pin ,1);
    TM_GPIO_SetPinValue(NRF24L.MOSI.GPIO,NRF24L.MOSI.Pin,1);  
    TM_SPI_RW(SPI,0xFF);
//    GPIO_InitTypeDef GPIO_InitStructure;	
//    /*Configure pins: CS */
//    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Pin   = SPI.CS.Pin ;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SPI.CS.GPIO, &GPIO_InitStructure);
//    /*Configure pins: SCK / MISO   */
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
//    GPIO_InitStructure.GPIO_Pin  = SPI.CLK.Pin ;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SPI.CLK.GPIO, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin  = SPI.MISO.Pin;
//    GPIO_Init(SPI.MISO.GPIO, &GPIO_InitStructure);    
//    
//    /*Configure SPI1 pins: MOSI*/
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
//    GPIO_InitStructure.GPIO_Pin  = SPI.MOSI.Pin ;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//    GPIO_Init(SPI.MOSI.GPIO, &GPIO_InitStructure);
    
    
//    GPIO_SetBits(SPI.CS.GPIO  ,SPI.CS.Pin  ); //初始片选电平
//    GPIO_SetBits(SPI.CLK.GPIO ,SPI.CLK.Pin ); 
//    GPIO_SetBits(SPI.MOSI.GPIO,SPI.MOSI.Pin);      
//    TM_SPI_RW(SPI,0xFF);	                  //启动传输
//    GPIO_WriteBit(SPI.CLK.GPIO ,SPI.CLK.Pin,(BitAction)0); //0
} 
 





//SPI 同时读写数据
void TM_SPI_SendMulti(TM_SPI_TypeDef SPI, uint8_t* dataOut, uint8_t* dataIn, uint32_t count) {
	while (count--) {
        TM_SPI_RW(SPI,*dataOut++);
		*dataIn = TM_SPI_RW(SPI,0xFF);
		dataIn++ ;
	}
}
//SPI 同时写数据
void TM_SPI_WriteMulti(TM_SPI_TypeDef SPI, uint8_t* dataOut, uint32_t count) {
	while (count--) {
         TM_SPI_RW(SPI,*dataOut++);
	}
}
//SPI 同时读数据
void TM_SPI_ReadMulti(TM_SPI_TypeDef SPI, uint8_t* dataIn, uint8_t dummy, uint32_t count) {
	while (count--) {
		*dataIn = TM_SPI_RW(SPI,dummy);
		dataIn++ ;
	}
}

//=============================================
//*********************************************
//            应用
//*********************************************
//=============================================

#define NFRCS    {GPIOA,GPIO_Pin_4} //输出
#define NFRCLK   {GPIOA,GPIO_Pin_5} //输出
#define NFRMOSI  {GPIOA,GPIO_Pin_6} //输出
#define NFRMISO  {GPIOA,GPIO_Pin_7} //输入


TM_SPI_TypeDef NRF24L ={
  .CS   =  NFRCS   ,
  .CLK  =  NFRCLK  ,
  .MOSI =  NFRMOSI ,
  .MISO =  NFRMISO ,
};






/********************************End of File************************************/

