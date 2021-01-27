#include "ADC.h"
#include "delay.h"

void ADC1PA0_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	  RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	  ADC_DeInit(ADC1);
	  ADC_InitStructure.ADC_Mode 					     = ADC_Mode_Independent; 			//����ģʽ
	  ADC_InitStructure.ADC_ScanConvMode       = DISABLE;     							//��ʹ�ܶ�ͨ��ģʽ
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 									//��ʹ�ܶ��ת��
	  ADC_InitStructure.ADC_ExternalTrigConv 	 = ADC_ExternalTrigConv_None; //���ⲿ��������
		ADC_InitStructure.ADC_DataAlign 				 = ADC_DataAlign_Right; 			//�����Ҷ���
	  ADC_InitStructure.ADC_NbrOfChannel 			 = 1; 												//ת��ADCͨ������Ŀ
	  ADC_Init(ADC1,&ADC_InitStructure);
	  
	  ADC_Cmd(ADC1,ENABLE);    
    ADC_ResetCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1)); //�ȴ���λУ׼
		ADC_StartCalibration(ADC1);
		while(ADC_GetCalibrationStatus(ADC1)); //�ȴ�У׼����

}

void ADC2PA1_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

	  RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	  ADC_DeInit(ADC2);
	  ADC_InitStructure.ADC_Mode 					     = ADC_Mode_Independent; 			//����ģʽ
	  ADC_InitStructure.ADC_ScanConvMode       = DISABLE;     							//��ʹ�ܶ�ͨ��ģʽ
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 									//��ʹ�ܶ��ת��
	  ADC_InitStructure.ADC_ExternalTrigConv 	 = ADC_ExternalTrigConv_None; //���ⲿ��������
		ADC_InitStructure.ADC_DataAlign 				 = ADC_DataAlign_Right; 			//�����Ҷ���
	  ADC_InitStructure.ADC_NbrOfChannel 			 = 1; 												//ת��ADCͨ������Ŀ
	  ADC_Init(ADC2,&ADC_InitStructure);
	  
	  ADC_Cmd(ADC2,ENABLE);    
    ADC_ResetCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2)); //�ȴ���λУ׼
		ADC_StartCalibration(ADC2);
		while(ADC_GetCalibrationStatus(ADC2)); //�ȴ�У׼����
}

//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc1(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,ͨ����Ŀ,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}


//���ADCֵ
//ch:ͨ��ֵ 0~3
u16 Get_Adc2(u8 ch)   
{
  	//����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,ͨ����Ŀ,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC2);	//�������һ��ADC1�������ת�����
}


//���3.10V
u16 Get_TADC1PA0Average(void)
{
	u32 temp_val=0;
	u8 t;
	static u16 ADtemp[10] = {0};
	static u8 i = 0;
	ADtemp[i]=Get_Adc1(ADC_Channel_0);
	i++;
	i%=10;
	for(t=0;t<10;t++)
	{
		temp_val+=ADtemp[t];
	}
	return temp_val/10;
}

//���3.10V
u16 Get_TADC2PA1Average(void)
{
	u32 temp_val=0;
	u8 t;
	static u16 ADtemp[10] = {0};
	static u8 i = 0;
	ADtemp[i]=Get_Adc2(ADC_Channel_1);
	i++;
	i%=10;
	for(t=0;t<10;t++)
	{
		temp_val+=ADtemp[t];
	}
	return temp_val/10;
}


//=======================================

//���3.10V
u16 Get_ADC1PA0Average(u8 AverCnt)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<AverCnt;t++)
	{
		temp_val+=Get_Adc1(ADC_Channel_0);
		delay_ms(1);
	}
	return temp_val/AverCnt;

}
//���3.10V
u16 Get_ADC2PA1Average(u8 AverCnt)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<AverCnt;t++)
	{
		temp_val+=Get_Adc2(ADC_Channel_1);
		delay_ms(1);
	}
	return temp_val/AverCnt;
}
















