#ifndef _ADC_H
#define _ADC_H

#include "stm32f10x.h"

void ADC1PA0_Init(void);
void ADC2PA1_Init(void);

u16 Get_ADC1PA0Average(u8 AverCnt);
u16 Get_ADC2PA1Average(u8 AverCnt);


u16 Get_TADC1PA0Average(void); //ADC1��ʱ����ȡֵ
u16 Get_TADC2PA1Average(void); //ADC2��ʱ����ȡֵ

#endif


