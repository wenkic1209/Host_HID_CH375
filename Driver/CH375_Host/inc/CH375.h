/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : CH375.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-08         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _CH375_H
#define _CH375_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __BSP_CH375_H__
#define __BSP_CH375_H__
 
#include "stm32f10x.h"
#include "stdbool.h"
#include "debug.h"
#include "TM_library.h"
#if 0
#define CH375_Log(...)
#else
#define CH375_Log(...)   do{sprint(__VA_ARGS__);sprint("\r\n");}while(0)
#define CH375_Logn(...)  do{sprint(__VA_ARGS__);}while(0)
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TURE
#define TURE  1
#endif
#ifndef TRUE
#define TRUE  1
#endif


//------------------------------------

//------------------------------------

 #define CH375_A0_GPIO_Port  GPIOA
 #define CH375_RD_GPIO_Port  GPIOE
 #define CH375_WR_GPIO_Port  GPIOE
 #define CH375_A0_Pin        GPIO_Pin_0
 #define CH375_RD_Pin        GPIO_Pin_9
 #define CH375_WR_Pin        GPIO_Pin_10
 #define CH375_INT_GPIO_Port GPIOA
 #define CH375_INT_Pin       GPIO_Pin_1
 
 
#define CH375_EndPointNum    0
#define USBD_VID             1155
#define USBD_PID_FS          22352
 
#define	UNKNOWN_USB_DEVICE	0xF1
#define	UNKNOWN_USB_HID   	0xF2
 
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
 
/* ********************************************************************************************************************* */
/* 硬件特性 */
 
#define	CH375_MAX_DATA_LEN	0x40			/* 最大数据包的长度, 缓冲区的长度 */
 
/* ********************************************************************************************************************* */
/* 命令代码 */
 
#define	CMD_GET_IC_VER		0x01			/* 获取芯片及固件版本 */
/* 输出: 版本号( 位7为1, 位6为0, 位5~位0为版本号 ) */
/*           CH375B返回版本号的值为0B7H即版本号为37H */
 
#define	CMD_ENTER_SLEEP		0x03			/* 进入睡眠状态 */
 
#define	CMD_SET_USB_SPEED	0x04			/* 设置USB总线速度, 在每次CMD_SET_USB_MODE设置USB工作模式时会自动恢复到12Mbps全速 */
/* 输入: 总线速度代码 */
/*           00H=12Mbps全速FullSpeed（默认值）, 01H=1.5Mbps（仅修改频率）, 02H=1.5Mbps低速LowSpeed */
#define	CMD_SET_SYS_FREQ	CMD_SET_USB_SPEED
 
#define	CMD_RESET_ALL		  0x05			/* 执行硬件复位 */
 
#define	CMD_CHECK_EXIST		0x06			/* 测试工作状态 */
/* 输入: 任意数据 */
/* 输出: 输入数据的按位取反 */
 
#define	CMD_GET_TOGGLE		0x0A			/* 获取OUT事务的同步状态 */
/* 输入: 数据1AH */
/* 输出: 同步状态 */
/*           位4为1则OUT事务同步, 否则OUT事务不同步 */
 
#define	CMD_CHK_SUSPEND		0x0B			/* 设备方式: 设置检查USB总线挂起状态的方式 */
/* 输入: 数据10H, 检查方式 */
/*                    00H=不检查USB挂起, 04H=以50mS为间隔检查USB挂起, 05H=以10mS为间隔检查USB挂起 */
 
#define	CMD_DELAY_100US		0x0F			/* 并口方式: 延时100uS */
/* 输出: 延时期间输出0, 延时结束输出非0 */
 
#define	CMD_SET_USB_ID		0x12			/* 设备方式: 设置USB厂商VID和产品PID */
/* 输入: 厂商ID低字节, 厂商ID高字节, 产品ID低字节, 产品ID高字节 */
 
#define	CMD_SET_USB_ADDR	0x13			/* 设置USB地址 */
/* 输入: 地址值 */
 
#define	CMD_SET_USB_MODE	0x15			/* 设置USB工作模式 */
/* 输入: 模式代码 */
/*       00H=未启用的设备方式, 01H=已启用的设备方式并且使用外部固件模式, 02H=已启用的设备方式并且使用内置固件模式 */
/*       04H=未启用的主机方式, 05H=已启用的主机方式, 06H=已启用的主机方式并且自动产生SOF包, 07H=已启用的主机方式并且复位USB总线 */
/* 输出: 操作状态( CMD_RET_SUCCESS或CMD_RET_ABORT, 其它值说明操作未完成 ) */
 
#define	CMD_SET_ENDP2		  0x18			/* 设备方式: 设置USB端点0的接收器 */
/* 输入: 工作方式 */
/*           位7为1则位6为同步触发位, 否则同步触发位不变 */
/*           位3~位0为事务响应方式:  0000-就绪ACK, 1110-正忙NAK, 1111-错误STALL */
 
#define	CMD_SET_ENDP3		  0x19			/* 设备方式: 设置USB端点0的发送器 */
/* 输入: 工作方式 */
/*           位7为1则位6为同步触发位, 否则同步触发位不变 */
/*           位3~位0为事务响应方式:  0000~1000-就绪ACK, 1110-正忙NAK, 1111-错误STALL */
 
#define	CMD_SET_ENDP4		  0x1A			/* 设备方式: 设置USB端点1的接收器 */
/* 输入: 工作方式 */
/*           位7为1则位6为同步触发位, 否则同步触发位不变 */
/*           位3~位0为事务响应方式:  0000-就绪ACK, 1110-正忙NAK, 1111-错误STALL */
 
#define	CMD_SET_ENDP5		  0x1B			/* 设备方式: 设置USB端点1的发送器 */
/* 输入: 工作方式 */
/*           位7为1则位6为同步触发位, 否则同步触发位不变 */
/*           位3~位0为事务响应方式:  0000~1000-就绪ACK, 1110-正忙NAK, 1111-错误STALL */
 
#define	CMD_SET_ENDP6		  0x1C			/* 设置USB端点2/主机端点的接收器 */
/* 输入: 工作方式 */
/*           位7为1则位6为同步触发位, 否则同步触发位不变 */
/*           位3~位0为事务响应方式:  0000-就绪ACK, 1101-就绪但不返回ACK, 1110-正忙NAK, 1111-错误STALL */
 
#define	CMD_SET_ENDP7		  0x1D			/* 设置USB端点2/主机端点的发送器 */
/* 输入: 工作方式 */
/*           位7为1则位6为同步触发位, 否则同步触发位不变 */
/*           位3~位0为事务响应方式:  0000-就绪ACK, 1101-就绪但无须应答, 1110-正忙NAK, 1111-错误STALL */
 
#define	CMD_GET_STATUS		0x22			/* 获取中断状态并取消中断请求 */
/* 输出: 中断状态 */
 
#define	CMD_UNLOCK_USB		0x23			/* 设备方式: 释放当前USB缓冲区 */
 
#define	CMD_RD_USB_DATA0	0x27			/* 从当前USB中断的端点缓冲区读取数据块 */
/* 输出: 长度, 数据流 */
 
#define	CMD_RD_USB_DATA		0x28			/* 从当前USB中断的端点缓冲区读取数据块, 并释放缓冲区, 相当于 CMD_RD_USB_DATA0 + CMD_UNLOCK_USB */
/* 输出: 长度, 数据流 */
 
#define	CMD_WR_USB_DATA3	0x29			/* 设备方式: 向USB端点0的发送缓冲区写入数据块 */
/* 输入: 长度, 数据流 */
 
#define	CMD_WR_USB_DATA5	0x2A			/* 设备方式: 向USB端点1的发送缓冲区写入数据块 */
/* 输入: 长度, 数据流 */
 
#define	CMD_WR_USB_DATA7	0x2B			/* 向USB端点2或者主机端点的发送缓冲区写入数据块 */
/* 输入: 长度, 数据流 */
 
/* ************************************************************************** */
/* 以下命令用于USB主机方式, 只有CH375支持 */
 
#define	CMD_SET_BAUDRATE	0x02			/* 主机方式 & 串口方式: 设置串口通讯波特率 */
/* 输入: 波特率分频系数, 波特率分频常数 */
/* 输出: 操作状态( CMD_RET_SUCCESS或CMD_RET_ABORT, 其它值说明操作未完成 ) */
 
#define	CMD_GET_DEV_RATE	0x0A			/* 主机方式: 获取当前连接的USB设备的数据速率类型 */
/* 输入: 数据07H */
/* 输出: 数据速率类型 */
/*                    位4为1则是1.5Mbps低速USB设备, 否则是12Mbps全速USB设备 */
 
#define	CMD_GET_MAX_LUN		0x0A			/* 主机方式: 获取USB存储器最大逻辑单元号 */
/* 输入: 数据38H */
/* 输出: 最大逻辑单元号 */
 
#define	CMD_SET_RETRY		  0x0B			/* 主机方式: 设置USB事务操作的重试次数 */
/* 输入: 数据25H, 重试次数 */
/*                    位7为0则收到NAK时不重试, 位7为1位6为0则收到NAK时无限重试, 位7为1位6为1则收到NAK时最多重试2秒, 位5~位0为超时后的重试次数 */
 
#define	CMD_SET_DISK_LUN	0x0B			/* 主机方式: 设置USB存储器的当前逻辑单元号 */
/* 输入: 数据34H, 新的当前逻辑单元号(00H-0FH) */
 
#define	CMD_SET_PKT_P_SEC	0x0B			/* 主机方式: 设置USB存储器的每扇区数据包总数 */
/* 输入: 数据39H, 新的每扇区数据包总数(08H,10H,20H,40H) */
 
#define	CMD_TEST_CONNECT	0x16			/* 主机方式: 检查USB设备连接状态 */
/* 输出: 状态( USB_INT_CONNECT或USB_INT_DISCONNECT或USB_INT_USB_READY, 其它值说明操作未完成 ) */
 
#define	CMD_ABORT_NAK		  0x17			/* 主机方式: 放弃当前NAK的重试 */
 
#define	CMD_CLR_STALL		  0x41			/* 主机方式: 控制传输-清除端点错误 */
/* 输入: 端点号 */
/* 输出中断 */
 
#define	CMD_SET_ADDRESS		0x45			/* 主机方式: 控制传输-设置USB地址 */
/* 输入: 地址值 */
/* 输出中断 */
 
#define	CMD_GET_DESCR		  0x46			/* 主机方式: 控制传输-获取描述符 */
/* 输入: 描述符类型 */
/* 输出中断 */
 
#define	CMD_SET_CONFIG		0x49			/* 主机方式: 控制传输-设置USB配置 */
/* 输入: 配置值 */
/* 输出中断 */
 
#define	CMD_AUTO_SETUP		0x4D			/* 主机方式: 自动配置USB设备 */
/* 输出中断 */
 
#define	CMD_ISSUE_TKN_X		0x4E			/* 主机方式: 发出同步令牌, 执行事务, 该命令可代替 CMD_SET_ENDP6/CMD_SET_ENDP7 + CMD_ISSUE_TOKEN */
/* 输入: 同步标志, 事务属性 */
/*           同步标志的位7为主机端点IN的同步触发位, 位6为主机端点OUT的同步触发位, 位5~位0必须为0 */
/*           事务属性的低4位是令牌, 高4位是端点号 */
/* 输出中断 */
 
#define	CMD_ISSUE_TOKEN		0x4F			/* 主机方式: 发出令牌, 执行事务 */
/* 输入: 事务属性 */
/*           低4位是令牌, 高4位是端点号 */
/* 输出中断 */
 
#define	CMD_DISK_BOC_CMD	0x50			/* 主机方式: 对USB存储器执行BulkOnly传输协议的命令 */
/* 输出中断 */
 
#define	CMD_DISK_INIT		  0x51			/* 主机方式: 初始化USB存储器 */
/* 输出中断 */
 
#define	CMD_DISK_RESET		0x52			/* 主机方式: 控制传输-复位USB存储器 */
/* 输出中断 */
 
#define	CMD_DISK_SIZE		  0x53			/* 主机方式: 获取USB存储器的容量 */
/* 输出中断 */
 
#define	CMD_DISK_READ		  0x54			/* 主机方式: 从USB存储器读数据块(以扇区为单位) */
/* 输入: LBA扇区地址(总长度32位, 低字节在前), 扇区数(01H~FFH) */
/* 输出中断 */
 
#define	CMD_DISK_RD_GO		0x55			/* 主机方式: 继续执行USB存储器的读操作 */
/* 输出中断 */
 
#define	CMD_DISK_WRITE		0x56			/* 主机方式: 向USB存储器写数据块(以扇区为单位) */
/* 输入: LBA扇区地址(总长度32位, 低字节在前), 扇区数(01H~FFH) */
/* 输出中断 */
 
#define	CMD_DISK_WR_GO		0x57			/* 主机方式: 继续执行USB存储器的写操作 */
/* 输出中断 */
 
#define	CMD_DISK_INQUIRY	0x58			/* 主机方式: 查询USB存储器特性 */
/* 输出中断 */
 
#define	CMD_DISK_READY		0x59			/* 主机方式: 检查USB存储器就绪 */
/* 输出中断 */
 
#define	CMD_DISK_R_SENSE	0x5A			/* 主机方式: 检查USB存储器错误 */
/* 输出中断 */
 
#define	CMD_DISK_MAX_LUN	0x5D			/* 主机方式: 控制传输-获取USB存储器最大逻辑单元号 */
/* 输出中断 */
 
/* ********************************************************************************************************************* */
/* 操作状态 */
 
#define	CMD_RET_SUCCESS		0x51			/* 命令操作成功 */
#define	CMD_RET_ABORT		  0x5F			/* 命令操作失败 */
 
/* ********************************************************************************************************************* */
/* USB中断状态 */
 
#ifndef	USB_INT_EP0_SETUP
 
/* 以下状态代码为特殊事件中断, 如果通过CMD_CHK_SUSPEND启用USB总线挂起检查, 那么必须处理USB总线挂起和睡眠唤醒的中断状态 */
#define	USB_INT_USB_SUSPEND	0x05			/* USB总线挂起事件 */
#define	USB_INT_WAKE_UP		  0x06			/* 从睡眠中被唤醒事件 */
 
/* 以下状态代码0XH用于USB设备方式 */
/*   内置固件模式下只需要处理: USB_INT_EP1_OUT, USB_INT_EP1_IN, USB_INT_EP2_OUT, USB_INT_EP2_IN */
/*   位7-位4为0000 */
/*   位3-位2指示当前事务, 00=OUT, 10=IN, 11=SETUP */
/*   位1-位0指示当前端点, 00=端点0, 01=端点1, 10=端点2, 11=USB总线复位 */
#define	USB_INT_EP0_SETUP	  0x0C			/* USB端点0的SETUP */
#define	USB_INT_EP0_OUT		  0x00			/* USB端点0的OUT */
#define	USB_INT_EP0_IN		  0x08			/* USB端点0的IN */
#define	USB_INT_EP1_OUT		  0x01			/* USB端点1的OUT */
#define	USB_INT_EP1_IN		  0x09			/* USB端点1的IN */
#define	USB_INT_EP2_OUT		  0x02			/* USB端点2的OUT */
#define	USB_INT_EP2_IN		  0x0A			/* USB端点2的IN */
/* USB_INT_BUS_RESET	0x0000XX11B */		/* USB总线复位 */
#define	USB_INT_BUS_RESET1	0x03			/* USB总线复位 */
#define	USB_INT_BUS_RESET2	0x07			/* USB总线复位 */
#define	USB_INT_BUS_RESET3	0x0B			/* USB总线复位 */
#define	USB_INT_BUS_RESET4	0x0F			/* USB总线复位 */
 
#endif
 
/* 以下状态代码2XH-3XH用于USB主机方式的通讯失败代码, 仅CH375支持 */
/*   位7-位6为00 */
/*   位5为1 */
/*   位4指示当前接收的数据包是否同步 */
/*   位3-位0指示导致通讯失败时USB设备的应答: 0010=ACK, 1010=NAK, 1110=STALL, 0011=DATA0, 1011=DATA1, XX00=超时 */
/* USB_INT_RET_ACK	0x001X0010B */			/* 错误:对于IN事务返回ACK */
/* USB_INT_RET_NAK	0x001X1010B */			/* 错误:返回NAK */
/* USB_INT_RET_STALL	0x001X1110B */		/* 错误:返回STALL */
/* USB_INT_RET_DATA0	0x001X0011B */		/* 错误:对于OUT/SETUP事务返回DATA0 */
/* USB_INT_RET_DATA1	0x001X1011B */		/* 错误:对于OUT/SETUP事务返回DATA1 */
/* USB_INT_RET_TOUT	0x001XXX00B */			/* 错误:返回超时 */
/* USB_INT_RET_TOGX	0x0010X011B */			/* 错误:对于IN事务返回数据不同步 */
/* USB_INT_RET_PID	0x001XXXXXB */			/* 错误:未定义 */
 
/* 以下状态代码1XH用于USB主机方式的操作状态代码, 仅CH375支持 */
#ifndef	USB_INT_SUCCESS
#define	USB_INT_SUCCESS		  0x14			/* USB事务或者传输操作成功 */
#define	USB_INT_CONNECT		  0x15			/* 检测到USB设备连接事件 */
#define	USB_INT_DISCONNECT	0x16			/* 检测到USB设备断开事件 */
#define	USB_INT_BUF_OVER	  0x17			/* USB控制传输的数据太多, 缓冲区溢出 */
#define	USB_INT_USB_READY	  0x18			/* USB设备已经被初始化（已分配USB地址） */
#define	USB_INT_DISK_READ	  0x1D			/* USB存储器读数据块, 请求数据读出 */
#define	USB_INT_DISK_WRITE	0x1E			/* USB存储器写数据块, 请求数据写入 */
#define	USB_INT_DISK_ERR	  0x1F			/* USB存储器操作失败 */
#endif
 
/* ********************************************************************************************************************* */
/* 常用USB定义 */
 
/* USB的包标识PID, 主机方式可能用到 */
#ifndef	DEF_USB_PID_SETUP
#define	DEF_USB_PID_NULL	  0x00			/* 保留PID, 未定义 */
#define	DEF_USB_PID_SOF		  0x05
#define	DEF_USB_PID_SETUP	  0x0D
#define	DEF_USB_PID_IN		  0x09
#define	DEF_USB_PID_OUT		  0x01
#define	DEF_USB_PID_ACK		  0x02
#define	DEF_USB_PID_NAK		  0x0A
#define	DEF_USB_PID_STALL	  0x0E
#define	DEF_USB_PID_DATA0	  0x03
#define	DEF_USB_PID_DATA1	  0x0B
#define	DEF_USB_PID_PRE		  0x0C
#endif
 
/* USB请求类型, 外置固件模式可能用到 */
#ifndef	DEF_USB_REQ_TYPE
#define	DEF_USB_REQ_READ	  0x80			/* 控制读操作 */
#define	DEF_USB_REQ_WRITE	  0x00			/* 控制写操作 */
#define	DEF_USB_REQ_TYPE	  0x60			/* 控制请求类型 */
#define	DEF_USB_REQ_STAND	  0x00			/* 标准请求 */
#define	DEF_USB_REQ_CLASS	  0x20			/* 设备类请求 */
#define	DEF_USB_REQ_VENDOR	  0x40			/* 厂商请求 */
#define	DEF_USB_REQ_RESERVE	  0x60			/* 保留请求 */
#endif
 
/* USB标准设备请求, RequestType的位6位5=00(Standard), 外置固件模式可能用到 */
#ifndef	DEF_USB_GET_DESCR
#define	DEF_USB_CLR_FEATURE	0x01
#define	DEF_USB_SET_FEATURE	0x03
#define	DEF_USB_GET_STATUS	0x00
#define	DEF_USB_SET_ADDRESS	0x05
#define	DEF_USB_GET_DESCR	  0x06
#define	DEF_USB_SET_DESCR	  0x07
#define	DEF_USB_GET_CONFIG	0x08
#define	DEF_USB_SET_CONFIG	0x09
#define	DEF_USB_GET_INTERF	0x0A
#define	DEF_USB_SET_INTERF	0x0B
#define	DEF_USB_SYNC_FRAME	0x0C
#endif
 
typedef union 					//请求包结构
{	
	struct
	{	
		uint8_t  bmRequestType;
		uint8_t  bRequest;
		uint16_t wValue;
		uint16_t wIndex;
		uint16_t wLength;
	}Req;
	uint8_t Req_buf[8];
}_REQUEST;
 
#pragma pack(1)
//USB标准设备描述符
typedef struct _USB_DEVICE_DESCRIPTOR 
{
	uint8_t   bLength;            //长度
	uint8_t   bDescriptorType;    //类型
	uint16_t  bcdUSB;          
	uint8_t   bDeviceClass;
	uint8_t   bDeviceSubClass;
	uint8_t   bDeviceProtocol;
	uint8_t   bMaxPacketSize0;    //最大包长度
	uint16_t  idVendor;           //设备VID
	uint16_t  idProduct;          //设备PID
	uint16_t  bcdDevice;          //设备PVN
	uint8_t   iManufacturer;
    uint8_t   iProduct;
	uint8_t   iSerialNumber;
	uint8_t   bNumConfigurations;
} USB_DEV_DESCR, *PUSB_DEV_DESCR;
 //配置描述符1_基本描述
typedef struct _USB_CONFIG_DESCRIPTOR 
{
	uint8_t   bLength;
	uint8_t   bDescriptorType;
	uint16_t  wTotalLength;
	uint8_t   bNumInterfaces;
	uint8_t   bConfigurationvalue;
	uint8_t   iConfiguration;
	uint8_t   bmAttributes;
	uint8_t   MaxPower;
} USB_CFG_DESCR, *PUSB_CFG_DESCR;
 //配置描述符2_自定义接口描述 
typedef struct _USB_INTERF_DESCRIPTOR
{
	uint8_t   bLength;
	uint8_t   bDescriptorType;
	uint8_t   bInterfaceNumber;
	uint8_t   bAlternateSetting;
	uint8_t   bNumEndpoints;
	uint8_t   bInterfaceClass;
	uint8_t   bInterfaceSubClass;
	uint8_t   bInterfaceProtocol;
	uint8_t   iInterface;
}USB_ITF_DESCR, *PUSB_ITF_DESCR;
 //配置描述符2_HID设备描述 
typedef struct _USB_HID_DESCRIPTOR
{
	uint8_t   bLength;           //长度 
	uint8_t   bDescriptorType1;  //类型
	uint16_t  bcdHID;            //
	uint8_t   bCountryCode;      //国家代码
	uint8_t   bNumDescriptors;   //
	uint8_t   bDescriptorType2;  //
	uint8_t   wItemLength;       //
	uint8_t   iInterface;        //
}USB_HID_DESCR, *PUSB_HID_DESCR;

 //配置描述符2_端点描述
typedef struct _USB_ENDPOINT_DESCRIPTOR 
{
	uint8_t   bLength;          //长度
	uint8_t   bDescriptorType;  //描述类型
	uint8_t   bEndpointAddress; //端点地址
	uint8_t   bmAttributes;     //中断端点
	uint16_t  wMaxPacketSize;   //最大包长
	uint8_t   bInterval;        //询问间隔
}USB_ENDP_DESCR, *PUSB_ENDP_DESCR;

#define USBH_MAX_NUM_INTERFACES               2  //最大接口数量
#define USBH_MAX_NUM_ENDPOINTS                2
typedef struct _USB_CONFIG_DESCRIPTOR_LONG 
{
	USB_CFG_DESCR cfg_descr;
	USB_ITF_DESCR itf_descr;
	USB_HID_DESCR hid_descr[USBH_MAX_NUM_INTERFACES];
	USB_ENDP_DESCR endp_descr[USBH_MAX_NUM_INTERFACES][USBH_MAX_NUM_ENDPOINTS];
} USB_CFG_DESCR_LONG, *PUSB_CFG_DESCR_LONG;
#pragma pack()
 
typedef  struct  _DescHeader 
{
    uint8_t  bLength;       
    uint8_t  bDescriptorType;
} 
USBH_DescHeader_t;

typedef enum
{
	Invalid_Device = 0,//未启用的 USB 设备方式（上电或复位后的默认方式）;
	ExtFM_Device,      //已启用的 USB 设备方式，外部固件模式；
	IntFM_Device,      //已启用的 USB 设备方式，内置固件模式；
	Invalid_Host = 4,  //未启用的 USB 主机方式；
	NoSOF_Host,        //已启用的 USB 主机方式，不产生 SOF 包；
	AutoSOF_Host,      //已启用的 USB 主机方式，自动产生 SOF 包；
	RstBUS_Host,       //已启用的 USB 主机方式，复位 USB 总线；
}USB_Mode_Type;
 
typedef struct
{
	uint8_t         Version;              //版本号
	USB_Mode_Type   USB_Mode;
	uint8_t         Endp6_Mode;
	uint8_t         Endp7_Mode;
	uint8_t         USB_Buffer[64];
	uint8_t         USB_DataIn_Buffer[64];
	uint8_t         USB_DataIn_Length;
	uint8_t         USB_DataIn_Flag;
}CH375_USB_DEF;
 
extern CH375_USB_DEF   CH375_USB_Info;
extern USB_CFG_DESCR_LONG    CH_DESCR;
extern USB_DEV_DESCR         CH_HostDev;

#define  LE16(addr)             (((u16)(*((u8 *)(addr))))\
                                + (((u16)(*(((u8 *)(addr)) + 1))) << 8))
typedef enum {
  USBH_OK   = 0,
  USBH_BUSY,
  USBH_FAIL,
  USBH_NOT_SUPPORTED,
  USBH_UNRECOVERED_ERROR,
  USBH_ERROR_SPEED_UNKNOWN,
  USBH_APPLY_DEINIT
}USBH_Status;


//=============================
void  USBH_ParseDevDesc (USB_DEV_DESCR* dev_desc,uint8_t *buf, uint16_t length);
USBH_Status USBH_ParseCfgDesc (USB_CFG_DESCR* cfg_desc,USB_ITF_DESCR* itf_desc,USB_ENDP_DESCR ep_desc[][2],USB_HID_DESCR *hid_descr, uint8_t *buf, uint16_t length);
//=============================







void CH375_IO_Init(void);                                      //管脚初始化
uint8_t Bsp_CH375_Get_DeviceVersion(void);                     //获取版本ID
uint8_t Bsp_CH375_Get_INTStatus(void);                         /* 等待37X产生中断 */
uint8_t Bsp_CH375_Wait_INT(void);                              //等待中断
uint8_t Bsp_CH375_Read_Data(void);                             //读取数据
void Bsp_CH375_Write_Cmd(uint8_t cmd );                        //写命令
void Bsp_CH375_Write_Data(uint8_t data );                      //写数据
void BSP_CH375_Toggle_Send(void);                              //主机发送成功后,切换DATA0和DATA1实现数据同步
void BSP_CH375_Toggle_Recv(void);                              //主机接收成功后,切换DATA0和DATA1实现数据同步
void Bsp_CH375_Write_USBData( uint8_t len, uint8_t *buffer );  //向CH37X写入数据块
void Bsp_CH375_Set_Retry(uint8_t num);                         /* 设置对响应NAK的重试次数 */
bool Bsp_CH375_SetUSBMode(USB_Mode_Type mode);                //设置CH375的工作模式
bool Bsp_CH375_Reset_Device(void);                             /* USB总线复位 */
 
uint8_t Bsp_CH375_SetConfig( uint8_t cfg );                    /* 设置设备端的USB配置 */
uint8_t Bsp_CH375_SetAddr( uint8_t addr );                     /* 设置设备端的USB地址 */
uint8_t Bsp_CH375_Get_USBDescr( uint8_t type );                /* 从设备端获取描述符 */
uint8_t Bsp_CH375_IssueToken(uint8_t endp_and_pid);            /* 执行USB事务 */
uint8_t Bsp_CH375_Read_USBData( uint8_t *buffer );             /* 主机接收, 返回长度 */
void Bsp_CH375_Host_Send( uint8_t len, uint8_t *buffer );      /* 主机发送 */
uint8_t Bsp_CH375_GetFreq(void);                               /* USB总线复位 加 获取速度参数 */
 
 
 
 
 
#if 1
 
 //函数返回值
#define ERRO_PRAM      0xF9
#define ERRO_UNKNOWN   0xFB
#define EDPMOUNTOVER   0xFC
#define ITFMOUNTOVER   0xFE
#define USB_CMD_ERR    0xFF

/* 设备速度 */
#define LOW_SPEED    1
#define FULL_SPEED   0

/* 设备类型 */
#define USB_DEV_CLASS_RESERVED  0x00
#define USB_DEV_CLASS_AUDIO     0x01
#define USB_DEV_CLASS_COMMUNIC  0x02
#define USB_DEV_CLASS_HID       0x03
#define USB_DEV_CLASS_MONITOR   0x04
#define USB_DEV_CLASS_PHYSIC_IF 0x05
#define USB_DEV_CLASS_POWER     0x06
#define USB_DEV_CLASS_PRINTER   0x07
#define USB_DEV_CLASS_STORAGE   0x08
#define USB_DEV_CLASS_HUB       0x09
#define USB_DEV_CLASS_VEN_SPEC  0xFF


/* 设备状态 */
#define USBD_UNCONNECT   0
#define USBD_CONNECT     1
#define USBD_READY       2

/* 用户配置区 */
#define  MAXEDPMOUNT     2       //最大端点数目
#define  MAXITFMOUNT0     2       //最大接口数量
//定义相应的，选择使用集成命令形式，否则使用控制传输方式
#define UseSysGetDevDesc         //获取设备描述符（建议定义）
#define UseSysSetDevAddr         //设置地址
//#define UseSysGetCfgDesc         //获取配置描述符（不建议定义，防止某些描述符太长，导致缓冲区溢出）
#define UseSysSetDevConf         //设置配置


/* 设备信息 */
typedef struct ENDP_INFO_
{
	uint8_t edpnum;   //端点号
	uint8_t tog;      //同步标志位
	uint8_t attr;     //端点属性
	uint8_t maxpack;  //最大包大小（最大支持64）
} ENDP_INFO;
typedef struct ITF_INFO_
{
	uint8_t iftnum;    //接口号
	uint8_t dclass;    //设备类
	uint8_t dsubclass; //设备子类
	uint8_t dprotocol; //协议码
	uint8_t edpmount;  //端点数量
	ENDP_INFO edp[ MAXEDPMOUNT ]; //端点结构体
} ITF_INFO;
//typedef struct DEVICE_INFO_
//{
//	uint8_t status;   //0:未连接  1：连接但未初始化  2：连接并且已经初始化
//	uint8_t speed;    //0:全速    1：低速
//	uint8_t addr;     //设备地址
//	uint8_t endp0_maxpack;  //端点0最大包大小 
////	uint8_t devtype;  //设备类型
//	uint8_t cgfvalue; //配置值
//	uint16_t dvid;    //厂商
//	uint16_t dpid;    //产品
//	uint8_t itfmount; //接口数量
//	ITF_INFO itf[ MAXITFMOUNT0 ];  //接口结构体
//} DEVICE_INFO_;
 #endif
 
 
 
 
 
 
#endif

/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

