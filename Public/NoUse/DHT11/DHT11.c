#include "DHT11.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>
#include "usart.h"
float DHT_temperature = 0; //�¶�
float DHT_humidity    = 0; //ʪ��

#if 0

//IO����Ϊ���
void DHTIO_Out(void);

void GPIO_DHT_Out_Mode(void);
void DHT11_Init(void)
{
    GPIO_DHT_Out_Mode();
    PBout(8) = 1;
    delay_ms(1000);
}

#define SetDatIOOut    DHTIO_Out
#define SetDatIOIn     DHTIO_In
#define DatIOOut       PBout(9)
#define DatIOIn        PBin(9)

void GPIO_DHT_Out_Mode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //��©���
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void GPIO_DHT_Input_Mode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//----------------------------------------------------------------------------------------------
//--- name	:	DHT11WriteStart
//--- ����	:	��DHT11д��һ����ȡ���ݵ�������
//----------------------------------------------------------------------------------------------
void DHT11WriteStart()
{
    GPIO_DHT_Out_Mode();
    DatIOOut = 1;
    DatIOOut = 0;
    delay_ms(25);//���͵�ƽ����18ms
    DatIOOut = 1;
    delay_us(30);
}
//----------------------------------------------------------------------------------------------
//--- name		:	DHT11ReadByte
//--- ����		:	��DHT11�ж�ȡ��һ���ֽ�
//--- ����ֵ	:	��ȡ��һ���ֽڵ�����
//----------------------------------------------------------------------------------------------
u8 DHT11ReadByte(void)
{
    u8 temp = 0, i, j = 0;
    for(i = 0; i < 8; i++) {
        temp <<= 1;
        while(0 == DatIOIn);//�ȴ���ߵ�ƽ
        while(1 == DatIOIn) { //����ߵ�ƽʱ��
            delay_us(1);
            j++;
        }
        if(j >= 30) {    //����30usȷ��Ϊ1
            temp = temp | 0x01;
            j = 0;
        }
        j = 0;
    }
    return temp;
}
//----------------------------------------------------------------------------------------------
//--- name		:	DHT11Read(u8 *RH_temp,u8 *RL_temp,u8 *TH_temp,u8 *TL_temp,u8 *CK_temp)
//--- ����		:	��DHT11�ж�ȡ����
//--- ˵��		:	���Թ����з����¶���ֵ���䣬С��ֵ�����㣬��ģ��δ���Գɹ���
//----------------------------------------------------------------------------------------------
void DHT11Read(u8 *RH_temp, u8 *RL_temp, u8 *TH_temp, u8 *TL_temp, u8 *CK_temp)
{
    DHT11WriteStart();//����ȡǰ���ź�
    GPIO_DHT_Input_Mode();//���ö˿�Ϊ����״̬
    if(!DatIOIn) {
        while(0 == DatIOIn);//�͵�ƽ����Ӧ�źţ�80us
        while(1 == DatIOIn);//��������80us�ĸߵ�ƽ����׼���ź�

        *RH_temp = DHT11ReadByte();//ʪ�ȸ�8λ
        *RL_temp = DHT11ReadByte();//ʪ�ȵ�8λ
        *TH_temp = DHT11ReadByte();//�¶ȸ�8λ
        *TL_temp = DHT11ReadByte();//�¶ȵ�8λ
        *CK_temp = DHT11ReadByte();//У���
        GPIO_DHT_Out_Mode();
        DatIOOut = 1;
        //����У��
        //untemp= *RH_temp+RL_temp+*TH_temp+TL_temp;
    }
}
void Read_DHT11(void)
{
    u8 TempDHT[5] = {0};

    DHT11Read(&TempDHT[0], &TempDHT[1], &TempDHT[2], &TempDHT[3], &TempDHT[4]);

    //���ݽ���״̬
    //�¶�����λ
    //�¶�С��λ
    //ʪ������λ
    //ʪ��С��λ
    //У���
    if(TempDHT[4] == (TempDHT[0] + TempDHT[1] + TempDHT[2] + TempDHT[3])) {
        DHT_temperature = TempDHT[1];
        DHT_temperature /= 1000; //С����
        DHT_temperature += TempDHT[0];

        DHT_humidity = TempDHT[3];
        DHT_humidity /= 1000; //С����
        DHT_humidity += TempDHT[2];
        printf("�¶� %f ʪ�� %f  \n", DHT_temperature, DHT_humidity);

    }
}
#else
static void                           DHT11_GPIO_Config                       ( void );
static void                           DHT11_Mode_IPU                          ( void );
static void                           DHT11_Mode_Out_PP                       ( void );
static uint8_t                        DHT11_ReadByte                          ( void );

#define SetDatIOOut    DHTIO_Out
#define SetDatIOIn     DHTIO_In
#define DatIOOut       PBout(9)
#define DatIOIn        PBin(9)

/**
 * @brief  DHT11 ��ʼ������
 * @param  ��
 * @retval ��
 */
void DHT11_Init ( void )
{
    DHT11_GPIO_Config ();
    DatIOOut = 1;               // ����
}


/*
 * ��������DHT11_GPIO_Config
 * ����  ������DHT11�õ���I/O��
 * ����  ����
 * ���  ����
 */
static void DHT11_GPIO_Config ( void )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init ( GPIOB, &GPIO_InitStructure );
}


/*
 * ��������DHT11_Mode_IPU
 * ����  ��ʹDHT11-DATA���ű�Ϊ��������ģʽ
 * ����  ����
 * ���  ����
 */
static void DHT11_Mode_IPU(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*
 * ��������DHT11_Mode_Out_PP
 * ����  ��ʹDHT11-DATA���ű�Ϊ�������ģʽ
 * ����  ����
 * ���  ����
 */
static void DHT11_Mode_Out_PP(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}


/*
 * ��DHT11��ȡһ���ֽڣ�MSB����
 */
static uint8_t DHT11_ReadByte ( void )
{
    uint8_t i, temp = 0;
	  u16 Cnt = 1;
    for(i = 0; i < 8; i++) {
        /*ÿbit��50us�͵�ƽ���ÿ�ʼ����ѯֱ���ӻ����� ��50us �͵�ƽ ����*/
        while(DatIOIn == Bit_RESET && Cnt){Cnt++;}

        /*DHT11 ��26~28us�ĸߵ�ƽ��ʾ��0������70us�ߵ�ƽ��ʾ��1����
         *ͨ����� x us��ĵ�ƽ��������������״ ��x ���������ʱ
         */
        delay_us(40); //��ʱx us �����ʱ��Ҫ��������0������ʱ�伴��

        if(DatIOIn == Bit_SET) { /* x us����Ϊ�ߵ�ƽ��ʾ���ݡ�1�� */
					  Cnt = 1;   /* �ȴ�����1�ĸߵ�ƽ���� */
            while(DatIOIn == Bit_SET && Cnt){Cnt++;}
            temp |= (uint8_t)(0x01 << (7 - i)); //�ѵ�7-iλ��1��MSB����
        } else {     // x us��Ϊ�͵�ƽ��ʾ���ݡ�0��
            temp &= (uint8_t)~(0x01 << (7 - i)); //�ѵ�7-iλ��0��MSB����
        }
    }
    return temp;

}

DHT11_TypeDef DHT11Dat;
/*
 * һ�����������ݴ���Ϊ40bit����λ�ȳ�
 * 8bit ʪ������ + 8bit ʪ��С�� + 8bit �¶����� + 8bit �¶�С�� + 8bit У���
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_TypeDef *DHT11_Data)
{ 
	  u16 Cnt =1;
    DHT11_Mode_Out_PP(); /*���ģʽ*/
    DatIOOut = 0;        /*��������*/
    delay_ms(18);			   /*��ʱ18ms*/
    DatIOOut = 1; 		   /*�������� ������ʱ30us*/
    delay_us(30);        //��ʱ30us
    DHT11_Mode_IPU();    /*������Ϊ���� �жϴӻ���Ӧ�ź�*/
    /*�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������*/
    if(DatIOIn == Bit_RESET) { 
			  Cnt = 1;
        while(DatIOIn == Bit_RESET && Cnt){Cnt++;}/*��ѯֱ���ӻ����� ��80us �͵�ƽ ��Ӧ�źŽ���*/
			  Cnt = 1;        
				while(DatIOIn == Bit_SET && Cnt){Cnt++;} /*��ѯֱ���ӻ������� 80us �ߵ�ƽ �����źŽ���*/
        /*��ʼ��������*/
        DHT11_Data->humidity1    = DHT11_ReadByte();   //ʪ��
        DHT11_Data->humidity2 	 = DHT11_ReadByte();
        DHT11_Data->temperature1 = DHT11_ReadByte();   //�¶�
        DHT11_Data->temperature2 = DHT11_ReadByte();				
        DHT11_Data->check     = DHT11_ReadByte();        
        DHT11_Mode_Out_PP(); /*��ȡ���������Ÿ�Ϊ���ģʽ*/   
        DatIOOut = 1; /*��������*/
        /*����ȡ�������Ƿ���ȷ*/
        if(DHT11_Data->check == DHT11_Data->temperature1 + DHT11_Data->temperature2 + DHT11_Data->humidity1 + DHT11_Data->humidity2) {
            return SUCCESS;
        } else {
            return ERROR;
        }
    }else {
       return ERROR;
    }
}

void Read_DHT11(void)
{
    if(DHT11_Read_TempAndHumidity(&DHT11Dat) == SUCCESS) {
       COM_Printf(5 ,"ʪ�� %d.%d%%RH �¶� %d.%d��\r\n", DHT11Dat.humidity1, DHT11Dat.humidity2,DHT11Dat.temperature1, DHT11Dat.temperature2);
    } else {
			 DHT11Dat.temperature1 = 0xFF; //�¶�
			 DHT11Dat.temperature2 = 0xFF; 
       DHT11Dat.humidity1 = 0xFF;   //ʪ��
       DHT11Dat.humidity2 = 0xFF;			
       COM_Printf(5 ,"��ʪ��ģ�����\r\n");
    }
}

/*************************************END OF FILE******************************/

#endif

