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

#define     STATUS_DISCONNECT       0   /* U盘尚未连接或者已经断开 */
#define     STATUS_CONNECT          1   /* U盘刚刚连接 */
#define     STATUS_ERROR            2   /* U盘操作错误或者不支持 */
#define     STATUS_WAIT             3   /* U盘正在操作 */
#define     STATUS_READY            4   /* U盘准备好接受操作 */

/*
  下面是USB移动存储设备的简单应用参考。在单片机应用程序中，
  可以定义USB存储设备的4种状态：已断开、已连接、初始化、可读写。其中"已连接"状态可选。
● 已断开是指USB存储设备已经断开（从USB插座中拔出），完全不可用；
● 已连接是指USB存储设备已经连接（插入USB插座中），但是尚未初始化；
● 初始化是指USB存储设备正在初始化或者初始化失败，所以不可以进行读写；
● 可读写是指USB存储设备初始化成功，可以进行数据读写。

单片机处理CH375中断的一般步骤如下：
㈠ CH375将INT#引脚设置为低电平，向单片机请求中断；
㈡ 单片机进入中断服务程序，首先执行GET_STATUS命令获取中断状态；
㈢ CH375在GET_STATUS命令完成后将INT#引脚恢复为高电平，取消中断请求；
㈣ 单片机可以参考下面的顺序分析GET_STATUS命令获取的中断状态：
  ⑴ 如果中断状态是USB_INT_DISCONNECT，则说明USB设备已经断开，置USB存储设备状态为"已断开"，
     如果在此之前USB存储设备有读写操作尚未完成，则放弃操作并按操作失败处理，然后退出中断，必要时可以通知单片机主程序。
  ⑵ 如果中断状态是USB_INT_CONNECT，则说明USB设备已经连接，置USB存储设备状态为"已连接"，
     接着发出DISK_INIT命令，并置USB存储设备状态为初始化，然后退出中断，必要时可以通知单片机主程序。
  ⑶ 如果当前的USB存储设备状态是"初始化"：
    ① 如果中断状态是USB_INT_SUCCESS，则说明USB存储设备初始化成功，置USB存储设备状态为"可读写"，
       然后退出中断，必要时可以通知单片机主程序。
    ② 如果中断状态是其它状态，则说明USB存储设备初始化失败，应该通知单片机主程序，
       提示该USB设备不是存储设备或者该USB设备不支持，然后退出中断。或者，单片机通过其它命令自行处理该USB存储设备的通讯协议。
  ⑷ 如果当前的USB存储设备状态是"可读写"，则可以保存中断状态并通知单片机主程序处理，
     然后直接退出中断；或者，在中断服务程序中继续处理：
    ① 如果中断状态是USB_INT_DISK_READ，则说明正在进行USB存储设备的读操作，需要取走64个字节的数据，
       可以发出RD_USB_DATA命令取走数据，接着再发出DISK_RD_GO命令使CH375继续读，然后退出中断。
    ② 如果中断状态是USB_INT_DISK_WRITE，则说明正在进行USB存储设备的写操作，需要提供64个字节的数据，
       可以发出WR_USB_DATA7命令提供数据，接着再发出DISK_WR_GO命令使CH375继续写，然后退出中断。
    ③ 如果中断状态是USB_INT_SUCCESS，则说明读写操作成功，直接退出中断并通知主程序该操作成功；
    ④ 如果中断状态是USB_INT_DISK_ERR，则说明读写操作失败，直接退出中断并通知主程序该操作失败；
    ⑤ 通常不会返回其它中断状态，如果有，则说明出现错误。
  ⑸ 通常不会在其它USB存储设备状态下返回其它中断状态，如果有，则说明出现错误，可以参照USB设备断开的状态处理。

当单片机主程序需要从USB存储设备中读写数据时，可以查询USB存储设备状态，
如果是"可读写"状态，则可以发出DISK_READ命令读数据，或者发出DISK_WRITE命令写数据。
后续的数据读写过程可以在单片机的中断服务程序中完成，也可以由中断程序置标志通知等待中的主程序，
由主程序分析中断状态再完成数据读写过程。
*/

#include "ch375.h"
#include "delay.h"
#include "string.h"

#define Bsp_DelayUs delay_us
#define Bsp_DelayMs delay_ms
#define delayms     delay_ms

uint8_t MaxBuffSize = 8;
uint8_t Rx_Buf[128] = {0};  //缓存区
uint8_t ep6_mode = 0x80, ep7_mode = 0x80; //同步标志位初值
void Bsp_CH375_HOST_Init(void);

union _REQUEST { //请求包结构
    struct {
        unsigned char bmRequestType; //请求类型
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
        case 0:  /* 获取设备描述符 */
        {
            status = Bsp_CH375_Get_USBDescr(1); 
            if(status == USB_INT_SUCCESS)
            {
               CH375_Log("获取设备描述符OK");                 
               len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
               if(len < 18 || p_dev_descr->bDescriptorType != 1) {
                   CH375_Log("意外错误:描述符长度错误或者类型错误");
               }  
               USBH_ParseDevDesc(&CH_HostDev,CH375_USB_Info.USB_Buffer,len); //显示设备描述 
               if(p_dev_descr->bDeviceClass != 0) {
                   CH375_Log("连接的USB设备不是HID设备,或者不符合USB规范 ");
                    Port = 0;
                   return (USBH_FAIL);
               }               
               Port = 1;               
            }else{
               CH375_Log("获取设备描述符Err"); 
                Port = 0;
               return  (USBH_FAIL);             
            }                
        }break;
        case 1: /* 设置地址 */
        {
            status = Bsp_CH375_SetAddr(2);
            if(status == USB_INT_SUCCESS) {
              CH375_Log("设置HID设备的USB地址成功");
              Port = 2;     
            }else{
              Port = 0;
              return  (USBH_FAIL);              
            }
        }break;
        case 2: /* 获取配置描述符 */
        {
            status = Bsp_CH375_Get_USBDescr(2);
            if(status == USB_INT_SUCCESS) {
                CH375_Log("获取配置描述符成功");
                len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
                USBH_ParseCfgDesc(&CH_DESCR.cfg_descr,&CH_DESCR.itf_descr,CH_DESCR.endp_descr,CH_DESCR.hid_descr,CH375_USB_Info.USB_Buffer,len ); //分析描述符
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
//                    CH375_Log("错误:不是HID设备或者不符合USB规范");
//                    return(UNKNOWN_USB_HID);
//                }
//                endp_out_addr = endp_in_addr = 0;
//                c = p_cfg_descr->endp_descr[0].bEndpointAddress;
//                if(c & 0x80) {
//                    endp_in_addr = c & 0x0f;
//                    CH375_Log("IN1端点地址:%d", endp_in_addr);
//                } else {
//                    endp_out_addr = c & 0x0f;
//                    endp_out_size = p_cfg_descr->endp_descr[0].wMaxPacketSize;
//                    CH375_Log("OUT1端点地址:%d, 最大长度:%d", endp_out_addr, endp_out_size);
//                }
//                if(p_cfg_descr->itf_descr.bNumEndpoints >= 2) {
//                    if(p_cfg_descr->endp_descr[1].bDescriptorType == 5) {
//                        c = p_cfg_descr->endp_descr[1].bEndpointAddress;
//                        if(c & 0x80) {
//                            endp_in_addr = c & 0x0f;
//                            CH375_Log("IN2端点地址:%d", endp_in_addr);
//                        } else {
//                            endp_out_addr = c & 0x0f;
//                            endp_out_size = p_cfg_descr->endp_descr[1].wMaxPacketSize;
//                            CH375_Log("OUT2端点地址:%d, 最大长度:%d", endp_out_addr, endp_out_size);
//                        }
//                    }
//                }
//                if(p_cfg_descr->itf_descr.bInterfaceProtocol <= 1) {
//                    endp_out_addr = 0;
//                }
//                if(endp_in_addr == 0) {
//                    return(UNKNOWN_USB_HID);
//                }
//                status = Bsp_CH375_SetConfig(p_cfg_descr->cfg_descr.bConfigurationvalue);//设置设备端的USB配置 
//                if(status == USB_INT_SUCCESS) {
//                    Bsp_CH375_Set_Retry(0x89);
//                }
//            }
//        }
//    }
//    return (status);

}



/* 控制传输,被枚举过程调用 */
unsigned short CH375_SETUP_Transfer(unsigned char *DataBuf, unsigned short *plen)
{
    unsigned char  len, state;
    unsigned short req_len, real_len = 0;
    unsigned char *p = DataBuf;
    ep7_mode = 0x80; //DATA0发送器
    ep6_mode = 0xc0; //DATA1接收器
    req_len = (unsigned short int)(USB_Request.Req_buf[7] << 8) | USB_Request.Req_buf[6];
    /* SETUP阶段 */
    Bsp_CH375_Write_USBData(8, USB_Request.Req_buf);
    BSP_CH375_Toggle_Send();                          //主机发送成功后,切换DATA0和DATA1实现数据同步
    state = Bsp_CH375_IssueToken(DEF_USB_PID_SETUP);      //高4位目的端点号, 低4位令牌PID
    if(state != USB_INT_SUCCESS) {
        return(0);
    }
    /* DATA阶段 */
    if(USB_Request.Req_buf[0] & 0x80) {   //IN数据
        while(req_len) {
            BSP_CH375_Toggle_Recv();
            state = Bsp_CH375_IssueToken(DEF_USB_PID_IN);
            if(state == USB_INT_SUCCESS) {
                len = Bsp_CH375_Read_USBData(p);
                real_len += len;
                if(len < MaxBuffSize) {      //短包
                    break;
                }
                p += state;
                req_len -= state;
            } else {
                return(0);
            }
        }
    } else {                        //OUT数据
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
    /* 状态阶段 */
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
    return(state);      //成功
}




typedef enum {
    HOST_IDLE = 0,           /*初始状态*/
    Host_TEST_HARDWARE,      /*测试硬件*/
    Host_GETHARDWAREVERSION, /*获取硬件版本*/
    Host_WAIT_CONNECT      , /*等待连接*/    
    Host_INIT,               /*初始化*/
    Host_Get_USB_DESC,       /*获取设备描述符*/
    Host_Get_USB_CFGDESC,    /*获取设备描述符*/
    
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
            Bsp_CH375_SetUSBMode(NoSOF_Host); //无连接:NoSOF_Host 插入USB 设备后先进入模式7 再换到模式6
        }break;            
        case Host_TEST_HARDWARE:
        {
            Bsp_CH375_Write_Cmd(CMD_CHECK_EXIST);   //测试通讯 ,写任意数据进,取反取出
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



/* 主函数 */
void CH375_Control_12()
{
    uint8_t i, s;
    unsigned short len;
    do {
        Bsp_CH375_Write_Cmd(CMD_CHECK_EXIST);   //测试通讯 ,写任意数据进,取反取出
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
    Bsp_CH375_SetUSBMode(NoSOF_Host);   /* 设置USB主机模式, 如果设备端是CH37X, 那么5和6均可 */
    while(1) {
        CH375_Log("wait connect...");
        while(Bsp_CH375_Get_INTStatus() != USB_INT_CONNECT);   /* 等待设备端连接上来 */
        delayms(200);                    /* 等待连接稳定 */
        /***** 复位检测USB设备速度 *****/
        CH375_Log("check rate");
        Bsp_CH375_Reset_Device();
        delayms(100);         //复位之后，相当于重连，必须有延时保证设备稳定
        /***** 获取设备描述符 *****/
        if(Bsp_CH375_Get_USBDescr(0x01) == USB_INT_SUCCESS) {     //获取设备描述符(1:设备 2:配置)
            uint8_t len = Bsp_CH375_Read_USBData(Rx_Buf);
            CH375_Logn("设备描述符是:");
            for(u8 i = 0; i < (len < 128 ? len : 64); i++) {
                CH375_Logn("%02x ", (unsigned int)Rx_Buf[i]);
            }
            CH375_Logn("\n");
        }
        MaxBuffSize = Rx_Buf[7];  //端点0最大包大小
        ////        Request.Req.bmRequestType=0x80;
        ////        Request.Req.bRequest=0x06;
        ////        Request.Req.wValue=0x0001;      /* 因为51单片机是大端存储，否则写成0x0100,下面类似 */
        ////        Request.Req.wIndex=0x0000;
        ////        Request.Req.wLength=0x0800;
        ////        if( SETUP_Transfer(data_buf, &len) == USB_INT_SUCCESS )
        ////        {
        ////            max_package = data_buf[7];  //端点0最大包大小
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
        ///***** 复位 *****/
//        Bsp_CH375_Reset_Device();
//        //        if( s&0x10 )  set_freq();     /* 切换使375B进入低速模式 */
//        delayms(100);                   //复位之后，相当于重连，必须有延时保证设备稳定
//        ///***** 设置地址  *****/
//        Bsp_CH375_SetAddr(5);   //设置地址
//        ///***** 获取配置描述符 *****/
//        if(Bsp_CH375_Get_USBDescr(0x02) == USB_INT_SUCCESS) {     //获取设备描述符(1:设备 2:配置) (0x02);//获取配置描述符
//            CH375_Log("配置描述符是");
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
        
             CH375_Log("状态 %02X",Bsp_CH375_Get_INTStatus());
             delay_ms(500);
        
        }
        /***** 分析配置描述符 *****/
        //      parse_config_descr(data_buf);   //保存描述符中一些值
        //       /***** 设置配置 *****/
        //      CH375_LogA("set config");
        //      set_config(config_value);     //设置配置
        //        /***** HID类命令 *****/
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
        //      CH375_LogA("set report");       //对于键盘这一步，是点亮指示灯
        //      CH_Request.Req.bmRequestType=0x21;
        //      CH_Request.Req.bRequest=0x09;
        //      CH_Request.Req.wValue=0x0002;
        //      CH_Request.Req.wIndex=0x0000;
        //      CH_Request.Req.wLength=0x0100;
        //      data_buf[0]=1;
        //      if(SETUP_Transfer(data_buf, &len)!=USB_INT_SUCCESS)
        //          CH375_LogA("set report failed\n");
        //       /* 设置对响应NAK的重试次数 */
        //      set_retry(3);  //超时重试3次，但收到NAK不重试
        //        /* 获取数据 */
        //      endp6_mode=0x80;  //复位同步标志
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
        CH375_Log("获取设备描述符成功");
        len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
        if(len < 18 || p_dev_descr->bDescriptorType != 1) {
            CH375_Log("意外错误:描述符长度错误或者类型错误");
            return(UNKNOWN_USB_DEVICE);
        }
        if(p_dev_descr->bDeviceClass != 0) {
            CH375_Log("连接的USB设备不是HID设备,或者不符合USB规范 ");
            return(UNKNOWN_USB_DEVICE);
        }
        if(p_dev_descr->idProduct != USBD_PID_FS || p_dev_descr->idVendor != USBD_VID) {
//            CH375_Log("连接的USB设备不是Eawada HID Device ");
//            return(UNKNOWN_USB_DEVICE);
        }
        status = Bsp_CH375_SetAddr(3);
        if(status == USB_INT_SUCCESS) {
            CH375_Log("设置HID设备的USB地址成功");
            status = Bsp_CH375_Get_USBDescr(2);
            if(status == USB_INT_SUCCESS) {
                CH375_Log("获取配置描述符成功");
                len = Bsp_CH375_Read_USBData(CH375_USB_Info.USB_Buffer);
                
                for(u8 i=0;i<len;i++)
                {
                    if(i%9==0)CH375_Logn("\r\n");
                    CH375_Logn("%02X ",CH375_USB_Info.USB_Buffer[i]);
                }
                CH375_Logn("\r\n");
                if(p_cfg_descr->itf_descr.bInterfaceClass != 3 || p_cfg_descr->itf_descr.bInterfaceSubClass != 0) {
                    CH375_Log("错误:不是HID设备或者不符合USB规范");
                    return(UNKNOWN_USB_HID);
                }
//                endp_out_addr = endp_in_addr = 0;
//                c = p_cfg_descr->endp_descr[0][0].bEndpointAddress;
//                if(c & 0x80) {
//                    endp_in_addr = c & 0x0f;
//                    CH375_Log("IN1端点地址:%d", endp_in_addr);
//                } else {
//                    endp_out_addr = c & 0x0f;
//                    endp_out_size = p_cfg_descr->endp_descr[0].wMaxPacketSize;
//                    CH375_Log("OUT1端点地址:%d, 最大长度:%d", endp_out_addr, endp_out_size);
//                }
//                if(p_cfg_descr->itf_descr.bNumEndpoints >= 2) {
//                    if(p_cfg_descr->endp_descr[1].bDescriptorType == 5) {
//                        c = p_cfg_descr->endp_descr[1].bEndpointAddress;
//                        if(c & 0x80) {
//                            endp_in_addr = c & 0x0f;
//                            CH375_Log("IN2端点地址:%d", endp_in_addr);
//                        } else {
//                            endp_out_addr = c & 0x0f;
//                            endp_out_size = p_cfg_descr->endp_descr[1].wMaxPacketSize;
//                            CH375_Log("OUT2端点地址:%d, 最大长度:%d", endp_out_addr, endp_out_size);
//                        }
//                    }
//                }
                USBH_ParseCfgDesc(&CH_DESCR.cfg_descr,&CH_DESCR.itf_descr,CH_DESCR.endp_descr,CH_DESCR.hid_descr,CH375_USB_Info.USB_Buffer,len ); //分析描述符
                endp_out_addr = endp_in_addr = 0;
                c = CH_DESCR.endp_descr[0][0].bEndpointAddress;
                if(c & 0x80) {
                    endp_in_addr = c & 0x0f;
                    CH375_Log("IN1端点地址:%d", endp_in_addr);
                } else {
                    endp_out_addr = c & 0x0f;
                    endp_out_size = CH_DESCR.endp_descr[0][0].wMaxPacketSize;
                    CH375_Log("OUT1端点地址:%d, 最大长度:%d", endp_out_addr, endp_out_size);
                }
                if(p_cfg_descr->itf_descr.bNumEndpoints >= 2) {
                    if(CH_DESCR.endp_descr[0][1].bDescriptorType == 5) {
                        c = CH_DESCR.endp_descr[0][1].bEndpointAddress;
                        if(c & 0x80) {
                            endp_in_addr = c & 0x0f;
                            CH375_Log("IN2端点地址:%d", endp_in_addr);
                        } else {
                            endp_out_addr = c & 0x0f;
                            endp_out_size = CH_DESCR.endp_descr[0][1].wMaxPacketSize;
                            CH375_Log("OUT2端点地址:%d, 最大长度:%d", endp_out_addr, endp_out_size);
                        }
                    }
                }
                if(p_cfg_descr->itf_descr.bInterfaceProtocol <= 1) {
                    endp_out_addr = 0;
                }
                if(endp_in_addr == 0) {
                    return(UNKNOWN_USB_HID);
                }
                status = Bsp_CH375_SetConfig(p_cfg_descr->cfg_descr.bConfigurationvalue);//设置设备端的USB配置 
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
         
    CH375_Log("USB设备枚举成功");
//    while(1)
//    {
//       CH375_Log("状态 %d")
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
//设备描述符是:12 01 00 02 00 00 00 40 42 50 00 02 00 02 01 02 03 01 
//Set USB Mode 6 success
//Wait for USB interrupt...
//Set USB Mode 7 success
//Set USB Mode 6 success
//Wait for USB interrupt...
//Wait for USB interrupt...
//获取设备描述符成功
//Wait for USB interrupt...
//设置HID设备的USB地址成功
//Wait for USB interrupt...
//获取配置描述符成功

//09 02 29 00 01 01 00 C0 32 
//09 04 00 00 02 03 00 00 00 
//09 21 10 01 00 01 22 21 00 
//07 05 81 03 3C 00 01 07 05 
//01 03 3C 00 01 
//IN1端点地址:1
//OUT2端点地址:1, 最大长度:60
//Wait for USB interrupt...
//USB设备枚举成功
//wait connect...
//CH375 Send OK
//USB version: 0x37
//Set USB Mode 5 success
//wait connect...
//check rate
//Set USB Mode 7 success
//Set USB Mode 6 success
//Wait for USB interrupt...
//设备描述符是:12 01 00 01 00 00 00 40 42 50 00 02 2b 7f 01 02 03 01 
//Set USB Mode 6 success
//Wait for USB interrupt...
//Set USB Mode 7 success
//Set USB Mode 6 success
//Wait for USB interrupt...
//Wait for USB interrupt...
//获取设备描述符成功
//Wait for USB interrupt...
//设置HID设备的USB地址成功
//Wait for USB interrupt...
//获取配置描述符成功

//09 02 29 00 01 01 00 E0 32 
//09 04 00 00 02 03 00 00 00 
//09 21 10 01 00 01 22 21 00 
//07 05 82 03 40 00 02 07 05 
//01 03 40 00 0A 
//IN1端点地址:2
//OUT2端点地址:1, 最大长度:64
//Wait for USB interrupt...
//USB设备枚举成功

#endif
