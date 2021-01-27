#include "cmdqueue.h"


//===========================
//      »º´æÈ¥Ð´
//===========================
void WriteList(ListObject*list, u8 *dat,u8 len )
{
  u8 i;
	if( list->msgWn == ( list->msgRn-1 ) || ( list->msgRn == 0 && list->msgWn == ( MAX_Msg-1 ))){
		list->msgRn++;
		list->msgRn %= MAX_Msg;
	}
  for( i=0;i<len;i++)
  {
    list->msg[list->msgWn][i] = dat[i];  
  }
  list->msgLen[list->msgWn] = len;
  if(list->msgCnt < MAX_Msg)
   list->msgCnt++;
	list->msgWn++;
	list->msgWn %= MAX_Msg;
}
//===========================
//      »º´æÈ¥¶Á
//===========================
u8  ReadList(ListObject*list, u8 *dat,u8 *len )
{
  u8 i;
	if( list->msgWn != list->msgRn && list->msgCnt) {
    for(i=0;i<list->msgLen[list->msgRn];i++)
    {
		  dat[i] = list->msg[list->msgRn][i];    
    }
    len[0] = list->msgLen[list->msgRn];
    if(list->msgCnt)list->msgCnt--;
		list->msgRn++;
		list->msgRn %= MAX_Msg;
		return 1;
	}
	return 0;
}
