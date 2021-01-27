/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : MyHID_Driver.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-12-29         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _MyHID_Driver_H
#define _MyHID_Driver_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "hw_config.h"
#include "usb_desc.h"
#include "usb_lib.h"
#include "usb_core.h"
#include "usb_pwr.h"

#include "debug.h"

#if 1
#define LCD_UsrLog(...)
#define LCD_ErrLog(...)
#define LCD_LOG_SetFooter(...)
#define LCD_LOG_SetHeader(...)
#define LCD_IstrLog(...)
#else
#define LCD_UsrLog(...)        sprint(__VA_ARGS__);sprint("\r\n")
#define LCD_IstrLog(...)      // sprint(__VA_ARGS__);sprint("\r\n")
#define LCD_ErrLog(...)        sprint(__VA_ARGS__);sprint("\r\n")
#define LCD_LOG_SetFooter(...) sprint(__VA_ARGS__);sprint("\r\n")
#define LCD_LOG_SetHeader(...) sprint(__VA_ARGS__);sprint("\r\n")
#define LCD_Log(...)           sprint(__VA_ARGS__);



//#define WKUP_CALLBACK
//#define CTR_CALLBACK
//#define RESET_CALLBACK
//#define SUSP_CALLBACK
//#define SOF_CALLBACK
//#define ESOF_CALLBACK

#endif

#define ReportMaxSize     0x40           //60=0x3C     //64=0x40         //设置报文长度
#define USBD_VID          0x5042         //0x5042 //默认0x0483
#define USBD_PID          0x0200         //0x0200 //默认0x5750
#define USBD_PVN          0x7F2B         //0x0200 //默认0x0200         
void USB_HID_Init(void);                //USB初始化
void HID_Send_Report(u8 *dat,u8 len);   //发送报文
void EP1_OUT_Callback(void);            //接收处理函数


/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

