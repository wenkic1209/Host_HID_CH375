#include "Timer.h"
#include "stm32f10x.h"
//==========================================================================
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
// arr：自动重装值。
// psc：时钟预分频数
//这里使用的是定时器3!(10-1,7200-1)
void Timer2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //时钟使能
    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period =
                    arr;  //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =
                    psc;  //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision =
                    TIM_CKD_DIV1;  //设置时钟分割:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM向上计数模式
    TIM_TimeBaseInit(
                    TIM2, &TIM_TimeBaseStructure);  //根据指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  //使能指定的TIM3中断,允许更新中断
    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;            // TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);                            //初始化NVIC寄存器
    TIM_Cmd(TIM2, ENABLE);  //使能TIMx
}

//==========================================================================
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
// arr：自动重装值。
// psc：时钟预分频数   7199(最好) Timer_fosc*100-1
//这里使用的是定时器3
// arr:时间 100us  psc :波特率 7200
//              (10-1,7200-1)
void Timer3_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_DeInit(TIM13);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //时钟使能 默认36MHz
    // timers=(1+Tim_Prescaler)/TiM_Clock*(1+TIM_Period)
    // timers==(1+psc)/72*(1+arr)
    // 1000000=(1+7199)/72*(1+9999)=100us*(计数+1)
    // 1000=(1+3599)/36*(1+9)   1ms ------
    // 1000=(1+7299)/72*(1+999)
    //     psc = Timer_fosc*100-1;
    //    arr = timerus/((1+psc)/Timer_fosc)-1;
    //定时器TIM3初始化
    TIM_TimeBaseStructure.TIM_Period =
                    arr;  //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
    TIM_TimeBaseStructure.TIM_Prescaler =
                    psc;  //设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision =
                    TIM_CKD_DIV1;  //设置时钟分割:TDTS = Tck_tim 36分成72MHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM向上计数模式
    TIM_TimeBaseInit(
                    TIM3, &TIM_TimeBaseStructure);  //根据指定的参数初始化TIMx的时间基数单位
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //使能指定的TIM3中断,允许更新中断
    //中断优先级NVIC设置
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;            // TIM3中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //先占优先级0级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //从优先级3级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQ通道被使能
    NVIC_Init(&NVIC_InitStructure);                            //初始化NVIC寄存器
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_Cmd(TIM3, ENABLE);  //使能TIMx
}

u16 srandCnt = 0;
extern u8 ms_flag ;
extern u16 delay_msA;
#include "public.h"
#include "driver.h"
#include "sys.h"
#include "SPI_communicate.h"
#include "key.h"
#include "printer.h"

//定时器2中断服务程序
void TIM2_IRQHandler(void)  // TIM3中断//==1ms中断
{
    static u16 Cnt = 0;
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) { //检查TIM3更新中断发生与否
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //清除TIMx更新中断标志
        srandCnt++;
        //函数
        if(Cnt == 0) {
            Cnt = 500;
            TM_GPIO_TogglePinValue(GPIOA, GPIO_Pin_8);
        } else {
            Cnt--;
        }
        ms_flag = 1;
        tm_Timer_MultiScan();           //定时器基准
		#ifdef SPI_MASTER
        if(SPI_Get_OutTime) { SPI_Get_OutTime--; }
		#endif
        if(delay_msA) { delay_msA--; }
		NoteCat_OutTimeTiming();   //通信猫超时计时计数
        Print_OutTimeTiming(); 
		
		
        Key_FilterScan();  //遥控信号检测
        //    if(Cnt%3==0)SegScan_Timer_ms(); //数码管扫扫描
    }  //中断中
}

//定时器3中断服务程序
void TIM3_IRQHandler(void)  // TIM3中断
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) { //检查TIM3更新中断发生与否
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //清除TIMx更新中断标志
        //函数
    }  //中断中
}
