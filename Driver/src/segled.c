  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : segled.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-11-09         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "segled.h"
#include "public.h"
//实质写入的数据
static u8 Seg_Dat[3]  = {0x01,0x02,0x03}; //刷新数据

#define sa  0x40
#define sb  0x20
#define sc  0x10
#define sd  0x08
#define se  0x04
#define sf  0x02
#define sg  0x01
#define sh  0x80


#define leg_null (0x00                 )
#define leg_0    (sa|sb|sc|sd|se|sf    )
#define leg_1    (se|sf                )
#define leg_2    (sa|sb|sg|se|sd       )
#define leg_3    (sa|sb|sc|sd|sg       )
#define leg_4    (sb|sc|sf|sg          )
#define leg_5    (sa|sf|sg|sc|sd       )
#define leg_6    (sa|sf|sg|sc|sd|se    )
#define leg_7    (sf|sa|sb|sc          )
#define leg_8    (sa|sb|sc|sd|se|sf|sg )
#define leg_9    (sa|sb|sc|sd|sf|sg    )
#define leg_A    (sa|sb|sc|se|sf|sg    )
#define leg_b    (sc|sd|se|sf|sg       )
#define leg_C    (sa|sd|se|sf          )
#define leg_d    (sb|sc|sd|se|sg       )
#define leg_E    (sa|sd|se|sf|sg       )
#define leg_F    (sa|se|sf|sg          )
#define leg_L    (sd|se|sf             )
#define leg_P    (sa|sb|se|sf|sg       )
#define leg_J    (sb|sc|sd|se          )
#define leg__    (sg                   )
#define leg_     (sd                   )
#define leg_w    (sh                   )
#define leg_H    (sb|sc|se|sf|sg       )
#define leg_U    (sb|sc|sd|se|sf       )
#define leg_u    (sc|sd|se             )
#define leg_n    (sc|se|sg             )
//static u8 OSeg_Dat[3] = {0}; //缓存数据
// *INDENT-OFF*
//--------------------------' '  '0'    1      2    3     4     5     6     7     8     9     A     b     C     d     E     F     L     P     J     _     -     .     H     U     u     n
const u8  DataTable1[27]  = {0x00, 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71, 0x38, 0x73, 0x1E, 0x08, 0x40, 0x80, 0x76, 0x3E, 0x1C, 0x54};
const u8  DataTable2[27]   = {0x00, 0x7E, 0x06, 0x6D, 0x79, 0x33, 0xB6, 0xBE, 0xE0, 0xFE, 0xE6, 0xEE, 0x3E, 0x9C, 0x7A, 0x9E, 0x8E, 0x1C, 0xCE, 0x78, 0x10, 0x02, 0x01, 0x6E, 0x7C, 0x38, 0x2A}; 
const u8  DataTable[27]  = {
leg_null, 
leg_0   , 
leg_1   , 
leg_2   , 
leg_3   , 
leg_4   , 
leg_5   , 
leg_6   , 
leg_7   , 
leg_8   , 
leg_9   , 
leg_A   , 
leg_b   , 
leg_C   , 
leg_d   , 
leg_E   , 
leg_F   , 
leg_L   , 
leg_P   , 
leg_J   , 
leg__   , 
leg_    , 
leg_w   ,
leg_H   , 
leg_U   , 
leg_u   , 
leg_n   
};
const u8  ASCIITable[27] = " 0123456789AbCdEFLPJ_-.HUun";
// *INDENT-ON*
#include "string.h"
#include "stdarg.h"
#include "stdio.h"

static void HC595_Write(u8 dat1,u8 dat2);

//写入数据
void Seg_print(char *fmt, ...)
{
    u8 Str[7]={0};
    u8 TStr[7]={0};
    u8 StrlenC,lenC = 0;
    u8 *pSearch = NULL; //查询字符返回返回的地址
    va_list ap; //定义个链表
    va_start(ap, fmt); //链表开始
    vsnprintf((char *)Str,7, fmt, ap); //数据格式化
    va_end(ap);       //链表结束
    StrlenC = strlen((char *)Str);
    memcpy(TStr, Seg_Dat, 3); //复制上次显示内容
    // *INDENT-OFF*
    for(u8 i = 0; i < StrlenC && lenC<3; i++) {
        //查找字符
        pSearch = (u8 *)strchr((const char *)ASCIITable, Str[i]);
        if(pSearch != NULL || Str[i] == '/' ) {               //查询列表中有这个字符
            if(Str[i] == '/' && Str[i+1]=='.') {  
                    TStr[lenC]      = 0x80; //第一为为'.',无数据
                    i++   ;
                    lenC++;            
            }           
            else if(Str[i] == '.') {        //判断刚刚的字符是否是'.',是就做特殊 处理
                if(lenC == 0 ) {            //只显示'.'
                    TStr[lenC]      = 0x80; //第一为为'.',无数据
                    lenC++;
                } else {
                    TStr[lenC - 1] |= 0x80; //其余是简便到前一个数据
                }
            } 
            else {
                if(pSearch != NULL)
                TStr[lenC] = DataTable[pSearch - ASCIITable]; //查表获得数据
                lenC++;
            }
        } else {
            TStr[lenC] = 0xFF;                                //查表无结果
            lenC++;
        }
        if(lenC>=3)break;
    }
    // *INDENT-ON*
    memcpy(Seg_Dat, TStr, 3); //赋予新值(更新显示)

}
//数码管初始化
void Seg_Init(void)
{
  TM_GPIO_Init(GPIOC,GPIO_Pin_7 ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);  //HC595_SDI  数据线
  TM_GPIO_Init(GPIOA,GPIO_Pin_12,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);  //HC595_SHCP 输入时钟
  TM_GPIO_Init(GPIOA,GPIO_Pin_11,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);  //HC595_STCP 数据锁存
  
  TM_GPIO_SetPinValue(GPIOC,GPIO_Pin_7 ,1);    
  TM_GPIO_SetPinValue(GPIOA,GPIO_Pin_12,1);
  TM_GPIO_SetPinValue(GPIOA,GPIO_Pin_11,1);  
}

#define seg_delay  100

//HC595写入数据
static void HC595_Write(u8 dat1,u8 dat2)
{
   u16 dat = (dat1<<8|dat2);
   for(u8 i=0;i<16;i++)
    {
        if(dat&0x8000){
            TM_GPIO_SetPinValue(GPIOC,GPIO_Pin_7 ,1);
        }else{
            TM_GPIO_SetPinValue(GPIOC,GPIO_Pin_7 ,0);
        }
        dat<<=1;
        TM_GPIO_SetPinValue(GPIOA,GPIO_Pin_11,0); 
        for(u8 m=0;m<seg_delay;m++){}   
        TM_GPIO_SetPinValue(GPIOA,GPIO_Pin_11,1);             
    }
    TM_GPIO_SetPinValue(GPIOA,GPIO_Pin_12,0); 
    for(u8 m=0;m<seg_delay;m++){}     
    TM_GPIO_SetPinValue(GPIOA,GPIO_Pin_12,1); 
}

//HC595刷数码管
void SegScan_Timer_ms(void)
{
   static u8 i = 0;
   HC595_Write(0x00,0x00);    
   HC595_Write(0x09<<(i+2),Seg_Dat[i]);
   i++;
   i %=3;
}
 
 
/********************************End of File************************************/

