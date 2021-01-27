  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : tm_string.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2021-01-13         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "tm_string.h"
#include "stdio.h"
/*
*dest    要被检索的内存起始地址
DestSize 检索匹配大小
*str     要搜索的内存起始地址
StrSize  搜索的大小
返回 :成功==>第一次出现的位置地址  失败==>NULL
*/
// 内存数据对比匹配(正叙查询第一个匹配数据串)
/*   要被检索的内存起始地址,检索匹配大小,要搜索的内存起始地址,搜索的大小*/
char *  tm_strstr( char *dest,unsigned int  DestSize,  char *str,unsigned int  StrSize)
{
    char *ptr=NULL;
    char *sr1 = (char*)dest;
    char *sr2 = (char*)str;
    unsigned int m1 = 0;
    unsigned int m2 = 0;
    if(dest==NULL || str==NULL || DestSize >StrSize)return NULL;
    
    for(unsigned int i=0;i<StrSize;i++) //暴力查询
    {
       m1 = 0;m2 = 0; 
       ptr = &sr2[i]; //起始位置
       while(m1 != DestSize && sr1[m1] == ptr[m2] && i+m2 <StrSize)
       {
         m1++;
         m2++;
       }
       if(m1 == DestSize)return (char *)ptr; //对比成功
    }
    return NULL;
}
/*
*dest    要被检索的内存起始地址
DestSize 检索匹配大小
*str     要搜索的内存起始地址
StrSize  搜索的大小
返回 :成功==>第一次出现的位置地址  失败==>NULL
*/
//   内存数据对比匹配(倒叙查询第一个匹配数据串)
/*   要被检索的内存起始地址,检索匹配大小,要搜索的内存起始地址,搜索的大小*/
char* tm_EndStrstr(char* dest, unsigned int  DestSize, char* str, unsigned int  StrSize)
{
    char* str_Dest = (char*)dest;
    char* str2 = (char*)str;
    unsigned int m1 = 0;
    unsigned int m2 = 0;
    if (dest == NULL || str == NULL || DestSize > StrSize)return NULL; 
    for (unsigned int i = StrSize; i; i--) //暴力查询
    {
        m1 = DestSize; m2 = StrSize- (StrSize-i); 
        if (m2 < DestSize) return NULL;
        while (m1&&m2)
        {
            if (str_Dest[m1 - 1] == str2[m2 - 1])
            {
                m1--;
                m2--;
                if (m1 == 0)return (char*)&str2[m2]; //对比成功
            }
            else {
                break;
            }
        }  
    }
    return NULL;
}


#include <stdio.h>
#include <string.h>

/*
*dest    要被检索的内存起始地址
DestSize 检索匹配大小
*str     要搜索的内存起始地址
StrSize  搜索的大小

返回 :成功==>下标位置  失败==>-1
*/
//串普通模式匹配算法的实现函数，其中 B是伪主串，A是伪子串
int tm_BF_strstr(char* dest, unsigned int  DestSize, char* str, unsigned int  StrSize) {
    int i = 0;//初始化位置记录变量 
    int j = 0;
    while (i < StrSize && j < DestSize )//若i和j都大于字符串的长度就结束循环
    {
        if (str[i] == dest[j])
        {
            i++;
            j++;
        }
        else//匹配失败时回溯 
        {
            i = i - j + 1;//被匹配字符串右移1 
            j = 0        ;//匹配字符串初始化下标值 
        }
    }
    if (j >= DestSize)       //联合上面的while进行对比 
        return i - DestSize; //返回的是第一次匹配到的字符的下标值 
    return -1;   //查询失败
}


//******************排序**********************************
//元素比较
static int less_u32(u32 a, u32 b)
{
	return (a < b)?1:0;
}
//元素位置交换
static void exch_u32(u32* dat, int i, int j)
{
	int a = dat[i];
	dat[i] = dat[j];
	dat[j] = a;
}
//选择排序（类似冒泡排序）
void sort_u32(u32* dat, int len)
{
	for (int i = 0; i < len; i++)
	{
		int min = i;
		//寻找最小元素
		for (int m = i+1; m < len; m++)
		{
			if (less_u32(dat[m], dat[min]))
				min = m;
		}
		exch_u32(dat, i, min);     //元素交换
	}
}

/********************************End of File************************************/



