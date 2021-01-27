/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : Public.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-11-09         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _Public_H
#define _Public_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "sys.h"      //位带操作
#include "delay.h"    //滴答定时延时
#include "queue.h"    //缓存队列
#include "usart.h"    //串口初始化
#include "PWM.h"      //串口初始化
#include "debug.h"    //调试信息输出
#include "timer.h"    //定时器初始化
#include "des.h"      //定时器初始化
#include "spi_dri.h"      //定时器初始化
#include "..\multi_timer\multi_timer.h"   //定时器操作

#include "tm_library.h"
#include "..\Public\tm_library\tm_stm32_gpio.h"  //管脚操作
#include "..\Public\tm_library\tm_stm32_spi.h"   //spi模版
#include "..\Public\tm_library\tm_que.h"         //spi模版
    

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

