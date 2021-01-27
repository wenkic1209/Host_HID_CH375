#ifndef _SPI_Dri_H
#define _SPI_Dri_H

#include "stm32f10x.h"



/*
SPI��һ�ָ��٣�����ʽ��ȫ˫����ͬ�������ͨ������

SPI����������������Ϊ4�������ߣ��ֱ���CS�� �� SCLK�ߣ�MOSI�ߺ�MOSO��

CS��Chip Select����Ƭѡ��
SCLK��Serial Clock ��������ʱ����
MOSI��Master Output Slaver  Input��ָ�豸������������Ϊ��������ߣ����豸���ݴӻ�ʱ��Ϊ����������
MISO��Master Input  Slaver Output��ָ�豸������������Ϊ���������ߣ����豸���ݴӻ�ʱ��Ϊ���������

------------------------------------------------------|
STM32F1---SPI                                         |
-----|-------|--------------|----------|--------------|
     | SPI_1 | ӳ��SPI_1    |  SPI_2   |   SPI_2      |
-----|-------|--------------|----------|--------------|
CS   | PA4   |  PA15        |  PB12    |   PA15       |
-----|-------|--------------|----------|--------------|
SCK  | PA5   | (JTD0)PB3    |  PB13    |  (JTD0)PB3   |
-----|-------|--------------|----------|--------------|
MISO | PA6   | (NJTRST)PB4  |  PB14    |  (NJTRST)PB4 |
-----|-------|--------------|----------|--------------|
MOSI | PA7   |  PB5         |  PB15    |   PB5        |
-----|-------|--------------|----------|--------------|

SPI1->CS    ------ PA4
SPI1->CLK   ------ PA5 
SPI1->MISO  ------ PA6
SPI1->MOSI  ------ PA7

SPI2->CS    ------ PB12
SPI2->CLK   ------ PB13 
SPI2->MISO  ------ PB14 
SPI2->MOSI  ------ PB15

SPI3->CS    ------ PA15  
SPI3->CLK   ------ PB3 
SPI3->MISO  ------ PB4 
SPI3->MOSI  ------ PB5

����SPI ����Ҫ�����RCCʱ��
��ģʽ��
CLK ���óɸ����������
MOSI ���óɸ����������
MISO ���ó���������
CS������Ӳ�������ó��������������������ģʽ���������ͨIO�����������


*/
//ѡ��SPI��
typedef enum
{
    SPIn1 = 0,
    SPIn2 = 1,
    SPIn3 = 2
} SPIn_TypeDef;

//void SPI1_Init(void);
//void SPIn2_Init(void);

//void FM25CL64B_Write_Enable(void);            //дʹ��
//void FM25CL64B_Write_Disable(void);           //д��ֹ

//void FM25CL64B_Write_stateRegister(u8 Wdat_S);//д״̬�Ĵ���
//u8 FM25CL64B_Read_stateRegister(void);        //��״̬�Ĵ���

//void FM25CL64B_WriteByte(u32 WAddr ,u8 Dat);  //д����
//u8 FM25CL64B_ReadByte(u32 WAddr );            //��ȡ����


//�ܽţ������ʼ��
void fm25CL64_gpio_Init(void);
u8 FM25CL64B_Text(u32 WAddr ,u8 Dat);         //�������ݴ洢���ȡ
void FM25WriteString(u32 WAddr, u8 *WDat ,u8 len); //дһ������
void FM25ReadString(u32 WAddr, u8 *RDat ,u8 len);  //��һ������

#endif
