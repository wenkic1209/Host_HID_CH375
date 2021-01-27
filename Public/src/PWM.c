/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2020,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : PWM.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2020-06-16
* Description  :
* Function List:
******************************************************************************
*/
/********************************End of Head************************************/
#include "PWM.h"
#include "debug.h"
/*                   |
  *TIM5复用功能重映射|
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *复用功能          | TIM5CH4_IREMAP = 0        |  TIM5CH4_IREMAP = 1                                                         |
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *TIM5_CH4          | TIM5的通道4连至PA3        |  LSI内部时钟连至TIM5_CH4的输入作为校准使用                                  |
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *TIM4复用功能重映射|
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *复用功能          | TIM4_REMAP = 0            |  TIM4_REMAP = 1                                                             |
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *TIM4_CH1          | PB6                       |  PD12                                                                       |
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *TIM4_CH2          | PB7                       |  PD13                                                                       |
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *TIM4_CH3          | PB8                       |  PD14                                                                       |
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *TIM4_CH4          | PB9                       |  PD15                                                                       |
  *------------------|---------------------------|-----------------------------------------------------------------------------|
  *TIM3复用功能重映射|
  *------------------|---------------------------|-------------------------|---------------------------------------------------|
  *复用功能          | TIM3_REMAP[1:0] = 00      |  TIM3_REMAP[1:0] = 10   |    TIM3_REMAP[1:0] = 01                           |
  *------------------|---------------------------|-------------------------|---------------------------------------------------|
  *TIM3_CH1          | PA6                       |  PB4                    |    PC6                                            |
  *------------------|---------------------------|-------------------------|---------------------------------------------------|
  *TIM3_CH2          | PA7                       |  PB5                    |    PC7                                            |
  *------------------|---------------------------|-------------------------|---------------------------------------------------|
  *TIM3_CH3          |               PB0                                   |    PC8                                            |
  *------------------|-----------------------------------------------------|---------------------------------------------------|
  *TIM3_CH4          |               PB1                                   |    PC9                                            |
  *------------------|-----------------------------------------------------|---------------------------------------------------|
  *TIM2复用功能重映射|
  *------------------|---------------------------|-------------------------|---------------------------|-----------------------|
  *复用功能          | TIM2_REMAP[1:0] = 00      |  TIM2_REMAP[1:0] =01    |   TIM2_REMAP[1:0] = 10    |  TIM2_REMAP[1:0] = 11 |
  *------------------|---------------------------|-------------------------|---------------------------|-----------------------|
  *TIM2_CH1_ETR(2)   | PA0                       |  PA15                   |   PA0                     |  PA15                 |
  *------------------|---------------------------|-------------------------|---------------------------|-----------------------|
  *TIM2_CH2          | PA1                       |  PB3                    |   PA1                     |  PA3                  |
  *------------------|---------------------------|-------------------------|---------------------------|-----------------------|
  *TIM2_CH3          |                     PA2                             |               PB10                                |
  *------------------|-----------------------------------------------------|---------------------------------------------------|
  *TIM2_CH4          |                     PA3                             |               PB11                                |
  *------------------|-----------------------------------------------------|---------------------------------------------------|
  *TIM1复用功能重映射
  *-------------------|--------------------------|-------------------------|---------------------------------------------------|
  *复用功能           | TIM1_REMAP[1:0] = 00     |   TIM1_REMAP[1:0] = 01  |    TIM1_REMAP[1:0] = 11                           |
  *-------------------|--------------------------|-------------------------|---------------------------------------------------|
  *TIM1_ETR           |                     PA12                           |    PE7                                            |
  *-------------------|----------------------------------------------------|---------------------------------------------------|
  *TIM1_CH1           |                     PA8                            |    PE9                                            |
  *-------------------|----------------------------------------------------|---------------------------------------------------|
  *TIM1_CH2           |                     PA9                            |    PE11                                           |
  *-------------------|----------------------------------------------------|---------------------------------------------------|
  *TIM1_CH3           |                     PA10                           |    PE12                                           |
  *-------------------|----------------------------------------------------|---------------------------------------------------|
  *TIM1_CH4           |                     PA11                           |    PE14                                           |
  *-------------------|--------------------------|-------------------------|---------------------------------------------------|
  *TIM1_BKIN          |  PB12                    |   PA6                   |    PE15                                           |
  *-------------------|--------------------------|-------------------------|---------------------------------------------------|
  *TIM1_CH1N          |  PB13                    |   PA7                   |    PE8                                            |
  *-------------------|--------------------------|-------------------------|---------------------------------------------------|
  *TIM1_CH2N          |  PB14                    |   PB0                   |    PE10                                           |
  *-------------------|--------------------------|-------------------------|---------------------------------------------------|
  *TIM1_CH3N          |  PB15                    |   PB1                   |    PE12                                           |
  *-------------------|--------------------------|-------------------------|---------------------------------------------------|
  *
  *--------------------------------------------------------------------------------------------------------------------------
  *PC6 TIM3_CH1
*/
//typedef enum _CH
//{
// CH1=0x01,
// CH2=0x02,
// CH3=0x04,
// CH4=0x08
//}CH_enum;

//typedef struct _PWM
//{
//    u16 Period;    //装载值
//    u16 Prescaler; //分频系数
//}PWM_TypeDef;

void Time1_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE); //打开时钟    
    
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&GPIO_InitStruct);
      
    //初始化TIM1
    TIM_DeInit ( TIM1 );
    //GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//改变指定管脚的映射
    //TIMx的时间基准校准
    //(1+TIM_Prescaler )/72M)*(1+TIM_Period )=((1+7199)/72M)*(1+9999)=1秒 = 10000
    //  (1+预分频数)(1+自动重装值) /系统时钟  = 时间
    //  Prescaler = SystemCoreClock / 1000 - 1; //得到1KHz (分频系数)
    //  Period    = n100us - 1;                 //(装载值)
    TIM_TimeBaseInitTypeDef  Tim_TimeBaseInitStruct; //初始化TIMx的时间基准校准
    /* 配置输出频率 */
    Tim_TimeBaseInitStruct.TIM_Period        = 0x18-1;                //自动装载值(PWM分辨率)
    Tim_TimeBaseInitStruct.TIM_Prescaler     = 3-1;                 //分频系数(Prescaler = 4,定时器频率为14.4KHz )(Prescaler = 9,定时器频率为7.2KHz )
    Tim_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1      ;//时钟分割
    Tim_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;//计数方向
    TIM_TimeBaseInit(TIM1,&Tim_TimeBaseInitStruct);
    //初始化定时TIMx外设
    TIM_OCInitTypeDef        TIM_OCInitStruct;                    
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM1;        //脉冲宽度调制模式1
    //输出极性
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_Low;        //输出极性         低  
    TIM_OCInitStruct.TIM_OCNPolarity  = TIM_OCNPolarity_High;      //互补输出极性     高  
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;       //在空闲时输出     高  
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;    //在空闲时互补输出 低 
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;    //PWM输出使能
    TIM_OCInitStruct.TIM_Pulse        = 0;                         //捕获比较的脉冲值
    
    TIM_OC1Init(TIM1,&TIM_OCInitStruct);   
    TIM_OC1PolarityConfig(TIM1, TIM_OCPreload_Enable);
    
    TIM_ARRPreloadConfig(TIM1, ENABLE);  //使能预装载寄存器
    TIM_Cmd(TIM1, ENABLE);               //使能定时器1
    TIM_CtrlPWMOutputs(TIM1, ENABLE);    //pwm输出
}

void Time4_PWM_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE); //打开时钟    
    
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStruct);
    
    
      
    //初始化TIM1
    TIM_DeInit ( TIM4 );
    //GPIO_PinRemapConfig(GPIO_FullRemap_TIM3,ENABLE);//改变指定管脚的映射
    //TIMx的时间基准校准
    //(1+TIM_Prescaler )/72M)*(1+TIM_Period )=((1+7199)/72M)*(1+9999)=1秒 = 10000
    //  (1+预分频数)(1+自动重装值) /系统时钟  = 时间
    //  Prescaler = SystemCoreClock / 1000 - 1; //得到1KHz (分频系数)
    //  Period    = n100us - 1;                 //(装载值)
    TIM_TimeBaseInitTypeDef  Tim_TimeBaseInitStruct; //初始化TIMx的时间基准校准
    /* 配置输出频率 */
    Tim_TimeBaseInitStruct.TIM_Period        = 0x18-1;            //自动装载值(PWM分辨率)
    Tim_TimeBaseInitStruct.TIM_Prescaler     = 3-1;               //分频系数(Prescaler = 4,定时器频率为14.4KHz )(Prescaler = 9,定时器频率为7.2KHz )
    Tim_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1      ;//时钟分割
    Tim_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;//计数方向
    TIM_TimeBaseInit(TIM4,&Tim_TimeBaseInitStruct);
    //初始化定时TIMx外设
    TIM_OCInitTypeDef        TIM_OCInitStruct;                    
    TIM_OCInitStruct.TIM_OCMode       = TIM_OCMode_PWM2;          //脉冲宽度调制模式1
    //输出极性
    TIM_OCInitStruct.TIM_OCPolarity   = TIM_OCPolarity_Low ;        //输出极性         低  TIM_OCPolarity_Low
    TIM_OCInitStruct.TIM_OCNPolarity  = TIM_OCNPolarity_High;      //互补输出极性     高  
    TIM_OCInitStruct.TIM_OCIdleState  = TIM_OCIdleState_Set;       //在空闲时输出     高  
    TIM_OCInitStruct.TIM_OCNIdleState = TIM_OCNIdleState_Reset;    //在空闲时互补输出 低 
    TIM_OCInitStruct.TIM_OutputState  = TIM_OutputState_Enable;    //PWM输出使能
    TIM_OCInitStruct.TIM_Pulse        = 0;                         //捕获比较的脉冲值
    
    TIM_OC1Init(TIM4,&TIM_OCInitStruct);   
    TIM_OC1PolarityConfig(TIM4, TIM_OCPreload_Enable);
    
    TIM_OC2Init(TIM4,&TIM_OCInitStruct);   
    TIM_OC2PolarityConfig(TIM4, TIM_OCPreload_Enable); 
    
    TIM_OC3Init(TIM4,&TIM_OCInitStruct);   
    TIM_OC3PolarityConfig(TIM4, TIM_OCPreload_Enable);  
    
    TIM_OC4Init(TIM4,&TIM_OCInitStruct);   
    TIM_OC4PolarityConfig(TIM4, TIM_OCPreload_Enable);      
    
    TIM_ARRPreloadConfig(TIM4, ENABLE);  //使能预装载寄存器
    TIM_Cmd(TIM4, ENABLE);               //使能定时器4
    TIM_CtrlPWMOutputs(TIM4, ENABLE);    //pwm输出
}
    
//素数判断
static int isPrime(u32 n) {
    for (u32 i = 2; i < n; i++) {
        //可以被别的数整除，不是素数
        if (n % i == 0)
            return 0;
    }
    return 1;
}
//质因数分解,分解素数为止
static int getPrimeFactor(u32 n,u32 *out) {    //可以不返回值，此处返回-1表示出错，返回1表示正常。
    //1以下都不是素数
    if (n < 2)
        return -1;
    //判断是否是素数,如果是直接输出
    if (isPrime(n)) {
          *out = n;
        return 1;
    }
    else {
        //递归因式分解
        for (u32 i = 2; i < n; i++) {

            if (n % i == 0) {
                getPrimeFactor(n/i,out); //下一轮求素数
                break;
            }
        }
    }
    return 1;
}

//计算分频初始值  （目标频率,获取参数）
void TIM_PWM_GetInitDat(u32 Target_Hz,PWM_TypeDef *PWM_Struct)
{
    RCC_ClocksTypeDef sys;
    RCC_GetClocksFreq(&sys);  // 获取系统频率
    //PWM_hz = sysFre/[(1+Pre)*(1+Per)]
    u32 sysHZ = sys.SYSCLK_Frequency;
    u32 Period=0;
    u32 Prescaler = 0;
    u32 temp = 0;    
    u32 TargetTemp = sysHZ/Target_Hz; //  
    //TargetTemp = (1+Pre)*(1+Per)
    
    //涉及质因数分解，素数
    if(getPrimeFactor(TargetTemp,&temp)==1)
    {
        Period    = TargetTemp/temp;
        Prescaler = temp;        
    }else{
        pr_err("因式分解输入错误");
        return ;
    }
    while(Period>=0xFFFF) //防止溢出，特别是小频率时
    {
        if(getPrimeFactor(Period,&temp)==1)
        {
            Prescaler *= temp;
            Period     = TargetTemp/Prescaler;            
        }else{
            //pr_err("因式分解输入错误");
            return ;
        }
    }
    //因式分解出的最小素数作为定时器的分频系数
    PWM_Struct->Period    = Period;      //预装载值
    PWM_Struct->Prescaler = Prescaler;   //分频系数
}

//调整定时器时钟基准
void adjuctTIM_Fre(TIM_TypeDef* TIMx,PWM_TypeDef PWM_Struct)
{
    TIM_TimeBaseInitTypeDef  Tim_TimeBaseInitStruct; //初始化TIMx的时间基准校准
    /* 配置输出频率 */
    Tim_TimeBaseInitStruct.TIM_Period        = PWM_Struct.Period?PWM_Struct.Period-1:0;         //自动装载值(PWM分辨率)
    Tim_TimeBaseInitStruct.TIM_Prescaler     = PWM_Struct.Prescaler?PWM_Struct.Prescaler-1:0;   //分频系数(Prescaler = 4,定时器频率为14.4KHz )(Prescaler = 9,定时器频率为7.2KHz )
    Tim_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1      ;//时钟分割
    Tim_TimeBaseInitStruct.TIM_CounterMode   = TIM_CounterMode_Up;//计数方向
    TIM_TimeBaseInit(TIMx,&Tim_TimeBaseInitStruct);
}


PWM_TypeDef PWM1;
PWM_TypeDef PWM4;
//(定时器 ,通道，转载值，新状态)
void Timer_PWM_Out(TIM_TypeDef* TIMx,u8 CHx,u16 PulseValue,FunctionalState NewState)
{
  if(NewState == DISABLE)
  { 
     if(CHx&0x01) TIM_SetCompare1(TIMx,0); 
     if(CHx&0x02) TIM_SetCompare2(TIMx,0); 
     if(CHx&0x04) TIM_SetCompare3(TIMx,0); 
     if(CHx&0x08) TIM_SetCompare4(TIMx,0);       
  }else{
     if(CHx&0x01) TIM_SetCompare1(TIMx,PulseValue); 
     if(CHx&0x02) TIM_SetCompare2(TIMx,PulseValue); 
     if(CHx&0x04) TIM_SetCompare3(TIMx,PulseValue); 
     if(CHx&0x08) TIM_SetCompare4(TIMx,PulseValue);   
  }
}

//(通道，参数，新状态)
void Timer1_PWM_Out(u16 CHx,PWM_TypeDef PWM_Struct,FunctionalState NewState)
{
  if(NewState == DISABLE)
  { 
     if(CHx&0x01) TIM_SetCompare1(TIM1,0); 
     if(CHx&0x02) TIM_SetCompare2(TIM1,0); 
     if(CHx&0x04) TIM_SetCompare3(TIM1,0); 
     if(CHx&0x08) TIM_SetCompare4(TIM1,0);       
  }else{
     if(CHx&0x01) TIM_SetCompare1(TIM1,PWM_Struct.Period/2); 
     if(CHx&0x02) TIM_SetCompare2(TIM1,PWM_Struct.Period/2); 
     if(CHx&0x04) TIM_SetCompare3(TIM1,PWM_Struct.Period/2); 
     if(CHx&0x08) TIM_SetCompare4(TIM1,PWM_Struct.Period/2);   
  }
}


//(通道，参数，新状态)
void Timer4_PWM_Out(u16 CHx,PWM_TypeDef PWM_Struct,FunctionalState NewState)
{
  if(NewState == DISABLE)
  { 
     if(CHx&0x01) TIM_SetCompare1(TIM4,0); 
     if(CHx&0x02) TIM_SetCompare2(TIM4,0); 
     if(CHx&0x04) TIM_SetCompare3(TIM4,0); 
     if(CHx&0x08) TIM_SetCompare4(TIM4,0);       
  }else{
     if(CHx&0x01) TIM_SetCompare1(TIM4,PWM_Struct.Period/2); 
     if(CHx&0x02) TIM_SetCompare2(TIM4,PWM_Struct.Period/2); 
     if(CHx&0x04) TIM_SetCompare3(TIM4,PWM_Struct.Period/2); 
     if(CHx&0x08) TIM_SetCompare4(TIM4,PWM_Struct.Period/2);   
  }
}


#if 0 //测试使用
void Use_PWM_Test(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    Time1_PWM_Init();                   //定时器1PWM初始化
    TIM_PWM_GetInitDat(1000000,&PWM1);  //计算时基1MHz PWM
    adjuctTIM_Fre(TIM1,PWM1);           //更新时基
    Timer1_PWM_Out(CH1,PWM1,ENABLE);    //输出PWM
}
#endif







/********************************End of File************************************/

