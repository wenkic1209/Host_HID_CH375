  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : tm_stm32_gpio.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-10-21         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include ".\tm_library\tm_stm32_gpio.h"

/* Private function */
static uint16_t GPIO_UsedPins[13] = {0,0,0,0,0,0,0,0,0,0,0,0,0};

#if defined(USE_STDPERIPH_DRIVER)
void TM_GPIO_INT_EnableClock(GPIO_TypeDef* GPIOx);
uint16_t TM_GPIO_GetPortSource(GPIO_TypeDef* GPIOx);
void TM_GPIO_INT_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,GPIOMode_TypeDef  GPIO_Mode,  GPIOSpeed_TypeDef GPIO_Speed);

//�ܽų�ʼ��(ʱ��+����)
void TM_GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,GPIOMode_TypeDef  GPIO_Mode,  GPIOSpeed_TypeDef GPIO_Speed) {	
	/* Check input */
	if (GPIO_Pin == 0x00) {
		return;
	}
	/* Enable clock for GPIO */
	TM_GPIO_INT_EnableClock(GPIOx);
	/* Do initialization */
	TM_GPIO_INT_Init(GPIOx, GPIO_Pin, GPIO_Mode, GPIO_Speed);
}



/* Private functions */
//�ܽų�ʼ��(����)
void TM_GPIO_INT_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,GPIOMode_TypeDef  GPIO_Mode,  GPIOSpeed_TypeDef GPIO_Speed) {
    GPIO_InitTypeDef GPIO_InitStruct;
    
	uint8_t i;
	uint8_t ptr = TM_GPIO_GetPortSource(GPIOx);
	
	/* Go through all pins */
	for (i = 0x00; i < 0x10; i++) {
		/* Pin is set */
		if (GPIO_Pin & (1 << i)) {
            		/* Pin is used */
		    GPIO_UsedPins[ptr] |= 1 << i;    //�˿���ʹ��
			/* Set 11 bits combination for analog mode */
            GPIO_InitStruct.GPIO_Pin  = GPIO_Pin  ;
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode ;
            GPIO_InitStruct.GPIO_Speed= GPIO_Speed;
            GPIO_Init(GPIOx,&GPIO_InitStruct);
		}
	}

}
//�ָ�ΪĬ��ģʽ
void TM_GPIO_DeInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	uint8_t i;
	uint8_t ptr = TM_GPIO_GetPortSource(GPIOx);
	
	/* Go through all pins */
	for (i = 0x00; i < 0x10; i++) {
		/* Pin is set */
		if (GPIO_Pin & (1 << i)) {
			/* Set 11 bits combination for analog mode */
            if(i<8)
            {
              GPIOx->CRL &= ~(0x0F<<i);     //Ĭ��00
            }else{
              GPIOx->CRH &= ~(0x0F<<(i-8));
            }			
			/* Pin is not used */
			GPIO_UsedPins[ptr] &= ~(1 << i);
		}
	}
}
//����ܽ�
void TM_GPIO_Lock(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	uint32_t d;
	
	/* Set GPIO pin with 16th bit set to 1 */
	d = 0x00010000 | GPIO_Pin;
	
	/* Write to LCKR register */
	GPIOx->LCKR = d;
	GPIOx->LCKR = GPIO_Pin;
	GPIOx->LCKR = d;
	
	/* Read twice */
	(void)GPIOx->LCKR;
	(void)GPIOx->LCKR;
}

//��ȡ�ܽ�λ��
uint16_t TM_GPIO_GetPinSource(uint16_t GPIO_Pin) {
	uint16_t pinsource = 0;
	
	/* Get pinsource */
	while (GPIO_Pin > 1) {
		/* Increase pinsource */
		pinsource++;
		/* Shift right */
		GPIO_Pin >>= 1;
	}
	
	/* Return source */
	return pinsource;
}

//��ȡ�˿ں�
uint16_t TM_GPIO_GetPortSource(GPIO_TypeDef* GPIOx) {
	/* Get port source number */
	/* Offset from GPIOA                       Difference between 2 GPIO addresses */
	return ((uint32_t)GPIOx - (GPIOA_BASE)) / ((GPIOB_BASE) - (GPIOA_BASE));
}

/* Private functions */
//ʹ�ܶ˿�ʱ��
void TM_GPIO_INT_EnableClock(GPIO_TypeDef* GPIOx) {
	/* Set bit according to the 1 << portsourcenumber */
#if defined(STM32F0xx)
	RCC->AHBENR |= (1 << (TM_GPIO_GetPortSource(GPIOx) + 17));
#elif defined(STM32F4xx) 
	RCC->AHB1ENR |= (1 << TM_GPIO_GetPortSource(GPIOx));    
#else
	RCC->APB2ENR |= (1 << TM_GPIO_GetPortSource(GPIOx));
#endif
}

//ʧ�ܶ˿�ʱ��
void TM_GPIO_INT_DisableClock(GPIO_TypeDef* GPIOx) {
	/* Clear bit according to the 1 << portsourcenumber */
#if defined(STM32F0xx)
	RCC->AHBENR   &= ~(1 << (TM_GPIO_GetPortSource(GPIOx) + 17));
#elif defined(STM32F4xx) 
	RCC->AHB1ENR  &= ~(1 << TM_GPIO_GetPortSource(GPIOx));    
#else
	RCC->APB2ENR &= ~(1 << TM_GPIO_GetPortSource(GPIOx));
#endif
}
//���ʹ�õĹܽ�
uint16_t TM_GPIO_GetUsedPins(GPIO_TypeDef* GPIOx) {
	/* Return used */
	return GPIO_UsedPins[TM_GPIO_GetPortSource(GPIOx)];
}

//������ɵĹܽ�
uint16_t TM_GPIO_GetFreePins(GPIO_TypeDef* GPIOx) {
	/* Return free pins */
	return ~GPIO_UsedPins[TM_GPIO_GetPortSource(GPIOx)];
}





#endif
 
/********************************End of File************************************/

