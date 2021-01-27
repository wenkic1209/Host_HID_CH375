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
    u8 defaults       : 1;           //Ĭ��״̬
    u8 input          : 1;           //����״̬
    u8 lastInput      : 1;           //�ϴ�״̬
    u8 status         : 1;           //�ź�״̬
    u8 Cnt            : 4;           //����
} KeyStateType;
 
//* @Name   signalStabillzerOneKey
//* @brief  ��һ�ź��ȶ���
//* @param  KeyStateType *Key
//* @retval None
static void oneSignalStabillzerOneKey(KeyStateType *Key)
{
    //״̬
    if(Key->lastInput != Key->input) {
        Key->lastInput = Key->input;
        Key->Cnt = 0;
    } else {
        if(Key->Cnt < 16) {
            Key->Cnt++;
        }
    }
    //����
    if(Key->Cnt >= 4 && Key->lastInput == !(Key->defaults)) {
        Key->status  = KEY_PRESS;   //�̰��¼�
    }
    //�ɿ�
    if(Key->Cnt >= 4 && Key->lastInput == Key->defaults) {
        Key->status  = KEY_LOOSEN;  //�������
    }
}
#include "debug.h"
#include "HID_Data.h"
#include "ch375_App.h"
const u8 IR_ResultTable[]=
{
0x03, //���ֺ���   	
0x05, //���ֺ���   		
0x09, //����÷��   	
0x07, //���ַ���   	
0x0D, //������ 
0x02, //���Ʊ��ֳԴ���С	
0x08, //ȡ������ҡ�ز���		
};



#define Key_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
u8 WillResult = 0;  //�����Ľ��
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
   
         if(state == IR_ResultTable[0]&& ms==0 ){WillResult = 1;ms = 10; if(flag== 0){Key_Log(">����    ����");flag = 1;} //���ֺ���   	
   }else if(state == IR_ResultTable[1]&& ms==0 ){WillResult = 2;ms = 10; if(flag== 0){Key_Log(">����    ����");flag = 1;} //���ֺ���   		
   }else if(state == IR_ResultTable[2]&& ms==0 ){WillResult = 3;ms = 10; if(flag== 0){Key_Log(">����    ÷��");flag = 1;} //����÷��   	
   }else if(state == IR_ResultTable[3]&& ms==0 ){WillResult = 4;ms = 10; if(flag== 0){Key_Log(">����    ����");flag = 1;} //���ַ���   	
   }else if(state == IR_ResultTable[4]&& ms==0 ){WillResult = 5;ms = 10; if(flag== 0){Key_Log(">����    ��  ");flag = 1;} //������ 
   }else if(state == IR_ResultTable[5]&& ms==0 ){WillResult = 6;ms = 10; if(flag== 0){Key_Log(">���ֳԴ���С");flag = 1;} //���Ʊ��ֳԴ���С
   }else if(state == IR_ResultTable[6]&& ms==0 ){WillResult = 0;ms = 10; if(flag== 0){Key_Log(">ҡ��    ȡ��");flag = 1;} //ȡ������ҡ�ز���  
   }else if(state == 0x04&& ms==0 ){ //����
       if(flag== 0)
       {
		  PrintfFlag = 1; //׼����
          GenerateWillResults();     //���ɲ������
          Key_Log("���ɲ������ ");
          flag = 1;           
       }  

   }else if(state == 0x0F&& ms==0){  //��ֵͬ��
       if(flag== 0)
       {
		   for(u8 i=0;i<40;i++)              //ͬ����ֵ
		   {
				SaveTotalPoints(i,0); //Ĭ�ϵĲ�ֵ�ܷ���0
			    PlayerDifferenceValue[i].data = ReadTotalPoints(i);
		   }
          Key_Log("��ֵͬ�� ");
          flag = 1;           
       }  
   }else if(state == 0x00 && ms==0){
		flag = 0;
   }
   

   

}



 
 
 
/********************************End of File************************************/

