#ifndef _Cmdqueue_H
#define _Cmdqueue_H

#include "stm32f10x.h"

#define MAX_Msg 5  //�����Ϣ����
typedef struct _list
{
  u8 msg[5][50];   //��Ϣ��
  u8 msgLen[10] ;   //��Ϣ��
  u8 msgWn;         //��Ϣдλ��
  u8 msgRn;         //��Ϣ��λ��
  u8 msgCnt;        //ʣ����Ϣ
}ListObject;


void WriteList(ListObject*list, u8 *dat,u8 len );
u8  ReadList(ListObject*list, u8 *dat,u8 *len );












#endif
