/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2020,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : Moter.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2020-12-11
* Description  : 电机控制部分
* Function List:
******************************************************************************
*/
/********************************End of
 * Head************************************/
#include "Moter.h"
#include "driver.h"
#include "public.h"

typedef struct _MoterControl {
  TM_GPIO Switch;  //开关
  TM_GPIO dir;     //方向
} MoterControlType;

//左右电机
static MoterControlType MoterLR = {
    {GPIOC, GPIO_Pin_0},
    {GPIOC, GPIO_Pin_2},
};
//上下电机
static MoterControlType MoterUD = {
    {GPIOA, GPIO_Pin_0},
    {GPIOB, GPIO_Pin_5},
};
//前后电机
static MoterControlType MoterBF = {
    {GPIOB, GPIO_Pin_3 },
    {GPIOC, GPIO_Pin_12},
};

typedef struct _Moter {
  u8 EN ;       //使能
  u8 dir;       //方向
  u16 Runtime;  //最大运行时间
  u16 Speed  ;  //速度
} TypeMoter;

TypeMoter MoterX = {0};
TypeMoter MoterY = {0};
TypeMoter MoterZ = {0};

//* @Name   Moter_GPIOInit
//* @brief  电机控制管脚初始化
//* @param  参数描述
//* @retval
void Moter_GPIOInit(void) {

  TM_GPIO_Init(MoterLR.Switch.GPIO, MoterLR.Switch.Pin, GPIO_Mode_Out_PP,
               GPIO_Speed_50MHz);  //左右
  TM_GPIO_Init(MoterLR.dir.GPIO, MoterLR.dir.Pin, GPIO_Mode_Out_PP,
               GPIO_Speed_50MHz);
  TM_GPIO_Init(MoterUD.Switch.GPIO, MoterUD.Switch.Pin, GPIO_Mode_Out_PP,
               GPIO_Speed_50MHz);  //上下
  TM_GPIO_Init(MoterUD.dir.GPIO, MoterUD.dir.Pin, GPIO_Mode_Out_PP,
               GPIO_Speed_50MHz);
  TM_GPIO_Init(MoterBF.Switch.GPIO, MoterBF.Switch.Pin, GPIO_Mode_Out_PP,
               GPIO_Speed_50MHz);  //前后
  TM_GPIO_Init(MoterBF.dir.GPIO, MoterBF.dir.Pin, GPIO_Mode_Out_PP,
               GPIO_Speed_50MHz);
  TM_GPIO_Init(GPIOC, GPIO_Pin_5, GPIO_Mode_Out_PP,
               GPIO_Speed_50MHz);  //爪子电机

 LRMoterControl(DISABLE,0);  //左右电机控制
 BFMoterControl(DISABLE,0);  //前后电机控制
 UDMoterControl(DISABLE,0);  //上下电机控制
                              
}

//左右电机控制
void LRMoterControl(u8 Switch, u8 dir) {  // *INDENT-OFF*
  static u8 Lsw = 0x05;
  static u8 Ldir= 0x05;
  if (Lsw != Switch) {
    Lsw = Switch;
    TM_GPIO_SetPinValue(MoterLR.Switch.GPIO, MoterLR.Switch.Pin, Switch);
  }
  if (Ldir != dir) {
    Ldir = dir;
    TM_GPIO_SetPinValue(MoterLR.dir.GPIO, MoterLR.dir.Pin, dir);
  }
}
//前后电机控制
void BFMoterControl(u8 Switch, u8 dir) {
  static u8 Lsw = 0x05;
  static u8 Ldir= 0x05;
  if (Lsw != Switch) {
    Lsw = Switch;
    TM_GPIO_SetPinValue(MoterBF.Switch.GPIO, MoterBF.Switch.Pin, Switch);
  }
  if (Ldir != dir) {
    Ldir = dir;
    TM_GPIO_SetPinValue(MoterBF.dir.GPIO, MoterBF.dir.Pin, dir);
  }
}
//上下电机控制
void UDMoterControl(u8 Switch, u8 dir) {

  static u8 Lsw = 0x05;
  static u8 Ldir= 0x05;
    
    
  if (Lsw != Switch) {
    Lsw = Switch;
    TM_GPIO_SetPinValue(MoterUD.Switch.GPIO, MoterUD.Switch.Pin, Switch);
  }  //开关
  if (Ldir != dir) {
    Ldir = dir;
    TM_GPIO_SetPinValue(MoterUD.dir.GPIO, MoterUD.dir.Pin, dir);
  }  //方向
}

//上下电机控制
void ClawMoterControl(u8 Switch) {

  static u8 Lsw = 0x05;
  if (Lsw != Switch) {
    Lsw = Switch;
    TM_GPIO_SetPinValue(GPIOC, GPIO_Pin_5, Switch);
  }  //开关
}
//* @Name   MainGameMoterControl
//* @brief  电机操作
//* @param  None
//* @retval None
void MainGameMoterControl(void) {
//  //左
//  if (!SQ_Left.status && dirLeft.status) {
//      LRMoterControl(ENABLE,0);            //控制        
//      Timer4_PWM_Out(CH4,PWM4,ENABLE) ;    //速度
//  }
//  //右
//  else if (dirRight.status) {
//      LRMoterControl(ENABLE,1);            //控制        
//      Timer4_PWM_Out(CH4,PWM4,ENABLE) ;    //速度 
//  } else {
//      Timer4_PWM_Out(CH4,PWM4,DISABLE) ;   //速度   
//      LRMoterControl(DISABLE,MoterX.dir);  //控制
//  }
//  //前
//  if ((!SQ_Front.status) && (dirFront.status)) {
//      BFMoterControl(ENABLE,0);            //关闭控制        
//      Timer4_PWM_Out(CH1,PWM4,ENABLE) ;    //输出PWM
//  }
//  //后
//  else if (!SQ_Back.status && dirBack.status) {
//      BFMoterControl(ENABLE,1);            //关闭控制        
//      Timer4_PWM_Out(CH1,PWM4,ENABLE) ;    //输出PWM      
//  } else {
//      Timer4_PWM_Out(CH1,PWM4,DISABLE) ;    //输出PWM
//      BFMoterControl(DISABLE,MoterX.dir);   //关闭控制
//  }
  
  //上
  if (!SQ_Up.status && dirFront.status) {
      UDMoterControl(ENABLE,0);            //关闭控制    上勾
      Timer4_PWM_Out(CH2,PWM4,ENABLE) ;    //输出PWM
  }
  //下
  else if (!SQ_Down.status&&dirBack.status) {
      UDMoterControl(ENABLE,1);            //关闭控制     下勾
      Timer4_PWM_Out(CH2,PWM4,ENABLE) ;    //输出PWM      
  } else {
      Timer4_PWM_Out(CH2,PWM4,DISABLE) ;    //输出PWM
      BFMoterControl(DISABLE,MoterZ.dir);   //关闭控制
  }  
  //爪子
  if (dirLeft.status) {
      ClawMoterControl(ENABLE);            //关闭控制        
      Timer4_PWM_Out(CH3,PWM4,ENABLE) ;    //输出PWM
  }else{
      Timer4_PWM_Out(CH3,PWM4,DISABLE) ;    //输出PWM          
      ClawMoterControl(DISABLE);            //关闭控制        
  }  
}

/********************************End of
 * File************************************/
