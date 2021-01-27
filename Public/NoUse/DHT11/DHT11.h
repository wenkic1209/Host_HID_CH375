#ifndef _DHT11_H
#define _DHT11_H


#include "stm32f10x.h"


typedef struct {
    u8 temperature1; //物業
    u8 temperature2;
    u8 humidity1;    //梁業
    u8 humidity2;
    u8 check;
} DHT11_TypeDef;
extern DHT11_TypeDef DHT11Dat;

extern float DHT_temperature ; //梁業
extern float DHT_humidity    ; //物業



void DHT11_Init(void);
void Read_DHT11(void);



#endif


