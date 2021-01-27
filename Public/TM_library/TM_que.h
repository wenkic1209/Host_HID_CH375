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
  u8 * Pbuf;              //��������ָ��
  volatile u16 w_index  ; //дλ��
  volatile u16 r_index  ; //��λ��
  volatile u16 buf_size ; //�����С
  volatile u16 unit_size; //��λ��С  
  volatile u16 Max_Cnt  ; //����ŵĶ�������(�Զ��е�λΪ���)
}TM_QueueObject;

extern TM_QueueObject HID_RxQue;
extern TM_QueueObject HID_TxQue;
extern TM_QueueObject TMQue_WayBill; //�����

void TM_que_AllRegistInit(void); //���Ҷ��г�ʼ��

//����ע��
void TM_que_Regist(TM_QueueObject *que,u8 *buf,u16 buf_size ,u16 unit_size);
//���д�Сδ��ȡ�ռ�
u16 TM_que_size(TM_QueueObject que);
//���д�Сδ��ȡ�ռ�
u16 TM_que_sizeIRQ(TM_QueueObject que);
//����д
void TM_que_Write(TM_QueueObject*que,u8 *dat ,u8 len);
//���ж�
int TM_que_Read(TM_QueueObject*que ,u8*dat,u8*len);
//�������
uint8_t TM_que_ClearAll(TM_QueueObject*que);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

