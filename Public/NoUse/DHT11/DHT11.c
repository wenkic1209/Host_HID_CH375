#include "DHT11.h"
#include "sys.h"
#include "delay.h"
#include <stdio.h>
#include "usart.h"
float DHT_temperature = 0; //温度
float DHT_humidity    = 0; //湿度

#if 0

//IO配置为输出
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
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void GPIO_DHT_Input_Mode(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

//----------------------------------------------------------------------------------------------
//--- name	:	DHT11WriteStart
//--- 功能	:	向DHT11写入一个读取数据的引导码
//----------------------------------------------------------------------------------------------
void DHT11WriteStart()
{
    GPIO_DHT_Out_Mode();
    DatIOOut = 1;
    DatIOOut = 0;
    delay_ms(25);//拉低电平至少18ms
    DatIOOut = 1;
    delay_us(30);
}
//----------------------------------------------------------------------------------------------
//--- name		:	DHT11ReadByte
//--- 功能		:	从DHT11中读取到一个字节
//--- 返回值	:	读取到一个字节的数据
//----------------------------------------------------------------------------------------------
u8 DHT11ReadByte(void)
{
    u8 temp = 0, i, j = 0;
    for(i = 0; i < 8; i++) {
        temp <<= 1;
        while(0 == DatIOIn);//等待变高电平
        while(1 == DatIOIn) { //计算高电平时长
            delay_us(1);
            j++;
        }
        if(j >= 30) {    //超过30us确认为1
            temp = temp | 0x01;
            j = 0;
        }
        j = 0;
    }
    return temp;
}
//----------------------------------------------------------------------------------------------
//--- name		:	DHT11Read(u8 *RH_temp,u8 *RL_temp,u8 *TH_temp,u8 *TL_temp,u8 *CK_temp)
//--- 功能		:	从DHT11中读取数据
//--- 说明		:	测试过程中发现温度数值不变，小数值都是零，此模块未测试成功！
//----------------------------------------------------------------------------------------------
void DHT11Read(u8 *RH_temp, u8 *RL_temp, u8 *TH_temp, u8 *TL_temp, u8 *CK_temp)
{
    DHT11WriteStart();//给读取前导信号
    GPIO_DHT_Input_Mode();//设置端口为输入状态
    if(!DatIOIn) {
        while(0 == DatIOIn);//低电平的响应信号，80us
        while(1 == DatIOIn);//紧接着是80us的高电平数据准备信号

        *RH_temp = DHT11ReadByte();//湿度高8位
        *RL_temp = DHT11ReadByte();//湿度低8位
        *TH_temp = DHT11ReadByte();//温度高8位
        *TL_temp = DHT11ReadByte();//温度低8位
        *CK_temp = DHT11ReadByte();//校验和
        GPIO_DHT_Out_Mode();
        DatIOOut = 1;
        //数据校验
        //untemp= *RH_temp+RL_temp+*TH_temp+TL_temp;
    }
}
void Read_DHT11(void)
{
    u8 TempDHT[5] = {0};

    DHT11Read(&TempDHT[0], &TempDHT[1], &TempDHT[2], &TempDHT[3], &TempDHT[4]);

    //数据接受状态
    //温度整数位
    //温度小数位
    //湿度整数位
    //湿度小数位
    //校验和
    if(TempDHT[4] == (TempDHT[0] + TempDHT[1] + TempDHT[2] + TempDHT[3])) {
        DHT_temperature = TempDHT[1];
        DHT_temperature /= 1000; //小数点
        DHT_temperature += TempDHT[0];

        DHT_humidity = TempDHT[3];
        DHT_humidity /= 1000; //小数点
        DHT_humidity += TempDHT[2];
        printf("温度 %f 湿度 %f  \n", DHT_temperature, DHT_humidity);

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
 * @brief  DHT11 初始化函数
 * @param  无
 * @retval 无
 */
void DHT11_Init ( void )
{
    DHT11_GPIO_Config ();
    DatIOOut = 1;               // 拉高
}


/*
 * 函数名：DHT11_GPIO_Config
 * 描述  ：配置DHT11用到的I/O口
 * 输入  ：无
 * 输出  ：无
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
 * 函数名：DHT11_Mode_IPU
 * 描述  ：使DHT11-DATA引脚变为上拉输入模式
 * 输入  ：无
 * 输出  ：无
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
 * 函数名：DHT11_Mode_Out_PP
 * 描述  ：使DHT11-DATA引脚变为推挽输出模式
 * 输入  ：无
 * 输出  ：无
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
 * 从DHT11读取一个字节，MSB先行
 */
static uint8_t DHT11_ReadByte ( void )
{
    uint8_t i, temp = 0;
	  u16 Cnt = 1;
    for(i = 0; i < 8; i++) {
        /*每bit以50us低电平标置开始，轮询直到从机发出 的50us 低电平 结束*/
        while(DatIOIn == Bit_RESET && Cnt){Cnt++;}

        /*DHT11 以26~28us的高电平表示“0”，以70us高电平表示“1”，
         *通过检测 x us后的电平即可区别这两个状 ，x 即下面的延时
         */
        delay_us(40); //延时x us 这个延时需要大于数据0持续的时间即可

        if(DatIOIn == Bit_SET) { /* x us后仍为高电平表示数据“1” */
					  Cnt = 1;   /* 等待数据1的高电平结束 */
            while(DatIOIn == Bit_SET && Cnt){Cnt++;}
            temp |= (uint8_t)(0x01 << (7 - i)); //把第7-i位置1，MSB先行
        } else {     // x us后为低电平表示数据“0”
            temp &= (uint8_t)~(0x01 << (7 - i)); //把第7-i位置0，MSB先行
        }
    }
    return temp;

}

DHT11_TypeDef DHT11Dat;
/*
 * 一次完整的数据传输为40bit，高位先出
 * 8bit 湿度整数 + 8bit 湿度小数 + 8bit 温度整数 + 8bit 温度小数 + 8bit 校验和
 */
uint8_t DHT11_Read_TempAndHumidity(DHT11_TypeDef *DHT11_Data)
{ 
	  u16 Cnt =1;
    DHT11_Mode_Out_PP(); /*输出模式*/
    DatIOOut = 0;        /*主机拉低*/
    delay_ms(18);			   /*延时18ms*/
    DatIOOut = 1; 		   /*总线拉高 主机延时30us*/
    delay_us(30);        //延时30us
    DHT11_Mode_IPU();    /*主机设为输入 判断从机响应信号*/
    /*判断从机是否有低电平响应信号 如不响应则跳出，响应则向下运行*/
    if(DatIOIn == Bit_RESET) { 
			  Cnt = 1;
        while(DatIOIn == Bit_RESET && Cnt){Cnt++;}/*轮询直到从机发出 的80us 低电平 响应信号结束*/
			  Cnt = 1;        
				while(DatIOIn == Bit_SET && Cnt){Cnt++;} /*轮询直到从机发出的 80us 高电平 标置信号结束*/
        /*开始接收数据*/
        DHT11_Data->humidity1    = DHT11_ReadByte();   //湿度
        DHT11_Data->humidity2 	 = DHT11_ReadByte();
        DHT11_Data->temperature1 = DHT11_ReadByte();   //温度
        DHT11_Data->temperature2 = DHT11_ReadByte();				
        DHT11_Data->check     = DHT11_ReadByte();        
        DHT11_Mode_Out_PP(); /*读取结束，引脚改为输出模式*/   
        DatIOOut = 1; /*主机拉高*/
        /*检查读取的数据是否正确*/
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
       COM_Printf(5 ,"湿度 %d.%d%%RH 温度 %d.%d℃\r\n", DHT11Dat.humidity1, DHT11Dat.humidity2,DHT11Dat.temperature1, DHT11Dat.temperature2);
    } else {
			 DHT11Dat.temperature1 = 0xFF; //温度
			 DHT11Dat.temperature2 = 0xFF; 
       DHT11Dat.humidity1 = 0xFF;   //湿度
       DHT11Dat.humidity2 = 0xFF;			
       COM_Printf(5 ,"温湿度模块错误\r\n");
    }
}

/*************************************END OF FILE******************************/

#endif

