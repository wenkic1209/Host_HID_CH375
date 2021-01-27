/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2021,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : HID_Data.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2021-01-10
* Description  :
* Function List:
  1. ....
      <version>:
        <staff>:
         <data>:
  <description>:
  2. ...

******************************************************************************
*/
/********************************End of Head************************************/
#include "HID_Data.h"
#include "string.h"
#include "public.h"
#include "driver.h"
#include "CH375_App.h"

extern u8 WillResult ;  //操作的结果
u8 LastFlag = 0;
#define CMDSTART1 0x49
#define CMDSTART2 0x43

//typedef struct _get_Pack
//{
// u8 port;
// u8 len ;
//}GetPack_MsgType;

#ifdef CH375_Debug
    #define CH375_Log(...)
    #define CH375_Logn(...)
#else
    #define HID_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
    #define HID_Logn(...)  do{sprint(__VA_ARGS__);}while(0)
#endif

u8 HID_Pack_OneCmd(u8 *buf, u8 len, u8 *out, u8 *olen)
{
    static u8 Port = 0;
    static u8 Plen = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
    for(u8 i = 0; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == CMDSTART1) { //包头字节1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //包头字节2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) {
                    memcpy(out, Buff, blen);
                    olen[0] = blen;
                    Port = 0;
                    blen = 0;
                    return 1;
                }
            }
            break;
        }
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
    return 0;
}

void HID_Pack_Deal(u8 *buf, u8 len, u8 *out, u8 *olen)
{
    static u8 Port = 0;
    static u8 Plen = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
    for(u8 i = 1; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == CMDSTART1) { //包头字节1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //包头字节2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) { Port = 0; }
            }
            break;
        }
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
}

void HID_Pack_Deal2(u8 *buf, u8 len, u8 *out, u8 *olen)
{
    static u8 Port = 0;
    static u8 Plen = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
    for(u8 i = 0; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == CMDSTART1) { //包头字节1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //包头字节2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {
                if(Plen) {Plen--;}
                if(buf[i] == 0xC0) { //排除心跳包 //|| buf[i] ==0x00
                    Port = 0;
                    Plen = 0;
                    if(blen >= 3) {
                        blen -= 3;
                    } else {
                        blen = 0;
                    }
                    break;
                } else {
                    Buff[blen++] = buf[i];
                    Port = 4;
                }
            }
            break;
            case 4: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) { Port = 0; }
            }
            break;
        }
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
}


u16 HID_GetOnePack(u8 *buf, u8 len, u8 *out, u8 *olen )
{
    static u8 Port = 0;
    static u8 Plen = 0;
	u16_unionType temp ;
	u8 i = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
	u8 flag = 0;
	temp.data = 0;
    for( i = 0; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == CMDSTART1) { //包头字节1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //包头字节2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //命令号字节
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                Port = 4;
            }
            break;
            case 4: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) { 
				  Port = 0; 
				  flag = 1; //完整一个包
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
	temp.byte[0] = i;             //读取的位置
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //空包
	}else{
	  if(flag) temp.byte[1]  = 1; //完整的一个包
	  else     temp.byte[1]  = 2; //不完整
	}
	return temp.data;  
}

u16 HID_GetPack(GetPack_MsgType*recover,u8 *buf, u8 len, u8 *out, u8 *olen )
{
    u8 Port = recover->port;
    u8 Plen = recover->len;
	u16_unionType temp ;
	u8 i = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
	u8 flag = 0;
	temp.data = 0;
    for( i = 0; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == CMDSTART1) { //包头字节1
                    Port = 1;
					Plen = 0;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //包头字节2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //命令号字节
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                Port = 4;
            }
            break;
            case 4: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) { 
				  Port = 0; 
				  flag = 1; //完整一个包
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0]      = blen;
	temp.byte[0] = i   ;          //读取的位置
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //空包
	}else{
	  if(flag) temp.byte[1]  = 1; //完整的一个包
	  else     temp.byte[1]  = 2; //不完整
	}
    recover->port = Port;
    recover->len  = Plen; //保留现场
	return temp.data;  
}


u16 HID_GetOnePack2_11(u8 *buf, u8 len, u8 *out, u8 *olen )
{
    static u8 Port = 0;
    static u8 Plen = 0;
	u16_unionType temp ;
	u8 i = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
	u8 flag = 0;
	temp.data = 0;
    for( i = 0; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == CMDSTART1) { //包头字节1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //包头字节2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //命令号字节
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                Port = 4;
            }
            break;
            case 4: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) { 
				  Port = 0; 
				  flag = 1; //完整一个包
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
	temp.byte[0] = i;             //读取的位置
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //空包
	}else{
	  if(flag) temp.byte[1]  = 1; //完整的一个包
	  else     temp.byte[1]  = 2; //不完整
	}
	return temp.data;  
}

u16 HID_GetOnePack_19(u8 *buf, u8 len, u8 *out, u8 *olen )
{
    static u8 Port = 0;
    static u8 Plen = 0;
	u16_unionType temp ;
	u8 i = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
	u8 flag = 0;
	temp.data = 0;
    for( i = 0; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == CMDSTART1) { //包头字节1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //包头字节2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //命令号字节
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                Port = 4;
            }
            break;
            case 4: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) { 
				  Port = 0; 
				  flag = 1; //完整一个包
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
	temp.byte[0] = i;             //读取的位置
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //空包
	}else{
	  if(flag) temp.byte[1]  = 1; //完整的一个包
	  else     temp.byte[1]  = 2; //不完整
	}
	return temp.data;  
}



#define MCMDSTART1 0x50
#define MCMDSTART2 0x43

void MHID_Pack_Deal(u8 *buf, u8 len, u8 *out, u8 *olen)
{
    static u8 Port = 0;
    static u8 Plen = 0;
    u8 Buff[64] = {0};
    u8 blen = 0;
    for(u8 i = 1; i < len; i++) {
        switch(Port) {
            case 0: {
                if(buf[i] == MCMDSTART1) { //包头字节1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == MCMDSTART2) { //包头字节2
                    Buff[blen++] = MCMDSTART1;
                    Buff[blen++] = MCMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //长度
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {
                if(Plen) {Plen--;}
                Buff[blen++] = buf[i];
                if(Plen == 0) { Port = 0; }
            }
            break;
        }
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
}








/// <summary>
/// 计算CRC校验
/// </summary>
/// <param name="buf"></param>
/// <param name="offset"></param>
/// <param name="len"></param>
/// <returns></returns>
u16 ComputeCRC16(unsigned char *buf, int offset, int len)
{
    u16 num = 0;
    for(int i = 0; i < len; i++) {
        unsigned char num3 = buf[i + offset];
        for(unsigned char j = 0; j < 8; j = (unsigned char)(j + 1)) {
            num = (((num3 ^ ((unsigned char)num)) & 1) == 0) ? ((u16)(num >> 1)) : ((u16)((num >> 1) ^ 0xa001));
            num3 = (unsigned char)(num3 >> 1);
        }
    }
    return num;
}


#if 0 //##铁电内存分配
0  测试字节
1  初始化字节
10 - 49  验证区

50 - 8K  数据区
{结果区}
1     目前的轮数
1     目前的轮数
100   100局结果

{30个网络玩家} 30 * 8 = 240
[4字节同步标记]
[4字节实际总分, 4字节虚拟总分]
#endif


//铁电写入一字节
static inline void dev_SaveWriteByte(u32 addr, u8 dat)
{
    FM25WriteString(addr, &dat, 1);
}
//铁电读出一字节
static inline u8 dev_SaveReadByte(u32 addr)
{
    u8 dat = 0;
    FM25ReadString(addr, &dat, 1);
    return dat;
}


#define FIRSTFLAG  0xFE
//初始设置
void checkFirstFlag(void)
{
	if(dev_SaveReadByte(1)!=FIRSTFLAG)
	{
	   dev_SaveWriteByte(1,FIRSTFLAG);
	   clearResult_100bout();             //清除100局记录   
       //playerSetALLSynchronzateFlag();  //设置一下同步标志(默认是同步分数)
	   for(u8 i=0;i<40;i++)
	   {
			SaveTotalPoints(i,0); //默认的差值总分是0
	   }
	   SaveBoutCout(0);           //局数为0 默认
	   SaveInningNum(0);          //轮数为0 默认
	   SavePlaySingle(0);         //轮数为0 默认
	}
	
	u8 temp1 = ReadBoutCout();
	u8 temp2 = ReadInningNum();
	User_Log(">上局的%d轮数 %d局数",temp2,temp1);
	for(u8 i=0;i<40;i++)
	{
	  PlayerDifferenceValue[i].data = ReadTotalPoints(i); //默认的差值总分是0
//	  User_Log("玩家的差值 %lu 0x%04X",PlayerDifferenceValue[i].data,PlayerDifferenceValue[i].data);
	}
	User_Log("开机结果<:");
	for(u8 i=0;i<100;i++)
	{
      HID_Logn("%02d:0x%02X ",i,ReadBoutResult(i+1));
	}
    User_Log("开机结果>");	
}


#define BoutResultAddr     50   //起始地址
#define BoutResultAdd      5    //基础信息偏移
#if 0
0 轮数   1 局数  2 打单标记  (5)
以下是100局结果
[0]---[9]
[0]---[9]
[0]---[9]
#endif
#define MaxBoutResultNum   110  //内存大小
//清除100局记录
void clearResult_100bout(void)
{
	for(u8 i=0;i<100;i++)
	{
		dev_SaveWriteByte(BoutResultAddr+BoutResultAdd+i,0); //默认值
	}
}
void  SaveBoutCout(u8 boutCnt)  //保存局数
{
	dev_SaveWriteByte(BoutResultAddr,boutCnt); //默认值
}
u8  ReadBoutCout(void)          //读取局数
{
	return dev_SaveReadByte(BoutResultAddr);
}

void  SaveInningNum(u8 boutCnt) //保存轮数
{
	dev_SaveWriteByte(BoutResultAddr+1,boutCnt); //默认值
}
u8  ReadInningNum(void)         //读取轮数
{
	return dev_SaveReadByte(BoutResultAddr+1);
}

void  SavePlaySingle(u8 boutCnt) //保存补单
{
	dev_SaveWriteByte(BoutResultAddr+2,boutCnt); //默认值
}
u8  ReadPlaySingle(void)         //读取补单
{
	return dev_SaveReadByte(BoutResultAddr+2);
}



//保存记录
void SaveBoutResult(u8 ID,u8 Result)
{
	if(ID<MaxBoutResultNum)
     dev_SaveWriteByte(BoutResultAddr+BoutResultAdd+ID,Result); 
}
//读取记录
u8 ReadBoutResult(u8 ID)
{
	if(ID<MaxBoutResultNum)
		return dev_SaveReadByte(BoutResultAddr+BoutResultAdd+ID);
     return 0;
}

#define  PlayerSaveAddr     (BoutResultAddr + MaxBoutResultNum) //起始地址
#define  MaxPlayerNum       40                //最多玩家
#define  MaxFlagSync        5                 //同步标记字节 
#define  MaxPlayerSaveNum   (8*MaxPlayerNum) + (MaxFlagSync)  //(30玩家+5字节的同步标志)

//保存玩家的玩家的差值
void SaveTotalPoints(u8 ID ,u32 realPoints)
{
   u32_unionType temp ;
	if(ID<MaxPlayerNum)
	{
		temp.data = realPoints;	
	    for(u8 i=0;i<4;i++)
		{
		   dev_SaveWriteByte(PlayerSaveAddr+MaxFlagSync+ID*8+i,temp.byte[i]);
		}
	}
}
//读取玩家的差值
u32 ReadTotalPoints(u8 ID)
{
   u32_unionType temp ;
	if(ID<MaxPlayerNum)
	{
	    for(u8 i=0;i<4;i++)
		{
		   temp.byte[i] = dev_SaveReadByte(PlayerSaveAddr+MaxFlagSync+ID*8+i);
		}
		return temp.data;
	}
	return 0;
}






//对接通信猫控制
void DockingNoteCat_Control(u8 i)
{	
	switch(i) {
		case 1: {GenerateWillResults();  PrintfFlag = 1;} break;//补单 //生成补单结果  //准备打单
		case 2: {WillResult = 0; } break;//取消所有操作
		case 3: {WillResult = 6; } break;//本局吃大赔小
		case 4: {WillResult = 1; } break;//本局黑桃
		case 5: {WillResult = 2; } break;//本局红桃				
		case 6: {WillResult = 3; } break;//本局梅花
		case 7: {WillResult = 4; } break;//本局方块
		case 8: {WillResult = 5; } break;//本局王				
		case 9: {} break;//查询所有路单差值
		case 10:{
		   for(u8 i=0;i<40;i++)              //同步差值
		   {
				SaveTotalPoints(i,0); //默认的差值总分是0
			    PlayerDifferenceValue[i].data = ReadTotalPoints(i);
		   }
		} break;//清除所有分机差值						
	}


}



/********************************End of File************************************/

