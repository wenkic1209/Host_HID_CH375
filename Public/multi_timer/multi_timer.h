/*
 * Copyright (c) 2020 https://blog.csdn.net/Wekic
 * All rights reserved
 */
 
#ifndef _MULTI_TIMER_H_
#define _MULTI_TIMER_H_

#include "stdint.h"
#include "stddef.h"

typedef struct Timer {
    uint32_t timeout;         //超时时间
    uint32_t repeat;          //重置时间
	u8       defult;
    u8       Flag  ;
//    void (*timeout_cb)(void); //回调任务函数
    struct Timer* next;       //下一个任务
}TM_Timer;

#ifdef __cplusplus  
extern "C" {  
#endif  

void tm_timer_init(struct Timer* handle,  uint32_t timeout, uint32_t repeat); //设置定时参数
int  tm_timer_start(struct Timer* handle);                                    //加入定时任务
void tm_timer_stop(struct Timer* handle);                                     //删除定时任务
int  tm_timer_CheckFlag(struct Timer* handle);       //检查是否完成定时
void tm_timer_ClearFlag(struct Timer* handle);      //清除任务标志
void tm_Timer_MultiScan(void);    //定时器基准

#ifdef __cplusplus
} 
#endif

#endif
