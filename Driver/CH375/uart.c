
#include "interface.h" 

/* UART2    9600
   PD5  TX
   PD6  RX
   PA3  INT
*/

#define CH375_INT_WIRE			PAin(3)

//端口初始化
void CH375_PORT_INIT( void )
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)( 48 * 1000000)/(9600 * 16);//得到USARTDIV@OVER8=0
	mantissa=temp;				 //得到整数部分
	fraction=(temp-mantissa)*16; //得到小数部分@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=(1<<3)|(1<<0);   	//使能PORTA、PORTD口时钟  
	RCC->APB1ENR|=1<<17;  	//使能串口2时钟 
	GPIO_Set(GPIOA,1<<3 ,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);    //INT
	GPIO_Set(GPIOD,PIN5|PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PD5,PD6,复用功能,上拉输出
 	GPIO_AF_Set(GPIOD,5,7);	//PD5,AF7
	GPIO_AF_Set(GPIOD,6,7); //PD6,AF7  	   
	//波特率设置
	USART2->CR1 = 0;
 	USART2->BRR=mantissa; 	//波特率设置	 
	USART2->CR1&=~(1<<15); 	//设置OVER8=0 
	USART2->CR1|=1<<12;     //9位数据位
	USART2->CR1|=1<<3;  	//串口发送使能 
	USART2->CR1|=1<<2;  	//串口接收使能
	USART2->CR1|=1<<13;  	//串口使能	
}

//写命令
void xWriteCH375Cmd( uint8_t cmd ) { 				 /* 向CH375的命令端口写入命令,周期不小于2uS,如果单片机较快则延时 */			
	while((USART2->SR&0x40)==0);
	USART2->DR = ((uint16_t)cmd | 0x0100 );
	delay_us(2);
}

//写数据
void xWriteCH375Data( uint8_t dat ) { 				 /* 向CH375的数据端口写入数据,周期不小于1uS,如果单片机较快则延时 */
	while((USART2->SR&0x40)==0);   
	USART2->DR = (uint16_t)dat;
	delay_us(1);
}

//读数据
uint8_t xReadCH375Data(void) {  				     /* 从CH375的数据端口读出数据,周期不小于1uS,如果单片机较快则延时 */
	uint32_t i;
	for(i=0;i<500000;i++)                      //设置500ms串口接收超时
	{
		if(USART2->SR&0x20)    //RXNE
		{
			return ((uint8_t)USART2->DR);
		}
		delay_us(1);
	}
	return ERR_USB_UNKNOWN;
}

/* 查询CH375中断(INT#低电平) */
uint8_t	Query375Interrupt( void )
{
	if( CH375_INT_WIRE ) return FALSE ;
	else{
		return TRUE ;
	}
}

/* CH375初始化代码 */
uint8_t	mInitCH375Host( void )  /* 初始化CH375 */
{
	uint8_t	res;	
	delay_ms(50);        /* 上电后至少延时50ms操作 */
	CH375_PORT_INIT( );  /* 接口硬件初始化 */
	xWriteCH375Cmd( CMD_CHECK_EXIST );  /* 测试单片机与CH375之间的通讯接口 */
	xWriteCH375Data( 0x65 );
	res = xReadCH375Data( );
	if ( res != 0x9A ) return( ERR_USB_UNKNOWN );  /* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),串口波特率,一直在复位,晶振不工作 */

	xWriteCH375Cmd( CMD_SET_USB_MODE );  /* 设备USB工作模式 */
	xWriteCH375Data( 0x06 );
	delay_us( 20 );
	res = xReadCH375Data( );
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else return( ERR_USB_UNKNOWN );  /* 设置模式错误 */		
	
}



