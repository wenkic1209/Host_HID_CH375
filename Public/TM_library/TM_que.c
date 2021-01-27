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
/*  计算数据剩余量  w:写位置 r:读位置 s:位置大小*/
#define QueFIFO_LEN(w,r,s)       (((w) >= (r)) ? ((w) - (r)) : ((w) + (s) - (r)))  


//typedef struct TM_Queue{
//  u8 * Pbuf;              //队列数组指针
//  volatile u16 w_index  ; //写位置
//  volatile u16 r_index  ; //读位置
//  volatile u16 buf_size ; //数组大小
//  volatile u16 unit_size; //单位大小  
//  volatile u16 Max_Cnt  ; //允许放的队列数量(以队列单位为间隔)
//}TM_QueueObject;

//* @Name   TM_que_Regist
//* @brief  队列的的注册
//* @param  que : 注册队列头
//          buf : 缓存地址
//          buf_size : 缓存地址内存
//          unit_size: 队列单位内存间隔
//* @retval  
void TM_que_Regist(TM_QueueObject *que,u8 *buf,u16 buf_size ,u16 unit_size)
{
   que->Pbuf     = buf;                 //地址
   que->buf_size = buf_size;            //地址内存大小
   que->unit_size= unit_size;           //偏移量
   que->r_index  = 0;                   //读的位置
   que->w_index  = 0;                   //写的位置
   que->Max_Cnt  = buf_size/unit_size;     
}


//检查队列是否为空
static int TM_que_CheckEmpty(TM_QueueObject*que)
{
	return !(que->r_index == que->w_index);
}
//队列大小未读取空间
u16 TM_que_size(TM_QueueObject que)
{
    return QueFIFO_LEN(que.w_index,que.r_index,que.Max_Cnt);
}
//队列大小未读取空间
u16 TM_que_sizeIRQ(TM_QueueObject que)
{
    return QueFIFO_LEN(que.w_index,que.r_index,que.Max_Cnt);
}
//队列写
void TM_que_Write(TM_QueueObject*que,u8 *dat ,u8 len)
{
  u16 BaseAddr = (que->w_index)*(que->unit_size);
  if(que->Pbuf == NULL)return;
  if(TM_que_size(*que)==(que->Max_Cnt-1))
  {
    que->r_index++;      //覆盖最早的队列节点
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
//队列读
int TM_que_Read(TM_QueueObject*que ,u8*dat,u8*len)
{
  u16 BaseAddr = (que->r_index)*(que->unit_size);
  if(que->Pbuf == NULL)return 0;
  if(TM_que_CheckEmpty(que))
  {
      for(u16 i=0;i<que->unit_size;i++)
      {
         dat[i] =que->Pbuf[BaseAddr+i]; //获得数据
      }
	  que->r_index++;             //队列节点下移
	  que->r_index %= que->Max_Cnt;
	  return 1;
  }else{
	return 0;
  }
}

//清除队列
uint8_t TM_que_ClearAll(TM_QueueObject*que)
{
   if(que->r_index != que->w_index  && (que->r_index !=0) && que->w_index !=0)
   {
       que->r_index  = 0;   //读的位置
       que->w_index  = 0;   //写的位置
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
    TM_que_Regist(&TMQue_WayBill ,Game_WayBillBuf,sizeof(Game_WayBillBuf),60);  //路单命令缓存
}



 
 
/********************************End of File************************************/

