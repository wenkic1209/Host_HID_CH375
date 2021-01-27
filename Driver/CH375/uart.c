
#include "interface.h" 

/* UART2    9600
   PD5  TX
   PD6  RX
   PA3  INT
*/

#define CH375_INT_WIRE			PAin(3)

//�˿ڳ�ʼ��
void CH375_PORT_INIT( void )
{
	float temp;
	u16 mantissa;
	u16 fraction;	   
	temp=(float)( 48 * 1000000)/(9600 * 16);//�õ�USARTDIV@OVER8=0
	mantissa=temp;				 //�õ���������
	fraction=(temp-mantissa)*16; //�õ�С������@OVER8=0 
    mantissa<<=4;
	mantissa+=fraction; 
	RCC->AHB1ENR|=(1<<3)|(1<<0);   	//ʹ��PORTA��PORTD��ʱ��  
	RCC->APB1ENR|=1<<17;  	//ʹ�ܴ���2ʱ�� 
	GPIO_Set(GPIOA,1<<3 ,GPIO_MODE_IN,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);    //INT
	GPIO_Set(GPIOD,PIN5|PIN6,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU); //PD5,PD6,���ù���,�������
 	GPIO_AF_Set(GPIOD,5,7);	//PD5,AF7
	GPIO_AF_Set(GPIOD,6,7); //PD6,AF7  	   
	//����������
	USART2->CR1 = 0;
 	USART2->BRR=mantissa; 	//����������	 
	USART2->CR1&=~(1<<15); 	//����OVER8=0 
	USART2->CR1|=1<<12;     //9λ����λ
	USART2->CR1|=1<<3;  	//���ڷ���ʹ�� 
	USART2->CR1|=1<<2;  	//���ڽ���ʹ��
	USART2->CR1|=1<<13;  	//����ʹ��	
}

//д����
void xWriteCH375Cmd( uint8_t cmd ) { 				 /* ��CH375������˿�д������,���ڲ�С��2uS,�����Ƭ���Ͽ�����ʱ */			
	while((USART2->SR&0x40)==0);
	USART2->DR = ((uint16_t)cmd | 0x0100 );
	delay_us(2);
}

//д����
void xWriteCH375Data( uint8_t dat ) { 				 /* ��CH375�����ݶ˿�д������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
	while((USART2->SR&0x40)==0);   
	USART2->DR = (uint16_t)dat;
	delay_us(1);
}

//������
uint8_t xReadCH375Data(void) {  				     /* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1uS,�����Ƭ���Ͽ�����ʱ */
	uint32_t i;
	for(i=0;i<500000;i++)                      //����500ms���ڽ��ճ�ʱ
	{
		if(USART2->SR&0x20)    //RXNE
		{
			return ((uint8_t)USART2->DR);
		}
		delay_us(1);
	}
	return ERR_USB_UNKNOWN;
}

/* ��ѯCH375�ж�(INT#�͵�ƽ) */
uint8_t	Query375Interrupt( void )
{
	if( CH375_INT_WIRE ) return FALSE ;
	else{
		return TRUE ;
	}
}

/* CH375��ʼ������ */
uint8_t	mInitCH375Host( void )  /* ��ʼ��CH375 */
{
	uint8_t	res;	
	delay_ms(50);        /* �ϵ��������ʱ50ms���� */
	CH375_PORT_INIT( );  /* �ӿ�Ӳ����ʼ�� */
	xWriteCH375Cmd( CMD_CHECK_EXIST );  /* ���Ե�Ƭ����CH375֮���ͨѶ�ӿ� */
	xWriteCH375Data( 0x65 );
	res = xReadCH375Data( );
	if ( res != 0x9A ) return( ERR_USB_UNKNOWN );  /* ͨѶ�ӿڲ�����,����ԭ����:�ӿ������쳣,�����豸Ӱ��(Ƭѡ��Ψһ),���ڲ�����,һֱ�ڸ�λ,���񲻹��� */

	xWriteCH375Cmd( CMD_SET_USB_MODE );  /* �豸USB����ģʽ */
	xWriteCH375Data( 0x06 );
	delay_us( 20 );
	res = xReadCH375Data( );
	if ( res == CMD_RET_SUCCESS ) return( USB_INT_SUCCESS );
	else return( ERR_USB_UNKNOWN );  /* ����ģʽ���� */		
	
}



