#include "Timer.h"
#include "stm32f10x.h"
//==========================================================================
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!(10-1,7200-1)
void Timer2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //ʱ��ʹ��
    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period =
                    arr;  //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =
                    psc;  //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision =
                    TIM_CKD_DIV1;  //����ʱ�ӷָ�:TDTS = Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM���ϼ���ģʽ
    TIM_TimeBaseInit(
                    TIM2, &TIM_TimeBaseStructure);  //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  //ʹ��ָ����TIM3�ж�,��������ж�
    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;            // TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);                            //��ʼ��NVIC�Ĵ���
    TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx
}

//==========================================================================
//ͨ�ö�ʱ��3�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��   7199(���) Timer_fosc*100-1
//����ʹ�õ��Ƕ�ʱ��3
// arr:ʱ�� 100us  psc :������ 7200
//              (10-1,7200-1)
void Timer3_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_DeInit(TIM13);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  //ʱ��ʹ�� Ĭ��36MHz
    // timers=(1+Tim_Prescaler)/TiM_Clock*(1+TIM_Period)
    // timers==(1+psc)/72*(1+arr)
    // 1000000=(1+7199)/72*(1+9999)=100us*(����+1)
    // 1000=(1+3599)/36*(1+9)   1ms ------
    // 1000=(1+7299)/72*(1+999)
    //     psc = Timer_fosc*100-1;
    //    arr = timerus/((1+psc)/Timer_fosc)-1;
    //��ʱ��TIM3��ʼ��
    TIM_TimeBaseStructure.TIM_Period =
                    arr;  //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
    TIM_TimeBaseStructure.TIM_Prescaler =
                    psc;  //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision =
                    TIM_CKD_DIV1;  //����ʱ�ӷָ�:TDTS = Tck_tim 36�ֳ�72MHz
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  // TIM���ϼ���ģʽ
    TIM_TimeBaseInit(
                    TIM3, &TIM_TimeBaseStructure);  //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  //ʹ��ָ����TIM3�ж�,��������ж�
    //�ж����ȼ�NVIC����
    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;            // TIM3�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;         //�����ȼ�3��
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            // IRQͨ����ʹ��
    NVIC_Init(&NVIC_InitStructure);                            //��ʼ��NVIC�Ĵ���
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);
    TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx
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

//��ʱ��2�жϷ������
void TIM2_IRQHandler(void)  // TIM3�ж�//==1ms�ж�
{
    static u16 Cnt = 0;
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET) { //���TIM3�����жϷ������
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);  //���TIMx�����жϱ�־
        srandCnt++;
        //����
        if(Cnt == 0) {
            Cnt = 500;
            TM_GPIO_TogglePinValue(GPIOA, GPIO_Pin_8);
        } else {
            Cnt--;
        }
        ms_flag = 1;
        tm_Timer_MultiScan();           //��ʱ����׼
		#ifdef SPI_MASTER
        if(SPI_Get_OutTime) { SPI_Get_OutTime--; }
		#endif
        if(delay_msA) { delay_msA--; }
		NoteCat_OutTimeTiming();   //ͨ��è��ʱ��ʱ����
        Print_OutTimeTiming(); 
		
		
        Key_FilterScan();  //ң���źż��
        //    if(Cnt%3==0)SegScan_Timer_ms(); //�����ɨɨ��
    }  //�ж���
}

//��ʱ��3�жϷ������
void TIM3_IRQHandler(void)  // TIM3�ж�
{
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) { //���TIM3�����жϷ������
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIMx�����жϱ�־
        //����
    }  //�ж���
}
