/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : SPI_communicate.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-05         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _SPI_communicate_H
#define _SPI_communicate_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"

extern u16     SPI_Get_OutTime;  //��ȡ�����ʱ
extern u8      SPI_TxBuf[64];    //���ͻ������ݰ�

//�������ݰ��ϳ�
void SPI_SendCmdPack(uint8_t cmd, uint8_t *dat, uint16_t len);
//SPIͨѶЭ�鴦��
void SPI_Communicate(void);

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

