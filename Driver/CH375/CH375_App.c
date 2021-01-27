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

extern u8 WillResult ;  //操作的结果

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
//  u32 totalScore;   //总分
//  u32 winScore;     //得分
//  u16 spadeBet;     //黑桃押分
//  u16 heartBet;     //红桃押分
//  u16 clubBet ;     //草花押分
//  u16 diamondBet;   //方块押分
//  u16 jokerBet;     //王  押分
//}POKER_UserTypde;
const POKER_UserTypde PlayerNull = {0, 0, 0, 0, 0, 0, 0};
POKER_UserTypde       Player[40] = {0}; //40个玩家本本局参数

u32_unionType PlayerDifferenceValue[40] = {0}; //40个玩家

//计算倍率得分(根据结果)
u32 CalculateTheMagnificationScore(POKER_UserTypde temp, u8 newResult)
{
    u32 Score = 0;
    if(newResult == 0x60 || newResult == 0x50) {      //王得分计算=20倍率*压分+其余4门压分
        Score  = 20 * temp.jokerBet + temp.spadeBet + temp.heartBet + temp.clubBet  + temp.diamondBet;
    } else if((newResult >> 4) == 1) { //3.8倍率
        Score  = 3.8 * temp.spadeBet;
    } else if((newResult >> 4) == 2) { //3.8倍率
        Score  = 3.8 * temp.heartBet;
    } else if((newResult >> 4) == 3) { //2倍率
        Score  = 4 * temp.clubBet;
    } else if((newResult >> 4) == 4) { //2倍率
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

//计算吃大赔小出的结果
u8  Calculate_EatBig_LoseSmall(void)
{
    u32 Fraction[5] = {0};
    u32 FraTable[5] = {0};
    u8  flag[5]    = {0}; //结果标记
    for(u8 i = StartID; i <= EndID; i++) {
        Fraction[0] += CalculateTheMagnificationScore(Player[i], 0x50); //出王   计算得分
        Fraction[1] += CalculateTheMagnificationScore(Player[i], 0x10); //出黑桃 计算得分
        Fraction[2] += CalculateTheMagnificationScore(Player[i], 0x20); //出红桃 计算得分
        Fraction[3] += CalculateTheMagnificationScore(Player[i], 0x30); //出草花 计算得分
        Fraction[4] += CalculateTheMagnificationScore(Player[i], 0x40); //出方块 计算得分
    }
    //标记
    for(u8 i = 0; i < 5; i++) {
        if(Fraction[i] == 0) {
            flag[i] = 1;
        }
        FraTable[i] = Fraction[i];
    }
    u8 temp = Calculate_ZeroCnt(flag, 5); //检测有0
    sort_u32(FraTable, 5); //数据排序
    if(temp == 0) { //无0值,需要排序
        for(u8 i = 0; i < 5; i++) {
            if(FraTable[0] == Fraction[i]) {
                if(i == 0) {
                    return 6; //出王
                } else {
                    return i; //除普通的
                }
            }
        }
    } else {    //随机
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
    CH375_Log(">吃大赔小 计算出错");
    return 1;
}



//计算本局的差值                       (原差值 ,本局的差值)
u32 CalculateTheDifference(u32 DifferencePoints, int32_t PoorInGame)
{
    u32 temp = DifferencePoints; //原差值
    if(PoorInGame) { //本局控输
        if(temp & 0x80000000) {      //原是负数
            u32 pointTemp = temp & 0x7FFFFFFF; //得到原差值
            if(pointTemp > PoorInGame) {
                temp = pointTemp - PoorInGame;
                temp |= 0x80000000;
            } else {                 //原是正数
                temp = PoorInGame - pointTemp;
            }
        } else {
            temp   += PoorInGame;
        }
    } else {                         //本局控赢
        if(temp & 0x80000000) { //原是负数
            u32 pointTemp   = temp & 0x7FFFFFFF; //得到原差值
            u32 nwPointDiff = labs(PoorInGame);
            temp  = pointTemp + nwPointDiff;
            temp |= 0x80000000;
        } else {
            u32 pointTemp   = temp ; //得到原差值
            u32 nwPointDiff = labs(PoorInGame);
            if(pointTemp < PoorInGame) {
                temp = PoorInGame - pointTemp;
                temp |= 0x80000000;
            } else {                 //原是正数
                temp =  pointTemp - PoorInGame;
            }
        }
    }
    return temp;
}

//获得差值后的总分
u32 TotalScoreAfterGettingTheDifference(u32 totalScore, u32 PointDiff)
{
    if(PointDiff & 0x80000000) { //负数
        return totalScore += (PointDiff & 0x7FFFFFFF); //差值计算
    } else {                   //正数
        return totalScore -= PointDiff;               //差值计算
    }
}

TM_Timer debug_CHms     ; //定时器
u8  desPassWord[8] = {0}; //密钥
u8  desBuff[8]     = {0}; //密钥
u8 resultSave[100] = {0};

const u8 CmdPackStart[][4] = {
    {0x49, 0x43, 0x67, 0xA0}, //<0>100局路单
    {0x49, 0x43, 0x0E, 0xA2}, //<1>开局--得到场次和局数
    {0x49, 0x43, 0x10, 0xA3}, //<2>控制结果
    {0x49, 0x43, 0x15, 0x11}, //<3>发送押分分数(改变分机当前总分)
    {0x49, 0x43, 0x15, 0x19}, //<4>发送押分分数(倒计时结束、线上玩家押分)(改变分机当前总分)(自己根据压分计算结果)
    {0x49, 0x43, 0x02, 0xB0}, //<5>开始下注命令
    {0x49, 0x43, 0x04, 0xB1}, //<6>倒计时
    {0x49, 0x43, 0x02, 0xB2}, //<7>停止下注命令
    {0x49, 0x43, 0x03, 0xB3}, //<8>计算中奖分
    {0x49, 0x43, 0x03, 0xB4}, //<9>结束中奖
};

u8 GetPackPort    = 0;
u8 GetPackPort_11 = 0;
u8 GetPackPort_19 = 0;
GetPackType Pack_11 = {{0}, 0};
GetPackType Pack_19 = {{0}, 0};
GetPackType Pack_A3 = {{0}, 0};
GetPack_MsgType GetPackMsg_A0    = {0, 0, 0}; //提取命令
GetPack_MsgType GetPackMsg_11    = {0, 0, 0}; //提取命令
GetPack_MsgType GetPackMsg_19    = {0, 0, 0}; //提取命令
GetPack_MsgType GetPackMsg_A2    = {0, 0, 0}; //提取命令
GetPack_MsgType GetPackMsg_A3    = {0, 0, 0}; //提取命令
GetPack_MsgType GetPackMsg_state = {0, 0, 0}; //提取命令

GetPack_MsgType *pPackMsg[] = {&GetPackMsg_A0, &GetPackMsg_11, &GetPackMsg_19, &GetPackMsg_A2, &GetPackMsg_A3};
u8 WillResultFlag        = 0; //计算标志
u8 CalculationResultFlag = 0; //计算标志
u8 ResultFlag            = 0; //结果标志

uint8_t  ache_Buff[2][64];  //命令缓存数据

void ReportPack_encrypt(u8 *Buf, u8 len); //数据包再加密()

inline u8 checkExtractionFlag(void)
{
    for(u8 i = 0; i < sizeof(pPackMsg) / sizeof(GetPack_MsgType *); i++) { //检查数据包是否有不完整的
        if(pPackMsg[i]->intactPort) {
            return 1;
        }
    }
    return 0;
}

//进来时，必须是改变过后的数据
void controlTheForwardingProcess(u8 *dat, u8 len)
{
    static u8 Port    = 0;
    static u8 lengthIndex = 0;
    u8 temp = checkExtractionFlag(); //检查是否有不完整的包
    if(temp) {
        if(Port == 0) {
            Port = 1;       //第一次不完整
        } else if(Port == 1) {
            Port = 2;       //在次不完整
        }
    } else {
        Port = 0; //都是完整数据包
    }
    switch(Port) {
        case 0: { //发送全部数据
            if(lengthIndex == 1) {
                lengthIndex = 0;
                ReportPack_encrypt(ache_Buff[0], 60); //数据在加密
                HID_Send_Report(ache_Buff[0], 60);   //发送数据
                memcpy(ache_Buff[0], dat, 60);       //复制新的数据
                ReportPack_encrypt(ache_Buff[0], 60); //数据在加密
                HID_Send_Report(ache_Buff[0], 60);   //发送数据
            } else {
                memcpy(ache_Buff[0], dat, 60);       //复制新的数据
                ReportPack_encrypt(ache_Buff[0], 60); //数据在加密
                HID_Send_Report(ache_Buff[0], 60);   //发送数据
            }
        }
        break;
        case 1: { //保留这次数据
            lengthIndex = 1;
            memcpy(ache_Buff[0], dat, 60);
        }
        break;
        case 2: { //发送前次数据,这次数据前移动
            lengthIndex = 1;
            ReportPack_encrypt(ache_Buff[0], 60); //数据在加密
            HID_Send_Report(ache_Buff[0], 60);   //发送数据
            memcpy(ache_Buff[0], dat, 60);       //复制新的数据
        }
        break;
    }
}


//数据命令重新校验
void commandDataReCRC(u8 *dat, u8 len)
{
    u16 CRC12 = ComputeCRC16(dat, 2, dat[2]);
    dat[len - 2] = CRC12 >> 8 & 0xFF;
    dat[len - 1] = CRC12 >> 0 & 0xFF;
}
//解密数据打印
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

/*USB标准设备描述复分析*/
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
/*CH375 发送数据包*/
void HostHID_SendReport(u8 *Buf, u8 len)
{
    u8 res = 0;
    u8 cnt = 0;
    if(USBD.status == USBD_READY) {
HostErr:
        if(USBD.itf[0].edp[0].edpnum == 0x01) {
            res = CH375OutTrans(USBD.itf[0].edp[0].edpnum & 0x0F, Buf, 60, 0);  //对端点发IN包,NAK不重试
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
            res = CH375OutTrans(USBD.itf[0].edp[1].edpnum & 0x0F, Buf, 60, 0);  //对端点发IN包,NAK不重试
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

//数据包再加密()
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
        //解密加密
        for(u8 i = 0; i < 7; i++) {
            des(&Buf[i * 8], Key, DES_Encrypt, &Buf[i * 8]);
        }
    }
}

//大端数据合成小端数字
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
//小端数字分解大端数组
void bigendDataFormDecomposition(u32 dat, u8 *str, u8 size)
{
    for(size_t i = 0; i < size; i++) {
        str[i] = dat >> ((size - 1 - i) * 8) & 0xFF;
    }
}
//玩家本局参数打印
void  POKER_UserTypdePrintf(u8 *str, POKER_UserTypde temp)
{
//    CH375_Logn(">%s", str);
//    CH375_Log("总分 %8lu 得分 %8lu  <黑>%4d <红>%4d <梅>%4d <方>%4d <王>%4d", temp.totalScore,
//              temp.winScore,
//              temp.spadeBet,
//              temp.heartBet,
//              temp.clubBet,
//              temp.diamondBet,
//              temp.jokerBet);
}
//数组(大端储存)合成数据(小端储存)
void  arrayCompositeData(POKER_UserTypde *temp, u8 *dat)
{
    temp->totalScore = bigendianSyntheticData(&dat[0 ], sizeof(u32)); //大端的数据合成
    temp->winScore   = bigendianSyntheticData(&dat[4 ], sizeof(u32));
    temp->spadeBet   = bigendianSyntheticData(&dat[8 ], sizeof(u16)); //大端的数据合成
    temp->heartBet   = bigendianSyntheticData(&dat[10], sizeof(u16));
    temp->clubBet    = bigendianSyntheticData(&dat[12], sizeof(u16)); //大端的数据合成
    temp->diamondBet = bigendianSyntheticData(&dat[14], sizeof(u16));
    temp->jokerBet   = bigendianSyntheticData(&dat[16], sizeof(u16)); //大端的数据合成
}
//数据(小端储存)合成数组(大端储存)
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
    uint8_t  inningNum;  //轮数
    uint8_t  BoutNum  ;  //局数
} StartMsgType;
StartMsgType StartMsg; //开局信息
typedef struct _ResultMsg {
    uint8_t  Result;        //原结果
    uint8_t  ChangeResult;  //改变后的结果
} ResultMsgType;
ResultMsgType ResultMsg; //开局信息
uint8_t  A0_Buff[2][60];    //解密数据
uint8_t changeCommandFlag = 0; //改变命令标志
uint8_t UserBuff[100];
//uint8_t DesOutBuff[256];
//uint8_t DesOutBuffLen;
//uint8_t oneCmdBuff[255]; //缓存长的命令
//uint8_t oneCmdBuffLen;
//uint8_t Cmd_11_Buff[255]; //缓存长的命令
//uint8_t Cmd_11_BuffLen;

PackType CmdPack_A0;


extern u8 WillResultFlag;
//生成剩余结果
void GenerateWillResults(void)
{
   u8 res;
   u8  result = 0;
	if(ReadPlaySingle()==0)
	{
		CH375_Log("路单结果1:");
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
	   CH375_Log("路单结果2");
	   SavePlaySingle(1); //已补单
	}
}



//检查是否允许修改结果
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
char *ResultStrGB[] = {"黑", "红", "梅", "方", "王"};
//处理路单结果命令(100局的路单)
void DealWithRoadOrderResult_A0(u8 *dat, u8 len)
{
    u8 boutCnt = ReadBoutCout(); //读取局数
	u8 inning  = ReadInningNum();
    u8 flag = 0;
    u8 Cnt  = 0;
	CH375_Log(">>>>>>>>>>");
	if(ReadPlaySingle()==0){
		if(boutCnt && inning==dat[4]) {
			for(u8 i = 0 ,m = 0; i < boutCnt; i++) {    //替换结果
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
		if(inning==dat[4]) { //轮数要对上
			for(u8 i = 0 ,m = 0; i < 100; i++) {    //100局全替换结果
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
		CH375_Log("A0 重新校验");
        commandDataReCRC(dat, len);          //重新校验
        GetPackMsg_A0.changeFlag = 1;
    }
    CH375_Log(">第%d轮路单", dat[4]);
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
//处理开局命令
void DealWithStartOrder_A2(u8 *dat, u8 len)
{
	WillResultFlag = 0;
    StartMsg.inningNum = dat[4]; //轮数
    StartMsg.BoutNum   = dat[5]; //局数
	if(StartMsg.BoutNum==1){
	  SavePlaySingle(0);  //补单清除
		for(u8 i = 0; i < 100; i++) { //清除本局押分
			SaveBoutResult(i+1,0);
		}	
	}
	
    for(u8 i = 0; i < 40; i++) { //清除本局押分
        Player[i] = PlayerNull;
    }
}
//处理结果命令(控制结果)
void DealWithResultOrder_A3(u8 *dat, u8 len)
{
	if(ReadPlaySingle())CH375_Logn("已补单 %02X %02X ",ReadBoutResult(StartMsg.BoutNum),dat[11]);
	else CH375_Logn("未补单 %02X ",dat[11]);
    ResultMsg.Result = dat[11];                        //记录原本结果
    if(CheckAllowChangeResult(dat, len)) {
           if(WillResult) {        //需要判断是否出特殊奖,已经补单了
            if(WillResult == 6) { 
                u8 temp = Calculate_EatBig_LoseSmall();//本局吃大赔小
                if(temp == 6) {    //本局出王
                    if(rand() % 2 == 1) {
                        dat[11] = 0x60;
                    } else {
                        dat[11] = 0x50;
                    }
                } else if(dat[11] == 0x60 || dat[11] == 0x50) {  //本局出王的,改为其他的
                    dat[11] = temp << 4 | ((rand() % 0x0D) + 1); //修改结果
                } else {
                    dat[11] = (dat[11] & 0x0F) | temp << 4;      //修改结果
                }
            } else if(WillResult == 5) {  //本局出王
                if(rand() % 2 == 1) {
                    dat[11] = 0x60;
                } else {
                    dat[11] = 0x50;
                }
            } else if((dat[11] == 0x60 || dat[11] == 0x50) && WillResult<=4 && WillResult) {        //本局出王的,改为其他的
                dat[11] = WillResult << 4 | ((rand() % 0x0D) + 1); //修改结果
            } else if(WillResult && WillResult<=4){
                dat[11] = (dat[11] & 0x0F) |  WillResult << 4;      //修改结果
            }
            GetPackMsg_A3.changeFlag = 1;
        }else if(ReadPlaySingle()){  //补单后的结果
			dat[11] = ReadBoutResult(StartMsg.BoutNum);	
			GetPackMsg_A3.changeFlag = 1;
	     }
		if(GetPackMsg_A3.changeFlag)
		{
			commandDataReCRC(dat, len);          //重新校验
			changeCommandFlag = 1;
		}
		 
    } else {
		if(ReadPlaySingle())SaveBoutResult(StartMsg.BoutNum,dat[11]); //补单后，有特殊结果，清除生成的结果
//        if(WillResult) { CH375_Log(">本局有特殊奖 控制作废 %d", WillResult); }
    }
	CH375_Log(" %02X \r\n",dat[11]);	
    WillResult = 0;
	SaveInningNum(StartMsg.inningNum);         //保存轮数
    SaveBoutCout(StartMsg.BoutNum);            //保存局数
    SaveBoutResult(StartMsg.BoutNum, dat[11]); //保存结果
    ResultMsg.ChangeResult = dat[11];          //修改后的结果
	WillResultFlag = 1;
}

//处理压分命令11
void DealWithStakeOrder_11(u8 *dat, u8 len)
{
    u8 ID         = dat[4];      //用户
    u32 pointDiff = 0;           //临时差值
    POKER_UserTypde User11_temp; //临时押分参数
    if(ID >= StartID && ID <= EndID) {
        //LogStr("11命令0 前", TempBuff, TempBuffLen);                    //打印数据
        pointDiff = ReadTotalPoints(ID);                                  //读取差值
        arrayCompositeData(&User11_temp, &dat[5]);                        //解析数据参数
//        CH375_Log("11 命令 分机 %d  差值=>%lu 实际总分 %lu", ID, pointDiff, User11_temp.totalScore);
//        //POKER_UserTypdePrintf((u8 *)"11 命令前", User11_temp) ;         //打印数据
//        //CalculationResultFlag 计算中奖得分标志
        if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {                       //本局修改得分
            User11_temp.winScore = CalculateTheMagnificationScore(User11_temp, ResultMsg.ChangeResult); //计算得分
//            CH375_Log(">本局的得分是 %lu", User11_temp.winScore);
        }
		Player[ID] = User11_temp; //保存压分
        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            User11_temp.totalScore = TotalScoreAfterGettingTheDifference(User11_temp.totalScore, pointDiff); //计算差值后的总分
            POKER_UserTypdePrintf((u8 *)"11 命令后", User11_temp) ; //打印数据
            dataSynthesisArray(User11_temp, &dat[5]);               //数据合成
            commandDataReCRC(dat, len);                             //整条命令重新校验
            GetPackMsg_11.changeFlag = 1;
            changeCommandFlag = 1;
            //LogStr("11命令0 后", TempBuff, TempBuffLen);          //打印数据
        }
    }
}
//处理压分命令19
void DealWithStakeOrder_19(u8 *dat, u8 len)
{
    u8 ID = dat[4];                                 //用户
    u32 pointDiff = 0;
    POKER_UserTypde User19_temp;                    //临时的数据参数
    if(ID >= StartID && ID <= EndID) {
        //LogStr("19命令0 前", TempBuff, TempBuffLen);       //打印数据
        pointDiff = ReadTotalPoints(ID);               //读取差值
        arrayCompositeData(&User19_temp, &dat[5]);           //解析数据参数
//        CH375_Log(">19 命令 分机 %d  差值=>%lu 实际总分 %lu", ID, pointDiff, User19_temp.totalScore);
//        //POKER_UserTypdePrintf((u8 *)"19 命令前", User19_temp) ; //打印数据
        //判断差值是否改变
        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            u32 TmpwinScore   = User19_temp.winScore  ;      //临时总分
            u32 TmpTotalScore = User19_temp.totalScore;      //临时总分
//            CH375_Log(">显示初始转换参数 %lu %lu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
            if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //修改过了结果
                User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //计算得分
//                CH375_Log(">本局的得分是 %lu", User19_temp.winScore);
            }
            User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore, pointDiff); //计算差值后的总分
            if(ResultFlag == 0) { //本局计算差值
                ResultFlag = 1;
                pointDiff = CalculateTheDifference(pointDiff, TmpwinScore - User19_temp.winScore);           //计算差值
                SaveTotalPoints(ID, pointDiff);                                                       //差值保存
            }
            if(pointDiff != 0) { CH375_Log(">分机 %d  差值 %lu  0x%-8X", ID, pointDiff, pointDiff); }
            dataSynthesisArray(User19_temp, &dat[5]);  //数据合成
            commandDataReCRC(dat, len);                //整条命令重新校验
            GetPackMsg_19.changeFlag = 1;
            changeCommandFlag = 1;
            POKER_UserTypdePrintf((u8 *)"19 命令后", User19_temp) ; //打印数据
            //LogStr("19命令0 后", TempBuff, TempBuffLen);      //打印数据
        }
    }
}

uint8_t  TempBuff[64];    //解密数据(临时命令缓存) //单条数据
uint8_t  TempBuffLen;
uint8_t  DesBuff[64];     //解密数据(解密数据)
uint8_t  DesBuffLen;
uint8_t  CmdBuff[64];     //源数据(用于转发)
uint8_t  CmdBuffLen;
uint8_t  UserBuffLen;

u8 PrintfFlag = 0;

//CH375主程序
void CH375_APP(void)
{
    uint8_t res, i, j;
    uint16_t len;
    res = mInitCH375Host();
    if(res != USB_INT_SUCCESS) { CH375_Log("ch375 init erro\n"); }
    CH375InitSysVar();                                        //上电初始化设备信息默认值
    tm_timer_init(&debug_CHms, 100, 500);
    tm_timer_start(&debug_CHms);
    while(1) {
		if(PrintfFlag)  //打单处理
		{
			u8 temp = PrintfPlaySingle();
		    if(temp != WAIT_BUSY)
			{
			  PrintfFlag = 0;
			}
		}
        GSM_ControlStateMachine(); //通信猫状态机控制
        #if 1 //前
        if(CH375CheckConnect() == USBD_CONNECT) {             /* 刚检测到一个设备接入，需要枚举 */
            CH375_Log("Device Connect\n");
            //开始枚举操作
            res = CH375BusReset();                   /* 总线复位 */
            if(res != USB_INT_SUCCESS) { CH375_Log("Bus Reset Erro\n"); }
            delay_ms(100);                            /* 等待设备稳定 */
            /* 获取设备描述符 */
            res = CH375GetDeviceDesc(UserBuff, &len);
            if(res == USB_INT_SUCCESS) {
                CH375_Logn("设备描述符");
                for(i = 0; i < len; i++)
                { CH375_Logn("%02x ", (uint16_t)UserBuff[i]); }
                CH375_Log("\r\n");
                //12 01 00 01 00 00 00 40 42 50 00 02 2b 7f 01 02 03 01
                USBH_ParseDevDesc(&HostDevDesc, UserBuff, len); //描述符解析,获得VID,PID,PVN
            } else { CH375_Log("Get Device Descr Erro:0x%02x\n", (uint16_t)res); }
            /* 设置地址 */
            res = CH375SetDeviceAddr(2);
            if(res != USB_INT_SUCCESS)
            { CH375_Log("Set Addr Erro:0x%02x\n", (uint16_t)res); }
            /* 获取配置描述符 */
            res = CH375GetConfDesc(UserBuff, &len);
            if(res == USB_INT_SUCCESS) {
                CH375_Logn("配置描述符");
                for(i = 0; i < len; i++)
                { CH375_Logn("%02x ", (uint16_t)UserBuff[i]); }
                CH375_Log("\n");
            } else { CH375_Log("Get Conf Descr Erro:0x%02x\n", (uint16_t)res); }
            /* 设置配置 */
            res = CH375SetDeviceConf(1);
            if(res != USB_INT_SUCCESS) { CH375_Log("Set Config Erro\n"); }
            CH375_Log("USBD.status          = %02X", USBD.status);
            CH375_Log("USBD.itfmount        = %02X", USBD.itfmount);
            CH375_Log("USBD.itf[i].edpmount = %02X", USBD.itf[0].edpmount);
            CH375_Log("USBD.itf[i].edpmount = %02X", USBD.itf[1].edpmount);
        }
        if(USBD.status == USBD_READY) {     //设备初始化已完成
            //根据设备信息结构体，找中断端点，并对其发送IN包
            for(i = 0; i != USBD.itfmount; i++) {
                for(j = 0; j != USBD.itf[i].edpmount; j++) {
                    if((USBD.itf[i].edp[j].attr == 0x03) && (USBD.itf[i].edp[j].edpnum & 0x80)) {   //中断上传端点
                        res = CH375InTrans(USBD.itf[i].edp[j].edpnum & 0x0F, UserBuff, &len, 10); //对端点发IN包,NAK不重试
                        if(res == USB_INT_SUCCESS) {
                            if(len) {
                                u8 buf[65] = {0};
                                if(len < 64) {
                                    buf[0] = len;
                                    memcpy(&buf[1], UserBuff, len);
                                    TM_que_Write(&HID_TxQue, buf, len + 1); //接收数据包
                                }
                            }
                        }
                    }
                }
            }
        }
        //HID_Rx ---> Usart1_TX
        if(que_size(Tx1_que)) { //串口数据
            Usart_SendByte(COM1, que_Read(&Tx1_que));
        }
        if(que_size(Rx2_que)) { //串口数据
            Usart_SendByte(COM1, que_Read(&Rx2_que));
        }	
        if(que_size(Rx1_que)) { //串口数据
            u8 dat = que_Read(&Rx1_que);
            if(dat == 0xF1) {
                for(u8 i = 0; i < 40; i++) {
                    if(i >= StartID && i <= EndID) {
                        PlayerDifferenceValue[i].data = ReadTotalPoints(i); //默认的差值总分是0
                        User_Log(">查询[%02d]玩家的差值 %lu 0x%08X", i, PlayerDifferenceValue[i].data, PlayerDifferenceValue[i].data);
                    }
                }
            } else if(dat == 0xF2) {
                for(u8 i = 0; i < 40; i++) {
                    PlayerDifferenceValue[i].data = ReadTotalPoints(i); //默认的差值总分是0
                    User_Log(">查询[%02d]玩家的差值 %lu 0x%08X", i, PlayerDifferenceValue[i].data, PlayerDifferenceValue[i].data);
                }
            }
			else 
			
			if(dat == 0xFF) {
                PrintfFlag = 1;
            }else{
			    Usart_SendByte(COM2, dat);			
			}

			
        }
        #if 0  //TM_HostHID_Tx //## 完全数据转发
        //TM_HostHID_Tx
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //未解密的数据包
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //复制数据包
            CmdBuffLen = UserBuff[0];                       //长度
            HID_Send_Report(CmdBuff, CmdBuffLen);
        }
        #endif
        #if 0  //TM_HostHID_Tx //##提取100局路单命令,A3控制结果命令
        //TM_HostHID_Tx
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //未解密的数据包
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //复制数据包
            CmdBuffLen = UserBuff[0];                       //长度
            u8 Flag = 0;
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            for(u8 i = 0; i < 7; i++) { //解密数据
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //填充4字节密钥
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            #if 0  //完全无过滤显示
            CH375_Logn("控制板 ");
            for(i = 0; i < 56 ; i++)
            { CH375_Log("%02X ", DesBuff[i]); }
            CH375_Logn("\r\n");
            #endif
            u8 *buf = NULL;
            if(GetPackPort == 0)
            { buf = (u8 *)tm_strstr((char *)CmdPackStart[0], 4, (char *)DesBuff, 56); } //100局路单
            if(buf || (GetPackPort && GetPackPort < 10)) {
                #if 1  //完全无过滤显示
                CH375_Logn("控制板 A0+1 :");
                for(i = 0; i < 60 ; i++)
                { CH375_Logn("%02X ", DesBuff[i]); }
                CH375_Logn("\r\n");
                #endif
                if(GetPackPort == 0)     { //包头区域
                    if(HID_Pack_OneCmd(buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen) == 0) { GetPackPort = 1; }
                    for(u8 i = 0; i < TempBuffLen - 4; i++) { //修改结果(排除包头,长度,命令)
                        TempBuff[4 + i] = 0x11; //出黑桃
                    }
                    memcpy(DesOutBuff, TempBuff, TempBuffLen); //暂存
                    DesOutBuffLen = TempBuffLen;             //长度
                    memcpy(buf, TempBuff, TempBuffLen);      //返回原位置
                    #if 1  //完全无过滤显示
                    CH375_Logn("控制板 A0   :");
                    for(i = 0; i < 60 ; i++)
                    { CH375_Logn("%02X ", DesBuff[i]); }
                    CH375_Logn("\r\n");
                    #endif
                    for(u8 i = 0; i < 7; i++) { //加密数据
                        des(&DesBuff[i * 8], desPassWord, DES_Encrypt, &DesBuff[i * 8]);
                    }
                    memcpy(A0_Buff[0], DesBuff, 60); //暂存 //缓存
                    //HID_Send_Report(DesBuff, 60);
                    Flag = 1; //数据已替换
                } else if(GetPackPort == 1) {
                    HID_Pack_OneCmd(&DesBuff[1], 56, TempBuff, &TempBuffLen);
                    for(u8 i = 0; i < TempBuffLen - 2; i++) {                  //修改结果(除去包尾)
                        TempBuff[i] = 0x11; //出黑桃
                    }
                    memcpy(&DesOutBuff[DesOutBuffLen], TempBuff, TempBuffLen); //跟着上次读取到的长度
                    DesOutBuffLen += TempBuffLen;            //长度
                    u16 CRC16 = ComputeCRC16(DesOutBuff, 2, DesOutBuff[2]); //CRC校验
                    TempBuff[TempBuffLen - 2] = CRC16 >> 8 & 0xFF;
                    TempBuff[TempBuffLen - 1] = CRC16 >> 0 & 0xFF;
                    memcpy(&DesBuff[1], TempBuff, TempBuffLen);             //返回原位置
                    #if 1  //完全无过滤显示
                    CH375_Logn("控制板 A0   :");
                    for(i = 0; i < 60 ; i++)
                    { CH375_Logn("%02X ", DesBuff[i]); }
                    CH375_Logn("\r\n");
                    #endif
                    for(u8 i = 0; i < 7; i++) { //加密数据
                        des(&DesBuff[i * 8], desPassWord, DES_Encrypt, &DesBuff[i * 8]);
                    }
                    HID_Send_Report(A0_Buff[0], 60);
                    HID_Send_Report(DesBuff, 60);
                    Flag = 1; //数据已替换
                    GetPackPort = 0;
                }
                #if 0  //完全无过滤显示
                CH375_Logn("控制板 A0 :");
                for(i = 0; i < TempBuffLen ; i++)
                { CH375_Logn("%02X ", TempBuff[i]); }
                CH375_Logn("\r\n");
                #endif
            }
            //修改结果(遥控控制)
            buf = (u8 *)tm_strstr((char *)CmdPackStart[2], 4, (char *)DesBuff, 56); //上传结果
            if(buf) {
                if(HID_Pack_OneCmd(buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen) == 1) { //检查包是否完整
                    if(WillResult) {
                        if(WillResult == 5) {
                            TempBuff[11] = 0x60;
                        } else if(TempBuff[11] == 0x60) { //小王
                            TempBuff[11] = WillResult << 4 | 1; //修改结果
                        } else {
                            TempBuff[11] = (TempBuff[11] & 0x0F) | WillResult << 4; //修改结果
                        }
                        WillResult = 0;
                    }
                    u16 CRC16 = ComputeCRC16(TempBuff, 2, TempBuff[2]); //CRC校验
                    TempBuff[TempBuffLen - 2] = CRC16 >> 8 & 0xFF;
                    TempBuff[TempBuffLen - 1] = CRC16 >> 0 & 0xFF;
                    memcpy(buf, TempBuff, TempBuffLen);
                    for(u8 i = 0; i < 7; i++) { //加密数据
                        des(&DesBuff[i * 8], desPassWord, DES_Encrypt, &DesBuff[i * 8]);
                    }
                    DesBuff[56] = UserBuff[57]; //填充4字节密钥
                    DesBuff[57] = UserBuff[58];
                    DesBuff[58] = UserBuff[59];
                    DesBuff[59] = UserBuff[60];
                    HID_Send_Report(DesBuff, 60);
                    Flag = 1; //数据已替换
                    #if 1  //完全无过滤显示
                    CH375_Logn("控制板 A3 :");
                    for(i = 0; i < TempBuffLen ; i++)
                    { CH375_Logn("%02X ", TempBuff[i]); }
                    CH375_Logn("\r\n");
                    #endif
                } else {
                    CH375_Logn("控制板 A3 :Err");
                }
            }
            #if 0
            //            u8 * buf = (u8*)strstr((char*)DesBuff,"IC"); //寻找包头
            ////            if(buf){
            ////                 CH375_Log("des = %X  buf %X",DesBuff,buf);
            ////            }
            //            if(buf){
            //
            //
            //               if(buf[3] == 0xA3)
            //               {
            //                 HID_Pack_OneCmd(buf,56-(buf-DesBuff),TempBuff,&TempBuffLen);
            //                #if 1  //完全无过滤显示
            //                   CH375_Logn("控制板 A3 :");
            //                 for( i = 0; i <TempBuffLen ; i++)
            //                    CH375_Logn("%02X ",TempBuff[i]);
            //                 CH375_Logn ("\r\n");
            //                #endif
            //               }
            //               buf = (u8*)strstr((char*)buf+3,"IC"); //寻找包头
            //               if(buf[3] == 0xA3)
            //               {
            //                 HID_Pack_OneCmd(buf,56-(buf-DesBuff),TempBuff,&TempBuffLen);
            //                #if 1  //完全无过滤显示
            //                   CH375_Logn("控制板 A3 :");
            //                 for( i = 0; i <TempBuffLen ; i++)
            //                    CH375_Logn("%02X ",TempBuff[i]);
            //                 CH375_Logn ("\r\n");
            //                #endif
            //               }
            //
            //            }
            //           HID_Pack_Deal(DesBuff,56 ,&UserBuff[1],&UserBuff[0]);                  //提取数据包
            //           HID_Pack_Deal2(&UserBuffer[1],UserBuff[0] ,&DesOutBuff,DesOutBuffLen); //过滤心跳包C0命令
            //            if(UserBuffer[0])
            //            {
            //              CH375_Logn("控制板 ");
            //              for( i = 0; i <UserBuffer[0] ; i++ )
            //              CH375_Logn("%02x ",(uint16_t)UserBuffer[i+1]);
            //              CH375_Logn ("\r\n");
            //            }
            #endif
            //判断命令号0xA0 100路单结果修改
            if(WillResult && WillResult <= 5) { //改变本局结果
            }
            //判断命令号0xA2 得到本次的局数
            if(WillResult && WillResult <= 5) { //改变本局结果
            }
            //判断命令号0xA3 得到本次的结果
            if(WillResult && WillResult <= 5) { //改变本局结果
            }
            if(Flag == 0)
            { HID_Send_Report(CmdBuff, CmdBuffLen); }
        }
        #endif
        #if 0  //TM_HostHID_Tx //#仅仅过滤命令
        //TM_HostHID_Tx
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //未解密的数据包
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //复制数据包
            CmdBuffLen = UserBuff[0];                       //长度
            u8 Flag = 0;
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            for(u8 i = 0; i < 7; i++) { //解密数据
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //填充4字节密钥
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            #if 0  //完全无过滤显示
            CH375_Logn("控制板 ");
            for(i = 0; i < 56 ; i++)
            { CH375_Log("%02X ", DesBuff[i]); }
            CH375_Logn("\r\n");
            #endif
            HID_Pack_Deal(DesBuff, 56, &UserBuff[1], &UserBuff[0]);                  //提取数据包
            HID_Pack_Deal2(&UserBuff[1]  ,UserBuff[0],  &UserBuff[1],&UserBuff[0]);  //过滤心跳包C0命令
            if(UserBuff[0]) {
                CH375_Logn("控制板 ");
                for(i = 0; i < UserBuff[0] ; i++)
                { CH375_Logn("%02x ", (uint16_t)UserBuff[i + 1]); }
                CH375_Logn("\r\n");
            }
            //判断命令号0xA0 100路单结果修改
            if(WillResult && WillResult <= 5) { //改变本局结果
            }
            //判断命令号0xA2 得到本次的局数
            if(WillResult && WillResult <= 5) { //改变本局结果
            }
            //判断命令号0xA3 得到本次的结果
            if(WillResult && WillResult <= 5) { //改变本局结果
            }
            if(Flag == 0)
            { HID_Send_Report(CmdBuff, CmdBuffLen); }
        }
        #endif
        #if 0  //TM_HostHID_Tx //## 修改总分 (完成数据的解密)
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //未解密的数据包
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //复制数据包
            CmdBuffLen = UserBuff[0];                       //长度
            /* 获得解密密钥*/
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            /* 数据解密*/
            for(u8 i = 0; i < 7; i++) { //解密数据
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //填充后4字节密钥
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            //检查局数和轮数
            u8 *buf = NULL;
            if(GetPackPort == 0) {
                buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 3, (char *)DesBuff, 56);        //检查局数与轮数  0x49, 0x43, 0x0E
                if(buf) { buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 3, (char *)DesBuff, 56); } //检查局数与轮数  0x49, 0x43, 0x0E, 0xA2
                if(buf) { //局数和轮数 命令号对上
                    u8 len = 56 - (buf - DesBuff);
                    u16 temp = HID_GetOnePack(buf, len, TempBuff, &TempBuffLen);
                    if(temp & 0x0100) {  //完整包
                        StartMsg.inningNum = TempBuff[4];
                        StartMsg.BoutNum   = TempBuff[5];
                        CH375_Log("开局1 %d轮 %d局 ", StartMsg.inningNum, StartMsg.BoutNum);
                    }
                    //else if(temp&0x0200) //不完整包
                    //{
                    //    GetPackPort = 1;
                    //    memcpy(oneCmdBuff,TempBuff,TempBuffLen); //
                    //    oneCmdBuffLen = TempBuffLen;
                    //}
                }
            }
            //else if(GetPackPort>=1&&GetPackPort<5)          //拼接开局命令
            //{
            //      u16 temp = HID_GetOnePack(&DesBuff[1],56,TempBuff,&TempBuffLen);
            //      if(temp&0x0100)      //完整包
            //      {
            //          memcpy(&oneCmdBuff[oneCmdBuffLen],TempBuff,TempBuffLen); //
            //          oneCmdBuffLen += TempBuffLen;
            //          StartMsg.inningNum = TempBuff[4];
            //          StartMsg.BoutNum   = TempBuff[5];
            //          CH375_Log("开局2 %d轮 %d局 ",StartMsg.inningNum,StartMsg.BoutNum);
            //      }
            //      else if(temp&0x0200) //不完整包
            //      {
            //          GetPackPort = 0;
            //            CH375_Log("开局2 获取失败",StartMsg.inningNum,StartMsg.BoutNum);
            //      }
            //}
            //检查100路单
            #if 1  //检查11命令
            if(GetPackMsg_11.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[3], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_11, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            LogStr("11命令0 ", TempBuff, TempBuffLen); //打印数据
                            GetPackMsg_11.intactPort = 0;
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包
                            GetPackMsg_11.intactPort = 1;
                            memcpy(Pack_11.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_11.BufLen = TempBuffLen;
                            buf = NULL; //退出循环
                        }
                    }
                }
            } else if(GetPackMsg_11.intactPort) {      //拼接未完场的包
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //完整包
                    memcpy(&Pack_11.Buf[Pack_11.BufLen], TempBuff, TempBuffLen); //
                    Pack_11.BufLen += TempBuffLen;
                    LogStr("11命令1 ", Pack_11.Buf, Pack_11.BufLen); //打印数据
                } else if(temp & 0x0200) { //不完整包
                    GetPackPort = 0;
                    CH375_Log(">11命令 拼接 失败");
                }
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[3], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_11, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            LogStr("11命令2 ", TempBuff, TempBuffLen); //打印数据
                            GetPackMsg_11.intactPort = 0;
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包
                            GetPackMsg_11.intactPort = 1;
                            memcpy(Pack_11.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_11.BufLen = TempBuffLen;
                            buf = NULL; //退出循环
                        }
                    } else {
                        GetPackMsg_11.intactPort = 0;
                    }
                }
            }
            #endif
            #if 1  //检查19命令
            if(GetPackMsg_19.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            LogStr("19命令0 ", TempBuff, TempBuffLen); //打印数据
                            GetPackMsg_19.intactPort = 0;
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包
                            GetPackMsg_19.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_19.BufLen = TempBuffLen;
                            buf = NULL; //退出循环
                        }
                    }
                }
            } else if(GetPackMsg_19.intactPort) {      //拼接未完场的包
                u16 temp = HID_GetPack(&GetPackMsg_19, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //完整包
                    memcpy(&Pack_19.Buf[Pack_19.BufLen], TempBuff, TempBuffLen); //
                    Pack_19.BufLen += TempBuffLen;
                    LogStr("19命令1 ", Pack_19.Buf, Pack_19.BufLen); //打印数据
                } else if(temp & 0x0200) { //不完整包
                    GetPackMsg_19.intactPort = 0;
                    CH375_Log(">=19命令 拼接 失败");
                }
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数 0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            LogStr("19命令2 ", TempBuff, TempBuffLen); //打印数据
                            GetPackMsg_19.intactPort = 0;
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包
                            GetPackMsg_19.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_19.BufLen  = TempBuffLen;
                            buf = NULL;                               //退出循环
                        }
                    } else {
                        GetPackMsg_19.intactPort = 0;
                    }
                }
            }
            #endif
            #if 1  //检查A3命令 结果命令 (重1开始,到100)
            if(GetPackMsg_A3.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[2], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_A3, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            LogStr("A3命令  ", TempBuff, TempBuffLen); //打印数据
                            GetPackMsg_19.intactPort = 0;
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包
                            GetPackMsg_A3.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_19.BufLen = TempBuffLen;
                            buf = NULL; //退出循环
                        }
                    }
                }
            } else if(GetPackMsg_A3.intactPort) {
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //完整包
                    memcpy(&Pack_A3.Buf[Pack_A3.BufLen], TempBuff, TempBuffLen); //
                    Pack_A3.BufLen += TempBuffLen;
                    LogStr("A3命令  ", Pack_A3.Buf, Pack_A3.BufLen); //打印数据
                }
                GetPackMsg_A3.intactPort = 0;
            }
            #endif
            //转发源数据
            HID_Send_Report(CmdBuff, CmdBuffLen);
        }
        #endif
        #endif
        #if 1  //TM_HostHID_Tx //## 修改总分 (增加总分值修改)正在修改
        if(TM_que_size(HID_TxQue)) {
            TM_que_Read(&HID_TxQue, UserBuff, &CmdBuffLen); //未解密的数据包
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //复制数据包
            CmdBuffLen = UserBuff[0];                       //长度
            /* 获得解密密钥 */
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8)  & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0)  & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            /* 数据解密*/
            for(u8 i = 0; i < 7; i++) { //解密数据
                des(&CmdBuff[i * 8], desPassWord, DES_Decrypt, &DesBuff[i * 8]);
            }
            DesBuff[56] = UserBuff[57]; //填充后4字节密钥
            DesBuff[57] = UserBuff[58];
            DesBuff[58] = UserBuff[59];
            DesBuff[59] = UserBuff[60];
            u8 *buf = NULL;
            #if 1 //检查局数和轮数
            if(GetPackMsg_A2.intactPort == 0) {
                buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 3, (char *)DesBuff, 56);        //检查局数与轮数  0x49, 0x43, 0x0E
                if(buf) { buf = (u8 *)tm_strstr((char *)CmdPackStart[1], 4, (char *)DesBuff, 56); } //检查局数与轮数  0x49, 0x43, 0x0E, 0xA2
                if(buf) { //局数和轮数 命令号对上
                    u16 temp = HID_GetPack(&GetPackMsg_A2, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                    if(temp & 0x0100) {  //完整包
                        DealWithStartOrder_A2(TempBuff, TempBuffLen); //命令处理
                        CH375_Log("开局1 %d轮 %d局 ", StartMsg.inningNum, StartMsg.BoutNum);
                    }
                }
            }
            #endif
            #if 1   //检查100路单
            //拼接上一个数据包
            if(GetPackMsg_A0.intactPort) {
                u16 temp = HID_GetPack(&GetPackMsg_A0, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //完整包
                    if(CmdPack_A0.pBuf) {
                        memcpy(&CmdPack_A0.pBuf[CmdPack_A0.NowLen], TempBuff, TempBuffLen); //
                        CmdPack_A0.NowLen += TempBuffLen;
                        if(CmdPack_A0.NowLen < CmdPack_A0.MaxLen)
                        { DealWithRoadOrderResult_A0(CmdPack_A0.pBuf, CmdPack_A0.NowLen); } //处理数据
                        if(GetPackMsg_A0.changeFlag) { //数据有改变
                            GetPackMsg_A0.changeFlag = 0;
                            memcpy(&ache_Buff[0][56 - GetPackMsg_A0.recordLength], CmdPack_A0.pBuf, GetPackMsg_A0.recordLength);                               //上一个数据包
                            memcpy(&DesBuff[1], &CmdPack_A0.pBuf[GetPackMsg_A0.recordLength], TempBuffLen);                                                    //本次的数据包
                        }
                    } else {
                        log_err((u8*)"A0命令申请空间失败");
                    }
                }
				if(CmdPack_A0.pBuf) {free(CmdPack_A0.pBuf);CmdPack_A0.pBuf=NULL; CmdPack_A0.MaxLen = 0;} //释放空间
                changeCommandFlag = 1;
                GetPackMsg_A0.intactPort = 0;
            }
            //一个数据包的搜索
            if(GetPackMsg_A0.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[0], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数 0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_A0, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {        //完整包
                        } else if(temp & 0x0200) { //不完整包
                            if(CmdPack_A0.pBuf) { free(CmdPack_A0.pBuf); } //释放空间
                            CmdPack_A0.pBuf = (u8 *)malloc(120);      //申请空间
                            if(CmdPack_A0.pBuf) {
                                CmdPack_A0.MaxLen          = 120;
                                memcpy(CmdPack_A0.pBuf, TempBuff, TempBuffLen); // 暂存命令数据
                                CmdPack_A0.NowLen          = TempBuffLen;
                                GetPackMsg_A0.recordLength = TempBuffLen;       //不完整命令长度
                            } else {
                                log_err((u8*)"A0命令申请空间失败");
                            }
                            GetPackMsg_A0.intactPort   = 1;
                            changeCommandFlag          = 1;
                            buf = NULL; //退出循环
                        }
                    }
                }
            }
            #endif
            #if 1  //检查11命令
            if(GetPackMsg_11.intactPort) {      //拼接上一个数据包
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //完整包
                    memcpy(&Pack_11.Buf[Pack_11.BufLen], TempBuff, TempBuffLen); //
                    Pack_11.BufLen += TempBuffLen;
                    DealWithStakeOrder_11(Pack_11.Buf, Pack_11.BufLen); //处理数据
                    if(GetPackMsg_11.changeFlag) { //数据有改变
                        GetPackMsg_11.changeFlag = 0;
                        //本次数据包
                        memcpy(&ache_Buff[0][56 - GetPackMsg_11.recordLength], Pack_11.Buf, GetPackMsg_11.recordLength);             //上一个数据包
                        memcpy(&DesBuff[1], &Pack_11.Buf[GetPackMsg_11.recordLength], Pack_11.BufLen - GetPackMsg_11.recordLength);  //本次的数据包
                    }
                    #if 0
                    u8 ID = Pack_11.Buf[4];                              //用户
                    if(ID >= StartID && ID <= EndID) {
                        //LogStr("11命令1 前", Pack_11.Buf, Pack_11.BufLen);   //打印数据
                        POKER_UserTypde User11_temp;                           //临时的数据参数
                        u32 pointDiff = ReadTotalPoints(ID);             //读取差值
                        arrayCompositeData(&User11_temp, &Pack_11.Buf[5]);     //解析数据参数
                        CH375_Log("11 命令 分机 %d  差值=>%lu 实际总分 %lu", ID, pointDiff, User11_temp.totalScore);
                        //POKER_UserTypdePrintf((u8 *)"11 命令前", User11_temp) ; //打印数据
                        if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //本局修改得分
                            User11_temp.winScore = CalculateTheMagnificationScore(User11_temp, ResultMsg.ChangeResult); //计算得分
                            CH375_Log(">本局的得分是 %lu", User11_temp.winScore);
                        }
                        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
                            User11_temp.totalScore = TotalScoreAfterGettingTheDifference(User11_temp.totalScore, pointDiff); //计算差值后的总分
                            POKER_UserTypdePrintf((u8 *)"11 命令后", User11_temp) ; //打印数据
                            dataSynthesisArray(User11_temp, &Pack_11.Buf[5]);  //数据合成
                            commandDataReCRC(Pack_11.Buf, Pack_11.BufLen);     //整条命令重新校验
                            memcpy(&ache_Buff[0][56 - GetPackMsg_11.recordLength], Pack_11.Buf, GetPackMsg_11.recordLength);
                            //本次数据包
                            memcpy(&DesBuff[1], &Pack_11.Buf[GetPackMsg_11.recordLength], Pack_11.BufLen - GetPackMsg_11.recordLength);
                            //LogStr("11命令1 后", Pack_11.Buf, Pack_11.BufLen); //打印数据
                        }
                    }
                    #endif
                    changeCommandFlag = 1;
                } else if(temp & 0x0200) { //不完整包
                    GetPackPort = 0;
                    CH375_Log(">11命令 拼接 失败");
                }
                GetPackMsg_11.intactPort = 0;
            }
            //一个数据包的搜索
            if(GetPackMsg_11.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[3], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数 0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_11, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            DealWithStakeOrder_11(TempBuff, TempBuffLen); //处理数据
                            if(GetPackMsg_11.changeFlag) { //数据有改变
                                GetPackMsg_11.changeFlag = 0;
                                //放回原数据
                                memcpy(buf, TempBuff, TempBuffLen);
                            }
                            GetPackMsg_11.intactPort = 0;
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包
                            GetPackMsg_11.intactPort   = 1;
                            changeCommandFlag          = 1;
                            memcpy(Pack_11.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_11.BufLen             = TempBuffLen;
                            GetPackMsg_11.recordLength = TempBuffLen;   //不完整命令长度
                            buf = NULL; //退出循环
                        }
                    }
                }
            }
            #endif
            #if 1  //检查19命令
            if(GetPackMsg_19.intactPort) {      //拼接数据包
                u16 temp = HID_GetPack(&GetPackMsg_19, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //完整包
                    memcpy(&Pack_19.Buf[Pack_19.BufLen], TempBuff, TempBuffLen); //
                    Pack_19.BufLen     += TempBuffLen;
                    DealWithStakeOrder_19(Pack_19.Buf, Pack_19.BufLen);
                    if(GetPackMsg_19.changeFlag) {
                        GetPackMsg_19.changeFlag = 0;
                        memcpy(&ache_Buff[0][56 - GetPackMsg_19.recordLength], Pack_19.Buf, GetPackMsg_19.recordLength);
                        memcpy(&DesBuff[1], &Pack_19.Buf[GetPackMsg_19.recordLength], Pack_19.BufLen - GetPackMsg_19.recordLength);//本次数据包
                    }
                    //                    u8 ID = Pack_19.Buf[4];                                 //用户
                    //                    if(ID >= StartID && ID <= EndID) {
                    //                        //LogStr("19命令1 前", Pack_19.Buf, Pack_19.BufLen);  //打印数据
                    //                        POKER_UserTypde User19_temp;                        //临时的数据参数
                    //                        u32 pointDiff = ReadTotalPoints(ID);     //读取差值
                    //                        arrayCompositeData(&User19_temp, &Pack_19.Buf[5]); //解析数据参数
                    //                        CH375_Log(">19 命令 分机 %d  差值=>%lu 实际总分 %lu", ID, pointDiff,User19_temp.totalScore);
                    //                        //POKER_UserTypdePrintf((u8 *)"19 命令前", User19_temp) ; //打印数据
                    //                        //判断差值是否改变
                    //                        if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
                    //                            u32 TmpwinScore   = User19_temp.winScore  ;      //临时总分
                    //                            u32 TmpTotalScore = User19_temp.totalScore;      //临时总分
                    //                          CH375_Log(">显示初始转换参数 %lu %lu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
                    //                            if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //修改过了结果
                    //                                CH375_Log(">结果修改计算");
                    //                                User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //计算得分
                    //                                CH375_Log(">本局的得分是 %lu", User19_temp.winScore);
                    //                            }
                    //                            User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore, pointDiff); //计算差值后的总分
                    //                            if(ResultFlag == 0) { //本局计算差值
                    //                                ResultFlag = 1;
                    //                                pointDiff = CalculateTheDifference(pointDiff, TmpwinScore - User19_temp.winScore);
                    //                                SaveTotalPoints(ID, pointDiff);        //差值保存
                    //                            }
                    //                            if(pointDiff != 0) { CH375_Log(">分机 %d  差值 %lu  0x%-8X", ID, pointDiff, pointDiff); }
                    //                            dataSynthesisArray(User19_temp, &Pack_19.Buf[5]);   //数据合成
                    //                            commandDataReCRC(Pack_19.Buf, Pack_19.BufLen);      //整条命令重新校验
                    //                            POKER_UserTypdePrintf((u8 *)"19 命令后", User19_temp) ; //打印数据
                    //                            //LogStr("19命令1 后", Pack_19.Buf, Pack_19.BufLen);      //打印数据
                    //                            memcpy(&ache_Buff[0][56 - GetPackMsg_19.recordLength], Pack_19.Buf, GetPackMsg_19.recordLength);
                    //                            //本次数据包
                    //                            memcpy(&DesBuff[1], &Pack_19.Buf[GetPackMsg_19.recordLength], Pack_19.BufLen - GetPackMsg_19.recordLength);
                    //                            //LogStr("19命令1 后", TempBuff, TempBuffLen);      //打印数据
                    //                        }
                    //                    }
                    changeCommandFlag = 1;
                } else if(temp & 0x0200) { //不完整包
                    GetPackMsg_19.intactPort = 0;
                    CH375_Log(">=19命令 拼接 失败");
                }
                GetPackMsg_19.intactPort = 0;
            }
            if(GetPackMsg_19.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            DealWithStakeOrder_19(TempBuff, TempBuffLen);
                            if(GetPackMsg_19.changeFlag) {
                                GetPackMsg_19.changeFlag = 0;
                                memcpy(buf, TempBuff, TempBuffLen);
                            }
                            //                            u8 ID = TempBuff[4];                                     //用户
                            //                            if(ID >= StartID && ID <= EndID) {
                            //                                //LogStr("19命令0 前", TempBuff, TempBuffLen);       //打印数据
                            //                                POKER_UserTypde User19_temp;                         //临时的数据参数
                            //                                u32 pointDiff = ReadTotalPoints(ID);           //读取差值
                            //                                arrayCompositeData(&User19_temp, &TempBuff[5]);      //解析数据参数
                            //                                CH375_Log(">19 命令 分机 %d  差值=>%lu 实际总分 %lu", ID, pointDiff,User19_temp.totalScore);
                            //                                //POKER_UserTypdePrintf((u8 *)"19 命令前", User19_temp) ; //打印数据
                            //                                //判断差值是否改变
                            //                                if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
                            //                                    u32 TmpwinScore   = User19_temp.winScore  ;      //临时总分
                            //                                    u32 TmpTotalScore = User19_temp.totalScore;      //临时总分
                            //                                    CH375_Log(">显示初始转换参数 %lu %lu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
                            //                                    if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //修改过了结果
                            //                                        User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //计算得分
                            //                                        CH375_Log(">本局的得分是 %lu", User19_temp.winScore);
                            //                                    }
                            //                                    User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore, pointDiff); //计算差值后的总分
                            //                                    if(ResultFlag == 0) { //本局计算差值
                            //                                        ResultFlag = 1;
                            //                                        pointDiff = CalculateTheDifference(pointDiff, TmpwinScore - User19_temp.winScore);
                            //                                        SaveTotalPoints(ID, pointDiff);                                //差值保存
                            //                                    }
                            //                                    if(pointDiff != 0) { CH375_Log(">分机 %d  差值 %lu  0x%-8X", ID, pointDiff, pointDiff); }
                            //                                    dataSynthesisArray(User19_temp, &TempBuff[5]);  //数据合成
                            //                                    commandDataReCRC(TempBuff, TempBuffLen);        //整条命令重新校验
                            //                                    //放回原数据
                            //                                    memcpy(buf, TempBuff, TempBuffLen);
                            //                                    changeCommandFlag = 1;
                            //                                    POKER_UserTypdePrintf((u8 *)"19 命令后", User19_temp) ; //打印数据
                            //                                    //LogStr("19命令0 后", TempBuff, TempBuffLen);      //打印数据
                            //                                }
                            //                            }
                            GetPackMsg_19.intactPort  = 0;
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包,要保存好数据包
                            GetPackMsg_19.intactPort   = 1;
                            changeCommandFlag          = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_19.BufLen             = TempBuffLen;
                            GetPackMsg_19.recordLength = TempBuffLen;
                            buf = NULL; //退出循环
                        }
                    }
                }
            }
            #if 0
            //if(GetPackMsg_19.intactPort) {      //拼接未完场的包
            //      u16 temp = HID_GetPack(&GetPackMsg_19, &DesBuff[1], 55, TempBuff, &TempBuffLen);
            //      if(temp & 0x0100) {  //完整包
            //          memcpy(&Pack_19.Buf[Pack_19.BufLen], TempBuff, TempBuffLen); //
            //          Pack_19.BufLen     += TempBuffLen;
            //          u8 ID = Pack_19.Buf[4];                                 //用户
            //          if(ID >= StartID && ID <= EndID) {
            //              //LogStr("19命令1 前", Pack_19.Buf, Pack_19.BufLen);  //打印数据
            //              POKER_UserTypde User19_temp;                        //临时的数据参数
            //              u32 pointDiff = ReadTotalPoints(ID);     //读取差值
            //              arrayCompositeData(&User19_temp, &Pack_19.Buf[5]); //解析数据参数
            //              CH375_Log(">获取原数据 ID = %d  现在的差值>%lu", ID, pointDiff);
            //              POKER_UserTypdePrintf((u8*)"19 命令前",User19_temp) ;  //打印数据
            //              //判断差值是否改变
            //              if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            //                  u32 TmpwinScore   = User19_temp.winScore  ;      //临时总分
            //                  u32 TmpTotalScore = User19_temp.totalScore;      //临时总分
            //                  CH375_Log(">显示初始转换参数 %llu %llu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
            //                  if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {  //修改过了结果
            //                      CH375_Log(">结果修改计算");
            //                      User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //计算得分
            //                      CH375_Log(">本局的得分是 %lu", User19_temp.winScore);
            //                  }
            //                  User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore,pointDiff); //计算差值后的总分
            //                  if(ResultFlag == 0) { //本局计算差值
            //                      ResultFlag = 1;
            //                      pointDiff = CalculateTheDifference(pointDiff,TmpwinScore - User19_temp.winScore);
            //                      SaveTotalPoints(ID, pointDiff);        //差值保存
            //                  }
            //                  if(pointDiff != 0) { CH375_Log(">分机 %d  差值 %lu  0x%8X", ID, pointDiff, pointDiff); }
            //                  dataSynthesisArray(User19_temp, &Pack_19.Buf[5]);   //数据合成
            //                  commandDataReCRC(Pack_19.Buf, Pack_19.BufLen);      //整条命令重新校验
            //                  POKER_UserTypdePrintf((u8*)"19 命令后",User19_temp) ;  //打印数据
            //                  //LogStr("19命令1 后", Pack_19.Buf, Pack_19.BufLen);      //打印数据
            //                  memcpy(&ache_Buff[0][56 - GetPackMsg_19.recordLength], Pack_19.Buf, GetPackMsg_19.recordLength);
            //                  //本次数据包
            //                  memcpy(&DesBuff[1], &Pack_19.Buf[GetPackMsg_19.recordLength], Pack_19.BufLen - GetPackMsg_19.recordLength);
            //                  //LogStr("19命令1 后", TempBuff, TempBuffLen);      //打印数据
            //              }
            //          }
            //          changeCommandFlag = 1;
            //      } else if(temp & 0x0200) { //不完整包
            //          GetPackMsg_19.intactPort = 0;
            //          CH375_Log(">=19命令 拼接 失败");
            //      }
            //      buf = DesBuff;
            //      while(buf) {
            //          buf = (u8 *)tm_strstr((char *)CmdPackStart[4], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数 0x49, 0x43, 0x0E
            //          if(buf) {
            //              u16 temp = HID_GetPack(&GetPackMsg_19, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
            //              if(temp & 0x0100) {  //完整包
            //                  u8 ID = TempBuff[4];                            //用户
            //                  if(ID >= StartID && ID <= EndID) {
            //                      //LogStr("19命令2 前", TempBuff, TempBuffLen); //打印数据
            //                      POKER_UserTypde User19_temp;                    //临时的数据参数
            //                      u32 pointDiff = ReadTotalPoints(ID);  //读取差值
            //                      arrayCompositeData(&User19_temp, &TempBuff[5]); //解析数据参数
            //                      CH375_Log(">获取原数据 ID = %d  现在的差值>%llu", ID, pointDiff);
            //                      POKER_UserTypdePrintf((u8*)"19 命令前",User19_temp) ;  //打印数据
            //                      //判断差值是否改变
            //                      if(pointDiff != 0 || ResultMsg.Result != ResultMsg.ChangeResult) {
            //                          u32 TmpwinScore   = User19_temp.winScore  ;      //临时总分
            //                          u32 TmpTotalScore = User19_temp.totalScore;      //临时总分
            //                          CH375_Log(">显示初始转换参数 %llu %llu = %lu %lu", TmpTotalScore, TmpwinScore, User19_temp.totalScore, User19_temp.winScore);
            //                          if(CalculationResultFlag && ResultMsg.Result != ResultMsg.ChangeResult) {   //修改过了结果
            //                              User19_temp.winScore = CalculateTheMagnificationScore(User19_temp, ResultMsg.ChangeResult); //计算得分
            //                              if(pointDiff != 0) { CH375_Log(">分机 %d  差值 %llu", ID, pointDiff); }
            //                              CH375_Log(">本局的得分是 %lu", User19_temp.winScore);
            //                          }
            //                          User19_temp.totalScore = TotalScoreAfterGettingTheDifference(User19_temp.totalScore,pointDiff); //计算差值后的总分
            //                          if(ResultFlag == 0) { //本局计算差值
            //                              ResultFlag = 1;
            //                            pointDiff = CalculateTheDifference(pointDiff,TmpwinScore - User19_temp.winScore);
            //                              SaveTotalPoints(ID, pointDiff);        //差值保存
            //                          }
            //                        if(pointDiff != 0) { CH375_Log(">分机 %d  差值 %lu  0x%8X", ID, pointDiff, pointDiff); }
            //                          dataSynthesisArray(User19_temp, &TempBuff[5]);  //数据合成
            //                          commandDataReCRC(TempBuff, TempBuffLen);        //整条命令重新校验
            //                          //放回原数据
            //                          memcpy(buf, TempBuff, TempBuffLen);
            //                          changeCommandFlag = 1;
            //                          POKER_UserTypdePrintf((u8*)"19 命令后",User19_temp) ;  //打印数据
            //                          //LogStr("19命令2 后", TempBuff, TempBuffLen);      //打印数据
            //                      }
            //                  }
            //                  GetPackMsg_19.intactPort = 0;
            //                  buf ++; //偏移
            //              } else if(temp & 0x0200) {                      //不完整包
            //                  GetPackMsg_19.intactPort = 1;
            //                  memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // 暂存命令数据
            //                  Pack_19.BufLen             = TempBuffLen;
            //                  GetPackMsg_19.recordLength = TempBuffLen;
            //                  changeCommandFlag          = 1;             //保存上一个数据包
            //                  buf = NULL;                                 //退出循环
            //              }
            //          } else {
            //              GetPackMsg_19.intactPort = 0;
            //          }
            //      }
            //  }
            #endif
            #endif
            #if 1  //检查A3命令 本局结果 (重1开始,到100)
            if(GetPackMsg_A3.intactPort == 0) {
                buf = DesBuff;
                while(buf) {
                    buf = (u8 *)tm_strstr((char *)CmdPackStart[2], 4, (char *)buf, 56 - (buf - DesBuff));   //检查局数与轮数  0x49, 0x43, 0x0E
                    if(buf) {
                        u16 temp = HID_GetPack(&GetPackMsg_A3, buf, 56 - (buf - DesBuff), TempBuff, &TempBuffLen);
                        if(temp & 0x0100) {  //完整包
                            //LogStr("A3命令1  前", TempBuff, TempBuffLen); //打印数据
                            GetPackMsg_19.intactPort = 0;
                            DealWithResultOrder_A3(TempBuff, TempBuffLen); //数据处理
                            if(GetPackMsg_A3.changeFlag) {                 //数据包有修改过
                                GetPackMsg_A3.changeFlag = 0;
                                memcpy(buf, TempBuff, TempBuffLen);   //放回原位置
                            }
                            CH375_Log(">结果命令  原 %02X   新 %02X", ResultMsg.Result, ResultMsg.ChangeResult);
                            //<保存结果>
                            buf ++; //偏移
                        } else if(temp & 0x0200) { //不完整包
                            GetPackMsg_A3.intactPort = 1;
                            memcpy(Pack_19.Buf, TempBuff, TempBuffLen); // 暂存命令数据
                            Pack_19.BufLen             = TempBuffLen;
                            GetPackMsg_A3.recordLength = TempBuffLen;
                            changeCommandFlag          = 1; //保存上一个数据包
                            buf = NULL; //退出循环
                        }
                    }
                }
            } else if(GetPackMsg_A3.intactPort) {
                u16 temp = HID_GetPack(&GetPackMsg_11, &DesBuff[1], 55, TempBuff, &TempBuffLen);
                if(temp & 0x0100) {  //完整包
                    memcpy(&Pack_A3.Buf[Pack_A3.BufLen], TempBuff, TempBuffLen); //
                    Pack_A3.BufLen += TempBuffLen;
                    DealWithResultOrder_A3(Pack_A3.Buf, Pack_A3.BufLen); //数据处理
                    if(GetPackMsg_A3.changeFlag) {            //数据包有修改过
                        GetPackMsg_A3.changeFlag = 0;
                        //放回原位置
                        memcpy(&ache_Buff[56 - GetPackMsg_A3.recordLength], TempBuff, GetPackMsg_A3.recordLength);               //上一个数据包
                        memcpy(&DesBuff[1], &TempBuff[GetPackMsg_A3.recordLength], Pack_A3.BufLen - GetPackMsg_A3.recordLength); //本次数据包
                    }
                    changeCommandFlag = 1; //数据有修改
                }
                GetPackMsg_A3.intactPort = 0;
            }
            #endif
            #if 1  //检查状态
            //{0x49, 0x43, 0x02, 0xB0}, //<5>开始下注命令
            //{0x49, 0x43, 0x04, 0xB1}, //<6>倒计时
            //{0x49, 0x43, 0x02, 0xB2}, //<7>停止下注命令
            //{0x49, 0x43, 0x03, 0xB3}, //<8>计算中奖分
            //{0x49, 0x43, 0x03, 0xB4}, //<9>结束中奖
            for(u8 i = 0; i < 5; i++) {
                buf = DesBuff;
                buf = (u8 *)tm_strstr((char *)CmdPackStart[5 + i], 4, (char *)buf, 56 - (buf - DesBuff)); //检查局数与轮数  0x49, 0x43, 0x0E
                if(buf) {
                    u8 tmpBuf[60] = {0};
                    u8 tmpBufLen = 0 ;
                    u16 temp = HID_GetPack(&GetPackMsg_state, buf, 56 - (buf - DesBuff), tmpBuf, &tmpBufLen);
                    if(temp & 0x0100) {  //完整包
                        if(tmpBuf[3] == 0xB0) {
                            CH375_Log(">0xB0 开始下注命令   收到");
                        } else if(tmpBuf[3] == 0xB1) {
                            if(tmpBuf[4] == tmpBuf[5] || tmpBuf[4] == 0)
                            { CH375_Log(">0xB1 倒计时命令%d秒 收到 ", tmpBuf[4]); }
                        } else if(tmpBuf[3] == 0xB2) {
                            CH375_Log(">0xB2 停止下注命令   收到");
                        } else if(tmpBuf[3] == 0xB3) {
                            CH375_Log(">0xB3 计算中奖分     收到");
                            CalculationResultFlag = 1;
                        } else if(tmpBuf[3] == 0xB4) {
                            //CH375_Log(">0xB4 结束中奖       收到");
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
                //转发源数据
                HID_Send_Report(CmdBuff, CmdBuffLen);
            }
            //            //转发源数据
            //            HID_Send_Report(CmdBuff, CmdBuffLen);
        }
        #endif
        #if 1  //TM_HostHID_Rx //## 上位机发送，控制板接收
        if(TM_que_size(HID_RxQue)) {
            TM_que_Read(&HID_RxQue, UserBuff, &CmdBuffLen); //未解密的数据包
            memcpy(CmdBuff, &UserBuff[1], UserBuff[0]);     //复制数据包到CmdBuff
            CmdBuffLen = UserBuff[0];                       //长度
            HostHID_SendReport(&UserBuff[1], UserBuff[0]);  //转发包
            #if 0 //上位机解密数据
            desPassWord[0] = UserBuff[57];
            desPassWord[1] = UserBuff[58];
            desPassWord[2] = UserBuff[59];
            desPassWord[3] = UserBuff[60];
            desPassWord[4] = (HostDevDesc.idVendor >> 8) & 0xFF;
            desPassWord[5] = (HostDevDesc.idVendor >> 0) & 0xFF;
            desPassWord[6] = (HostDevDesc.idProduct >> 8) & 0xFF;
            desPassWord[7] = (HostDevDesc.idProduct >> 0) & 0xFF;
            for(u8 i = 0; i < 7; i++) { //解密数据
                des(&UserBuff[1 + i * 8], desPassWord, DES_Decrypt, &UserBuff[1 + i * 8]);
            }
            MHID_Pack_Deal(&UserBuff[1], 56, &UserBuff[1], &UserBuff[0]); //过滤命令数据 提取命令
            if(UserBuff[0]) {
                CH375_Logn("上位机 ");
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
    if(CH375CheckConnect() == USBD_CONNECT) {          /* 刚检测到一个设备接入，需要枚举 */
        CH375_Log("Device Connect\n");
        //开始枚举操作
        res = CH375BusReset();                   /* 总线复位 */
        if(res != USB_INT_SUCCESS) { CH375_Log("Bus Reset Erro\n"); }
        delay_ms(50);                            /* 等待设备稳定 */
        /* 获取设备描述符 */
        res = CH375GetDeviceDesc(UserBuffer, &len);
        if(res == USB_INT_SUCCESS) {
            CH375_Logn("设备描述符");
            for(i = 0; i < len; i++)
            { CH375_Logn("%02x ", (uint16_t)UserBuffer[i]); }
            CH375_Log("\r\n");
        } else { CH375_Log("Get Device Descr Erro:0x%02x\n", (uint16_t)res); }
        /* 设置地址 */
        res = CH375SetDeviceAddr(2);
        if(res != USB_INT_SUCCESS)
        { CH375_Log("Set Addr Erro:0x%02x\n", (uint16_t)res); }
        /* 获取配置描述符 */
        res = CH375GetConfDesc(UserBuffer, &len);
        if(res == USB_INT_SUCCESS) {
            CH375_Logn("配置描述符");
            for(i = 0; i < len; i++)
            { CH375_Logn("%02x ", (uint16_t)UserBuffer[i]); }
            CH375_Log("\n");
        } else { CH375_Log("Get Conf Descr Erro:0x%02x\n", (uint16_t)res); }
        /* 设置配置 */
        res = CH375SetDeviceConf(1);
        if(res != USB_INT_SUCCESS) { CH375_Log("Set Config Erro\n"); }
    }
    if(USBD.status == USBD_READY) {     //设备初始化已完成
        //根据设备信息结构体，找中断端点，并对其发送IN包
        for(i = 0; i != USBD.itfmount; i++) {
            if(USBD.itf[i].edpmount == 0)
                for(j = 0; j != USBD.itf[i].edpmount; j++) {
                    if((USBD.itf[i].edp[j].attr == 0x03) && (USBD.itf[i].edp[j].edpnum & 0x80)) { //中断上传端点
                        res = CH375InTrans(USBD.itf[i].edp[j].edpnum & 0x0F, UserBuffer, &len, 0);  //对端点发IN包,NAK不重试
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

