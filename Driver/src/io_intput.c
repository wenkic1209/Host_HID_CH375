  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : io_intput.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-11-16         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "io_intput.h"
#include "tm_library.h"

/*
  HC245输入总线 PA15 PD2  PB4 PA1 PC3 PC1 PC13 PB11
  PC6:使能端
  D1->前
  D2->后1
  D3->左
  D4->右
  D5->下沟
  D6->光眼信号
  D7->投币1
  D8->空--暂时无

  PB1:使能端  
  D1->平衡
  D2->测试/进入设置
  D3->后停SW
  D4->前停SW
  D5->左停SW
  D6->上停SW
  D7->下勾停SW
  D8->空--暂时无

*/

static TM_GPIO HC245Enable[2]=
{
    {GPIOC,GPIO_Pin_6},
    {GPIOB,GPIO_Pin_1},
};
u8 HC245Dat[2] = {0};
/**
  * @Name    HC245_GPIO_Init
  * @brief   HC245输入总线管脚初始化
  * @param   None
  * @retval  None
  * @Data    None
 **/
void HC245_GPIO_Init(void)
{
   TM_GPIO_SetPinValue(GPIOC,GPIO_Pin_6 ,1); 
   TM_GPIO_SetPinValue(GPIOB,GPIO_Pin_1 ,1);
   //总线输入
   TM_GPIO_Init(GPIOA,GPIO_Pin_15,GPIO_Mode_IPU,GPIO_Speed_50MHz);
   TM_GPIO_Init(GPIOD,GPIO_Pin_2 ,GPIO_Mode_IPU,GPIO_Speed_50MHz);
   TM_GPIO_Init(GPIOB,GPIO_Pin_4 ,GPIO_Mode_IPU,GPIO_Speed_50MHz);
   TM_GPIO_Init(GPIOA,GPIO_Pin_1 ,GPIO_Mode_IPU,GPIO_Speed_50MHz);
   TM_GPIO_Init(GPIOC,GPIO_Pin_3 ,GPIO_Mode_IPU,GPIO_Speed_50MHz);
   TM_GPIO_Init(GPIOC,GPIO_Pin_1 ,GPIO_Mode_IPU,GPIO_Speed_50MHz); 
   TM_GPIO_Init(GPIOC,GPIO_Pin_13,GPIO_Mode_IPU,GPIO_Speed_50MHz);
   TM_GPIO_Init(GPIOB,GPIO_Pin_11,GPIO_Mode_IPU,GPIO_Speed_50MHz);
   //使能端
   TM_GPIO_Init(GPIOC,GPIO_Pin_6 ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
   TM_GPIO_Init(GPIOB,GPIO_Pin_1 ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
}

static u8 ReadHC245Bus(void)
{
    u8 temp = 0;
    temp |= TM_GPIO_GetInputPinValue(GPIOA,GPIO_Pin_15)<<0;
    temp |= TM_GPIO_GetInputPinValue(GPIOD,GPIO_Pin_2 )<<1;
    temp |= TM_GPIO_GetInputPinValue(GPIOB,GPIO_Pin_4 )<<2;
    temp |= TM_GPIO_GetInputPinValue(GPIOA,GPIO_Pin_1 )<<3;
    temp |= TM_GPIO_GetInputPinValue(GPIOC,GPIO_Pin_3 )<<4;
    temp |= TM_GPIO_GetInputPinValue(GPIOC,GPIO_Pin_1 )<<5;
    temp |= TM_GPIO_GetInputPinValue(GPIOC,GPIO_Pin_13)<<6;
    temp |= TM_GPIO_GetInputPinValue(GPIOB,GPIO_Pin_11)<<7;
    return temp;
}
static u8 ReadHC245Bus2(void)
{
    u8 temp = 0;
    temp |= TM_GPIO_GetInputPinValue(GPIOA,GPIO_Pin_15)<<7;
    temp |= TM_GPIO_GetInputPinValue(GPIOD,GPIO_Pin_2 )<<6;
    temp |= TM_GPIO_GetInputPinValue(GPIOB,GPIO_Pin_4 )<<5;
    temp |= TM_GPIO_GetInputPinValue(GPIOA,GPIO_Pin_1 )<<4;
    temp |= TM_GPIO_GetInputPinValue(GPIOC,GPIO_Pin_3 )<<3;
    temp |= TM_GPIO_GetInputPinValue(GPIOC,GPIO_Pin_1 )<<2;
    temp |= TM_GPIO_GetInputPinValue(GPIOC,GPIO_Pin_13)<<1;
    temp |= TM_GPIO_GetInputPinValue(GPIOB,GPIO_Pin_11)<<0;
    return temp;
}


//HC245总线输入扫描
void HC245_Scan(void)
{
   TM_GPIO_SetPinValue(HC245Enable[0].GPIO,HC245Enable[0].Pin,0);    
   HC245Dat[0] = ReadHC245Bus2();
   TM_GPIO_SetPinValue(HC245Enable[0].GPIO,HC245Enable[0].Pin,1);     
   TM_GPIO_SetPinValue(HC245Enable[1].GPIO,HC245Enable[1].Pin,0);    
   HC245Dat[1] = ReadHC245Bus();
   TM_GPIO_SetPinValue(HC245Enable[1].GPIO,HC245Enable[1].Pin,1);     
}
//* @Name   signalStabillzerOneKey
//* @brief  单一信号稳定器
//* @param  KeyStateType *Key
//* @retval None
static void oneSignalStabillzerOneKey(KeyStateType *Key)
{
    //状态
    if(Key->lastInput != Key->input) {
        Key->lastInput = Key->input;
        Key->Cnt = 0;
    } else {
        if(Key->Cnt < 16) {
            Key->Cnt++;
        }
    }
    //按下
    if(Key->Cnt >= 2 && Key->lastInput == !(Key->defaults)) {
        Key->status  = KEY_PRESS; //短按事件
    }
    //松开
    if(Key->Cnt >= 3 && Key->lastInput == Key->defaults) {
        Key->status  = KEY_LOOSEN;  //清除长按
    }
}
//* @Name   signalStabillzerOneKey
//* @brief  单一信号按键扫描
//* @param  KeyType *Key 按键数据结构
//* @retval None
static void OneSingleKeyScanning(KeyType *Key)
{
    //状态
    if(Key->lastInput != Key->input) {
        Key->lastInput = Key->input;
        Key->Cnt = 0;
    } else {
        if(Key->Cnt < 510) {
            Key->Cnt++;
        }
        //连续按下
        if((Key->shortLongKey==0) && Key->holdClick && (Key->Cnt > 80) && Key->lastInput == !(Key->defaults)) {
            if((Key->Cnt - 80) % 5 == 0) {     //40*5 = 200 // 20*5=100
                Key->shortPress = 1; //短按事件
                Key->Cnt = 80;
            }
        }
    }
    //初次短按事件
    if((Key->shortLongKey==0) && Key->Cnt == 5 && Key->lastInput == !(Key->defaults)) {
        Key->shortPress = 1; //短按事件
    }
    //初始长按事件 
    if ((Key->shortLongKey==1)&& Key->Cnt == 100 && Key->lastInput == !(Key->defaults))
    {
        Key->longPress = 1;  //长按事件
    }
    //初次按键松开 30ms (超时未处理)
    if(Key->Cnt == 10 && Key->lastInput == (Key->defaults)) {
        Key->longPress  = 0;  //清除长按
        Key->shortPress = 0;  //清除短按
    }
}
//方向控制状态
KeyStateType dirLeft = {1}; //左
KeyStateType dirRight= {1}; //右
KeyStateType dirFront= {1}; //前
KeyStateType dirBack = {1}; //后
//限位状态
KeyStateType SQ_Left = {1}; //左限位
//KeyStateType dirRightSQ= {0}; //右
KeyStateType SQ_Front= {1}; //前限位
KeyStateType SQ_Back = {1}; //后限位
KeyStateType SQ_Down = {1}; //下限位
KeyStateType SQ_Up   = {1}; //上限位

KeyStateType SQ_Balance   = {1}; //平衡检测

static KeyStateType * StateTable[10]={
                              &dirLeft     ,//左
                              &dirRight    ,//右    
                              &dirFront    ,//前    
                              &dirBack     ,//后    
                              &SQ_Left     ,//左限位    
                              &SQ_Front    ,//前限位
                              &SQ_Back     ,//后限位
                              &SQ_Down     ,//下限位
                              &SQ_Up       ,//上限位
                              &SQ_Balance  , //平衡检测
};


//按键
KeyType KeySet  = {0};   //测试/进入设置
KeyType KeyLeft = {0};   //左
KeyType KeyRight= {0};   //右
KeyType KeyFront= {0};   //前
KeyType KeyBack = {0};   //后
KeyType KeyHook = {0};   //下勾

KeyType KeyGift = {0};   //礼品检测 (光眼信号)

KeyType * KeyTable[7]={
                 &KeySet  ,   //测试/进入设置
                 &KeyLeft ,   //左
                 &KeyRight,   //右
                 &KeyFront,   //前
                 &KeyBack ,   //后
                 &KeyHook ,   //下勾
                 &KeyGift ,   //礼品检测 (光眼信号)
};


//* @Name     KeyScan1
//* @brief    按键扫描1
//* @param    None
//* @retval   None
static void KeyScan1(void)
{

    dirLeft .input = HC245Dat[0]>>3&0x01; //左
    dirRight.input = HC245Dat[0]>>2&0x01; //右
    dirFront.input = HC245Dat[0]>>0&0x01; //前
    dirBack .input = HC245Dat[0]>>1&0x01; //后
    
    SQ_Left .input = HC245Dat[1]>>4&0x01; //左限位
    SQ_Front.input = HC245Dat[1]>>3&0x01; //前限位
    SQ_Back .input = HC245Dat[1]>>2&0x01; //后限位
    SQ_Down .input = HC245Dat[1]>>6&0x01; //下限位
    SQ_Up   .input = HC245Dat[1]>>5&0x01; //上限位
    
    SQ_Balance.input = HC245Dat[1]>>0&0x01; //平衡检测
    //状态扫描
    for(u8 i=0;i<sizeof(StateTable)/sizeof(KeyStateType*);i++)
    {
       oneSignalStabillzerOneKey(StateTable[i]);
    }
}
//* @Name     KeyScan2
//* @brief    按键扫描2
//* @param    None
//* @retval   None
static void KeyScan2(void)
{
    KeySet  .input = HC245Dat[1]>>1&0x01;  //测试/进入设置
    KeyLeft .input = HC245Dat[0]>>3&0x01;  //左    
    KeyRight.input = HC245Dat[0]>>2&0x01;  //右    
    KeyFront.input = HC245Dat[0]>>0&0x01;  //前    
    KeyBack .input = HC245Dat[0]>>1&0x01;  //后    
    KeyHook .input = HC245Dat[0]>>4&0x01;  //下勾
    KeyGift .input = HC245Dat[0]>>5&0x01;  //礼品检测 (光眼信号)
    //按键扫描
    for(u8 i=0;i<sizeof(KeyTable)/sizeof(KeyType*);i++)
    {
       OneSingleKeyScanning(KeyTable[i]);
    }    
}
//* @Name     KeyScan
//* @brief    按键扫描
//* @param    None
//* @retval   None
void KeyScan(void)
{
    static u8 filp = 0;
    filp = !filp;
    if(filp == 0){KeyScan1();} //状态获取
    if(filp == 1){KeyScan2();} //按键扫描
}

 
/********************************End of File************************************/

