/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : TM_que.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-04         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _TM_que_H
#define _TM_que_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

typedef struct TM_Queue{
  u8 * Pbuf;              //队列数组指针
  volatile u16 w_index  ; //写位置
  volatile u16 r_index  ; //读位置
  volatile u16 buf_size ; //数组大小
  volatile u16 unit_size; //单位大小  
  volatile u16 Max_Cnt  ; //允许放的队列数量(以队列单位为间隔)
}TM_QueueObject;

extern TM_QueueObject HID_RxQue;
extern TM_QueueObject HID_TxQue;
extern TM_QueueObject TMQue_WayBill; //命令缓存

void TM_que_AllRegistInit(void); //左右队列初始化

//队列注册
void TM_que_Regist(TM_QueueObject *que,u8 *buf,u16 buf_size ,u16 unit_size);
//队列大小未读取空间
u16 TM_que_size(TM_QueueObject que);
//队列大小未读取空间
u16 TM_que_sizeIRQ(TM_QueueObject que);
//队列写
void TM_que_Write(TM_QueueObject*que,u8 *dat ,u8 len);
//队列读
int TM_que_Read(TM_QueueObject*que ,u8*dat,u8*len);
//清除队列
uint8_t TM_que_ClearAll(TM_QueueObject*que);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

