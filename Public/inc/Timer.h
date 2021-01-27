#ifndef _Timer_H
#define _Timer_H

#include "stm32f10x.h"
extern u16 srandCnt; //这是随机数种子
void Timer2_Init(u16 arr,u16 psc);
void Timer3_Init(u16 arr,u16 psc);


#endif
