/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2021,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : notecat_Demo.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2021-01-15
* Description  :
* Function List:
******************************************************************************
*/
/********************************End of Head************************************/
#include "notecat.h"
#include "public.h"
#include "stdlib.h"
#include "debug.h"
#include "multi_timer.h"
#include "HID_data.h"

#define MaxSms  5 //���Ķ��ź�


#if 1
    #define NoteCat_Log(...)
    #define NoteCat_Logn(...)
	#define Note_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
#else
    #define NoteCat_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
    #define NoteCat_Logn(...)  do{sprint(__VA_ARGS__);}while(0)
	#define Note_Log(...) 
#endif

typedef enum _COMMUNICATION_CAT_STATUS {
    ST_IDIE  = 0,
    ST_Busy,
    ST_Failed,
    ST_Success
} CatStateEnum;
typedef enum _CONNECTION_State {
    UNCONNECTION = 0,
    CONNECTION
} LineStateEnum;

//ͨ��è����������Ϣ�ṹ
typedef struct _NoteCat_Rec {
    u8 *pRecDat        ; //��������
    u16 NowCount       ; //�������ݼ���
    u16 Cnt            ;
    u16 MaxReceive     ; //������������
    u16 MaxCmdBuf      ; //���ٵĿռ�
    u16 OutTime        ; //��ʱ��ʱ
    u16 MaxOutTime     ; //���ʱʱ��
    u16 MaxTimeCnt     ; //���ʱʱ��
    u8  RecFlag        ; //���ձ�־	
} AcceptDataType;


//���Ž��հ����ݽ����ṹ
typedef struct _NoteCat_SmsPack { 
    u16 ID;
    u64 PhoneNumber;
    u8 *pPack      ;
    u16 PackLen    ;
} SmsMsgType;


u64 AdministratorPhone = 0;

u8 SmsSendPack[300]={0};
SmsMsgType  SmsMsg     = {0, 0, NULL, 0};         //���ն��Ž���
SendSmsType SendSms    = {NULL,0,0};            //���Ͷ�������
AcceptDataType NoteCat = {NULL, 0, 0, 0, 0}; //Ĭ�ϲ���
u8 GSM_Buf[200];


static     TM_Timer GSM_NextTime ;//����������
static u8  GSM_ConnectFlag = UNCONNECTION;



void USART3_IRQHandler(void)
{
	#if 1
    if(USART3->SR & (1 << 5)) {
        u8 dat = USART3->DR;
        if(NoteCat.pRecDat !=NULL && !NoteCat.RecFlag && NoteCat.MaxReceive) {
            if(NoteCat.Cnt < NoteCat.MaxCmdBuf) {       //����ԭ����
                NoteCat.Cnt++;
                NoteCat.OutTime = 250;
            } else if( NoteCat.NowCount < NoteCat.MaxReceive) { //���ݰ�
                if(NoteCat.OutTime) { NoteCat.OutTime = NoteCat.MaxOutTime; } //���ݼ��
                NoteCat.pRecDat[NoteCat.NowCount]  = dat;
                NoteCat.NowCount++;
				if(NoteCat.NowCount < NoteCat.MaxReceive)
				NoteCat.pRecDat[NoteCat.NowCount] = 0;
            } else {
                NoteCat.RecFlag = 1; //�������
            }
        } else {
//            que_Write(&Tx1_que, dat);
        }
    }
	#else
	if(USART3->SR&(1<<5))
	{
	  u8 dat = USART3->DR;
      que_Write(&Rx3_que,dat);
	}
	#endif
}







/*
*dest    Ҫ���������ڴ���ʼ��ַ
DestSize ����ƥ���С
*str     Ҫ�������ڴ���ʼ��ַ
StrSize  �����Ĵ�С
���� :�ɹ�==>��һ�γ��ֵ�λ�õ�ַ  ʧ��==>NULL
*/
// �ڴ����ݶԱ�ƥ��(�����ѯ��һ��ƥ�����ݴ�)
/*   Ҫ���������ڴ���ʼ��ַ,����ƥ���С,Ҫ�������ڴ���ʼ��ַ,�����Ĵ�С*/
char   *NoteCat_strstr(char *dest, unsigned int  DestSize,  char *str, unsigned int  StrSize)
{
    char *ptr = NULL;
    char *sr1 = (char *)dest;
    char *sr2 = (char *)str;
    unsigned int m1 = 0;
    unsigned int m2 = 0;
    if(dest == NULL || str == NULL || DestSize > StrSize) { return NULL; }
    for(unsigned int i = 0; i < StrSize; i++) { //������ѯ
        m1 = 0;
        m2 = 0;
        ptr = &sr2[i]; //��ʼλ��
        while(m1 != DestSize && sr1[m1] == ptr[m2] && i + m2 < StrSize) {
            m1++;
            m2++;
        }
        if(m1 == DestSize) { return (char *)ptr; } //�Աȳɹ�
    }
    return NULL;
}


//��������:ͨ��è��ʱ����,�ж�һ�����ݰ��Ƿ����
void NoteCat_OutTimeTiming(void)
{
    if(NoteCat.OutTime)         { NoteCat.OutTime--;   } //��ʱ
    if(NoteCat.OutTime == 1)    { NoteCat.RecFlag = 1; } //���ݰ����
	if(NoteCat.MaxTimeCnt)NoteCat.MaxTimeCnt--;
	if(NoteCat.MaxTimeCnt == 1) { NoteCat.RecFlag = 1; } //���ݰ����
}


//void USART1_IRQHandler(void)
//{
//    if(USART1->SR & (1 << 5)) {
//        u8 dat = USART1->DR;
//        que_Write(&Rx1_que, dat);
//    }
//}


//��������:׼����ʼ�������ݿ�ʼע�� (����������Ŀռ�, ���ʱʱ��)
u8 NoteCat_ReadyRegistered( u16 MaxOutTime)
{
//    if(NoteCat.pRecDat) { free(NoteCat.pRecDat); NoteCat.pRecDat = NULL;} //�ͷſռ�
	if(NoteCat.pRecDat) { NoteCat.pRecDat = NULL;} //�ͷſռ�
	if(NoteCat.pRecDat == NULL)
	{
		memset(GSM_Buf,0,NoteCat.NowCount);		
		NoteCat.pRecDat = GSM_Buf;	
		NoteCat.MaxReceive = sizeof(GSM_Buf);
		NoteCat.NowCount   = 0;
	}
	NoteCat.NowCount   = 0;
    NoteCat.Cnt        = 0;
    NoteCat.MaxCmdBuf  = 0;
    NoteCat.RecFlag    = 0;
	NoteCat.MaxTimeCnt = MaxOutTime;
    NoteCat.MaxOutTime = 250;
    NoteCat.OutTime    = 250;
    return 0;
}

u8 NoteCat_RecDatUser( u16 MaxOutTime)
{
//    if(NoteCat.pRecDat) { free(NoteCat.pRecDat); NoteCat.pRecDat = NULL;} //�ͷſռ�
	if(NoteCat.pRecDat) { NoteCat.pRecDat = NULL;} //�ͷſռ�
	if(NoteCat.pRecDat == NULL)
	{
		memset(GSM_Buf,0,NoteCat.NowCount);
		NoteCat.pRecDat = GSM_Buf;	
		NoteCat.MaxReceive = sizeof(GSM_Buf);
		NoteCat.NowCount   = 0;
	}
	NoteCat.NowCount   = 0;
    NoteCat.Cnt        = 0;
    NoteCat.MaxCmdBuf  = 0;
    NoteCat.RecFlag    = 0;
	NoteCat.MaxTimeCnt = MaxOutTime;
    NoteCat.MaxOutTime = 0;
    NoteCat.OutTime    = 0;
    return 0;
}

//��������:ע�����յ�������Ϣ
void NoteCat_ReadyUnRegistered(void)
{
//    if(NoteCat.pRecDat) { free(NoteCat.pRecDat); NoteCat.pRecDat= NULL; } //�ͷſռ�
	if(NoteCat.pRecDat) { NoteCat.pRecDat = NULL;} //�ͷſռ�
    NoteCat.RecFlag    = 0;
    NoteCat.MaxReceive = 0;
    NoteCat.NowCount   = 0;
}
//���ע����
u8  GetNoteCat_RecFlag(void)
{
    return NoteCat.RecFlag;
}
void ClearNoteCat_RecFlag(void)
{
    NoteCat.RecFlag = 0;
}

//��������:ͨ��è����һ�ֽ�����
void NoteCat_SendByte(u8 dat)
{
    Usart_SendByte(COM3, dat);
}
//��������:ͨ��è����һ������
void NoteCat_SendString(u8 *dat, u16 len)
{
    while(len--) {
        Usart_SendByte(COM3, *dat++);
    }
}

//ת���������
void  NoteCat_PrintfCmd(char *fmt, ...)
{
    volatile u8  buf[100];
    u8 len;
    va_list ap;                                   //��������б�
    va_start(ap, fmt);                            //��ʼ���б�
    vsnprintf((char *)buf, sizeof(buf), fmt, ap); //ת��Ϊ�ַ�
    va_end(ap);                                   //�����б�
    len = strlen((char *)buf);                    //�������ݳ���
    for(u8 i = 0; i < len; i++) {
        NoteCat_SendByte(buf[i]);
    }
}


#if 0
//01.���ÿ��ƺ���         SET13562781133  ���� OK13562781133   //ֻ��������ֻ�����
//02.����                 PRINT           ���� PRINT-OK
//03.ȡ�����в���         CANCEL          ���� CANCEL-OK
//04.���Ʊ��ֳԴ���С     EAT             ���� EAT-OK
//05.���ֺ���             SPADE           ���� SPADE-OK
//06.���ֺ���             HEARTS          ���� HEARTS-OK
//07.����÷��             CLUB            ���� CLUB-OK
//08.���ַ���             BLOCK           ���� BLOCK-OK
//09.������               KING            ���� KING-OK
//10.��ѯ���зֻ���ֵ��Ŀ INQUIRE         ���� 11:0,12:1000,13:-3000,14:0 ----40:0
//11.������зֻ���ֵ��Ŀ CLEAR           ���� CLEAR-OK

ע��:
������ֳ��Ľ���Ϊ���⽱��ʱ�����ܸı���, ���ָı���������Ч

#endif

//����ͷ
const u8 *CmdStr[] = {
    (u8 *)"SET",    //3
    (u8 *)"PRINT",  //5
    (u8 *)"CANCEL", //6
    (u8 *)"EAT",    //3
    (u8 *)"SPADE",  //5
    (u8 *)"HEARTS", //6
    (u8 *)"CLUB",   //4
    (u8 *)"BLOCK",  //5
    (u8 *)"KING",   //4
    (u8 *)"INQUIRE",//7
    (u8 *)"CLEAR"   //5

};

//-->AT  OK
//�������� AT+<x>=? ���������ڲ�ѯ����������ڲ��������õĲ����Լ���ȡֵ��Χ��
//��ѯ���� AT+<x>?    ���������ڷ��ز����ĵ�ǰֵ��
//�������� AT+<x>=<...> ���������������û��Զ���Ĳ���ֵ��
//ִ������ AT+<x>   ���������ڶ�ȡ��GSM ģ���ڲ�������ƵĲ��ɱ����

#if 0
//AT + CSQ = ?          == => ��ѯ�ź�
//+CSQ : (0 - 31, 99), (0 - 7, 99)
//+CSQ:  **, ##
//+CSQ:  <sddi>,<ber>
//**Ӧ�� 0 �� 31 ֮�䣨99��ʾ���źţ�����ֵԽ������ź�����Խ�ã�
//##Ϊ������,������99��ֵ�� 0 �� 99 ֮�䡣����Ӧ������߻� SIM ���Ƿ���ȷ��װ


//��������
//AT\r\n                   ����AT����ָ��
//AT + CSQ\r\n             GSM�źż��ָ��
//AT + CMGF = 1\r\n        ����TXT��ʽ���Ͷ���

////���Ͷ���
//AT + CMGS = 18924294786\r\n  ���ͽ��շ��ֻ�����
//> ��������
//1A(����)

////���ն���
//AT + CMGR =              ��ĳ������   = ? (����)(1 - 8) //���ŵ�λ��
//AT + CMGD =              ɾ��ĳ������ = ? (����)(1 - 8)


//��������
> AT + CMGL = ?
              +CMGL : ("REC UNREAD", "REC READ", "STO UNSENT", "STO SENT", "ALL")
              OK
              //��ȡ���ŵ��б�
              //AT + CMGL = ALL   == > ��ѯ�����б�
              //AT + CMGL = ALL
              //+ CMGL : 1, "REC READ", "+8618924294786",, "21/01/15,17:09:41+32"
              //123456
              //+ CMGL : 2, "REC READ", "+8618924294786",, "21/01/15,17:11:56+32"
              //123
              //OK
              //? AT + CMGL = ? //��������
              //? AT + CMGL = ?
              // +CMGL : ("REC UNREAD", "REC READ", "STO UNSENT", "STO SENT", "ALL")
              //  OK
#endif


void GSM_SendCmd(u16 MaxOutTime, char *fmt, ...)
{
      volatile u8  buf[100];
      u16 len;
      va_list ap;                                     //��������б�
      va_start(ap, fmt);                              //��ʼ���б�
      vsnprintf((char *)buf, sizeof(buf), fmt, ap);   //ת��Ϊ�ַ�
      va_end(ap);                                     //�����б�
      len = strlen((char *)buf);                      //�������ݳ���
	  NoteCat_RecDatUser(  MaxOutTime);
      NoteCat_SendString((u8 *)buf, len);             //���ͳ�ȥ
}	


//�������     (����ִ�,�����ִ�)  BasicCommands
u8 GSM_BasicCommands(u8 *dest, u8 *cmdStr)
{
    static u8 Port  = 0; //���̿���
    u8 state = ST_Busy;
    switch(Port) {
        case 0: { //��������
			GSM_SendCmd(500, "%s", cmdStr);
			Port = 1;
        }
        break;
        case 1: { //�������� (����ض��ִ�)
            u8 *pStr = (u8 *)NoteCat_strstr((char *)dest, strlen((const char *)dest), (char *)NoteCat.pRecDat, NoteCat.NowCount);
			if(pStr){
                state = ST_Success;			
//				NoteCat_Log(">%s", NoteCat.pRecDat);
//				NoteCat_Log("==============");	
				Port = 0;				
			}
            if(NoteCat.RecFlag)
			{
                state = ST_Failed;
				Port = 0;			
			}
        }
        break;
    }
    return state;
}

//�������     (����ִ�,�����ִ�)  BasicCommands
u8 GSM_CMRG(u8 NumSms)
{
    static u8 Port  = 0; //���̿���
    u8 state = ST_Busy;
    switch(Port) {
        case 0: { //��������
		    GSM_SendCmd(500,  "AT+CMGR=%d\r\n",NumSms);
			Port = 1;
        }
        break;
        case 1: { //�������� (����ض��ִ�)
            u8 *pStr = (u8 *)NoteCat_strstr((char *)"OK"   , 2, (char *)NoteCat.pRecDat, NoteCat.NowCount);
			if(pStr)
			{
			   NoteCat.pRecDat[NoteCat.NowCount]=0;
               state = ST_Success;	 Port = 0;
			   NoteCat_Log(">AT+CMGR=%d OK",NumSms);
//			   NoteCat_Log(">len[%d] >%s", NoteCat.NowCount,NoteCat.pRecDat);
//			   NoteCat_Log("==============");				
			}
			
            pStr = (u8 *)NoteCat_strstr((char *)"ERROR", 5, (char *)NoteCat.pRecDat, NoteCat.NowCount);			
			if(pStr)
			{
			   NoteCat_Log(">AT+CMGR=%d Err",NumSms);
               state = ST_Failed;	 Port = 0;	
			
			}
            if(NoteCat.RecFlag)
			{
			   NoteCat_Log(">AT+CMGR=%d Err",NumSms);
               state = ST_Failed;	 Port = 0;				
			}
        }
        break;
    }
    return state;
}

//���GSM����
u8 checkGSM_Connect(void)
{
    u8 state = GSM_BasicCommands((u8 *)"OK", (u8 *)"AT\r\n");
    return state;
}

//���ģ����ź���
u8 GetGSM_CSQ(void)
{
    u8 state = GSM_BasicCommands((u8 *)"OK", (u8 *)"AT+CSQ\r\n");
    if(state == ST_Success) {
        u16 rssi = 0;
        u16 ber  = 0;
        u8 *pstr = (u8 *) NoteCat_strstr("+CSQ:", strlen("+CSQ:"), (char *)NoteCat.pRecDat, NoteCat.MaxReceive); //Ѱ����ͷ
        if(pstr) {
            u8 Cnt = sscanf((char *)pstr, "+CSQ: %d, %d", (int *)&rssi, (int *)&ber); //���ַ�����ȡ��ʽ������
            if(Cnt == 2) {
                NoteCat_Log("��ȡ���ź��� %d,%d", rssi, ber);
                if(ber == 99 && rssi >= 10 && rssi <= 32) {
                    GSM_ConnectFlag = CONNECTION;
                } else {
                    GSM_ConnectFlag = UNCONNECTION;
                }
            }
        }
    }
    return state;
}
//��ȡ�������ݰ�����ʼ��ַ
char *GetSmsContentPackStartAddr(u8 *str, u8 strlen)
{
    u8 *pstr = (u8 *)NoteCat_strstr((char *)",,", 2, (char *)str, strlen);
    if(pstr) {
        pstr = (u8 *)NoteCat_strstr((char *)"\"", 1, (char *)(pstr + 3), strlen - (pstr - str));
        if(pstr) {
            return (char *)(pstr + 3);
        }
    }
    return NULL;
}

//�ַ�ת����
u64 StrToNumber(u8 *str, u8 len)
{
    u64 temp = 0;
    //��⿪ͷ�������ַ�
    while((*str < '0' ||  *str > '9') && len) {
        len--;
        str++;
    }
    for(u8 i = 0 ; i < len; i++) {
        if((*str >= '0' &&  *str <= '9')) {
            temp *= 10;
            temp += *str - '0';
            str++;
        } else {
            return temp;
        }
    }
    return temp;
}







u8 GSM_Port = 0;

//׼����������,��ȥ���ù���Ա��
void GAM_SmsRecCms(u8 i)
{
	memset(SmsSendPack,0,sizeof(SmsSendPack));
	switch(i) {
		case 1: { //����
		   Note_Log(">���ڴ������ ���� PRINT ");
		   memcpy(SmsSendPack,"PRINT-OK",strlen((const char*)"PRINT-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;
		case 2: { //ȡ�����в���
		   Note_Log(">���ڴ������ ȡ�����в��� CANCEL ");
		   memcpy(SmsSendPack,"CANCEL-OK",strlen((const char*)"CANCEL-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;
		case 3: {//���ֳԴ���С
		   Note_Log(">���ڴ������ �Դ���С EAT ");
		   memcpy(SmsSendPack,"EAT-OK",strlen((const char*)"EAT-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;
		case 4: {//���ֺ���
			Note_Log(">���ڴ������ ���ֺ��� SPADE ");
		   memcpy(SmsSendPack,"SPADE-OK",strlen((const char*)"SPADE-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;
		case 5: {//���ֺ���
			Note_Log(">���ڴ������ ���ֺ��� HEARTS ");
		   memcpy(SmsSendPack,"HEARTS-OK",strlen((const char*)"HEARTS-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;				
		case 6: {//����÷��
		   Note_Log(">���ڴ������ ����÷�� CLUB ");
		   memcpy(SmsSendPack,"CLUB-OK",strlen((const char*)"CLUB-OK")); 
		} break;
		case 7: {//���ַ���
		   Note_Log(">���ڴ������ ���ַ��� BLOCK ");
		   memcpy(SmsSendPack,"BLOCK-OK",strlen((const char*)"BLOCK-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;
		case 8: {//������
			Note_Log(">���ڴ������ ������ HING ");
		   memcpy(SendSms.pStr,"HING-OK",strlen((const char*)"HING-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;				
		case 9: {//��ѯ����·����ֵ
			//׼����������
		} break;
		case 10:{//������зֻ���ֵ
			Note_Log(">���ڴ������ �����ֵ CLEAR ");
		   memcpy(SendSms.pStr,"CLEAR-OK",strlen((const char*)"CLEAR-OK")); 
		   GSM_Port = 8; //���Ͷ���	
		} break;						
	}

}


//����һ������
void GSM_SmsOneDeal(u8 *dat, u16 len)
{
    u16 ID = 0;
    u8 PhoneNumStr[20] = {0};
    u8 NULLStr[20] = {0};
    u8 *pstr = NULL;
    u8 BackCnt = sscanf((char *)dat, "+CMGL: %d,\"%[^\"]\",\"%[^\"]\",,", (int *)&ID, (char *)NULLStr, PhoneNumStr);
    if(BackCnt == 3) {
        if(PhoneNumStr[0] == '+') { //�ֻ�����
            u8 len = strlen((const char *)&PhoneNumStr[3]);
            memcpy(PhoneNumStr, &PhoneNumStr[3], len);
            memset(&PhoneNumStr[len], 0, sizeof(PhoneNumStr) - len);
        }
        SmsMsg.ID = ID;
        SmsMsg.PhoneNumber = StrToNumber(PhoneNumStr, sizeof(PhoneNumStr)); //�绰תΪ����
        NoteCat_Log("����Ϣ ID = %d  �ֻ���Ϊ%s  numBer=%llu", ID, PhoneNumStr, SmsMsg.PhoneNumber);
		Note_Log(">����Ϣ ID = %d  �ֻ���Ϊ%s  numBer=%llu", SmsMsg.ID , PhoneNumStr, SmsMsg.PhoneNumber);
        pstr = (u8 *)GetSmsContentPackStartAddr(dat, len); //ʵ�ʶ����������ݰ�
        if(pstr) {
            SmsMsg.pPack   = pstr;
            SmsMsg.PackLen = len - (pstr - dat);
            u8 i = 0;
			//���ΰ�ͷ�ж�
            for(i = 0; i < sizeof(CmdStr) / sizeof(u8 *); i++) {
                if(pstr[0] == CmdStr[i][0] && pstr[1] == CmdStr[i][1] && pstr[2] == CmdStr[i][2]) {          //����Ƿ����õ绰����
                   break;
                }
            }
			//�������� (��ӡ������������)
            NoteCat_Logn("����=> len=%-0.3d >", SmsMsg.PackLen);
            for(u8 i = 0; i < SmsMsg.PackLen; i++) {
                NoteCat_Logn("%02X ", SmsMsg.pPack[i]);
            }
            NoteCat_Logn("\r\n");
            Usart_SendString(COM1,pstr,SmsMsg.PackLen);
			NoteCat_Logn("<---------->\r\n");
			//=================
			// ���Ŵ���
			//=================
			if(i==0) //���õ绰����
			{
				u64 Phone = StrToNumber(pstr, SmsMsg.PackLen);       //�绰תΪ����
				if(Phone >= 10000000000LL ) //�жϵ绰��11λ����
				{
				   AdministratorPhone = Phone;
				   NoteCat_Log("AdministratorPhone = %llu", AdministratorPhone); //
				   SaveAdministratorPhone(AdministratorPhone);
				   u8 buf[64] = {0};
                   sprintf((char*)buf,"OK%llu",AdministratorPhone);
				   memset(SmsSendPack,0,sizeof(SmsSendPack));
				   memcpy(SmsSendPack,buf,len); 	             
				   GSM_Port = 8; //���Ͷ���					
				}else{ //ɾ������
				   GSM_Port = 9; 
				}
			}else if(i < sizeof(CmdStr) / sizeof(u8 *)){
				NoteCat_ReadyUnRegistered();  //�ͷſռ�
				//�Ա��Ƿ��ǹ���Ա�绰
			    if(SmsMsg.PhoneNumber == AdministratorPhone){				
					u8 * ptr = (u8*)NoteCat_strstr((char*)CmdStr[i],strlen((const char*)CmdStr[i]),(char*)SmsMsg.pPack,SmsMsg.PackLen); //�ٴ�ƥ������
					if(ptr==NULL) //��,˵��ƥ��ʧ��
					{
					  i = sizeof(CmdStr) / sizeof(u8 *);
					}
					GAM_SmsRecCms(i);         //���Ŵ���
					DockingNoteCat_Control(i);//�Խ�ͨ��è����
			    }			
			}else{ //��ָ�����,ɾ������
				NoteCat_ReadyUnRegistered();  //�ͷſռ�
			    GSM_Port = 9;
			}
        }
    }
}




void GSM_SmsOneDeal_2(u8 *dat, u16 len)
{
    u8 PhoneNumStr[20] = {0};
    u8 NULLStr[20] = {0};
    u8 *pstr = NULL;
    u8 BackCnt = sscanf((char *)dat, "+CMGR: \"%[^\"]\",\"%[^\"]\",,", (char *)NULLStr, PhoneNumStr);
    if(BackCnt == 2) {
        if(PhoneNumStr[0] == '+') { //�ֻ�����
            u8 len = strlen((const char *)&PhoneNumStr[3]);
            memcpy(PhoneNumStr, &PhoneNumStr[3], len);
            memset(&PhoneNumStr[len], 0, sizeof(PhoneNumStr) - len);
        }
        SmsMsg.PhoneNumber = StrToNumber(PhoneNumStr, sizeof(PhoneNumStr)); //�绰תΪ����
        NoteCat_Log("����Ϣ ID = %d  �ֻ���Ϊ%s  numBer=%llu", SmsMsg.ID , PhoneNumStr, SmsMsg.PhoneNumber);
		Note_Log("����Ϣ ID = %d  �ֻ���Ϊ%s  numBer=%llu", SmsMsg.ID , PhoneNumStr, SmsMsg.PhoneNumber);
        pstr = (u8 *)GetSmsContentPackStartAddr(dat, len); //ʵ�ʶ����������ݰ�
        if(pstr) {
            SmsMsg.pPack   = pstr;
            SmsMsg.PackLen = len - (pstr - dat);
            u8 i = 0;
			//���ΰ�ͷ�ж�
            for(i = 0; i < sizeof(CmdStr) / sizeof(u8 *); i++) {
                if(pstr[0] == CmdStr[i][0] && pstr[1] == CmdStr[i][1] && pstr[2] == CmdStr[i][2]) {          //����Ƿ����õ绰����
                   break;
                }
            }
			//�������� (��ӡ������������)
            NoteCat_Logn("����=> len=%-0.3d >", SmsMsg.PackLen);
            for(u8 i = 0; i < SmsMsg.PackLen; i++) {
                NoteCat_Logn("%02X ", SmsMsg.pPack[i]);
            }
            NoteCat_Logn("\r\n");
            Usart_SendString(COM1,pstr,SmsMsg.PackLen);
			NoteCat_Logn("<---------->\r\n");
			//=================
			// ���Ŵ���
			//=================
			if(i==0) //���õ绰����
			{
				u64 Phone = StrToNumber(pstr, SmsMsg.PackLen);       //�绰תΪ����
				if(Phone >= 10000000000LL ) //�жϵ绰��11λ����
				{
				   AdministratorPhone = Phone;
				   NoteCat_Log("AdministratorPhone = %llu", AdministratorPhone); //
				   SaveAdministratorPhone(AdministratorPhone);
				   u8 buf[64] = {0};
                   sprintf((char*)buf,"OK%llu",AdministratorPhone);
				   memset(SmsSendPack,0,sizeof(SmsSendPack));
				   memcpy(SmsSendPack,buf,strlen((const char*)buf)); 
                   Note_Log("���ڴ��� ���õ绰����Ա %llu",AdministratorPhone);
				   GSM_Port = 8; //���Ͷ���					
				}else{ //ɾ������
				   GSM_Port = 9; 
				}
			}else if(i < sizeof(CmdStr) / sizeof(u8 *)){
				NoteCat_ReadyUnRegistered();  //�ͷſռ�
				//�Ա��Ƿ��ǹ���Ա�绰
			    if(SmsMsg.PhoneNumber == AdministratorPhone){				
					u8 * ptr = (u8*)NoteCat_strstr((char*)CmdStr[i],strlen((const char*)CmdStr[i]),(char*)SmsMsg.pPack,SmsMsg.PackLen); //�ٴ�ƥ������
					if(ptr==NULL) //��,˵��ƥ��ʧ��
					{
					  i = sizeof(CmdStr) / sizeof(u8 *);
					}
					GAM_SmsRecCms(i);         //���Ŵ���
					DockingNoteCat_Control(i);//�Խ�ͨ��è����
			    }			
			}else{ //��ָ�����,ɾ������
				NoteCat_ReadyUnRegistered();  //�ͷſռ�
			    GSM_Port = 9;
			}
        }
    }
}





//ɾ������
u8 GSM_DeleteSMS(u8 ID)
{
  static u8 Port = 0;
  u8 state = ST_Busy;
  u8 temp  = 0;
  switch(Port)
  {
        case 0: { //����豸
            temp = checkGSM_Connect(); //AT����ָ��
            if(temp == ST_Success) {
                Port ++; 
				GSM_SendCmd(500, "AT+CMGD=%d\r\n",ID); //������������ò���ʱ��
            }
        }
        break;
        case 1: { //ɾ������
			u8 *pstr = (u8*)NoteCat_strstr("OK",2,(char*)NoteCat.pRecDat,NoteCat.NowCount);
            if(pstr)
			{
			   state = ST_Success; Port = 0; Note_Log(">ɾ��%d���ųɹ�",ID);
			}
			if(NoteCat.RecFlag)
			{
			   state = ST_Failed ; Port = 0; Note_Log(">ɾ��%d����ʧ��",ID);	
			}
        }
        break;
  }
  return state;
}
//���Ͷ���  
u8 GSM_SendSMS_Pack(u8*dat,u16 len)
{
  static u8 Port = 0;
  u8 state = ST_Busy;
  switch(Port)
  {
        case 0: { //���Ͳ���ָ��
			GSM_SendCmd(500,"AT+CMGS=?\r\n");
			Port++;	
        }
        break;
        case 1:{
			if(NoteCat.RecFlag){
			   NoteCat_Log(">GSM Send Check AT+CMGS=? Err");
			   state = ST_Failed;
			   Port = 0;				
			}			
			u8 *pstr = (u8*)NoteCat_strstr("OK",2,(char*)NoteCat.pRecDat,NoteCat.NowCount);
			if(pstr){
			   NoteCat_Log(">GSM Send Check AT+CMGS=? OK");
			   Port++;	
			   GSM_SendCmd(500,"AT+CMGS=\"%llu\"\r\n",AdministratorPhone); //���õ绰����			
			}
			pstr = (u8*)NoteCat_strstr("ERROR",5,(char*)NoteCat.pRecDat,NoteCat.NowCount);
			if(pstr){
			   NoteCat_Log(">GSM Send Check AT+CMGS=? Err");
			   state = ST_Failed;
			   Port = 0;				
			}
		
		}break;
        case 2: { //���õ绰

			    u8 *pstr = (u8*)NoteCat_strstr(">",1,(char*)NoteCat.pRecDat,NoteCat.NowCount);
				if(pstr){
				   Note_Log(">���Ͷ������õ绰  OK");
				   Port++;					
				}
				if(NoteCat.RecFlag){
				   Note_Log(">���Ͷ������õ绰 Err");
				   state = ST_Failed;
			       Port = 0;
				}

        }
        break;
		case 3:{
				NoteCat_RecDatUser(2000);
				Note_Log(">len %d����%s",strlen((const char*)dat),dat);
				NoteCat_SendString(dat  ,strlen((const char*)dat));						
				Port ++;				
		}break;
		case 4:{
		   u8* pstr = (u8*)NoteCat_strstr((char*)dat,strlen((const char*)dat),(char*)NoteCat.pRecDat,NoteCat.NowCount);
		   if(pstr || NoteCat.RecFlag || NoteCat.NowCount>=len)
		   {
		      NoteCat.pRecDat[NoteCat.NowCount]=0;
			  Note_Log(">>1����%s",NoteCat.pRecDat);
			  NoteCat_RecDatUser(10000);
			  NoteCat_SendByte(0x1A);    //���Ž���
			  Port ++;			      
		   }
		}break;
		case 5:{
			u8* pstr = (u8*)NoteCat_strstr("+CMGS",5,(char*)NoteCat.pRecDat,NoteCat.NowCount);
            if(pstr){state = ST_Success; Port = 0; Note_Log(">���Ͷ��ųɹ�");}
			pstr = (u8*)NoteCat_strstr("ERROR",5,(char*)NoteCat.pRecDat,NoteCat.NowCount);
            if(pstr){state = ST_Success; Port = 0; Note_Log(">���Ͷ��ųɹ�");}
		    if(NoteCat.RecFlag  )
		    {
		       NoteCat.pRecDat[NoteCat.NowCount]=0;
			    NoteCat_RecDatUser(0);
		       Port = 0;
		    }
		}break;
  }
  return state;

}


//GSM ����״̬��
void  GSM_ControlStateMachine(void)
{
	static u8 GSM_Cnt  = 0;
    u8 temp = 0;
    switch(GSM_Port) {
        case 0: { //����豸
            temp = checkGSM_Connect(); //AT����ָ��
            if(temp == ST_Success) {
                GSM_Port ++;
            }
        }
        break;
        case 1: { //����ź��Ƿ���Է�����Ϣ
            temp = GetGSM_CSQ(); //����ź���
            if(temp == ST_Success && GSM_ConnectFlag == CONNECTION) {
                GSM_Port ++;
				GSM_Port = 2;
            }
        }
        break;
		
		case 0xFF:{ //ɾ������
		
		
		}break;
		
        case 2: { //���ö���ģʽ
            temp =  GSM_BasicCommands((u8 *)"OK", (u8 *)"AT+CMGF=1\r\n"); //AT����ָ��
            if(temp == ST_Success) {
                GSM_Port ++;
				NoteCat_Log(">AT+CMGF=1  OK");
            }else if(temp == ST_Failed){
				NoteCat_Log(">AT+CMGF=1  Err");
			    GSM_Port =  0;//��ȡ����ź�
			}
        }
        break;

        case 3: { //���Զ���ָ��
            temp = GSM_BasicCommands((u8 *)"OK", (u8 *)"AT+CMGL=?\r\n");
            if(temp == ST_Success) {
                u8 *pstr = (u8 *) NoteCat_strstr("\"ALL\"", strlen("\"ALL\""), (char *)NoteCat.pRecDat, NoteCat.NowCount); //Ѱ����ͷ
                if(pstr) {
					NoteCat_Log(">AT+CMGL=? OK");
                    GSM_Port ++;
                } else {
					NoteCat_Log(">AT+CMGL=? Err");
                    GSM_Port =  1;//��ȡ����ź�
                }
            }else if(temp == ST_Failed && NoteCat.NowCount == 0){
			    GSM_Port =  0;//��ȡ����ź�
			}
        }
        break;	

        case 4: { //���Զ���ָ��
			
            temp = GSM_CMRG(GSM_Cnt);
            if(temp == ST_Success ) {
				if(NoteCat.NowCount >50)
				{
//					NoteCat_Log(">AT+CMGR=%d",GSM_Cnt);
//					NoteCat_Log("<======%d========>",GSM_Cnt);
//					NoteCat_SendString(NoteCat.pRecDat, NoteCat.NowCount);
//					NoteCat_Log("<================>");
					SmsMsg.ID = GSM_Cnt;
					GSM_SmsOneDeal_2(NoteCat.pRecDat, NoteCat.NowCount);				
				}
				GSM_Cnt %=MaxSms;
			    GSM_Cnt++;
				GSM_Port = 5;
				GSM_SendCmd(1000,"AT+CMGL=ALL\r\n"); //�鿴ȫ������
            }else  if(temp == ST_Failed){
				GSM_Cnt %=MaxSms;
			    GSM_Cnt++;
				
			    GSM_Port = 6;
				tm_timer_init(&GSM_NextTime,100,0);
				tm_timer_start(&GSM_NextTime);
			}
        }
        break;		
        case 5: { //���ն���
            u8 *pstr1 = (u8 *) NoteCat_strstr("OK", 2, (char *)NoteCat.pRecDat, NoteCat.NowCount); //Ѱ����ͷ
			if(pstr1)
			{
                NoteCat_Log(">AT+CMGL=ALL");
                NoteCat_Log("<======1========>");
                pstr1 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)NoteCat.pRecDat, NoteCat.NowCount ); //Ѱ����ͷ
                if(pstr1) {
                    u16 RemainingLength = NoteCat.NowCount - (pstr1 - NoteCat.pRecDat); //ʣ�೤��
                    u8 *pstr2 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)(pstr1 + 1), RemainingLength - 1); //Ѱ����ͷ
                    if(pstr2) { //�ڶ�������
                        GSM_SmsOneDeal(pstr1, pstr2 - pstr1);
                        NoteCat_Log("<======2========>");
                        pstr1++;
                    } else {   //���һ��������
                        if(RemainingLength > 6) {
                            GSM_SmsOneDeal(pstr1, RemainingLength - 6);
                            pstr1 = NULL; //�˳�ѭ��
                        }
                    }
                }

				if(GSM_Port == 5){
				    tm_timer_init(&GSM_NextTime,100,0);
					tm_timer_start(&GSM_NextTime);
                   GSM_Port++;				
				}
			
			}
			if(NoteCat.RecFlag)
			{
			   pstr1 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)NoteCat.pRecDat, NoteCat.NowCount ); //Ѱ����ͷ
			   if(pstr1) {
						u16 RemainingLength = NoteCat.NowCount - (pstr1 - NoteCat.pRecDat); //ʣ�೤��
						u8 *pstr2 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)(pstr1 + 1), RemainingLength - 1); //Ѱ����ͷ
						if(pstr2) { //�ڶ�������
							GSM_SmsOneDeal(pstr1, pstr2 - pstr1);
							NoteCat_Log("<======2========>");
							pstr1++;
						} else {   //���һ��������
							if(RemainingLength > 6) {
								GSM_SmsOneDeal(pstr1, RemainingLength - 6);
								pstr1 = NULL; //�˳�ѭ��
							}
						}
				}
				if(GSM_Port == 5){
					tm_timer_init(&GSM_NextTime,100,0);
					tm_timer_start(&GSM_NextTime);
				    GSM_Port++;				
				}
			
			}
        }
        break;
        case 6: { //�ȴ�1S ���һ��
           if(tm_timer_CheckFlag(&GSM_NextTime))
		   {
				tm_timer_ClearFlag(&GSM_NextTime);
			    GSM_Port = 3;
		   }
        } break;		
		
        //��2���Ƿ��Ͷ���
        case 8: { 
			temp = GSM_SendSMS_Pack(SmsSendPack,strlen((const char*)GSM_SendSMS_Pack));
			if(temp==ST_Success)
			{
				GSM_Port = 9;  //ɾ������
			}
        }
        break;
        //������ɾ������
        case 9: { //ɾ������
			temp = GSM_DeleteSMS(SmsMsg.ID);  //ɾ������
			if(temp == ST_Failed || temp==ST_Success)
			{
			   GSM_Port = 0;
			}
        } break;
    }
}



#if 0
void notecat_Main(void)
{
    Usart_Init(COM3, 115200, 2, 1);
    NoteCat_Log("unsigned long int = %d byte", sizeof(unsigned long long));
    while(1) {
        u8 temp = checkGSM_Connect();
        if(temp == ST_Success) {
            NoteCat_Log("GSM Connect Success");
            break;
        }
        //͸��
        if(que_size(Tx1_que)) {
            Usart_SendByte(COM1, que_Read(&Tx1_que));
        }
    }
    while(1) {
        GSM_ControlStateMachine();
        //͸��
        if(que_size(Rx1_que)) {
            Usart_SendByte(COM3, que_Read(&Rx1_que));
        }
        //        //͸��
        //        if(que_size(Tx1_que)) {
        //            Usart_SendByte(COM1, que_Read(&Tx1_que));
        //        }
    }
}
#endif





//=====================�������=================
#include "spi_dri.h"
//�绰����
void SaveAdministratorPhone(u64 Phone)
{
   uint64_unionType temp ;
   temp.data = Phone;
   for(u8 i =0;i<sizeof(u64);i++)
   {
	 FM25WriteString(40, &temp.byte[0], sizeof(u64));
   }
}
u64  ReadAdministratorPhone(void)
{
   uint64_unionType temp ;
   FM25ReadString(40, &temp.byte[0], sizeof(u64));
   return temp.data;
}


//������ʼ��
void StartOpenGSM_Init(void)
{
  AdministratorPhone = ReadAdministratorPhone();      //��ù���Ա�绰
  NoteCat_Log("GSM ����Ա�绰 %llu",AdministratorPhone);
}

/********************************End of File************************************/

