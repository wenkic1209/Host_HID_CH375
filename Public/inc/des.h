/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : des.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-10         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _des_H
#define _des_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#include "stm32f10x.h"

enum{
DES_Encrypt, /*加密*/
DES_Decrypt  /*解密*/
};

/*
  des加密解密函数：
   参数：plain_strng 原数据(需要加密/解密数据8字节)
         key         密钥
         d           控制（0:加密 1:解密）
         ciph_strng  输出数据(加密/解密后的数据)
*/
void des(uint8_t *plain_strng, uint8_t *key, uint8_t d, uint8_t *ciph_strng);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

