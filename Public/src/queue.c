#include "queue.h"
#include "stdio.h"

/*
V1.0  基本队列实现
V2.0  添加任意数组大小队列注册
*/

//typedef struct _Queue{
//  u8 * Pbuf;            //队列数组指针
//  volatile u16 w_index; //写位置
//  volatile u16 r_index; //读位置
//  volatile u16 size;    //数组大小
//}QueueObject;

/** 
 * \defgroup 队列长度计算
 * \{
 */
 #define FIFO_LEN(w,r,s)       (((w) >= (r)) ? ((w) - (r)) : ((w) + (s) - (r)))  

//  Array  Regist become queue
// 数组注册为队列
int que_Regist(QueueObject*que,void*bufF,u16 Buf_Size)
{
	if(bufF==NULL || Buf_Size == 0)
	{
		return -1;
	}else{
		que->Pbuf = bufF;
		que->r_index = 0;
		que->w_index = 0;
		que->size    = Buf_Size;
        return 0;		
	}
}
//检查队列是否为空
static int Check_empty(QueueObject*que)
{
	return !(que->r_index == que->w_index);
}
//队列大小结算
u16 que_size(QueueObject que)
{
    return FIFO_LEN(que.w_index,que.r_index,que.size);
}

//队列写
void que_Write(QueueObject*que,u8 dat)
{
  if(que->Pbuf == NULL)return;
  if(que_size(que[0])==(que->size-1))
  {
    que->r_index++;      //覆盖最早的队列节点
	que->r_index %= que->size;
  }
  que->Pbuf[que->w_index] = dat;
  que->w_index++;
  que->w_index %= que->size;  
}
//队列读
int que_Read(QueueObject*que)
{
  int dat = 0;
  if(que->Pbuf == NULL)return -1;
  if(Check_empty(que))
  {
	 dat=que->Pbuf[que->r_index]; //获得数据
	 que->r_index++;             //队列节点下移
	 que->r_index %= que->size;
	 return dat;
  }else{
	return -1;
  }
}
//清除队列
uint8_t que_clearAll(QueueObject*que)
{
   if(que->r_index != que->w_index  && (que->r_index !=0) && que->w_index !=0)
   {
       que->r_index  = 0;   //读的位置
       que->w_index  = 0;   //写的位置
       return 1;
   }
   return 0;

}

static u8 Rx1_Buff[300]={0};
QueueObject Rx1_que;

static u8 Rx2_Buff[300]={0};
QueueObject Rx2_que;
static u8 Rx3_Buff[300]={0};
QueueObject Rx3_que;


static u8 Tx1_Buff[300]={0};
QueueObject Tx1_que;

#if defined(SPI_SALVE)
static u8 R_SalveSPIBuff[300]={0};
QueueObject SalveSPI_Rque;
static u8 T_SalveSPIBuff[300]={0};
QueueObject SalveSPI_Tque;
#endif

#if defined(SPI_MASTER)
static u8 R_MasterSPIBuff[2000]={0};
QueueObject MasterSPI_Rque;
static u8 T_MasterSPIBuff[2000]={0};
QueueObject MasterSPI_Tque;
#endif

static u8 HID_TBuff[1000]={0};
QueueObject HID_Tque;



void queue_Init(void)
{
  que_Regist(&Rx1_que       ,Rx1_Buff       ,sizeof(Rx1_Buff));	
  que_Regist(&Rx2_que       ,Rx2_Buff       ,sizeof(Rx2_Buff));		
  que_Regist(&Rx3_que       ,Rx3_Buff       ,sizeof(Rx3_Buff));		
	
  que_Regist(&Tx1_que       ,Tx1_Buff       ,sizeof(Tx1_Buff));	    
  #if defined(SPI_SALVE)	
  que_Regist(&SalveSPI_Rque ,R_SalveSPIBuff ,sizeof(R_SalveSPIBuff ));
  que_Regist(&SalveSPI_Tque ,T_SalveSPIBuff ,sizeof(T_SalveSPIBuff )); 
  #endif  
  #if defined(SPI_MASTER)    
  que_Regist(&MasterSPI_Rque,R_MasterSPIBuff,sizeof(R_MasterSPIBuff));
  que_Regist(&MasterSPI_Tque,T_MasterSPIBuff,sizeof(T_MasterSPIBuff));
  #endif  
  que_Regist(&HID_Tque      ,HID_TBuff      ,sizeof(HID_TBuff)      );    
}
	
#if 0  //测试队列消息转发
       if(que_size(Rx1_que))
	   {
	     Usart_SendByte(COM4,que_Read(&Rx1_que));
	   }
       if(que_size(Rx4_que))
	   {
	     Usart_SendByte(COM1,que_Read(&Rx4_que));
	   }
#endif

