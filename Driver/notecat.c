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

#define MaxSms  5 //最大的短信号


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

//通信猫接收数据信息结构
typedef struct _NoteCat_Rec {
    u8 *pRecDat        ; //保存数据
    u16 NowCount       ; //接收数据计数
    u16 Cnt            ;
    u16 MaxReceive     ; //最大接收数据量
    u16 MaxCmdBuf      ; //开辟的空间
    u16 OutTime        ; //超时计时
    u16 MaxOutTime     ; //最大超时时间
    u16 MaxTimeCnt     ; //最大超时时间
    u8  RecFlag        ; //接收标志	
} AcceptDataType;


//短信接收包数据解析结构
typedef struct _NoteCat_SmsPack { 
    u16 ID;
    u64 PhoneNumber;
    u8 *pPack      ;
    u16 PackLen    ;
} SmsMsgType;


u64 AdministratorPhone = 0;

u8 SmsSendPack[300]={0};
SmsMsgType  SmsMsg     = {0, 0, NULL, 0};         //接收短信解析
SendSmsType SendSms    = {NULL,0,0};            //发送短信内容
AcceptDataType NoteCat = {NULL, 0, 0, 0, 0}; //默认参数
u8 GSM_Buf[200];


static     TM_Timer GSM_NextTime ;//设置命令间隔
static u8  GSM_ConnectFlag = UNCONNECTION;



void USART3_IRQHandler(void)
{
	#if 1
    if(USART3->SR & (1 << 5)) {
        u8 dat = USART3->DR;
        if(NoteCat.pRecDat !=NULL && !NoteCat.RecFlag && NoteCat.MaxReceive) {
            if(NoteCat.Cnt < NoteCat.MaxCmdBuf) {       //过滤原命令
                NoteCat.Cnt++;
                NoteCat.OutTime = 250;
            } else if( NoteCat.NowCount < NoteCat.MaxReceive) { //数据包
                if(NoteCat.OutTime) { NoteCat.OutTime = NoteCat.MaxOutTime; } //数据间隔
                NoteCat.pRecDat[NoteCat.NowCount]  = dat;
                NoteCat.NowCount++;
				if(NoteCat.NowCount < NoteCat.MaxReceive)
				NoteCat.pRecDat[NoteCat.NowCount] = 0;
            } else {
                NoteCat.RecFlag = 1; //接收完成
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
*dest    要被检索的内存起始地址
DestSize 检索匹配大小
*str     要搜索的内存起始地址
StrSize  搜索的大小
返回 :成功==>第一次出现的位置地址  失败==>NULL
*/
// 内存数据对比匹配(正叙查询第一个匹配数据串)
/*   要被检索的内存起始地址,检索匹配大小,要搜索的内存起始地址,搜索的大小*/
char   *NoteCat_strstr(char *dest, unsigned int  DestSize,  char *str, unsigned int  StrSize)
{
    char *ptr = NULL;
    char *sr1 = (char *)dest;
    char *sr2 = (char *)str;
    unsigned int m1 = 0;
    unsigned int m2 = 0;
    if(dest == NULL || str == NULL || DestSize > StrSize) { return NULL; }
    for(unsigned int i = 0; i < StrSize; i++) { //暴力查询
        m1 = 0;
        m2 = 0;
        ptr = &sr2[i]; //起始位置
        while(m1 != DestSize && sr1[m1] == ptr[m2] && i + m2 < StrSize) {
            m1++;
            m2++;
        }
        if(m1 == DestSize) { return (char *)ptr; } //对比成功
    }
    return NULL;
}


//基础函数:通信猫超时计数,判断一个数据包是否完成
void NoteCat_OutTimeTiming(void)
{
    if(NoteCat.OutTime)         { NoteCat.OutTime--;   } //计时
    if(NoteCat.OutTime == 1)    { NoteCat.RecFlag = 1; } //数据包完成
	if(NoteCat.MaxTimeCnt)NoteCat.MaxTimeCnt--;
	if(NoteCat.MaxTimeCnt == 1) { NoteCat.RecFlag = 1; } //数据包完成
}


//void USART1_IRQHandler(void)
//{
//    if(USART1->SR & (1 << 5)) {
//        u8 dat = USART1->DR;
//        que_Write(&Rx1_que, dat);
//    }
//}


//基础函数:准备开始接收数据开始注册 (数据量申请的空间, 最大超时时间)
u8 NoteCat_ReadyRegistered( u16 MaxOutTime)
{
//    if(NoteCat.pRecDat) { free(NoteCat.pRecDat); NoteCat.pRecDat = NULL;} //释放空间
	if(NoteCat.pRecDat) { NoteCat.pRecDat = NULL;} //释放空间
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
//    if(NoteCat.pRecDat) { free(NoteCat.pRecDat); NoteCat.pRecDat = NULL;} //释放空间
	if(NoteCat.pRecDat) { NoteCat.pRecDat = NULL;} //释放空间
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

//基础函数:注销接收的数据信息
void NoteCat_ReadyUnRegistered(void)
{
//    if(NoteCat.pRecDat) { free(NoteCat.pRecDat); NoteCat.pRecDat= NULL; } //释放空间
	if(NoteCat.pRecDat) { NoteCat.pRecDat = NULL;} //释放空间
    NoteCat.RecFlag    = 0;
    NoteCat.MaxReceive = 0;
    NoteCat.NowCount   = 0;
}
//检查注册标记
u8  GetNoteCat_RecFlag(void)
{
    return NoteCat.RecFlag;
}
void ClearNoteCat_RecFlag(void)
{
    NoteCat.RecFlag = 0;
}

//基础函数:通信猫发送一字节数据
void NoteCat_SendByte(u8 dat)
{
    Usart_SendByte(COM3, dat);
}
//基础函数:通信猫发送一串数据
void NoteCat_SendString(u8 *dat, u16 len)
{
    while(len--) {
        Usart_SendByte(COM3, *dat++);
    }
}

//转换命令输出
void  NoteCat_PrintfCmd(char *fmt, ...)
{
    volatile u8  buf[100];
    u8 len;
    va_list ap;                                   //建立变参列表
    va_start(ap, fmt);                            //初始化列表
    vsnprintf((char *)buf, sizeof(buf), fmt, ap); //转换为字符
    va_end(ap);                                   //结束列表
    len = strlen((char *)buf);                    //计算数据长度
    for(u8 i = 0; i < len; i++) {
        NoteCat_SendByte(buf[i]);
    }
}


#if 0
//01.设置控制号码         SET13562781133  返回 OK13562781133   //只处理这个手机号码
//02.补单                 PRINT           返回 PRINT-OK
//03.取消所有操作         CANCEL          返回 CANCEL-OK
//04.控制本局吃大陪小     EAT             返回 EAT-OK
//05.本局黑桃             SPADE           返回 SPADE-OK
//06.本局红桃             HEARTS          返回 HEARTS-OK
//07.本局梅花             CLUB            返回 CLUB-OK
//08.本局方块             BLOCK           返回 BLOCK-OK
//09.本局王               KING            返回 KING-OK
//10.查询所有分机差值帐目 INQUIRE         返回 11:0,12:1000,13:-3000,14:0 ----40:0
//11.清除所有分机差值帐目 CLEAR           返回 CLEAR-OK

注意:
如果本局出的奖项为特殊奖项时，不能改变结果, 本局改变结果操作无效

#endif

//命令头
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
//测试命令 AT+<x>=? 该命令用于查询设置命令或内部程序设置的参数以及其取值范围。
//查询命令 AT+<x>?    该命令用于返回参数的当前值。
//设置命令 AT+<x>=<...> 该命令用于设置用户自定义的参数值。
//执行命令 AT+<x>   该命令用于读取受GSM 模块内部程序控制的不可变参数

#if 0
//AT + CSQ = ?          == => 查询信号
//+CSQ : (0 - 31, 99), (0 - 7, 99)
//+CSQ:  **, ##
//+CSQ:  <sddi>,<ber>
//**应在 0 到 31 之间（99表示无信号），数值越大表明信号质量越好；
//##为误码率,正常在99，值在 0 到 99 之间。否则应检查天线或 SIM 卡是否正确安装


//基本操作
//AT\r\n                   发送AT测试指令
//AT + CSQ\r\n             GSM信号检测指令
//AT + CMGF = 1\r\n        设置TXT方式发送短信

////发送短信
//AT + CMGS = 18924294786\r\n  发送接收方手机号码
//> 短信内容
//1A(结束)

////接收短信
//AT + CMGR =              读某条短信   = ? (测试)(1 - 8) //短信的位置
//AT + CMGD =              删除某条短信 = ? (测试)(1 - 8)


//测试命令
> AT + CMGL = ?
              +CMGL : ("REC UNREAD", "REC READ", "STO UNSENT", "STO SENT", "ALL")
              OK
              //获取短信的列表
              //AT + CMGL = ALL   == > 查询短信列表
              //AT + CMGL = ALL
              //+ CMGL : 1, "REC READ", "+8618924294786",, "21/01/15,17:09:41+32"
              //123456
              //+ CMGL : 2, "REC READ", "+8618924294786",, "21/01/15,17:11:56+32"
              //123
              //OK
              //? AT + CMGL = ? //测试命令
              //? AT + CMGL = ?
              // +CMGL : ("REC UNREAD", "REC READ", "STO UNSENT", "STO SENT", "ALL")
              //  OK
#endif


void GSM_SendCmd(u16 MaxOutTime, char *fmt, ...)
{
      volatile u8  buf[100];
      u16 len;
      va_list ap;                                     //建立变参列表
      va_start(ap, fmt);                              //初始化列表
      vsnprintf((char *)buf, sizeof(buf), fmt, ap);   //转换为字符
      va_end(ap);                                     //结束列表
      len = strlen((char *)buf);                      //计算数据长度
	  NoteCat_RecDatUser(  MaxOutTime);
      NoteCat_SendString((u8 *)buf, len);             //发送出去
}	


//命令测试     (检查字串,命令字串)  BasicCommands
u8 GSM_BasicCommands(u8 *dest, u8 *cmdStr)
{
    static u8 Port  = 0; //流程控制
    u8 state = ST_Busy;
    switch(Port) {
        case 0: { //发送命令
			GSM_SendCmd(500, "%s", cmdStr);
			Port = 1;
        }
        break;
        case 1: { //处理数据 (检查特定字串)
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

//命令测试     (检查字串,命令字串)  BasicCommands
u8 GSM_CMRG(u8 NumSms)
{
    static u8 Port  = 0; //流程控制
    u8 state = ST_Busy;
    switch(Port) {
        case 0: { //发送命令
		    GSM_SendCmd(500,  "AT+CMGR=%d\r\n",NumSms);
			Port = 1;
        }
        break;
        case 1: { //处理数据 (检查特定字串)
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

//检查GSM连接
u8 checkGSM_Connect(void)
{
    u8 state = GSM_BasicCommands((u8 *)"OK", (u8 *)"AT\r\n");
    return state;
}

//获得模块的信号量
u8 GetGSM_CSQ(void)
{
    u8 state = GSM_BasicCommands((u8 *)"OK", (u8 *)"AT+CSQ\r\n");
    if(state == ST_Success) {
        u16 rssi = 0;
        u16 ber  = 0;
        u8 *pstr = (u8 *) NoteCat_strstr("+CSQ:", strlen("+CSQ:"), (char *)NoteCat.pRecDat, NoteCat.MaxReceive); //寻找起头
        if(pstr) {
            u8 Cnt = sscanf((char *)pstr, "+CSQ: %d, %d", (int *)&rssi, (int *)&ber); //从字符串读取格式化输入
            if(Cnt == 2) {
                NoteCat_Log("读取到信号量 %d,%d", rssi, ber);
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
//获取数据内容包的起始地址
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

//字符转数字
u64 StrToNumber(u8 *str, u8 len)
{
    u64 temp = 0;
    //检测开头是数字字符
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

//准备发送内容,除去设置管理员外
void GAM_SmsRecCms(u8 i)
{
	memset(SmsSendPack,0,sizeof(SmsSendPack));
	switch(i) {
		case 1: { //补单
		   Note_Log(">正在处理短信 补单 PRINT ");
		   memcpy(SmsSendPack,"PRINT-OK",strlen((const char*)"PRINT-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;
		case 2: { //取消所有操作
		   Note_Log(">正在处理短信 取消所有操作 CANCEL ");
		   memcpy(SmsSendPack,"CANCEL-OK",strlen((const char*)"CANCEL-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;
		case 3: {//本局吃大赔小
		   Note_Log(">正在处理短信 吃大赔小 EAT ");
		   memcpy(SmsSendPack,"EAT-OK",strlen((const char*)"EAT-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;
		case 4: {//本局黑桃
			Note_Log(">正在处理短信 本局黑桃 SPADE ");
		   memcpy(SmsSendPack,"SPADE-OK",strlen((const char*)"SPADE-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;
		case 5: {//本局红桃
			Note_Log(">正在处理短信 本局红桃 HEARTS ");
		   memcpy(SmsSendPack,"HEARTS-OK",strlen((const char*)"HEARTS-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;				
		case 6: {//本局梅花
		   Note_Log(">正在处理短信 本局梅花 CLUB ");
		   memcpy(SmsSendPack,"CLUB-OK",strlen((const char*)"CLUB-OK")); 
		} break;
		case 7: {//本局方块
		   Note_Log(">正在处理短信 本局方块 BLOCK ");
		   memcpy(SmsSendPack,"BLOCK-OK",strlen((const char*)"BLOCK-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;
		case 8: {//本局王
			Note_Log(">正在处理短信 本局王 HING ");
		   memcpy(SendSms.pStr,"HING-OK",strlen((const char*)"HING-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;				
		case 9: {//查询所有路单差值
			//准备发送内容
		} break;
		case 10:{//清除所有分机差值
			Note_Log(">正在处理短信 清除差值 CLEAR ");
		   memcpy(SendSms.pStr,"CLEAR-OK",strlen((const char*)"CLEAR-OK")); 
		   GSM_Port = 8; //发送短信	
		} break;						
	}

}


//处理一跳短信
void GSM_SmsOneDeal(u8 *dat, u16 len)
{
    u16 ID = 0;
    u8 PhoneNumStr[20] = {0};
    u8 NULLStr[20] = {0};
    u8 *pstr = NULL;
    u8 BackCnt = sscanf((char *)dat, "+CMGL: %d,\"%[^\"]\",\"%[^\"]\",,", (int *)&ID, (char *)NULLStr, PhoneNumStr);
    if(BackCnt == 3) {
        if(PhoneNumStr[0] == '+') { //手机号码
            u8 len = strlen((const char *)&PhoneNumStr[3]);
            memcpy(PhoneNumStr, &PhoneNumStr[3], len);
            memset(&PhoneNumStr[len], 0, sizeof(PhoneNumStr) - len);
        }
        SmsMsg.ID = ID;
        SmsMsg.PhoneNumber = StrToNumber(PhoneNumStr, sizeof(PhoneNumStr)); //电话转为数字
        NoteCat_Log("本信息 ID = %d  手机号为%s  numBer=%llu", ID, PhoneNumStr, SmsMsg.PhoneNumber);
		Note_Log(">本信息 ID = %d  手机号为%s  numBer=%llu", SmsMsg.ID , PhoneNumStr, SmsMsg.PhoneNumber);
        pstr = (u8 *)GetSmsContentPackStartAddr(dat, len); //实际短信内容数据包
        if(pstr) {
            SmsMsg.pPack   = pstr;
            SmsMsg.PackLen = len - (pstr - dat);
            u8 i = 0;
			//初次包头判断
            for(i = 0; i < sizeof(CmdStr) / sizeof(u8 *); i++) {
                if(pstr[0] == CmdStr[i][0] && pstr[1] == CmdStr[i][1] && pstr[2] == CmdStr[i][2]) {          //检查是否设置电话号码
                   break;
                }
            }
			//继续处理 (打印本条短信内容)
            NoteCat_Logn("内容=> len=%-0.3d >", SmsMsg.PackLen);
            for(u8 i = 0; i < SmsMsg.PackLen; i++) {
                NoteCat_Logn("%02X ", SmsMsg.pPack[i]);
            }
            NoteCat_Logn("\r\n");
            Usart_SendString(COM1,pstr,SmsMsg.PackLen);
			NoteCat_Logn("<---------->\r\n");
			//=================
			// 短信处理
			//=================
			if(i==0) //设置电话号码
			{
				u64 Phone = StrToNumber(pstr, SmsMsg.PackLen);       //电话转为数字
				if(Phone >= 10000000000LL ) //判断电话是11位数的
				{
				   AdministratorPhone = Phone;
				   NoteCat_Log("AdministratorPhone = %llu", AdministratorPhone); //
				   SaveAdministratorPhone(AdministratorPhone);
				   u8 buf[64] = {0};
                   sprintf((char*)buf,"OK%llu",AdministratorPhone);
				   memset(SmsSendPack,0,sizeof(SmsSendPack));
				   memcpy(SmsSendPack,buf,len); 	             
				   GSM_Port = 8; //发送短信					
				}else{ //删除短信
				   GSM_Port = 9; 
				}
			}else if(i < sizeof(CmdStr) / sizeof(u8 *)){
				NoteCat_ReadyUnRegistered();  //释放空间
				//对比是否是管理员电话
			    if(SmsMsg.PhoneNumber == AdministratorPhone){				
					u8 * ptr = (u8*)NoteCat_strstr((char*)CmdStr[i],strlen((const char*)CmdStr[i]),(char*)SmsMsg.pPack,SmsMsg.PackLen); //再次匹配数据
					if(ptr==NULL) //空,说明匹配失败
					{
					  i = sizeof(CmdStr) / sizeof(u8 *);
					}
					GAM_SmsRecCms(i);         //短信处理
					DockingNoteCat_Control(i);//对接通信猫控制
			    }			
			}else{ //非指令短信,删除短信
				NoteCat_ReadyUnRegistered();  //释放空间
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
        if(PhoneNumStr[0] == '+') { //手机号码
            u8 len = strlen((const char *)&PhoneNumStr[3]);
            memcpy(PhoneNumStr, &PhoneNumStr[3], len);
            memset(&PhoneNumStr[len], 0, sizeof(PhoneNumStr) - len);
        }
        SmsMsg.PhoneNumber = StrToNumber(PhoneNumStr, sizeof(PhoneNumStr)); //电话转为数字
        NoteCat_Log("本信息 ID = %d  手机号为%s  numBer=%llu", SmsMsg.ID , PhoneNumStr, SmsMsg.PhoneNumber);
		Note_Log("本信息 ID = %d  手机号为%s  numBer=%llu", SmsMsg.ID , PhoneNumStr, SmsMsg.PhoneNumber);
        pstr = (u8 *)GetSmsContentPackStartAddr(dat, len); //实际短信内容数据包
        if(pstr) {
            SmsMsg.pPack   = pstr;
            SmsMsg.PackLen = len - (pstr - dat);
            u8 i = 0;
			//初次包头判断
            for(i = 0; i < sizeof(CmdStr) / sizeof(u8 *); i++) {
                if(pstr[0] == CmdStr[i][0] && pstr[1] == CmdStr[i][1] && pstr[2] == CmdStr[i][2]) {          //检查是否设置电话号码
                   break;
                }
            }
			//继续处理 (打印本条短信内容)
            NoteCat_Logn("内容=> len=%-0.3d >", SmsMsg.PackLen);
            for(u8 i = 0; i < SmsMsg.PackLen; i++) {
                NoteCat_Logn("%02X ", SmsMsg.pPack[i]);
            }
            NoteCat_Logn("\r\n");
            Usart_SendString(COM1,pstr,SmsMsg.PackLen);
			NoteCat_Logn("<---------->\r\n");
			//=================
			// 短信处理
			//=================
			if(i==0) //设置电话号码
			{
				u64 Phone = StrToNumber(pstr, SmsMsg.PackLen);       //电话转为数字
				if(Phone >= 10000000000LL ) //判断电话是11位数的
				{
				   AdministratorPhone = Phone;
				   NoteCat_Log("AdministratorPhone = %llu", AdministratorPhone); //
				   SaveAdministratorPhone(AdministratorPhone);
				   u8 buf[64] = {0};
                   sprintf((char*)buf,"OK%llu",AdministratorPhone);
				   memset(SmsSendPack,0,sizeof(SmsSendPack));
				   memcpy(SmsSendPack,buf,strlen((const char*)buf)); 
                   Note_Log("正在处理 设置电话管理员 %llu",AdministratorPhone);
				   GSM_Port = 8; //发送短信					
				}else{ //删除短信
				   GSM_Port = 9; 
				}
			}else if(i < sizeof(CmdStr) / sizeof(u8 *)){
				NoteCat_ReadyUnRegistered();  //释放空间
				//对比是否是管理员电话
			    if(SmsMsg.PhoneNumber == AdministratorPhone){				
					u8 * ptr = (u8*)NoteCat_strstr((char*)CmdStr[i],strlen((const char*)CmdStr[i]),(char*)SmsMsg.pPack,SmsMsg.PackLen); //再次匹配数据
					if(ptr==NULL) //空,说明匹配失败
					{
					  i = sizeof(CmdStr) / sizeof(u8 *);
					}
					GAM_SmsRecCms(i);         //短信处理
					DockingNoteCat_Control(i);//对接通信猫控制
			    }			
			}else{ //非指令短信,删除短信
				NoteCat_ReadyUnRegistered();  //释放空间
			    GSM_Port = 9;
			}
        }
    }
}





//删除短信
u8 GSM_DeleteSMS(u8 ID)
{
  static u8 Port = 0;
  u8 state = ST_Busy;
  u8 temp  = 0;
  switch(Port)
  {
        case 0: { //检查设备
            temp = checkGSM_Connect(); //AT测试指令
            if(temp == ST_Success) {
                Port ++; 
				GSM_SendCmd(500, "AT+CMGD=%d\r\n",ID); //发送命令和设置差事时间
            }
        }
        break;
        case 1: { //删除短信
			u8 *pstr = (u8*)NoteCat_strstr("OK",2,(char*)NoteCat.pRecDat,NoteCat.NowCount);
            if(pstr)
			{
			   state = ST_Success; Port = 0; Note_Log(">删除%d短信成功",ID);
			}
			if(NoteCat.RecFlag)
			{
			   state = ST_Failed ; Port = 0; Note_Log(">删除%d短信失败",ID);	
			}
        }
        break;
  }
  return state;
}
//发送短信  
u8 GSM_SendSMS_Pack(u8*dat,u16 len)
{
  static u8 Port = 0;
  u8 state = ST_Busy;
  switch(Port)
  {
        case 0: { //发送测试指令
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
			   GSM_SendCmd(500,"AT+CMGS=\"%llu\"\r\n",AdministratorPhone); //设置电话号码			
			}
			pstr = (u8*)NoteCat_strstr("ERROR",5,(char*)NoteCat.pRecDat,NoteCat.NowCount);
			if(pstr){
			   NoteCat_Log(">GSM Send Check AT+CMGS=? Err");
			   state = ST_Failed;
			   Port = 0;				
			}
		
		}break;
        case 2: { //设置电话

			    u8 *pstr = (u8*)NoteCat_strstr(">",1,(char*)NoteCat.pRecDat,NoteCat.NowCount);
				if(pstr){
				   Note_Log(">发送短信设置电话  OK");
				   Port++;					
				}
				if(NoteCat.RecFlag){
				   Note_Log(">发送短信设置电话 Err");
				   state = ST_Failed;
			       Port = 0;
				}

        }
        break;
		case 3:{
				NoteCat_RecDatUser(2000);
				Note_Log(">len %d内容%s",strlen((const char*)dat),dat);
				NoteCat_SendString(dat  ,strlen((const char*)dat));						
				Port ++;				
		}break;
		case 4:{
		   u8* pstr = (u8*)NoteCat_strstr((char*)dat,strlen((const char*)dat),(char*)NoteCat.pRecDat,NoteCat.NowCount);
		   if(pstr || NoteCat.RecFlag || NoteCat.NowCount>=len)
		   {
		      NoteCat.pRecDat[NoteCat.NowCount]=0;
			  Note_Log(">>1内容%s",NoteCat.pRecDat);
			  NoteCat_RecDatUser(10000);
			  NoteCat_SendByte(0x1A);    //短信结束
			  Port ++;			      
		   }
		}break;
		case 5:{
			u8* pstr = (u8*)NoteCat_strstr("+CMGS",5,(char*)NoteCat.pRecDat,NoteCat.NowCount);
            if(pstr){state = ST_Success; Port = 0; Note_Log(">发送短信成功");}
			pstr = (u8*)NoteCat_strstr("ERROR",5,(char*)NoteCat.pRecDat,NoteCat.NowCount);
            if(pstr){state = ST_Success; Port = 0; Note_Log(">发送短信成功");}
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


//GSM 控制状态机
void  GSM_ControlStateMachine(void)
{
	static u8 GSM_Cnt  = 0;
    u8 temp = 0;
    switch(GSM_Port) {
        case 0: { //检查设备
            temp = checkGSM_Connect(); //AT测试指令
            if(temp == ST_Success) {
                GSM_Port ++;
            }
        }
        break;
        case 1: { //检查信号是否可以发送信息
            temp = GetGSM_CSQ(); //获得信号量
            if(temp == ST_Success && GSM_ConnectFlag == CONNECTION) {
                GSM_Port ++;
				GSM_Port = 2;
            }
        }
        break;
		
		case 0xFF:{ //删除短信
		
		
		}break;
		
        case 2: { //设置短信模式
            temp =  GSM_BasicCommands((u8 *)"OK", (u8 *)"AT+CMGF=1\r\n"); //AT测试指令
            if(temp == ST_Success) {
                GSM_Port ++;
				NoteCat_Log(">AT+CMGF=1  OK");
            }else if(temp == ST_Failed){
				NoteCat_Log(">AT+CMGF=1  Err");
			    GSM_Port =  0;//获取检查信号
			}
        }
        break;

        case 3: { //测试短信指令
            temp = GSM_BasicCommands((u8 *)"OK", (u8 *)"AT+CMGL=?\r\n");
            if(temp == ST_Success) {
                u8 *pstr = (u8 *) NoteCat_strstr("\"ALL\"", strlen("\"ALL\""), (char *)NoteCat.pRecDat, NoteCat.NowCount); //寻找起头
                if(pstr) {
					NoteCat_Log(">AT+CMGL=? OK");
                    GSM_Port ++;
                } else {
					NoteCat_Log(">AT+CMGL=? Err");
                    GSM_Port =  1;//获取检查信号
                }
            }else if(temp == ST_Failed && NoteCat.NowCount == 0){
			    GSM_Port =  0;//获取检查信号
			}
        }
        break;	

        case 4: { //测试短信指令
			
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
				GSM_SendCmd(1000,"AT+CMGL=ALL\r\n"); //查看全部命令
            }else  if(temp == ST_Failed){
				GSM_Cnt %=MaxSms;
			    GSM_Cnt++;
				
			    GSM_Port = 6;
				tm_timer_init(&GSM_NextTime,100,0);
				tm_timer_start(&GSM_NextTime);
			}
        }
        break;		
        case 5: { //接收短信
            u8 *pstr1 = (u8 *) NoteCat_strstr("OK", 2, (char *)NoteCat.pRecDat, NoteCat.NowCount); //寻找起头
			if(pstr1)
			{
                NoteCat_Log(">AT+CMGL=ALL");
                NoteCat_Log("<======1========>");
                pstr1 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)NoteCat.pRecDat, NoteCat.NowCount ); //寻找起头
                if(pstr1) {
                    u16 RemainingLength = NoteCat.NowCount - (pstr1 - NoteCat.pRecDat); //剩余长度
                    u8 *pstr2 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)(pstr1 + 1), RemainingLength - 1); //寻找起头
                    if(pstr2) { //第二条短信
                        GSM_SmsOneDeal(pstr1, pstr2 - pstr1);
                        NoteCat_Log("<======2========>");
                        pstr1++;
                    } else {   //最后一条短信了
                        if(RemainingLength > 6) {
                            GSM_SmsOneDeal(pstr1, RemainingLength - 6);
                            pstr1 = NULL; //退出循环
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
			   pstr1 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)NoteCat.pRecDat, NoteCat.NowCount ); //寻找起头
			   if(pstr1) {
						u16 RemainingLength = NoteCat.NowCount - (pstr1 - NoteCat.pRecDat); //剩余长度
						u8 *pstr2 = (u8 *) NoteCat_strstr("+CMGL:", strlen("+CMGL:"), (char *)(pstr1 + 1), RemainingLength - 1); //寻找起头
						if(pstr2) { //第二条短信
							GSM_SmsOneDeal(pstr1, pstr2 - pstr1);
							NoteCat_Log("<======2========>");
							pstr1++;
						} else {   //最后一条短信了
							if(RemainingLength > 6) {
								GSM_SmsOneDeal(pstr1, RemainingLength - 6);
								pstr1 = NULL; //退出循环
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
        case 6: { //等待1S 检查一次
           if(tm_timer_CheckFlag(&GSM_NextTime))
		   {
				tm_timer_ClearFlag(&GSM_NextTime);
			    GSM_Port = 3;
		   }
        } break;		
		
        //以2下是发送短信
        case 8: { 
			temp = GSM_SendSMS_Pack(SmsSendPack,strlen((const char*)GSM_SendSMS_Pack));
			if(temp==ST_Success)
			{
				GSM_Port = 9;  //删除短信
			}
        }
        break;
        //以下是删除短信
        case 9: { //删除短信
			temp = GSM_DeleteSMS(SmsMsg.ID);  //删除短信
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
        //透传
        if(que_size(Tx1_que)) {
            Usart_SendByte(COM1, que_Read(&Tx1_que));
        }
    }
    while(1) {
        GSM_ControlStateMachine();
        //透传
        if(que_size(Rx1_que)) {
            Usart_SendByte(COM3, que_Read(&Rx1_que));
        }
        //        //透传
        //        if(que_size(Tx1_que)) {
        //            Usart_SendByte(COM1, que_Read(&Tx1_que));
        //        }
    }
}
#endif





//=====================保存参数=================
#include "spi_dri.h"
//电话号码
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


//开机初始化
void StartOpenGSM_Init(void)
{
  AdministratorPhone = ReadAdministratorPhone();      //获得管理员电话
  NoteCat_Log("GSM 管理员电话 %llu",AdministratorPhone);
}

/********************************End of File************************************/

