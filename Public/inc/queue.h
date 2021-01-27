#ifndef _queue_H
#define _queue_H

#include "stm32f10x.h"

typedef struct _Queue{
  u8 * Pbuf;            //队列数组指针
  volatile u16 w_index; //写位置
  volatile u16 r_index; //读位置
  volatile u16 size;    //数组大小
}QueueObject;

// 数组注册为队列
int que_Regist(QueueObject*que,void*bufF,u16 Buf_Size);  //队列注册

u16 que_size(QueueObject que);         //获取队列未读节点大小
void que_Write(QueueObject*que,u8 dat);//队列写尾节点
int que_Read(QueueObject*que);         //队列读头节点
uint8_t que_clearAll(QueueObject*que); //清除队列

extern QueueObject Rx1_que;
extern QueueObject Rx2_que;
extern QueueObject Rx3_que;


extern QueueObject Tx1_que;

#if defined(SPI_SALVE)  //SPI从机
extern QueueObject SalveSPI_Rque;
extern QueueObject SalveSPI_Tque;
#endif
#if defined(SPI_MASTER) //SPI主机
extern QueueObject MasterSPI_Rque;
extern QueueObject MasterSPI_Tque;
#endif
extern QueueObject HID_Tque;


void queue_Init(void); //队列初始化





#endif
