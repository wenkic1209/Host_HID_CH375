
#include "interface.h" 
#include "delay.h"
#include "debug.h"
#include "TM_library.h"
#include "stdio.h"
/* 接口配置描述 */
/* A0    --- PA0
   D0-D7 --- PE0-PE7
   CS    --- NC
   RD    --- PE10
   WR    --- PE9
   INT   --- PA1  
*/

#if 0
#define CH375_Log(...)
#else
#define CH375_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
#define CH375_Logn(...)  do{sprint(__VA_ARGS__);}while(0)
#endif

#define Bsp_DelayUs delay_us 
#define Bsp_DelayMs delay_ms


#define CH375_WR                      PEout(10)
#define CH375_CS                      //PEout(9)
#define CH375_A0                      PAout(0)
#define CH375_RD                      PEout(9)
#define CH375_DATA_DIR_IN()           GPIOE->CRL = 0x44444444
#define CH375_DATA_DIR_OUT()          GPIOE->CRL = 0x33333333
#define CH375_DATA_DAT_OUT( mCmd )    GPIOE->ODR = (GPIOE->ODR & 0xFF00)|(uint8_t)mCmd
#define CH375_DATA_DAT_IN( )          (uint8_t)GPIOE->IDR
#define CH375_INT_WIRE                PAin(1)


#define CH375_DATA_PORT      GPIOE
//#define CH375_DATA_OUTPUT()  CH375_DATA_PORT->CLR  = 0x33333333   //推挽输出(PE0--PE7)
//#define CH375_DATA_INPUT()   CH375_DATA_PORT->CLR  = 0x88888888   //输入模式
#define CH375_DATA_OUTPUT()  TM_GPIO_Init(CH375_DATA_PORT,0x00FF,GPIO_Mode_Out_PP,GPIO_Speed_50MHz) //推挽输出(PE0--PE7)
#define CH375_DATA_INPUT()   TM_GPIO_Init(CH375_DATA_PORT,0x00FF,GPIO_Mode_IPU   ,GPIO_Speed_50MHz) //输入模式

#define CH375_DATA(x)        {CH375_DATA_PORT->ODR &= 0xFF00;CH375_DATA_PORT->ODR |= x&0xFF;}
 
#define CH375_CS_1           //CH375_CS_GPIO_Port->ODR |=  CH375_CS_Pin
#define CH375_CS_0           //CH375_CS_GPIO_Port->ODR &= ~CH375_CS_Pin
 
#define CH375_A0_1           CH375_A0_GPIO_Port->ODR |=  CH375_A0_Pin
#define CH375_A0_0           CH375_A0_GPIO_Port->ODR &= ~CH375_A0_Pin
 
#define CH375_RD_1           CH375_RD_GPIO_Port->ODR |=  CH375_RD_Pin
#define CH375_RD_0           CH375_RD_GPIO_Port->ODR &= ~CH375_RD_Pin
 
#define CH375_WR_1           CH375_WR_GPIO_Port->ODR |=  CH375_WR_Pin
#define CH375_WR_0           CH375_WR_GPIO_Port->ODR &= ~CH375_WR_Pin
#define CH375_A0_GPIO_Port  GPIOA
#define CH375_RD_GPIO_Port  GPIOE
#define CH375_WR_GPIO_Port  GPIOE
#define CH375_A0_Pin        GPIO_Pin_0
#define CH375_RD_Pin        GPIO_Pin_9
#define CH375_WR_Pin        GPIO_Pin_10
#define CH375_INT_GPIO_Port GPIOA
#define CH375_INT_Pin       GPIO_Pin_1

void CH375_PORT_INIT( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOC, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = 0x00FF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9 |GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
#ifdef CH375_INT_WIRE	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//INT
    GPIO_Init(GPIOA, &GPIO_InitStructure);
#endif
	CH375_WR = 1;
	CH375_RD = 1;
	CH375_A0 = 0;
	CH375_DATA_DIR_IN( );                                   /* 设置并口输入 */
}
//写命令
void xWriteCH375Cmd( uint8_t cmd ) { 				 /* 向CH375的命令端口写入命令,周期不小于2uS,如果单片机较快则延时 */
	Bsp_DelayUs(5);
	CH375_CS_0;
	CH375_A0_1;
	CH375_DATA(cmd);
	CH375_WR_0;
	Bsp_DelayUs(5);
	CH375_CS_1;
	CH375_WR_1;
}
//写数据
void xWriteCH375Data( uint8_t dat ) { 				 /* 向CH375的数据端口写入数据,周期不小于1uS,如果单片机较快则延时 */
	Bsp_DelayUs(5);
	CH375_CS_0;
	CH375_A0_0;
	CH375_DATA(dat);
	CH375_WR_0;
	Bsp_DelayUs(5);
	CH375_CS_1;
	CH375_WR_1;
	CH375_A0_1;
}
//读数据
uint8_t xReadCH375Data(void) {  				     /* 从CH375的数据端口读出数据,周期不小于1uS,如果单片机较快则延时 */
	uint8_t data;
	CH375_CS_0;
	CH375_A0_0;
	CH375_RD_0;
	CH375_DATA_INPUT();
	Bsp_DelayUs(5);
	data = CH375_DATA_PORT->IDR;
	CH375_CS_1;
	CH375_A0_1;
	CH375_RD_1;
	CH375_DATA_OUTPUT();
	return data;
}
//读状态
uint8_t xReadCH375Status() {  				         /* 从CH375的数据端口读出数据,周期不小于1uS,如果单片机较快则延时 */
	uint8_t data;
	CH375_CS_0;
	CH375_A0_1;
	CH375_RD_0;
	CH375_DATA_INPUT();
	Bsp_DelayUs(5);
	data = CH375_DATA_PORT->IDR;
	CH375_CS_1;
	CH375_A0_1;
	CH375_RD_1;
	CH375_DATA_OUTPUT();
	return data;
}


/* 查询CH375中断(INT#低电平) */
uint8_t	Query375Interrupt( void )
{
	/* 如果连接了CH375的中断引脚则直接查询中断引脚 */
	/* 如果未连接CH375的中断引脚则查询状态端口 */
#ifdef	CH375_INT_WIRE
	return( CH375_INT_WIRE ? FALSE : TRUE ); 
#else
	return( xReadCH375Status( ) & 0x80 ? FALSE : TRUE );  
#endif	
}

/* CH375初始化代码 */
uint8_t	mInitCH375Host( void )  /* 初始化CH375 */
{
	uint8_t	res;	
	delay_ms(50);        /* 上电后至少延时50ms操作 */
	CH375_PORT_INIT( );  /* 接口硬件初始化 */
    do{
        xWriteCH375Cmd( CMD_CHECK_EXIST );  /* 测试单片机与CH375之间的通讯接口 */
        xWriteCH375Data( 0x65 );
        res = xReadCH375Data( );
        if ( res == 0x9A ) 
        {
            break;
    //		CH375_Log ("check erro:%02x\n",(uint16_t)res );
    //		return( ERR_USB_UNKNOWN );  /* 通讯接口不正常,可能原因有:接口连接异常,其它设备影响(片选不唯一),串口波特率,一直在复位,晶振不工作 */
        }
    }while(1);
	xWriteCH375Cmd( CMD_SET_USB_MODE );  /* 设备USB工作模式 */
	xWriteCH375Data( 0x06 );
	delay_us( 20 );
	res = xReadCH375Data( );
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else 
	{
		CH375_Log ("mode erro:%02x\n",(uint16_t)res );
		return( ERR_USB_UNKNOWN );  /* 设置模式错误 */	
	}
	
}





