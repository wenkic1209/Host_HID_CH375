/**
  ******************************************************************************
  * @file    usb_desc.c
  * @author  MCD Application Team
  * @version V4.0.0
  * @date    21-January-2013
  * @brief   Descriptors for Custom HID Demo
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
#include "usb_lib.h"
#include "usb_desc.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#ifndef USBD_VID
#define USBD_VID  0x0483
#endif
#ifndef USBD_PID
#define USBD_PID  0x5750
#endif
#ifndef USBD_PVN
#define USBD_PVN  0x0200
#endif

#define LOBYTE(x)  ((uint8_t)((x) & 0x00FF))
#define HIBYTE(x)  ((uint8_t)(((x) & 0xFF00) >>8))

/* USB Standard Device Descriptor */
const uint8_t CustomHID_DeviceDescriptor[CUSTOMHID_SIZ_DEVICE_DESC] =
  {
    0x12,                       /*0-bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*1-bDescriptorType*/
    0x00,                       /*2-bcdUSB */
    0x02,                       /*3-端点数目*/
    0x00,                       /*4-bDeviceClass*/
    0x00,                       /*5-bDeviceSubClass*/
    0x00,                       /*6-bDeviceProtocol*/
    0x40,                       //7-ReportMaxSize,              /*bMaxPacketSize40*/
    LOBYTE(USBD_VID),//0x83,    /*idVendor (0x0483)*/
    HIBYTE(USBD_VID),//0x04,
    LOBYTE(USBD_PID),//0x50,    /*idProduct = 0x5750*/
    HIBYTE(USBD_PID),//0x57,
    LOBYTE(USBD_PVN),//0x00,                       /*bcdDevice rel. 2.00*/
    HIBYTE(USBD_PVN),//0x02,
    1,                          /*Index of string descriptor describing
                                              manufacturer */
    2,                          /*Index of string descriptor describing
                                             product*/
    3,                          /*Index of string descriptor describing the
                                             device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* CustomHID_DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const uint8_t CustomHID_ConfigDescriptor[CUSTOMHID_SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuration Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration 0x02*/
    CUSTOMHID_SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned 0x29=41*/
    0x00,
    0x01,         /* bNumInterfaces: 1 interface */
    0x01,         /* bConfigurationValue: Configuration value */
    0x00,         /* iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0xC0,         /* bmAttributes: Self powered */
    0x32,         /* MaxPower 100 mA: this current is used for detecting Vbus */

    /************** Descriptor of Custom HID interface ****************/
    /* 09 */
    0x09,         /* bLength: Interface Descriptor size */
    USB_INTERFACE_DESCRIPTOR_TYPE,/* bDescriptorType: Interface descriptor type 0x04*/
    0x00,         /* bInterfaceNumber: Number of Interface */
    0x00,         /* bAlternateSetting: Alternate setting */
    0x02,         /* bNumEndpoints */
    0x03,         /* bInterfaceClass: 0x03[HID] 0x02[CDCC]*/
    0x00,         /* bInterfaceSubClass : 1=BOOT, 0=no boot */
    0x00,         /* nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse */
    0,            /* iInterface: Index of string descriptor */
    /******************** Descriptor of Custom HID HID ********************/
    /* 18 */
    0x09,         /* bLength: HID Descriptor size */
    HID_DESCRIPTOR_TYPE, /* bDescriptorType: HID 0x21*/
    0x10,         /* bcdHID: HID Class Spec release number */
    0x01,
    0x00,         /* bCountryCode: Hardware target country */
    0x01,         /* bNumDescriptors: Number of HID class descriptors to follow */
    0x22,         /* bDescriptorType */
    CUSTOMHID_SIZ_REPORT_DESC,/* wItemLength: Total length of Report descriptor */
    0x00,
    /******************** Descriptor of Custom HID endpoints ******************/
    /* 27 */
    0x07,          /* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: 0x05*/

    0x81,          /* bEndpointAddress: Endpoint Address (IN)0x81(EP1-IN) 0x82(EP3-IN) */
    0x03,          /* bmAttributes: Interrupt endpoint */
    IN_BuffMaxSize  ,   /* wMaxPacketSize: 2 Bytes max */
    0x00,
    0x01,          /* bInterval: Polling Interval (32 ms) */
    /* 34 */
    	
    0x07,	/* bLength: Endpoint Descriptor size */
    USB_ENDPOINT_DESCRIPTOR_TYPE,	/* bDescriptorType: 0x05*/
			/*	Endpoint descriptor type */
    0x01,	/* bEndpointAddress: */
			/*	Endpoint Address (OUT) */
    0x03,	/* bmAttributes: Interrupt endpoint */
    OUT_BuffMaxSize,	/* wMaxPacketSize: 2 Bytes max  */
    0x00,
    0x01,	/* bInterval: Polling Interval (20 ms) */
    /* 41 */
  }
  ; /* CustomHID_ConfigDescriptor */
const uint8_t CustomHID_ReportDescriptor[CUSTOMHID_SIZ_REPORT_DESC] =
  {   
#if 0   /*162*/   
//    0x06, 0xFF, 0x00,      /* USAGE_PAGE (Vendor Page: 0xFF00) */                       
//    0x09, 0x01,            /* USAGE (Demo Kit)               */    
//    0xa1, 0x01,            /* COLLECTION (Application)       */            
//    /* 6 */
//    
//    /* Led 1 */        
//    0x85, 0x01,            /*     REPORT_ID (1)		     */
//    0x09, 0x01,            /*     USAGE (LED 1)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x01,            /*     REPORT_ID (1)              */
//    0x09, 0x01,            /*     USAGE (LED 1)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 26 */
//    
//    /* Led 2 */
//    0x85, 0x02,            /*     REPORT_ID 2		     */
//    0x09, 0x02,            /*     USAGE (LED 2)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x02,            /*     REPORT_ID (2)              */
//    0x09, 0x02,            /*     USAGE (LED 2)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 46 */
//    
//    /* Led 3 */        
//    0x85, 0x03,            /*     REPORT_ID (3)		     */
//    0x09, 0x03,            /*     USAGE (LED 3)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,             /*    FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x03,            /*     REPORT_ID (3)              */
//    0x09, 0x03,            /*     USAGE (LED 3)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 66 */
//    
//    /* Led 4 */
//    0x85, 0x04,            /*     REPORT_ID 4)		     */
//    0x09, 0x04,            /*     USAGE (LED 4)	             */
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */          
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */           
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */        
//    0x95, 0x01,            /*     REPORT_COUNT (1)           */       
//    0xB1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */     

//    0x85, 0x04,            /*     REPORT_ID (4)              */
//    0x09, 0x04,            /*     USAGE (LED 4)              */
//    0x91, 0x82,            /*     OUTPUT (Data,Var,Abs,Vol)  */
//    /* 86 */
//    
//    /* key Push Button */  
//    0x85, 0x05,            /*     REPORT_ID (5)              */
//    0x09, 0x05,            /*     USAGE (Push Button)        */      
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */      
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */      
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */  
//    0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */   
//    
//    0x09, 0x05,            /*     USAGE (Push Button)        */               
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */           
//    0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */  
//         
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */                    
//    0x85, 0x05,            /*     REPORT_ID (2)              */         
//                    
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */                      
//    /* 114 */

//    /* Tamper Push Button */  
//    0x85, 0x06,            /*     REPORT_ID (6)              */
//    0x09, 0x06,            /*     USAGE (Tamper Push Button) */      
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */      
//    0x25, 0x01,            /*     LOGICAL_MAXIMUM (1)        */      
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */  
//    0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */   
//    
//    0x09, 0x06,            /*     USAGE (Tamper Push Button) */               
//    0x75, 0x01,            /*     REPORT_SIZE (1)            */           
//    0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */  
//         
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0x81, 0x83,            /*     INPUT (Cnst,Var,Abs,Vol)   */                    
//    0x85, 0x06,            /*     REPORT_ID (6)              */         
//                    
//    0x75, 0x07,            /*     REPORT_SIZE (7)            */           
//    0xb1, 0x83,            /*     FEATURE (Cnst,Var,Abs,Vol) */  
//    /* 142 */
//    
//    /* ADC IN */
//    0x85, 0x07,            /*     REPORT_ID (7)              */         
//    0x09, 0x07,            /*     USAGE (ADC IN)             */          
//    0x15, 0x00,            /*     LOGICAL_MINIMUM (0)        */               
//    0x26, 0xff, 0x00,      /*     LOGICAL_MAXIMUM (255)      */                 
//    0x75, 0x08,            /*     REPORT_SIZE (8)            */           
//    0x81, 0x82,            /*     INPUT (Data,Var,Abs,Vol)   */                    
//    0x85, 0x07,            /*     REPORT_ID (7)              */                 
//    0x09, 0x07,            /*     USAGE (ADC in)             */                     
//    0xb1, 0x82,            /*     FEATURE (Data,Var,Abs,Vol) */                                 
//    /* 161 */

//    0xc0 	          /*     END_COLLECTION	             */
#endif
#if 0   /*33*/
   //*************************************************************** 
    //0x05:0000 01 01 这是个全局条目，用途页为ST页
    0x05, 0x8c, /* USAGE_PAGE (ST Page) */
    //0x09:0000 10 01 这是个局部变量，用途为Demo Kit
    0x09, 0x01, /* USAGE (Demo Kit) */
    //0xa1:1010 00 01 这是一个主条目，集合为应用集合
    0xa1, 0x01, /* COLLECTION (Application) */
    
    // The Input report 
    //0x09:0000 10 01 这是个局部条目，用途为厂商ID
    0x09,0x03,             // USAGE ID - Vendor defined 
    //0x15:0001 01 01 这是个全局条目，逻辑最小值为0
    0x15,0x00,             // LOGICAL_MINIMUM (0) 
    //0x26:0010 01 10 这是个全局条目，逻辑最大值为255
    0x26,0x00, 0xFF,       // LOGICAL_MAXIMUM (255) 
    //0x75:0111 01 01 这是个全局条目，报告大小为8位
    0x75,0x08,             // REPORT_SIZE (8bit)
    //0x95:1001 01 01 这是个全局条目，报告数量为64    
    0x95,0x40,             // REPORT_COUNT (64Byte) 
    //0x81:1000 00 01 这是个主条目，做输入，Data表示这些数据可变，Var表示这些徐居于是独立的变量，Abs表示绝对值
    0x81,0x02,             // INPUT (Data,Var,Abs) 

    // The Output report 
    //0x09:0000 10 01 这是个局部条目，用途为厂商ID
    0x09,0x04,             // USAGE ID - Vendor defined 
    //0x15:0001 01 01 这是个全局条目，逻辑最小值为0
    0x15,0x00,             // LOGICAL_MINIMUM (0) 
    //0x26:0010 01 10 这是个全局条目，逻辑最大值为255
    0x26,0x00,0xFF,        // LOGICAL_MAXIMUM (255)
    //0x75:0111 01 01 这是个全局条目，报告大小为8位    
    0x75,0x08,             // REPORT_SIZE (8bit) 
    //0x95:1001 01 01 这是个全局条目，报告数量为64
    0x95,0x40,             // REPORT_COUNT (64Byte) 
    //0x91:1001 00 01 这是个全局条目，做输出，Data表示这些数据可变，Var表示这些徐居于是独立的变量，Abs表示绝对值
    0x91,0x02,             // OUTPUT (Data,Var,Abs) 
    0xc0 /* END_COLLECTION */
    /* 33 */
#endif
 
    //***************************************************************
#if 1
	0x05, 0x8c, /* USAGE_PAGE (ST Page) */ 
	0x09, 0x01, /* USAGE (Demo Kit) */ 
	0xa1, 0x01, /* COLLECTION (Application) */ 
	/* 6 */ 

	// The Input report 
	0x09,0x03,            // USAGE ID - Vendor defined 
	0x15,0x00,            // LOGICAL_MINIMUM (0) 
	0x26,0x00, 0xFF,      // LOGICAL_MAXIMUM (255) 
	0x75,0x08,            // REPORT_SIZE (8) 
	0x95,IN_BuffMaxSize,  // REPORT_COUNT (22) 
	0x81,0x02,            // INPUT (Data,Var,Abs) 
	//19
	// The Output report 
	0x09,0x04,            // USAGE ID - Vendor defined 
	0x15,0x00,            // LOGICAL_MINIMUM (0) 
	0x26,0x00,0xFF,       // LOGICAL_MAXIMUM (255) 
	0x75,0x08,            // REPORT_SIZE (8) 
	0x95,OUT_BuffMaxSize, // REPORT_COUNT (22) 
	0x91,0x02,            // OUTPUT (Data,Var,Abs) 
	//32

	/* 33 */ 
	0xc0 /* END_COLLECTION */ 
#endif    
    
  }; /* CustomHID_ReportDescriptor */

/* USB String Descriptors (optional) */
const uint8_t CustomHID_StringLangID[CUSTOMHID_SIZ_STRING_LANGID] =
  {
    CUSTOMHID_SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
  }
  ; /* LangID = 0x0409: U.S. English */

const uint8_t CustomHID_StringVendor[CUSTOMHID_SIZ_STRING_VENDOR] =
  {
//    CUSTOMHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
//    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
//    /* Manufacturer: "STMicroelectronics" */
//    'S', 0, 'T', 0, 'M', 0, 'i', 0, 'c', 0, 'r', 0, 'o', 0, 'e', 0,
//    'l', 0, 'e', 0, 'c', 0, 't', 0, 'r', 0, 'o', 0, 'n', 0, 'i', 0,
//    'c', 0, 's', 0
      
    CUSTOMHID_SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
    /* Manufacturer: "Maxim" */
    'M', 0, 'a', 0, 'x', 0, 'i', 0, 'm', 0
  };

const uint8_t CustomHID_StringProduct[CUSTOMHID_SIZ_STRING_PRODUCT] =
  {
    CUSTOMHID_SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0, '3', 0, '2', 0, ' ', 0, 'C', 0,
    'u', 0, 's', 0, 't', 0, 'm', 0, ' ', 0, 'H', 0, 'I', 0,
    'D', 0
  };
uint8_t CustomHID_StringSerial[CUSTOMHID_SIZ_STRING_SERIAL] =
  {
    CUSTOMHID_SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
    'S', 0, 'T', 0, 'M', 0,'3', 0,'2', 0
  };

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

