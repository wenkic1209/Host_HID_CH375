  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : debug.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-06-15         
  * Description  :    
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "debug.h"
#include "usart.h"

#include "stdarg.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "queue.h"
//===============================================================================
//              打印调试输出
/*================================= 接口函数 =================================*/

#define debugCom  COM1    //输出端口

void sprint(const char* fmt, ...)
{
    u8 StrlenC  = 0;           //长度
    u8 Str[100] = {0};            //缓存
    va_list ap;                //定义个链表
	//信息转换
    va_start(ap, fmt); //链表开始
    vsnprintf((char*)Str,100, fmt, ap);  //数据格式化
    va_end(ap);       //链表结束
    StrlenC = strlen((char*)Str);
	//输出信息
    Usart_SendString(debugCom, Str, StrlenC);
}
/*!
 * \brief 调试输出接口
 *
 * 备注：还有另外一种方法，用一个缓存使用snprintf实现，这样既可以控制
 *       输出，还可以方便存日志，这种方法后续再做
 *
 * \param[in] tag       调试级别
 * \param[in] tagstr    调试级别字符串
 * \param[in] fmt, ...  printf的可变参数
 */
int pr(int tag, char *tagstr, char *fmt, ...)
{
    /* 将变量定义放在if里面，提高输出关闭时的执行效率 */
    if (tag <= PRINT_LEVEL) {
		//级别字符
        Print("[%s] ",tagstr); 
		//输出信息转化 
		u8 Str[100] = {0};            
		va_list ap;                          //定义个链表
		va_start(ap, fmt);                   //链表开始
		vsnprintf((char*)Str,100, fmt, ap);  //数据格式化
		va_end(ap);                          //链表结束
		//输出信息
		Print("%s",Str);
        Print("\r\n");	
    }

    return 0;
}

/*!
 * \brief 调试信息加上级别、时间、文件名、函数名、文件行、换行信息
 *
 *        因为__func__ __LINE__ __FILE__这几个变量不能和printf的fmt参数
 *        直接拼接起来，所以这里用参数传入
 *
 * \param[in] tag       调试级别
 * \param[in] tagstr    调试级别字符串
 * \param[in] filestr   文件名字符串
 * \param[in] funcstr   函数名字符串
 * \param[in] line      当前代码行数
 * \param[in] fmt, ...  printf的可变参数
 */
int pr_func(int tag, char *tagstr, char *filestr, const char *funcstr, int line, char *fmt, ...)
{
    if (tag <= PRINT_LEVEL) {
		//打印信息
        Print("[%s]<%s %s() line:%d> ", tagstr, filestr, funcstr, line);
		//信息转换
		u8 *Str = NULL;            //缓存
		Str = malloc(100);
	    if(Str==NULL)return 1;
		va_list ap;                //定义个链表
		va_start(ap, fmt);         //链表开始
		vsnprintf((char*)Str,100, fmt, ap);  //数据格式化
		va_end(ap);                //链表结束
		//输出转换信息
		Print("%s",Str);
        Print("\r\n");
		//释放内存
		if(Str)free(Str);
    }

    return 0;
}

/*!
 * \brief 调试输出单元测试用例
 */
#ifdef PRINT_CTRL_MODULE_UNITEST
int print_ctrl_unitest()
{
    int i;
    pr_entry(in);

    pr_func(ERROR, name2str(ERROR), __FILE__, __func__, __LINE__, "err pr_func...");
    pr_err("err %s %d %f", "pr_err", 1024, 5.20);

    pr(WARN, name2str(WARN), "warn pr int: %d", 100);
    pr_warn("pr_warn %s", "test");

    pr(INFO, name2str(INFO), "int: %d", 100);
    pr(INFO, name2str(INFO), "info string %s", "test");

    pr(DEBUG, name2str(DEBUG), "pr debug string %s", "test");
    pr_debug("pr_debug string%s\r\n", "test");

    for (i = 0; i < 100; i++)
        pr_info_pure("%d ", i);
    pr_info_pure("\r\n");

    for (i = 0; i < 100; i++)
        pr_debug_pure("%d ", i);
    pr_debug_pure("\r\n");

    pr_entry(out);

    return 0;
}

/* 如果不是所有模块一起做单元测试，则需要自己定义main函数入口 */
#ifndef ALL_MODULES_UNITEST
int main()
{
    return print_ctrl_unitest();
}
#endif /* ALL_MODULES_UNITEST */
#endif
/********************************End of File************************************/

