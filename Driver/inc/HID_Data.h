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
//数据包提取参数
typedef struct _get_Pack
{
 u8 port;          //提取命令的步骤
 u8 len ;          //提取命令长度预判
 u8 intactPort;    //提取完整步骤
 u8 recordLength;  //不完整长度记录
 u8 changeFlag  ;  //修改标记
}GetPack_MsgType;
//命令提取数组
typedef struct _cmd_Pack{
 u8 Buf[100];
 u8 BufLen ;
}GetPackType;

typedef struct _cmd_Packs{
 u8 *pBuf   ;
 u8 NowLen    ;
 u8 MaxLen ;
}PackType;

//基本玩家参数
typedef struct _POKER
{
  u32 totalScore; //总分
  u32 winScore;   //得分
  u16 spadeBet;   //黑桃押分
  u16 heartBet;   //红桃押分
  u16 clubBet ;	  //草花押分
  u16 diamondBet; //方块押分
  u16 jokerBet;	  //王  押分
}POKER_UserTypde;

extern POKER_UserTypde Player[40];

extern u32_unionType PlayerDifferenceValue[40];

u16 HID_GetPack(GetPack_MsgType*recover,u8 *buf, u8 len, u8 *out, u8 *olen ); //解析命令数据包


//u16 HID_GetOnePack(u8 *buf, u8 len, u8 *out, u8 *olen );       //解析命令
//u16 HID_GetOnePack2_11(u8 *buf, u8 len, u8 *out, u8 *olen );   //解析11命令包
//u16 HID_GetOnePack_19(u8 *buf, u8 len, u8 *out, u8 *olen );    //解析19命令包





//u8 HID_Pack_OneCmd(u8 *buf,u8 len ,u8 *out,u8 *olen);    //解析一个完整的包
void HID_Pack_Deal(u8 *buf,u8 len ,u8 *out,u8 *olen);
void HID_Pack_Deal2(u8 *buf,u8 len ,u8 *out,u8 *olen);
//void MHID_Pack_Deal(u8 *buf,u8 len ,u8 *out,u8 *olen);
u16 ComputeCRC16(unsigned char* buf, int offset, int len); //CRC校验



void  checkFirstFlag(void);               //初始设置
void  clearResult_100bout(void);          //清除100局记录
void  SaveBoutCout(u8 boutCnt);           //保存局数
u8    ReadBoutCout(void);                 //读取局数
void  SaveInningNum(u8 boutCnt);          //保存轮数
u8    ReadInningNum(void);                //读取轮数
void SaveBoutResult(u8 ID,u8 Result);     //保存结果
u8   ReadBoutResult(u8 ID);				  //读取结果

void  SavePlaySingle(u8 boutCnt); //保存补单
u8  ReadPlaySingle(void) ;        //读取补单



void SaveTotalPoints(u8 ID ,u32 realPoints );//保存玩家的总分  [编号,实际总分，虚拟总分]
u32  ReadTotalPoints(u8 ID);                //读取玩家的差值[编号,实际总分，虚拟总分]



void DockingNoteCat_Control(u8 i);//对接通信猫控制

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

