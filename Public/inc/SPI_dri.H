#ifndef _SPI_Dri_H
#define _SPI_Dri_H

#include "stm32f10x.h"



/*
SPI是一种高速，主从式，全双工，同步传输的通信总线

SPI总线在物理层体现为4根传输线，分别是CS线 ， SCLK线，MOSI线和MOSO线

CS：Chip Select，即片选线
SCLK：Serial Clock ，即串行时钟线
MOSI：Master Output Slaver  Input，指设备扮演主机是作为数据输出线，而设备扮演从机时作为数据输入线
MISO：Master Input  Slaver Output，指设备扮演主机是作为数据输入线，而设备扮演从机时作为数据输出线

------------------------------------------------------|
STM32F1---SPI                                         |
-----|-------|--------------|----------|--------------|
     | SPI_1 | 映射SPI_1    |  SPI_2   |   SPI_2      |
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

对于SPI ，需要打开相关RCC时钟
主模式下
CLK 配置成复用推挽输出
MOSI 配置成复用推挽输出
MISO 配置成上拉输入
CS若采用硬件则配置成推挽输出，若采用软件模式，则采用普通IO推挽输出即可


*/
//选择SPI号
typedef enum
{
    SPIn1 = 0,
    SPIn2 = 1,
    SPIn3 = 2
} SPIn_TypeDef;

//void SPI1_Init(void);
//void SPIn2_Init(void);

//void FM25CL64B_Write_Enable(void);            //写使能
//void FM25CL64B_Write_Disable(void);           //写禁止

//void FM25CL64B_Write_stateRegister(u8 Wdat_S);//写状态寄存器
//u8 FM25CL64B_Read_stateRegister(void);        //读状态寄存器

//void FM25CL64B_WriteByte(u32 WAddr ,u8 Dat);  //写数据
//u8 FM25CL64B_ReadByte(u32 WAddr );            //读取数据


//管脚，外设初始化
void fm25CL64_gpio_Init(void);
u8 FM25CL64B_Text(u32 WAddr ,u8 Dat);         //测试数据存储与读取
void FM25WriteString(u32 WAddr, u8 *WDat ,u8 len); //写一串数据
void FM25ReadString(u32 WAddr, u8 *RDat ,u8 len);  //读一串数据

#endif

