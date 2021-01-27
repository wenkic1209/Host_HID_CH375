#ifndef _queue_H
#define _queue_H

#include "stm32f10x.h"

typedef struct _Queue{
  u8 * Pbuf;            //��������ָ��
  volatile u16 w_index; //дλ��
  volatile u16 r_index; //��λ��
  volatile u16 size;    //�����С
}QueueObject;

// ����ע��Ϊ����
int que_Regist(QueueObject*que,void*bufF,u16 Buf_Size);  //����ע��

u16 que_size(QueueObject que);         //��ȡ����δ���ڵ��С
void que_Write(QueueObject*que,u8 dat);//����дβ�ڵ�
int que_Read(QueueObject*que);         //���ж�ͷ�ڵ�
uint8_t que_clearAll(QueueObject*que); //�������

extern QueueObject Rx1_que;
extern QueueObject Rx2_que;
extern QueueObject Rx3_que;


extern QueueObject Tx1_que;

#if defined(SPI_SALVE)  //SPI�ӻ�
extern QueueObject SalveSPI_Rque;
extern QueueObject SalveSPI_Tque;
#endif
#if defined(SPI_MASTER) //SPI����
extern QueueObject MasterSPI_Rque;
extern QueueObject MasterSPI_Tque;
#endif
extern QueueObject HID_Tque;


void queue_Init(void); //���г�ʼ��





#endif
