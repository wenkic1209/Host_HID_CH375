
/****************************************
**  名称: CH375或CH376操作HID设备，比如键盘，鼠标
**  版权：版权所有(C) 1999-2017    
**  网站: http://www.wch.com    
**  版本: V1.0
**  作者: GJ  2017.3.31
**  描述: 全速USB2.0 主机例程，适应于CH375&CH376（只需要修改成对应头文件）
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

extern void Bsp_CH375_Write_Cmd(uint8_t cmd );   /* 向CH375的命令端口写入命令*/
extern void Bsp_CH375_Write_Data(uint8_t data ); /* 向CH375的命令端口写入数据*/
extern uint8_t Bsp_CH375_Read_Data(void);

#if 0
#define CH375_LogA(...)
#define CH375_LogB(...)
#else
#define CH375_LogA(...)  do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
#define CH375_LogB(...)  do{sprint(__VA_ARGS__);}while(0)
#endif


//unsigned char volatile 	CH375_CMD_PORT; // _at_ 0xBDF1;	/* CH375命令端口的I/O地址 */
//unsigned char volatile 	CH375_DAT_PORT; // _at_ 0xBCF0;	/* CH375数据端口的I/O地址 */
//u8	CH375_INT_WIRE	=		0xB0^2;	/* P3.2, INT0, 连接CH375的INT#引脚,用于查询中断状态 */

#define	TRUE	1
#define	FALSE	0
#define	MAXSETUPPACK	8
unsigned char max_package;      //端点0最大包大小
unsigned char endp_int;			//中断端点号
unsigned char num_interfaces;	//接口数
unsigned char config_value;		//配置值
unsigned char report_descr0_len=0;//REPORT0描述符长度
unsigned char report_descr1_len=0;//REPORT1描述符长度
unsigned char endp6_mode=0x80, endp7_mode=0x80;//同步标志位初值
unsigned char data_buf[128];      //描述符缓冲区可以适当减小

union _REQUEST					//请求包结构
{	struct
	{	unsigned char bmRequestType;
		unsigned char bRequest;
		unsigned int wValue;
		unsigned int wIndex;
		unsigned int wLength;
	}Req;
	unsigned char Req_buf[8];
}CH_Request;

/* 代码中用到的延时函数 */
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


/* 读写寄存器函数 */
void CH374_PORT_INIT( void )  /* 由于使用通用I/O模拟并口读写时序,所以进行初始化 */
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
	CH375_DATA_DIR_IN( );                                   /* 设置并口输入 */
}


/* 底层读写函数,也是移植必须要修改的部分 */
void CH375_WR_CMD_PORT( unsigned char cmd ) {  /* 向CH375的命令端口写入命令,周期不小于4uS,如果单片机较快则延时 */

    Bsp_CH375_Write_Cmd(cmd);
}
void CH375_WR_DAT_PORT( unsigned char dat ) {  /* 向CH375的数据端口写入数据,周期不小于1.5uS,如果单片机较快则延时 */
    Bsp_CH375_Write_Data(dat);
 
}
unsigned char CH375_RD_DAT_PORT() {  /* 从CH375的数据端口读出数据,周期不小于1.5uS,如果单片机较快则延时 */

	return (Bsp_CH375_Read_Data() );
}
/* 设置USB模式 */
unsigned char set_usb_mode( unsigned char mode ) {  /* 设置CH37X的工作模式 */
	unsigned char i;
	CH375_WR_CMD_PORT( CMD_SET_USB_MODE );
	CH375_WR_DAT_PORT( mode );
	endp6_mode=endp7_mode=0x80;  /* 主机端复位USB数据同步标志 */
	for( i=0; i!=100; i++ ) {  /* 等待设置模式操作完成,不超过30uS */
		if ( CH375_RD_DAT_PORT()==CMD_RET_SUCCESS ) return( TRUE );  /* 成功 */
	}
	return( FALSE );  /* CH375出错,例如芯片型号错或者处于串口方式或者不支持 */
}
/* 设置USB通讯速度为低速，如果不设置，默认为全速 */
void set_freq(void)
{	
	CH375_WR_CMD_PORT(0x0b);    /* 切换使375B进入低速模式 */
	CH375_WR_DAT_PORT(0x17);
	CH375_WR_DAT_PORT(0xd8);
}

/* 数据同步 */
/* USB的数据同步通过切换DATA0和DATA1实现: 在设备端, CH372/CH375可以自动切换;
   在主机端, 必须由SET_ENDP6和SET_ENDP7命令控制CH375切换DATA0与DATA1.
   主机端的程序处理方法是为SET_ENDP6和SET_ENDP7分别提供一个全局变量,
   初始值均为80H, 每执行一次成功事务后将位6取反, 每执行一次失败事务后将其复位为80H. */
void toggle_recv() {  /* 主机接收成功后,切换DATA0和DATA1实现数据同步 */
	CH375_WR_CMD_PORT( CMD_SET_ENDP6 );
	CH375_WR_DAT_PORT( endp6_mode );
	endp6_mode^=0x40;
	delay2us();
}

void toggle_send() {  /* 主机发送成功后,切换DATA0和DATA1实现数据同步 */
	CH375_WR_CMD_PORT( CMD_SET_ENDP7 );
	CH375_WR_DAT_PORT( endp7_mode );
	endp7_mode^=0x40;
	delay2us();
}
/* 等待37X产生中断 */
unsigned char wait_interrupt() {          /* 主机端等待操作完成, 返回操作状态 */
	while( CH375_INT_WIRE );              /* 查询等待CH375操作完成中断(INT#低电平) */
	CH375_WR_CMD_PORT( CMD_GET_STATUS );  /* 产生操作完成中断, 获取中断状态 */
	return( CH375_RD_DAT_PORT() );
}
/* 读写内部缓冲区 */
unsigned char rd_usb_data( unsigned char *buf ) {  /* 从CH37X读出数据块 */
	unsigned char i, len;
	CH375_WR_CMD_PORT( CMD_RD_USB_DATA );  /* 从CH375的端点缓冲区读取接收到的数据 */
	len=CH375_RD_DAT_PORT();  /* 后续数据长度 */
	for ( i=0; i!=len; i++ ) *buf++=CH375_RD_DAT_PORT();
	return( len );
}
void wr_usb_data( unsigned char len, unsigned char *buf ) {  /* 向CH37X写入数据块 */
	CH375_WR_CMD_PORT( CMD_WR_USB_DATA7 );  /* 向CH375的端点缓冲区写入准备发送的数据 */
	CH375_WR_DAT_PORT( len );  /* 后续数据长度, len不能大于64 */
	while( len-- ) CH375_WR_DAT_PORT( *buf++ );
}
 /* 执行USB事务 */
unsigned char issue_token( unsigned char endpnum, unsigned char pid ) 
{ 
	CH375_WR_CMD_PORT( CMD_ISSUE_TOKEN );
	CH375_WR_DAT_PORT( (endpnum<<4)|pid  );  /* 高4位目的端点号, 低4位令牌PID */
	return (wait_interrupt());
}
/* 设置对响应NAK的重试次数 */
void set_retry(unsigned char num)
{	CH375_WR_CMD_PORT( CMD_SET_RETRY); 
	CH375_WR_DAT_PORT( 0x25);
	CH375_WR_DAT_PORT( num);
	delay2us();
}
/* 控制传输,被枚举过程调用 */
unsigned short SETUP_Transfer( unsigned char *DataBuf, unsigned short *plen)
{	
	unsigned char  l, s;
	unsigned short req_len, real_len = 0;
	unsigned char *p = DataBuf;
	endp7_mode=0x80;	//DATA0发送器
	endp6_mode=0xc0;	//DATA1接收器
	req_len = (unsigned short int)(CH_Request.Req_buf[7]<<8) | CH_Request.Req_buf[6];
	
/* SETUP阶段 */	
	wr_usb_data(8, CH_Request.Req_buf);	
	toggle_send();
	s = issue_token(0,DEF_USB_PID_SETUP );
	if(s != USB_INT_SUCCESS)		
	{	
		return(0);
	}	
/* DATA阶段 */	
	if(CH_Request.Req_buf[0]&0x80)		//IN数据
	{
		while(req_len)
		{		
			toggle_recv();
			s = issue_token(0 , DEF_USB_PID_IN);
			if( s == USB_INT_SUCCESS )
			{
				l = rd_usb_data( p );
				real_len += l;
				if( l<max_package )		//短包
				{
					break;
				}
				p += l;
				req_len -= l;
			}
			else return(0);
		}	
	}
	else							//OUT数据
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
	
/* 状态阶段 */
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
	return(s);		//成功
}

/* USB总线复位 */
void reset_device()
{	
/* USB规范中未要求在USB设备插入后必须复位该设备,但是计算机的WINDOWS总是这样做,所以有些USB设备也要求在插入后必须先复位才能工作 */
	set_usb_mode( 7 );  /* 复位USB设备,CH375向USB信号线的D+和D-输出低电平 */
	delayms(20);
	set_usb_mode( 6 );  /* 结束复位 */
	while ( wait_interrupt()!=USB_INT_CONNECT );  /* 等待复位之后的设备端再次连接上来 */

}
/* 使用获取描述符命令,参数1获取设备描述符，参数2获取配置描述符 */
/* 因为内部缓冲区只有64字节，所以如果设备描述符超过64字节，可能或发生溢出错误，所以本例程未使用此方式 */
unsigned char get_descr( unsigned char type ) {  /* 从设备端获取描述符 */
	unsigned char status;
	CH375_WR_CMD_PORT( CMD_GET_DESCR );
	CH375_WR_DAT_PORT( type );  /* 描述符类型, 只支持1(设备)或者2(配置) */
	status=wait_interrupt();  /* 等待CH375操作完成 */
	if ( status==USB_INT_SUCCESS ) {  /* 操作成功 */
		unsigned char i, len;
		len=rd_usb_data( data_buf );
		printf( "%s描述符是:", type==1?"设备":"配置" );
		for ( i=0; i!=len; i++ ) printf( "%02x ", (unsigned int)data_buf[i] );
		printf( "\n" );
	}
	return( status );
}
/* 设置USB地址 */
unsigned char set_addr( unsigned char addr ) {  /* 设置设备端的USB地址 */
	unsigned char status;
	CH375_WR_CMD_PORT( CMD_SET_ADDRESS );  /* 设置USB设备端的USB地址 */
	CH375_WR_DAT_PORT( addr );  /* 地址, 从1到127之间的任意值, 常用2到20 */
	status=wait_interrupt();  /* 等待CH375操作完成 */
	if ( status==USB_INT_SUCCESS ) {  /* 操作成功 */
		CH375_WR_CMD_PORT( CMD_SET_USB_ADDR );  /* 设置USB主机端的USB地址 */
		CH375_WR_DAT_PORT( addr );  /* 当目标USB设备的地址成功修改后,应该同步修改主机端的USB地址 */
	}
	return( status );
}
/* 设置配置 */
unsigned char set_config( unsigned char cfg ) {  /* 设置设备端的USB配置 */
	CH375_WR_CMD_PORT( CMD_SET_CONFIG );  /* 设置USB设备端的配置值 */
	CH375_WR_DAT_PORT( cfg );  /* 此值取自USB设备的配置描述符中 */
	return( wait_interrupt() );  /* 等待CH375操作完成 */
}
/* 分析配置描述符信息,并找到通信端点 */
void parse_config_descr(unsigned char *p)
{	
	unsigned char i,l;
	unsigned char s = 0;
	unsigned char ok = 0;
	num_interfaces=p[4];		//保存接口数
	config_value=p[5];		   //保存配置值
	for(i=0;i<p[2];i+=l)
	{
		if((p[i]==0x09)&&(p[i+1]==0x04))//接口描述符
		{
			s++;
			if((p[i+5]==0x03)&&(p[i+7]!=0x00))
				ok = 1;    
		}
		else if((p[i]==0x09)&&(p[i+1]==0x21))//HID描述符
		{
			if(s==1)
				report_descr0_len=p[i+7];    //REPORT描述符长度
			else
				report_descr1_len=p[i+7];    //REPORT描述符长度		
		}			
		else if((p[i]==0x07)&&(p[i+1]==0x05))//端点描述符
		{	
			if(ok)   
			{				
				if((p[i+3]==0x03) && (p[i+2]&0x80))
				{
					ok = 0;
					endp_int=p[i+2]&0x0f;       //中断端点号（只存储键鼠的上传端点）
				}
			}
			
		}	
		l = p[i];
	}	
}
/* 获取设备速度 */
unsigned char get_freq(void)
{		
	CH375_WR_CMD_PORT(0x0a);    /* 查询当前设备速度 */
	CH375_WR_DAT_PORT(0x07);
	return (CH375_RD_DAT_PORT());
}
unsigned char read_id(void)
{
	CH375_WR_CMD_PORT( CMD_GET_IC_VER);
	return (CH375_RD_DAT_PORT());
}
/* 主函数 */
void CH375_Control() 
{
	unsigned char i,s;
	unsigned short len;

	max_package = MAXSETUPPACK;
	CH374_PORT_INIT( ); //管脚初始化
    delayms(100);
	s = read_id();
	CH375_LogA("chip-id:0x%02x",(unsigned short)s);

	set_usb_mode( 5 );  /* 设置USB主机模式, 如果设备端是CH37X, 那么5和6均可 */
	CH375_LogA("wait connect...");
	while(1)
	{
		while ( wait_interrupt()!=USB_INT_CONNECT );  /* 等待设备端连接上来 */
		delayms(200);                    /* 等待连接稳定 */
/***** 复位检测USB设备速度 *****/
		CH375_LogA("check rate");
		s = get_freq();	
		reset_device();
        if( s&0x10 )	
		{
			CH375_LogA("low rate");
			set_freq();		        /* 切换使375B进入低速模式 */
		}
		else
			CH375_LogA("full rate");
		delayms(100);         //复位之后，相当于重连，必须有延时保证设备稳定
/***** 获取设备描述符 *****/
		get_descr(0x01);//获取设备描述符
		max_package = data_buf[7];  //端点0最大包大小

//		Request.Req.bmRequestType=0x80;
//		Request.Req.bRequest=0x06;
//		Request.Req.wValue=0x0001;      /* 因为51单片机是大端存储，否则写成0x0100,下面类似 */
//		Request.Req.wIndex=0x0000;
//		Request.Req.wLength=0x0800;
//		if( SETUP_Transfer(data_buf, &len) == USB_INT_SUCCESS )
//		{	
//			max_package = data_buf[7];  //端点0最大包大小
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
/***** 复位 *****/
		reset_device();
        if( s&0x10 )	set_freq();		/* 切换使375B进入低速模式 */
		delayms(100);                   //复位之后，相当于重连，必须有延时保证设备稳定
/***** 设置地址  *****/			
		set_addr(5);//设置地址
/***** 获取配置描述符 *****/
//		get_descr(0x02);//获取配置描述符
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
       /***** 分析配置描述符 *****/
		parse_config_descr(data_buf);   //保存描述符中一些值
       /***** 设置配置 *****/		
    	CH375_LogA("set config");
		set_config(config_value);     //设置配置
        /***** HID类命令 *****/	
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
		CH375_LogA("set report");		//对于键盘这一步，是点亮指示灯
		CH_Request.Req.bmRequestType=0x21;
		CH_Request.Req.bRequest=0x09;
		CH_Request.Req.wValue=0x0002;
		CH_Request.Req.wIndex=0x0000;
		CH_Request.Req.wLength=0x0100;	
		data_buf[0]=1;		
		if(SETUP_Transfer(data_buf, &len)!=USB_INT_SUCCESS)
			CH375_LogA("set report failed\n");	
       /* 设置对响应NAK的重试次数 */
		set_retry(3);  //超时重试3次，但收到NAK不重试
        /* 获取数据 */
		endp6_mode=0x80;  //复位同步标志
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
///* 主函数 */
//void main() 
//{
//	unsigned char i,s;
//	unsigned short len;

//	max_package = MAXSETUPPACK;
//	mInitSTDIO( );

//	s = read_id();
//	printf("chip-id:0x%02x\n",(unsigned short)s);

//	set_usb_mode( 5 );  /* 设置USB主机模式, 如果设备端是CH37X, 那么5和6均可 */
//	printf("wait connect...\n");
//	while(1)
//	{
//		while ( wait_interrupt()!=USB_INT_CONNECT );  /* 等待设备端连接上来 */
//		delayms(200);                    /* 等待连接稳定 */
///***** 复位检测USB设备速度 *****/
//		printf("check rate\n");
//		s = get_freq();	
//		reset_device();
//        if( s&0x10 )	
//		{
//			printf("low rate\n");
//			set_freq();		/* 切换使375B进入低速模式 */
//		}
//		else
//			printf("full rate\n");
//		delayms(100);         //复位之后，相当于重连，必须有延时保证设备稳定
///***** 获取设备描述符 *****/
//		get_descr(0x01);//获取设备描述符
//		max_package = data_buf[7];  //端点0最大包大小

////		Request.Req.bmRequestType=0x80;
////		Request.Req.bRequest=0x06;
////		Request.Req.wValue=0x0001;      /* 因为51单片机是大端存储，否则写成0x0100,下面类似 */
////		Request.Req.wIndex=0x0000;
////		Request.Req.wLength=0x0800;
////		if( SETUP_Transfer(data_buf, &len) == USB_INT_SUCCESS )
////		{	
////			max_package = data_buf[7];  //端点0最大包大小
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
///***** 复位 *****/
//		reset_device();
//        if( s&0x10 )	set_freq();		/* 切换使375B进入低速模式 */
//		delayms(100);                   //复位之后，相当于重连，必须有延时保证设备稳定
///***** 设置地址  *****/			
//		set_addr(5);//设置地址
///***** 获取配置描述符 *****/
////		get_descr(0x02);//获取配置描述符
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
///***** 分析配置描述符 *****/
//		parse_config_descr(data_buf);   //保存描述符中一些值
///***** 设置配置 *****/		
//    	printf("set config\n");
//		set_config(config_value);     //设置配置
///***** HID类命令 *****/	
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
//		printf("set report\n");		//对于键盘这一步，是点亮指示灯
//		Request.Req.bmRequestType=0x21;
//		Request.Req.bRequest=0x09;
//		Request.Req.wValue=0x0002;
//		Request.Req.wIndex=0x0000;
//		Request.Req.wLength=0x0100;	
//		data_buf[0]=1;		
//		if(SETUP_Transfer(data_buf, &len)!=USB_INT_SUCCESS)
//			printf("set report failed\n");	
///* 设置对响应NAK的重试次数 */
//		set_retry(3);  //超时重试3次，但收到NAK不重试
///* 获取数据 */
//		endp6_mode=0x80;  //复位同步标志
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
