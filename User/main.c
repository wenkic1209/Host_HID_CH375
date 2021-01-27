/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "string.h"
#include "delay.h"
#include "usart.h"
#include "debug.h"
//----------------
#include "public.h"
#include "driver.h"
//----------------
#include <cm_backtrace.h>  //调试输出
//----------------
//HID
#include "MyHID_Driver.h"
#include "SPI_Communicate.h"
#include "CH375INC.h"
#include "CH375_App.h"
#include "Key.h"
//#include "CH375.h"
//#include "CH375_Device.h"
//----------------
#define HARDWARE_VERSION               "V1.0.0"
#define SOFTWARE_VERSION               "V0.1.0"
/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

u16 TempSeg[2] = {0x1E0, (~0x1E0) & 0x1FF};

TM_Timer debug_ms ;
__IO uint8_t PrevXferComplete = 1;

extern u8 ResumeSeState;
u8 *idStr[] = {
    (u8 *)"UNCONNECTED",
    (u8 *)"ATTACHED",
    (u8 *)"POWERED",
    (u8 *)"SUSPENDED",
    (u8 *)"ADDRESSED",
    (u8 *)"CONFIGURED"
};
u8 *sdStr[RESUME_ESOF + 1] = {
    (u8 *)"RESUME_EXTERNAL",
    (u8 *)"RESUME_INTERNAL",
    (u8 *)"RESUME_LATER",
    (u8 *)"RESUME_WAIT",
    (u8 *)"RESUME_START",
    (u8 *)"RESUME_ON",
    (u8 *)"RESUME_OFF",   //断开连接
    (u8 *)"RESUME_ESOF"
};

#define SPI_COMMUNICATE_OFF  0

u8 ms_flag    = 0;
u16 delay_msA = 0;




int main(void)
{
    RCC_ClocksTypeDef Clock;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    delay_init();
    queue_Init();
    TM_que_AllRegistInit(); //左右队列初始化
    Key_defaults();
    /* CmBacktrace initialize 错误代码自动追踪、定位，错误原因自动分析初始化*/
    cm_backtrace_init("CH375_F103Host转发", HARDWARE_VERSION, SOFTWARE_VERSION);  //错误调试跟踪
    //addr2line -e demo.axf -a -f 08003442 08003534
    /*(输出错误现场的 函数调用栈（需配合 addr2line 工具进行精确定位），还原发生错误时的现场信息，定位问题代码位置、逻辑更加快捷、精准)*/
    Usart_Init(COM1, 256000, 3, 3); //115200 调试端口
	Usart_Init(COM2, 9600  , 3, 3); //9600   打印机
	Usart_Init(COM3, 115200, 3, 3); //115200 通信猫
    TM_GPIO_Init(GPIOA, GPIO_Pin_8, GPIO_Mode_Out_PP, GPIO_Speed_50MHz); //RUN LED
    TM_GPIO_Init(GPIOC, GPIO_Pin_11, GPIO_Mode_IPU, GPIO_Speed_50MHz); //遥控按键A  PC11
    TM_GPIO_Init(GPIOC, GPIO_Pin_12, GPIO_Mode_IPU, GPIO_Speed_50MHz); //遥控按键B  PC12
    TM_GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_IPU, GPIO_Speed_50MHz);  //遥控按键C  PB3
    TM_GPIO_Init(GPIOB, GPIO_Pin_8, GPIO_Mode_IPU, GPIO_Speed_50MHz);  //遥控按键D  PB9
    USB_HID_Init();                 //USB初始化
    PCout(9) = 1;
	

    fm25CL64_gpio_Init();
	if(FM25CL64B_Text(0,0xAA)==0xAA)
	{
		Usart_SendString(COM1,(u8*)"铁电运行OK\r\n",sizeof("铁电运行正常\r\n"));
	}else{
	    Usart_SendString(COM1,(u8*)"铁电运行Err\r\n",sizeof("铁电运行正常\r\n"));
	}
	checkFirstFlag();	      //第一次初始设置
    StartOpenGSM_Init();      //开机初始化-->获取管理员电话
	
//	User_Log("long =>%d",sizeof(long long));
//	User_Log("int64_t =>%d",sizeof(int64_t));	
//	User_Log("int64_C =>%d",sizeof(int64_c));		
    #if SPI_COMMUNICATE_OFF
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    SPI_MasterCommunication_Init(); //主机通讯初始化
    #endif
    RCC_GetClocksFreq(&Clock);
    Timer2_Init(10 - 1, 7200 - 1);
    tm_timer_init(&debug_ms, 100, 500);
    tm_timer_start(&debug_ms);
    pr_debug("SYSCLK_Frequency  %ld", Clock.SYSCLK_Frequency);
    pr_debug("HCLK_Frequency    %ld", Clock.HCLK_Frequency);
    pr_debug("PCLK1_Frequency   %ld", Clock.PCLK1_Frequency);
    pr_debug("PCLK2_Frequency   %ld", Clock.PCLK2_Frequency);
    pr_debug("ADCCLK_Frequency  %ld", Clock.ADCCLK_Frequency);

    while(1) {
        CH375_APP();
    }
	
	
	while(1)
	{
	    //测试 打印机透传
        if(que_size(Rx1_que)) { //串口数据
            Usart_SendByte(COM2, que_Read(&Rx1_que));
			
        }
        if(que_size(Rx2_que)) { //串口数据
            Usart_SendByte(COM1, que_Read(&Rx2_que));
        }	
//		//测试 通信猫透传
//		if(que_size(Rx1_que)) { //串口数据
//            Usart_SendByte(COM3, que_Read(&Rx1_que));
//        }
//	    if(que_size(Rx3_que)) { //串口数据
//            Usart_SendByte(COM1, que_Read(&Rx3_que));
//        }		
	}
	
    #if 0
    u16 X = 0;
    while(1) {
        //        static u8 id = 0xFE;
        //        if(id!=bDeviceState)
        //        {
        //            id = bDeviceState;
        //            LCD_UsrLog("Drivce = %s",idStr[id]);
        //        }
        #if SPI_COMMUNICATE_OFF
        SPI_Communicate();     //SPI通讯协议处理
        //HID Tx --> SPI1_Tx   //HID数据转发
        if(que_size(MasterSPI_Tque)) { //串口数据
            uint16_t dat =  que_Read(&MasterSPI_Tque);
            MasterSPI_SendByte(dat | 0x0100);
        } else {
            MasterSPI_SendByte(0x00FF);
        }
        #endif
        //HID_Rx ---> Usart1_TX
        if(que_size(Tx1_que)) { //串口数据
            Usart_SendByte(COM1, que_Read(&Tx1_que));
        }
        //       if(que_size(Rx1_que)) //串口数据
        //       {
        //          u8 tdat = que_Read(&Rx1_que);
        //          que_Write(&MasterSPI_Tque,tdat);
        //       }
        //        static u8 sd = 0xFE;
        //        if(sd!=ResumeSeState && ResumeSeState <=RESUME_ESOF)  //拔掉 ResumeSeState == 0  默认等6
        //        {
        //            sd = ResumeSeState;
        //            LCD_UsrLog("<%d>ResumeS = %s",ResumeSeState,sdStr[sd]);
        //        }
        //        static u8 ad = 0xFE;
        //        if(ad!=fSuspendEnabled)
        //        {
        //            ad = fSuspendEnabled;
        //            LCD_UsrLog("<%d>fSuspendEnabled ",fSuspendEnabled);
        //        }
        //        if(X != bDeviceState)
        //        {
        //            X= bDeviceState;
        //            pr_debug("bDeviceState <%d>",bDeviceState);
        //        }
        //           //USART1_RX --> SPI1_Tx
        //           if(que_size(Rx1_que)) //串口数据
        //           {
        //              uint16_t dat =  que_Read(&Rx1_que);
        //              MasterSPI_SendByte(dat|0x0100);
        //           }else{
        //              MasterSPI_SendByte(0x00FF);
        //           }
        //           //SPI1_Rx -->USART1_TX
        //           if(que_size(MasterSPI_Rque)) //接收的数据
        //           {
        //              uint16_t dat =  que_Read(&MasterSPI_Rque);
        //              Usart_SendByte(COM1,dat&0xFF);
        //           }
        //        if(que_size(MasterSPI_Rque)) //接收的数据
        //        {
        //           uint16_t dat =  que_Read(&MasterSPI_Rque);
        //           Usart_SendByte(COM1,dat&0xFF);
        //        }
        //Usart1_RX ---> Usart1_TX
        //       if(que_size(Rx1_que)) //串口数据
        //       {
        //          u8 tdat = que_Read(&Rx1_que);
        //          Usart_SendByte(COM1,tdat);
        //          if(tdat==1)  {
        //          u8 buff[64]={0};
        //              for(u8 i=0;i<64;i++)
        //              {
        //                 buff[i]= (i/10)<<4|(i%10);
        //              }
        //              HID_Send_Report(buff,64);
        //          }
        //       }
        #if 1
        if(tm_timer_CheckFlag(debug_ms)) {
            tm_timer_ClearFlag(&debug_ms);
            //          TM_GPIO_TogglePinValue(GPIOA,GPIO_Pin_8);
            //          Bsp_CH375_Get_DeviceVersion();
            //          TM_GPIO_TogglePinValue(GPIOC,0x0F);
            //          u8 buff[64]={0};
            //          for(u8 i=0;i<64;i++)
            //          {
            //             buff[i]= (i/10)<<4|(i%10);
            //          }
            //          HID_Send_Report(buff,64);
            //          TM_GPIO_TogglePinValue(GPIOC,GPIO_Pin_2);
            //          Usart_SendByte(COM1,0xAA);
            //            pr_debug("输出测试 %d",X++);
            //          tm_timer_init(&debug_ms,300,0);
            //          tm_timer_start(&debug_ms);
        }
        #endif
    }
    #endif
}



#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* Infinite loop */
    while(1) {
    }
}
#endif


/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
