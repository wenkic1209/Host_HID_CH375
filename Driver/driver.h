/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : driver.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-11-09         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _driver_H
#define _driver_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stdlib.h"
#include "segled.h"      //数码管控制
#include "io_intput.h"   //信号输入处理
#include "HID_Data.h"    //数据处理
#include "notecat.h"

#include ".\SalveSPI\SalveSPI.h"  //从机SPI
#include "./MasterSPI/MasterSPI.h"//主机SPI

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

