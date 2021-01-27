/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : tm_stm32_gpio.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-10-21         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _tm_stm32_gpio_H
#define _tm_stm32_gpio_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

/**
 * @brief GPIO Pins declarations 
 * @note  For HAL drivers compatibility
 */
  
#ifndef GPIO_PIN_0
#define GPIO_PIN_0		((uint16_t)0x0001)
#define GPIO_PIN_1		((uint16_t)0x0002)
#define GPIO_PIN_2		((uint16_t)0x0004)
#define GPIO_PIN_3		((uint16_t)0x0008)
#define GPIO_PIN_4		((uint16_t)0x0010)
#define GPIO_PIN_5		((uint16_t)0x0020)
#define GPIO_PIN_6		((uint16_t)0x0040)
#define GPIO_PIN_7		((uint16_t)0x0080)
#define GPIO_PIN_8		((uint16_t)0x0100)
#define GPIO_PIN_9		((uint16_t)0x0200)
#define GPIO_PIN_10		((uint16_t)0x0400)
#define GPIO_PIN_11		((uint16_t)0x0800)
#define GPIO_PIN_12		((uint16_t)0x1000)
#define GPIO_PIN_13		((uint16_t)0x2000)
#define GPIO_PIN_14		((uint16_t)0x4000)
#define GPIO_PIN_15		((uint16_t)0x8000)
#define GPIO_PIN_ALL	((uint16_t)0xFFFF)
#endif

/**
 * @brief GPIO Pins declarations 
 * @note  For STD Periph drivers compatibility
 */

#ifndef GPIO_Pin_0
#define GPIO_Pin_0		((uint16_t)0x0001)
#define GPIO_Pin_1		((uint16_t)0x0002)
#define GPIO_Pin_2		((uint16_t)0x0004)
#define GPIO_Pin_3		((uint16_t)0x0008)
#define GPIO_Pin_4		((uint16_t)0x0010)
#define GPIO_Pin_5		((uint16_t)0x0020)
#define GPIO_Pin_6		((uint16_t)0x0040)
#define GPIO_Pin_7		((uint16_t)0x0080)
#define GPIO_Pin_8		((uint16_t)0x0100)
#define GPIO_Pin_9		((uint16_t)0x0200)
#define GPIO_Pin_10		((uint16_t)0x0400)
#define GPIO_Pin_11		((uint16_t)0x0800)
#define GPIO_Pin_12		((uint16_t)0x1000)
#define GPIO_Pin_13		((uint16_t)0x2000)
#define GPIO_Pin_14		((uint16_t)0x4000)
#define GPIO_Pin_15		((uint16_t)0x8000)
#define GPIO_Pin_All	((uint16_t)0xFFFF)
#endif

/**
 * @brief  Initializes GPIO pins(s) (管脚初始化)(时钟+配置)
 * @note   This function also enables clock for GPIO port
 * @param  GPIOx: Pointer to GPIOx port you will use for initialization
 * @param  GPIO_Pin: GPIO pin(s) you will use for initialization
 * @param  GPIO_Mode: Select GPIO mode. This parameter can be a value of @ref TM_GPIO_Mode_t enumeration
 * @param  GPIO_Speed: Select GPIO speed. This parameter can be a value of @ref TM_GPIO_Speed_t enumeration
 * @retval None
 */
void TM_GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin,GPIOMode_TypeDef  GPIO_Mode,  GPIOSpeed_TypeDef GPIO_Speed);

/**
 * @brief  Sets pin(s) low  (管脚输出低电平)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin low
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them low
 * @retval None
 */
#define TM_GPIO_SetPinLow(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)(((uint32_t)GPIO_Pin) << 16))

/**
 * @brief  Sets pin(s) high  (管脚输出高电平)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin high
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them high
 * @retval None
 */
#define TM_GPIO_SetPinHigh(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)(GPIO_Pin))

/**
 * @brief  Sets pin(s) value (设置管脚的输出)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to set them value
 * @param  val: If parameter is 0 then pin will be low, otherwise high
 * @retval None
 */
#define TM_GPIO_SetPinValue(GPIOx, GPIO_Pin, val)	((val) ? TM_GPIO_SetPinHigh(GPIOx, GPIO_Pin) : TM_GPIO_SetPinLow(GPIOx, GPIO_Pin))

/**
 * @brief  Toggles pin(s) (电平反转)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to toggle pin value
 * @param  GPIO_Pin: Select GPIO pin(s). You can select more pins with | (OR) operator to toggle them all at a time
 * @retval None
 */
#define TM_GPIO_TogglePinValue(GPIOx, GPIO_Pin)		((GPIOx)->ODR ^= (GPIO_Pin))

/**
 * @brief  Sets value to entire GPIO PORT (设置端口值)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to set value
 * @param  value: Value for GPIO OUTPUT data
 * @retval None
 */
#define TM_GPIO_SetPortValue(GPIOx, value)			((GPIOx)->ODR = (value))

/**
 * @brief  Gets input data bit (管脚输入读取)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define TM_GPIO_GetInputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->IDR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets output data bit (获取输出值)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output bit value
 * @param  GPIO_Pin: GPIO pin where you want to read value
 * @retval 1 in case pin is high, or 0 if low
 */
#define TM_GPIO_GetOutputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->ODR & (GPIO_Pin)) == 0 ? 0 : 1)

/**
 * @brief  Gets input value from entire GPIO PORT (获取端口输入值)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read input data value
 * @retval Entire PORT INPUT register
 */
#define TM_GPIO_GetPortInputValue(GPIOx)			((GPIOx)->IDR)

/**
 * @brief  Gets output value from entire GPIO PORT (获取端口输出值)
 * @note   Defined as macro to get maximum speed using register access
 * @param  GPIOx: GPIOx PORT where you want to read output data value
 * @retval Entire PORT OUTPUT register
 */
#define TM_GPIO_GetPortOutputValue(GPIOx)			((GPIOx)->ODR)

/**
 * @brief  Locks GPIOx register for future changes
 * @note   You are not able to config GPIO registers until new MCU reset occurs
 * @param  *GPIOx: GPIOx PORT where you want to lock config registers
 * @param  GPIO_Pin: GPIO pin(s) where you want to lock config registers
 * @retval None
 */
void TM_GPIO_Lock(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

//获取管脚位置
uint16_t TM_GPIO_GetPinSource(uint16_t GPIO_Pin);
/*获取端口号位置 */
uint16_t TM_GPIO_GetPortSource(GPIO_TypeDef* GPIOx);
/* C++ detection */
#ifdef __cplusplus
}
#endif
#endif
/********************************End of File************************************/

