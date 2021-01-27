#include "SPI_dri.h"

u8 SPI_Send_Byte(u8 WRDat);

void SPI1_Init(void);
void SPIn2_Init(void);
void FM25CL64B_Write_Enable(void);            //写使能
void FM25CL64B_Write_Disable(void);           //写禁止
void FM25CL64B_Write_stateRegister(u8 Wdat_S);//写状态寄存器
u8 FM25CL64B_Read_stateRegister(void);        //读状态寄存器
void FM25CL64B_WriteByte(u32 WAddr, u8 Dat);  //写数据
u8 FM25CL64B_ReadByte(u32 WAddr );            //读取数据
u8 FM25CL64B_Text(u32 WAddr, u8 Dat);         //测试数据存储与读取

/*
SPI_1  :APB2总线

SPI_2/3:APB1总线

*/


void SPI1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure ;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    /*Configure SPI1 pins: CS */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /*Configure SPI1 pins:  SCK / MISO */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /*Configure SPI1 pins: MOSI*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_SetBits(GPIOA, GPIO_Pin_4);	//初始片选电平
    //SPI初始化
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //设置SPI的数据收发模式
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master ;                  //设置SPI主从模式
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b ;                  //设置SPI的数据宽度大小
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High   ;                  //设置串行时钟空闲时保持电平状态
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //设置数据捕获时的时钟边沿
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft    ;                  //指定NSS信号(即CS)由硬件控制，还是软件控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;   //定义波特率预分频值
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //指定数据传输从MSB位还是LSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //定义用于CRC值计算的多项式

    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE); //使能SPI设备
    SPI_Send_Byte(0xFF);	 //启动传输
}

void SPIn2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure ;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE);

    /*Configure SPI1 pins: CS */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /*Configure SPI1 pins: SCK / MISO   */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13 | GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    /*Configure SPI1 pins: MOSI*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15 ;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_12); //初始片选电平
    //SPI初始化
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //设置SPI的数据收发模式
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master ;                  //设置SPI主从模式
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b ;                  //设置SPI的数据宽度大小
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low    ;                  //设置串行时钟空闲时保持电平状态
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //设置数据捕获时的时钟边沿
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft    ;                  //指定NSS信号(即CS)由硬件控制，还是软件控制
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //定义波特率预分频值
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //指定数据传输从MSB位还是LSB位开始
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //定义用于CRC值计算的多项式

    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE); //使能SPI设备
    SPI_Send_Byte(0xFF);	//启动传输
}




#define   FM25CL64_SPIRead        SPI_I2S_ReceiveData(SPI1,Dat)
#define   FM25CL64_SPIWrite(Dat)  SPI_I2S_SendData(SPI1,Dat)


#define		FM25L64B_CS			GPIOA, GPIO_Pin_4
#define		FM25L64B_DELAY		2
//FM25L16B命令                   //
#define  	FM25L64B_WREN		0x06   //设置写使能锁存器
#define  	FM25L64B_WRDI		0x04   //写禁止
#define  	FM25L64B_RDSR		0x05   //读状态寄存器
#define  	FM25L64B_WRSR		0x01   //写状态寄存器
#define  	FM25L64B_READ		0x03   //读存储器数据
#define  	FM25L64B_WRITE	0x02   //写存储器数据


#define 	FM25L64B_DUMMY  0xFF   //读取数据填充

inline u8 SPI_Send_Byte(u8 WRDat)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); //等待发送完成
    SPI_I2S_SendData(SPI1, WRDat); //发送数据
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0); //等待接受完成
    return (SPI_I2S_ReceiveData(SPI1)); //返回数据
}


//写使能
inline void FM25CL64B_Write_Enable(void)
{
    GPIO_ResetBits(FM25L64B_CS);  //拉低使能
    SPI_Send_Byte(FM25L64B_WREN); //写使能命令,0x06
    GPIO_SetBits(FM25L64B_CS);    //拉高使能
}
//写禁止
inline void FM25CL64B_Write_Disable(void)
{
    GPIO_ResetBits(FM25L64B_CS);  //拉低使能
    SPI_Send_Byte(FM25L64B_WRDI); //写禁止命令,0x04
    GPIO_SetBits(FM25L64B_CS);    //拉高使能
}
//读状态寄存器
inline u8 FM25CL64B_Read_stateRegister(void)
{
    u8 Tempdat;
    GPIO_ResetBits(FM25L64B_CS);  //拉低使能
    SPI_Send_Byte(FM25L64B_RDSR); //读状态寄存器命令，0x05
    Tempdat = SPI_Send_Byte(FM25L64B_DUMMY); //数据填充返回数据
    GPIO_SetBits(FM25L64B_CS);    //拉高使能
    return Tempdat;
}
//写状态寄存器
inline void FM25CL64B_Write_stateRegister(u8 Wdat_S)
{
    GPIO_ResetBits(FM25L64B_CS);  //拉低使能
    SPI_Send_Byte(FM25L64B_WRSR); //写状态寄存器命令，0x01
    SPI_Send_Byte(Wdat_S);        //数据
    GPIO_SetBits(FM25L64B_CS);    //拉高使能
}
//读取数据
inline u8 FM25CL64B_ReadByte(u32 WAddr )
{
    u8 tempdat;
    GPIO_ResetBits(FM25L64B_CS);  //拉低使能
    SPI_Send_Byte(FM25L64B_READ); //读命令,0x03
    SPI_Send_Byte((WAddr >> 8) & 0xFF);
    SPI_Send_Byte((WAddr >> 0) & 0xFF);
    tempdat = SPI_Send_Byte(FM25L64B_DUMMY); //数据填充返回数据
    GPIO_SetBits(FM25L64B_CS);    //拉高使能
    return tempdat;
}


//写数据
inline void FM25CL64B_WriteByte(u32 WAddr, u8 Dat)
{
    u16 i;
    FM25CL64B_Write_Enable(); //写使能
    for(i = 0; i < 100; i++) {;}
    GPIO_ResetBits(FM25L64B_CS);  //拉低使能
    SPI_Send_Byte(FM25L64B_WRITE); //写命令,0x02
    SPI_Send_Byte((WAddr >> 8) & 0xFF);
    SPI_Send_Byte((WAddr >> 0) & 0xFF);
    SPI_Send_Byte(Dat); //写数据
    GPIO_SetBits(FM25L64B_CS);    //拉高使能
}

//管脚，外设初始化
void fm25CL64_gpio_Init(void)
{
   SPI1_Init();
}


//铁电测试
u8 FM25CL64B_Text(u32 WAddr, u8 Dat)
{
    u8 tempdat;
    FM25CL64B_WriteByte(WAddr, Dat);
    tempdat = FM25CL64B_ReadByte(WAddr);
    return tempdat;
}


void FM25WriteString(u32 WAddr, u8 *WDat ,u8 len)
{
	u8 i = 0;
	for(i=0;i<len;i++)
	{
		FM25CL64B_WriteByte(WAddr+i, WDat[i]);
	}
}
void FM25ReadString(u32 WAddr, u8 *RDat ,u8 len)
{
	u8 i = 0;
	for(i=0;i<len;i++)
	{
		 RDat[i] = FM25CL64B_ReadByte(WAddr+i);
	}
}

