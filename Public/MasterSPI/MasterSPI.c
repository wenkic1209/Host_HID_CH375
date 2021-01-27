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
//�ܽ�Ԫ��
struct MasterGPIO
{
   GPIO_TypeDef * GPIO;
   uint16_t       Pin ;  
};
//SPI�ܽ�
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
     SPI����DMA�ж�
*/
void DMA1_Channel2_IRQHandler(void)
{
//    u16 Cnt = 0; 
    if(DMA_GetITStatus(DMA1_IT_TC2))
    {
        DMA_Cmd(DMA1_Channel2,DISABLE);              //�رշ����ж� 
        DMA_ClearITPendingBit(DMA1_IT_TC2);          //���ȫ���жϱ�־           
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
SPI����DMA�ж�
*/
void DMA1_Channel3_IRQHandler(void)
{
//    u16 Cnt = 0;
    if(DMA_GetITStatus(DMA1_IT_TC3))
    {

//        while(SPI_I2S_GetFlagStatus(SPI1,SPI_I2S_FLAG_RXNE)==0);
//        //��������
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
        DMA_ClearITPendingBit(DMA1_IT_TC3); //���ȫ���жϱ�־        
    }
}



////�ⲿ�жϹܽų�ʼ��========/*�ܽ���Ϣ          ,��ռ���ȼ�                , ��Ӧ���ȼ�*/
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
    PA4   ------> SPI1_NSS   ����ʽ���
    PA5   ------> SPI1_SCK   ����ʽ���ù���
    PA7   ------> SPI1_MOSI  ����ʽ���ù���
    PA6   ------> SPI1_MISO  ��������    
    */
    
    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_Out_PP     ,GPIO_Speed_50MHz); //��� PA4  ����ʽ���
    TM_GPIO_Init(SPI->CLK.GPIO ,SPI->CLK.Pin ,GPIO_Mode_AF_PP      ,GPIO_Speed_50MHz); //��� PA5  ����ʽ���ù���
    TM_GPIO_Init(SPI->MOSI.GPIO,SPI->MOSI.Pin,GPIO_Mode_AF_PP      ,GPIO_Speed_50MHz); //���� PA7  ����ʽ���ù���    
    TM_GPIO_Init(SPI->MISO.GPIO,SPI->MISO.Pin,GPIO_Mode_IPU        ,GPIO_Speed_50MHz); //��� PA6  ��������    

    
    TM_GPIO_SetPinHigh(SPI->CS.GPIO  ,SPI->CS.Pin  );                             //��ʼƬѡ��ƽ
    TM_GPIO_SetPinHigh(SPI->CLK.GPIO ,SPI->CLK.Pin );                               
    TM_GPIO_SetPinHigh(SPI->MISO.GPIO,SPI->MISO.Pin);                             
    TM_GPIO_SetPinHigh(SPI->MOSI.GPIO,SPI->MOSI.Pin);                              
 
    //SPI��ʼ��
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //����SPI�������շ�ģʽ
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master;                   //����SPI����ģʽSPI_Mode_Slave 
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_16b;                 //����SPI�����ݿ�ȴ�С
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low    ;                  //���ô���ʱ�ӿ���ʱ���ֵ�ƽ״̬
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //�������ݲ���ʱ��ʱ�ӱ���
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft    ;                  //ָ��NSS�ź�(��CS)��Ӳ�����ƣ������������SPI_NSS_Soft
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //���岨����Ԥ��Ƶֵ
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //ָ�����ݴ����MSBλ����LSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //��������CRCֵ����Ķ���ʽ

    SPI_Init(SPIx, &SPI_InitStructure);
    SPI_Cmd(SPIx, ENABLE);                                               //ʹ��SPI�豸
   

    TM_DMA_Config(DMA1_Channel3,(u32)&SPI1->DR,(u32)SPI1_SendBuff  ,dir_OUT,Dat_HALFWORD,1);
    TM_DMA_Config(DMA1_Channel2,(u32)&SPI1->DR,(u32)SPI1_ReviceBuff,dir_IN ,Dat_HALFWORD,1);
    DMA_Cmd(DMA1_Channel2,DISABLE);        
    DMA_Cmd(DMA1_Channel3,DISABLE);
    /* Enable SPI1 TX/RX request */
    SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Rx|SPI_I2S_DMAReq_Tx , ENABLE); 
    TM_DMA_NVIC_Configuration(DMA1_Channel2_IRQn,3,0); //����DMA1_2���ж�����
    TM_DMA_NVIC_Configuration(DMA1_Channel3_IRQn,3,0); //����DMA1_2���ж�����    
    
    DMA_ITConfig(DMA1_Channel2,DMA_IT_TC,ENABLE);      //����DMA�жϴ�
    DMA_ITConfig(DMA1_Channel3,DMA_IT_TC,ENABLE);      //����DMA�жϴ�      
}

//SPI����ģʽ��ʼ��
void SPI_MasterCommunication_Init(void)
{
     MasterSPI_GPIO_Init(SPI1,&MasterSPI_1);
}

#include "public.h"
void MasterSPI_SetSPI_CS(u8 Dat)
{
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,Dat);
}
//SPI��������һ������
u16 MasterSPI_SendByte(uint16_t dat)
{

    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);
    #if 0
    
    uint16_t Rdat = 0;    
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);    
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); //�ȴ��������  
    SPI_I2S_SendData(SPI1,dat);      //�������� ��     
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0); //�ȴ��������  
    Rdat = SPI_I2S_ReceiveData(SPI1);  
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);    
    if(Rdat&0x0100)                            //�ж��Ƿ���Ч
    {
      que_Write(&MasterSPI_Rque,Rdat&0x00FF);  //�������
      return (1); //����������Ч                 
    }else{
      return (0); //����������Ч    
    }    
    #else
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);
    SPI1_SendBuff[0] = dat;
    Rev_Flag = 1;
    TM_DMA_SetCountAndState(DMA1_Channel3,1,ENABLE); //DMA����ʹ��
    while(Rev_Flag);                                 //�ȴ����
    Rev_Flag = 1;
    TM_DMA_SetCountAndState(DMA1_Channel2,1,ENABLE); //DMA����ʹ��
    while(Rev_Flag);                                 //�ȴ����
    
    TM_GPIO_SetPinValue(MasterSPI_1.CS.GPIO,MasterSPI_1.CS.Pin,0);  
    return (0); //����������Ч      
    #endif
} 
/********************************End of File************************************/

