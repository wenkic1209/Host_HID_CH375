#include "SPI_dri.h"

u8 SPI_Send_Byte(u8 WRDat);

void SPI1_Init(void);
void SPIn2_Init(void);
void FM25CL64B_Write_Enable(void);            //дʹ��
void FM25CL64B_Write_Disable(void);           //д��ֹ
void FM25CL64B_Write_stateRegister(u8 Wdat_S);//д״̬�Ĵ���
u8 FM25CL64B_Read_stateRegister(void);        //��״̬�Ĵ���
void FM25CL64B_WriteByte(u32 WAddr, u8 Dat);  //д����
u8 FM25CL64B_ReadByte(u32 WAddr );            //��ȡ����
u8 FM25CL64B_Text(u32 WAddr, u8 Dat);         //�������ݴ洢���ȡ

/*
SPI_1  :APB2����

SPI_2/3:APB1����

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

    GPIO_SetBits(GPIOA, GPIO_Pin_4);	//��ʼƬѡ��ƽ
    //SPI��ʼ��
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //����SPI�������շ�ģʽ
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master ;                  //����SPI����ģʽ
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b ;                  //����SPI�����ݿ�ȴ�С
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_High   ;                  //���ô���ʱ�ӿ���ʱ���ֵ�ƽ״̬
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //�������ݲ���ʱ��ʱ�ӱ���
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft    ;                  //ָ��NSS�ź�(��CS)��Ӳ�����ƣ������������
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;   //���岨����Ԥ��Ƶֵ
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //ָ�����ݴ����MSBλ����LSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //��������CRCֵ����Ķ���ʽ

    SPI_Init(SPI1, &SPI_InitStructure);
    SPI_Cmd(SPI1, ENABLE); //ʹ��SPI�豸
    SPI_Send_Byte(0xFF);	 //��������
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

    GPIO_SetBits(GPIOB, GPIO_Pin_12); //��ʼƬѡ��ƽ
    //SPI��ʼ��
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   //����SPI�������շ�ģʽ
    SPI_InitStructure.SPI_Mode      = SPI_Mode_Master ;                  //����SPI����ģʽ
    SPI_InitStructure.SPI_DataSize  = SPI_DataSize_8b ;                  //����SPI�����ݿ�ȴ�С
    SPI_InitStructure.SPI_CPOL      = SPI_CPOL_Low    ;                  //���ô���ʱ�ӿ���ʱ���ֵ�ƽ״̬
    SPI_InitStructure.SPI_CPHA      = SPI_CPHA_2Edge  ;                  //�������ݲ���ʱ��ʱ�ӱ���
    SPI_InitStructure.SPI_NSS       = SPI_NSS_Soft    ;                  //ָ��NSS�ź�(��CS)��Ӳ�����ƣ������������
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;   //���岨����Ԥ��Ƶֵ
    SPI_InitStructure.SPI_FirstBit  = SPI_FirstBit_MSB;                  //ָ�����ݴ����MSBλ����LSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             //��������CRCֵ����Ķ���ʽ

    SPI_Init(SPI2, &SPI_InitStructure);
    SPI_Cmd(SPI2, ENABLE); //ʹ��SPI�豸
    SPI_Send_Byte(0xFF);	//��������
}




#define   FM25CL64_SPIRead        SPI_I2S_ReceiveData(SPI1,Dat)
#define   FM25CL64_SPIWrite(Dat)  SPI_I2S_SendData(SPI1,Dat)


#define		FM25L64B_CS			GPIOA, GPIO_Pin_4
#define		FM25L64B_DELAY		2
//FM25L16B����                   //
#define  	FM25L64B_WREN		0x06   //����дʹ��������
#define  	FM25L64B_WRDI		0x04   //д��ֹ
#define  	FM25L64B_RDSR		0x05   //��״̬�Ĵ���
#define  	FM25L64B_WRSR		0x01   //д״̬�Ĵ���
#define  	FM25L64B_READ		0x03   //���洢������
#define  	FM25L64B_WRITE	0x02   //д�洢������


#define 	FM25L64B_DUMMY  0xFF   //��ȡ�������

inline u8 SPI_Send_Byte(u8 WRDat)
{
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == 0); //�ȴ��������
    SPI_I2S_SendData(SPI1, WRDat); //��������
    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == 0); //�ȴ��������
    return (SPI_I2S_ReceiveData(SPI1)); //��������
}


//дʹ��
inline void FM25CL64B_Write_Enable(void)
{
    GPIO_ResetBits(FM25L64B_CS);  //����ʹ��
    SPI_Send_Byte(FM25L64B_WREN); //дʹ������,0x06
    GPIO_SetBits(FM25L64B_CS);    //����ʹ��
}
//д��ֹ
inline void FM25CL64B_Write_Disable(void)
{
    GPIO_ResetBits(FM25L64B_CS);  //����ʹ��
    SPI_Send_Byte(FM25L64B_WRDI); //д��ֹ����,0x04
    GPIO_SetBits(FM25L64B_CS);    //����ʹ��
}
//��״̬�Ĵ���
inline u8 FM25CL64B_Read_stateRegister(void)
{
    u8 Tempdat;
    GPIO_ResetBits(FM25L64B_CS);  //����ʹ��
    SPI_Send_Byte(FM25L64B_RDSR); //��״̬�Ĵ������0x05
    Tempdat = SPI_Send_Byte(FM25L64B_DUMMY); //������䷵������
    GPIO_SetBits(FM25L64B_CS);    //����ʹ��
    return Tempdat;
}
//д״̬�Ĵ���
inline void FM25CL64B_Write_stateRegister(u8 Wdat_S)
{
    GPIO_ResetBits(FM25L64B_CS);  //����ʹ��
    SPI_Send_Byte(FM25L64B_WRSR); //д״̬�Ĵ������0x01
    SPI_Send_Byte(Wdat_S);        //����
    GPIO_SetBits(FM25L64B_CS);    //����ʹ��
}
//��ȡ����
inline u8 FM25CL64B_ReadByte(u32 WAddr )
{
    u8 tempdat;
    GPIO_ResetBits(FM25L64B_CS);  //����ʹ��
    SPI_Send_Byte(FM25L64B_READ); //������,0x03
    SPI_Send_Byte((WAddr >> 8) & 0xFF);
    SPI_Send_Byte((WAddr >> 0) & 0xFF);
    tempdat = SPI_Send_Byte(FM25L64B_DUMMY); //������䷵������
    GPIO_SetBits(FM25L64B_CS);    //����ʹ��
    return tempdat;
}


//д����
inline void FM25CL64B_WriteByte(u32 WAddr, u8 Dat)
{
    u16 i;
    FM25CL64B_Write_Enable(); //дʹ��
    for(i = 0; i < 100; i++) {;}
    GPIO_ResetBits(FM25L64B_CS);  //����ʹ��
    SPI_Send_Byte(FM25L64B_WRITE); //д����,0x02
    SPI_Send_Byte((WAddr >> 8) & 0xFF);
    SPI_Send_Byte((WAddr >> 0) & 0xFF);
    SPI_Send_Byte(Dat); //д����
    GPIO_SetBits(FM25L64B_CS);    //����ʹ��
}

//�ܽţ������ʼ��
void fm25CL64_gpio_Init(void)
{
   SPI1_Init();
}


//�������
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

