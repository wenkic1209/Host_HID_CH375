#ifndef _Exti_H
#define _Exti_H

#include "stm32f10x.h"

struct ExitGPIO{
   GPIO_TypeDef * GPIO;
   uint16_t       Pin ;  
};

//外部中断管脚初始化========/*管脚信息          ,抢占优先级                , 响应优先级*/
void ExitGPIO_Init(struct ExitGPIO GPIO,uint8_t PreemptionPriority, uint8_t SubPriority);

void ExtiPA12_init(void);
extern u16 MoterSpeedCnt;

#endif
