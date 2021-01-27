/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : notecat.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-15         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _notecat_H
#define _notecat_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f10x.h"
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

void NoteCat_OutTimeTiming(void);   //放入定时器
void GSM_ControlStateMachine(void); //通信猫状态机控制
void GSM_StateMachine(void);
typedef struct _SendSms{
  u8 * pStr;
  u16 Len  ;
  u16 MaxLen;
}SendSmsType;
//开机初始化-->获取管理员电话
void StartOpenGSM_Init(void);
//extern u64 AdministratorPhone;
void SaveAdministratorPhone(u64 Phone); //保存管理员电话
u64  ReadAdministratorPhone(void);      //获得管理员电话


void notecat_Main(void);
/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

