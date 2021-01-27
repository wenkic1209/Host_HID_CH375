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

extern u8 WillResult ;  //�����Ľ��
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
                if(buf[i] == CMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
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
                if(buf[i] == CMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
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
                if(buf[i] == CMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {
                if(Plen) {Plen--;}
                if(buf[i] == 0xC0) { //�ų������� //|| buf[i] ==0x00
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
                if(buf[i] == CMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //������ֽ�
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
				  flag = 1; //����һ����
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
	temp.byte[0] = i;             //��ȡ��λ��
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //�հ�
	}else{
	  if(flag) temp.byte[1]  = 1; //������һ����
	  else     temp.byte[1]  = 2; //������
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
                if(buf[i] == CMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
					Plen = 0;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //������ֽ�
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
				  flag = 1; //����һ����
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0]      = blen;
	temp.byte[0] = i   ;          //��ȡ��λ��
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //�հ�
	}else{
	  if(flag) temp.byte[1]  = 1; //������һ����
	  else     temp.byte[1]  = 2; //������
	}
    recover->port = Port;
    recover->len  = Plen; //�����ֳ�
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
                if(buf[i] == CMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //������ֽ�
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
				  flag = 1; //����һ����
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
	temp.byte[0] = i;             //��ȡ��λ��
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //�հ�
	}else{
	  if(flag) temp.byte[1]  = 1; //������һ����
	  else     temp.byte[1]  = 2; //������
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
                if(buf[i] == CMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == CMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = CMDSTART1;
                    Buff[blen++] = CMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
                Plen = buf[i] + 1;
                Buff[blen++] = buf[i];
                Port = 3;
            }
            break;
            case 3: {  //������ֽ�
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
				  flag = 1; //����һ����
				}
            }
            break;
        }
		if(flag)break;
    }
    memcpy(out, Buff, blen);
    olen[0] = blen;
	temp.byte[0] = i;             //��ȡ��λ��
	if(blen == 0)
	{
	   temp.byte[1]= 0;           //�հ�
	}else{
	  if(flag) temp.byte[1]  = 1; //������һ����
	  else     temp.byte[1]  = 2; //������
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
                if(buf[i] == MCMDSTART1) { //��ͷ�ֽ�1
                    Port = 1;
                }
            }
            break;
            case 1: {
                if(buf[i] == MCMDSTART2) { //��ͷ�ֽ�2
                    Buff[blen++] = MCMDSTART1;
                    Buff[blen++] = MCMDSTART2;
                    Port = 2;
                    Plen = 0;
                } else {
                    Port = 0;
                }
            }
            break;
            case 2: {                //����
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
/// ����CRCУ��
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


#if 0 //##�����ڴ����
0  �����ֽ�
1  ��ʼ���ֽ�
10 - 49  ��֤��

50 - 8K  ������
{�����}
1     Ŀǰ������
1     Ŀǰ������
100   100�ֽ��

{30���������} 30 * 8 = 240
[4�ֽ�ͬ�����]
[4�ֽ�ʵ���ܷ�, 4�ֽ������ܷ�]
#endif


//����д��һ�ֽ�
static inline void dev_SaveWriteByte(u32 addr, u8 dat)
{
    FM25WriteString(addr, &dat, 1);
}
//�������һ�ֽ�
static inline u8 dev_SaveReadByte(u32 addr)
{
    u8 dat = 0;
    FM25ReadString(addr, &dat, 1);
    return dat;
}


#define FIRSTFLAG  0xFE
//��ʼ����
void checkFirstFlag(void)
{
	if(dev_SaveReadByte(1)!=FIRSTFLAG)
	{
	   dev_SaveWriteByte(1,FIRSTFLAG);
	   clearResult_100bout();             //���100�ּ�¼   
       //playerSetALLSynchronzateFlag();  //����һ��ͬ����־(Ĭ����ͬ������)
	   for(u8 i=0;i<40;i++)
	   {
			SaveTotalPoints(i,0); //Ĭ�ϵĲ�ֵ�ܷ���0
	   }
	   SaveBoutCout(0);           //����Ϊ0 Ĭ��
	   SaveInningNum(0);          //����Ϊ0 Ĭ��
	   SavePlaySingle(0);         //����Ϊ0 Ĭ��
	}
	
	u8 temp1 = ReadBoutCout();
	u8 temp2 = ReadInningNum();
	User_Log(">�Ͼֵ�%d���� %d����",temp2,temp1);
	for(u8 i=0;i<40;i++)
	{
	  PlayerDifferenceValue[i].data = ReadTotalPoints(i); //Ĭ�ϵĲ�ֵ�ܷ���0
//	  User_Log("��ҵĲ�ֵ %lu 0x%04X",PlayerDifferenceValue[i].data,PlayerDifferenceValue[i].data);
	}
	User_Log("�������<:");
	for(u8 i=0;i<100;i++)
	{
      HID_Logn("%02d:0x%02X ",i,ReadBoutResult(i+1));
	}
    User_Log("�������>");	
}


#define BoutResultAddr     50   //��ʼ��ַ
#define BoutResultAdd      5    //������Ϣƫ��
#if 0
0 ����   1 ����  2 �򵥱��  (5)
������100�ֽ��
[0]---[9]
[0]---[9]
[0]---[9]
#endif
#define MaxBoutResultNum   110  //�ڴ��С
//���100�ּ�¼
void clearResult_100bout(void)
{
	for(u8 i=0;i<100;i++)
	{
		dev_SaveWriteByte(BoutResultAddr+BoutResultAdd+i,0); //Ĭ��ֵ
	}
}
void  SaveBoutCout(u8 boutCnt)  //�������
{
	dev_SaveWriteByte(BoutResultAddr,boutCnt); //Ĭ��ֵ
}
u8  ReadBoutCout(void)          //��ȡ����
{
	return dev_SaveReadByte(BoutResultAddr);
}

void  SaveInningNum(u8 boutCnt) //��������
{
	dev_SaveWriteByte(BoutResultAddr+1,boutCnt); //Ĭ��ֵ
}
u8  ReadInningNum(void)         //��ȡ����
{
	return dev_SaveReadByte(BoutResultAddr+1);
}

void  SavePlaySingle(u8 boutCnt) //���油��
{
	dev_SaveWriteByte(BoutResultAddr+2,boutCnt); //Ĭ��ֵ
}
u8  ReadPlaySingle(void)         //��ȡ����
{
	return dev_SaveReadByte(BoutResultAddr+2);
}



//�����¼
void SaveBoutResult(u8 ID,u8 Result)
{
	if(ID<MaxBoutResultNum)
     dev_SaveWriteByte(BoutResultAddr+BoutResultAdd+ID,Result); 
}
//��ȡ��¼
u8 ReadBoutResult(u8 ID)
{
	if(ID<MaxBoutResultNum)
		return dev_SaveReadByte(BoutResultAddr+BoutResultAdd+ID);
     return 0;
}

#define  PlayerSaveAddr     (BoutResultAddr + MaxBoutResultNum) //��ʼ��ַ
#define  MaxPlayerNum       40                //������
#define  MaxFlagSync        5                 //ͬ������ֽ� 
#define  MaxPlayerSaveNum   (8*MaxPlayerNum) + (MaxFlagSync)  //(30���+5�ֽڵ�ͬ����־)

//������ҵ���ҵĲ�ֵ
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
//��ȡ��ҵĲ�ֵ
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






//�Խ�ͨ��è����
void DockingNoteCat_Control(u8 i)
{	
	switch(i) {
		case 1: {GenerateWillResults();  PrintfFlag = 1;} break;//���� //���ɲ������  //׼����
		case 2: {WillResult = 0; } break;//ȡ�����в���
		case 3: {WillResult = 6; } break;//���ֳԴ���С
		case 4: {WillResult = 1; } break;//���ֺ���
		case 5: {WillResult = 2; } break;//���ֺ���				
		case 6: {WillResult = 3; } break;//����÷��
		case 7: {WillResult = 4; } break;//���ַ���
		case 8: {WillResult = 5; } break;//������				
		case 9: {} break;//��ѯ����·����ֵ
		case 10:{
		   for(u8 i=0;i<40;i++)              //ͬ����ֵ
		   {
				SaveTotalPoints(i,0); //Ĭ�ϵĲ�ֵ�ܷ���0
			    PlayerDifferenceValue[i].data = ReadTotalPoints(i);
		   }
		} break;//������зֻ���ֵ						
	}


}



/********************************End of File************************************/

