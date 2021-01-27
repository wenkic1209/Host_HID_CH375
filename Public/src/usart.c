#include "sys.h"
#include "usart.h"	  
////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
//#include "includes.h"					//ucos 使用	  
#include "FreeRTOS.h"
#endif

 
#if 1

#define COMn

//===================串口中断列表=========================
//选择串口号
USART_TypeDef* COMn_USART[COMn]       = {EVAL_COM1, 			    EVAL_COM2,	 			 EVAL_COM3
#if STM32F10X_HD
                                          , 			    EVAL_COM4,              EVAL_COM5
#endif																					
																					};
//串口中断号
const uint16_t COMn_IRQn[COMn]        = {EVAL_COM1_IRQn,  		EVAL_COM2_IRQn,  		EVAL_COM3_IRQn
#if STM32F10X_HD
																				,  		EVAL_COM4_IRQn,  		EVAL_COM5_IRQn
#endif																				
																				};
//串口外设时钟
const uint32_t COMn_USART_CLK[COMn]   = {EVAL_COM1_CLK, 		  EVAL_COM2_CLK, 		 EVAL_COM3_CLK
#if STM32F10X_HD
																				,  		EVAL_COM4_CLK,  		EVAL_COM5_CLK
#endif																				
																				};
//发送管脚时钟
const uint32_t COMn_TX_PORT_CLK[COMn] = {EVAL_COM1_TX_GPIO_CLK,  EVAL_COM2_TX_GPIO_CLK,  EVAL_COM3_TX_GPIO_CLK  
#if STM32F10X_HD
																				,  		EVAL_COM4_TX_GPIO_CLK,  		EVAL_COM5_TX_GPIO_CLK
#endif																				
																				};

//===================串口管脚列表==========================
//发送管脚端口
GPIO_TypeDef*  COMn_TX_PORT[COMn]     = {EVAL_COM1_TX_GPIO_PORT, EVAL_COM2_TX_GPIO_PORT, EVAL_COM3_TX_GPIO_PORT
#if STM32F10X_HD
																				,  		EVAL_COM4_TX_GPIO_PORT,  		EVAL_COM5_TX_GPIO_PORT
#endif																				
																				};

//接受管脚端口
GPIO_TypeDef*  COMn_RX_PORT[COMn]     = {EVAL_COM1_RX_GPIO_PORT, EVAL_COM2_RX_GPIO_PORT, EVAL_COM3_RX_GPIO_PORT
#if STM32F10X_HD
																				,  		EVAL_COM4_RX_GPIO_PORT,  		EVAL_COM5_RX_GPIO_PORT
#endif																				
																				};
//接受管脚时钟
const uint32_t COMn_RX_PORT_CLK[COMn] = {EVAL_COM1_RX_GPIO_CLK,  EVAL_COM2_RX_GPIO_CLK,  EVAL_COM3_RX_GPIO_CLK
#if STM32F10X_HD
																				,  		EVAL_COM4_RX_GPIO_CLK,  		EVAL_COM5_RX_GPIO_CLK
#endif																				
																				};
//发送管脚
const uint16_t COMn_TX_PIN[COMn]      = {EVAL_COM1_TX_PIN,  	  EVAL_COM2_TX_PIN,       EVAL_COM3_TX_PIN
#if STM32F10X_HD
																				,  		EVAL_COM4_TX_PIN,  		EVAL_COM5_TX_PIN
#endif																				
																				};
//接受管脚
const uint16_t COMn_RX_PIN[COMn]      = {EVAL_COM1_RX_PIN,  	  EVAL_COM2_RX_PIN, 	  EVAL_COM3_RX_PIN
#if STM32F10X_HD
																				,  		EVAL_COM4_RX_PIN,  		EVAL_COM4_RX_PIN
#endif																				
																				};

//====================串口接受数组缓存列表===========================

#if 0
//发送管脚端口
RingBuffType*  COMn_Buff[COMn] = { &COM1Dat, &COM2Dat, &COM3Dat, &COM4Dat, &COM5Dat};
#endif




static u8 COM_State[5]={0,0,0,0,0};







//==================函数===================================================

void Usart_Init(COM_TypeDef COMx, u32 Baud, u8 Priority1, u8 Priority2)
{
//=======COM1=======
//默认: PA9 :Tx  PA10:Rx (PA:APB2  Usart1:APB2)
//映射: PB6 :Tx  PB7 :Rx (PA:APB2  Usart1:APB2)
//=======COM2=======
//默认: PA2 :Tx  PA3 :Rx (PA:APB2  Usart2:APB1)
//映射: PD5 :Tx  PD6 :Rx (PA:APB2  Usart2:APB1)
//=======COM3=======
//默认: PB10:Tx  PB11 :Rx  (PA:APB2  Usart3:APB1)
//映射: PD8 :Tx  PD9  :Rx  (PA:APB2  Usart3:APB1)
//映射: PC10:Tx  PC11 :Rx  (PA:APB2  Usart3:APB1)
//=======COM4=======
//默认: PC10:Tx  PC11 :Rx  (PA:APB2  Usart4:APB1)
//=======COM5=======
//默认: PC12:Tx  PD2 :Rx   (PA:APB2  Usart5:APB1)
    GPIO_InitTypeDef GPIO_InitStructure;   //管脚配置
    USART_InitTypeDef USART_InitStructure; //串口配置
    NVIC_InitTypeDef NVIC_InitStructure;   //中断优先级配置

//=====================外设时钟配置==========================================================================
    /* 使能管脚时钟 */
    RCC_APB2PeriphClockCmd(COMn_TX_PORT_CLK[COMx] | COMn_RX_PORT_CLK[COMx] | RCC_APB2Periph_AFIO, ENABLE);

    COM_State[COMx] = 1; //使能串口发送
    /* 使能串口外设时钟 */
    if(COMx == COM1)
    {
#if (Mapping_COM1OFFON == 1)  //串口1开管脚重映射
        GPIO_PinRemapConfig(GPIO_Remap_USART1, ENABLE);
#endif
        RCC_APB2PeriphClockCmd(COMn_USART_CLK[COMx], ENABLE);
    }
    else
    {
        /* 串口管脚软件重映射 */
        if(COMx == COM2)
        {
#if (Mapping_COM2OFFON == 1)  //串口2开管脚重映射
            GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
#endif
        }
        else if(COMx == COM3)
        {
#if (Mapping_COM3OFFON == 1)  //串口3开管脚重映射
            GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);
#elif (Mapping_COM3OFFON == 2)
            GPIO_PinRemapConfig(GPIO_FullRemap_USART3, ENABLE);
#endif
        }

        RCC_APB1PeriphClockCmd(COMn_USART_CLK[COMx], ENABLE);
    }

//==========串口管脚配置========================================================
    /* 配置串口发送脚--->复用推挽输出 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = COMn_TX_PIN[COMx];
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(COMn_TX_PORT[COMx], &GPIO_InitStructure);

    /* 配置串口接受脚--->浮空输入 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Pin = COMn_RX_PIN[COMx];
    GPIO_Init(COMn_RX_PORT[COMx], &GPIO_InitStructure);
//==========串口参数配置======================
    USART_InitStructure.USART_BaudRate = Baud;    //配置波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    /* 根据GPIO_InitStruct中指定的参数初始化外设GPIOx寄存器 */
    USART_Init(COMn_USART[COMx], &USART_InitStructure);

    /* 使能串口 */
    USART_Cmd(COMn_USART[COMx], ENABLE);
    /* 开启串口接受中断*/
    USART_ITConfig(COMn_USART[COMx], USART_IT_RXNE, ENABLE);//开启串口接受中断
    /* 清除串口发送完成标志(防止第一字节丢失)*/
    USART_ClearFlag(COMn_USART[COMx], USART_FLAG_TC);      //清除发送完成标志

//=======串口中断优先级配置==============================================================
    //Usart1 NVIC 配置
    if(Priority1 >= 15)Priority1 = 15;

    if(Priority2 >= 15)Priority2 = 15;

    NVIC_InitStructure.NVIC_IRQChannel = COMn_IRQn[COMx];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = Priority1 ; //抢占优先级3 (0----15)
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = Priority2;				 //子优先级3   (0----15)
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;										 //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);																     //根据指定的参数初始化VIC寄存器
}

//==============数据发送 ===========================================
/*
**********************************************************************
*功能：选择串口号发送单字节
*入口(COMx,DAT)
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
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#ifndef __MICROLIB
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
//	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
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
/*使用microLib的方法*/
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
//======LCD输出转接打印==========================================

const u8 enumA[] = {
    0,
    1,  //RC522模块信息
//    2,  //AS608应用
//    3,  //ADC数据读取
//    4,  //LED的状态
//    5,  //温湿度模块
//    6,  //锁的控制	
//	  7,  //FM24L64铁电模块
//	  8,  //打印风机信息	
};
#include "stdlib.h"
void COM_Printf(u8 type, char *fmt, ...)
{

    u8 i;
    u8 StrlenC = 0;
	  u8 *Com_pstr = NULL;
    va_list ap; //定义个链表
    for(i = 0; i < sizeof(enumA); i++) {
        if(type == enumA[i] && type != 0) {
            goto enumA1;
        }
    }
    return ;
enumA1:
	Com_pstr = malloc(200);  //分配空间
	if(Com_pstr == NULL)return;
    va_start(ap, fmt); //链表开始
    vsnprintf((char *)Com_pstr,200, fmt, ap); //数据格式化
    va_end(ap);       //链表结束
    StrlenC = strlen((char *)Com_pstr);
    Usart_SendString(COM2, Com_pstr, StrlenC);
		free(Com_pstr);  //释放空间
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

