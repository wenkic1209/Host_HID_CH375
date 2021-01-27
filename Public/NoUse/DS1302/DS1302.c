  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : DS1302.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-06-15         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "DS1302.h"
#include "iic.h"
#include "sys.h"
#include "config.h"

//================================================================================
// 函数声明
void SaveDate_Write(u32 addr,_TimeType Time); //保存日期（年月日）
void SaveDate_Read(u32 addr,_TimeType* Time); //读取日期（年月日）
void SaveTime_Write(u32 addr,_TimeType Time); //保存日期（年月日时分秒）
void SaveTime_Read(u32 addr,_TimeType* Time); //读取日期（年月日时分秒）

static _TimeType StartTime   = {0}; //开始时间
static _TimeType EndTime     = {0}; //结束时间
TimeType SurplusTime  = {0}; //剩余时间

//================================================================================
//              DS1302驱动
//================================================================================
_TimeType Date = 
{
    .year = 2020, /* 年  , 0 to 99 */
    .mon  = 8, /* 月  , 1 to 12 */	
    .day  = 6, /* 日  , 1 to 31 */
    .wday = 4, /* 星期, 1 to 7  */
    .hour = 0, /* 时  , 0 to 23 */	
    .min  = 0, /* 分  , 0 to 59 */	
    .sec  = 0, /* 秒  , 0 to 59 */
}; //

//内部时间（模版时间）
const _TimeType RTC_Date = 
{
    .year = 0, /* 年  , 0 to 99 */
    .mon  = 1, /* 月  , 1 to 12 */	
    .day  = 2, /* 日  , 1 to 31 */
    .wday = 4, /* 星期, 1 to 7  */
    .hour = 0, /* 时  , 0 to 23 */	
    .min  = 0, /* 分  , 0 to 59 */	
    .sec  = 0, /* 秒  , 0 to 59 */
}; //


const _TimeType defaultTime = {0}; //默认时间

/*
DS1302
RST : PB5 
IO  : PB6
SCLK: PB7
*/

#define DS13_RST        PBout(5)  
#define DS13_IO         PBout(6)  
#define DS13_IOin       PBin(6)  
#define DS13_CLK        PBout(7)  

void DS1302_IOInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	DS13_RST = 0;
	DS13_IO  = 0;
	DS13_CLK = 0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	DS13_RST = 0;
	DS13_IO  = 0;
	DS13_CLK = 0;	
	
}

//数据脚方向为输出
void DS13_IO_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

}
//数据脚方向为输入
void DS13_IO_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//写入1字节
void DS13_WByte(u8 dat)
{
    unsigned char i;
	//输出数据
    for (i=0x01; i!=0; i<<=1){ //低位在前，逐位移出
        if (i&dat){ 
            DS13_IO = 1;
        }else{
            DS13_IO = 0;
        }
        DS13_CLK = 1; //然后拉高时钟
        DS13_CLK = 0; //再拉低时钟，完成一个位的操作
    }
    DS13_IO = 1;      //最后确保释放 IO 引脚
}

//读取1字节
u8  DS13_RByte(void)
{
   u8 i=0,dat=0;
	DS13_IO_IN();
    for(i=0x01;i!=0;i<<=1)
	{
		if(DS13_IOin){dat |=i;}
        DS13_CLK = 1; //然后拉高时钟
        DS13_CLK = 0; //再拉低时钟，完成一个位的操作		
	}
	DS13_IO_Out();
	return dat;
}

/* 用单次写操作向某一寄存器写入一个字节，reg-寄存器地址，dat-待写入字节 */
void DS1302_WriteReg(unsigned char reg, unsigned char dat){
    DS13_RST = 1; //使能片选信号
    DS13_WByte((reg<<1)|0x80); //发送写寄存器指令
    DS13_WByte(dat);           //写入字节数据
    DS13_RST = 0;                  //除能片选信号
}
/* 用单次读操作从某一寄存器读取一个字节，reg-寄存器地址，返回值-读到的字节 */
unsigned char DS1302_ReadReg(unsigned char reg){
    unsigned char dat;
    DS13_RST = 1;              //使能片选信号
    DS13_WByte((reg<<1)|0x81); //发送读寄存器指令
    dat = DS13_RByte();        //读取字节数据
    DS13_RST = 0;              //除能片选信号
    return dat;
}


//转码
unsigned char bcd_to_dat(unsigned char data)
{
    unsigned char temp;

    temp = ((data>>4)*10 + (data&0x0f));
    return temp;
}
//转码
unsigned char dat_to_bcd(unsigned char data)
{
    unsigned char temp;
    temp = ((data/10)<<4)|(data%10);
    return temp;
}


//写入时间
void DS1302_Write_date(_TimeType Date)
{
//读取时间    
	DS1302_WriteReg(0x06,dat_to_bcd(Date.year%100)); //年  Date.year%100
	DS1302_WriteReg(0x05,dat_to_bcd(Date.wday    )); //星期      
	DS1302_WriteReg(0x04,dat_to_bcd(Date.mon     )); //月  
	DS1302_WriteReg(0x03,dat_to_bcd(Date.day     )); //日
	DS1302_WriteReg(0x02,dat_to_bcd(Date.hour    )); //时
	DS1302_WriteReg(0x01,dat_to_bcd(Date.min     )); //分  
	DS1302_WriteReg(0x00,dat_to_bcd(Date.sec     )); //秒
}
void DS1302_Read_Date(_TimeType*Date)
{
	 Date->year =      bcd_to_dat( DS1302_ReadReg(0x06));
	 Date->wday =      bcd_to_dat((DS1302_ReadReg(0x05)&0x07));    
	 Date->mon  =      bcd_to_dat((DS1302_ReadReg(0x04)&0x1F));
	 Date->day  =      bcd_to_dat((DS1302_ReadReg(0x03)&0x3F));
	 Date->hour =      bcd_to_dat((DS1302_ReadReg(0x02)&0x3F));
	 Date->min  =      bcd_to_dat((DS1302_ReadReg(0x01)&0x7F));
	 Date->sec  =      bcd_to_dat((DS1302_ReadReg(0x00)&0x7F));	
}



/*
地址/命令直接
  7    6      5  4    3   2   1    0
  1  RAM/CK  A4  A3  A2  A1  A0  RD/WR

RAM/CK   1:RAM   0:RTC
RD/WR    1:Read  0:Write

---------------------------------------------------------------------------------------------
读取寄存器
---------------------------------------------------------------------------------------------
RTC  时钟寄存器
---------------------------------------------------------------------------------------------
 (1/2)前字节   |                           后字节             
READ   WRITE   |   BIT 7   BIT 6   BIT 5   BIT 4   BIT 3    BIT 2   BIT 1   BIT 0   RANGE
81h    80h     |   CH                 10 Seconds                          Seconds   00–59
83h    82h     |                      10 Minutes                          Minutes   00–59
85h    84h     |   12/24      0  10/(AM/PM) Hour                             Hour   1–12/0–23
87h    86h     |   0       0             10 Date                             Date   1–31
89h    88h     |   0       0       0    10 Month                            Month   1–12
8Bh    8Ah     |   0       0       0           0       0                      Day   1–7
8Dh    8Ch     |                         10 Year                             Year   00–99
8Fh    8Eh     |   WP      0       0           0       0        0       0       0     —
91h    90h     |   TCS    TCS     TCS        TCS      DS       DS      RS      RS     —
----------------------------------------------------------------------------------------------
==============================================================================================
----------------------------------------------------------------------------------------------
RAM     储存
----------------------------------------------------------------------------------------------
C1h    C0h                                                                          00-FFh
C3h    C2h                                                                          00-FFh
C5h    C4h                                                                          00-FFh
.       .                                                                             .
.       .                                                                             .
.       .                                                                             .
FDh    FCh                                                                          00-FFh
----------------------------------------------------------------------------------------------

CH  1:时钟禁止  0:时钟运行  (上电需要读取，如果等于1,需要置0)

*/



#include "debug.h"
#include "usart.h"
#include "spi_dri.h"
void DS1302_Init(void){
    _TimeType temp;
    DS1302_IOInit();
    
    
    //检查是否需要初始化
    if(FM25ReadByte(15)!=FristDateFlag)
    {
       FM25WriteByte(15,FristDateFlag);
       //写入默认的时间(默认系统时间)
       SetNewSystempDate(Date);
       //初始化
       StartTime = defaultTime; //得到现在时间
       EndTime   = defaultTime; //计算X小时后的时间
       SaveTime_Write(20,StartTime);
       SaveTime_Write(30,EndTime)  ; 
    }
    SaveDate_Read(10,&Date);  //上电读取日期
    
    //检查时钟芯片是否掉过电池    
	DS1302_Read_Date(&temp);
    pr_debug("Frist Read Date:%4d-%2d-%2d %2d:%2d:%2d wday:%d",temp.year,temp.mon,temp.day,temp.hour,temp.min,temp.sec,temp.wday);	
    
    //月日时间不合理，重写时间
    if(temp.day== 0 || temp.mon == 0)
    {
      pr_debug("RTC Clock battery lost1");
      DS1302_Write_date(RTC_Date);  //初始时间
      ValidTimeDisable();           //清除有效时间
    }
    //小于0年1月2日,判断为掉电过，需要初始时间，清除已有时间
    else if(temp.year==0&&temp.day==1&&temp.mon < 2){
      pr_debug("RTC Clock battery lost2");
      DS1302_Write_date(RTC_Date);  //初始时间 
      ValidTimeDisable();           //清除有效时间        
    }
   
    
	//检查时间是否停止
	if(DS1302_ReadReg(0)&0x80) 
	{
		//读出时间
	    DS1302_Read_Date(&temp);
	    //写入时间
	    DS1302_Write_date(temp);
		DS1302_WriteReg(0x07,0x00|0x10|0x00);
	}
    //获取上次保存的时间（有效时间区间）
    SaveTime_Read(20,&StartTime);
    SaveTime_Read(30,&EndTime)  ;  
    pr_debug("Start Time %4d-%2d-%2d %2d:%2d:%2d wday:%d",StartTime.year,StartTime.mon,StartTime.day,StartTime.hour,StartTime.min,StartTime.sec,StartTime.wday);
    pr_debug("End   Time %4d-%2d-%2d %2d:%2d:%2d wday:%d",EndTime.year,EndTime.mon,EndTime.day,EndTime.hour,EndTime.min,EndTime.sec,EndTime.wday);    
    
}
//===============================================================================
//*******************************************************************************
//===============================================================================
#include "debug.h"
#include "spi_dri.h"
//===============================================================================
//-------------------------------------------------------------------------------
//打印时间输出
void print_date(void)
{
	pr_debug("date:%4d-%2d-%2d %2d:%2d:%2d wday:%d",Date.year,Date.mon,Date.day,Date.hour,Date.min,Date.sec,Date.wday);	
    pr_debug("SurplusTime %5d:%2d:%2d",SurplusTime.hour,SurplusTime.min,SurplusTime.sec);
    //	pr_debug("wday:%d",Date.wday);	    
}




//判断闰年
/*判断是否为闰年，是，返回 1； 不是，返回0 */
static u8 IsLeap(u16 year)
{
    if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
static u8 mondays[2][13] =
{
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, //0--11月 //0是无效的 不是闰年
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, //0--11月 //0是无效的  是闰年
};

//检查是否还有天数
u8 Check_haveOneDaySub( _TimeType Time)
{
    if(Time.year == 0 && Time.mon <= 1 && Time.day <= 2)
    {
        return 0;
    }
    return 1;
}

//一百年内可能的闰年
//0       4       8       12      16      20      24      28      32      36      40      44      48      
//52      56     60       64      68      72      76      80      84      88      92      96

//减少日期的一天
_TimeType TimeSub_OneDay(_TimeType Time)
{
   //检查是否还有天数
   if(Check_haveOneDaySub(Time))
   {
     if (Time.day == 1)
     {
         if (Time.mon == 1)
         {
             //减去年
             if (Time.year){Time.year--;}
             Time.mon = 12;
             Time.day = mondays[IsLeap(Time.year)][Time.mon];
         }
         else {
             //减去一个月
             Time.mon--;
             Time.day = mondays[IsLeap(Time.year)][Time.mon];
         }
     }else{
       Time.day--;
     }
   }
   return Time;
}
//增加日期的一天
_TimeType TimeAdd_OneDay(_TimeType Time)
{
   //判断是否足够一个月
   if(Time.day == mondays[IsLeap(Time.year)][Time.mon])
   {
      Time.day = 1;
      if(Time.mon == 12)
      {
         Time.mon  = 1;
         Time.year  ++;
      }else{
         Time.mon++;
      }
   }else{
      Time.day++;
   }
   return Time;
}






//日的比较 (0日期相同 1 日期未到  2日期已过 ) （年月日）判断
int TimeCompareDate(_TimeType minTime,_TimeType maxTime)
{
    if(minTime.year < maxTime.year){
       return 1;
    }else if(minTime.year == maxTime.year && minTime.mon < maxTime.mon){
       return 1;
    }else if(minTime.year == maxTime.year && minTime.mon == maxTime.mon && minTime.day < maxTime.day){
       return 1;
    }else if(minTime.year == maxTime.year && minTime.mon == maxTime.mon && minTime.day == maxTime.day){
       return 0; 
    }else{
       return 2;
    }
}


//时间的比较 (0时间相同 1 时间未到  2时间已过 )（年月日时分秒）判断
int TimeCompareTime(_TimeType minTime,_TimeType maxTime)
{

    if(minTime.year <= maxTime.year)
    {
       if(minTime.year == maxTime.year)
       {
            if(minTime.mon <= maxTime.mon)
            {
               if(minTime.mon == maxTime.mon)
               {
                    if(minTime.day <= maxTime.day)
                    {
                       if(minTime.day == maxTime.day)
                       {
                            if(minTime.hour <= maxTime.hour)
                            {
                               if(minTime.hour == maxTime.hour)
                               {
                                    if(minTime.min <= maxTime.min)
                                    {
                                       if(minTime.min == maxTime.min)
                                       {
                                            if(minTime.sec <= maxTime.sec)
                                            {
                                               if(minTime.sec == maxTime.sec)
                                               {
                                                   return 0;   //时间相等
                                               }
                                               return 1;       //时间未到
                                            }else {
                                               return 2;       //时间已过
                                            }  
                                       }
                                       return 1;  
                                    }else {
                                       return 2;
                                    } 
                               }
                               return 1;  
                            }else {
                               return 2;
                            }
                       }
                       return 1;  
                    }else {
                       return 2;
                    } 
               }
               return 1;  
            }else {
               return 2;
            } 
       }
       return 1;  
    }else {
       return 2;
    }
}
//比较时间是否相同
int CompareTimeSame(TimeType Start,TimeType End)
{
   if(Start.hour == End.hour && Start.min == End.min && Start.sec == End.sec)
   {
     return 0;
   }
   return 1;
}

//计算时间差(间隔年月日时分秒日期转换为时分秒时间)
TimeType differenceTime(_TimeType NowTime,_TimeType EndTime)
{
    TimeType Atemp = {0};
    u32 NowSec = 0;
    u32 EndSec = 0;
    u32 Sec = 0;
    //先判断日
    while(1)
    {
        int temp = TimeCompareDate(NowTime,EndTime); //比较时间
        //日期未过,s时间差超过一天
        if(temp == 1)
        {
           NowTime = TimeAdd_OneDay(NowTime);
           Atemp.hour += 24;
        }
        //日期已过
        else if(temp == 2){
            return Atemp;
        }
        //日期相同
        else if(temp == 0){
           break;
        }
    }
    NowSec = NowTime.hour*3600+NowTime.min*60+NowTime.sec;
    EndSec = EndTime.hour*3600+EndTime.min*60+EndTime.sec;
    //小时转换
    if(Atemp.hour == 0)
    {
       if(NowSec <= EndSec) 
       {
          Sec = EndSec-NowSec;
          Atemp.hour = Sec/3600;
          Sec %=3600;
          Atemp.min  = Sec/60; 
          Sec %=60;           
          Atemp.sec  = Sec;            
       }else{
          Atemp.hour = 0;
          Atemp.min  = 0;
          Atemp.sec  = 0;
       }
    }else{
       if(NowSec <= EndSec) 
       {
          Sec = EndSec-NowSec;
          Atemp.hour += Sec/3600;
          Sec %=3600;
          Atemp.min  = Sec/60; 
          Sec %=60;           
          Atemp.sec  = Sec;            
       }else{
          Sec = NowSec-EndSec;
          Atemp.hour -= Sec/3600;
          Sec %=3600;           
          if(Sec)
          {
              Atemp.hour--;
              Sec = 3600 - Sec;
              Atemp.min  = Sec/60; 
              Sec %=60;           
              Atemp.sec  = Sec;          
          }
       }
    }
    return Atemp;
}








//保存日期（年月日）
void SaveDate_Write(u32 addr,_TimeType Time)
{
   FM25WriteByte(addr+0,Time.year&0xFF);
   FM25WriteByte(addr+1,Time.year>>8);
   FM25WriteByte(addr+2,Time.mon);
   FM25WriteByte(addr+3,Time.day);
}
//读取日期（年月日）
void SaveDate_Read(u32 addr,_TimeType* Time)
{
   Time->year  = FM25ReadByte(addr+0);
   Time->year |= FM25ReadByte(addr+1)<<8;
   Time->mon   = FM25ReadByte(addr+2);
   Time->day   = FM25ReadByte(addr+3);
}

//保存日期（年月日时分秒）
void SaveTime_Write(u32 addr,_TimeType Time)
{
   u8 *dat = (u8*)&Time;
   for(u8 i=0;i<sizeof(_TimeType);i++)
   {
      FM25WriteByte(addr+i,*dat);
      dat++;
   }    
}
//读取日期（年月日时分秒）
void SaveTime_Read(u32 addr,_TimeType* Time)
{
   u8 *dat = (u8*)Time;
   for(u8 i=0;i<sizeof(_TimeType);i++)
   {
      dat[i] = FM25ReadByte(addr+i);
   } 
}

//判断时间的合法性 Start <Now <End
int JudgeTimeLegal(_TimeType Start,_TimeType Now,_TimeType End)
{
    //开始时间与结束时间相等
    if(TimeCompareTime(Start,End)==0) 
    {
        return 0;
    }
    //时间在 Start > End
    else if(TimeCompareTime(Start,End)==2){
        return 0;
    }
    //时间在 Start < Now < End
    else if(TimeCompareTime(Now,Start)== 2 && TimeCompareTime(Now,End)== 1)
    {
        return 1;       
    }
    return 0;
}




//检查时钟走的天数，有一天就累积到日期上，RTC天计数就减少一天
void RTC_Data_1sCheck(void)
{
    _TimeType RTCTime;
    _TimeType TempTime;
    static _TimeType OldTime;
    static u8 Cnt     = 0;  //      
    static u16 LoadMs = 0;
 
    if(LoadMs)LoadMs--;
    if(LoadMs==0)
    {
        LoadMs = 1000;  
        DS1302_Read_Date(&RTCTime); //获取时间
        Date.wday = RTCTime.wday;   //具体时间
        Date.sec  = RTCTime.sec;
        Date.min  = RTCTime.min;
        Date.hour = RTCTime.hour;
        
        while(Check_haveOneDaySub(RTCTime)) //判断DS1302日期是否是0年1月1日
        {
           TempTime = TimeSub_OneDay(RTCTime);  //减少一天
           DS1302_Read_Date(&RTCTime);          //获取时间（先读在写减少误差）
           RTCTime.year = TempTime.year;        //更新日期
           RTCTime.mon  = TempTime.mon;
           RTCTime.day  = TempTime.day;
           DS1302_Write_date(RTCTime);          //写入时间
            
           //更新现在的日期
           Date = TimeAdd_OneDay(Date);               //增加一天
           SaveDate_Write(10,Date);                   //保存日期
        }
        if(OldTime.sec == Date.sec)
        {
            OldTime.sec = Date.sec;
            if(Cnt<10){Cnt++;}
            else{
              //硬件故障 == 》检查是否还有时间==》清除剩余时间
              if(Check_ValidTime()) 
              {
                 ValidTimeDisable();   //清除有效时间
                 SurplusTime.hour = 0;
                 SurplusTime.min  = 0;
                 SurplusTime.sec  = 0;
              }
              Cnt = 0;              
            }
            return ;
        }else{
            OldTime.sec = Date.sec;
            Cnt=0;        
        }
        
        
        
        //先判断时间的合法性
        if(JudgeTimeLegal(StartTime,Date,EndTime)==1)
        {
           SurplusTime = differenceTime(Date, EndTime);  //计算剩余时间        
        }else{
           SurplusTime.hour = 0;
           SurplusTime.min  = 0;
           SurplusTime.sec  = 0;
        }

    }        
}


//======================================================
//======================================================
//预测n小时后的日期时间
_TimeType Get_nHourFutureTime(_TimeType Time,u16 hour)
{
   //一天的计数
   while(hour>=24)
   {
       Time = TimeAdd_OneDay(Time);         //增加一天
       hour -= 24;
   }
   //剩余的时间刚好够一天
   if((Time.hour+hour)>=24)
   {  
       Time.hour  = (Time.hour+hour)%24;
       Time = TimeAdd_OneDay(Time);         //增加一天
   }else{
       Time.hour  += hour;
   }
   return Time;
}

//预测n分钟后的日期时间
_TimeType Get_nMinFutureTime(_TimeType Time,u16 min)
{
   if(min/60)
   {
      Time = Get_nHourFutureTime(Time,min/60); //先预测n小时后的日期   
   }
   min %=60; //得到剩余分钟
   Time.min +=min;
   if(Time.min >=60)
   {
       Time.hour++;
       Time.min %=60;
       if(Time.hour>=24)
       {
         Time.hour = 0;
         Time = TimeAdd_OneDay(Time);  //日期增加一天
       }
   }
   return Time;
}




//======================================================
/*
例如：写入24小时
先判断是否还有时间
1.记录写入时间：记录现在时间(写入铁电)
2.预测24小时后的日期时间，(写入铁电)
3.写入有时间标记
*/
//设置有效时间
void RegisterValidTime_hour(u16 hour)
{
   StartTime = Date;                            //得到现在时间
   EndTime   = Get_nHourFutureTime( Date,hour); //计算X小时后的时间
   SaveTime_Write(20,StartTime);
   SaveTime_Write(30,EndTime)  ; 
}
//设置有效时间
void RegisterValidTime_min(u16 min)
{
   StartTime = Date;                              //得到现在时间
   EndTime   = Get_nMinFutureTime( Date,min);     //计算X小时后的时间
   SaveTime_Write(20,StartTime);
   SaveTime_Write(30,EndTime)  ; 
}

//有效时间取消(清除时间)
void ValidTimeDisable(void)
{
   SaveTime_Read(30,&EndTime);
   StartTime = EndTime;                            //得到现在时间
   SaveTime_Write(20,StartTime);
}

//检查是否还有时间可用(0 无  1有)
int Check_ValidTime(void)
{
  if(SurplusTime.hour)
  {
    return 1;
  }else if(SurplusTime.min){
    return 1;
  }else if(SurplusTime.sec){
    return 1;
  }
  return 0;
}

//======================================================
//设置系统时间(年月日时分秒)
void SetNewSystempDate(_TimeType Time)
{
    _TimeType tempTime = RTC_Date;    //获取模版时间
    Date.year     = Time.year;        //获得新的日期
    Date.mon      = Time.mon;
    Date.day      = Time.day;
    Date.wday     = Time.wday;   
    tempTime.wday = Time.wday; 
    tempTime.hour = Time.hour;
    tempTime.min  = Time.min ;
    tempTime.sec  = Time.sec ;
    
    DS1302_Write_date(tempTime);          //写入新时间
    DS1302_WriteReg(0x07,0x00|0x10|0x00); //配置启动
    
    SaveDate_Write(10,Date);         //保存正常时间日期    
}













/********************************End of File************************************/

