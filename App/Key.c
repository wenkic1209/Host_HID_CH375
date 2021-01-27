  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : Key.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2021-01-13         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "Key.h"
#include "public.h"
 
#define KEY_PRESS    1
#define KEY_LOOSEN   0
typedef struct _Keyfilter {
    u8 defaults       : 1;           //默认状态
    u8 input          : 1;           //输入状态
    u8 lastInput      : 1;           //上次状态
    u8 status         : 1;           //信号状态
    u8 Cnt            : 4;           //计数
} KeyStateType;
 
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
    if(Key->Cnt >= 4 && Key->lastInput == !(Key->defaults)) {
        Key->status  = KEY_PRESS;   //短按事件
    }
    //松开
    if(Key->Cnt >= 4 && Key->lastInput == Key->defaults) {
        Key->status  = KEY_LOOSEN;  //清除长按
    }
}
#include "debug.h"
#include "HID_Data.h"
#include "ch375_App.h"
const u8 IR_ResultTable[]=
{
0x03, //本局黑桃   	
0x05, //本局红桃   		
0x09, //本局梅花   	
0x07, //本局方块   	
0x0D, //本局王 
0x02, //控制本局吃大陪小	
0x08, //取消所有摇控操作		
};



#define Key_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
u8 WillResult = 0;  //操作的结果
static KeyStateType Key[4] = {1,1,1,1};

void Key_defaults(void)
{
   for(u8 i=0;i<4;i++)
   {
     Key[i].defaults = 1;
   } 
}


void Key_FilterScan(void)
{
   static u8 ms = 100; 
   static u8 flag = 0;
   u8 state = 0;
   Key[0].input =PBin(9) ; //A
   Key[1].input =PBin(3) ; //B
   Key[2].input =PCin(12); //C
   Key[3].input =PCin(11); //D
   
   for(u8 i=0;i<4;i++)
   {
       oneSignalStabillzerOneKey(&Key[i]);
       if(Key[i].status == KEY_PRESS)
       {
          state |= 1<<i;
       }       
   }    
   if(ms)ms--;
   
         if(state == IR_ResultTable[0]&& ms==0 ){WillResult = 1;ms = 10; if(flag== 0){Key_Log(">本局    黑桃");flag = 1;} //本局黑桃   	
   }else if(state == IR_ResultTable[1]&& ms==0 ){WillResult = 2;ms = 10; if(flag== 0){Key_Log(">本局    红桃");flag = 1;} //本局红桃   		
   }else if(state == IR_ResultTable[2]&& ms==0 ){WillResult = 3;ms = 10; if(flag== 0){Key_Log(">本局    梅花");flag = 1;} //本局梅花   	
   }else if(state == IR_ResultTable[3]&& ms==0 ){WillResult = 4;ms = 10; if(flag== 0){Key_Log(">本局    方块");flag = 1;} //本局方块   	
   }else if(state == IR_ResultTable[4]&& ms==0 ){WillResult = 5;ms = 10; if(flag== 0){Key_Log(">本局    王  ");flag = 1;} //本局王 
   }else if(state == IR_ResultTable[5]&& ms==0 ){WillResult = 6;ms = 10; if(flag== 0){Key_Log(">本局吃大陪小");flag = 1;} //控制本局吃大陪小
   }else if(state == IR_ResultTable[6]&& ms==0 ){WillResult = 0;ms = 10; if(flag== 0){Key_Log(">摇控    取消");flag = 1;} //取消所有摇控操作  
   }else if(state == 0x04&& ms==0 ){ //补单
       if(flag== 0)
       {
		  PrintfFlag = 1; //准备打单
          GenerateWillResults();     //生成补单结果
          Key_Log("生成补单结果 ");
          flag = 1;           
       }  

   }else if(state == 0x0F&& ms==0){  //差值同步
       if(flag== 0)
       {
		   for(u8 i=0;i<40;i++)              //同步差值
		   {
				SaveTotalPoints(i,0); //默认的差值总分是0
			    PlayerDifferenceValue[i].data = ReadTotalPoints(i);
		   }
          Key_Log("差值同步 ");
          flag = 1;           
       }  
   }else if(state == 0x00 && ms==0){
		flag = 0;
   }
   

   

}



 
 
 
/********************************End of File************************************/

