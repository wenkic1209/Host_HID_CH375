/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : DS1302.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-06-15         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _DS1302_H
#define _DS1302_H

#include "stm32f10x.h"
    
typedef struct _Time
{
    u16 year;  /* 年  , 0 to 99 */
    u8  mon;   /* 月  , 1 to 12 */	
    u8  day;   /* 日  , 1 to 31 */
    u8  wday;  /* 星期, 1 to 7  */
    u8  hour;  /* 时  , 0 to 23 */	
    u8  min;   /* 分  , 0 to 59 */	
    u8  sec;   /* 秒  , 0 to 59 */
}_TimeType;
extern _TimeType Date; //日期

typedef struct __Time
{
    u16  hour;  /* 时  , 0 to 23 */	
    u8   min;   /* 分  , 0 to 59 */	
    u8   sec;   /* 秒  , 0 to 59 */
}TimeType;
extern TimeType SurplusTime  ; //剩余时间

void DS1302_Init(void);                   //芯片初始化
//void DS1302_Write_date(_TimeType Date); //写入时间
//void DS1302_Read_Date(_TimeType*Date);  //读取时间

void print_date(void);   //打印调试信息
void RTC_Data_1sCheck(void);      //时间读取（1S更新一次）
void SetNewSystempDate(_TimeType Time);  //设置系统时间
void RegisterValidTime_hour(u16 hour);   //增加时间（小时级增加）
void RegisterValidTime_min(u16 min);     //增加时间（分钟级增加）
void ValidTimeDisable(void);             //有效时间取消(清除时间)
int Check_ValidTime(void);               //检查是否还有时间可用(0 无  1有)
int CompareTimeSame(TimeType Start,TimeType End); //比较时间是否相同

#endif
/********************************End of File************************************/

