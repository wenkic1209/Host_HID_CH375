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
//    u8 defaults       : 1;           //默认状态
//    u8 input          : 1;           //输入状态
//    u8 lastInput      : 1;           //上次状态
//    u8 status         : 1;           //信号状态
//    u8 Cnt            : 4;           //计数
//} KeyStateType;

//#define getKeyStatus(X)   X.status   

//方向控制状态
//extern KeyStateType dirLeft     ; //左
//extern KeyStateType dirRight    ; //右
//extern KeyStateType dirFront    ; //前
//extern KeyStateType dirBack     ; //后
//限位状态
//extern KeyStateType SQ_Left ; //左限位
//extern KeyStateType dirRightSQ; //右
//extern KeyStateType SQ_Front    ; //前限位
//extern KeyStateType SQ_Back     ; //后限位
//extern KeyStateType SQ_Down     ; //下限位
//extern KeyStateType SQ_Up       ; //上限位

//extern KeyStateType SQ_Balance  ; //平衡检测


typedef struct _KeyType{
   u16 defaults      :1;            //默认电平
   u16 input         :1;            //按键输入
   u16 lastInput     :1;            //按键历史
   u16 holdClick     :1;            //连按打开
   u16 shortPress    :1;            //短按标志
   u16 longPress     :1;            //长按标志
   u16 shortLongKey  :1;            //0:短按按键  1:长按按键
   u16 Cnt           :9;            //时间(最大511)
 }KeyType;

 #define getKeyShortFlag(X)   X.shortPress
 #define getKeyLongFlag(X)    X.longPress 
 #define clearKeyShortFlag(X) X.shortPress = 0 
 
//按键
extern KeyType KeySet  ;   //测试/进入设置
extern KeyType KeyLeft ;   //左
extern KeyType KeyRight;   //右
extern KeyType KeyFront;   //前
extern KeyType KeyBack ;   //后
extern KeyType KeyHook ;   //下勾
extern KeyType KeyGift ;   //礼品检测 (光眼信号)
 
 void HC245_GPIO_Init(void);
 void HC245_Scan(void);//总线扫描
 void KeyScan(void);   //按键扫描
 
/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

