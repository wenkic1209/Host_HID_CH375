
#include "interface.h" 
#include "delay.h"
#include "debug.h"
#include "TM_library.h"
#include "stdio.h"
/* �ӿ��������� */
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
//#define CH375_DATA_OUTPUT()  CH375_DATA_PORT->CLR  = 0x33333333   //�������(PE0--PE7)
//#define CH375_DATA_INPUT()   CH375_DATA_PORT->CLR  = 0x88888888   //����ģʽ
#define CH375_DATA_OUTPUT()  TM_GPIO_Init(CH375_DATA_PORT,0x00FF,GPIO_Mode_Out_PP,GPIO_Speed_50MHz) //�������(PE0--PE7)
#define CH375_DATA_INPUT()   TM_GPIO_Init(CH375_DATA_PORT,0x00FF,GPIO_Mode_IPU   ,GPIO_Speed_50MHz) //����ģʽ

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
	CH375_DATA_DIR_IN( );                                   /* ���ò������� */
}
//д����
void xWriteCH375Cmd( uint8_t cmd ) { 				 /* ��CH375������˿�д������,���ڲ�С��2uS,�����Ƭ���Ͽ�����ʱ */
	Bsp_DelayUs(5);
	CH375_CS_0;
	CH375_A0_1;
	CH375_DATA(cmd);
	CH375_WR_0;
	Bsp_DelayUs(5);
	CH375_CS_1;
	CH375_WR_1;
}
//д����
void xWriteCH375Data( uint8_t dat ) { 				 /* ��CH375�����ݶ˿�д������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
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
//������
uint8_t xReadCH375Data(void) {  				     /* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
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
//��״̬
uint8_t xReadCH375Status() {  				         /* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
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


/* ��ѯCH375�ж�(INT#�͵�ƽ) */
uint8_t	Query375Interrupt( void )
{
	/* ���������CH375���ж�������ֱ�Ӳ�ѯ�ж����� */
	/* ���δ����CH375���ж��������ѯ״̬�˿� */
#ifdef	CH375_INT_WIRE
	return( CH375_INT_WIRE ? FALSE : TRUE ); 
#else
	return( xReadCH375Status( ) & 0x80 ? FALSE : TRUE );  
#endif	
}

/* CH375��ʼ������ */
uint8_t	mInitCH375Host( void )  /* ��ʼ��CH375 */
{
	uint8_t	res;	
	delay_ms(50);        /* �ϵ��������ʱ50ms���� */
	CH375_PORT_INIT( );  /* �ӿ�Ӳ����ʼ�� */
    do{
        xWriteCH375Cmd( CMD_CHECK_EXIST );  /* ���Ե�Ƭ����CH375֮���ͨѶ�ӿ� */
        xWriteCH375Data( 0x65 );
        res = xReadCH375Data( );
        if ( res == 0x9A ) 
        {
            break;
    //		CH375_Log ("check erro:%02x\n",(uint16_t)res );
    //		return( ERR_USB_UNKNOWN );  /* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),���ڲ�����,һֱ�ڸ�λ,���񲻹��� */
        }
    }while(1);
	xWriteCH375Cmd( CMD_SET_USB_MODE );  /* �豸USB����ģʽ */
	xWriteCH375Data( 0x06 );
	delay_us( 20 );
	res = xReadCH375Data( );
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else 
	{
		CH375_Log ("mode erro:%02x\n",(uint16_t)res );
		return( ERR_USB_UNKNOWN );  /* ����ģʽ���� */	
	}
	
}





