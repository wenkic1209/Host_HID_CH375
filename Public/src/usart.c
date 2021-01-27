#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
//#include "includes.h"					//ucos ʹ��	  
#include "FreeRTOS.h"
#endif

 
#if 1

#define COMn

//===================�����ж��б�=========================
//ѡ�񴮿ں�
USART_TypeDef* COMn_USART[COMn]       = {EVAL_COM1, 			    EVAL_COM2,	 			 EVAL_COM3
#if STM32F10X_HD
                                          , 			    EVAL_COM4,              EVAL_COM5
#endif																					
																					};
//�����жϺ�
const uint16_t COMn_IRQn[COMn]        = {EVAL_COM1_IRQn,  		EVAL_COM2_IRQn,  		EVAL_COM3_IRQn
#if STM32F10X_HD
																				,  		EVAL_COM4_IRQn,  		EVAL_COM5_IRQn
#endif																				
																				};
//��������ʱ��
const uint32_t COMn_USART_CLK[COMn]   = {EVAL_COM1_CLK, 		  EVAL_COM2_CLK, 		 EVAL_COM3_CLK
#if STM32F10X_HD
																				,  		EVAL_COM4_CLK,  		EVAL_COM5_CLK
#endif																				
																				};
//���͹ܽ�ʱ��
const uint32_t COMn_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK,  EVAL_COM2_TX_GPIO_CLK,  EVAL_COM3_TX_GPIO_CLK  
#if STM32F10X_HD
																				,  		EVAL_COM4_TX_GPIO_CLK,  		EVAL_COM5_TX_GPIO_CLK
#endif																				
																				};

//===================���ڹܽ��б�==========================
//���͹ܽŶ˿�
GPIO_TypeDef*  COMn_TX_PORT[COMn]     = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT, EVAL_COM3_TX_GPIO_PORT
#if STM32F10X_HD
																				,  		EVAL_COM4_TX_GPIO_PORT,  		EVAL_COM5_TX_GPIO_PORT
#endif																				
																				};

//���ܹܽŶ˿�
GPIO_TypeDef*  COMn_RX_PORT[COMn]     = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM2_RX_GPIO_PORT, EVAL_COM3_RX_GPIO_PORT
#if STM32F10X_HD
																				,  		EVAL_COM4_RX_GPIO_PORT,  		EVAL_COM5_RX_GPIO_PORT
#endif																				
																				};
//���ܹܽ�ʱ��
const uint32_t COMn_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK,  EVAL_COM2_RX_GPIO_CLK,  EVAL_COM3_RX_GPIO_CLK
#if STM32F10X_HD
																				,  		EVAL_COM4_RX_GPIO_CLK,  		EVAL_COM5_RX_GPIO_CLK
#endif																				
																				};
//���͹ܽ�
const uint16_t COMn_TX_PIN[COMn]      = {EVAL_COM1_TX_PIN,  	  EVAL_COM2_TX_PIN,       EVAL_COM3_TX_PIN
#if STM32F10X_HD
																				,  		EVAL_COM4_TX_PIN,  		EVAL_COM5_TX_PIN
#endif																				
																				};
//���ܹܽ�
const uint16_t COMn_RX_PIN[COMn]      = {EVAL_COM1_RX_PIN,  	  EVAL_COM2_RX_PIN, 	  EVAL_COM3_RX_PIN
#if STM32F10X_HD
																				,  		EVAL_COM4_RX_PIN,  		EVAL_COM4_RX_PIN
#endif																				
																				};

//====================���ڽ������黺���б�===========================

#if 0
//���͹ܽŶ˿�
RingBuffType*  COMn_Buff[COMn] = { &COM1Dat, &COM2Dat, &COM3Dat, &COM4Dat, &COM5Dat};
#endif




static u8 COM_State[5]={0,0,0,0,0};







//==================����===================================================

void Usart_Init(COM_TypeDef COMx, u32 Baud, u8 Priority1, u8 Priority2)
{
//=======COM1=======
//Ĭ��: PA9 :Tx  PA10:Rx (PA:APB2  Usart1:APB2)
//ӳ��: PB6 :Tx  PB7 :Rx (PA:APB2  Usart1:APB2)
//=======COM2=======
//Ĭ��: PA2 :Tx  PA3 :Rx (PA:APB2  Usart2:APB1)
//ӳ��: PD5 :Tx  PD6 :Rx (PA:APB2  Usart2:APB1)
//=======COM3=======
//Ĭ��: PB10:Tx  PB11 :Rx  (PA:APB2  Usart3:APB1)
//ӳ��: PD8 :Tx  PD9  :Rx  (PA:APB2  Usart3:APB1)
//ӳ��: PC10:Tx  PC11 :Rx  (PA:APB2  Usart3:APB1)
//=======COM4=======
//Ĭ��: PC10:Tx  PC11 :Rx  (PA:APB2  Usart4:APB1)
//=======COM5=======
//Ĭ��: PC12:Tx  PD2 :Rx   (PA:APB2  Usart5:APB1)
    GPIO_InitTypeDef GPIO_InitStructure;   //�ܽ�����
    USART_InitTypeDef USART_InitStructure; //��������
    NVIC_InitTypeDef NVIC_InitStructure;   //�ж����ȼ�����

//=====================����ʱ������==========================================================================
    /* ʹ�ܹܽ�ʱ�� */
    RCC_APB2PeriphClockCmd(COMn_TX_PORT_CLK[COMx] | COMn_RX_PORT_CLK[COMx] | RCC_APB2Periph_AFIO, ENABLE);

    COM_State[COMx] = 1; //ʹ�ܴ��ڷ���
    /* ʹ�ܴ�������ʱ�� */
    if(COMx == COM1)
    {
#if (Mapping_COM1OFFON == 1)  //����1���ܽ���ӳ��
        GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
#endif
        RCC_APB2PeriphClockCmd(COMn_USART_CLK[COMx], ENABLE);
    }
    else
    {
        /* ���ڹܽ������ӳ�� */
        if(COMx == COM2)
        {
#if (Mapping_COM2OFFON == 1)  //����2���ܽ���ӳ��
            GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#endif
        }
        else if(COMx == COM3)
        {
#if (Mapping_COM3OFFON == 1)  //����3���ܽ���ӳ��
            GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
#elif (Mapping_COM3OFFON == 2)
            GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
#endif
        }

        RCC_APB1PeriphClockCmd(COMn_USART_CLK[COMx], ENABLE);
    }

//==========���ڹܽ�����========================================================
    /* ���ô��ڷ��ͽ�--->����������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = COMn_TX_PIN[COMx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COMn_TX_PORT[COMx], &GPIO_InitStructure);

    /* ���ô��ڽ��ܽ�--->�������� */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = COMn_RX_PIN[COMx];
    GPIO_Init(COMn_RX_PORT[COMx], &GPIO_InitStructure);
//==========���ڲ�������======================
    USART_InitStructure.USART_BaudRate = Baud;    //���ò�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* ����GPIO_InitStruct��ָ���Ĳ�����ʼ������GPIOx�Ĵ��� */
    USART_Init(COMn_USART[COMx], &USART_InitStructure);

    /* ʹ�ܴ��� */
    USART_Cmd(COMn_USART[COMx], ENABLE);
    /* �������ڽ����ж�*/
    USART_ITConfig(COMn_USART[COMx], USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    /* ������ڷ�����ɱ�־(��ֹ��һ�ֽڶ�ʧ)*/
    USART_ClearFlag(COMn_USART[COMx], USART_FLAG_TC);      //���������ɱ�־

//=======�����ж����ȼ�����==============================================================
    //Usart1 NVIC ����
    if(Priority1 >= 15)Priority1 = 15;

    if(Priority2 >= 15)Priority2 = 15;

    NVIC_InitStructure.NVIC_IRQChannel = COMn_IRQn[COMx];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Priority1 ; //��ռ���ȼ�3 (0----15)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = Priority2;				 //�����ȼ�3   (0----15)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										 //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);																     //����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

//==============���ݷ��� ===========================================
/*
**********************************************************************
*���ܣ�ѡ�񴮿ںŷ��͵��ֽ�
*���(COMx,DAT)
**********************************************************************
*/
void Usart_SendByte(COM_TypeDef COMx, u8  Dat)
{
    if(COM_State[COMx]==0)return;
    USART_SendData(COMn_USART[COMx], (u8)Dat);
    while(USART_GetFlagStatus(COMn_USART[COMx], USART_FLAG_TXE) == RESET);

}

void Usart_SendString(COM_TypeDef COMx, u8 *  Dat,u16 len)
{
     if(COM_State[COMx]==0)return;
	 while(len--)
	 {
	  Usart_SendByte( COMx, *Dat++);
	 }
}

//////////////////////////////////////////////////////////////////
#if 1
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#ifndef __MICROLIB
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
//	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
//    USART1->DR = (u8) ch;  
  Usart_SendByte( COM1, (uint8_t) ch );	
	return ch;
}
#else
int fputc(int ch, FILE *f)
{
//	USART_SendData(USART1, (uint8_t) ch);

//	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
    Usart_SendByte( COM1, (uint8_t) ch );
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}

#endif 
#endif 
/*ʹ��microLib�ķ���*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/

#if (debug == 1)
#include "string.h"
#include "stdarg.h"
#include "stdio.h"
//======LCD���ת�Ӵ�ӡ==========================================

const u8 enumA[] = {
    0,
    1,  //RC522ģ����Ϣ
//    2,  //AS608Ӧ��
//    3,  //ADC���ݶ�ȡ
//    4,  //LED��״̬
//    5,  //��ʪ��ģ��
//    6,  //���Ŀ���	
//	  7,  //FM24L64����ģ��
//	  8,  //��ӡ�����Ϣ	
};
#include "stdlib.h"
void COM_Printf(u8 type, char *fmt, ...)
{

    u8 i;
    u8 StrlenC = 0;
	  u8 *Com_pstr = NULL;
    va_list ap; //���������
    for(i = 0; i < sizeof(enumA); i++) {
        if(type == enumA[i] && type != 0) {
            goto enumA1;
        }
    }
    return ;
enumA1:
	Com_pstr = malloc(200);  //����ռ�
	if(Com_pstr == NULL)return;
    va_start(ap, fmt); //����ʼ
    vsnprintf((char *)Com_pstr,200, fmt, ap); //���ݸ�ʽ��
    va_end(ap);       //�������
    StrlenC = strlen((char *)Com_pstr);
    Usart_SendString(COM2, Com_pstr, StrlenC);
		free(Com_pstr);  //�ͷſռ�
		Com_pstr =NULL;
}
#else
void COM_Printf(u8 type, char *fmt, ...)
{

}
#endif

#if 1
#include "public.h"
void USART1_IRQHandler()
{
	
	if(USART1->SR&(1<<5))
	{
	  u8 dat = USART1->DR;
      que_Write(&Rx1_que,dat);
	}
//	if(USART_GetITStatus(USART1,USART_IT_RXNE) != SET)
//	{
//		USART_ReceiveData(USART1);
//	}
}

//void USART2_IRQHandler()
//{
//	
//	if(USART2->SR&(1<<5))
//	{
//	  u8 dat = USART2->DR;
//      que_Write(&Rx2_que,dat);
//	}
//}
//void USART3_IRQHandler()
//{
//	
//	if(USART3->SR&(1<<5))
//	{
//	  u8 dat = USART3->DR;
//      que_Write(&Rx3_que,dat);
//	}
//}
#endif
#endif	

