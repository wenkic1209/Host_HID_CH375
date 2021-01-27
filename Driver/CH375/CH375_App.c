/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2021,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : CH375_App.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2021-01-09
* Description  :
* Function List:
******************************************************************************
*/
/********************************End of Head************************************/
#include "CH375_App.h"
#include "debug.h"
#include "driver.h"
#include "public.h"
#include "MyHID_Driver.h"
#include "string.h"
#include "printer.h"

extern u8 WillResult ;  //�����Ľ��

#define StartID  10
#define EndID    39

//#define CH375_Debug

#ifdef CH375_Debug
    #define CH375_Log(...)
    #define CH375_Logn(...)
#else
    #define CH375_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
    #define CH375_Logn(...)  do{sprint(__VA_ARGS__);}while(0)
#endif

//typedef struct _POKER
//{
//  u32 totalScore;   //�ܷ�
//  u32 winScore;     //�÷�
//  u16 spadeBet;     //����Ѻ��
//  u16 heartBet;     //����Ѻ��
//  u16 clubBet ;     //�ݻ�Ѻ��
//  u16 diamondBet;   //����Ѻ��
//  u16 jokerBet;     //��  Ѻ��
//}POKER_UserTypde;
const POKER_UserTypde PlayerNull = {0, 0, 0, 0, 0, 0, 0};
POKER_UserTypde       Player[40] = {0}; //40����ұ����ֲ���

u32_unionType PlayerDifferenceValue[40] = {0}; //40�����

//���㱶�ʵ÷�(���ݽ��)
u32 CalculateTheMagnificationScore(POKER_UserTypde temp, u8 newResult)
{
    u32 Score = 0;
    if(newResult == 0x60 || newResult == 0x50) {      //���÷ּ���=20����*ѹ��+����4��ѹ��
        Score  = 20 * temp.jokerBet + temp.spadeBet + temp.heartBet + temp.clubBet  + temp.diamondBet;
    } else if((newResult >> 4) == 1) { //3.8����
        Score  = 3.8 * temp.spadeBet;
    } else if((newResult >> 4) == 2) { //3.8����
        Score  = 3.8 * temp.heartBet;
    } else if((newResult >> 4) == 3) { //2����
        Score  = 4 * temp.clubBet;
    } else if((newResult >> 4) == 4) { //2����
        Score  = 4 * temp.diamondBet;
    }
    return Score;
}

u8 Calculate_ZeroCnt(u8 *dat, u8 len)
{
    u8 temp = 0;
    for(u8 i = 0; i < len; i++) {
        if(dat[i] == 1) {
            temp++;
        }
    }
    return temp;
}

//����Դ���С���Ľ��
u8  Calculate_EatBig_LoseSmall(void)
{
    u32 Fraction[5] = {0};
    u32 FraTable[5] = {0};
    u8  flag[5]    = {0}; //������
    for(u8 i = StartID; i <= EndID; i++) {
        Fraction[0] += CalculateTheMagnificationScore(Player[i], 0x50); //����   ����÷�
        Fraction[1] += CalculateTheMagnificationScore(Player[i], 0x10); //������ ����÷�
        Fraction[2] += CalculateTheMagnificationScore(Player[i], 0x20); //������ ����÷�
        Fraction[3] += CalculateTheMagnificationScore(Player[i], 0x30); //���ݻ� ����÷�
        Fraction[4] += CalculateTheMagnificationScore(Player[i], 0x40); //������ ����÷�
    }
    //���
    for(u8 i = 0; i < 5; i++) {
        if(Fraction[i] == 0) {
            flag[i] = 1;
        }
        FraTable[i] = Fraction[i];
    }
    u8 temp = Calculate_ZeroCnt(flag, 5); //�����0
    sort_u32(FraTable, 5); //��������
    if(temp == 0) { //��0ֵ,��Ҫ����
        for(u8 i = 0; i < 5; i++) {
            if(FraTable[0] == Fraction[i]) {
                if(i == 0) {
                    return 6; //����
                } else {
                    return i; //����ͨ��
                }
            }
        }
    } else {    //���
        temp = rand() % temp + 1;
        for(u8 i = 0, m = 0; i < 5; i++) {
            if(flag[i]) {
                m++;
                if(m == temp) {
                    if(i == 0) {
                        return 6;
                    } else {
                        return i;
                    }
                }
            }
        }
    }
    CH375_Log(">�Դ���С �������");
    return 1;
}



//���㱾�ֵĲ�ֵ                       (ԭ��ֵ ,���ֵĲ�ֵ)
u32 CalculateTheDifference(u32 DifferencePoints, int32_t PoorInGame)
{
    u32 temp = DifferencePoints; //ԭ��ֵ
    if(PoorInGame) { //���ֿ���
        if(temp & 0x80000000) {      //ԭ�Ǹ���
            u32 pointTemp = temp & 0x7FFFFFFF; //�õ�ԭ��ֵ
            if(pointTemp > PoorInGame) {
                temp = pointTemp - PoorInGame;
                temp |= 0x80000000;
            } else {                 //ԭ������
                temp = PoorInGame - pointTemp;
            }
        } else {
            temp   += PoorInGame;
        }
    } else {                         //���ֿ�Ӯ
        if(temp & 0x80000000) { //ԭ�Ǹ���
            u32 pointTemp   = temp & 0x7FFFFFFF; //�õ�ԭ��ֵ
            u32 nwPointDiff = labs(PoorInGame);
            temp  = pointTemp + nwPointDiff;
            temp |= 0x80000000;
        } else {
            u32 pointTemp   = temp ; //�õ�ԭ��ֵ
            u32 nwPointDiff = labs(PoorInGame);
            if(pointTemp < PoorInGame) {
                temp = PoorInGame - pointTemp;
                temp |= 0x80000000;
            } else {                 //ԭ������
                temp =  pointTemp - PoorInGame;
            }
        }
    }
    return temp;
}

//��ò�ֵ����ܷ�
u32 TotalScoreAfterGettingTheDifference(u32 totalScore, u32 PointDiff)
{
    if(PointDiff & 0x80000000) { //����
        return totalScore += (PointDiff & 0x7FFFFFFF); //��ֵ����
    } else {                   //����
        return totalScore -= PointDiff;               //��ֵ����
    }
}

TM_Timer debug_CHms     ; //��ʱ��
u8  desPassWord[8] = {0}; //��Կ
u8  desBuff[8]     = {0}; //��Կ
u8 resultSave[100] = {0};

const u8 CmdPackStart[][4] = {
    {0x49, 0x43, 0x67, 0xA0}, //<0>100��·��
    {0x49, 0x43, 0x0E, 0xA2}, //<1>����--�õ����κ;���
    {0x49, 0x43, 0x10, 0xA3}, //<2>���ƽ��
    {0x49, 0x43, 0x15, 0x11}, //<3>����Ѻ�ַ���(�ı�ֻ���ǰ�ܷ�)
    {0x49, 0x43, 0x15, 0x19}, //<4>����Ѻ�ַ���(����ʱ�������������Ѻ��)(�ı�ֻ���ǰ�ܷ�)(�Լ�����ѹ�ּ�����)
    {0x49, 0x43, 0x02, 0xB0}, //<5>��ʼ��ע����
    {0x49, 0x43, 0x04, 0xB1}, //<6>����ʱ
    {0x49, 0x43, 0x02, 0xB2}, //<7>ֹͣ��ע����
    {0x49, 0x43, 0x03, 0xB3}, //<8>�����н���
    {0x49, 0x43, 0x03, 0xB4}, //<9>�����н�
};

u8 GetPackPort    = 0;
u8 GetPackPort_11 = 0;
u8 GetPackPort_19 = 0;
GetPackType Pack_11 = {{0}, 0};
GetPackType Pack_19 = {{0}, 0};
GetPackType Pack_A3 = {{0}, 0};
GetPack_MsgType GetPackMsg_A0    = {0, 0, 0}; //��ȡ����
GetPack_MsgType GetPackMsg_11    = {0, 0, 0}; //��ȡ����
GetPack_MsgType GetPackMsg_19    = {0, 0, 0}; //��ȡ����
GetPack_MsgType GetPackMsg_A2    = {0, 0, 0}; //��ȡ����
GetPack_MsgType GetPackMsg_A3    = {0, 0, 0}; //��ȡ����
GetPack_MsgType GetPackMsg_state = {0, 0, 0}; //��ȡ����

GetPack_MsgType *pPackMsg[] = {&GetPackMsg_A0, &GetPackMsg_11, &GetPackMsg_19, &GetPackMsg_A2, &GetPackMsg_A3};
u8 WillResultFlag        = 0; //�����־
u8 CalculationResultFlag = 0; //�����־
u8 ResultFlag            = 0; //�����־

uint8_t  ache_Buff[2][64];  //���������

void ReportPack_encrypt(u8 *Buf, u8 len); //���ݰ��ټ���()

inline u8 checkExtractionFlag(void)
{
    for(u8 i = 0; i < sizeof(pPackMsg) / sizeof(GetPack_MsgType *); i++) { //������ݰ��Ƿ��в�������
        if(pPackMsg[i]->intactPort) {
            return 1;
        }
    }
    return 0;
}

//����ʱ�������Ǹı���������
void controlTheForwardingProcess(u8 *dat, u8 len)
{
    static u8 Port    = 0;
    static u8 lengthIndex = 0;
    u8 temp = checkExtractionFlag(); //����Ƿ��в������İ�
    if(temp) {
        if(Port == 0) {
            Port = 1;       //��һ�β�����
        } else if(Port == 1) {
            Port = 2;       //�ڴβ�����
        }
    } else {
        Port = 0; //�����������ݰ�
    }
    switch(Port) {
        case 0: { //����ȫ������
            if(lengthIndex == 1) {
                lengthIndex = 0;
                ReportPack_encrypt(ache_Buff[0], 60); //�����ڼ���
                HID_Send_Report(ache_Buff[0], 60);   //��������
                memcpy(ache_Buff[0], dat, 60);       //�����µ�����
                ReportPack_encrypt(ache_Buff[0], 60); //�����ڼ���
                HID_Send_Report(ache_Buff[0], 60);   //��������
            } else {
                memcpy(ache_Buff[0], dat, 60);       //�����µ�����
                ReportPack_encrypt(ache_Buff[0], 60); //�����ڼ���
                HID_Send_Report(ache_Buff[0], 60);   //��������
            }
        }
        break;
        case 1: { //�����������
            lengthIndex = 1;
            memcpy(ache_Buff[0], dat, 60);
        }
        break;
        case 2: { //����ǰ������,�������ǰ�ƶ�
            lengthIndex = 1;
            ReportPack_encrypt(ache_Buff[0], 60); //�����ڼ���
            HID_Send_Report(ache_Buff[0], 60);   //��������
            memcpy(ache_Buff[0], dat, 60);       //�����µ�����
        }
        break;
    }
}


//������������У��
void commandDataReCRC(u8 *dat, u8 len)
{
    u16 CRC12 = ComputeCRC16(dat, 2, dat[2]);
    dat[len - 2] = CRC12 >> 8 & 0xFF;
    dat[len - 1] = CRC12 >> 0 & 0xFF;
}
//�������ݴ�ӡ
void LogStr(char *logName, u8 *dat, u8 len)
{
    CH375_Logn("%s >", logName);
    for(u8 i = 0; i < len; i++)
    { CH375_Logn("%02x ", (uint16_t)dat[i]); }
    CH375_Logn("\r\n");
}

USB_DEV_DESCR HostDevDesc;
u8 CheckNullPack(u8 *dat, u16 len)
{
    while(len--) {
        if(*dat) { return 1; }
        dat++;
    }
    return 0;
}

/*USB��׼�豸����������*/
void  USBH_ParseDevDesc(USB_DEV_DESCR *dev_desc,
                        uint8_t *buf,
                        uint16_t length)
{
    dev_desc->bLength            = *(uint8_t *)(buf +  0);
    dev_desc->bDescriptorType    = *(uint8_t *)(buf +  1);
    dev_desc->bcdUSB             = LE16(buf +  2);
    dev_desc->bDeviceClass       = *(uint8_t *)(buf +  4);
    dev_desc->bDeviceSubClass    = *(uint8_t *)(buf +  5);
    dev_desc->bDeviceProtocol    = *(uint8_t *)(buf +  6);
    dev_desc->bMaxPacketSize0    = *(uint8_t *)(buf +  7);
    CH375_Log(">DevDesc_1 %02X %02X %04X %02X %02X %02X %02X", dev_desc->bLength,
              dev_desc->bDescriptorType,
              dev_desc->bcdUSB,
              dev_desc->bDeviceClass,
              dev_desc->bDeviceSubClass,
              dev_desc->bDeviceProtocol,
              dev_desc->bMaxPacketSize0);
    if(length > 8) {
        /* For 1st time after device connection, Host may issue only 8 bytes for
        Device Descriptor Length  */
        dev_desc->idVendor           = LE16(buf +  8);
        dev_desc->idProduct          = LE16(buf + 10);
        dev_desc->bcdDevice          = LE16(buf + 12);
        dev_desc->iManufacturer      = *(uint8_t *)(buf + 14);
        dev_desc->iProduct           = *(uint8_t *)(buf + 15);
        dev_desc->iSerialNumber      = *(uint8_t *)(buf + 16);
        dev_desc->bNumConfigurations = *(uint8_t *)(buf + 17);
        CH375_Log(">DevDesc_2 %04X %04X %04X %02X %02X %02X %02X", dev_desc->idVendor,
                  dev_desc->idProduct,
                  dev_desc->bcdDevice,
                  dev_desc->iManufacturer,
                  dev_desc->iProduct,
                  dev_desc->iSerialNumber,
                  dev_desc->bNumConfigurations);
    }
}
/*CH375 �������ݰ�*/
void HostHID_SendReport(u8 *Buf, u8 len)
{
    u8 res = 0;
    u8 cnt = 0;
    if(USBD.status == USBD_READY) {
HostErr:
        if(USBD.itf[0].edp[0].edpnum == 0x01) {
            res = CH375OutTrans(USBD.itf[0].edp[0].edpnum & 0x0F, Buf, 60, 0);  //�Զ˵㷢IN��,NAK������
            if(res != USB_INT_SUCCESS) {
                cnt++;
                if(cnt < 3) {
                    goto HostErr;
                } else {
                    CH375_Log("trans Err1 %02X", res);
                    USBD.status = UNCONNECTED;
                }
            }
        } else if(USBD.itf[0].edp[1].edpnum == 0x01) {
            res = CH375OutTrans(USBD.itf[0].edp[1].edpnum & 0x0F, Buf, 60, 0);  //�Զ˵㷢IN��,NAK������
            if(res != USB_INT_SUCCESS) {
                cnt++;
                if(cnt < 10) {
                    delay_us(100);
                    goto HostErr;
                } else {
                    CH375_Log("trans Err2 %02X", res);
                    USBD.status = UNCONNECTED;
                }
            }
        }
    }
}

//���ݰ��ټ���()
void ReportPack_encrypt(u8 *Buf, u8 len)
{
    u8 Key[8] = {0};
    if(len <= 60) {
        Key[0] = Buf[56];
        Key[1] = Buf[57];
        Key[2] = Buf[58];
        Key[3] = Buf[59];
        Key[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
        Key[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
        Key[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
        Key[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
        //���ܼ���
        for(u8 i = 0; i < 7; i++) {
            des(&Buf[i * 8], Key, DES_Encrypt, &Buf[i * 8]);
        }
    }
}

//������ݺϳ�С������
u32 bigendianSyntheticData(u8 *dat, u8 size)
{
    u32 temp = 0;
    u32 tdat = 0;
    for(size_t i = 0; i < size; i++) {
        tdat = *dat++;
        temp |= tdat << (size - 1 - i) * 8;
    }
    return temp;
}
//С�����ַֽ�������
void bigendDataFormDecomposition(u32 dat, u8 *str, u8 size)
{
    for(size_t i = 0; i < size; i++) {
        str[i] = dat >> ((size - 1 - i) * 8) & 0xFF;
    }
}
//��ұ��ֲ�����ӡ
void  POKER_UserTypdePrintf(u8 *str, POKER_UserTypde temp)
{
//    CH375_Logn(">%s", str);
//    CH375_Log("�ܷ� %8lu �÷� %8lu  <��>%4d <��>%4d <÷>%4d <��>%4d <��>%4d", temp.totalScore,
//              temp.winScore,
//              temp.spadeBet,
//              temp.heartBet,
//              temp.clubBet,
//              temp.diamondBet,
//              temp.jokerBet);
}
//����(��˴���)�ϳ�����(С�˴���)
void  arrayCompositeData(POKER_UserTypde *temp, u8 *dat)
{
    temp->totalScore = bigendianSyntheticData(&dat[0 ], sizeof(u32)); //��˵����ݺϳ�
    temp->winScore   = bigendianSyntheticData(&dat[4 ], sizeof(u32));
    temp->spadeBet   = bigendianSyntheticData(&dat[8 ], sizeof(u16)); //��˵����ݺϳ�
    temp->heartBet   = bigendianSyntheticData(&dat[10], sizeof(u16));
    temp->clubBet    = bigendianSyntheticData(&dat[12], sizeof(u16)); //��˵����ݺϳ�
    temp->diamondBet = bigendianSyntheticData(&dat[14], sizeof(u16));
    temp->jokerBet   = bigendianSyntheticData(&dat[16], sizeof(u16)); //��˵����ݺϳ�
}
//����(С�˴���)�ϳ�����(��˴���)
void dataSynthesisArray(POKER_UserTypde temp, u8 *dat)
{
    bigendDataFormDecomposition(temp.totalScore, &dat[0 ], sizeof(u32));
    bigendDataFormDecomposition(temp.winScore, &dat[4 ], sizeof(u32));
    bigendDataFormDecomposition(temp.spadeBet, &dat[8 ], sizeof(u16));
    bigendDataFormDecomposition(temp.heartBet, &dat[10], sizeof(u16));
    bigendDataFormDecomposition(temp.clubBet, &dat[12], sizeof(u16));
    bigendDataFormDecomposition(temp.diamondBet, &dat[14], sizeof(u16));
    bigendDataFormDecomposition(temp.jokerBet, &dat[16], sizeof(u16));
}
typedef struct _StartMsg {
    uint8_t  inningNum;  //����
    uint8_t  BoutNum  ;  //����
} StartMsgType;
StartMsgType StartMsg; //������Ϣ
typedef struct _ResultMsg {
    uint8_t  Result;        //ԭ���
    uint8_t  ChangeResult;  //�ı��Ľ��
} ResultMsgType;
ResultMsgType ResultMsg; //������Ϣ
uint8_t  A0_Buff[2][60];    //��������
uint8_t changeCommandFlag = 0; //�ı������־
uint8_t UserBuff[100];
//uint8_t DesOutBuff[256];
//uint8_t DesOutBuffLen;
//uint8_t oneCmdBuff[255]; //���泤������
//uint8_t oneCmdBuffLen;
//uint8_t Cmd_11_Buff[255]; //���泤������
//uint8_t Cmd_11_BuffLen;

PackType CmdPack_A0;


extern u8 WillResultFlag;
//����ʣ����
void GenerateWillResults(void)
{
   u8 res;
   u8  result = 0;
	if(ReadPlaySingle()==0)
	{
		CH375_Log("·�����1:");
		for(u8 i=WillResultFlag?StartMsg.BoutNum+1:StartMsg.BoutNum;i<=100;i++)
	   {
		   /*1 2 3 4 5 6*/
		   res = rand()%6+1;
		   result = res<<4;
		   if(res<=4){
			  result |= (rand()%0x0D+1);
		   }
		   CH375_Logn("%02d:0x%02X ",i,result);
		   SaveBoutResult(i,result);
	   }	
	   CH375_Log("·�����2");
	   SavePlaySingle(1); //�Ѳ���
	}
}



//����Ƿ������޸Ľ��
u8 CheckAllowChangeResult(u8 *dat, u8 len)
{
    for(u8 i = 0; i < 14; i++) {
        if(i == 7) {
        } else if(dat[4 + i]) {
            return 0;
        }
    }
    return 1;
}
char *ResultStrGB[] = {"��", "��", "÷", "��", "��"};
//����·���������(100�ֵ�·��)
void DealWithRoadOrderResult_A0(u8 *dat, u8 len)
{
    u8 boutCnt = ReadBoutCout(); //��ȡ����
	u8 inning  = ReadInningNum();
    u8 flag = 0;
    u8 Cnt  = 0;
	CH375_Log(">>>>>>>>>>");
	if(ReadPlaySingle()==0){
		if(boutCnt && inning==dat[4]) {
			for(u8 i = 0 ,m = 0; i < boutCnt; i++) {    //�滻���
				m++;
				u8 temp = ReadBoutResult(i + 1);
				CH375_Logn("%02X ",temp);			
				if(m % 10 == 0) {
					CH375_Logn("\r\n");
				}
				
				if(dat[5 + i] != temp) {
					dat[5 + i] = temp;
					flag = 1;
				}
			}
		}	
	}else {
		if(inning==dat[4]) { //����Ҫ����
			for(u8 i = 0 ,m = 0; i < 100; i++) {    //100��ȫ�滻���
				m++;
				u8 temp = ReadBoutResult(i + 1);
				CH375_Logn("%02X ",temp);			
				if(m % 10 == 0) {
					CH375_Logn("\r\n");
				}
				if(dat[5 + i] != temp) {
					dat[5 + i] = temp;
					flag = 1;
				}
			}
		}	
	
	}

	CH375_Logn("\r\n");
	CH375_Log(">>>>>>>>>>");
    if(flag) {
		CH375_Log("A0 ����У��");
        commandDataReCRC(dat, len);          //����У��
        GetPackMsg_A0.changeFlag = 1;
    }
    CH375_Log(">��%d��·��", dat[4]);
    CH375_Log(">>>>>>>>>>");
    for(u8 m = 0; m < 100; m++) {
        Cnt++;
        u8 temp = dat[5 + m] >> 4;
        if(temp && temp <= 4) {
            CH375_Logn("%s ", ResultStrGB[temp - 1]);
        } else if(temp == 6 || temp == 5) {
            CH375_Logn("%s ", ResultStrGB[4]);
        } else {
            CH375_Logn("%02X ", dat[5 + m]);
        }
        if(Cnt % 10 == 0) {
            CH375_Logn("\r\n");
        }
    }
    CH375_Logn("\r\n");
    CH375_Log("<<<<<<<<<");
    changeCommandFlag = 1;
}
//����������
void DealWithStartOrder_A2(u8 *dat, u8 len)
{
	WillResultFlag = 0;
    StartMsg.inningNum = dat[4]; //����
    StartMsg.BoutNum   = dat[5]; //����
	if(StartMsg.BoutNum==1){
	  SavePlaySingle(0);  //�������
		for(u8 i = 0; i < 100; i++) { //�������Ѻ��
			SaveBoutResult(i+1,0);
		}	
	}
	
    for(u8 i = 0; i < 40; i++) { //�������Ѻ��
        Player[i] = PlayerNull;
    }
}
//����������(���ƽ��)
void DealWithResultOrder_A3(u8 *dat, u8 len)
{
	if(ReadPlaySingle())CH375_Logn("�Ѳ��� %02X %02X ",ReadBoutResult(StartMsg.BoutNum),dat[11]);
	else CH375_Logn("δ���� %02X ",dat[11]);
    ResultMsg.Result = dat[11];                        //��¼ԭ�����
    if(CheckAllowChangeResult(dat, len)) {
           if(WillResult) {        //��Ҫ�ж��Ƿ�����⽱,�Ѿ�������
            if(WillResult == 6) { 
                u8 temp = Calculate_EatBig_LoseSmall();//���ֳԴ���С
                if(temp == 6) {    //���ֳ���
                    if(rand() % 2 == 1) {
                        dat[11] = 0x60;
                    } else {
                        dat[11] = 0x50;
                    }
                } else if(dat[11] == 0x60 || dat[11] == 0x50) {  //���ֳ�����,��Ϊ������
                    dat[11] = temp << 4 | ((rand() % 0x0D) + 1); //�޸Ľ��
                } else {
                    dat[11] = (dat[11] & 0x0F) | temp << 4;      //�޸Ľ��
                }
            } else if(WillResult == 5) {  //���ֳ���
                if(rand() % 2 == 1) {
                    dat[11] = 0x60;
                } else {
                    dat[11] = 0x50;
                }
            } else if((dat[11] == 0x60 || dat[11] == 0x50) && WillResult<=4 && WillResult) {        //���ֳ�����,��Ϊ������
                dat[11] = WillResult << 4 | ((rand() % 0x0D) + 1); //�޸Ľ��
            } else if(WillResult && WillResult<=4){
                dat[11] = (dat[11] & 0x0F) |  WillResult << 4;      //�޸Ľ��
            }
            GetPackMsg_A3.changeFlag = 1;
        }else if(ReadPlaySingle()){  //������Ľ��
			dat[11] = ReadBoutResult(StartMsg.BoutNum);	
			GetPackMsg_A3.changeFlag = 1;
	     }
		if(GetPackMsg_A3.changeFlag)
		{
			commandDataReCRC(dat, len);          //����У��
			changeCommandFlag = 1;
		}
		 
    } else {
		if(ReadPlaySingle())SaveBoutResult(StartMsg.BoutNum,dat[11]); //����������������������ɵĽ��
//        if(WillResult) { CH375_Log(">���������⽱ �������� %d", WillResult); }
    }
	CH375_Log(" %02X \r\n",dat[11]);	
    WillResult = 0;
	SaveInningNum(StartMsg.inningNum);         //��������
    SaveBoutCout(StartMsg.BoutNum);            //�������
    SaveBoutResult(StartMsg.BoutNum, dat[11]); //������
    ResultMsg.ChangeResult = dat[11];          //�޸ĺ�Ľ��
	WillResultFlag = 1;
}

//����ѹ������11
void DealWithStakeOrder_11(u8 *dat, u8 len)
{
    u8 ID         = dat[4];      //�û�
    u32 pointDiff = 0;           //��ʱ��ֵ
    POKER_UserTypde User11_temp; //��ʱѺ�ֲ���
    if(ID >= StartID && ID <= EndID) {
        //LogStr("11����0 ǰ", TempBuff, TempBuffLen);                    //��ӡ����
        pointDiff = ReadTotalPoints(ID);                                  //��ȡ��ֵ
        arrayCompositeData(&User11_temp, &dat[5]);                        //�������ݲ���
//        CH375_Log("11 ���� �ֻ� %d  ��ֵ=>%lu ʵ���ܷ� %lu", ID, pointDiff, User11_temp.totalScore);
//        //POKER_UserTypdePrintf((u8 *)"11 ����ǰ", User11_temp) ;         //��ӡ����
//        //CalculationResultFlag �����н��÷ֱ�־
        if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {                       //�����޸ĵ÷�
            User11_temp.winScore = CalculateTheMagnificationScore(User11_temp, ResultMsg.ChangeResult); //����÷�
//            CH375_Log(">���ֵĵ÷��� %lu", User11_temp.winScore);
        }
		Player[ID] = User11_temp; //����ѹ��
        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            User11_temp.totalScore = TotalScoreAfterGettingTheDifference(User11_temp.totalScore, pointDiff); //�����ֵ����ܷ�
            POKER_UserTypdePrintf((u8 *)"11 �����", User11_temp) ; //��ӡ����
            dataSynthesisArray(User11_temp, &dat[5]);               //���ݺϳ�
            commandDataReCRC(dat, len);                             //������������У��
            GetPackMsg_11.changeFlag = 1;
            changeCommandFlag = 1;
            //LogStr("11����0 ��", TempBuff, TempBuffLen);          //��ӡ����
        }
    }
}
//����ѹ������19
void DealWithStakeOrder_19(u8 *dat, u8 len)
{
    u8 ID = dat[4];                                 //�û�
    u32 pointDiff = 0;
    POKER_UserTypde User19_temp;                    //��ʱ�����ݲ���
    if(ID >= StartID && ID <= EndID) {
        //LogStr("19����0 ǰ", TempBuff, TempBuffLen);       //��ӡ����
        pointDiff = ReadTotalPoints(ID);               //��ȡ��ֵ
        arrayCompositeData(&User19_temp, &dat[5]);           //�������ݲ���
//        CH375_Log(">19 ���� �ֻ� %d  ��ֵ=>%lu ʵ���ܷ� %lu", ID, pointDiff, User19_temp.totalScore);
//        //POKER_UserTypdePrintf((u8 *)"19 ����ǰ", User19_temp) ; //��ӡ����
        //�жϲ�ֵ�Ƿ�ı�
        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            u32 TmpwinScore   = User19_temp.winScore  ;      //��ʱ�ܷ�
            u32 TmpTotalScore = User19_temp.totalScore;      //��ʱ�ܷ�
//            CH375_Log(">��ʾ��ʼת������ %lu %lu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
            if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //�޸Ĺ��˽��
                User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //����÷�
//                CH375_Log(">���ֵĵ÷��� %lu", User19_temp.winScore);
            }
            User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore, pointDiff); //�����ֵ����ܷ�
            if(ResultFlag == 0) { //���ּ����ֵ
                ResultFlag = 1;
                pointDiff = CalculateTheDifference(pointDiff, TmpwinScore - User19_temp.winScore);           //�����ֵ
                SaveTotalPoints(ID, pointDiff);                                                       //��ֵ����
            }
            if(pointDiff != 0) { CH375_Log(">�ֻ� %d  ��ֵ %lu  0x%-8X", ID, pointDiff, pointDiff); }
            dataSynthesisArray(User19_temp, &dat[5]);  //���ݺϳ�
            commandDataReCRC(dat, len);                //������������У��
            GetPackMsg_19.changeFlag = 1;
            changeCommandFlag = 1;
            POKER_UserTypdePrintf((u8 *)"19 �����", User19_temp) ; //��ӡ����
            //LogStr("19����0 ��", TempBuff, TempBuffLen);      //��ӡ����
        }
    }
}

uint8_t  TempBuff[64];    //��������(��ʱ�����) //��������
uint8_t  TempBuffLen;
uint8_t  DesBuff[64];     //��������(��������)
uint8_t  DesBuffLen;
uint8_t  CmdBuff[64];     //Դ����(����ת��)
uint8_t  CmdBuffLen;
uint8_t  UserBuffLen;

u8 PrintfFlag = 0;

//CH375������
void CH375_APP(void)
{
    uint8_t res, i, j;
    uint16_t len;
    res = mInitCH375Host();
    if(res != USB_INT_SUCCESS) { CH375_Log("ch375 init erro\n"); }
    CH375InitSysVar();                                        //�ϵ��ʼ���豸��ϢĬ��ֵ
    tm_timer_init(&debug_CHms, 100, 500);
    tm_timer_start(&debug_CHms);
    while(1) {
		if(PrintfFlag)  //�򵥴���
		{
			u8 temp = PrintfPlaySingle();
		    if(temp != WAIT_BUSY)
			{
			  PrintfFlag = 0;
			}
		}
        GSM_ControlStateMachine(); //ͨ��è״̬������
        #if 1 //ǰ
        if(CH375CheckConnect() == USBD_CONNECT) {             /* �ռ�⵽һ���豸���룬��Ҫö�� */
            CH375_Log("Device Connect\n");
            //��ʼö�ٲ���
            res = CH375BusReset();                   /* ���߸�λ */
            if(res != USB_INT_SUCCESS) { CH375_Log("Bus Reset Erro\n"); }
            delay_ms(100);                            /* �ȴ��豸�ȶ� */
            /* ��ȡ�豸������ */
            res = CH375GetDeviceDesc(UserBuff, &len);
            if(res == USB_INT_SUCCESS) {
                CH375_Logn("�豸������");
                for(i = 0; i < len; i++)
                { CH375_Logn("%02x ", (uint16_t)UserBuff[i]); }
                CH375_Log("\r\n");
                //12 01 00 01 00 00 00 40 42 50 00 02 2b 7f 01 02 03 01
                USBH_ParseDevDesc(&HostDevDesc, UserBuff, len); //����������,���VID,PID,PVN
            } else { CH375_Log("Get Device Descr Erro:0x%02x\n", (uint16_t)res); }
            /* ���õ�ַ */
            res = CH375SetDeviceAddr(2);
            if(res != USB_INT_SUCCESS)
            { CH375_Log("Set Addr Erro:0x%02x\n", (uint16_t)res); }
            /* ��ȡ���������� */
            res = CH375GetConfDesc(UserBuff, &len);
            if(res == USB_INT_SUCCESS) {
                CH375_Logn("����������");
                for(i = 0; i < len; i++)
                { CH375_Logn("%02x ", (uint16_t)UserBuff[i]); }
                CH375_Log("\n");
            } else { CH375_Log("Get Conf Descr Erro:0x%02x\n", (uint16_t)res); }
            /* �������� */
            res = CH375SetDeviceConf(1);
            if(res != USB_INT_SUCCESS) { CH375_Log("Set Config Erro\n"); }
            CH375_Log("USBD.status          = %02X", USBD.status);
            CH375_Log("USBD.itfmount        = %02X", USBD.itfmount);
            CH375_Log("USBD.itf[i].edpmount = %02X", USBD.itf[0].edpmount);
            CH375_Log("USBD.itf[i].edpmount = %02X", USBD.itf[1].edpmount);
        }
        if(USBD.status == USBD_READY) {     //�豸��ʼ�������
            //�����豸��Ϣ�ṹ�壬���ж϶˵㣬�����䷢��IN��
            for(i = 0; i != USBD.itfmount; i++) {
                for(j = 0; j != USBD.itf[i].edpmount; j++) {
                    if((USBD.itf[i].edp[j].attr == 0x03) && (USBD.itf[i].edp[j].edpnum & 0x80)) {   //�ж��ϴ��˵�
                        res = CH375InTrans(USBD.itf[i].edp[j].edpnum & 0x0F, UserBuff, &len, 10); //�Զ˵㷢IN��,NAK������
                        if(res == USB_INT_SUCCESS) {
                            if(len) {
                                u8 buf[65] = {0};
                                if(len < 64) {
                                    buf[0] = len;
                                    memcpy(&buf[1], UserBuff, len);
                                    TM_que_Write(&HID_TxQue, buf, len + 1); //�������ݰ�
                                }
                            }
                        }
                    }
                }
            }
        }
        //HID_Rx ---> Usart1_TX
        if(que_size(Tx1_que)) { //��������
            Usart_SendByte(COM1, que_Read(&Tx1_que));
        }
        if(que_size(Rx2_que)) { //��������
            Usart_SendByte(COM1, que_Read(&Rx2_que));
        }	
        if(que_size(Rx1_que)) { //��������
            u8 dat = que_Read(&Rx1_que);
            if(dat == 0xF1) {
                for(u8 i = 0; i < 40; i++) {
                    if(i >= StartID && i <= EndID) {
                        PlayerDifferenceValue[i].data = ReadTotalPoints(i); //Ĭ�ϵĲ�ֵ�ܷ���0
                        User_Log(">��ѯ[%02d]��ҵĲ�ֵ %lu 0x%08X", i, PlayerDifferenceValue[i].data, PlayerDifferenceValue[i].data);
                    }
                }
            } else if(dat == 0xF2) {
                for(u8 i = 0; i < 40; i++) {
                    PlayerDifferenceValue[i].data = ReadTotalPoints(i); //Ĭ�ϵĲ�ֵ�ܷ���0
                    User_Log(">��ѯ[%02d]��ҵĲ�ֵ %lu 0x%08X", i, PlayerDifferenceValue[i].data, PlayerDifferenceValue[i].data);
                }
            }
			else 
			
			if(dat == 0xFF) {
                PrintfFlag = 1;
            }else{
			    Usart_SendByte(COM2, dat);			
			}

			
        }
        #if 0  //TM_HostHID_Tx //## ��ȫ����ת��
        //TM_HostHID_Tx
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //δ���ܵ����ݰ�
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //�������ݰ�
            CmdBuffLen = UserBuff[0];                       //����
            HID_Send_Report(CmdBuff, CmdBuffLen);
        }
        #endif
        #if 0  //TM_HostHID_Tx //##��ȡ100��·������,A3���ƽ������
        //TM_HostHID_Tx
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //δ���ܵ����ݰ�
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //�������ݰ�
            CmdBuffLen = UserBuff[0];                       //����
            u8 Flag = 0;
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            for(u8 i = 0; i < 7; i++) { //��������
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //���4�ֽ���Կ
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            #if 0  //��ȫ�޹�����ʾ
            CH375_Logn("���ư� ");
            for(i = 0; i < 56 ; i++)
            { CH375_Log("%02X ", DesBuff[i]); }
            CH375_Logn("\r\n");
            #endif
            u8 *buf = NULL;
            if(GetPackPort == 0)
            { buf = (u8 *)tm_strstr((char *)CmdPackStart[0], 4, (char *)DesBuff, 56); } //100��·��
            if(buf || (GetPackPort && GetPackPort < 10)) {
                #if 1  //��ȫ�޹�����ʾ
                CH375_Logn("���ư� A0+1 :");
                for(i = 0; i < 60 ; i++)
                { CH375_Logn("%02X ", DesBuff[i]); }
                CH375_Logn("\r\n");
                #endif
                if(GetPackPort == 0)     { //��ͷ����
                    if(HID_Pack_OneCmd(buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen) == 0) { GetPackPort = 1; }
                    for(u8 i = 0; i < TempBuffLen - 4; i++) { //�޸Ľ��(�ų���ͷ,����,����)
                        TempBuff[4 + i] = 0x11; //������
                    }
                    memcpy(DesOutBuff, TempBuff, TempBuffLen); //�ݴ�
                    DesOutBuffLen = TempBuffLen;             //����
                    memcpy(buf, TempBuff, TempBuffLen);      //����ԭλ��
                    #if 1  //��ȫ�޹�����ʾ
                    CH375_Logn("���ư� A0   :");
                    for(i = 0; i < 60 ; i++)
                    { CH375_Logn("%02X ", DesBuff[i]); }
                    CH375_Logn("\r\n");
                    #endif
                    for(u8 i = 0; i < 7; i++) { //��������
                        des(&DesBuff[i * 8], desPassWord, DES_Encrypt, &DesBuff[i * 8]);
                    }
                    memcpy(A0_Buff[0], DesBuff, 60); //�ݴ� //����
                    //HID_Send_Report(DesBuff, 60);
                    Flag = 1; //�������滻
                } else if(GetPackPort == 1) {
                    HID_Pack_OneCmd(&DesBuff[1], 56, TempBuff, &TempBuffLen);
                    for(u8 i = 0; i < TempBuffLen - 2; i++) {                  //�޸Ľ��(��ȥ��β)
                        TempBuff[i] = 0x11; //������
                    }
                    memcpy(&DesOutBuff[DesOutBuffLen], TempBuff, TempBuffLen); //�����ϴζ�ȡ���ĳ���
                    DesOutBuffLen += TempBuffLen;            //����
                    u16 CRC16 = ComputeCRC16(DesOutBuff, 2, DesOutBuff[2]); //CRCУ��
                    TempBuff[TempBuffLen - 2] = CRC16 >> 8 & 0xFF;
                    TempBuff[TempBuffLen - 1] = CRC16 >> 0 & 0xFF;
                    memcpy(&DesBuff[1], TempBuff, TempBuffLen);             //����ԭλ��
                    #if 1  //��ȫ�޹�����ʾ
                    CH375_Logn("���ư� A0   :");
                    for(i = 0; i < 60 ; i++)
                    { CH375_Logn("%02X ", DesBuff[i]); }
                    CH375_Logn("\r\n");
                    #endif
                    for(u8 i = 0; i < 7; i++) { //��������
                        des(&DesBuff[i * 8], desPassWord, DES_Encrypt, &DesBuff[i * 8]);
                    }
                    HID_Send_Report(A0_Buff[0], 60);
                    HID_Send_Report(DesBuff, 60);
                    Flag = 1; //�������滻
                    GetPackPort = 0;
                }
                #if 0  //��ȫ�޹�����ʾ
                CH375_Logn("���ư� A0 :");
                for(i = 0; i < TempBuffLen ; i++)
                { CH375_Logn("%02X ", TempBuff[i]); }
                CH375_Logn("\r\n");
                #endif
            }
            //�޸Ľ��(ң�ؿ���)
            buf = (u8 *)tm_strstr((char *)CmdPackStart[2], 4, (char *)DesBuff, 56); //�ϴ����
            if(buf) {
                if(HID_Pack_OneCmd(buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen) == 1) { //�����Ƿ�����
                    if(WillResult) {
                        if(WillResult == 5) {
                            TempBuff[11] = 0x60;
                        } else if(TempBuff[11] == 0x60) { //С��
                            TempBuff[11] = WillResult << 4 | 1; //�޸Ľ��
                        } else {
                            TempBuff[11] = (TempBuff[11] & 0x0F) | WillResult << 4; //�޸Ľ��
                        }
                        WillResult = 0;
                    }
                    u16 CRC16 = ComputeCRC16(TempBuff, 2, TempBuff[2]); //CRCУ��
                    TempBuff[TempBuffLen - 2] = CRC16 >> 8 & 0xFF;
                    TempBuff[TempBuffLen - 1] = CRC16 >> 0 & 0xFF;
                    memcpy(buf, TempBuff, TempBuffLen);
                    for(u8 i = 0; i < 7; i++) { //��������
                        des(&DesBuff[i * 8], desPassWord, DES_Encrypt, &DesBuff[i * 8]);
                    }
                    DesBuff[56] = UserBuff[57]; //���4�ֽ���Կ
                    DesBuff[57] = UserBuff[58];
                    DesBuff[58] = UserBuff[59];
                    DesBuff[59] = UserBuff[60];
                    HID_Send_Report(DesBuff, 60);
                    Flag = 1; //�������滻
                    #if 1  //��ȫ�޹�����ʾ
                    CH375_Logn("���ư� A3 :");
                    for(i = 0; i < TempBuffLen ; i++)
                    { CH375_Logn("%02X ", TempBuff[i]); }
                    CH375_Logn("\r\n");
                    #endif
                } else {
                    CH375_Logn("���ư� A3 :Err");
                }
            }
            #if 0
            //            u8 * buf = (u8*)strstr((char*)DesBuff,"IC"); //Ѱ�Ұ�ͷ
            ////            if(buf){
            ////                 CH375_Log("des = %X  buf %X",DesBuff,buf);
            ////            }
            //            if(buf){
            //
            //
            //               if(buf[3] == 0xA3)
            //               {
            //                 HID_Pack_OneCmd(buf,56-(buf-DesBuff),TempBuff,&TempBuffLen);
            //                #if 1  //��ȫ�޹�����ʾ
            //                   CH375_Logn("���ư� A3 :");
            //                 for( i = 0; i <TempBuffLen ; i++)
            //                    CH375_Logn("%02X ",TempBuff[i]);
            //                 CH375_Logn ("\r\n");
            //                #endif
            //               }
            //               buf = (u8*)strstr((char*)buf+3,"IC"); //Ѱ�Ұ�ͷ
            //               if(buf[3] == 0xA3)
            //               {
            //                 HID_Pack_OneCmd(buf,56-(buf-DesBuff),TempBuff,&TempBuffLen);
            //                #if 1  //��ȫ�޹�����ʾ
            //                   CH375_Logn("���ư� A3 :");
            //                 for( i = 0; i <TempBuffLen ; i++)
            //                    CH375_Logn("%02X ",TempBuff[i]);
            //                 CH375_Logn ("\r\n");
            //                #endif
            //               }
            //
            //            }
            //           HID_Pack_Deal(DesBuff,56 ,&UserBuff[1],&UserBuff[0]);                  //��ȡ���ݰ�
            //           HID_Pack_Deal2(&UserBuffer[1],UserBuff[0] ,&DesOutBuff,DesOutBuffLen); //����������C0����
            //            if(UserBuffer[0])
            //            {
            //              CH375_Logn("���ư� ");
            //              for( i = 0; i <UserBuffer[0] ; i++ )
            //              CH375_Logn("%02x ",(uint16_t)UserBuffer[i+1]);
            //              CH375_Logn ("\r\n");
            //            }
            #endif
            //�ж������0xA0 100·������޸�
            if(WillResult && WillResult <= 5) { //�ı䱾�ֽ��
            }
            //�ж������0xA2 �õ����εľ���
            if(WillResult && WillResult <= 5) { //�ı䱾�ֽ��
            }
            //�ж������0xA3 �õ����εĽ��
            if(WillResult && WillResult <= 5) { //�ı䱾�ֽ��
            }
            if(Flag == 0)
            { HID_Send_Report(CmdBuff, CmdBuffLen); }
        }
        #endif
        #if 0  //TM_HostHID_Tx //#������������
        //TM_HostHID_Tx
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //δ���ܵ����ݰ�
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //�������ݰ�
            CmdBuffLen = UserBuff[0];                       //����
            u8 Flag = 0;
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            for(u8 i = 0; i < 7; i++) { //��������
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //���4�ֽ���Կ
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            #if 0  //��ȫ�޹�����ʾ
            CH375_Logn("���ư� ");
            for(i = 0; i < 56 ; i++)
            { CH375_Log("%02X ", DesBuff[i]); }
            CH375_Logn("\r\n");
            #endif
            HID_Pack_Deal(DesBuff, 56, &UserBuff[1], &UserBuff[0]);                  //��ȡ���ݰ�
            HID_Pack_Deal2(&UserBuff[1]  ,UserBuff[0],  &UserBuff[1],&UserBuff[0]);  //����������C0����
            if(UserBuff[0]) {
                CH375_Logn("���ư� ");
                for(i = 0; i < UserBuff[0] ; i++)
                { CH375_Logn("%02x ", (uint16_t)UserBuff[i + 1]); }
                CH375_Logn("\r\n");
            }
            //�ж������0xA0 100·������޸�
            if(WillResult && WillResult <= 5) { //�ı䱾�ֽ��
            }
            //�ж������0xA2 �õ����εľ���
            if(WillResult && WillResult <= 5) { //�ı䱾�ֽ��
            }
            //�ж������0xA3 �õ����εĽ��
            if(WillResult && WillResult <= 5) { //�ı䱾�ֽ��
            }
            if(Flag == 0)
            { HID_Send_Report(CmdBuff, CmdBuffLen); }
        }
        #endif
        #if 0  //TM_HostHID_Tx //## �޸��ܷ� (������ݵĽ���)
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //δ���ܵ����ݰ�
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //�������ݰ�
            CmdBuffLen = UserBuff[0];                       //����
            /* ��ý�����Կ*/
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            /* ���ݽ���*/
            for(u8 i = 0; i < 7; i++) { //��������
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //����4�ֽ���Կ
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            //������������
            u8 *buf = NULL;
            if(GetPackPort == 0) {
                buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 3, (char *)DesBuff, 56);        //������������  0x49, 0x43, 0x0E
                if(buf) { buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 3, (char *)DesBuff, 56); } //������������  0x49, 0x43, 0x0E, 0xA2
                if(buf) { //���������� ����Ŷ���
                    u8 len = 56 - (buf - DesBuff);
                    u16 temp = HID_GetOnePack(buf, len, TempBuff, &TempBuffLen);
                    if(temp & 0x0100) {  //������
                        StartMsg.inningNum = TempBuff[4];
                        StartMsg.BoutNum   = TempBuff[5];
                        CH375_Log("����1 %d�� %d�� ", StartMsg.inningNum, StartMsg.BoutNum);
                    }
                    //else if(temp&0x0200) //��������
                    //{
                    //    GetPackPort = 1;
                    //    memcpy(oneCmdBuff,TempBuff,TempBuffLen); //
                    //    oneCmdBuffLen = TempBuffLen;
                    //}
                }
            }
            //else if(GetPackPort>=1&&GetPackPort<5)          //ƴ�ӿ�������
            //{
            //      u16 temp = HID_GetOnePack(&DesBuff[1],56,TempBuff,&TempBuffLen);
            //      if(temp&0x0100)      //������
            //      {
            //          memcpy(&oneCmdBuff[oneCmdBuffLen],TempBuff,TempBuffLen); //
            //          oneCmdBuffLen += TempBuffLen;
            //          StartMsg.inningNum = TempBuff[4];
            //          StartMsg.BoutNum   = TempBuff[5];
            //          CH375_Log("����2 %d�� %d�� ",StartMsg.inningNum,StartMsg.BoutNum);
            //      }
            //      else if(temp&0x0200) //��������
            //      {
            //          GetPackPort = 0;
            //            CH375_Log("����2 ��ȡʧ��",StartMsg.inningNum,StartMsg.BoutNum);
            //      }
            //}
            //���100·��
            #if 1  //���11����
            if(GetPackMsg_11.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[3], 4, (char *)buf, 56 - (buf - DesBuff));   //������������  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_11, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            LogStr("11����0 ", TempBuff, TempBuffLen); //��ӡ����
                            GetPackMsg_11.intactPort = 0;
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������
                            GetPackMsg_11.intactPort = 1;
                            memcpy(Pack_11.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_11.BufLen = TempBuffLen;
                            buf = NULL; //�˳�ѭ��
                        }
                    }
                }
            } else if(GetPackMsg_11.intactPort) {      //ƴ��δ�곡�İ�
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //������
                    memcpy(&Pack_11.Buf[Pack_11.BufLen], TempBuff, TempBuffLen); //
                    Pack_11.BufLen += TempBuffLen;
                    LogStr("11����1 ", Pack_11.Buf, Pack_11.BufLen); //��ӡ����
                } else if(temp & 0x0200) { //��������
                    GetPackPort = 0;
                    CH375_Log(">11���� ƴ�� ʧ��");
                }
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[3], 4, (char *)buf, 56 - (buf - DesBuff));   //������������  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_11, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            LogStr("11����2 ", TempBuff, TempBuffLen); //��ӡ����
                            GetPackMsg_11.intactPort = 0;
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������
                            GetPackMsg_11.intactPort = 1;
                            memcpy(Pack_11.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_11.BufLen = TempBuffLen;
                            buf = NULL; //�˳�ѭ��
                        }
                    } else {
                        GetPackMsg_11.intactPort = 0;
                    }
                }
            }
            #endif
            #if 1  //���19����
            if(GetPackMsg_19.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //������������  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            LogStr("19����0 ", TempBuff, TempBuffLen); //��ӡ����
                            GetPackMsg_19.intactPort = 0;
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������
                            GetPackMsg_19.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_19.BufLen = TempBuffLen;
                            buf = NULL; //�˳�ѭ��
                        }
                    }
                }
            } else if(GetPackMsg_19.intactPort) {      //ƴ��δ�곡�İ�
                u16 temp = HID_GetPack(&GetPackMsg_19, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //������
                    memcpy(&Pack_19.Buf[Pack_19.BufLen], TempBuff, TempBuffLen); //
                    Pack_19.BufLen += TempBuffLen;
                    LogStr("19����1 ", Pack_19.Buf, Pack_19.BufLen); //��ӡ����
                } else if(temp & 0x0200) { //��������
                    GetPackMsg_19.intactPort = 0;
                    CH375_Log(">=19���� ƴ�� ʧ��");
                }
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //������������ 0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            LogStr("19����2 ", TempBuff, TempBuffLen); //��ӡ����
                            GetPackMsg_19.intactPort = 0;
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������
                            GetPackMsg_19.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_19.BufLen  = TempBuffLen;
                            buf = NULL;                               //�˳�ѭ��
                        }
                    } else {
                        GetPackMsg_19.intactPort = 0;
                    }
                }
            }
            #endif
            #if 1  //���A3���� ������� (��1��ʼ,��100)
            if(GetPackMsg_A3.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[2], 4, (char *)buf, 56 - (buf - DesBuff));   //������������  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_A3, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            LogStr("A3����  ", TempBuff, TempBuffLen); //��ӡ����
                            GetPackMsg_19.intactPort = 0;
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������
                            GetPackMsg_A3.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_19.BufLen = TempBuffLen;
                            buf = NULL; //�˳�ѭ��
                        }
                    }
                }
            } else if(GetPackMsg_A3.intactPort) {
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //������
                    memcpy(&Pack_A3.Buf[Pack_A3.BufLen], TempBuff, TempBuffLen); //
                    Pack_A3.BufLen += TempBuffLen;
                    LogStr("A3����  ", Pack_A3.Buf, Pack_A3.BufLen); //��ӡ����
                }
                GetPackMsg_A3.intactPort = 0;
            }
            #endif
            //ת��Դ����
            HID_Send_Report(CmdBuff, CmdBuffLen);
        }
        #endif
        #endif
        #if 1  //TM_HostHID_Tx //## �޸��ܷ� (�����ܷ�ֵ�޸�)�����޸�
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //δ���ܵ����ݰ�
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //�������ݰ�
            CmdBuffLen = UserBuff[0];                       //����
            /* ��ý�����Կ */
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            /* ���ݽ���*/
            for(u8 i = 0; i < 7; i++) { //��������
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //����4�ֽ���Կ
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            u8 *buf = NULL;
            #if 1 //������������
            if(GetPackMsg_A2.intactPort == 0) {
                buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 3, (char *)DesBuff, 56);        //������������  0x49, 0x43, 0x0E
                if(buf) { buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 4, (char *)DesBuff, 56); } //������������  0x49, 0x43, 0x0E, 0xA2
                if(buf) { //���������� ����Ŷ���
                    u16 temp = HID_GetPack(&GetPackMsg_A2, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                    if(temp & 0x0100) {  //������
                        DealWithStartOrder_A2(TempBuff, TempBuffLen); //�����
                        CH375_Log("����1 %d�� %d�� ", StartMsg.inningNum, StartMsg.BoutNum);
                    }
                }
            }
            #endif
            #if 1   //���100·��
            //ƴ����һ�����ݰ�
            if(GetPackMsg_A0.intactPort) {
                u16 temp = HID_GetPack(&GetPackMsg_A0, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //������
                    if(CmdPack_A0.pBuf) {
                        memcpy(&CmdPack_A0.pBuf[CmdPack_A0.NowLen], TempBuff, TempBuffLen); //
                        CmdPack_A0.NowLen += TempBuffLen;
                        if(CmdPack_A0.NowLen < CmdPack_A0.MaxLen)
                        { DealWithRoadOrderResult_A0(CmdPack_A0.pBuf, CmdPack_A0.NowLen); } //��������
                        if(GetPackMsg_A0.changeFlag) { //�����иı�
                            GetPackMsg_A0.changeFlag = 0;
                            memcpy(&ache_Buff[0][56 - GetPackMsg_A0.recordLength], CmdPack_A0.pBuf, GetPackMsg_A0.recordLength);                               //��һ�����ݰ�
                            memcpy(&DesBuff[1], &CmdPack_A0.pBuf[GetPackMsg_A0.recordLength], TempBuffLen);                                                    //���ε����ݰ�
                        }
                    } else {
                        log_err((u8*)"A0��������ռ�ʧ��");
                    }
                }
				if(CmdPack_A0.pBuf) {free(CmdPack_A0.pBuf);CmdPack_A0.pBuf=NULL; CmdPack_A0.MaxLen = 0;} //�ͷſռ�
                changeCommandFlag = 1;
                GetPackMsg_A0.intactPort = 0;
            }
            //һ�����ݰ�������
            if(GetPackMsg_A0.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[0], 4, (char *)buf, 56 - (buf - DesBuff));   //������������ 0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_A0, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {        //������
                        } else if(temp & 0x0200) { //��������
                            if(CmdPack_A0.pBuf) { free(CmdPack_A0.pBuf); } //�ͷſռ�
                            CmdPack_A0.pBuf = (u8 *)malloc(120);      //����ռ�
                            if(CmdPack_A0.pBuf) {
                                CmdPack_A0.MaxLen          = 120;
                                memcpy(CmdPack_A0.pBuf, TempBuff, TempBuffLen); // �ݴ���������
                                CmdPack_A0.NowLen          = TempBuffLen;
                                GetPackMsg_A0.recordLength = TempBuffLen;       //�����������
                            } else {
                                log_err((u8*)"A0��������ռ�ʧ��");
                            }
                            GetPackMsg_A0.intactPort   = 1;
                            changeCommandFlag          = 1;
                            buf = NULL; //�˳�ѭ��
                        }
                    }
                }
            }
            #endif
            #if 1  //���11����
            if(GetPackMsg_11.intactPort) {      //ƴ����һ�����ݰ�
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //������
                    memcpy(&Pack_11.Buf[Pack_11.BufLen], TempBuff, TempBuffLen); //
                    Pack_11.BufLen += TempBuffLen;
                    DealWithStakeOrder_11(Pack_11.Buf, Pack_11.BufLen); //��������
                    if(GetPackMsg_11.changeFlag) { //�����иı�
                        GetPackMsg_11.changeFlag = 0;
                        //�������ݰ�
                        memcpy(&ache_Buff[0][56 - GetPackMsg_11.recordLength], Pack_11.Buf, GetPackMsg_11.recordLength);             //��һ�����ݰ�
                        memcpy(&DesBuff[1], &Pack_11.Buf[GetPackMsg_11.recordLength], Pack_11.BufLen - GetPackMsg_11.recordLength);  //���ε����ݰ�
                    }
                    #if 0
                    u8 ID = Pack_11.Buf[4];                              //�û�
                    if(ID >= StartID && ID <= EndID) {
                        //LogStr("11����1 ǰ", Pack_11.Buf, Pack_11.BufLen);   //��ӡ����
                        POKER_UserTypde User11_temp;                           //��ʱ�����ݲ���
                        u32 pointDiff = ReadTotalPoints(ID);             //��ȡ��ֵ
                        arrayCompositeData(&User11_temp, &Pack_11.Buf[5]);     //�������ݲ���
                        CH375_Log("11 ���� �ֻ� %d  ��ֵ=>%lu ʵ���ܷ� %lu", ID, pointDiff, User11_temp.totalScore);
                        //POKER_UserTypdePrintf((u8 *)"11 ����ǰ", User11_temp) ; //��ӡ����
                        if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //�����޸ĵ÷�
                            User11_temp.winScore = CalculateTheMagnificationScore(User11_temp, ResultMsg.ChangeResult); //����÷�
                            CH375_Log(">���ֵĵ÷��� %lu", User11_temp.winScore);
                        }
                        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
                            User11_temp.totalScore = TotalScoreAfterGettingTheDifference(User11_temp.totalScore, pointDiff); //�����ֵ����ܷ�
                            POKER_UserTypdePrintf((u8 *)"11 �����", User11_temp) ; //��ӡ����
                            dataSynthesisArray(User11_temp, &Pack_11.Buf[5]);  //���ݺϳ�
                            commandDataReCRC(Pack_11.Buf, Pack_11.BufLen);     //������������У��
                            memcpy(&ache_Buff[0][56 - GetPackMsg_11.recordLength], Pack_11.Buf, GetPackMsg_11.recordLength);
                            //�������ݰ�
                            memcpy(&DesBuff[1], &Pack_11.Buf[GetPackMsg_11.recordLength], Pack_11.BufLen - GetPackMsg_11.recordLength);
                            //LogStr("11����1 ��", Pack_11.Buf, Pack_11.BufLen); //��ӡ����
                        }
                    }
                    #endif
                    changeCommandFlag = 1;
                } else if(temp & 0x0200) { //��������
                    GetPackPort = 0;
                    CH375_Log(">11���� ƴ�� ʧ��");
                }
                GetPackMsg_11.intactPort = 0;
            }
            //һ�����ݰ�������
            if(GetPackMsg_11.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[3], 4, (char *)buf, 56 - (buf - DesBuff));   //������������ 0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_11, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            DealWithStakeOrder_11(TempBuff, TempBuffLen); //��������
                            if(GetPackMsg_11.changeFlag) { //�����иı�
                                GetPackMsg_11.changeFlag = 0;
                                //�Ż�ԭ����
                                memcpy(buf, TempBuff, TempBuffLen);
                            }
                            GetPackMsg_11.intactPort = 0;
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������
                            GetPackMsg_11.intactPort   = 1;
                            changeCommandFlag          = 1;
                            memcpy(Pack_11.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_11.BufLen             = TempBuffLen;
                            GetPackMsg_11.recordLength = TempBuffLen;   //�����������
                            buf = NULL; //�˳�ѭ��
                        }
                    }
                }
            }
            #endif
            #if 1  //���19����
            if(GetPackMsg_19.intactPort) {      //ƴ�����ݰ�
                u16 temp = HID_GetPack(&GetPackMsg_19, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //������
                    memcpy(&Pack_19.Buf[Pack_19.BufLen], TempBuff, TempBuffLen); //
                    Pack_19.BufLen     += TempBuffLen;
                    DealWithStakeOrder_19(Pack_19.Buf, Pack_19.BufLen);
                    if(GetPackMsg_19.changeFlag) {
                        GetPackMsg_19.changeFlag = 0;
                        memcpy(&ache_Buff[0][56 - GetPackMsg_19.recordLength], Pack_19.Buf, GetPackMsg_19.recordLength);
                        memcpy(&DesBuff[1], &Pack_19.Buf[GetPackMsg_19.recordLength], Pack_19.BufLen - GetPackMsg_19.recordLength);//�������ݰ�
                    }
                    //                    u8 ID = Pack_19.Buf[4];                                 //�û�
                    //                    if(ID >= StartID && ID <= EndID) {
                    //                        //LogStr("19����1 ǰ", Pack_19.Buf, Pack_19.BufLen);  //��ӡ����
                    //                        POKER_UserTypde User19_temp;                        //��ʱ�����ݲ���
                    //                        u32 pointDiff = ReadTotalPoints(ID);     //��ȡ��ֵ
                    //                        arrayCompositeData(&User19_temp, &Pack_19.Buf[5]); //�������ݲ���
                    //                        CH375_Log(">19 ���� �ֻ� %d  ��ֵ=>%lu ʵ���ܷ� %lu", ID, pointDiff,User19_temp.totalScore);
                    //                        //POKER_UserTypdePrintf((u8 *)"19 ����ǰ", User19_temp) ; //��ӡ����
                    //                        //�жϲ�ֵ�Ƿ�ı�
                    //                        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
                    //                            u32 TmpwinScore   = User19_temp.winScore  ;      //��ʱ�ܷ�
                    //                            u32 TmpTotalScore = User19_temp.totalScore;      //��ʱ�ܷ�
                    //                          CH375_Log(">��ʾ��ʼת������ %lu %lu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
                    //                            if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //�޸Ĺ��˽��
                    //                                CH375_Log(">����޸ļ���");
                    //                                User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //����÷�
                    //                                CH375_Log(">���ֵĵ÷��� %lu", User19_temp.winScore);
                    //                            }
                    //                            User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore, pointDiff); //�����ֵ����ܷ�
                    //                            if(ResultFlag == 0) { //���ּ����ֵ
                    //                                ResultFlag = 1;
                    //                                pointDiff = CalculateTheDifference(pointDiff, TmpwinScore - User19_temp.winScore);
                    //                                SaveTotalPoints(ID, pointDiff);        //��ֵ����
                    //                            }
                    //                            if(pointDiff != 0) { CH375_Log(">�ֻ� %d  ��ֵ %lu  0x%-8X", ID, pointDiff, pointDiff); }
                    //                            dataSynthesisArray(User19_temp, &Pack_19.Buf[5]);   //���ݺϳ�
                    //                            commandDataReCRC(Pack_19.Buf, Pack_19.BufLen);      //������������У��
                    //                            POKER_UserTypdePrintf((u8 *)"19 �����", User19_temp) ; //��ӡ����
                    //                            //LogStr("19����1 ��", Pack_19.Buf, Pack_19.BufLen);      //��ӡ����
                    //                            memcpy(&ache_Buff[0][56 - GetPackMsg_19.recordLength], Pack_19.Buf, GetPackMsg_19.recordLength);
                    //                            //�������ݰ�
                    //                            memcpy(&DesBuff[1], &Pack_19.Buf[GetPackMsg_19.recordLength], Pack_19.BufLen - GetPackMsg_19.recordLength);
                    //                            //LogStr("19����1 ��", TempBuff, TempBuffLen);      //��ӡ����
                    //                        }
                    //                    }
                    changeCommandFlag = 1;
                } else if(temp & 0x0200) { //��������
                    GetPackMsg_19.intactPort = 0;
                    CH375_Log(">=19���� ƴ�� ʧ��");
                }
                GetPackMsg_19.intactPort = 0;
            }
            if(GetPackMsg_19.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //������������  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            DealWithStakeOrder_19(TempBuff, TempBuffLen);
                            if(GetPackMsg_19.changeFlag) {
                                GetPackMsg_19.changeFlag = 0;
                                memcpy(buf, TempBuff, TempBuffLen);
                            }
                            //                            u8 ID = TempBuff[4];                                     //�û�
                            //                            if(ID >= StartID && ID <= EndID) {
                            //                                //LogStr("19����0 ǰ", TempBuff, TempBuffLen);       //��ӡ����
                            //                                POKER_UserTypde User19_temp;                         //��ʱ�����ݲ���
                            //                                u32 pointDiff = ReadTotalPoints(ID);           //��ȡ��ֵ
                            //                                arrayCompositeData(&User19_temp, &TempBuff[5]);      //�������ݲ���
                            //                                CH375_Log(">19 ���� �ֻ� %d  ��ֵ=>%lu ʵ���ܷ� %lu", ID, pointDiff,User19_temp.totalScore);
                            //                                //POKER_UserTypdePrintf((u8 *)"19 ����ǰ", User19_temp) ; //��ӡ����
                            //                                //�жϲ�ֵ�Ƿ�ı�
                            //                                if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
                            //                                    u32 TmpwinScore   = User19_temp.winScore  ;      //��ʱ�ܷ�
                            //                                    u32 TmpTotalScore = User19_temp.totalScore;      //��ʱ�ܷ�
                            //                                    CH375_Log(">��ʾ��ʼת������ %lu %lu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
                            //                                    if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //�޸Ĺ��˽��
                            //                                        User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //����÷�
                            //                                        CH375_Log(">���ֵĵ÷��� %lu", User19_temp.winScore);
                            //                                    }
                            //                                    User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore, pointDiff); //�����ֵ����ܷ�
                            //                                    if(ResultFlag == 0) { //���ּ����ֵ
                            //                                        ResultFlag = 1;
                            //                                        pointDiff = CalculateTheDifference(pointDiff, TmpwinScore - User19_temp.winScore);
                            //                                        SaveTotalPoints(ID, pointDiff);                                //��ֵ����
                            //                                    }
                            //                                    if(pointDiff != 0) { CH375_Log(">�ֻ� %d  ��ֵ %lu  0x%-8X", ID, pointDiff, pointDiff); }
                            //                                    dataSynthesisArray(User19_temp, &TempBuff[5]);  //���ݺϳ�
                            //                                    commandDataReCRC(TempBuff, TempBuffLen);        //������������У��
                            //                                    //�Ż�ԭ����
                            //                                    memcpy(buf, TempBuff, TempBuffLen);
                            //                                    changeCommandFlag = 1;
                            //                                    POKER_UserTypdePrintf((u8 *)"19 �����", User19_temp) ; //��ӡ����
                            //                                    //LogStr("19����0 ��", TempBuff, TempBuffLen);      //��ӡ����
                            //                                }
                            //                            }
                            GetPackMsg_19.intactPort  = 0;
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������,Ҫ��������ݰ�
                            GetPackMsg_19.intactPort   = 1;
                            changeCommandFlag          = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_19.BufLen             = TempBuffLen;
                            GetPackMsg_19.recordLength = TempBuffLen;
                            buf = NULL; //�˳�ѭ��
                        }
                    }
                }
            }
            #if 0
            //if(GetPackMsg_19.intactPort) {      //ƴ��δ�곡�İ�
            //      u16 temp = HID_GetPack(&GetPackMsg_19, &DesBuff[1], 55, TempBuff, &TempBuffLen);
            //      if(temp & 0x0100) {  //������
            //          memcpy(&Pack_19.Buf[Pack_19.BufLen], TempBuff, TempBuffLen); //
            //          Pack_19.BufLen     += TempBuffLen;
            //          u8 ID = Pack_19.Buf[4];                                 //�û�
            //          if(ID >= StartID && ID <= EndID) {
            //              //LogStr("19����1 ǰ", Pack_19.Buf, Pack_19.BufLen);  //��ӡ����
            //              POKER_UserTypde User19_temp;                        //��ʱ�����ݲ���
            //              u32 pointDiff = ReadTotalPoints(ID);     //��ȡ��ֵ
            //              arrayCompositeData(&User19_temp, &Pack_19.Buf[5]); //�������ݲ���
            //              CH375_Log(">��ȡԭ���� ID = %d  ���ڵĲ�ֵ>%lu", ID, pointDiff);
            //              POKER_UserTypdePrintf((u8*)"19 ����ǰ",User19_temp) ;  //��ӡ����
            //              //�жϲ�ֵ�Ƿ�ı�
            //              if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            //                  u32 TmpwinScore   = User19_temp.winScore  ;      //��ʱ�ܷ�
            //                  u32 TmpTotalScore = User19_temp.totalScore;      //��ʱ�ܷ�
            //                  CH375_Log(">��ʾ��ʼת������ %llu %llu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
            //                  if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //�޸Ĺ��˽��
            //                      CH375_Log(">����޸ļ���");
            //                      User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //����÷�
            //                      CH375_Log(">���ֵĵ÷��� %lu", User19_temp.winScore);
            //                  }
            //                  User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore,pointDiff); //�����ֵ����ܷ�
            //                  if(ResultFlag == 0) { //���ּ����ֵ
            //                      ResultFlag = 1;
            //                      pointDiff = CalculateTheDifference(pointDiff,TmpwinScore - User19_temp.winScore);
            //                      SaveTotalPoints(ID, pointDiff);        //��ֵ����
            //                  }
            //                  if(pointDiff != 0) { CH375_Log(">�ֻ� %d  ��ֵ %lu  0x%8X", ID, pointDiff, pointDiff); }
            //                  dataSynthesisArray(User19_temp, &Pack_19.Buf[5]);   //���ݺϳ�
            //                  commandDataReCRC(Pack_19.Buf, Pack_19.BufLen);      //������������У��
            //                  POKER_UserTypdePrintf((u8*)"19 �����",User19_temp) ;  //��ӡ����
            //                  //LogStr("19����1 ��", Pack_19.Buf, Pack_19.BufLen);      //��ӡ����
            //                  memcpy(&ache_Buff[0][56 - GetPackMsg_19.recordLength], Pack_19.Buf, GetPackMsg_19.recordLength);
            //                  //�������ݰ�
            //                  memcpy(&DesBuff[1], &Pack_19.Buf[GetPackMsg_19.recordLength], Pack_19.BufLen - GetPackMsg_19.recordLength);
            //                  //LogStr("19����1 ��", TempBuff, TempBuffLen);      //��ӡ����
            //              }
            //          }
            //          changeCommandFlag = 1;
            //      } else if(temp & 0x0200) { //��������
            //          GetPackMsg_19.intactPort = 0;
            //          CH375_Log(">=19���� ƴ�� ʧ��");
            //      }
            //      buf = DesBuff;
            //      while(buf) {
            //          buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //������������ 0x49, 0x43, 0x0E
            //          if(buf) {
            //              u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
            //              if(temp & 0x0100) {  //������
            //                  u8 ID = TempBuff[4];                            //�û�
            //                  if(ID >= StartID && ID <= EndID) {
            //                      //LogStr("19����2 ǰ", TempBuff, TempBuffLen); //��ӡ����
            //                      POKER_UserTypde User19_temp;                    //��ʱ�����ݲ���
            //                      u32 pointDiff = ReadTotalPoints(ID);  //��ȡ��ֵ
            //                      arrayCompositeData(&User19_temp, &TempBuff[5]); //�������ݲ���
            //                      CH375_Log(">��ȡԭ���� ID = %d  ���ڵĲ�ֵ>%llu", ID, pointDiff);
            //                      POKER_UserTypdePrintf((u8*)"19 ����ǰ",User19_temp) ;  //��ӡ����
            //                      //�жϲ�ֵ�Ƿ�ı�
            //                      if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            //                          u32 TmpwinScore   = User19_temp.winScore  ;      //��ʱ�ܷ�
            //                          u32 TmpTotalScore = User19_temp.totalScore;      //��ʱ�ܷ�
            //                          CH375_Log(">��ʾ��ʼת������ %llu %llu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
            //                          if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {   //�޸Ĺ��˽��
            //                              User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //����÷�
            //                              if(pointDiff != 0) { CH375_Log(">�ֻ� %d  ��ֵ %llu", ID, pointDiff); }
            //                              CH375_Log(">���ֵĵ÷��� %lu", User19_temp.winScore);
            //                          }
            //                          User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore,pointDiff); //�����ֵ����ܷ�
            //                          if(ResultFlag == 0) { //���ּ����ֵ
            //                              ResultFlag = 1;
            //                            pointDiff = CalculateTheDifference(pointDiff,TmpwinScore - User19_temp.winScore);
            //                              SaveTotalPoints(ID, pointDiff);        //��ֵ����
            //                          }
            //                        if(pointDiff != 0) { CH375_Log(">�ֻ� %d  ��ֵ %lu  0x%8X", ID, pointDiff, pointDiff); }
            //                          dataSynthesisArray(User19_temp, &TempBuff[5]);  //���ݺϳ�
            //                          commandDataReCRC(TempBuff, TempBuffLen);        //������������У��
            //                          //�Ż�ԭ����
            //                          memcpy(buf, TempBuff, TempBuffLen);
            //                          changeCommandFlag = 1;
            //                          POKER_UserTypdePrintf((u8*)"19 �����",User19_temp) ;  //��ӡ����
            //                          //LogStr("19����2 ��", TempBuff, TempBuffLen);      //��ӡ����
            //                      }
            //                  }
            //                  GetPackMsg_19.intactPort = 0;
            //                  buf ++; //ƫ��
            //              } else if(temp & 0x0200) {                      //��������
            //                  GetPackMsg_19.intactPort = 1;
            //                  memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // �ݴ���������
            //                  Pack_19.BufLen             = TempBuffLen;
            //                  GetPackMsg_19.recordLength = TempBuffLen;
            //                  changeCommandFlag          = 1;             //������һ�����ݰ�
            //                  buf = NULL;                                 //�˳�ѭ��
            //              }
            //          } else {
            //              GetPackMsg_19.intactPort = 0;
            //          }
            //      }
            //  }
            #endif
            #endif
            #if 1  //���A3���� ���ֽ�� (��1��ʼ,��100)
            if(GetPackMsg_A3.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[2], 4, (char *)buf, 56 - (buf - DesBuff));   //������������  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_A3, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //������
                            //LogStr("A3����1  ǰ", TempBuff, TempBuffLen); //��ӡ����
                            GetPackMsg_19.intactPort = 0;
                            DealWithResultOrder_A3(TempBuff, TempBuffLen); //���ݴ���
                            if(GetPackMsg_A3.changeFlag) {                 //���ݰ����޸Ĺ�
                                GetPackMsg_A3.changeFlag = 0;
                                memcpy(buf, TempBuff, TempBuffLen);   //�Ż�ԭλ��
                            }
                            CH375_Log(">�������  ԭ %02X   �� %02X", ResultMsg.Result, ResultMsg.ChangeResult);
                            //<������>
                            buf ++; //ƫ��
                        } else if(temp & 0x0200) { //��������
                            GetPackMsg_A3.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // �ݴ���������
                            Pack_19.BufLen             = TempBuffLen;
                            GetPackMsg_A3.recordLength = TempBuffLen;
                            changeCommandFlag          = 1; //������һ�����ݰ�
                            buf = NULL; //�˳�ѭ��
                        }
                    }
                }
            } else if(GetPackMsg_A3.intactPort) {
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //������
                    memcpy(&Pack_A3.Buf[Pack_A3.BufLen], TempBuff, TempBuffLen); //
                    Pack_A3.BufLen += TempBuffLen;
                    DealWithResultOrder_A3(Pack_A3.Buf, Pack_A3.BufLen); //���ݴ���
                    if(GetPackMsg_A3.changeFlag) {            //���ݰ����޸Ĺ�
                        GetPackMsg_A3.changeFlag = 0;
                        //�Ż�ԭλ��
                        memcpy(&ache_Buff[56 - GetPackMsg_A3.recordLength], TempBuff, GetPackMsg_A3.recordLength);               //��һ�����ݰ�
                        memcpy(&DesBuff[1], &TempBuff[GetPackMsg_A3.recordLength], Pack_A3.BufLen - GetPackMsg_A3.recordLength); //�������ݰ�
                    }
                    changeCommandFlag = 1; //�������޸�
                }
                GetPackMsg_A3.intactPort = 0;
            }
            #endif
            #if 1  //���״̬
            //{0x49, 0x43, 0x02, 0xB0}, //<5>��ʼ��ע����
            //{0x49, 0x43, 0x04, 0xB1}, //<6>����ʱ
            //{0x49, 0x43, 0x02, 0xB2}, //<7>ֹͣ��ע����
            //{0x49, 0x43, 0x03, 0xB3}, //<8>�����н���
            //{0x49, 0x43, 0x03, 0xB4}, //<9>�����н�
            for(u8 i = 0; i < 5; i++) {
                buf = DesBuff;
                buf = (u8 *)tm_strstr((char *)CmdPackStart[5 + i], 4, (char *)buf, 56 - (buf - DesBuff)); //������������  0x49, 0x43, 0x0E
                if(buf) {
                    u8 tmpBuf[60] = {0};
                    u8 tmpBufLen = 0 ;
                    u16 temp = HID_GetPack(&GetPackMsg_state, buf, 56 - (buf - DesBuff), tmpBuf, &tmpBufLen);
                    if(temp & 0x0100) {  //������
                        if(tmpBuf[3] == 0xB0) {
                            CH375_Log(">0xB0 ��ʼ��ע����   �յ�");
                        } else if(tmpBuf[3] == 0xB1) {
                            if(tmpBuf[4] == tmpBuf[5] || tmpBuf[4] == 0)
                            { CH375_Log(">0xB1 ����ʱ����%d�� �յ� ", tmpBuf[4]); }
                        } else if(tmpBuf[3] == 0xB2) {
                            CH375_Log(">0xB2 ֹͣ��ע����   �յ�");
                        } else if(tmpBuf[3] == 0xB3) {
                            CH375_Log(">0xB3 �����н���     �յ�");
                            CalculationResultFlag = 1;
                        } else if(tmpBuf[3] == 0xB4) {
                            //CH375_Log(">0xB4 �����н�       �յ�");
                            CH375_Log(">");
                            CalculationResultFlag = 0;
                            ResultFlag = 0;
                        }
                    }
                }
            }
            #endif
            if(changeCommandFlag == 1) {
                changeCommandFlag = 0;
                controlTheForwardingProcess(DesBuff, 60);
            } else {
                //ת��Դ����
                HID_Send_Report(CmdBuff, CmdBuffLen);
            }
            //            //ת��Դ����
            //            HID_Send_Report(CmdBuff, CmdBuffLen);
        }
        #endif
        #if 1  //TM_HostHID_Rx //## ��λ�����ͣ����ư����
        if(TM_que_size(HID_RxQue)) {
            TM_que_Read(&HID_RxQue, UserBuff, &CmdBuffLen); //δ���ܵ����ݰ�
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //�������ݰ���CmdBuff
            CmdBuffLen = UserBuff[0];                       //����
            HostHID_SendReport(&UserBuff[1], UserBuff[0]);  //ת����
            #if 0 //��λ����������
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8) & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0) & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            for(u8 i = 0; i < 7; i++) { //��������
                des(&UserBuff[1 + i * 8], desPassWord, DES_Decrypt, &UserBuff[1 + i * 8]);
            }
            MHID_Pack_Deal(&UserBuff[1], 56, &UserBuff[1], &UserBuff[0]); //������������ ��ȡ����
            if(UserBuff[0]) {
                CH375_Logn("��λ�� ");
                for(i = 0; i < UserBuff[0]; i++)
                { CH375_Logn("%02x ", (uint16_t)UserBuff[i + 1]); }
                CH375_Logn("\r\n");
            }
            #endif
        }
        #endif
        if(tm_timer_CheckFlag(&debug_CHms)) {
            tm_timer_ClearFlag(&debug_CHms);
            // CH375_Log("A<%d> B<%d> C<%d> D<%d>",PCin(11),PCin(12),PBin(3),PBin(9));
        }
    }
}
#if 0
void CH375_ReadHost_HID(void)
{
    uint8_t res = 0, i = 0, j = 0;
    uint16_t len = 0;
    if(CH375CheckConnect() == USBD_CONNECT) {          /* �ռ�⵽һ���豸���룬��Ҫö�� */
        CH375_Log("Device Connect\n");
        //��ʼö�ٲ���
        res = CH375BusReset();                   /* ���߸�λ */
        if(res != USB_INT_SUCCESS) { CH375_Log("Bus Reset Erro\n"); }
        delay_ms(50);                            /* �ȴ��豸�ȶ� */
        /* ��ȡ�豸������ */
        res = CH375GetDeviceDesc(UserBuffer, &len);
        if(res == USB_INT_SUCCESS) {
            CH375_Logn("�豸������");
            for(i = 0; i < len; i++)
            { CH375_Logn("%02x ", (uint16_t)UserBuffer[i]); }
            CH375_Log("\r\n");
        } else { CH375_Log("Get Device Descr Erro:0x%02x\n", (uint16_t)res); }
        /* ���õ�ַ */
        res = CH375SetDeviceAddr(2);
        if(res != USB_INT_SUCCESS)
        { CH375_Log("Set Addr Erro:0x%02x\n", (uint16_t)res); }
        /* ��ȡ���������� */
        res = CH375GetConfDesc(UserBuffer, &len);
        if(res == USB_INT_SUCCESS) {
            CH375_Logn("����������");
            for(i = 0; i < len; i++)
            { CH375_Logn("%02x ", (uint16_t)UserBuffer[i]); }
            CH375_Log("\n");
        } else { CH375_Log("Get Conf Descr Erro:0x%02x\n", (uint16_t)res); }
        /* �������� */
        res = CH375SetDeviceConf(1);
        if(res != USB_INT_SUCCESS) { CH375_Log("Set Config Erro\n"); }
    }
    if(USBD.status == USBD_READY) {     //�豸��ʼ�������
        //�����豸��Ϣ�ṹ�壬���ж϶˵㣬�����䷢��IN��
        for(i = 0; i != USBD.itfmount; i++) {
            if(USBD.itf[i].edpmount == 0)
                for(j = 0; j != USBD.itf[i].edpmount; j++) {
                    if((USBD.itf[i].edp[j].attr == 0x03) && (USBD.itf[i].edp[j].edpnum & 0x80)) { //�ж��ϴ��˵�
                        res = CH375InTrans(USBD.itf[i].edp[j].edpnum & 0x0F, UserBuffer, &len, 0);  //�Զ˵㷢IN��,NAK������
                        if(res == USB_INT_SUCCESS) {
                            if(len) {
                                for(i = 0; i < len; i++)
                                { CH375_Logn("%02x ", (uint16_t)UserBuffer[i]); }
                                CH375_Log("\n");
                            }
                        }
                    }
                }
        }
    }
}








#endif
/********************************End of File************************************/

