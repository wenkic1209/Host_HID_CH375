  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : tm_stm32_fm25cl64.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2021-01-15         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include "tm_fm25cl64.h"
#include "tm_dev_spi.h"

struct dev_spi FM_SPI;
struct dev_spi* m_spi = &FM_SPI;

void FM25CL_SPI_Init(void)
{
	dev_spi_init(m_spi,DEV_SPI1_GPIOA,SPI_MODE_3,SPI_BaudRatePrescaler_256);
}

#define		FM25L64B_DELAY		2
//FM25L16B����                   //
#define  	FM25L64B_WREN		0x06   //����дʹ��������
#define  	FM25L64B_WRDI		0x04   //д��ֹ
#define  	FM25L64B_RDSR		0x05   //��״̬�Ĵ���
#define  	FM25L64B_WRSR		0x01   //д״̬�Ĵ���
#define  	FM25L64B_READ		0x03   //���洢������
#define  	FM25L64B_WRITE	    0x02   //д�洢������


#define 	FM25L64B_DUMMY  0xFF   //��ȡ�������
// ## SPI ������д����
//u8 spi_WriteRead_Byte(u8 WRDat)
//{
//	dev_spi_send8(m_spi, &WRDat, 1); 
//	dev_spi_wait(m_spi);
//	dev_spi_recv8(m_spi, &WRDat, 1);
//	dev_spi_wait(m_spi);
//	return WRDat;
//}

void spi_Write_Byte(u8 WRDat)
{
	dev_spi_send8(m_spi, &WRDat, 1); 
	dev_spi_wait(m_spi);
}

u8 spi_Read_Byte(u8 WRDat)
{
	dev_spi_recv8(m_spi, &WRDat, 1);
	dev_spi_wait(m_spi);
	return WRDat;
}

//дʹ��
inline void fm25cl64_Write_Enable(void)
{
	dev_spi_start(m_spi);              //SPI����ʹ��
    spi_Write_Byte(FM25L64B_WREN);     //дʹ������,0x06
    dev_spi_stop(m_spi);               //����ʹ��
}
//д��ֹ
inline void fm25cl64_Write_Disable(void)
{
    dev_spi_start(m_spi);              //SPI����ʹ��
    spi_Write_Byte(FM25L64B_WRDI);     //д��ֹ����,0x04
    dev_spi_stop(m_spi);               //����ʹ��
}
//��״̬�Ĵ���
inline u8 fm25cl64_Read_stateRegister(void)
{
    u8 Tempdat;
    dev_spi_start(m_spi);              //SPI����ʹ��
    spi_Write_Byte(FM25L64B_RDSR);     //��״̬�Ĵ������0x05
    Tempdat = spi_Read_Byte(FM25L64B_DUMMY); //������䷵������
    dev_spi_stop(m_spi);               //����ʹ��
    return Tempdat;
}
//д״̬�Ĵ���
inline void fm25cl64_Write_stateRegister(u8 Wdat_S)
{
    dev_spi_start(m_spi);              //SPI����ʹ��
    spi_Write_Byte(FM25L64B_WRSR);     //д״̬�Ĵ������0x01
    spi_Write_Byte(Wdat_S);            //����
    dev_spi_stop(m_spi);               //����ʹ��
}
//��ȡ����
u8 fm25cl64_ReadByte(u32 WAddr )
{
    u8 tempdat;
    dev_spi_start(m_spi);                    //SPI����ʹ��
    spi_Write_Byte(FM25L64B_READ);           //������,0x03
    spi_Write_Byte((WAddr >> 8) & 0xFF);
    spi_Write_Byte((WAddr >> 0) & 0xFF);
    tempdat = spi_Read_Byte(FM25L64B_DUMMY); //������䷵������
    dev_spi_stop(m_spi);                     //����ʹ��
    return tempdat;
}


//д����
void fm25cl64_WriteByte(u32 WAddr, u8 Dat)
{
    u16 i;
    fm25cl64_Write_Enable();            //дʹ��
    for(i = 0; i < 100; i++) {;}
    dev_spi_start(m_spi);               //SPI����ʹ��
    spi_Write_Byte(FM25L64B_WRITE);     //д����,0x02
    spi_Write_Byte((WAddr >> 8) & 0xFF);
    spi_Write_Byte((WAddr >> 0) & 0xFF);
    spi_Write_Byte(Dat);                //д����
    dev_spi_stop(m_spi);                //����ʹ��
}





//�������
u8 fm25cl64_Text(u32 WAddr, u8 Dat)
{
    u8 tempdat;
    fm25cl64_WriteByte(WAddr, Dat);
    tempdat = fm25cl64_ReadByte(WAddr);
    return tempdat;
}


//void FM24WriteString(u32 WAddr, u8 *WDat ,u8 len)
//{
//	u8 i = 0;
//	for(i=0;i<len;i++)
//	{
//		fm25cl64_WriteByte(WAddr+i, WDat[i]);
//	}
//}
//void FM24ReadString(u32 WAddr, u8 *RDat ,u8 len)
//{
//	u8 i = 0;
//	for(i=0;i<len;i++)
//	{
//		 RDat[i] = fm25cl64_ReadByte(WAddr+i);
//	}
//}




// 
// 

/********************************End of File************************************/

