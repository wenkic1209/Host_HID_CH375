/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : tm_string.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-13         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _tm_string_H
#define _tm_string_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f10x.h"

typedef union _u32{
	u32 data   ;
    u8  byte[4];
}u32_unionType;

typedef union _int64_t{
	u64      data               ;
    u8       byte[sizeof(u64)]  ;
}uint64_unionType;

typedef union _u16 {
	u16 data   ;
    u8  byte[2];
}u16_unionType;


// 内存数据对比匹配
/*
*dest    要被检索的内存起始地址
DestSize 检索匹配大小
*str     要搜索的内存起始地址
StrSize  搜索的大小
返回 :成功==>第一次出现的位置地址  失败==>NULL
*/
char *  tm_strstr( char *dest,unsigned int  DestSize,  char *str,unsigned int  StrSize);
/*
*dest    要被检索的内存起始地址
DestSize 检索匹配大小
*str     要搜索的内存起始地址
StrSize  搜索的大小
返回 :成功==>第一次出现的位置地址  失败==>NULL
*/
//   内存数据对比匹配(倒叙查询第一个匹配数据串)
/*   要被检索的内存起始地址,检索匹配大小,要搜索的内存起始地址,搜索的大小*/
char* tm_EndStrstr(char* dest, unsigned int  DestSize, char* str, unsigned int  StrSize);
/*
*dest    要被检索的内存起始地址
DestSize 检索匹配大小
*str     要搜索的内存起始地址
StrSize  搜索的大小

返回 :成功==>下标位置  失败==>-1
*/
int tm_BF_strstr(char* dest, unsigned int  DestSize, char* str, unsigned int  StrSize);


//选择排序（类似冒泡排序）
void sort_u32(u32* dat, int len);


/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

