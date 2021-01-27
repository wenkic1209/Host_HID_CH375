#include "ADC.h"
#include "delay.h"

void ADC1PA0_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	  RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	  ADC_DeInit(ADC1);
	  ADC_InitStructure.ADC_Mode 					     = ADC_Mode_Independent; 			//独立模式
	  ADC_InitStructure.ADC_ScanConvMode       = DISABLE;     							//不使能多通道模式
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 									//不使能多次转换
	  ADC_InitStructure.ADC_ExternalTrigConv 	 = ADC_ExternalTrigConv_None; //无外部触发启动
		ADC_InitStructure.ADC_DataAlign 				 = ADC_DataAlign_Right; 			//数据右对齐
	  ADC_InitStructure.ADC_NbrOfChannel 			 = 1; 												//转换ADC通道的数目
	  ADC_Init(ADC1,&ADC_InitStructure);
	  
	  ADC_Cmd(ADC1,ENABLE);    
    ADC_ResetCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1)); //等待复位校准
		ADC_StartCalibration(ADC1);
		while(ADC_GetCalibrationStatus(ADC1)); //等待校准结束

}

void ADC2PA1_Init(void)
{
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

	  RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1|GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	  ADC_DeInit(ADC2);
	  ADC_InitStructure.ADC_Mode 					     = ADC_Mode_Independent; 			//独立模式
	  ADC_InitStructure.ADC_ScanConvMode       = DISABLE;     							//不使能多通道模式
	  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 									//不使能多次转换
	  ADC_InitStructure.ADC_ExternalTrigConv 	 = ADC_ExternalTrigConv_None; //无外部触发启动
		ADC_InitStructure.ADC_DataAlign 				 = ADC_DataAlign_Right; 			//数据右对齐
	  ADC_InitStructure.ADC_NbrOfChannel 			 = 1; 												//转换ADC通道的数目
	  ADC_Init(ADC2,&ADC_InitStructure);
	  
	  ADC_Cmd(ADC2,ENABLE);    
    ADC_ResetCalibration(ADC2);
    while(ADC_GetCalibrationStatus(ADC2)); //等待复位校准
		ADC_StartCalibration(ADC2);
		while(ADC_GetCalibrationStatus(ADC2)); //等待校准结束
}

//获得ADC值
//ch:通道值 0~3
u16 Get_Adc1(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,通道数目,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}


//获得ADC值
//ch:通道值 0~3
u16 Get_Adc2(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC2, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,通道数目,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC2, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC2);	//返回最近一次ADC1规则组的转换结果
}


//最大3.10V
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

//最大3.10V
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

//最大3.10V
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
//最大3.10V
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
















