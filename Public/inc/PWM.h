/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : PWM.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-06-16         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _PWM_H
#define _PWM_H

#include "stm32f10x.h"

typedef enum _CH
{
 CH1=0x01,
 CH2=0x02,
 CH3=0x04,
 CH4=0x08
}CH_enum;

typedef struct _PWM
{
    u16 Period;    //装载值
    u16 Prescaler; //分频系数
}PWM_TypeDef;

extern PWM_TypeDef PWM1;
extern PWM_TypeDef PWM4;

void Time1_PWM_Init(void);                                                     //定时器1PWM初始化
void Time4_PWM_Init(void);                                                     //定时器4PWM初始化

void TIM_PWM_GetInitDat(u32 Target_Hz,PWM_TypeDef *PWM_Struct);                //计算分频初始值    （目标频率,获取参数）计算
void adjuctTIM_Fre(TIM_TypeDef* TIMx,PWM_TypeDef PWM_Struct);                  //调整定时器时钟基准                     设置
void Timer1_PWM_Out(u16 CHx,PWM_TypeDef PWM_Struct,FunctionalState NewState);  //通道输出  //(通道，参数，新状态)       启动
void Timer4_PWM_Out(u16 CHx,PWM_TypeDef PWM_Struct,FunctionalState NewState);  //通道输出  //(通道，参数，新状态)       启动
//(定时器 ,通道，转载值，新状态)
void Timer_PWM_Out(TIM_TypeDef* TIMx,u8 CHx,u16 PulseValue,FunctionalState NewState);
#endif
/********************************End of File************************************/

