/*
 * Copyright (c) 2020 https://blog.csdn.net/Wekic
 * All rights reserved
 */

#include "..\multi_timer\multi_timer.h"

//timer handle list head.
static struct Timer* head_handle = NULL;

//Timer ticks
static uint32_t _timer_ticks = 0;

/**
  * @brief  Initializes the timer struct handle.
  * @param  handle: the timer handle strcut.
  * @param  timeout_cb: timeout callback.
  * @param  repeat: repeat interval time.
  * @retval None
  */
 //定时器回调任务初始化 (数据块,超时时间,回调时间)
void tm_timer_init(struct Timer* handle, uint32_t timeout, uint32_t repeat)
{
	// memset(handle, sizeof(struct Timer), 0);
    // handle->timeout_cb = timeout_cb;        //回调块
    handle->Flag    = 0;                       //清除标志
	handle->timeout = _timer_ticks + timeout;  //设置超时时间
	handle->repeat  = repeat;                  //设置回调时间
}

/**
  * @brief  Start the timer work, add the handle into work list.
  * @param  btn: target handle strcut.
  * @retval 0: succeed. -1: already exist.
  */
 //注册任务块
int tm_timer_start(struct Timer* handle)
{
	struct Timer* target = head_handle;  //获取头
	if(handle->defult==0)handle->defult =1;
	if(handle->Flag)handle->Flag = 0;
	//寻找链表尾
	while(target) {                      //
		if(target == handle) return -1;	//already exist.(重复任务，注册失败)
		target = target->next;
	}
    //链接新的任务
	handle->next = head_handle;
	head_handle = handle;
	return 0;
}

/**
  * @brief  Stop the timer work, remove the handle off work list.
  * @param  handle: target handle strcut.
  * @retval None
  */
 //删除任务块
void tm_timer_stop(struct Timer* handle)
{
	struct Timer** curr;
	//得到链表头地址
	for(curr = &head_handle; *curr; ) {
		//获取现在的地址
		struct Timer* entry = *curr;
		//判断是否与传入的定时任务回调块地址相同
		if (entry == handle) {
			*curr = entry->next;
//			free(entry);
		} 
		//偏移下一个回调块
		else{
			curr = &entry->next;
		}

	}
}

/**
  * @brief  Check Timer Run Result Flag
  * @param  handle: target handle strcut.
  * @retval None
  */
int tm_timer_CheckFlag(struct Timer* handle)
{
   if(handle->defult==0){
	   handle->defult = 1;
	   handle->Flag   = 1;
   }
   return handle->Flag; 
}
/**
  * @brief  Clear Timer  Result Flag
  * @param  handle: target handle strcut.
  * @retval None
  */
void tm_timer_ClearFlag(struct Timer* handle)
{
   handle->Flag = 0;
}

/**
  * @brief  main loop.
  * @param  None.
  * @retval None
  */
 //任务链表检查
static void timer_loop()
{
	struct Timer* target;
	//循环 （1.得到链表头 2.判断下一个是否为空 3.继续下一个）
	for(target=head_handle; target; target=target->next) {
		//超出时间
		if(_timer_ticks >= target->timeout) {
			//重载为0,去掉这个定时任务
			if(target->repeat == 0) {
				tm_timer_stop(target);
			} 
			//设置下一次超时时间
			else {
				target->timeout = _timer_ticks + target->repeat;
			}
			//运行回调函数
			//target->timeout_cb();
            target->Flag = 1; //定时标记
		}
	}
}

/**
  * @brief  background ticks, timer repeat invoking interval 1ms.
  * @param  None.
  * @retval None.
  */
 //提供时间基准
static void timer_ticks()
{
	_timer_ticks++;
}

/**
  * @brief  background ticks, timer repeat invoking interval 1ms.
  * @param  None.
  * @retval None.
  */
void tm_Timer_MultiScan(void)
{
   timer_ticks(); //时间基准
   timer_loop();  //定时
}
