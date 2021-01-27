/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : CH375_App.H   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-09         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _CH375_App_H
#define _CH375_App_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "interface.h"     //底层接口函数
#include "HOST_SYS.H"      //主机操作函数

 #define User_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
 
 extern u8 PrintfFlag ;
 
//生成剩余结果
void GenerateWillResults(void);  
void CH375_APP(void);
    
/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

