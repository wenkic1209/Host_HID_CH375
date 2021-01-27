  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : TM_que.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2021-01-04         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "TM_que.h"
#include "stdio.h"
/*  ��������ʣ����  w:дλ�� r:��λ�� s:λ�ô�С*/
#define QueFIFO_LEN(w,r,s)       (((w) >= (r)) ? ((w) - (r)) : ((w) + (s) - (r)))  


//typedef struct TM_Queue{
//  u8 * Pbuf;              //��������ָ��
//  volatile u16 w_index  ; //дλ��
//  volatile u16 r_index  ; //��λ��
//  volatile u16 buf_size ; //�����С
//  volatile u16 unit_size; //��λ��С  
//  volatile u16 Max_Cnt  ; //����ŵĶ�������(�Զ��е�λΪ���)
//}TM_QueueObject;

//* @Name   TM_que_Regist
//* @brief  ���еĵ�ע��
//* @param  que : ע�����ͷ
//          buf : �����ַ
//          buf_size : �����ַ�ڴ�
//          unit_size: ���е�λ�ڴ���
//* @retval  
void TM_que_Regist(TM_QueueObject *que,u8 *buf,u16 buf_size ,u16 unit_size)
{
   que->Pbuf     = buf;                 //��ַ
   que->buf_size = buf_size;            //��ַ�ڴ��С
   que->unit_size= unit_size;           //ƫ����
   que->r_index  = 0;                   //����λ��
   que->w_index  = 0;                   //д��λ��
   que->Max_Cnt  = buf_size/unit_size;     
}


//�������Ƿ�Ϊ��
static int TM_que_CheckEmpty(TM_QueueObject*que)
{
	return !(que->r_index == que->w_index);
}
//���д�Сδ��ȡ�ռ�
u16 TM_que_size(TM_QueueObject que)
{
    return QueFIFO_LEN(que.w_index,que.r_index,que.Max_Cnt);
}
//���д�Сδ��ȡ�ռ�
u16 TM_que_sizeIRQ(TM_QueueObject que)
{
    return QueFIFO_LEN(que.w_index,que.r_index,que.Max_Cnt);
}
//����д
void TM_que_Write(TM_QueueObject*que,u8 *dat ,u8 len)
{
  u16 BaseAddr = (que->w_index)*(que->unit_size);
  if(que->Pbuf == NULL)return;
  if(TM_que_size(*que)==(que->Max_Cnt-1))
  {
    que->r_index++;      //��������Ķ��нڵ�
	que->r_index %= que->Max_Cnt;
  }
  for(u16 i=0;i<(que->unit_size);i++)
  {
      if(i<len)
      {
        que->Pbuf[BaseAddr+i] = dat[i];       
      }else{
        que->Pbuf[BaseAddr+i]  = 0;           
      }
  }
  que->w_index++;
  que->w_index %= que->Max_Cnt;  
}
//���ж�
int TM_que_Read(TM_QueueObject*que ,u8*dat,u8*len)
{
  u16 BaseAddr = (que->r_index)*(que->unit_size);
  if(que->Pbuf == NULL)return 0;
  if(TM_que_CheckEmpty(que))
  {
      for(u16 i=0;i<que->unit_size;i++)
      {
         dat[i] =que->Pbuf[BaseAddr+i]; //�������
      }
	  que->r_index++;             //���нڵ�����
	  que->r_index %= que->Max_Cnt;
	  return 1;
  }else{
	return 0;
  }
}

//�������
uint8_t TM_que_ClearAll(TM_QueueObject*que)
{
   if(que->r_index != que->w_index  && (que->r_index !=0) && que->w_index !=0)
   {
       que->r_index  = 0;   //����λ��
       que->w_index  = 0;   //д��λ��
       return 1;
   }
   return 0;
}

static u8 HID_RxBuf[65*20] = {0};
static u8 HID_TxBuf[65*60] = {0};
TM_QueueObject HID_RxQue;
TM_QueueObject HID_TxQue;
static u8 Game_WayBillBuf[60*3] = {0};
TM_QueueObject TMQue_WayBill;

void TM_que_AllRegistInit(void)
{
    TM_que_Regist(&HID_RxQue ,HID_RxBuf,sizeof(HID_RxBuf),65);
    TM_que_Regist(&HID_TxQue ,HID_TxBuf,sizeof(HID_TxBuf),65);
    TM_que_Regist(&TMQue_WayBill ,Game_WayBillBuf,sizeof(Game_WayBillBuf),60);  //·�������
}



 
 
/********************************End of File************************************/

