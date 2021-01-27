/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2021,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : CH375_Device.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2021-01-08
* Description  :
* Function List:
******************************************************************************
*/
/********************************End of Head************************************/
#include "CH375_Device.h"

#define     STATUS_DISCONNECT       0   /* U����δ���ӻ����Ѿ��Ͽ� */
#define     STATUS_CONNECT          1   /* U�̸ո����� */
#define     STATUS_ERROR            2   /* U�̲���������߲�֧�� */
#define     STATUS_WAIT             3   /* U�����ڲ��� */
#define     STATUS_READY            4   /* U��׼���ý��ܲ��� */

/*
  ������USB�ƶ��洢�豸�ļ�Ӧ�òο����ڵ�Ƭ��Ӧ�ó����У�
  ���Զ���USB�洢�豸��4��״̬���ѶϿ��������ӡ���ʼ�����ɶ�д������"������"״̬��ѡ��
�� �ѶϿ���ָUSB�洢�豸�Ѿ��Ͽ�����USB�����аγ�������ȫ�����ã�
�� ��������ָUSB�洢�豸�Ѿ����ӣ�����USB�����У���������δ��ʼ����
�� ��ʼ����ָUSB�洢�豸���ڳ�ʼ�����߳�ʼ��ʧ�ܣ����Բ����Խ��ж�д��
�� �ɶ�д��ָUSB�洢�豸��ʼ���ɹ������Խ������ݶ�д��

��Ƭ������CH375�жϵ�һ�㲽�����£�
�� CH375��INT#��������Ϊ�͵�ƽ����Ƭ�������жϣ�
�� ��Ƭ�������жϷ����������ִ��GET_STATUS�����ȡ�ж�״̬��
�� CH375��GET_STATUS������ɺ�INT#���Żָ�Ϊ�ߵ�ƽ��ȡ���ж�����
�� ��Ƭ�����Բο������˳�����GET_STATUS�����ȡ���ж�״̬��
  �� ����ж�״̬��USB_INT_DISCONNECT����˵��USB�豸�Ѿ��Ͽ�����USB�洢�豸״̬Ϊ"�ѶϿ�"��
     ����ڴ�֮ǰUSB�洢�豸�ж�д������δ��ɣ������������������ʧ�ܴ���Ȼ���˳��жϣ���Ҫʱ����֪ͨ��Ƭ��������
  �� ����ж�״̬��USB_INT_CONNECT����˵��USB�豸�Ѿ����ӣ���USB�洢�豸״̬Ϊ"������"��
     ���ŷ���DISK_INIT�������USB�洢�豸״̬Ϊ��ʼ����Ȼ���˳��жϣ���Ҫʱ����֪ͨ��Ƭ��������
  �� �����ǰ��USB�洢�豸״̬��"��ʼ��"��
    �� ����ж�״̬��USB_INT_SUCCESS����˵��USB�洢�豸��ʼ���ɹ�����USB�洢�豸״̬Ϊ"�ɶ�д"��
       Ȼ���˳��жϣ���Ҫʱ����֪ͨ��Ƭ��������
    �� ����ж�״̬������״̬����˵��USB�洢�豸��ʼ��ʧ�ܣ�Ӧ��֪ͨ��Ƭ��������
       ��ʾ��USB�豸���Ǵ洢�豸���߸�USB�豸��֧�֣�Ȼ���˳��жϡ����ߣ���Ƭ��ͨ�������������д����USB�洢�豸��ͨѶЭ�顣
  �� �����ǰ��USB�洢�豸״̬��"�ɶ�д"������Ա����ж�״̬��֪ͨ��Ƭ����������
     Ȼ��ֱ���˳��жϣ����ߣ����жϷ�������м�������
    �� ����ж�״̬��USB_INT_DISK_READ����˵�����ڽ���USB�洢�豸�Ķ���������Ҫȡ��64���ֽڵ����ݣ�
       ���Է���RD_USB_DATA����ȡ�����ݣ������ٷ���DISK_RD_GO����ʹCH375��������Ȼ���˳��жϡ�
    �� ����ж�״̬��USB_INT_DISK_WRITE����˵�����ڽ���USB�洢�豸��д��������Ҫ�ṩ64���ֽڵ����ݣ�
       ���Է���WR_USB_DATA7�����ṩ���ݣ������ٷ���DISK_WR_GO����ʹCH375����д��Ȼ���˳��жϡ�
    �� ����ж�״̬��USB_INT_SUCCESS����˵����д�����ɹ���ֱ���˳��жϲ�֪ͨ������ò����ɹ���
    �� ����ж�״̬��USB_INT_DISK_ERR����˵����д����ʧ�ܣ�ֱ���˳��жϲ�֪ͨ������ò���ʧ�ܣ�
    �� ͨ�����᷵�������ж�״̬������У���˵�����ִ���
  �� ͨ������������USB�洢�豸״̬�·��������ж�״̬������У���˵�����ִ��󣬿��Բ���USB�豸�Ͽ���״̬����

����Ƭ����������Ҫ��USB�洢�豸�ж�д����ʱ�����Բ�ѯUSB�洢�豸״̬��
�����"�ɶ�д"״̬������Է���DISK_READ��������ݣ����߷���DISK_WRITE����д���ݡ�
���������ݶ�д���̿����ڵ�Ƭ�����жϷ����������ɣ�Ҳ�������жϳ����ñ�־֪ͨ�ȴ��е�������
������������ж�״̬��������ݶ�д���̡�
*/

#include "ch375.h"
#include "delay.h"
#include "string.h"

#define Bsp_DelayUs delay_us
#define Bsp_DelayMs delay_ms
#define delayms     delay_ms

uint8_t MaxBuffSize = 8;
uint8_t Rx_Buf[128] = {0};  //������
uint8_t ep6_mode = 0x80, ep7_mode = 0x80; //ͬ����־λ��ֵ
void Bsp_CH375_HOST_Init(void);

union _REQUEST { //������ṹ
    struct {
        unsigned char bmRequestType; //��������
        unsigned char bRequest;      //
        unsigned int wValue;
        unsigned int wIndex;
        unsigned int wLength;
    } Req;
    unsigned char Req_buf[8];
} USB_Request;

u8 CH375_HostDesc_Init(void)
{
    static uint8_t Port  = 0;
    static uint8_t state = USBH_BUSY;
    PUSB_DEV_DESCR          p_dev_descr;
    PUSB_CFG_DESCR_LONG     p_cfg_descr;
    uint8_t status, len, c;
    p_dev_descr = (PUSB_DEV_DESCR)CH375_USB_Info.USB_Buffer;
    p_cfg_descr = (PUSB_CFG_DESCR_LONG)CH375_USB_Info.USB_Buffer;

    switch(Port)
    {
        case 0:  /* ��ȡ�豸������ */
        {
            status = Bsp_CH375_Get_USBDescr(1); 
            if(status == USB_INT_SUCCESS)
            {
               CH375_Log("��ȡ�豸������OK");                 
               len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
               if(len < 18 || p_dev_descr->bDescriptorType != 1) {
                   CH375_Log("�������:���������ȴ���������ʹ���");
               }  
               USBH_ParseDevDesc(&CH_HostDev,CH375_USB_Info.USB_Buffer,len); //��ʾ�豸���� 
               if(p_dev_descr->bDeviceClass != 0) {
                   CH375_Log("���ӵ�USB�豸����HID�豸,���߲�����USB�淶 ");
                    Port = 0;
                   return (USBH_FAIL);
               }               
               Port = 1;               
            }else{
               CH375_Log("��ȡ�豸������Err"); 
                Port = 0;
               return  (USBH_FAIL);             
            }                
        }break;
        case 1: /* ���õ�ַ */
        {
            status = Bsp_CH375_SetAddr(2);
            if(status == USB_INT_SUCCESS) {
              CH375_Log("����HID�豸��USB��ַ�ɹ�");
              Port = 2;     
            }else{
              Port = 0;
              return  (USBH_FAIL);              
            }
        }break;
        case 2: /* ��ȡ���������� */
        {
            status = Bsp_CH375_Get_USBDescr(2);
            if(status == USB_INT_SUCCESS) {
                CH375_Log("��ȡ�����������ɹ�");
                len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
                USBH_ParseCfgDesc(&CH_DESCR.cfg_descr,&CH_DESCR.itf_descr,CH_DESCR.endp_descr,CH_DESCR.hid_descr,CH375_USB_Info.USB_Buffer,len ); //����������
                if(p_cfg_descr->itf_descr.bInterfaceClass != 3 || p_cfg_descr->itf_descr.bInterfaceSubClass != 0) {
                    Port = 0;
                    return(USBH_FAIL);
                }
                Port = 3;
             
            }else{
                Port = 0;
                return(USBH_FAIL);            
            }
        }break;  
        case 3:
        {
             status = Bsp_CH375_SetAddr(1);
             if(status == USB_INT_SUCCESS){
                return(USBH_OK);               
             }else{
                Port = 0;
                return USBH_FAIL;             
             }
        }break;            
    }
    if(Bsp_CH375_Get_INTStatus() == USB_INT_DISCONNECT)  
    {

    }
    return state;

        



                
//                for(u8 i=0;i<len;i++)
//                {
//                    if(i%9==0)CH375_Logn("\r\n");
//                    CH375_Logn("%02X ",CH375_USB_Info.USB_Buffer[i]);
//                }
//                CH375_Logn("\r\n");
//                if(p_cfg_descr->itf_descr.bInterfaceClass != 3 || p_cfg_descr->itf_descr.bInterfaceSubClass != 0) {
//                    CH375_Log("����:����HID�豸���߲�����USB�淶");
//                    return(UNKNOWN_USB_HID);
//                }
//                endp_out_addr = endp_in_addr = 0;
//                c = p_cfg_descr->endp_descr[0].bEndpointAddress;
//                if(c & 0x80) {
//                    endp_in_addr = c & 0x0f;
//                    CH375_Log("IN1�˵��ַ:%d", endp_in_addr);
//                } else {
//                    endp_out_addr = c & 0x0f;
//                    endp_out_size = p_cfg_descr->endp_descr[0].wMaxPacketSize;
//                    CH375_Log("OUT1�˵��ַ:%d, ��󳤶�:%d", endp_out_addr, endp_out_size);
//                }
//                if(p_cfg_descr->itf_descr.bNumEndpoints >= 2) {
//                    if(p_cfg_descr->endp_descr[1].bDescriptorType == 5) {
//                        c = p_cfg_descr->endp_descr[1].bEndpointAddress;
//                        if(c & 0x80) {
//                            endp_in_addr = c & 0x0f;
//                            CH375_Log("IN2�˵��ַ:%d", endp_in_addr);
//                        } else {
//                            endp_out_addr = c & 0x0f;
//                            endp_out_size = p_cfg_descr->endp_descr[1].wMaxPacketSize;
//                            CH375_Log("OUT2�˵��ַ:%d, ��󳤶�:%d", endp_out_addr, endp_out_size);
//                        }
//                    }
//                }
//                if(p_cfg_descr->itf_descr.bInterfaceProtocol <= 1) {
//                    endp_out_addr = 0;
//                }
//                if(endp_in_addr == 0) {
//                    return(UNKNOWN_USB_HID);
//                }
//                status = Bsp_CH375_SetConfig(p_cfg_descr->cfg_descr.bConfigurationvalue);//�����豸�˵�USB���� 
//                if(status == USB_INT_SUCCESS) {
//                    Bsp_CH375_Set_Retry(0x89);
//                }
//            }
//        }
//    }
//    return (status);

}



/* ���ƴ���,��ö�ٹ��̵��� */
unsigned short CH375_SETUP_Transfer(unsigned char *DataBuf, unsigned short *plen)
{
    unsigned char  len, state;
    unsigned short req_len, real_len = 0;
    unsigned char *p = DataBuf;
    ep7_mode = 0x80; //DATA0������
    ep6_mode = 0xc0; //DATA1������
    req_len = (unsigned short int)(USB_Request.Req_buf[7] << 8) | USB_Request.Req_buf[6];
    /* SETUP�׶� */
    Bsp_CH375_Write_USBData(8, USB_Request.Req_buf);
    BSP_CH375_Toggle_Send();                          //�������ͳɹ���,�л�DATA0��DATA1ʵ������ͬ��
    state = Bsp_CH375_IssueToken(DEF_USB_PID_SETUP);      //��4λĿ�Ķ˵��, ��4λ����PID
    if(state != USB_INT_SUCCESS) {
        return(0);
    }
    /* DATA�׶� */
    if(USB_Request.Req_buf[0] & 0x80) {   //IN����
        while(req_len) {
            BSP_CH375_Toggle_Recv();
            state = Bsp_CH375_IssueToken(DEF_USB_PID_IN);
            if(state == USB_INT_SUCCESS) {
                len = Bsp_CH375_Read_USBData(p);
                real_len += len;
                if(len < MaxBuffSize) {      //�̰�
                    break;
                }
                p += state;
                req_len -= state;
            } else {
                return(0);
            }
        }
    } else {                        //OUT����
        while(req_len) {
            len = (req_len > MaxBuffSize) ? MaxBuffSize : req_len;
            Bsp_CH375_Write_USBData(len, p);
            BSP_CH375_Toggle_Send();
            state =  Bsp_CH375_IssueToken(DEF_USB_PID_OUT);
            if(state == USB_INT_SUCCESS) {
                real_len += len;
                p += len;
                req_len -= len;
            } else {
                return(0);
            }
        }
    }
    /* ״̬�׶� */
    ep7_mode = ep6_mode = 0xc0; //DATA1
    if(USB_Request.Req_buf[0] & 0x80) {
        Bsp_CH375_Write_USBData(0, USB_Request.Req_buf);
        BSP_CH375_Toggle_Send();
        state = Bsp_CH375_IssueToken(DEF_USB_PID_OUT);
    } else {
        BSP_CH375_Toggle_Recv();
        state = Bsp_CH375_IssueToken(DEF_USB_PID_IN);
    }
    if(state != USB_INT_SUCCESS) {
        return(0);
    }
    *plen = real_len;
    return(state);      //�ɹ�
}




typedef enum {
    HOST_IDLE = 0,           /*��ʼ״̬*/
    Host_TEST_HARDWARE,      /*����Ӳ��*/
    Host_GETHARDWAREVERSION, /*��ȡӲ���汾*/
    Host_WAIT_CONNECT      , /*�ȴ�����*/    
    Host_INIT,               /*��ʼ��*/
    Host_Get_USB_DESC,       /*��ȡ�豸������*/
    Host_Get_USB_CFGDESC,    /*��ȡ�豸������*/
    
} Host_State;

u8 HostPort = HOST_IDLE;

void CH375_HostPrpess(void)
{
    u8 state = 0;
    switch(HostPort)
    {
        case HOST_IDLE:  
        {
            HostPort = Host_TEST_HARDWARE;
            Bsp_CH375_SetUSBMode(NoSOF_Host); //������:NoSOF_Host ����USB �豸���Ƚ���ģʽ7 �ٻ���ģʽ6
        }break;            
        case Host_TEST_HARDWARE:
        {
            Bsp_CH375_Write_Cmd(CMD_CHECK_EXIST);   //����ͨѶ ,д�������ݽ�,ȡ��ȡ��
            Bsp_CH375_Write_Data(0x65);
            uint8_t s = Bsp_CH375_Read_Data();
            if(s == (0x9A)) {
                HostPort = Host_GETHARDWAREVERSION;
                CH375_Log("CH375 Send OK");
                break;
            }        
        }break;            
        case Host_GETHARDWAREVERSION:
        {
            uint8_t s = Bsp_CH375_Get_DeviceVersion();
            if(s == 0x37) {
                HostPort = Host_WAIT_CONNECT;
                Bsp_CH375_SetUSBMode(AutoSOF_Host);                 
                CH375_Log("wait connect...");
            }
        }break; 
        case Host_WAIT_CONNECT:
        {
            if(Bsp_CH375_Get_INTStatus() == USB_INT_CONNECT)  
            {
               HostPort = Host_INIT;
               Bsp_CH375_Reset_Device();
               delay_ms(10);
//               Bsp_CH375_SetUSBMode(AutoSOF_Host);                
            }
        }break;            
        case Host_INIT: 
        {
            state = CH375_HostDesc_Init();
            if(state == USBH_OK)
            {
               HostPort = Host_Get_USB_DESC;
               CH375_Log(">Host_Get_USB_DESC");
            }else if(state == USBH_FAIL){
               HostPort = HOST_IDLE;
            }
        }break;            
        case Host_Get_USB_DESC:
        {
           if(Bsp_CH375_Get_INTStatus() == USB_INT_DISCONNECT)   
            {
               HostPort = Host_WAIT_CONNECT;
            }        
            
        }break;            
        case Host_Get_USB_CFGDESC:
        {
           if(Bsp_CH375_Get_INTStatus() == USB_INT_DISCONNECT)  
            {
               HostPort = Host_WAIT_CONNECT;
            }          
        }break;

    }
    
    
}



/* ������ */
void CH375_Control_12()
{
    uint8_t i, s;
    unsigned short len;
    do {
        Bsp_CH375_Write_Cmd(CMD_CHECK_EXIST);   //����ͨѶ ,д�������ݽ�,ȡ��ȡ��
        Bsp_CH375_Write_Data(0x65);
        s = Bsp_CH375_Read_Data();
        if(s == (0x9A)) {
            CH375_Log("CH375 Send OK");
            break;
        }
    } while(1);
    do {
        s = Bsp_CH375_Get_DeviceVersion();
        if(s != 0x37) {
            delayms(100);
        }
    } while(s != 0x37);
    Bsp_CH375_SetUSBMode(NoSOF_Host);   /* ����USB����ģʽ, ����豸����CH37X, ��ô5��6���� */
    while(1) {
        CH375_Log("wait connect...");
        while(Bsp_CH375_Get_INTStatus() != USB_INT_CONNECT);   /* �ȴ��豸���������� */
        delayms(200);                    /* �ȴ������ȶ� */
        /***** ��λ���USB�豸�ٶ� *****/
        CH375_Log("check rate");
        Bsp_CH375_Reset_Device();
        delayms(100);         //��λ֮���൱����������������ʱ��֤�豸�ȶ�
        /***** ��ȡ�豸������ *****/
        if(Bsp_CH375_Get_USBDescr(0x01) == USB_INT_SUCCESS) {     //��ȡ�豸������(1:�豸 2:����)
            uint8_t len = Bsp_CH375_Read_USBData(Rx_Buf);
            CH375_Logn("�豸��������:");
            for(u8 i = 0; i < (len < 128 ? len : 64); i++) {
                CH375_Logn("%02x ", (unsigned int)Rx_Buf[i]);
            }
            CH375_Logn("\n");
        }
        MaxBuffSize = Rx_Buf[7];  //�˵�0������С
        ////        Request.Req.bmRequestType=0x80;
        ////        Request.Req.bRequest=0x06;
        ////        Request.Req.wValue=0x0001;      /* ��Ϊ51��Ƭ���Ǵ�˴洢������д��0x0100,�������� */
        ////        Request.Req.wIndex=0x0000;
        ////        Request.Req.wLength=0x0800;
        ////        if( SETUP_Transfer(data_buf, &len) == USB_INT_SUCCESS )
        ////        {
        ////            max_package = data_buf[7];  //�˵�0������С
        ////            Request.Req.wLength=0x1200;
        ////            if( SETUP_Transfer(data_buf, &len) )
        ////            {
        ////                for(i=0; i!=len; i++)
        ////                    CH375_LogB("%02x ",(unsigned int)data_buf[i]);
        ////                    CH375_LogA(" ");
        ////            }
        ////        }
        ////        else
        ////            CH375_Log("get device descr failed\n");
        ///***** ��λ *****/
//        Bsp_CH375_Reset_Device();
//        //        if( s&0x10 )  set_freq();     /* �л�ʹ375B�������ģʽ */
//        delayms(100);                   //��λ֮���൱����������������ʱ��֤�豸�ȶ�
//        ///***** ���õ�ַ  *****/
//        Bsp_CH375_SetAddr(5);   //���õ�ַ
//        ///***** ��ȡ���������� *****/
//        if(Bsp_CH375_Get_USBDescr(0x02) == USB_INT_SUCCESS) {     //��ȡ�豸������(1:�豸 2:����) (0x02);//��ȡ����������
//            CH375_Log("������������");
//        }
//        USB_Request.Req.bmRequestType = 0x80;
//        USB_Request.Req.bRequest = 0x06;
//        USB_Request.Req.wValue   = 0x0200;
//        USB_Request.Req.wIndex   = 0x0000;
//        USB_Request.Req.wLength  = 0x0004;
//        if(CH375_SETUP_Transfer(Rx_Buf, &len) == USB_INT_SUCCESS) {
//            USB_Request.Req_buf[6] = Rx_Buf[2];
//            USB_Request.Req_buf[7] = Rx_Buf[3];
//            CH375_SETUP_Transfer(Rx_Buf, &len);
//            for(i = 0; i != len; i++) {
//                CH375_Logn("%02x ", (unsigned int)Rx_Buf[i]);
//            }
//            CH375_Logn("\n");
//        } else {
//            CH375_Log("get config descr failed");
//        }
        Bsp_CH375_HOST_Init();
        while(Bsp_CH375_Get_INTStatus() != USB_INT_DISCONNECT)
        {
        
             CH375_Log("״̬ %02X",Bsp_CH375_Get_INTStatus());
             delay_ms(500);
        
        }
        /***** �������������� *****/
        //      parse_config_descr(data_buf);   //������������һЩֵ
        //       /***** �������� *****/
        //      CH375_LogA("set config");
        //      set_config(config_value);     //��������
        //        /***** HID������ *****/
        //      for(s=0;s<num_interfaces;s++)
        //      {
        //          CH375_LogA("set idle");
        //          CH_Request.Req.bmRequestType=0x21;
        //          CH_Request.Req.bRequest=0x0A;
        //          CH_Request.Req.wValue=0x0000;
        //          CH_Request.Req.wIndex=0x0000;
        //          CH_Request.Req.wLength=0x0000;
        //          CH_Request.Req_buf[4] = s;
        //          if(SETUP_Transfer(NULL, NULL)!=USB_INT_SUCCESS)
        //              CH375_LogA("set idle failed");
        //
        //          CH375_LogA("get report");
        //          CH_Request.Req.bmRequestType=0x81;
        //          CH_Request.Req.bRequest=0x06;
        //          CH_Request.Req.wValue=0x0022;
        //          CH_Request.Req.wIndex=0x0000;
        //          CH_Request.Req_buf[4] = s;
        //          if(s==0) CH_Request.Req.wLength=0x0000|((unsigned short)(report_descr0_len+0x40)<<8);
        //          else CH_Request.Req.wLength=0x0000|((unsigned short)(report_descr1_len+0x40)<<8);
        //
        //          if(SETUP_Transfer(data_buf, &len)==USB_INT_SUCCESS)
        //          {
        //              for(i=0;i!=len;i++)
        //                  CH375_LogB("%02x ",(unsigned short)data_buf[i]);
        //              CH375_LogA(" ");
        //          }
        //          else CH375_LogA("get report descr failed");
        //      }
        //      CH375_LogA("set report");       //���ڼ�����һ�����ǵ���ָʾ��
        //      CH_Request.Req.bmRequestType=0x21;
        //      CH_Request.Req.bRequest=0x09;
        //      CH_Request.Req.wValue=0x0002;
        //      CH_Request.Req.wIndex=0x0000;
        //      CH_Request.Req.wLength=0x0100;
        //      data_buf[0]=1;
        //      if(SETUP_Transfer(data_buf, &len)!=USB_INT_SUCCESS)
        //          CH375_LogA("set report failed\n");
        //       /* ���ö���ӦNAK�����Դ��� */
        //      set_retry(3);  //��ʱ����3�Σ����յ�NAK������
        //        /* ��ȡ���� */
        //      endp6_mode=0x80;  //��λͬ����־
        //      toggle_recv();
        //      while(1)
        //      {
        //          s = issue_token( endp_int,DEF_USB_PID_IN);
        //          if(s==USB_INT_SUCCESS)
        //          {
        //              toggle_recv();
        //              len = rd_usb_data( data_buf );
        //              for(i=0;i!=len;i++)
        //                  CH375_LogB("%02x ",(unsigned short)data_buf[i]);
        //                CH375_LogA(" ");
        //          }
        //      }
    }
}





//CH375_USB_DEF   CH375_USB_Info;
uint8_t endp_out_addr;
uint8_t endp_out_size;
uint8_t tog_send;
uint8_t endp_in_addr;
uint8_t tog_recv;

uint8_t Bsp_CH375_Device_Init(void)
{
    PUSB_DEV_DESCR          p_dev_descr;
    PUSB_CFG_DESCR_LONG     p_cfg_descr;
    uint8_t status, len, c;
    p_dev_descr = (PUSB_DEV_DESCR)CH375_USB_Info.USB_Buffer;
    p_cfg_descr = (PUSB_CFG_DESCR_LONG)CH375_USB_Info.USB_Buffer;
    
    status = Bsp_CH375_Get_USBDescr(1);
    if(status == USB_INT_SUCCESS){
        CH375_Log("��ȡ�豸�������ɹ�");
        len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
        if(len < 18 || p_dev_descr->bDescriptorType != 1) {
            CH375_Log("�������:���������ȴ���������ʹ���");
            return(UNKNOWN_USB_DEVICE);
        }
        if(p_dev_descr->bDeviceClass != 0) {
            CH375_Log("���ӵ�USB�豸����HID�豸,���߲�����USB�淶 ");
            return(UNKNOWN_USB_DEVICE);
        }
        if(p_dev_descr->idProduct != USBD_PID_FS || p_dev_descr->idVendor != USBD_VID) {
//            CH375_Log("���ӵ�USB�豸����Eawada HID Device ");
//            return(UNKNOWN_USB_DEVICE);
        }
        status = Bsp_CH375_SetAddr(3);
        if(status == USB_INT_SUCCESS) {
            CH375_Log("����HID�豸��USB��ַ�ɹ�");
            status = Bsp_CH375_Get_USBDescr(2);
            if(status == USB_INT_SUCCESS) {
                CH375_Log("��ȡ�����������ɹ�");
                len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
                
                for(u8 i=0;i<len;i++)
                {
                    if(i%9==0)CH375_Logn("\r\n");
                    CH375_Logn("%02X ",CH375_USB_Info.USB_Buffer[i]);
                }
                CH375_Logn("\r\n");
                if(p_cfg_descr->itf_descr.bInterfaceClass != 3 || p_cfg_descr->itf_descr.bInterfaceSubClass != 0) {
                    CH375_Log("����:����HID�豸���߲�����USB�淶");
                    return(UNKNOWN_USB_HID);
                }
//                endp_out_addr = endp_in_addr = 0;
//                c = p_cfg_descr->endp_descr[0][0].bEndpointAddress;
//                if(c & 0x80) {
//                    endp_in_addr = c & 0x0f;
//                    CH375_Log("IN1�˵��ַ:%d", endp_in_addr);
//                } else {
//                    endp_out_addr = c & 0x0f;
//                    endp_out_size = p_cfg_descr->endp_descr[0].wMaxPacketSize;
//                    CH375_Log("OUT1�˵��ַ:%d, ��󳤶�:%d", endp_out_addr, endp_out_size);
//                }
//                if(p_cfg_descr->itf_descr.bNumEndpoints >= 2) {
//                    if(p_cfg_descr->endp_descr[1].bDescriptorType == 5) {
//                        c = p_cfg_descr->endp_descr[1].bEndpointAddress;
//                        if(c & 0x80) {
//                            endp_in_addr = c & 0x0f;
//                            CH375_Log("IN2�˵��ַ:%d", endp_in_addr);
//                        } else {
//                            endp_out_addr = c & 0x0f;
//                            endp_out_size = p_cfg_descr->endp_descr[1].wMaxPacketSize;
//                            CH375_Log("OUT2�˵��ַ:%d, ��󳤶�:%d", endp_out_addr, endp_out_size);
//                        }
//                    }
//                }
                USBH_ParseCfgDesc(&CH_DESCR.cfg_descr,&CH_DESCR.itf_descr,CH_DESCR.endp_descr,CH_DESCR.hid_descr,CH375_USB_Info.USB_Buffer,len ); //����������
                endp_out_addr = endp_in_addr = 0;
                c = CH_DESCR.endp_descr[0][0].bEndpointAddress;
                if(c & 0x80) {
                    endp_in_addr = c & 0x0f;
                    CH375_Log("IN1�˵��ַ:%d", endp_in_addr);
                } else {
                    endp_out_addr = c & 0x0f;
                    endp_out_size = CH_DESCR.endp_descr[0][0].wMaxPacketSize;
                    CH375_Log("OUT1�˵��ַ:%d, ��󳤶�:%d", endp_out_addr, endp_out_size);
                }
                if(p_cfg_descr->itf_descr.bNumEndpoints >= 2) {
                    if(CH_DESCR.endp_descr[0][1].bDescriptorType == 5) {
                        c = CH_DESCR.endp_descr[0][1].bEndpointAddress;
                        if(c & 0x80) {
                            endp_in_addr = c & 0x0f;
                            CH375_Log("IN2�˵��ַ:%d", endp_in_addr);
                        } else {
                            endp_out_addr = c & 0x0f;
                            endp_out_size = CH_DESCR.endp_descr[0][1].wMaxPacketSize;
                            CH375_Log("OUT2�˵��ַ:%d, ��󳤶�:%d", endp_out_addr, endp_out_size);
                        }
                    }
                }
                if(p_cfg_descr->itf_descr.bInterfaceProtocol <= 1) {
                    endp_out_addr = 0;
                }
                if(endp_in_addr == 0) {
                    return(UNKNOWN_USB_HID);
                }
                status = Bsp_CH375_SetConfig(p_cfg_descr->cfg_descr.bConfigurationvalue);//�����豸�˵�USB���� 
                if(status == USB_INT_SUCCESS) {
                    Bsp_CH375_Set_Retry(0x89);
                }
            }
        }
    }
    return (status);
}

void Bsp_CH375_HOST_Init(void)
{
//    NVIC_DisableIRQ(EXTI4_15_IRQn);
    
//    loop:
    Bsp_DelayMs(200);
    if(Bsp_CH375_SetUSBMode(AutoSOF_Host) == FALSE) {CH375_Log(">False 1");} //goto loop;
    if(Bsp_CH375_Wait_INT() != USB_INT_CONNECT)     {CH375_Log(">False 2");} //goto loop;
    if(Bsp_CH375_Reset_Device() == FALSE)           {CH375_Log(">False 3");} //goto loop;
    if(Bsp_CH375_Wait_INT() != USB_INT_CONNECT)     {CH375_Log(">False 4");} //goto loop;
    Bsp_DelayMs(200);
    if ( USB_INT_SUCCESS != Bsp_CH375_Device_Init() )
    {
      while(1){}
    }
         
    CH375_Log("USB�豸ö�ٳɹ�");
//    while(1)
//    {
//       CH375_Log("״̬ %d")
//    
//    }
//    SysParam_Info.USB_Connect_Status = 1;
//    NVIC_EnableIRQ(EXTI4_15_IRQn);
}


#if 1
//wait connect...
//check rate
//Set USB Mode 7 success
//Set USB Mode 6 success
//Wait for USB interrupt...
//�豸��������:12 01 00 02 00 00 00 40 42 50 00 02 00 02 01 02 03 01 
//Set USB Mode 6 success
//Wait for USB interrupt...
//Set USB Mode 7 success
//Set USB Mode 6 success
//Wait for USB interrupt...
//Wait for USB interrupt...
//��ȡ�豸�������ɹ�
//Wait for USB interrupt...
//����HID�豸��USB��ַ�ɹ�
//Wait for USB interrupt...
//��ȡ�����������ɹ�

//09 02 29 00 01 01 00 C0 32 
//09 04 00 00 02 03 00 00 00 
//09 21 10 01 00 01 22 21 00 
//07 05 81 03 3C 00 01 07 05 
//01 03 3C 00 01 
//IN1�˵��ַ:1
//OUT2�˵��ַ:1, ��󳤶�:60
//Wait for USB interrupt...
//USB�豸ö�ٳɹ�
//wait connect...
//CH375 Send OK
//USB version: 0x37
//Set USB Mode 5 success
//wait connect...
//check rate
//Set USB Mode 7 success
//Set USB Mode 6 success
//Wait for USB interrupt...
//�豸��������:12 01 00 01 00 00 00 40 42 50 00 02 2b 7f 01 02 03 01 
//Set USB Mode 6 success
//Wait for USB interrupt...
//Set USB Mode 7 success
//Set USB Mode 6 success
//Wait for USB interrupt...
//Wait for USB interrupt...
//��ȡ�豸�������ɹ�
//Wait for USB interrupt...
//����HID�豸��USB��ַ�ɹ�
//Wait for USB interrupt...
//��ȡ�����������ɹ�

//09 02 29 00 01 01 00 E0 32 
//09 04 00 00 02 03 00 00 00 
//09 21 10 01 00 01 22 21 00 
//07 05 82 03 40 00 02 07 05 
//01 03 40 00 0A 
//IN1�˵��ַ:2
//OUT2�˵��ַ:1, ��󳤶�:64
//Wait for USB interrupt...
//USB�豸ö�ٳɹ�

#endif
