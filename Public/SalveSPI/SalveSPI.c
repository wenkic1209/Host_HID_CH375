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
//�ܽ�Ԫ��
struct SalveGPIO
{
   GPIO_TypeDef * GPIO;
   uint16_t       Pin ;  
};
//SPI�ܽ�
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
////�ⲿ�жϹܽų�ʼ��========/*�ܽ���Ϣ          ,��ռ���ȼ�                , ��Ӧ���ȼ�*/
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
    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_IN_FLOATING ,GPIO_Speed_50MHz); //���
    TM_GPIO_Init(SPI->CLK.GPIO ,SPI->CLK.Pin ,GPIO_Mode_IN_FLOATING ,GPIO_Speed_50MHz); //��� 
    TM_GPIO_Init(SPI->MISO.GPIO,SPI->MISO.Pin,GPIO_Mode_AF_PP       ,GPIO_Speed_50MHz); //���    
    TM_GPIO_Init(SPI->MOSI.GPIO,SPI->MOSI.Pin,GPIO_Mode_IN_FLOATING ,GPIO_Speed_50MHz); //����
    
//    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //���
//    TM_GPIO_Init(SPI->CLK.GPIO ,SPI->CLK.Pin ,GPIO_Mode_AF_PP ,GPIO_Speed_50MHz); //��� 
//    TM_GPIO_Init(SPI->MISO.GPIO,SPI->MISO.Pin,GPIO_Mode_AF_PP ,GPIO_Speed_50MHz); //���    
//    TM_GPIO_Init(SPI->MOSI.GPIO,SPI->MOSI.Pin,GPIO_Mode_IPU   ,GPIO_Speed_50MHz); //����
    TM_GPIO_SetPinHigh(SPI->CS.GPIO  ,SPI->CS.Pin  );                             //��ʼƬѡ��ƽ
    TM_GPIO_SetPinHigh(SPI->CLK.GPIO ,SPI->CLK.Pin );                               
    TM_GPIO_SetPinHigh(SPI->MISO.GPIO,SPI->MISO.Pin);                             
    TM_GPIO_SetPinHigh(SPI->MOSI.GPIO,SPI->MOSI.Pin);                              
    
//    SalveSPI_ExitGPIO_Init(SPI,0,1);                  //Ƭѡ�ܽ�����Ϊ�ж�
    
    

    
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
    
    
    //SPI��ʼ��
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //����SPI�������շ�ģʽ
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Slave  ;                  //����SPI����ģʽ SPI_Mode_Master
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b ;                  //����SPI�����ݿ�ȴ�С
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low    ;                  //���ô���ʱ�ӿ���ʱ���ֵ�ƽ״̬
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //�������ݲ���ʱ��ʱ�ӱ���
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Hard    ;                  //ָ��NSS�ź�(��CS)��Ӳ�����ƣ������������SPI_NSS_Soft
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //���岨����Ԥ��Ƶֵ
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //ָ�����ݴ����MSBλ����LSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //��������CRCֵ����Ķ���ʽ

    SPI_Init(SPIx, &SPI_InitStructure);
	//���������ж�
//	SPI_I2S_ITConfig(SPIx, SPI_I2S_IT_RXNE, ENABLE);
    SPI_Cmd(SPIx, ENABLE);  //ʹ��SPI�豸
}

//const uint8_t EXIT_Channel[16]={EXTI0_IRQn    ,EXTI1_IRQn    ,EXTI2_IRQn    ,EXTI3_IRQn     ,EXTI4_IRQn    ,  
//                                EXTI9_5_IRQn  , EXTI9_5_IRQn , EXTI9_5_IRQn , EXTI9_5_IRQn  , EXTI9_5_IRQn ,  
//                                EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,   
//                                EXTI15_10_IRQn };

////�ⲿ�жϹܽų�ʼ��========/*�ܽ���Ϣ          ,��ռ���ȼ�                , ��Ӧ���ȼ�*/
//static void SalveSPI_ExitGPIO_Init(struct SalveSPI* SPI,uint8_t PreemptionPriority, uint8_t SubPriority)
//{
//    EXTI_InitTypeDef EXTI_InitStructure;
//    NVIC_InitTypeDef NVIC_InitStructure;
//    TM_GPIO_Init(SPI->CS.GPIO  ,SPI->CS.Pin  ,GPIO_Mode_IPU,GPIO_Speed_50MHz); //���
//    //�ⲿ�ж� ����
//    EXTI_InitStructure.EXTI_Line    = SPI->CS.Pin  ;       //�ⲿ�ж���
//    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //�ж�״̬
//    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt; //ѡ���ж�
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//�½��ش���
//    EXTI_Init(&EXTI_InitStructure);
//	//�ж����ȼ�����
//	NVIC_InitStructure.NVIC_IRQChannel    = EXIT_Channel[TM_GPIO_GetPinSource(SPI->CS.Pin  )];//ͨ���ܽŲ�ѯ�жϺ��б�
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority  ;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = SubPriority;
//	NVIC_Init(&NVIC_InitStructure);
//	//����ӦIO���ӵ��ⲿ�ж���
//    uint8_t GPIO_Port = TM_GPIO_GetPortSource(SPI->CS.GPIO  ); //��ȡ���˿�λ��
//	GPIO_EXTILineConfig(GPIO_Port,GPIO_PinSource12);
//}

//void EXTI1_IRQHandler(void)
//{
//   if(EXTI_GetITStatus(EXTI_Line1) != RESET)
//	 {
//	    EXTI_ClearITPendingBit(EXTI_Line1); // ����ж�
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

//    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); //�ȴ��������
//    SPI_I2S_SendData(SPI1, WRDat); //��������
//    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0); //�ȴ��������
//    return (SPI_I2S_ReceiveData(SPI1)); //��������


//SPI�жϷ�����
void SPI1_IRQHandler(void)
{
	uint16_t data;
	//�����ж�
	if(SPI_I2S_GetITStatus(SALVESPIx, SPI_I2S_IT_RXNE))
	{
		//������������������
		data = SPI_I2S_ReceiveData(SALVESPIx);
		//SPI_I2S_SendData(SPIX,data);
		SPI1->DR = data;
        que_Write(&SalveSPI_Rque,data);
		//��������жϱ�־λ����ȡ����ʱ�������
		//����û�а취��������RXNE��־������ȥ���ֲ�
	}
}
 
/********************************End of File************************************/

