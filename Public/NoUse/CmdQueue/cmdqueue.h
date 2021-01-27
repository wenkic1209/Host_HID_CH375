#ifndef _Cmdqueue_H
#define _Cmdqueue_H

#include "stm32f10x.h"

#define MAX_Msg 5  //最大信息缓存
typedef struct _list
{
  u8 msg[5][50];   //信息体
  u8 msgLen[10] ;   //信息量
  u8 msgWn;         //信息写位置
  u8 msgRn;         //信息读位置
  u8 msgCnt;        //剩余信息
}ListObject;


void WriteList(ListObject*list, u8 *dat,u8 len );
u8  ReadList(ListObject*list, u8 *dat,u8 *len );












#endif
