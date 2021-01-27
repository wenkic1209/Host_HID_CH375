/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : debug.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-06-15         
  * Description  :    
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _debug_H
#define _debug_H

#include "stm32f10x.h"
#include "usart.h"

//声明调试输出函数
void sprint(const char* fmt, ...);
#if 0
#define   debug_log(...)
#else
#define   debug_log(...) do{sprint( __VA_ARGS__ );sprint("\r\n") ; }while(0)
#endif
#ifndef Print  //默认输出函数
  #define Print(...)    sprint( __VA_ARGS__ )   
#endif
/*
sprint ()默认打印函数函数
*/


//打印调试输出
/*!
 * \brief 调试输出级别
 */
#define NONE    0   //无
#define ERROR   1   //严重错误
#define WARN    2   //警告
#define INFO    3   //消息
#define ENTRY   4   //动作
#define DEBUG   5   //调试

/* 在publicdef中去定义 */
#define PRINT_LEVEL     NONE    /**< 默认调试级别 */
#define PRINT_ERR       ERROR    /**< 默认调试级别 */


#if (PRINT_ERR >= ERROR)
#define log_err(x)     Usart_SendString(COM1,x,strlen((const char*)x))
#else
#define log_err(...)
#endif


/*!
 * \brief   将变量名转成字符串的宏定义函数
 *          #是宏定义中转换字符串的用法
 */
#define name2str(name) (#name)

/*!
 * \brief   必须定义PRINT_LEVEL宏定义，否则编译阶段就直接报错退出
 */
#ifndef PRINT_LEVEL
#   error MUST Define PRINT_LEVEL!
#endif

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
int pr(int tag, char *tagstr, char *fmt, ...);

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
int pr_func(int tag, char *tagstr, char *filestr, const char *funcstr, int line, char *fmt, ...);



//错误        打印
#if (PRINT_LEVEL >= ERROR)
#define pr_err(...)     \
                        pr_func(ERROR, name2str(ERROR), __FILE__, __func__, __LINE__, __VA_ARGS__)
#else
#define pr_err(...)
#endif
//警告        打印
#if (PRINT_LEVEL >= WARN)
#define pr_warn(...)    pr(WARN, name2str(WARN), __VA_ARGS__);
#else
#define pr_warn(...)
#endif
//消息         打印
#if (PRINT_LEVEL >= INFO)
#define pr_info(...)    pr(INFO, name2str(INFO), __VA_ARGS__);
#else
#define pr_info(...)
#endif
//行动(进入)  打印
#if (PRINT_LEVEL >= ENTRY)
#define pr_entry(inout) \
                        pr(ENTRY, name2str(ENTRY), "%s() %s", __func__, #inout);
#else
#define pr_entry(inout)
#endif

//调试        打印
#if (PRINT_LEVEL >= DEBUG)
#define pr_debug(...)   pr(DEBUG, name2str(DEBUG), __VA_ARGS__);
#else
#define pr_debug(...)
#endif

/*!
 * \brief   带调试级别的打印输出宏函数
 *
 *          ...作为宏函数的可变变量，使用时用__VA_ARGS__
 *          或者##_VA_ARGS__(这里的##意思是即使参数为空也正常执行)。
 *          另一种不同的用法是宏定义中##可以拼接两个变量名，将其变成一个字符串。
 *          printf的第一个参数可以用多个字符串拼起来，不加逗号，
 *          如printf("Th""is"" is"" a"" string");
 */
void print(char* fmt, ...); 

//打印函数
#define pr_pure(tag, ...)    do {                                   \
            if (tag <= PRINT_LEVEL)                                 \
                Print(__VA_ARGS__);                                \
        } while(0);

#define pr_err_pure(fmt, ...)            pr_pure(ERROR, fmt, ##__VA_ARGS__);
#define pr_warn_pure(fmt, ...)           pr_pure(WARN, fmt, ##__VA_ARGS__);
#define pr_info_pure(fmt, ...)           pr_pure(INFO, fmt, ##__VA_ARGS__);
#define pr_debug_pure(fmt, ...)          pr_pure(DEBUG, fmt, ##__VA_ARGS__);


#endif
/********************************End of File************************************/

