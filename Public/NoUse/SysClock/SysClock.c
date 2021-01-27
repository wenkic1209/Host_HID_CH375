#include "stm32f10x.h"
#include "SysClock.h"

//系统时钟初始化配置
 //如果自己配置为内部高速时钟源
 //需要覆盖以下原本变量  
// u32 SystemCoreClock = 64000000;  //系统时钟频率为64MHz 
//void Check_SysClock(void)
//{
//	RCC_ClocksTypeDef SysClock;	 
//	RCC_GetClocksFreq(&SysClock);
//	printf("SysCLK = %ld \r\n",(u32)SysClock.SYSCLK_Frequency);
//	printf("HclCLK = %ld \r\n",(u32)SysClock.HCLK_Frequency);
//	printf("PCLK1  = %ld \r\n",(u32)SysClock.PCLK1_Frequency);
//	printf("PCLK2  = %ld \r\n",(u32)SysClock.PCLK2_Frequency);
//	printf("ADCCLK = %ld \r\n",(u32)SysClock.ADCCLK_Frequency); 

//}

 //自己配置的
 void RCC_ConfigInit()
 {
	ErrorStatus HSEStartUPStatus;    //定义错误枚举状态变量
	 
	#if 0
	Check_SysClock();  //打印系统各个时钟的大小 (串口一打印)	 
	#endif
	 
	RCC_DeInit();                    //将RCC初始化,重新设置为默认值
	
  if(1) //外部高速时钟源HSE 72MHz(最大速度)
	{
		 RCC_HSEConfig(RCC_HSE_ON);
		 //打开外部高速时钟晶振,使能HSE
		 //RCC_HSE_ON 开
		 //RCC_HSE_OFF 关
		 //RCC_HSE_BYPASS HSE晶振被外部时钟旁路
		 
		 HSEStartUPStatus = RCC_WaitForHSEStartUp(); //等待HSE起振并稳定
		 
		 if(HSEStartUPStatus == SUCCESS) //判断HSE是否起振成功,是则进入if()内部
		 {
			RCC_HCLKConfig(RCC_SYSCLK_Div1);         //AHB时钟(HCLK) = 系统时钟
			RCC_PCLK1Config(RCC_HCLK_Div2);          //设置低俗AHB时钟(APB1总线)  == HCLK/2
			RCC_PCLK2Config(RCC_HCLK_Div1);		     //设置 高速AHB时钟(APB2总线) == HCLK
			FLASH_SetLatency(FLASH_Latency_2);		 //设置FLASH延时周期为2
			FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);//使能Flash预取缓存
			
						//设置时钟源分频数       倍频系数
			RCC_PLLConfig(RCC_PLLSource_HSE_Div1,RCC_PLLMul_9);//
			/*
				RCC_PLLSource_HSE_Div1  外部高速时钟1分频   == 8MHz * 倍频系数 (最大72MHz)->  8*9  =72MHz
				RCC_PLLSource_HSE_Div2  外部高速时钟2分频   == 4MHz * 倍频系数 (最大64MHz)->  4*16 =64MHz
				RCC_PLLSource_HSI_Div2  内部部高速时钟2分频 == 4MHz * 倍频系数 (最大64MHz)->  4*16 =64MHz 
			 */
			 RCC_PLLCmd(ENABLE);    //PLL倍频器使能
			 
			 while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET ); //等待PLL设置就绪

			 RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //设置PLL时钟源为系统时钟
				
			 while(RCC_GetSYSCLKSource() != 0x08); //判断PLL是否为系统时钟源
			 /*
				0x00  :HSI 为时钟源
				0x04  :HSE 为时钟源
				0x08  :PLL 为时钟源	
			 */
		 }	
	}
  if(0) //内部高速时钟源HSI 64MHz(最大速度)
	{
		RCC_HSICmd(ENABLE);  //内部高速时钟晶振使能
		while(RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);  //等待HSI设置就绪
		
		RCC_HCLKConfig(RCC_SYSCLK_Div1);         //AHB时钟(HCLK) = 系统时钟
		RCC_PCLK1Config(RCC_HCLK_Div2);          //设置低俗AHB时钟(APB1总线)  == HCLK/2
		RCC_PCLK2Config(RCC_HCLK_Div1);		     //设置 高速AHB时钟(APB2总线) == HCLK
		FLASH_SetLatency(FLASH_Latency_2);		 //设置FLASH延时周期为2
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable); //使能Flash预取缓存
		
					//设置时钟源分频数       倍频系数
		RCC_PLLConfig(RCC_PLLSource_HSI_Div2,RCC_PLLMul_16);//
		/*            内部部高速时钟2分频     
			RCC_PLLSource_HSE_Div1  外部高速时钟1分频   == 8MHz * 倍频系数 (最大72MHz)->  8*9  =72MHz
			RCC_PLLSource_HSE_Div2  外部高速时钟2分频   == 4MHz * 倍频系数 (最大64MHz)->  4*16 =64MHz
			RCC_PLLSource_HSI_Div2  内部部高速时钟2分频 == 4MHz * 倍频系数 (最大64MHz)->  4*16 =64MHz
		*/
		RCC_PLLCmd(ENABLE);    //PLL倍频器使能
		
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET ); //等待PLL设置就绪
    
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK); //设置PLL时钟源为系统时钟
		 
		while(RCC_GetSYSCLKSource() != 0x08); //判断PLL是否为系统时钟源
		/*
		 	0x00  :HSI 为时钟源
		 	0x04  :HSE 为时钟源
		 	0x08  :PLL 为时钟源	
		*/
	}		
}
 


//官方配置
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
GPIO_InitTypeDef GPIO_InitStructure;
RCC_ClocksTypeDef RCC_ClockFreq;
ErrorStatus HSEStartUpStatus;

/* Private function prototypes -----------------------------------------------*/
void NVIC_Configuration(void);


void SetSysClock(void);

#ifdef SYSCLK_HSE
  void SetSysClockToHSE(void);
#elif defined SYSCLK_FREQ_24MHz
  void SetSysClockTo24(void);
#elif defined SYSCLK_FREQ_36MHz
  void SetSysClockTo36(void);
#elif defined SYSCLK_FREQ_48MHz
  void SetSysClockTo48(void);
#elif defined SYSCLK_FREQ_56MHz
  void SetSysClockTo56(void);  
#elif defined SYSCLK_FREQ_72MHz
  void SetSysClockTo72(void);
#endif

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
void Rcc_SysClock(void)
{
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f10x_xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f10x.c file
     */     
       
  /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
  SetSysClock(); //设置时钟频率

  /* This function fills the RCC_ClockFreq structure with the current
     frequencies of different on chip clocks (for debug purpose) */
  RCC_GetClocksFreq(&RCC_ClockFreq);     //获取时钟频率
  /* Enable Clock Security System(CSS): this will generate an NMI exception
     when HSE clock fails */
  RCC_ClockSecuritySystemCmd(ENABLE);   //启用或禁止时钟安全系统
     
  /* NVIC configuration ------------------------------------------------------*/
  NVIC_Configuration(); 
}

/**
  * @brief  Configures the System clock frequency, HCLK, PCLK2 and PCLK1
  *         prescalers.
  * @param  None
  * @retval None
  */
void SetSysClock(void)
{    
/* The System clock configuration functions defined below assume that:
        - For Low, Medium and High density devices an external 8MHz crystal is
          used to drive the System clock.
        - For Connectivity line devices an external 25MHz crystal is used to drive
          the System clock.
     If you are using different crystal you have to adapt those functions accordingly.*/

#if defined SYSCLK_HSE
  SetSysClockToHSE();
#elif defined SYSCLK_FREQ_24MHz
  SetSysClockTo24();  
#elif defined SYSCLK_FREQ_36MHz
  SetSysClockTo36();
#elif defined SYSCLK_FREQ_48MHz
  SetSysClockTo48();
#elif defined SYSCLK_FREQ_56MHz
  SetSysClockTo56(); 
#elif defined SYSCLK_FREQ_72MHz
  SetSysClockTo72();
#endif
 
 /* If none of the define above is enabled, the HSI is used as System clock
    source (default after reset) */ 
}

/**
  * @brief  Selects HSE as System clock source and configure HCLK, PCLK2
  *         and PCLK1 prescalers. 
  * @param  None
  * @retval None
  */
void SetSysClockToHSE(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

#ifndef STM32F10X_CL
    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_Latency_0);
#else
    if (HSE_Value <= 24000000)
	{
      /* Flash 0 wait state */
      FLASH_SetLatency(FLASH_Latency_0);
	}
	else
	{
      /* Flash 1 wait state */
      FLASH_SetLatency(FLASH_Latency_1);
	}

#endif /* STM32F10X_CL */
#endif /* STM32F10X_LD_VL && STM32F10X_MD_VL */
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* Select HSE as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_HSE);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x04)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}

/**
  * @brief  Sets System clock frequency to 24MHz and configure HCLK, PCLK2 
  *         and PCLK1 prescalers. 
  * @param  None
  * @retval None
  */
void SetSysClockTo24(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_Latency_0);
#endif /* STM32F10X_LD_VL && STM32F10X_MD_VL */
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

#ifdef STM32F10X_CL
    /* Configure PLLs *********************************************************/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    /* Enable PLL2 */
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {}

    /* PLL configuration: PLLCLK = (PLL2 / 10) * 6 = 24 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div10);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
#elif defined STM32F10X_LD_VL || defined STM32F10X_MD_VL || defined STM32F10X_HD_VL 
    /* PLLCLK = (8MHz/2) * 6 = 24 MHz */
    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div2);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
#else
    /* PLLCLK = 8MHz * 3 = 24 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_3);
#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}
#if !defined STM32F10X_LD_VL && !defined STM32F10X_MD_VL && !defined STM32F10X_HD_VL
/**
  * @brief  Sets System clock frequency to 36MHz and configure HCLK, PCLK2 
  *         and PCLK1 prescalers. 
  * @param  None
  * @retval None
  */
void SetSysClockTo36(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 1 wait state */
    FLASH_SetLatency(FLASH_Latency_1);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

#ifdef STM32F10X_CL
    /* Configure PLLs *********************************************************/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    /* Enable PLL2 */
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {}

    /* PLL configuration: PLLCLK = (PLL2 / 10) * 9 = 36 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div10);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
    /* PLLCLK = (8MHz / 2) * 9 = 36 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div2, RCC_PLLMul_9);
#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}

/**
  * @brief  Sets System clock frequency to 48MHz and configure HCLK, PCLK2 
  *         and PCLK1 prescalers. 
  * @param  None
  * @retval None
  */
void SetSysClockTo48(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 1 wait state */
    FLASH_SetLatency(FLASH_Latency_1);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

#ifdef STM32F10X_CL
    /* Configure PLLs *********************************************************/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    /* Enable PLL2 */
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {}

    /* PLL configuration: PLLCLK = (PLL2 / 5) * 6 = 48 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
#else
    /* PLLCLK = 8MHz * 6 = 48 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_6);
#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}

/**
  * @brief  Sets System clock frequency to 56MHz and configure HCLK, PCLK2 
  *         and PCLK1 prescalers. 
  * @param  None
  * @retval None
  */
void SetSysClockTo56(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

#ifdef STM32F10X_CL
    /* Configure PLLs *********************************************************/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    /* Enable PLL2 */
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {}

    /* PLL configuration: PLLCLK = (PLL2 / 5) * 7 = 56 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_7);
#else
    /* PLLCLK = 8MHz * 7 = 56 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_7);
#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}

/**
  * @brief  Sets System clock frequency to 72MHz and configure HCLK, PCLK2 
  *         and PCLK1 prescalers. 
  * @param  None
  * @retval None
  */
void SetSysClockTo72(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/   
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

#ifdef STM32F10X_CL
    /* Configure PLLs *********************************************************/
    /* PLL2 configuration: PLL2CLK = (HSE / 5) * 8 = 40 MHz */
    RCC_PREDIV2Config(RCC_PREDIV2_Div5);
    RCC_PLL2Config(RCC_PLL2Mul_8);

    /* Enable PLL2 */
    RCC_PLL2Cmd(ENABLE);

    /* Wait till PLL2 is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLL2RDY) == RESET)
    {}

    /* PLL configuration: PLLCLK = (PLL2 / 5) * 9 = 72 MHz */ 
    RCC_PREDIV1Config(RCC_PREDIV1_Source_PLL2, RCC_PREDIV1_Div5);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_9);
#else
    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif

    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  { /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */    

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}

#endif /* STM32F10X_LD_VL && STM32F10X_MD_VL */

/**
  * @brief  Configures Vector Table base location.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable and configure RCC global IRQ channel */
  NVIC_InitStructure.NVIC_IRQChannel = RCC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}

#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/

 
 







