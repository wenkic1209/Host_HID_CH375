/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : HID_Data.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-10         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _HID_Data_H
#define _HID_Data_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f10x.h"
#include "public.h"
#include "driver.h"
//���ݰ���ȡ����
typedef struct _get_Pack
{
 u8 port;          //��ȡ����Ĳ���
 u8 len ;          //��ȡ�����Ԥ��
 u8 intactPort;    //��ȡ��������
 u8 recordLength;  //���������ȼ�¼
 u8 changeFlag  ;  //�޸ı��
}GetPack_MsgType;
//������ȡ����
typedef struct _cmd_Pack{
 u8 Buf[100];
 u8 BufLen ;
}GetPackType;

typedef struct _cmd_Packs{
 u8 *pBuf   ;
 u8 NowLen    ;
 u8 MaxLen ;
}PackType;

//������Ҳ���
typedef struct _POKER
{
  u32 totalScore; //�ܷ�
  u32 winScore;   //�÷�
  u16 spadeBet;   //����Ѻ��
  u16 heartBet;   //����Ѻ��
  u16 clubBet ;	  //�ݻ�Ѻ��
  u16 diamondBet; //����Ѻ��
  u16 jokerBet;	  //��  Ѻ��
}POKER_UserTypde;

extern POKER_UserTypde Player[40];

extern u32_unionType PlayerDifferenceValue[40];

u16 HID_GetPack(GetPack_MsgType*recover,u8 *buf, u8 len, u8 *out, u8 *olen ); //�����������ݰ�


//u16 HID_GetOnePack(u8 *buf, u8 len, u8 *out, u8 *olen );       //��������
//u16 HID_GetOnePack2_11(u8 *buf, u8 len, u8 *out, u8 *olen );   //����11�����
//u16 HID_GetOnePack_19(u8 *buf, u8 len, u8 *out, u8 *olen );    //����19�����





//u8 HID_Pack_OneCmd(u8 *buf,u8 len ,u8 *out,u8 *olen);    //����һ�������İ�
void HID_Pack_Deal(u8 *buf,u8 len ,u8 *out,u8 *olen);
void HID_Pack_Deal2(u8 *buf,u8 len ,u8 *out,u8 *olen);
//void MHID_Pack_Deal(u8 *buf,u8 len ,u8 *out,u8 *olen);
u16 ComputeCRC16(unsigned char* buf, int offset, int len); //CRCУ��



void  checkFirstFlag(void);               //��ʼ����
void  clearResult_100bout(void);          //���100�ּ�¼
void  SaveBoutCout(u8 boutCnt);           //�������
u8    ReadBoutCout(void);                 //��ȡ����
void  SaveInningNum(u8 boutCnt);          //��������
u8    ReadInningNum(void);                //��ȡ����
void SaveBoutResult(u8 ID,u8 Result);     //������
u8   ReadBoutResult(u8 ID);				  //��ȡ���

void  SavePlaySingle(u8 boutCnt); //���油��
u8  ReadPlaySingle(void) ;        //��ȡ����



void SaveTotalPoints(u8 ID ,u32 realPoints );//������ҵ��ܷ�  [���,ʵ���ܷ֣������ܷ�]
u32  ReadTotalPoints(u8 ID);                //��ȡ��ҵĲ�ֵ[���,ʵ���ܷ֣������ܷ�]



void DockingNoteCat_Control(u8 i);//�Խ�ͨ��è����

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

