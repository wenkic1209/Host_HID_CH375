
/****************************************
**  ����: CH375��CH376����HID�豸��������̣����
**  ��Ȩ����Ȩ����(C) 1999-2017    
**  ��վ: http://www.wch.com    
**  �汾: V1.0
**  ����: GJ  2017.3.31
**  ����: ȫ��USB2.0 �������̣���Ӧ��CH375&CH376��ֻ��Ҫ�޸ĳɶ�Ӧͷ�ļ���
****************************************
*/


#include <stdio.h>
#include "CH375INC.H"
#include "delay.h"
#include "sys.h"
#include "debug.h"

#define CH375_WR                      PEout(10)
#define CH375_CS                      //PEout(9)
#define CH375_A0                      PAout(0)
#define CH375_RD                      PEout(9)
#define CH375_DATA_DIR_IN()           GPIOE->CRL = 0x44444444
#define CH375_DATA_DIR_OUT()          GPIOE->CRL = 0x33333333
#define CH375_DATA_DAT_OUT( mCmd )    GPIOE->ODR = (GPIOE->ODR & 0xFF00)|(uint8_t)mCmd
#define CH375_DATA_DAT_IN( )          (uint8_t)GPIOE->IDR
#define CH375_INT_WIRE                PAin(1)

extern void Bsp_CH375_Write_Cmd(uint8_t cmd );   /* ��CH375������˿�д������*/
extern void Bsp_CH375_Write_Data(uint8_t data ); /* ��CH375������˿�д������*/
extern uint8_t Bsp_CH375_Read_Data(void);

#if 0
#define CH375_LogA(...)
#define CH375_LogB(...)
#else
#define CH375_LogA(...)  do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
#define CH375_LogB(...)  do{sprint(__VA_ARGS__);}while(0)
#endif


//unsigned char volatile 	CH375_CMD_PORT; // _at_ 0xBDF1;	/* CH375����˿ڵ�I/O��ַ */
//unsigned char volatile 	CH375_DAT_PORT; // _at_ 0xBCF0;	/* CH375���ݶ˿ڵ�I/O��ַ */
//u8	CH375_INT_WIRE	=		0xB0^2;	/* P3.2, INT0, ����CH375��INT#����,���ڲ�ѯ�ж�״̬ */

#define	TRUE	1
#define	FALSE	0
#define	MAXSETUPPACK	8
unsigned char max_package;      //�˵�0������С
unsigned char endp_int;			//�ж϶˵��
unsigned char num_interfaces;	//�ӿ���
unsigned char config_value;		//����ֵ
unsigned char report_descr0_len=0;//REPORT0����������
unsigned char report_descr1_len=0;//REPORT1����������
unsigned char endp6_mode=0x80, endp7_mode=0x80;//ͬ����־λ��ֵ
unsigned char data_buf[128];      //�����������������ʵ���С

union _REQUEST					//������ṹ
{	struct
	{	unsigned char bmRequestType;
		unsigned char bRequest;
		unsigned int wValue;
		unsigned int wIndex;
		unsigned int wLength;
	}Req;
	unsigned char Req_buf[8];
}CH_Request;

/* �������õ�����ʱ���� */
static void	delay2us( )
{
   delay_us(2);
}
static void	delay1us( )
{
   delay_us(1);
}
static void delayms(unsigned char delay)
{	
   delay_ms(delay);
}


/* ��д�Ĵ������� */
void CH374_PORT_INIT( void )  /* ����ʹ��ͨ��I/Oģ�Ⲣ�ڶ�дʱ��,���Խ��г�ʼ�� */
{
	GPIO_InitTypeDef GPIO_InitStructure;

	
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


/* �ײ��д����,Ҳ����ֲ����Ҫ�޸ĵĲ��� */
void CH375_WR_CMD_PORT( unsigned char cmd ) {  /* ��CH375������˿�д������,���ڲ�С��4uS,�����Ƭ���Ͽ�����ʱ */

    Bsp_CH375_Write_Cmd(cmd);
}
void CH375_WR_DAT_PORT( unsigned char dat ) {  /* ��CH375�����ݶ˿�д������,���ڲ�С��1.5uS,�����Ƭ���Ͽ�����ʱ */
    Bsp_CH375_Write_Data(dat);
 
}
unsigned char CH375_RD_DAT_PORT() {  /* ��CH375�����ݶ˿ڶ�������,���ڲ�С��1.5uS,�����Ƭ���Ͽ�����ʱ */

	return (Bsp_CH375_Read_Data() );
}
/* ����USBģʽ */
unsigned char set_usb_mode( unsigned char mode ) {  /* ����CH37X�Ĺ���ģʽ */
	unsigned char i;
	CH375_WR_CMD_PORT( CMD_SET_USB_MODE );
	CH375_WR_DAT_PORT( mode );
	endp6_mode=endp7_mode=0x80;  /* �����˸�λUSB����ͬ����־ */
	for( i=0; i!=100; i++ ) {  /* �ȴ�����ģʽ�������,������30uS */
		if ( CH375_RD_DAT_PORT()==CMD_RET_SUCCESS ) return( TRUE );  /* �ɹ� */
	}
	return( FALSE );  /* CH375����,����оƬ�ͺŴ����ߴ��ڴ��ڷ�ʽ���߲�֧�� */
}
/* ����USBͨѶ�ٶ�Ϊ���٣���������ã�Ĭ��Ϊȫ�� */
void set_freq(void)
{	
	CH375_WR_CMD_PORT(0x0b);    /* �л�ʹ375B�������ģʽ */
	CH375_WR_DAT_PORT(0x17);
	CH375_WR_DAT_PORT(0xd8);
}

/* ����ͬ�� */
/* USB������ͬ��ͨ���л�DATA0��DATA1ʵ��: ���豸��, CH372/CH375�����Զ��л�;
   ��������, ������SET_ENDP6��SET_ENDP7�������CH375�л�DATA0��DATA1.
   �����˵ĳ�����������ΪSET_ENDP6��SET_ENDP7�ֱ��ṩһ��ȫ�ֱ���,
   ��ʼֵ��Ϊ80H, ÿִ��һ�γɹ������λ6ȡ��, ÿִ��һ��ʧ��������临λΪ80H. */
void toggle_recv() {  /* �������ճɹ���,�л�DATA0��DATA1ʵ������ͬ�� */
	CH375_WR_CMD_PORT( CMD_SET_ENDP6 );
	CH375_WR_DAT_PORT( endp6_mode );
	endp6_mode^=0x40;
	delay2us();
}

void toggle_send() {  /* �������ͳɹ���,�л�DATA0��DATA1ʵ������ͬ�� */
	CH375_WR_CMD_PORT( CMD_SET_ENDP7 );
	CH375_WR_DAT_PORT( endp7_mode );
	endp7_mode^=0x40;
	delay2us();
}
/* �ȴ�37X�����ж� */
unsigned char wait_interrupt() {          /* �����˵ȴ��������, ���ز���״̬ */
	while( CH375_INT_WIRE );              /* ��ѯ�ȴ�CH375��������ж�(INT#�͵�ƽ) */
	CH375_WR_CMD_PORT( CMD_GET_STATUS );  /* ������������ж�, ��ȡ�ж�״̬ */
	return( CH375_RD_DAT_PORT() );
}
/* ��д�ڲ������� */
unsigned char rd_usb_data( unsigned char *buf ) {  /* ��CH37X�������ݿ� */
	unsigned char i, len;
	CH375_WR_CMD_PORT( CMD_RD_USB_DATA );  /* ��CH375�Ķ˵㻺������ȡ���յ������� */
	len=CH375_RD_DAT_PORT();  /* �������ݳ��� */
	for ( i=0; i!=len; i++ ) *buf++=CH375_RD_DAT_PORT();
	return( len );
}
void wr_usb_data( unsigned char len, unsigned char *buf ) {  /* ��CH37Xд�����ݿ� */
	CH375_WR_CMD_PORT( CMD_WR_USB_DATA7 );  /* ��CH375�Ķ˵㻺����д��׼�����͵����� */
	CH375_WR_DAT_PORT( len );  /* �������ݳ���, len���ܴ���64 */
	while( len-- ) CH375_WR_DAT_PORT( *buf++ );
}
 /* ִ��USB���� */
unsigned char issue_token( unsigned char endpnum, unsigned char pid ) 
{ 
	CH375_WR_CMD_PORT( CMD_ISSUE_TOKEN );
	CH375_WR_DAT_PORT( (endpnum<<4)|pid  );  /* ��4λĿ�Ķ˵��, ��4λ����PID */
	return (wait_interrupt());
}
/* ���ö���ӦNAK�����Դ��� */
void set_retry(unsigned char num)
{	CH375_WR_CMD_PORT( CMD_SET_RETRY); 
	CH375_WR_DAT_PORT( 0x25);
	CH375_WR_DAT_PORT( num);
	delay2us();
}
/* ���ƴ���,��ö�ٹ��̵��� */
unsigned short SETUP_Transfer( unsigned char *DataBuf, unsigned short *plen)
{	
	unsigned char  l, s;
	unsigned short req_len, real_len = 0;
	unsigned char *p = DataBuf;
	endp7_mode=0x80;	//DATA0������
	endp6_mode=0xc0;	//DATA1������
	req_len = (unsigned short int)(CH_Request.Req_buf[7]<<8) | CH_Request.Req_buf[6];
	
/* SETUP�׶� */	
	wr_usb_data(8, CH_Request.Req_buf);	
	toggle_send();
	s = issue_token(0,DEF_USB_PID_SETUP );
	if(s != USB_INT_SUCCESS)		
	{	
		return(0);
	}	
/* DATA�׶� */	
	if(CH_Request.Req_buf[0]&0x80)		//IN����
	{
		while(req_len)
		{		
			toggle_recv();
			s = issue_token(0 , DEF_USB_PID_IN);
			if( s == USB_INT_SUCCESS )
			{
				l = rd_usb_data( p );
				real_len += l;
				if( l<max_package )		//�̰�
				{
					break;
				}
				p += l;
				req_len -= l;
			}
			else return(0);
		}	
	}
	else							//OUT����
	{
		while(req_len)
		{		
			l = (req_len>max_package)?max_package:req_len;
			wr_usb_data(l, p);
			toggle_send();			
			s = issue_token(0 , DEF_USB_PID_OUT);
			if( s == USB_INT_SUCCESS )
			{
				real_len += l;
				p += l;
				req_len -= l;
			}
			else return(0);
		}
	}
	
/* ״̬�׶� */
	endp7_mode = endp6_mode = 0xc0;	//DATA1
	if(CH_Request.Req_buf[0]&0x80)		
	{
		wr_usb_data(0, CH_Request.Req_buf);
		toggle_send();
		s = issue_token(0, DEF_USB_PID_OUT);
	}
	else
	{
		toggle_recv();
		s = issue_token(0 , DEF_USB_PID_IN);
	}
	if(s != USB_INT_SUCCESS)
	{
		return(0);
	}

	*plen = real_len;
	return(s);		//�ɹ�
}

/* USB���߸�λ */
void reset_device()
{	
/* USB�淶��δҪ����USB�豸�������븴λ���豸,���Ǽ������WINDOWS����������,������ЩUSB�豸ҲҪ���ڲ��������ȸ�λ���ܹ��� */
	set_usb_mode( 7 );  /* ��λUSB�豸,CH375��USB�ź��ߵ�D+��D-����͵�ƽ */
	delayms(20);
	set_usb_mode( 6 );  /* ������λ */
	while ( wait_interrupt()!=USB_INT_CONNECT );  /* �ȴ���λ֮����豸���ٴ��������� */

}
/* ʹ�û�ȡ����������,����1��ȡ�豸������������2��ȡ���������� */
/* ��Ϊ�ڲ�������ֻ��64�ֽڣ���������豸����������64�ֽڣ����ܻ�������������Ա�����δʹ�ô˷�ʽ */
unsigned char get_descr( unsigned char type ) {  /* ���豸�˻�ȡ������ */
	unsigned char status;
	CH375_WR_CMD_PORT( CMD_GET_DESCR );
	CH375_WR_DAT_PORT( type );  /* ����������, ֻ֧��1(�豸)����2(����) */
	status=wait_interrupt();  /* �ȴ�CH375������� */
	if ( status==USB_INT_SUCCESS ) {  /* �����ɹ� */
		unsigned char i, len;
		len=rd_usb_data( data_buf );
		printf( "%s��������:", type==1?"�豸":"����" );
		for ( i=0; i!=len; i++ ) printf( "%02x ", (unsigned int)data_buf[i] );
		printf( "\n" );
	}
	return( status );
}
/* ����USB��ַ */
unsigned char set_addr( unsigned char addr ) {  /* �����豸�˵�USB��ַ */
	unsigned char status;
	CH375_WR_CMD_PORT( CMD_SET_ADDRESS );  /* ����USB�豸�˵�USB��ַ */
	CH375_WR_DAT_PORT( addr );  /* ��ַ, ��1��127֮�������ֵ, ����2��20 */
	status=wait_interrupt();  /* �ȴ�CH375������� */
	if ( status==USB_INT_SUCCESS ) {  /* �����ɹ� */
		CH375_WR_CMD_PORT( CMD_SET_USB_ADDR );  /* ����USB�����˵�USB��ַ */
		CH375_WR_DAT_PORT( addr );  /* ��Ŀ��USB�豸�ĵ�ַ�ɹ��޸ĺ�,Ӧ��ͬ���޸������˵�USB��ַ */
	}
	return( status );
}
/* �������� */
unsigned char set_config( unsigned char cfg ) {  /* �����豸�˵�USB���� */
	CH375_WR_CMD_PORT( CMD_SET_CONFIG );  /* ����USB�豸�˵�����ֵ */
	CH375_WR_DAT_PORT( cfg );  /* ��ֵȡ��USB�豸�������������� */
	return( wait_interrupt() );  /* �ȴ�CH375������� */
}
/* ����������������Ϣ,���ҵ�ͨ�Ŷ˵� */
void parse_config_descr(unsigned char *p)
{	
	unsigned char i,l;
	unsigned char s = 0;
	unsigned char ok = 0;
	num_interfaces=p[4];		//����ӿ���
	config_value=p[5];		   //��������ֵ
	for(i=0;i<p[2];i+=l)
	{
		if((p[i]==0x09)&&(p[i+1]==0x04))//�ӿ�������
		{
			s++;
			if((p[i+5]==0x03)&&(p[i+7]!=0x00))
				ok = 1;    
		}
		else if((p[i]==0x09)&&(p[i+1]==0x21))//HID������
		{
			if(s==1)
				report_descr0_len=p[i+7];    //REPORT����������
			else
				report_descr1_len=p[i+7];    //REPORT����������		
		}			
		else if((p[i]==0x07)&&(p[i+1]==0x05))//�˵�������
		{	
			if(ok)   
			{				
				if((p[i+3]==0x03) && (p[i+2]&0x80))
				{
					ok = 0;
					endp_int=p[i+2]&0x0f;       //�ж϶˵�ţ�ֻ�洢������ϴ��˵㣩
				}
			}
			
		}	
		l = p[i];
	}	
}
/* ��ȡ�豸�ٶ� */
unsigned char get_freq(void)
{		
	CH375_WR_CMD_PORT(0x0a);    /* ��ѯ��ǰ�豸�ٶ� */
	CH375_WR_DAT_PORT(0x07);
	return (CH375_RD_DAT_PORT());
}
unsigned char read_id(void)
{
	CH375_WR_CMD_PORT( CMD_GET_IC_VER);
	return (CH375_RD_DAT_PORT());
}
/* ������ */
void CH375_Control() 
{
	unsigned char i,s;
	unsigned short len;

	max_package = MAXSETUPPACK;
	CH374_PORT_INIT( ); //�ܽų�ʼ��
    delayms(100);
	s = read_id();
	CH375_LogA("chip-id:0x%02x",(unsigned short)s);

	set_usb_mode( 5 );  /* ����USB����ģʽ, ����豸����CH37X, ��ô5��6���� */
	CH375_LogA("wait connect...");
	while(1)
	{
		while ( wait_interrupt()!=USB_INT_CONNECT );  /* �ȴ��豸���������� */
		delayms(200);                    /* �ȴ������ȶ� */
/***** ��λ���USB�豸�ٶ� *****/
		CH375_LogA("check rate");
		s = get_freq();	
		reset_device();
        if( s&0x10 )	
		{
			CH375_LogA("low rate");
			set_freq();		        /* �л�ʹ375B�������ģʽ */
		}
		else
			CH375_LogA("full rate");
		delayms(100);         //��λ֮���൱����������������ʱ��֤�豸�ȶ�
/***** ��ȡ�豸������ *****/
		get_descr(0x01);//��ȡ�豸������
		max_package = data_buf[7];  //�˵�0������С

//		Request.Req.bmRequestType=0x80;
//		Request.Req.bRequest=0x06;
//		Request.Req.wValue=0x0001;      /* ��Ϊ51��Ƭ���Ǵ�˴洢������д��0x0100,�������� */
//		Request.Req.wIndex=0x0000;
//		Request.Req.wLength=0x0800;
//		if( SETUP_Transfer(data_buf, &len) == USB_INT_SUCCESS )
//		{	
//			max_package = data_buf[7];  //�˵�0������С
//			Request.Req.wLength=0x1200;
//			if( SETUP_Transfer(data_buf, &len) )
//			{			
//				for(i=0; i!=len; i++)
//					CH375_LogB("%02x ",(unsigned int)data_buf[i]);	
//				    CH375_LogA(" ");
//			}
//		}
//		else 
//			CH375_Log("get device descr failed\n");
/***** ��λ *****/
		reset_device();
        if( s&0x10 )	set_freq();		/* �л�ʹ375B�������ģʽ */
		delayms(100);                   //��λ֮���൱����������������ʱ��֤�豸�ȶ�
/***** ���õ�ַ  *****/			
		set_addr(5);//���õ�ַ
/***** ��ȡ���������� *****/
//		get_descr(0x02);//��ȡ����������
		CH_Request.Req.bmRequestType=0x80;
		CH_Request.Req.bRequest=0x06;
		CH_Request.Req.wValue=0x0002;
		CH_Request.Req.wIndex=0x0000;
		CH_Request.Req.wLength=0x0400;
		if(SETUP_Transfer(data_buf, &len)==USB_INT_SUCCESS)
		{
			CH_Request.Req_buf[6] = data_buf[2];
			CH_Request.Req_buf[7] = data_buf[3];
			SETUP_Transfer(data_buf, &len);
			for(i=0;i!=len;i++)
				CH375_LogB("%02x ",(unsigned int)data_buf[i]);	
			CH375_LogA("\n");
		}
		else CH375_LogA("get config descr failed");
       /***** �������������� *****/
		parse_config_descr(data_buf);   //������������һЩֵ
       /***** �������� *****/		
    	CH375_LogA("set config");
		set_config(config_value);     //��������
        /***** HID������ *****/	
		for(s=0;s<num_interfaces;s++)
		{
			CH375_LogA("set idle");
			CH_Request.Req.bmRequestType=0x21;
			CH_Request.Req.bRequest=0x0A;
			CH_Request.Req.wValue=0x0000;
			CH_Request.Req.wIndex=0x0000;
			CH_Request.Req.wLength=0x0000;
			CH_Request.Req_buf[4] = s;
			if(SETUP_Transfer(NULL, NULL)!=USB_INT_SUCCESS)	
				CH375_LogA("set idle failed");
			
			CH375_LogA("get report");
			CH_Request.Req.bmRequestType=0x81;
			CH_Request.Req.bRequest=0x06;
			CH_Request.Req.wValue=0x0022;
			CH_Request.Req.wIndex=0x0000;
			CH_Request.Req_buf[4] = s;
			if(s==0) CH_Request.Req.wLength=0x0000|((unsigned short)(report_descr0_len+0x40)<<8);
			else CH_Request.Req.wLength=0x0000|((unsigned short)(report_descr1_len+0x40)<<8);
			
			if(SETUP_Transfer(data_buf, &len)==USB_INT_SUCCESS)
			{
				for(i=0;i!=len;i++)
					CH375_LogB("%02x ",(unsigned short)data_buf[i]);	
				CH375_LogA(" ");			
			}
			else CH375_LogA("get report descr failed");
		}		
		CH375_LogA("set report");		//���ڼ�����һ�����ǵ���ָʾ��
		CH_Request.Req.bmRequestType=0x21;
		CH_Request.Req.bRequest=0x09;
		CH_Request.Req.wValue=0x0002;
		CH_Request.Req.wIndex=0x0000;
		CH_Request.Req.wLength=0x0100;	
		data_buf[0]=1;		
		if(SETUP_Transfer(data_buf, &len)!=USB_INT_SUCCESS)
			CH375_LogA("set report failed\n");	
       /* ���ö���ӦNAK�����Դ��� */
		set_retry(3);  //��ʱ����3�Σ����յ�NAK������
        /* ��ȡ���� */
		endp6_mode=0x80;  //��λͬ����־
		toggle_recv();
		while(1)
		{
			s = issue_token( endp_int,DEF_USB_PID_IN);
			if(s==USB_INT_SUCCESS)
			{
				toggle_recv();
				len = rd_usb_data( data_buf );
				for(i=0;i!=len;i++)
					CH375_LogB("%02x ",(unsigned short)data_buf[i]);
                CH375_LogA(" ");

			}
		}
	}
}

#if 0
///* ������ */
//void main() 
//{
//	unsigned char i,s;
//	unsigned short len;

//	max_package = MAXSETUPPACK;
//	mInitSTDIO( );

//	s = read_id();
//	printf("chip-id:0x%02x\n",(unsigned short)s);

//	set_usb_mode( 5 );  /* ����USB����ģʽ, ����豸����CH37X, ��ô5��6���� */
//	printf("wait connect...\n");
//	while(1)
//	{
//		while ( wait_interrupt()!=USB_INT_CONNECT );  /* �ȴ��豸���������� */
//		delayms(200);                    /* �ȴ������ȶ� */
///***** ��λ���USB�豸�ٶ� *****/
//		printf("check rate\n");
//		s = get_freq();	
//		reset_device();
//        if( s&0x10 )	
//		{
//			printf("low rate\n");
//			set_freq();		/* �л�ʹ375B�������ģʽ */
//		}
//		else
//			printf("full rate\n");
//		delayms(100);         //��λ֮���൱����������������ʱ��֤�豸�ȶ�
///***** ��ȡ�豸������ *****/
//		get_descr(0x01);//��ȡ�豸������
//		max_package = data_buf[7];  //�˵�0������С

////		Request.Req.bmRequestType=0x80;
////		Request.Req.bRequest=0x06;
////		Request.Req.wValue=0x0001;      /* ��Ϊ51��Ƭ���Ǵ�˴洢������д��0x0100,�������� */
////		Request.Req.wIndex=0x0000;
////		Request.Req.wLength=0x0800;
////		if( SETUP_Transfer(data_buf, &len) == USB_INT_SUCCESS )
////		{	
////			max_package = data_buf[7];  //�˵�0������С
////			Request.Req.wLength=0x1200;
////			if( SETUP_Transfer(data_buf, &len) )
////			{			
////				for(i=0; i!=len; i++)
////					printf("%02x ",(unsigned int)data_buf[i]);	
////				printf("\n");
////			}
////		}
////		else 
////			printf("get device descr failed\n");
///***** ��λ *****/
//		reset_device();
//        if( s&0x10 )	set_freq();		/* �л�ʹ375B�������ģʽ */
//		delayms(100);                   //��λ֮���൱����������������ʱ��֤�豸�ȶ�
///***** ���õ�ַ  *****/			
//		set_addr(5);//���õ�ַ
///***** ��ȡ���������� *****/
////		get_descr(0x02);//��ȡ����������
//		Request.Req.bmRequestType=0x80;
//		Request.Req.bRequest=0x06;
//		Request.Req.wValue=0x0002;
//		Request.Req.wIndex=0x0000;
//		Request.Req.wLength=0x0400;
//		if(SETUP_Transfer(data_buf, &len)==USB_INT_SUCCESS)
//		{
//			Request.Req_buf[6] = data_buf[2];
//			Request.Req_buf[7] = data_buf[3];
//			SETUP_Transfer(data_buf, &len);
//			for(i=0;i!=len;i++)
//				printf("%02x ",(unsigned int)data_buf[i]);	
//			printf("\n");
//		}
//		else printf("get config descr failed\n");
///***** �������������� *****/
//		parse_config_descr(data_buf);   //������������һЩֵ
///***** �������� *****/		
//    	printf("set config\n");
//		set_config(config_value);     //��������
///***** HID������ *****/	
//		for(s=0;s<num_interfaces;s++)
//		{
//			printf("set idle\n");
//			Request.Req.bmRequestType=0x21;
//			Request.Req.bRequest=0x0A;
//			Request.Req.wValue=0x0000;
//			Request.Req.wIndex=0x0000;
//			Request.Req.wLength=0x0000;
//			Request.Req_buf[4] = s;
//			if(SETUP_Transfer(NULL, NULL)!=USB_INT_SUCCESS)	
//				printf("set idle failed\n");
//			
//			printf("get report\n");
//			Request.Req.bmRequestType=0x81;
//			Request.Req.bRequest=0x06;
//			Request.Req.wValue=0x0022;
//			Request.Req.wIndex=0x0000;
//			Request.Req_buf[4] = s;
//			if(s==0) Request.Req.wLength=0x0000|((unsigned short)(report_descr0_len+0x40)<<8);
//			else Request.Req.wLength=0x0000|((unsigned short)(report_descr1_len+0x40)<<8);
//			
//			if(SETUP_Transfer(data_buf, &len)==USB_INT_SUCCESS)
//			{
//				for(i=0;i!=len;i++)
//					printf("%02x ",(unsigned short)data_buf[i]);	
//				printf("\n");			
//			}
//			else printf("get report descr failed\n");
//		}		
//		printf("set report\n");		//���ڼ�����һ�����ǵ���ָʾ��
//		Request.Req.bmRequestType=0x21;
//		Request.Req.bRequest=0x09;
//		Request.Req.wValue=0x0002;
//		Request.Req.wIndex=0x0000;
//		Request.Req.wLength=0x0100;	
//		data_buf[0]=1;		
//		if(SETUP_Transfer(data_buf, &len)!=USB_INT_SUCCESS)
//			printf("set report failed\n");	
///* ���ö���ӦNAK�����Դ��� */
//		set_retry(3);  //��ʱ����3�Σ����յ�NAK������
///* ��ȡ���� */
//		endp6_mode=0x80;  //��λͬ����־
//		toggle_recv();
//		while(1)
//		{
//			s = issue_token( endp_int,DEF_USB_PID_IN);
//			if(s==USB_INT_SUCCESS)
//			{
//				toggle_recv();
//				len = rd_usb_data( data_buf );
//				for(i=0;i!=len;i++)
//					printf("%02x ",(unsigned short)data_buf[i]);
//				printf("\n");
//			}
//		}
//	}
//}
#endif