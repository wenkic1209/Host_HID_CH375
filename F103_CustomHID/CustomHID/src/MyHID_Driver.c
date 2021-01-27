  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : MyHID_Driver.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-12-29         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "MyHID_Driver.h"
#include "string.h"
//------------------------
#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"
//------------------------

/*  User DEMO
  void main(void)
  {
	  u8 buf[64] = 0;
	  u8 len     = 60;
	  USB_HID_Init();
	  while(1)
	  {
		  HID_Send_Report(buf,len); //发送数据
	  }
  }
*/


//USB初始化
void USB_HID_Init(void)
{
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);          //创建不了HID,就打开这些时钟   
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
//    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    //USB_HID
    Set_System();
    USB_Interrupts_Config();
    Set_USBClock();
    USB_Init();
    Resume_Init();
}
#include "public.h"
/* Private functions ---------------------------------------------------------*/
void HID_Send_Report(u8 *dat,u8 len)
{
   u8 Buff[64]={0};
   if(bDeviceState!=CONFIGURED)return;
   memcpy(Buff,dat,(size_t)(len%(sizeof(Buff)+1)));
   while(GetEPTxStatus(ENDP1) == EP_TX_VALID);                  //等待硬件置位端点为EP_TX_NAK（这一条语句很重要）  
   UserToPMABufferCopy(Buff, ENDP1_TXADDR, ReportMaxSize);      //复制进去      ReportMaxSize(报文长度)
   SetEPTxCount(ENDP1, 60);                                     //设置发送长度  ReportMaxSize(报文长度)            
   SetEPTxValid(ENDP1); 
}


//接收处理函数
uint8_t Receive_Buffer[64]={0};
#include "public.h"
#include "SPI_communicate.h"
void EP1_OUT_Callback(void)
{
//  u8 TxBuff[64] = {0};
  #if 1
  u8 DataLen = 0;    
  DataLen = GetEPRxCount(ENDP1);     //获取长度
  PMAToUserBufferCopy(Receive_Buffer, ENDP1_RXADDR, DataLen); //复制出来
  SetEPRxValid(ENDP1);               //恢复默认值地址  
  if(DataLen <64)
  {
      uint8_t Buf[70]={0};
      Buf[0] = DataLen;
      memcpy(&Buf[1],Receive_Buffer,DataLen);      
      TM_que_Write(&HID_RxQue,Buf,DataLen+1);  
  
  }

    
    
//  SPI_SendCmdPack(0x02, Receive_Buffer, DataLen);
//  HID_Send_Report(Receive_Buffer,DataLen); //收到什么数据，就发出什么数据    
//  for(u8 i=0;i<DataLen;i++){
//    que_Write(&Tx1_que,Receive_Buffer[i]); //数据放入缓存
//  }
  #endif
    

    
}
 
/********************************End of File************************************/

