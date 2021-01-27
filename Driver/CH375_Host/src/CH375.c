  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : CH375.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2021-01-08         
  * Description  :    
  * Function List:  
    1. ....
        <version>:       
          <staff>:
           <data>:
    <description>:  
    2. ...

  ******************************************************************************
  */
/********************************End of Head************************************/
#include "CH375.h"
#include "delay.h"
#include "TM_library.h" 
#include "exti.h"

CH375_USB_DEF   CH375_USB_Info;
USB_CFG_DESCR_LONG    CH_DESCR;
USB_DEV_DESCR         CH_HostDev;
DEVICE_INFO_    USBD;

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

void Error_Handler(void)
{
    CH375_Log("Error_Handler");
}

void CH375_IO_Init(void) 
{
//    CH375_DATA_OUTPUT();
//    CH375_DATA(0xFF);
//    TM_GPIO_SetPinHigh(CH375_A0_GPIO_Port,CH375_A0_Pin);
//    TM_GPIO_SetPinHigh(CH375_RD_GPIO_Port,CH375_RD_Pin);
//    TM_GPIO_SetPinHigh(CH375_WR_GPIO_Port,CH375_WR_Pin);
//    TM_GPIO_Init(CH375_A0_GPIO_Port,CH375_A0_Pin,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);
//    TM_GPIO_Init(CH375_RD_GPIO_Port,CH375_RD_Pin,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);    
//    TM_GPIO_Init(CH375_WR_GPIO_Port,CH375_WR_Pin,GPIO_Mode_Out_PP,GPIO_Speed_50MHz);    
//    ExitGPIO_Init(CH375_INT,2,3);
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

// 
 
void Bsp_CH375_Write_Cmd(uint8_t cmd ) /* ��CH375������˿�д������*/
{  
	Bsp_DelayUs(5);
	CH375_CS_0;
	CH375_A0_1;
	CH375_DATA(cmd);
	CH375_WR_0;
	Bsp_DelayUs(5);
	CH375_CS_1;
	CH375_WR_1;
}
 
void Bsp_CH375_Write_Data(uint8_t data ) /* ��CH375������˿�д������*/
{  
	Bsp_DelayUs(5);
	CH375_CS_0;
	CH375_A0_0;
	CH375_DATA(data);
	CH375_WR_0;
	Bsp_DelayUs(5);
	CH375_CS_1;
	CH375_WR_1;
	CH375_A0_1;
}
 
uint8_t Bsp_CH375_Read_Data(void)
{
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
 
/* �ȴ�37X�����ж� */
uint8_t Bsp_CH375_Get_INTStatus(void)
{
	uint8_t status;
	Bsp_CH375_Write_Cmd(CMD_GET_STATUS); 
	status = Bsp_CH375_Read_Data();
//	CH375_Log("USB interrupt status %d",status);
	return status;
}
 
uint8_t Bsp_CH375_Wait_INT(void)
{
	CH375_Log("Wait for USB interrupt...");
	while(CH375_INT_GPIO_Port->IDR & CH375_INT_Pin);
	return Bsp_CH375_Get_INTStatus();
}
//��ȡ�汾ID
uint8_t Bsp_CH375_Get_DeviceVersion(void)
{
	uint8_t version;
	Bsp_CH375_Write_Cmd(CMD_GET_IC_VER);
	Bsp_DelayUs(5);
	version = Bsp_CH375_Read_Data();
	version &= 0x7F;
	CH375_Log("USB version: 0x%X",version);
	return(version); 
}
//����CH375�Ĺ���ģʽ
bool Bsp_CH375_SetUSBMode(USB_Mode_Type mode) {  /* ����CH375�Ĺ���ģʽ */
	uint8_t i;
	Bsp_CH375_Write_Cmd( CMD_SET_USB_MODE );
	Bsp_CH375_Write_Data(mode);
	CH375_USB_Info.Endp6_Mode = CH375_USB_Info.Endp7_Mode = 0x80;
	Bsp_DelayUs(5);
	for( i=0; i!=10; i++ ) {  /* �ȴ�����ģʽ�������,������30uS */
		if( Bsp_CH375_Read_Data()==CMD_RET_SUCCESS ) 
		{
			CH375_USB_Info.USB_Mode = mode;
			CH375_Log("Set USB Mode %d success",mode);
			return( TRUE );  /* �ɹ� */
		}
	}
	CH375_Log("Set USB Mode faild");
	return(FALSE);  /* CH375����,����оƬ�ͺŴ���ߴ��ڴ��ڷ�ʽ���߲�֧�� */
}
 
/* ����ͬ�� */
/* USB������ͬ��ͨ���л�DATA0��DATA1ʵ��: ���豸��, CH372/CH375�����Զ��л�;
   ��������, ������SET_ENDP6��SET_ENDP7�������CH375�л�DATA0��DATA1.
   �����˵ĳ���������ΪSET_ENDP6��SET_ENDP7�ֱ��ṩһ��ȫ�ֱ���,
   ��ʼֵ��Ϊ80H, ÿִ��һ�γɹ������λ6ȡ��, ÿִ��һ��ʧ��������临λΪ80H. */
 
/* �������ճɹ���,�л�DATA0��DATA1ʵ������ͬ�� */
void BSP_CH375_Toggle_Recv(void) 
{  
	Bsp_CH375_Write_Cmd(CMD_SET_ENDP6);
	Bsp_CH375_Write_Data(CH375_USB_Info.Endp6_Mode);
	CH375_USB_Info.Endp6_Mode ^= 0x40;
	Bsp_DelayUs(2);
}
 
/* �������ͳɹ���,�л�DATA0��DATA1ʵ������ͬ�� */
void BSP_CH375_Toggle_Send(void) 
{  
	Bsp_CH375_Write_Cmd(CMD_SET_ENDP7);
	Bsp_CH375_Write_Data(CH375_USB_Info.Endp7_Mode);
	CH375_USB_Info.Endp7_Mode ^= 0x40;
	Bsp_DelayUs(2);
}
 
/* ��������ʧ�ܺ�,��λ�豸�˵�����ͬ����DATA0 */
uint8_t Bsp_CH375_ClrStall6(void) 
{  
	Bsp_CH375_Write_Cmd( CMD_CLR_STALL );
	Bsp_CH375_Write_Data( CH375_EndPointNum | 0x80 );  /* ����豸�˲���CH37XоƬ,��ô��Ҫ�޸Ķ˵�� */
	CH375_USB_Info.Endp6_Mode = 0x80;
	return( Bsp_CH375_Wait_INT() );
}
 
/* ��������ʧ�ܺ�,��λ�豸�˵�����ͬ����DATA0 */
uint8_t Bsp_CH375_ClrStall7(void) 
{  
	Bsp_CH375_Write_Cmd( CMD_CLR_STALL );
	Bsp_CH375_Write_Data( CH375_EndPointNum );  /* ����豸�˲���CH37XоƬ,��ô��Ҫ�޸Ķ˵�� */
	CH375_USB_Info.Endp7_Mode=0x80;
	return( Bsp_CH375_Wait_INT() );
}
 
/* ���ݶ�д, ��Ƭ����дCH372����CH375оƬ�е����ݻ����� */
 
/* ��CH37X�������ݿ� */
uint8_t Bsp_CH375_Read_USBData( uint8_t *buffer ) 
{
	uint8_t  i, len;
	Bsp_CH375_Write_Cmd( CMD_RD_USB_DATA );  /* ��CH375�Ķ˵㻺������ȡ���յ������� */
	len = Bsp_CH375_Read_Data();  /* �������ݳ��� */
	for ( i=0; i!=len; i++ ) 
		*buffer++ = Bsp_CH375_Read_Data();
	return( len );
}
 
/* ��CH37Xд�����ݿ� */
void Bsp_CH375_Write_USBData( uint8_t len, uint8_t *buffer ) 
{  
	Bsp_CH375_Write_Cmd( CMD_WR_USB_DATA7 );  /* ��CH375�Ķ˵㻺����д��׼�����͵����� */
	Bsp_CH375_Write_Data( len );  /* �������ݳ���, len���ܴ���64 */
	while( len-- ) 
		Bsp_CH375_Write_Data( *buffer++ );
}
 
/* �������� */
 
/* ִ��USB���� */
uint8_t Bsp_CH375_IssueToken(uint8_t endp_and_pid) 
{  
	/* ִ����ɺ�, �������ж�֪ͨ��Ƭ��, �����USB_INT_SUCCESS��˵�������ɹ� */
	uint8_t status;
	Bsp_CH375_Write_Cmd(CMD_ISSUE_TOKEN);
	Bsp_CH375_Write_Data(endp_and_pid);   /* ��4λĿ�Ķ˵��, ��4λ����PID */
	status=Bsp_CH375_Wait_INT();          /* �ȴ�CH375������� */
	if ( status!=USB_INT_SUCCESS && status!=USB_INT_USB_READY && ( endp_and_pid&0xF0) == (CH375_EndPointNum << 4) ) /* ����ʧ��,����豸�˲���CH37XоƬ,��ô��Ҫ�޸Ķ˵�� */
	{  
		if ( (endp_and_pid&0x0F) == DEF_USB_PID_OUT ) 
			Bsp_CH375_ClrStall7();  /* ��λ�豸�˽��� */
		else if ( (endp_and_pid&0x0F) == DEF_USB_PID_IN ) 
			Bsp_CH375_ClrStall6();  /* ��λ�豸�˷��� */
	}
	return( status );
}
 
/* �������� */
void Bsp_CH375_Host_Send( uint8_t len, uint8_t *buffer ) 
{  
	Bsp_CH375_Write_USBData( len, buffer );
	BSP_CH375_Toggle_Send();
	if ( Bsp_CH375_IssueToken( ( CH375_EndPointNum << 4 ) | DEF_USB_PID_OUT )!=USB_INT_SUCCESS ) 
		Error_Handler();  /* ����豸�˲���CH37XоƬ,��ô��Ҫ�޸Ķ˵�� */
}
 
/* ��������, ���س��� */
uint8_t Bsp_CH375_Host_Recv( uint8_t *buffer ) 
{
	BSP_CH375_Toggle_Recv();
	if ( Bsp_CH375_IssueToken( ( CH375_EndPointNum << 4 ) | DEF_USB_PID_IN )!=USB_INT_SUCCESS ) 
		Error_Handler();  /* ����豸�˲���CH37XоƬ,��ô��Ҫ�޸Ķ˵�� */
	return( Bsp_CH375_Read_USBData( buffer ) );
}
 
 /* ���豸�˻�ȡ������ */
uint8_t Bsp_CH375_Get_USBDescr( uint8_t type ) 
{ 
	uint8_t status;
	Bsp_CH375_Write_Cmd( CMD_GET_DESCR );
	Bsp_CH375_Write_Data( type );   /* ����������, ֻ֧��1(�豸)����2(����) */
	status = Bsp_CH375_Wait_INT();  /* �ȴ�CH375������� */
	return( status );
}
/* �����豸�˵�USB��ַ */
uint8_t Bsp_CH375_SetAddr( uint8_t addr ) 
{  
	uint8_t status;
	Bsp_CH375_Write_Cmd( CMD_SET_ADDRESS );  /* ����USB�豸�˵�USB��ַ */
	Bsp_CH375_Write_Data( addr );  /* ��ַ, ��1��127֮�������ֵ, ����2��20 */
	status=Bsp_CH375_Wait_INT();  /* �ȴ�CH375������� */
	/* �����ɹ� */
	if ( status==USB_INT_SUCCESS ) 
	{  
		Bsp_CH375_Write_Cmd( CMD_SET_USB_ADDR );  /* ����USB�����˵�USB��ַ */
		Bsp_CH375_Write_Data( addr );             /* ��Ŀ��USB�豸�ĵ�ַ�ɹ��޸ĺ�,Ӧ��ͬ���޸������˵�USB��ַ */
	}
	return( status );
}
 
/* �����豸�˵�USB���� */
uint8_t Bsp_CH375_SetConfig( uint8_t cfg ) 
{  
	CH375_USB_Info.Endp6_Mode = CH375_USB_Info.Endp7_Mode = 0x80;  /* ��λUSB����ͬ����־ */
	Bsp_CH375_Write_Cmd( CMD_SET_CONFIG );  /* ����USB�豸�˵�����ֵ */
	Bsp_CH375_Write_Data( cfg );  /* ��ֵȡ��USB�豸�������������� */
	return( Bsp_CH375_Wait_INT() );  /* �ȴ�CH375������� */
}
 
/* ���ö���ӦNAK�����Դ��� */
void Bsp_CH375_Set_Retry(uint8_t num)
{	
	Bsp_CH375_Write_Cmd( CMD_SET_RETRY); 
	Bsp_CH375_Write_Data( 0x25);
	Bsp_CH375_Write_Data( num);
	Bsp_DelayUs(2);
}
 
/* USB���߸�λ */
bool Bsp_CH375_Reset_Device(void)
{
	/* USB�淶��δҪ����USB�豸�������븴λ���豸,���Ǽ������WINDOWS����������,������ЩUSB�豸ҲҪ���ڲ��������ȸ�λ���ܹ��� */
	if(Bsp_CH375_SetUSBMode( RstBUS_Host ) == FALSE)return FALSE;  /* ��λUSB�豸,CH375��USB�ź��ߵ�D+��D-����͵�ƽ */
	Bsp_DelayMs(10);
	if(Bsp_CH375_SetUSBMode( AutoSOF_Host ) == FALSE)return FALSE;  /* ������λ */
	Bsp_DelayMs(100);
	return TRUE;
}

/* USB���߸�λ �� ��ȡ�ٶȲ��� */
uint8_t Bsp_CH375_GetFreq(void)
{
	uint8_t res;
	res = Bsp_CH375_SetUSBMode( 5 );
	if(res != USB_INT_SUCCESS ) return ( USB_CMD_ERR );
	Bsp_CH375_Write_Cmd( CMD_GET_DEV_RATE );   
	Bsp_CH375_Write_Data( 0x07 );
	res = Bsp_CH375_Read_Data();
	if( res & 0x10 )
		USBD.speed = LOW_SPEED;
	else
		USBD.speed = FULL_SPEED;
	return ( res );
}

/********************************End of File************************************/
void  USBH_ParseDevDesc (USB_DEV_DESCR* dev_desc,
                                uint8_t *buf, 
                                uint16_t length)
{
  dev_desc->bLength            = *(uint8_t  *) (buf +  0);
  dev_desc->bDescriptorType    = *(uint8_t  *) (buf +  1);
  dev_desc->bcdUSB             = LE16 (buf +  2);
  dev_desc->bDeviceClass       = *(uint8_t  *) (buf +  4);
  dev_desc->bDeviceSubClass    = *(uint8_t  *) (buf +  5);
  dev_desc->bDeviceProtocol    = *(uint8_t  *) (buf +  6);
  dev_desc->bMaxPacketSize0    = *(uint8_t  *) (buf +  7);
 
  CH375_Log(">DevDesc_1 %02X %02X %04X %02X %02X %02X %02X",dev_desc->bLength         ,   
                                                             dev_desc->bDescriptorType ,
                                                             dev_desc->bcdUSB          ,
                                                             dev_desc->bDeviceClass    ,
                                                             dev_desc->bDeviceSubClass ,
                                                             dev_desc->bDeviceProtocol ,
                                                             dev_desc->bMaxPacketSize0 );    
    
  if (length > 8)
  { /* For 1st time after device connection, Host may issue only 8 bytes for 
    Device Descriptor Length  */
    dev_desc->idVendor           = LE16 (buf +  8);
    dev_desc->idProduct          = LE16 (buf + 10);
    dev_desc->bcdDevice          = LE16 (buf + 12);
    dev_desc->iManufacturer      = *(uint8_t  *) (buf + 14);
    dev_desc->iProduct           = *(uint8_t  *) (buf + 15);
    dev_desc->iSerialNumber      = *(uint8_t  *) (buf + 16);
    dev_desc->bNumConfigurations = *(uint8_t  *) (buf + 17);
    CH375_Log(">DevDesc_2 %04X %04X %04X %02X %02X %02X %02X",dev_desc->idVendor           ,   
                                                               dev_desc->idProduct          ,
                                                               dev_desc->bcdDevice          ,
                                                               dev_desc->iManufacturer      ,
                                                               dev_desc->iProduct           ,
                                                               dev_desc->iSerialNumber      ,
                                                               dev_desc->bNumConfigurations); 
  }
}

USBH_DescHeader_t  *USBH_GetNextDesc (uint8_t   *pbuf, uint16_t  *ptr)
{
  USBH_DescHeader_t  *pnext;
 
  *ptr += ((USBH_DescHeader_t *)pbuf)->bLength;  
  pnext = (USBH_DescHeader_t *)((uint8_t *)pbuf + \
         ((USBH_DescHeader_t *)pbuf)->bLength);
 
  return(pnext);
}

void  USBH_ParseInterfaceDesc (USB_ITF_DESCR *if_descriptor, uint8_t *buf)
{
  if_descriptor->bLength            = *(uint8_t  *) (buf + 0);
  if_descriptor->bDescriptorType    = *(uint8_t  *) (buf + 1);
  if_descriptor->bInterfaceNumber   = *(uint8_t  *) (buf + 2);
  if_descriptor->bAlternateSetting  = *(uint8_t  *) (buf + 3);
  if_descriptor->bNumEndpoints      = *(uint8_t  *) (buf + 4);
  if_descriptor->bInterfaceClass    = *(uint8_t  *) (buf + 5);
  if_descriptor->bInterfaceSubClass = *(uint8_t  *) (buf + 6);
  if_descriptor->bInterfaceProtocol = *(uint8_t  *) (buf + 7);
  if_descriptor->iInterface         = *(uint8_t  *) (buf + 8);
    
   CH375_Log(">Desc = %02X %02X %02X %02X %02X %02X %02X %02X %02X",if_descriptor->bLength            , 
                                                                     if_descriptor->bDescriptorType    ,
                                                                     if_descriptor->bInterfaceNumber   ,
                                                                     if_descriptor->bAlternateSetting  ,
                                                                     if_descriptor->bNumEndpoints      ,
                                                                     if_descriptor->bInterfaceClass    ,
                                                                     if_descriptor->bInterfaceSubClass ,
                                                                     if_descriptor->bInterfaceProtocol ,
                                                                     if_descriptor->iInterface         
    ); 
    
}
void  USBH_ParseHID_Desc (USB_HID_DESCR*hid_descr, uint8_t *buf)
{
  hid_descr->bLength          = *(uint8_t  *) (buf + 0);
  hid_descr->bDescriptorType1 = *(uint8_t  *) (buf + 1);
  hid_descr->bcdHID           = LE16 (buf + 2);
  hid_descr->bCountryCode     = *(uint8_t  *) (buf + 4);
  hid_descr->bNumDescriptors  = *(uint8_t  *) (buf + 5);
  hid_descr->bDescriptorType2 = *(uint8_t  *) (buf + 6);
  hid_descr->wItemLength      = *(uint8_t  *) (buf + 7);
  hid_descr->iInterface       = *(uint8_t  *) (buf + 8);

    
   CH375_Log(">HIDDesc = %02X %02X %04X %02X %02X %02X %02X %02X ", hid_descr->bLength          , 
                                                                     hid_descr->bDescriptorType1 ,
                                                                     hid_descr->bcdHID           ,
                                                                     hid_descr->bCountryCode     ,
                                                                     hid_descr->bNumDescriptors  ,
                                                                     hid_descr->bDescriptorType2 ,
                                                                     hid_descr->wItemLength      ,
                                                                     hid_descr->iInterface      
                                                                      
    ); 
    
}
void  USBH_ParseEPDesc (USB_ENDP_DESCR  *ep_descriptor, 
                               uint8_t *buf)
{
  
  ep_descriptor->bLength          = *(uint8_t  *) (buf + 0);
  ep_descriptor->bDescriptorType  = *(uint8_t  *) (buf + 1);
  ep_descriptor->bEndpointAddress = *(uint8_t  *) (buf + 2);
  ep_descriptor->bmAttributes     = *(uint8_t  *) (buf + 3);
  ep_descriptor->wMaxPacketSize   = LE16 (buf + 4);
  ep_descriptor->bInterval        = *(uint8_t  *) (buf + 6);
  CH375_Log(">EPDesc %02X %02X %02X %02X %02X %02X ",ep_descriptor->bLength            ,
                                                     ep_descriptor->bDescriptorType  ,
                                                     ep_descriptor->bEndpointAddress ,
                                                     ep_descriptor->bmAttributes     ,
                                                     ep_descriptor->wMaxPacketSize   ,
                                                     ep_descriptor->bInterval );       
    
}

USBH_Status USBH_ParseCfgDesc (USB_CFG_DESCR* cfg_desc,USB_ITF_DESCR* itf_desc,USB_ENDP_DESCR ep_desc[][2], USB_HID_DESCR *hid_descr,uint8_t *buf, uint16_t length)
{  
  USB_ITF_DESCR    *pif ;
  USB_ITF_DESCR    temp_pif ;  
  USB_ENDP_DESCR           *pep;  
  USBH_DescHeader_t             *pdesc = (USBH_DescHeader_t *)buf;
  uint16_t                      ptr;
  int8_t                        if_ix = 0;
  int8_t                        ep_ix = 0;  
  static uint16_t               prev_ep_size = 0;
  static uint8_t                prev_itf = 0;  
  USBH_Status status;
  
  
  pdesc   = (USBH_DescHeader_t *)buf;
  
  /* Parse configuration descriptor */
  cfg_desc->bLength             = *(uint8_t  *) (buf + 0);
  cfg_desc->bDescriptorType     = *(uint8_t  *) (buf + 1);
  cfg_desc->wTotalLength        = LE16 (buf + 2);
  cfg_desc->bNumInterfaces      = *(uint8_t  *) (buf + 4);
  cfg_desc->bConfigurationvalue = *(uint8_t  *) (buf + 5);
  cfg_desc->iConfiguration      = *(uint8_t  *) (buf + 6);
  cfg_desc->bmAttributes        = *(uint8_t  *) (buf + 7);
  cfg_desc->MaxPower            = *(uint8_t  *) (buf + 8);    
  
  CH375_Log(">cfg_desc %02X %02X %04X %02X %02X %02X %02X %02X",cfg_desc->bLength             ,  
                                                                cfg_desc->bDescriptorType     ,
                                                                cfg_desc->wTotalLength        ,
                                                                cfg_desc->bNumInterfaces      ,
                                                                cfg_desc->bConfigurationvalue ,
                                                                cfg_desc->iConfiguration      ,
                                                                cfg_desc->bmAttributes        ,
                                                                cfg_desc->MaxPower           );
  if (length > 9)
  {
    ptr = 0x09; //Desc //��ʼλ��
    
    if ( cfg_desc->bNumInterfaces <= 2) 
    {
      pif = (USB_ITF_DESCR *)0;
      while (ptr < cfg_desc->wTotalLength ) //��û��������
      {
        pdesc = USBH_GetNextDesc((uint8_t *)pdesc, &ptr);           //������һ��
        if (pdesc->bDescriptorType   == 4) //�����豸����==0x04
        {
          if_ix             = *(((uint8_t *)pdesc ) + 2);  //�˵���Ŀ
          pif               = &itf_desc[if_ix];            //
          if((*((uint8_t *)pdesc + 3)) < 3)
          {
            USBH_ParseInterfaceDesc (&temp_pif, (uint8_t *)pdesc);  //�����豸����          
            ep_ix = 0;
            
            /* Parse Ep descriptors relative to the current interface */
            if(temp_pif.bNumEndpoints <= 4)
            {          
              while (ep_ix < temp_pif.bNumEndpoints) 
              {
                pdesc = USBH_GetNextDesc((void* )pdesc, &ptr);      //������һ��
                if (pdesc->bDescriptorType   == 5)                  //����==0x05(�˵�����)
                {  
                  pep               = &ep_desc[if_ix][ep_ix];
                  if(prev_itf != if_ix)
                  {
                    prev_itf = if_ix;
                    USBH_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif); 
                    CH375_Logn("<1>");
                  }
                  else
                  {
                    if(prev_ep_size > LE16((uint8_t *)pdesc + 4))
                    {
                      break;
                    }
                    else
                    {
                      USBH_ParseInterfaceDesc (pif, (uint8_t *)&temp_pif);    
                      CH375_Logn("<2>");
                    }
                  }
                  USBH_ParseEPDesc (pep, (uint8_t *)pdesc);
                  prev_ep_size = LE16((uint8_t *)pdesc + 4);
                  ep_ix++;
                }else if(pdesc->bDescriptorType == 0x21){    //HID�豸����
                    
                   USB_HID_DESCR * HDesc = (hid_descr+sizeof(USB_HID_DESCR)*if_ix);
                   USBH_ParseHID_Desc(HDesc, (uint8_t *)pdesc);
                   CH375_Logn("HID�豸����:");
                   uint8_t * pstrn = (uint8_t *)pdesc;
                   for(u8 i=0;i<pdesc->bLength;i++)
                    {
                      CH375_Logn("%02X ",pstrn[i]);
                    }
                    CH375_Logn("\r\n");  
                }else{
                   CH375_Logn("δ��������:");
                   uint8_t * pstrn = (uint8_t *)pdesc;
                   for(u8 i=0;i<pdesc->bLength;i++)
                    {
                      CH375_Logn("%02X ",pstrn[i]);
                    }
                    CH375_Logn("\r\n");                
                }
              }
            }
            else /*num endpoints exceeded */
            {
              CH375_Log("endpoints USBH_NOT_SUPPORTED");
              status = USBH_NOT_SUPPORTED;
              return status;
            } 
          }
        }
      }
    }
    else /*num interfaces exceeded */
    {
      status = USBH_NOT_SUPPORTED;
      return status;
    } 
    prev_ep_size = 0;
    prev_itf = 0; 
  }
  return USBH_OK ;
}