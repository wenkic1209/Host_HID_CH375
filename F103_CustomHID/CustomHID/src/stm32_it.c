/**
  ******************************************************************************
  * @file    stm32_it.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/

#include "stm32_it.h"
#include "usb_istr.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

extern __IO uint8_t PrevXferComplete;
extern __IO uint32_t TimingDelay;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            STM32 Peripherals Interrupt Handlers                        */
/******************************************************************************/

/*******************************************************************************
* Function Name  : USB_IRQHandler
* Description    : This function handles USB Low Priority interrupts
*                  requests.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS) || defined (STM32F37X)
void USB_LP_IRQHandler(void)
#else
void USB_LP_CAN1_RX0_IRQHandler(void)
#endif
{
  USB_Istr();
}

/*******************************************************************************
* Function Name  : DMA1_Channel1_IRQHandler
* Description    : This function handles DMA1 Channel 1 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{  
//  Send_Buffer[0] = 0x07;
//  
//  if((ADC_ConvertedValueX >>4) - (ADC_ConvertedValueX_1 >>4) > 4)
//  {
//    if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
//    {
//      Send_Buffer[1] = (uint8_t)(ADC_ConvertedValueX >>4);
//      
//      /* Write the descriptor through the endpoint */
//      USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2);  
//      SetEPTxValid(ENDP1);
//      ADC_ConvertedValueX_1 = ADC_ConvertedValueX;
//      PrevXferComplete = 0;
//    }
//  }
//  
//  DMA_ClearFlag(DMA1_FLAG_TC1);
}

/*******************************************************************************
* Function Name  : EXTI_IRQHandler
* Description    : This function handles External lines interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)
void EXTI0_IRQHandler(void)
#elif defined (STM32F37X)
void EXTI2_TS_IRQHandler(void)
#else
void EXTI9_5_IRQHandler(void)
#endif
{
//  if(EXTI_GetITStatus(KEY_BUTTON_EXTI_LINE) != RESET)
//  {  
//    if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
//    {
//      Send_Buffer[0] = 0x05; 
//#if defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)
//      if (!STM_EVAL_PBGetState(Button_KEY) == Bit_RESET)
//#else
//      if (STM_EVAL_PBGetState(Button_KEY) == Bit_RESET)       
//#endif      
//      {
//        Send_Buffer[1] = 0x01;
//      }
//      else 
//      {
//        Send_Buffer[1] = 0x00;
//      }  
//      
//      /* Write the descriptor through the endpoint */
//      USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2);  
//      SetEPTxValid(ENDP1);
//      PrevXferComplete = 0;
//    }
//    /* Clear the EXTI line  pending bit */
//    EXTI_ClearITPendingBit(KEY_BUTTON_EXTI_LINE);
//  }
}
#if !defined(STM32L1XX_MD) &&  !defined(STM32L1XX_HD) && !defined(STM32L1XX_MD_PLUS)&& ! defined (STM32F37X) && ! defined (STM32F30X)
/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : This function handles External lines 15 to 10 interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void EXTI15_10_IRQHandler(void)
//{
////  if(EXTI_GetITStatus(TAMPER_BUTTON_EXTI_LINE) != RESET)
////  {  
////    if ((PrevXferComplete) && (bDeviceState == CONFIGURED))
////    {
////      Send_Buffer[0] = 0x06;
////      
////      if (STM_EVAL_PBGetState(Button_TAMPER) == Bit_RESET)
////      {
////        Send_Buffer[1] = 0x01;
////      }
////      else 
////      {
////        Send_Buffer[1] = 0x00;
////      }
////      
////      /* Write the descriptor through the endpoint */    
////      USB_SIL_Write(EP1_IN, (uint8_t*) Send_Buffer, 2);  
////     
////      SetEPTxValid(ENDP1);

////      PrevXferComplete = 0;
////    }
////    /* Clear the EXTI line 13 pending bit */
////    EXTI_ClearITPendingBit(TAMPER_BUTTON_EXTI_LINE);
////  }
//}

#endif /*STM32L1XX_HD*/

/*******************************************************************************
* Function Name  : USB_FS_WKUP_IRQHandler
* Description    : This function handles USB WakeUp interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

#if defined(STM32L1XX_MD) || defined(STM32L1XX_HD)|| defined(STM32L1XX_MD_PLUS)
void USB_FS_WKUP_IRQHandler(void)
#else
void USBWakeUp_IRQHandler(void)
#endif
{
  EXTI_ClearITPendingBit(EXTI_Line18);
}
/******************************************************************************/
/*                 STM32 Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32xxx.s).                                            */
/******************************************************************************/

/*******************************************************************************
* Function Name  : PPP_IRQHandler
* Description    : This function handles PPP interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*void PPP_IRQHandler(void)
{
}*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
