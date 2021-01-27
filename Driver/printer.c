/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2021,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : printer_Demo.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2021-01-11
* Description  :
* Function List:
******************************************************************************
*/
/********************************End of Head************************************/
#include "printer.h"
#include "public.h"
#include "stdlib.h"
#include "string.h"
#include "multi_timer.h"
#include "HID_data.h"

#if 0
    #define Printer_Log(...)
    #define Printer_Logn(...)
#else
    #define Printer_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
    #define Printer_Logn(...)  do{sprint(__VA_ARGS__);}while(0)
#endif

u8 PrintBuf[64]={0};
typedef struct _NoteCat_Rec {
    u8 *pRecDat        ; //保存数据
    u16 NowCount       ; //接收数据计数
    u16 MaxReceive     ; //最大接收数据量
    u16 MaxCmdBuf      ; //开辟的空间
    u16 OutTime        ; //超时计时
    u16 MaxOutTime     ; //最大超时时间
    u16 MaxTimeCnt     ; //最大超时时间
    u8  RecFlag        ; //接收标志	
} RecDatType;

RecDatType Printfer ={NULL,0,0,0,0,0,0,0};


typedef struct _AsciiToGB{
 char Word ;
 u8  dat[2];	
}AsciiToGB_Type;

TM_Timer Print_ms ;
TM_Timer CheckKey_ms ;


const AsciiToGB_Type AsciiToGB[]={
//{'A',{0xA3,0xC1}},
//{'B',{0xA3,0xC2}},
//{'C',{0xA3,0xC3}},
//{'D',{0xA3,0xC4}},
//{'E',{0xA3,0xC5}},
//{'F',{0xA3,0xC6}},
//{'G',{0xA3,0xC7}},
//{'H',{0xA3,0xC8}},
//{'I',{0xA3,0xC9}},
//{'J',{0xA3,0xCA}},
//{'K',{0xA3,0xCB}},
//{'L',{0xA3,0xCC}},
//{'M',{0xA3,0xCD}},
//{'N',{0xA3,0xCE}},
//{'O',{0xA3,0xCF}},
//{'P',{0xA3,0xD0}},
//{'Q',{0xA3,0xD1}},
//{'R',{0xA3,0xD2}},
//{'S',{0xA3,0xD3}},
//{'T',{0xA3,0xD4}},
//{'U',{0xA3,0xD5}},
//{'V',{0xA3,0xD6}},
//{'W',{0xA3,0xD7}},
//{'Z',{0xA3,0xD8}},

//{'0',{0xA3,0xB0}},
//{'1',{0xA3,0xB1}},
//{'2',{0xA3,0xB2}},
//{'3',{0xA3,0xB3}},
//{'4',{0xA3,0xB4}},
//{'5',{0xA3,0xB5}},
//{'6',{0xA3,0xB6}},
//{'7',{0xA3,0xB7}},
//{'8',{0xA3,0xB8}},
//{'9',{0xA3,0xB9}},
{'}',{0xA3,0xFD}},
};

//基础函数:通信猫超时计数,判断一个数据包是否完成
void Print_OutTimeTiming(void)
{
    if(Printfer.OutTime)         { Printfer.OutTime--;   } //计时
    if(Printfer.OutTime == 1)    { Printfer.RecFlag = 1; } //数据包完成
	if(Printfer.MaxTimeCnt)      {Printfer.MaxTimeCnt--; }
	if(Printfer.MaxTimeCnt == 1) { Printfer.RecFlag = 1; } //数据包完成
}
	
void USART2_IRQHandler()
{
    if(USART2->SR & (1 << 5)) {
        u8 dat = USART2->DR;
        if(Printfer.pRecDat !=NULL && !Printfer.RecFlag && Printfer.MaxReceive) {
            if( Printfer.NowCount < Printfer.MaxReceive) { //数据包
                if(Printfer.OutTime) { Printfer.OutTime = Printfer.MaxOutTime; } //数据间隔
                Printfer.pRecDat[Printfer.NowCount]  = dat;
                Printfer.NowCount++;
				if(Printfer.NowCount < Printfer.MaxReceive)
				Printfer.pRecDat[Printfer.NowCount] = 0;
            } else {
                Printfer.RecFlag = 1; //接收完成
            }
        } else {
            que_Write(&Rx2_que, dat);
        }
    }
}


u8 Printer_RecDatUser( u16 MaxOutTime ,u16 NextByteTime)
{
	if(Printfer.pRecDat) { Printfer.pRecDat = NULL;} //释放空间
	if(Printfer.pRecDat == NULL)
	{
		memset(PrintBuf,0,sizeof(PrintBuf));
		Printfer.pRecDat = PrintBuf;	
		Printfer.MaxReceive = sizeof(PrintBuf);
		Printfer.NowCount   = 0;
	}
	Printfer.NowCount   = 0;
    Printfer.MaxCmdBuf  = 0;
    Printfer.RecFlag    = 0;
	Printfer.MaxTimeCnt = MaxOutTime;
	if(NextByteTime==0){
		Printfer.MaxOutTime = 0;
		Printfer.OutTime    = 0;	
	}else{
		Printfer.MaxOutTime = NextByteTime;
		Printfer.OutTime    = 250;	
	}
    return 0;
}



//检查打印机
u8 Check_Printer(void);

//基本操作-->发送数据
void PinterSendByte(u8 dat)
{
    Usart_SendByte(COM2, dat);
}
//基本命令写入
void PinterSendCmd(u8 dat1,u8 dat2,u8 dat3 ,u8 dat4,u8 len)
{
    
   if(len>=1)PinterSendByte(dat1);
   if(len>=2)PinterSendByte(dat2);
   if(len>=3)PinterSendByte(dat3);
   if(len>=4)PinterSendByte(dat4);
}


#include "string.h"
#include "stdarg.h"
#include "stdio.h"
//void PrinterStrOut(char *fmt, ...) //打印机信息打印
//{
//    volatile u8  buf[100];
//    u8 len;
//    va_list ap;  //建立变参列表
//    va_start(ap, fmt);   //参数格式化成ASCII
//    vsnprintf((char *)buf,sizeof(buf), fmt, ap);
//    va_end(ap);
//    len = strlen((char *)buf); //计算数据长度
//    for(u8 i=0;i<len;i++)
//    {
//       PinterSendByte(buf[i]);
//       if(buf[i]==0x0A)
//       {
//           delay_ms(10);
//           if(Check_Printer()==0)
//           {
//             Printer_Log("打印机出错");
//             return ;
//           }
//       }
//    }
//}

//FAA1  FAA2 FAA3 FAA4 FAA5   对应  王 黑 草 红 方
                     /*黑桃 , 红心 ,梅花  ,方块  ,王   */
const u16 FontList[]={0xFAA2,0xFAA4,0xFAA3,0xFAA5,0xFAA1};
// 黑(0)
// 桃(1)
// 红(2)
// 心(3)
// 草(4)
// 花(5)
// 方(6)
// 块(7)
// 王(8)

//Ascii码转为国标
void Printf_AsciiToGb(char Asicc)
{
	u16 len = sizeof(AsciiToGB)/sizeof(AsciiToGB_Type); //结算字模的长度
	u16 i = 0;
	for(i=0;i<len;i++)
	{
		if(AsciiToGB[i].Word == Asicc)
		{
		   PinterSendByte(AsciiToGB[i].dat[0]);
		   PinterSendByte(AsciiToGB[i].dat[1]);
		}
	}
	PinterSendByte(Asicc);	
}


//1B 2A 21 02 00 FF FF FF FF FF FF
//输出结果编码字符
void PrintWordEncoding(u8 Numbering)
{
	if(Numbering && Numbering<=sizeof(FontList)/sizeof(u16))
	{
	   PinterSendByte(FontList[Numbering-1]>>8);
	   PinterSendByte(FontList[Numbering-1]>>0);
	}
}

void PrintStrOut(u8 Open,char *fmt, ...) //打印机信息打印
{
    volatile u8  buf[100];
    u8 len;
    va_list ap;  //建立变参列表
    va_start(ap, fmt);   //参数格式化成ASCII
    vsnprintf((char *)buf,sizeof(buf), fmt, ap);
    va_end(ap);
    len = strlen((char *)buf); //计算数据长度
	Printer_Logn("%s",buf);
	
    for(u8 i=0;i<len;i++)
    {
        if(buf[i]<0x7F) //英文
		{
		   if(buf[i]==0x0A) 
		   {
			   PinterSendCmd(0x1B,0x4A,33,0x00,3); //打印并前进n(100)点 
		   }else if(buf[i]==0x0D){
			   	i++;
	            PinterSendCmd(0x1B,0x4A,33,0x00,3); //打印并前进n(100)点 
		   }else if(Open == 1 && buf[i]=='-'){
			   	i++;
	            PrintWordEncoding(buf[i]-'0');//显示结果
		   }else{
		        Printf_AsciiToGb(buf[i]);    //显示字母 
		   }		
		}else if(buf[i]>=0x7F) //中文
		{
		   PinterSendByte(buf[i  ]);
		   PinterSendByte(buf[i+1]);
		   i++;
		}
		

    }
}

//检查纸的状态
static u8 CheckProntPaper(void)
{
    static u8 Port = 0;
	u8 StartPort = 2;
    u16 Cnt = 0;
    u8 state = WAIT_BUSY;
    switch(Port) {
        case 0: { //10 04 00 打印机重新开机
            PinterSendByte(0x10);
            PinterSendByte(0x04);
            PinterSendByte(0x00);
			tm_timer_init(&Print_ms,400,0);
			tm_timer_start(&Print_ms);
            Port = 1;
        }
        break;
        case 1: { //等待
            if(tm_timer_CheckFlag(&Print_ms)) {
                tm_timer_ClearFlag(&Print_ms);
                Port = 2;
            }
        }
        break;
        case 2: { //10 04 01 返回打印机状态  ==>0x16
				Printer_RecDatUser(100,0); 
				PinterSendCmd(0x10,0x04,0x01,0x00,3); //返回打印机状态
			    Port++;
        }
        break;		
		
		
        case 3: { //10 04 01 返回打印机状态  ==>0x16
            if(Printfer.NowCount >=1){
				Printer_Log(">10 04 01   ==>0x%02d",Printfer.pRecDat[0]);
				if((Printfer.pRecDat[0] & 0x02) && (Printfer.pRecDat[0] & 0x04)) {
					Port ++;
					Printer_RecDatUser(100,0); 
					PinterSendCmd(0x10,0x04,0x04,0x00,3); //返回打印机状态
				}else{
					Port = StartPort;
					return WAIT_FALL;				
				} 
			}
			if(Printfer.RecFlag ){
					Port = StartPort;
					return WAIT_FALL;
			}	
        }
        break;
        case 4: { //10 04 04 返回纸的状态   ==>0x1E
            if(Printfer.NowCount >=1){
				Printer_Log(">10 04 04   ==>0x%02d",Printfer.pRecDat[0]);
				if(Printfer.pRecDat[0]==0x1E) {
					Printer_Log(">返回纸的状态   ==>0x1E ok");
                    Port = StartPort;
                    return WAIT_OK;
				}else{
					Printer_Log(">返回纸的状态1   ==>0x1E err %02X",Printfer.pRecDat[0]);
					Port = StartPort;
					return WAIT_FALL;				
				} 
			}
			if(Printfer.RecFlag ){
				    Printer_Log(">返回纸的状态2   ==>0x1E err");
					Port = StartPort;
					return WAIT_FALL;
			}
        }
        break;
        case 5: { //设置一些状态

            PinterSendCmd(0x1B,0x05,0x30,0x00,3); //清除错误     
            PinterSendCmd(0x1B,0x72,0x30,0x00,3); //头1走纸 两个机芯同时走纸
            PinterSendCmd(0x1B,0x73,0x30,0x00,3); //头1打印 两个机芯同时工作
            Port = StartPort;
            state =  WAIT_OK;           
        }
        break;        
    }
    return state;
}




//检查纸的状态
static u8 PreparePint(void)
{
    static u8 Port = 0;
    u16 Cnt = 0;
    u8 state = WAIT_BUSY;
    switch(Port) {
        case 0: { //10 04 00 打印机重新开机
            PinterSendByte(0x10);
            PinterSendByte(0x04);
            PinterSendByte(0x00);
            tm_timer_init(&Print_ms, 1000, 0); //设置等待时间
            tm_timer_start(&Print_ms);
            Port = 1;
        }
        break;
        case 1: { //等待
            if(tm_timer_CheckFlag(&Print_ms)) {
                tm_timer_ClearFlag(&Print_ms);
                Port = 2;

            }
        }
        break;
        case 2: { //10 04 01 返回打印机状态
            que_clearAll(&Tx1_que);
            PinterSendCmd(0x10,0x04,0x01,0x00,3); //返回打印机状态
            Cnt = 0;
            while(que_size(Tx1_que) ==0) {        //等待接收数据
                Cnt++;
                if(Cnt > 0xFFF0) {
                    Port = 2;
                    return WAIT_FALL;
                }
            }
            u8 dat = que_Read(&Tx1_que);
            if((dat & 0x02) && (dat & 0x04)) {
                Port = 3;
            } else {
                Port = 2;
                return WAIT_FALL;
            }
        }
        break;
        case 3: { //10 04 04 返回纸的状态
            que_clearAll(&Tx1_que);
            PinterSendCmd(0x10,0x04,0x04,0x00,3); //返回纸的状态
            Cnt = 0;
            while(que_size(Tx1_que)==0) { //等待接收数据
                Cnt++;
                if(Cnt > 0xFFF0) {
                    Port = 2;
                    return WAIT_FALL;
                }
            }
            u8 dat = que_Read(&Tx1_que);
            if(dat == 0x1E) { //有纸
                Port = 2;
                return WAIT_OK;
            } else {
                Port = 2;
                state =  WAIT_FALL;  
            }
        }
        break;
        case 4: { //设置一些状态

            PinterSendCmd(0x1B,0x05,0x30,0x00,3); //清除错误     
            PinterSendCmd(0x1B,0x72,0x30,0x00,3); //头1走纸 两个机芯同时走纸
            PinterSendCmd(0x1B,0x73,0x30,0x00,3); //头1打印 两个机芯同时工作
            Port = 2;
            state =  WAIT_OK;           
        }
        break;        
    }
    return state;
}




//检查打印机
u8 Check_Printer(void)
{
    u8 res ;
    do {
        res =  PreparePint();
    } while(res == WAIT_BUSY);
    if(res == WAIT_OK) {
       return 1;
    } else {
       return 0;
    }
}

//检查打印机
u8 Check_Paper(void)
{
    u8 res  =  CheckProntPaper();
    return res;
}

 u8 PrintPort = 0;

//打单控制流程
 u8 PrintfPlaySingle(void)
 {
     u8 state = WAIT_BUSY;
	 u8 temp  = 0;
	 switch(PrintPort)
	 {
		 case 0:{
	       temp = Check_Paper();		 
		   if(temp == WAIT_OK)
		   {
			  Printer_Log(">1纸张 OK");
			  PrintPort ++;
			  PinterSendCmd(0x1D,0x69,0x03,0x00,3); //浓度加厚
              PinterSendCmd(0x1B,0x21,0x30,0x00,3); //开启高宽加倍
              PinterSendCmd(0x1B,0x61,0x01,0x00,3); //字体居中(0:左对齐 1:中心对齐 2:右对齐)
			  PrintStrOut(0,"预警-无线版\n");
			  tm_timer_init(&Print_ms,100,0);
			  tm_timer_start(&Print_ms);
		   }else if(temp ==WAIT_FALL ){
			   Printer_Log(">1纸张 Err");
		      state = WAIT_FALL;
		   } 
		 }break;
		 case 1:{
			 if(tm_timer_CheckFlag(&Print_ms))
			 {
				PinterSendCmd(0x1D,0x69,0x02,0x00,3); //浓度加厚(不加厚)
                PinterSendCmd(0x1B,0x4A,0x05,0x00,3); //打印并前进n(100)点
                PinterSendCmd(0x1B,0x21,0x00,0x00,3); //关闭高宽加倍 
                PinterSendCmd(0x1B,0x33,0x00,0x00,3); //设置行距(默认30点)
                PrintStrOut(0,"机号: 0 0 5 4 1 0 1 5\n");            
                PrintStrOut(0,"2 0 2 0 年 1 2 月 2 2 日第 0 2 轮\n");            
                PrintStrOut(0,"防盗路单前两行结果无效\n");            
                PrintStrOut(0,"结果与字母无关\n"); 
				PrintPort ++;
				tm_timer_init(&Print_ms,200,0);
			    tm_timer_start(&Print_ms);
			 }
		 }break;
		 case 2:{
			 if(tm_timer_CheckFlag(&Print_ms))
			 {
                tm_timer_ClearFlag(&Print_ms);
				PinterSendCmd(0x1D,0x69,0x03,0x00,3); //浓度加厚(加厚)
				u8 Result[     100]={0};
                u8 ResultAscii[100]={0};
				for(u8 i=0;i<100;i++)  //读取结果
				{
					Result[i]      = (ReadBoutResult(i+1)>>4);
					ResultAscii[i] = (rand()%26)+'A'    ;
				}
				u8 temp1[12]={0};
				u8 temp2[12]={0};
				for(u8 i=0;i<12;i++)  //读取结果
				{
					temp1[i] = (rand()%5)+1       ;
					temp2[i] = (rand()%26)+'A'    ;
				}				 
				for(u8 i=0;i<2;i++)
				{ 
				  PrintStrOut(1,"-%d %c  -%d %c  -%d %c  -%d %c  -%d %c  -%d %c\n",  temp1[i*6+0],temp2[i*6+0],
					                                                                 temp1[i*6+1],temp2[i*6+1],
					                                                                 temp1[i*6+2],temp2[i*6+2],
					                                                                 temp1[i*6+3],temp2[i*6+3],
					                                                                 temp1[i*6+4],temp2[i*6+4],
					                                                                 temp1[i*6+5],temp2[i*6+5]);				
				}
				for(u8 i=0;i<100/6;i++)
				{
				  PrintStrOut(1,"-%d %c  -%d %c  -%d %c  -%d %c  -%d %c  -%d %c\n",(Result[i*6+0]>=6)?5:Result[i*6+0],ResultAscii[i*6+0],
					                                                               (Result[i*6+1]>=6)?5:Result[i*6+1],ResultAscii[i*6+1],
					                                                               (Result[i*6+2]>=6)?5:Result[i*6+2],ResultAscii[i*6+2],
					                                                               (Result[i*6+3]>=6)?5:Result[i*6+3],ResultAscii[i*6+3],
					                                                               (Result[i*6+4]>=6)?5:Result[i*6+4],ResultAscii[i*6+4],
					                                                               (Result[i*6+5]>=6)?5:Result[i*6+5],ResultAscii[i*6+5]);				
				}
				PrintStrOut(1,"-%d %c  -%d %c  -%d %c  -%d %c            \n",Result[(100/6)*6+0]>=6?5:Result[(100/6)*6+0],ResultAscii[(100/6)*6+0],
					                                                         Result[(100/6)*6+1]>=6?5:Result[(100/6)*6+1],ResultAscii[(100/6)*6+1],
					                                                         Result[(100/6)*6+2]>=6?5:Result[(100/6)*6+2],ResultAscii[(100/6)*6+2],
					                                                         Result[(100/6)*6+3]>=6?5:Result[(100/6)*6+3],ResultAscii[(100/6)*6+3]);
				 
                tm_timer_init(&Print_ms,4000,0);
			    tm_timer_start(&Print_ms);
				Printer_Log(">打印机 输入完成1");
				PrintPort++;
			 }
		 }break;	
         case 3:{
			 if(tm_timer_CheckFlag(&Print_ms))
			 {
                tm_timer_ClearFlag(&Print_ms);
				Printer_Log(">打印机 输入完成2");
				PinterSendCmd(0x1B,0x4A,0x0A,0x00,3); //打印并前进n(100)点  
                PrintStrOut(0,"打印于2 0 2 0年1 2月2 2日1 4时1 9分\n");
                PrintStrOut(1,"-1黑桃:-2红心:-3草花:-4方块:-5王\n");                
                PinterSendCmd(0x1B,0x4A,0x64,0x00,3); //打印并前进n(100)点 
                PinterSendCmd(0x1B,0x4A,0x64,0x00,3); //打印并前进n(100)点 	
                PinterSendCmd(0x1B,0x69,0x00,0x00,2); //打印并前进n(100)点 					 
				state = WAIT_OK;
				PrintPort = 0;	
             }				 
		 }break;		 
	 }
	 return state;
 }




//Main
void Printer(void)
{
    u8 res ;
    do {
        res =  PreparePint();
    } while(res == WAIT_BUSY);
    if(res == WAIT_OK) {
        Printer_Logn("打印机 正常\r\n");
    } else {
        Printer_Logn("打印机 故障\r\n");
    }
    tm_timer_init(&CheckKey_ms,1000,1000);
    tm_timer_start(&CheckKey_ms);

    while(1) {
        if(que_size(Rx1_que)) {
            Usart_SendByte(COM3, que_Read(&Rx1_que));
        }
        if(que_size(Tx1_que)) {
            Usart_SendByte(COM1, que_Read(&Tx1_que));
        }
        if(tm_timer_CheckFlag(&CheckKey_ms))
        {
            tm_timer_ClearFlag(&CheckKey_ms);
            if(PBin(12)==0)
            {
                PinterSendCmd(0x1B,0x40,0x00,0x00,2); //清空缓存
                if(Check_Printer()==0)
                {
                 Printer_Log("打印机出错");
                 return ;
                }
				PinterSendCmd(0x1D,0x69,0x03,0x00,3); //关闭高宽加倍 
                PinterSendCmd(0x1B,0x21,0x30,0x00,3); //开启高宽加倍
                PinterSendCmd(0x1B,0x61,0x01,0x00,3); //字体居中(0:左对齐 1:中心对齐 2:右对齐)
//                PrinterStrOut("预警-无线版\n");
                PinterSendCmd(0x1B,0x4A,0x05,0x00,3); //打印并前进n(100)点
                PinterSendCmd(0x1B,0x21,0x00,0x00,3); //关闭高宽加倍 
                PinterSendCmd(0x1B,0x33,0x00,0x00,3); //设置行距(默认30点)
				
//                PrinterStrOut("机 号: 0 0 5 4 1 0 1 5\n");            
//                PrinterStrOut("2 0 2 0年1 2月2 2日第0 2轮\n");            
//                PrinterStrOut("防盗路单前两行结果无效\n");            
//                PrinterStrOut("结果与字母无关\n"); 
				
				
//                PinterSendCmd(0x1B,0x21,0x10,0x00,3); //开启高宽加倍
//                PrinterStrOut("测X  试I  打Z  印K  结F  果K\n");	
				u8 Result[     100]={0};
				u8 ResultAscii[100]={0};				
				for(u8 i=0;i<100;i++)
				{
					Result[i] = rand()%5+1;
					ResultAscii[i]= (rand()%26)+'A';
				}
				
//				PrintStrOut("-%dX  -%dI  -%dZ  -%dK  -%dF  -%dK",1,2,3,4,5,5);
//				PinterSendCmd(0x1B,0x4A,20,0x00,3); //打印并前进n(100)点
//				PrintStrOut("-%dX  -%dI  -%dZ  -%dK  -%dF  -%dK",1,2,3,4,5,5);
//				PinterSendCmd(0x1B,0x4A,20,0x00,3); //打印并前进n(100)点
//				 PinterSendCmd(0x1B,0x33,30,0x00,3); //设置行距(默认30点)
				
//				for(u8 i=0;i<100/6;i++)
//				{
//				  PrintStrOut("-%d%c  -%d%c  -%d%c  -%d%c  -%d%c  -%d%c\n",Result[i*6+0],ResultAscii[i*6+0],
//					                                                         Result[i*6+1],ResultAscii[i*6+1],
//					                                                         Result[i*6+2],ResultAscii[i*6+2],
//					                                                         Result[i*6+3],ResultAscii[i*6+3],
//					                                                         Result[i*6+4],ResultAscii[i*6+4],
//					                                                         Result[i*6+5],ResultAscii[i*6+5]);				
//				}
//				PrintStrOut("-%d%c  -%d%c  -%d%c  -%d%c          \n",Result[(100/6)*6+0],ResultAscii[(100/6)*6+0],
//					                                                    Result[(100/6)*6+1],ResultAscii[(100/6)*6+1],
//					                                                    Result[(100/6)*6+2],ResultAscii[(100/6)*6+2],
//					                                                    Result[(100/6)*6+3],ResultAscii[(100/6)*6+3]);
				
				
			
				
				
				
                PinterSendCmd(0x1B,0x4A,0x0A,0x00,3); //打印并前进n(100)点  
                PinterSendCmd(0x1B,0x4A,0x00,0x00,3); //打印并前进n(100)点 

                
//                PinterSendCmd(0x1B,0x21,0x00,0x00,3); //关闭高宽加倍 

//                PrinterStrOut("打印于2 0 2 0年1 2月2 2日1 4时1 9分\n");
//                PrintStrOut("-1黑桃:-2红心:-3草花:-4方块:-5王\n"); 
////                PrinterStrOut2("-1黑桃:-2红心:-3草花:-4方块:-5王\n");                 
//                PinterSendCmd(0x1B,0x4A,0xFF,0x00,3); //打印并前进n(100)点                
            }
        
        
        
        }
        
    }
}



/********************************End of File************************************/

