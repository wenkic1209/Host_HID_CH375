/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : io_intput.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2020-11-16         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _io_intput_H
#define _io_intput_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
extern u8 HC245Dat[2] ;
#define KEY_PRESS    1
#define KEY_LOOSEN   0

//typedef struct _Keyfilter {
//    u8 defaults       : 1;           //Ĭ��״̬
//    u8 input          : 1;           //����״̬
//    u8 lastInput      : 1;           //�ϴ�״̬
//    u8 status         : 1;           //�ź�״̬
//    u8 Cnt            : 4;           //����
//} KeyStateType;

//#define getKeyStatus(X)   X.status   

//�������״̬
//extern KeyStateType dirLeft     ; //��
//extern KeyStateType dirRight    ; //��
//extern KeyStateType dirFront    ; //ǰ
//extern KeyStateType dirBack     ; //��
//��λ״̬
//extern KeyStateType SQ_Left ; //����λ
//extern KeyStateType dirRightSQ; //��
//extern KeyStateType SQ_Front    ; //ǰ��λ
//extern KeyStateType SQ_Back     ; //����λ
//extern KeyStateType SQ_Down     ; //����λ
//extern KeyStateType SQ_Up       ; //����λ

//extern KeyStateType SQ_Balance  ; //ƽ����


typedef struct _KeyType{
   u16 defaults      :1;            //Ĭ�ϵ�ƽ
   u16 input         :1;            //��������
   u16 lastInput     :1;            //������ʷ
   u16 holdClick     :1;            //������
   u16 shortPress    :1;            //�̰���־
   u16 longPress     :1;            //������־
   u16 shortLongKey  :1;            //0:�̰�����  1:��������
   u16 Cnt           :9;            //ʱ��(���511)
 }KeyType;

 #define getKeyShortFlag(X)   X.shortPress
 #define getKeyLongFlag(X)    X.longPress 
 #define clearKeyShortFlag(X) X.shortPress = 0 
 
//����
extern KeyType KeySet  ;   //����/��������
extern KeyType KeyLeft ;   //��
extern KeyType KeyRight;   //��
extern KeyType KeyFront;   //ǰ
extern KeyType KeyBack ;   //��
extern KeyType KeyHook ;   //�¹�
extern KeyType KeyGift ;   //��Ʒ��� (�����ź�)
 
 void HC245_GPIO_Init(void);
 void HC245_Scan(void);//����ɨ��
 void KeyScan(void);   //����ɨ��
 
/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

