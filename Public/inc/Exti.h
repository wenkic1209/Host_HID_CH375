#ifndef _Exti_H
#define _Exti_H

#include "stm32f10x.h"

struct ExitGPIO{
   GPIO_TypeDef * GPIO;
   uint16_t       Pin ;  
};

//�ⲿ�жϹܽų�ʼ��========/*�ܽ���Ϣ          ,��ռ���ȼ�                , ��Ӧ���ȼ�*/
void ExitGPIO_Init(struct ExitGPIO GPIO,uint8_t PreemptionPriority, uint8_t SubPriority);

void ExtiPA12_init(void);
extern u16 MoterSpeedCnt;

#endif
