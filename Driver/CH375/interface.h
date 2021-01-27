#ifndef _INTERFACE_H_
#define _INTERFACE_H_

#include "CH375INC.H"
#include "sys.h"
#include "delay.h"


#ifndef		TRUE
#define		TRUE	1
#define		FALSE	0
#endif

#define ERR_USB_UNKNOWN  0xFA

extern void delay_us(uint32_t nus);
extern void delay_ms(uint16_t nms);
extern void CH375_PORT_INIT( void );  		/* CH375通讯接口初始化 */
extern void xWriteCH375Cmd( uint8_t mCmd );	/* 向CH375写命令 */
extern void xWriteCH375Data( uint8_t mData );	/* 向CH375写数据 */
extern uint8_t	xReadCH375Data( void );			/* 从CH375读数据 */
extern uint8_t	Query375Interrupt( void );		/* 查询CH375中断(INT#引脚为低电平) */
extern uint8_t	mInitCH375Host( void );			/* 初始化CH375 */

#endif

