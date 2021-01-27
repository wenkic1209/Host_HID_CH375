#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//����1��ʼ��		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/8/18
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//********************************************************************************
//V1.3�޸�˵�� 
//֧����Ӧ��ͬƵ���µĴ��ڲ���������.
//�����˶�printf��֧��
//�����˴��ڽ��������.
//������printf��һ���ַ���ʧ��bug
//V1.4�޸�˵��
//1,�޸Ĵ��ڳ�ʼ��IO��bug
//2,�޸���USART_RX_STA,ʹ�ô����������ֽ���Ϊ2��14�η�
//3,������USART_REC_LEN,���ڶ��崮�����������յ��ֽ���(������2��14�η�)
//4,�޸���EN_USART1_RX��ʹ�ܷ�ʽ
//V1.5�޸�˵��
//1,�����˶�UCOSII��֧��

#define debug 1 //(0�� 1��)
void COM_Printf(u8 type, char *fmt, ...);
#if 1
typedef enum
{
    COM1 = 0,
    COM2 = 1,
    COM3 = 2,
    COM4 = 3,
    COM5 = 4,
} COM_TypeDef;

int fputc(int ch, FILE *f);
void Usart_Init(COM_TypeDef COMx, u32 Baud, u8 Priority1, u8 Priority2);
void Usart_SendByte(COM_TypeDef COMx, u8  Dat);
void Usart_SendString(COM_TypeDef COMx, u8 *  Dat,u16 len);

extern USART_TypeDef* COMn_USART[];

//==================�꿪��============================================
//COM����
#define COM1_OFFON  0
#if (COM1_OFFON == 1)
extern RingBuffType COM1Dat;
extern  COMnDealType COM1Deal;
#endif
#define COM2_OFFON  0
#if (COM2_OFFON == 1)
extern RingBuffType COM2Dat;
#endif
#define COM3_OFFON  0
#if (COM3_OFFON == 1)
extern RingBuffType COM3Dat;
#endif
#define COM4_OFFON  0
#if (COM4_OFFON == 1)
extern RingBuffType COM4Dat;
#endif
#define COM5_OFFON  0
#if (COM5_OFFON == 1)
extern RingBuffType COM5Dat;
#endif



//=======COM1=======
//Ĭ��: PA9 :Tx  PA10:Rx   (PA:APB2  Usart1:APB2)
//ӳ��: PB6 :Tx  PB7 :Rx   (PA:APB2  Usart1:APB2)
#define Mapping_COM1OFFON  0

#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_IRQn                   USART1_IRQn

#if (Mapping_COM1OFFON == 0) //Ĭ��
#define EVAL_COM1_TX_PIN                 GPIO_Pin_9
#define EVAL_COM1_TX_GPIO_PORT           GPIOA
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM1_RX_PIN                 GPIO_Pin_10
#define EVAL_COM1_RX_GPIO_PORT           GPIOA
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOA
#elif (Mapping_COM1OFFON == 1)
#define EVAL_COM1_TX_PIN                 GPIO_Pin_6
#define EVAL_COM1_TX_GPIO_PORT           GPIOB
#define EVAL_COM1_TX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define EVAL_COM1_RX_PIN                 GPIO_Pin_7
#define EVAL_COM1_RX_GPIO_PORT           GPIOB
#define EVAL_COM1_RX_GPIO_CLK            RCC_APB2Periph_GPIOB

#endif

//=======COM2=======
//Ĭ��: PA2 :Tx  PA3 :Rx (PA:APB2  Usart2:APB1)
//ӳ��: PD5 :Tx  PD6 :Rx (PA:APB2  Usart2:APB1)
#define Mapping_COM2OFFON  0

#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_IRQn                   USART2_IRQn

#if (Mapping_COM2OFFON == 0) //Ĭ��
#define EVAL_COM2_TX_PIN                 GPIO_Pin_2
#define EVAL_COM2_TX_GPIO_PORT           GPIOA
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOA
#define EVAL_COM2_RX_PIN                 GPIO_Pin_3
#define EVAL_COM2_RX_GPIO_PORT           GPIOA
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOA

#elif (Mapping_COM2OFFON == 1)

#define EVAL_COM2_TX_PIN                 GPIO_Pin_5
#define EVAL_COM2_TX_GPIO_PORT           GPIOD
#define EVAL_COM2_TX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM2_RX_PIN                 GPIO_Pin_6
#define EVAL_COM2_RX_GPIO_PORT           GPIOD
#define EVAL_COM2_RX_GPIO_CLK            RCC_APB2Periph_GPIOD

#endif

//=======COM3=======
//Ĭ��: PB10:Tx  PB11 :Rx (PA:APB2  Usart3:APB1) 0
//ӳ��: PC10:Tx  PC11 :Rx (PA:APB2  Usart3:APB1) 1 //����ӳ��
//ӳ��: PD8 :Tx  PD9  :Rx (PA:APB2  Usart3:APB1) 2 //ȫ��ӳ��
#define Mapping_COM3OFFON  0
/*            Ĭ��     ����ӳ��   ȫ��ӳ��
USART3_RX  |  PB11    | PC11    |  PD9
USART3_TX  |  PB12    | PC12    |  PD10
USART3_CTS |      PB13          |  PD11
USART3_RTS |      PB14          |  PD12
*/
#define EVAL_COM3                        USART3
#define EVAL_COM3_CLK                    RCC_APB1Periph_USART3
#define EVAL_COM3_IRQn                   USART3_IRQn

#if  (Mapping_COM3OFFON==0) //Ĭ��

#define EVAL_COM3_TX_PIN                 GPIO_Pin_10
#define EVAL_COM3_TX_GPIO_PORT           GPIOB
#define EVAL_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define EVAL_COM3_RX_PIN                 GPIO_Pin_11
#define EVAL_COM3_RX_GPIO_PORT           GPIOB
#define EVAL_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOB

#elif (Mapping_COM3OFFON==1) //����ӳ��

#define EVAL_COM3_TX_PIN                 GPIO_Pin_10
#define EVAL_COM3_TX_GPIO_PORT           GPIOC
#define EVAL_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM3_RX_PIN                 GPIO_Pin_11
#define EVAL_COM3_RX_GPIO_PORT           GPIOC
#define EVAL_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOC

#elif (Mapping_COM3OFFON==2)// ȫ��ӳ��

#define EVAL_COM3_TX_PIN                 GPIO_Pin_8
#define EVAL_COM3_TX_GPIO_PORT           GPIOD
#define EVAL_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM3_RX_PIN                 GPIO_Pin_9
#define EVAL_COM3_RX_GPIO_PORT           GPIOD
#define EVAL_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOD

#endif



//=======COM4=======
//Ĭ��: PC10:Tx  PC11 :Rx (PA:APB2  Uart4:APB1)
#define EVAL_COM4                        UART4
#define EVAL_COM4_CLK                    RCC_APB1Periph_UART4
#define EVAL_COM4_TX_PIN                 GPIO_Pin_10
#define EVAL_COM4_TX_GPIO_PORT           GPIOC
#define EVAL_COM4_TX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM4_RX_PIN                 GPIO_Pin_11
#define EVAL_COM4_RX_GPIO_PORT           GPIOC
#define EVAL_COM4_RX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM4_IRQn                   UART4_IRQn

//=======COM5=======
//Ĭ��: PC12:Tx  PD2 :Rx  (PA:APB2  Uart5:APB1)
#define EVAL_COM5                        UART5                //���ں�
#define EVAL_COM5_CLK                    RCC_APB1Periph_UART5 //����ʱ��

#define EVAL_COM5_TX_PIN                 GPIO_Pin_12          //���͹ܽ�
#define EVAL_COM5_TX_GPIO_PORT           GPIOC                //���͹ܽŶ�
#define EVAL_COM5_TX_GPIO_CLK            RCC_APB2Periph_GPIOC //���͹ܽ�ʱ��

#define EVAL_COM5_RX_PIN                 GPIO_Pin_2           //���չܽ�
#define EVAL_COM5_RX_GPIO_PORT           GPIOD                //���չܽŶ�
#define EVAL_COM5_RX_GPIO_CLK            RCC_APB2Periph_GPIOD //���չܽ�ʱ��

#define EVAL_COM5_IRQn                   UART5_IRQn           //�����жϺ�

#else

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);

#endif

#endif

