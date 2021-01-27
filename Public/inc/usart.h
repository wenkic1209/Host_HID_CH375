#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//串口1初始化		   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/8/18
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved
//********************************************************************************
//V1.3修改说明 
//支持适应不同频率下的串口波特率设置.
//加入了对printf的支持
//增加了串口接收命令功能.
//修正了printf第一个字符丢失的bug
//V1.4修改说明
//1,修改串口初始化IO的bug
//2,修改了USART_RX_STA,使得串口最大接收字节数为2的14次方
//3,增加了USART_REC_LEN,用于定义串口最大允许接收的字节数(不大于2的14次方)
//4,修改了EN_USART1_RX的使能方式
//V1.5修改说明
//1,增加了对UCOSII的支持

#define debug 1 //(0关 1开)
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

//==================宏开关============================================
//COM开关
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
//默认: PA9 :Tx  PA10:Rx   (PA:APB2  Usart1:APB2)
//映射: PB6 :Tx  PB7 :Rx   (PA:APB2  Usart1:APB2)
#define Mapping_COM1OFFON  0

#define EVAL_COM1                        USART1
#define EVAL_COM1_CLK                    RCC_APB2Periph_USART1
#define EVAL_COM1_IRQn                   USART1_IRQn

#if (Mapping_COM1OFFON == 0) //默认
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
//默认: PA2 :Tx  PA3 :Rx (PA:APB2  Usart2:APB1)
//映射: PD5 :Tx  PD6 :Rx (PA:APB2  Usart2:APB1)
#define Mapping_COM2OFFON  0

#define EVAL_COM2                        USART2
#define EVAL_COM2_CLK                    RCC_APB1Periph_USART2
#define EVAL_COM2_IRQn                   USART2_IRQn

#if (Mapping_COM2OFFON == 0) //默认
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
//默认: PB10:Tx  PB11 :Rx (PA:APB2  Usart3:APB1) 0
//映射: PC10:Tx  PC11 :Rx (PA:APB2  Usart3:APB1) 1 //部分映射
//映射: PD8 :Tx  PD9  :Rx (PA:APB2  Usart3:APB1) 2 //全部映射
#define Mapping_COM3OFFON  0
/*            默认     部分映射   全部映射
USART3_RX  |  PB11    | PC11    |  PD9
USART3_TX  |  PB12    | PC12    |  PD10
USART3_CTS |      PB13          |  PD11
USART3_RTS |      PB14          |  PD12
*/
#define EVAL_COM3                        USART3
#define EVAL_COM3_CLK                    RCC_APB1Periph_USART3
#define EVAL_COM3_IRQn                   USART3_IRQn

#if  (Mapping_COM3OFFON==0) //默认

#define EVAL_COM3_TX_PIN                 GPIO_Pin_10
#define EVAL_COM3_TX_GPIO_PORT           GPIOB
#define EVAL_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOB
#define EVAL_COM3_RX_PIN                 GPIO_Pin_11
#define EVAL_COM3_RX_GPIO_PORT           GPIOB
#define EVAL_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOB

#elif (Mapping_COM3OFFON==1) //部分映射

#define EVAL_COM3_TX_PIN                 GPIO_Pin_10
#define EVAL_COM3_TX_GPIO_PORT           GPIOC
#define EVAL_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOC
#define EVAL_COM3_RX_PIN                 GPIO_Pin_11
#define EVAL_COM3_RX_GPIO_PORT           GPIOC
#define EVAL_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOC

#elif (Mapping_COM3OFFON==2)// 全部映射

#define EVAL_COM3_TX_PIN                 GPIO_Pin_8
#define EVAL_COM3_TX_GPIO_PORT           GPIOD
#define EVAL_COM3_TX_GPIO_CLK            RCC_APB2Periph_GPIOD
#define EVAL_COM3_RX_PIN                 GPIO_Pin_9
#define EVAL_COM3_RX_GPIO_PORT           GPIOD
#define EVAL_COM3_RX_GPIO_CLK            RCC_APB2Periph_GPIOD

#endif



//=======COM4=======
//默认: PC10:Tx  PC11 :Rx (PA:APB2  Uart4:APB1)
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
//默认: PC12:Tx  PD2 :Rx  (PA:APB2  Uart5:APB1)
#define EVAL_COM5                        UART5                //串口号
#define EVAL_COM5_CLK                    RCC_APB1Periph_UART5 //串口时钟

#define EVAL_COM5_TX_PIN                 GPIO_Pin_12          //发送管脚
#define EVAL_COM5_TX_GPIO_PORT           GPIOC                //发送管脚端
#define EVAL_COM5_TX_GPIO_CLK            RCC_APB2Periph_GPIOC //发送管脚时钟

#define EVAL_COM5_RX_PIN                 GPIO_Pin_2           //接收管脚
#define EVAL_COM5_RX_GPIO_PORT           GPIOD                //接收管脚端
#define EVAL_COM5_RX_GPIO_CLK            RCC_APB2Periph_GPIOD //接收管脚时钟

#define EVAL_COM5_IRQn                   UART5_IRQn           //串口中断号

#else

#define USART_REC_LEN  			200  	//定义最大接收字节数 200
#define EN_USART1_RX 			1		//使能（1）/禁止（0）串口1接收
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);

#endif

#endif

