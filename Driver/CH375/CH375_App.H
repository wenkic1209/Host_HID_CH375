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
#include "interface.h"     //�ײ�ӿں���
#include "HOST_SYS.H"      //������������

 #define User_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
 
 extern u8 PrintfFlag ;
 
//����ʣ����
void GenerateWillResults(void);  
void CH375_APP(void);
    
/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/
