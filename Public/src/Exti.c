#include "exti.h"
#include "usart.h"

void ExtiPA12_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    //GPIO 配置
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //外部中断 配置
    EXTI_InitStructure.EXTI_Line    = EXTI_Line12;         //外部中断线
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //中断状态
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt; //选择中断
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
    EXTI_Init(&EXTI_InitStructure);
	  //中断优先级配置
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
	  NVIC_Init(&NVIC_InitStructure);
		//把相应IO连接到外部中断线
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource12);
}

//============================================================================
//            外部中断
//============================================================================
const uint8_t EXIT_Channel[16]={EXTI0_IRQn    ,EXTI1_IRQn    ,EXTI2_IRQn    ,EXTI3_IRQn     ,EXTI4_IRQn    ,  
                                EXTI9_5_IRQn  ,EXTI9_5_IRQn , EXTI9_5_IRQn , EXTI9_5_IRQn  , EXTI9_5_IRQn ,  
                                EXTI15_10_IRQn,EXTI15_10_IRQn,EXTI15_10_IRQn, EXTI15_10_IRQn, EXTI15_10_IRQn,   
                                EXTI15_10_IRQn };
#include "tm_library.h"
//struct ExitGPIO{
//   GPIO_TypeDef * GPIO;
//   uint16_t       Pin ;  
//};
                                
//外部中断管脚初始化========/*管脚信息          ,抢占优先级                , 响应优先级*/
void ExitGPIO_Init(struct ExitGPIO GPIO,uint8_t PreemptionPriority, uint8_t SubPriority)
{
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TM_GPIO_Init(GPIO.GPIO  ,GPIO.Pin  ,GPIO_Mode_IPU,GPIO_Speed_50MHz); //输出
    //外部中断 配置
    EXTI_InitStructure.EXTI_Line    = GPIO.Pin  ;         //外部中断线
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;              //中断状态
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt; //选择中断
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿触发
    EXTI_Init(&EXTI_InitStructure);
	//中断优先级配置
    uint8_t pinsource = 0;
	while (GPIO.Pin > 1) {   
     if(GPIO.Pin&0x01)
     {
        NVIC_InitStructure.NVIC_IRQChannel    = EXIT_Channel[pinsource];//通过管脚查询中断号列表
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = PreemptionPriority  ;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority        = SubPriority;
        NVIC_Init(&NVIC_InitStructure);
        //把相应IO连接到外部中断线
        uint8_t GPIO_Port = TM_GPIO_GetPortSource(GPIO.GPIO  ); //获取到端口位置
        GPIO_EXTILineConfig(GPIO_Port,pinsource);     
     }          
        GPIO.Pin >>= 1; 
        pinsource++;          
//	    GPIO_EXTILineConfig(GPIO_Port,GPIO_PinSource12);        
	}

}

//==============================================================
//               中断函数
//==============================================================
//u16 MoterSpeedCnt = 0;
//void EXTI15_10_IRQHandler(void)
//{
//   if(EXTI_GetITStatus(EXTI_Line12) != RESET)
//	 {
//	    EXTI_ClearITPendingBit(EXTI_Line12); // 清除中断
//	    MoterSpeedCnt ++;
////		   COM_Printf(6,"%d\r\n",MoterSpeedCnt);
//	 }

//}













