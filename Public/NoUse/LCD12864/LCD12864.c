  /*
  ************************************* Copyright ****************************** 
  *
  *                 (C) Copyright 2020,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                      https://blog.csdn.net/Wekic
  *    
  * FileName     : LCD12864.c   
  * Version      : v1.0       
  * Author       : Wenkic         
  * Date         : 2020-11-14         
  * Description  :    
  * Function List:  
  ******************************************************************************
  */
/********************************End of Head************************************/
#include ".\LCD12864\LCD12864.h"
#include "driver.h"
#include "tm_library.h" 
#include "sys.h"
#include "delay.h"


const u8  LCD_icon[2][32]={//+
    
 {0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0xFF,0xFF,
  0xFF,0xFF,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
 },
 {
   0x0F,0xF0,0x1F,0xF8,0x3F,0xFC,0x7F,0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
   0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0x7F,0xFE,0x3F,0xFC,0x1F,0xF8,0x0F,0xF0, 
 }
};

u8 Num16x32Table[10][64]=  //0123456789 (16x32)
{
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xC0,0x06,0x20,
 0x0C,0x30,0x18,0x18,0x18,0x18,0x18,0x08,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,
 0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x18,0x08,0x18,0x18,
 0x18,0x18,0x0C,0x30,0x06,0x20,0x03,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"0",0*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,0x01,0x80,
 0x1F,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
 0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,0x01,0x80,
 0x01,0x80,0x01,0x80,0x03,0xC0,0x1F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"1",1*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x08,0x38,
 0x10,0x18,0x20,0x0C,0x20,0x0C,0x30,0x0C,0x30,0x0C,0x00,0x0C,0x00,0x18,0x00,0x18,
 0x00,0x30,0x00,0x60,0x00,0xC0,0x01,0x80,0x03,0x00,0x02,0x00,0x04,0x04,0x08,0x04,
 0x10,0x04,0x20,0x0C,0x3F,0xF8,0x3F,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"2",2*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x18,0x60,
 0x30,0x30,0x30,0x18,0x30,0x18,0x30,0x18,0x00,0x18,0x00,0x18,0x00,0x30,0x00,0x60,
 0x03,0xC0,0x00,0x70,0x00,0x18,0x00,0x08,0x00,0x0C,0x00,0x0C,0x30,0x0C,0x30,0x0C,
 0x30,0x08,0x30,0x18,0x18,0x30,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"3",3*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x60,
 0x00,0xE0,0x00,0xE0,0x01,0x60,0x01,0x60,0x02,0x60,0x04,0x60,0x04,0x60,0x08,0x60,
 0x08,0x60,0x10,0x60,0x30,0x60,0x20,0x60,0x40,0x60,0x7F,0xFC,0x00,0x60,0x00,0x60,
 0x00,0x60,0x00,0x60,0x00,0x60,0x03,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"4",4*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xFC,0x0F,0xFC,
 0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x13,0xE0,0x14,0x30,
 0x18,0x18,0x10,0x08,0x00,0x0C,0x00,0x0C,0x00,0x0C,0x00,0x0C,0x30,0x0C,0x30,0x0C,
 0x20,0x18,0x20,0x18,0x18,0x30,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"5",5*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xE0,0x06,0x18,
 0x0C,0x18,0x08,0x18,0x18,0x00,0x10,0x00,0x10,0x00,0x30,0x00,0x33,0xE0,0x36,0x30,
 0x38,0x18,0x38,0x08,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x18,0x0C,
 0x18,0x08,0x0C,0x18,0x0E,0x30,0x03,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"6",6*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xFC,0x1F,0xFC,
 0x10,0x08,0x30,0x10,0x20,0x10,0x20,0x20,0x00,0x20,0x00,0x40,0x00,0x40,0x00,0x40,
 0x00,0x80,0x00,0x80,0x01,0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x03,0x00,0x03,0x00,
 0x03,0x00,0x03,0x00,0x03,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"7",7*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xE0,0x0C,0x30,
 0x18,0x18,0x30,0x0C,0x30,0x0C,0x30,0x0C,0x38,0x0C,0x38,0x08,0x1E,0x18,0x0F,0x20,
 0x07,0xC0,0x18,0xF0,0x30,0x78,0x30,0x38,0x60,0x1C,0x60,0x0C,0x60,0x0C,0x60,0x0C,
 0x60,0x0C,0x30,0x18,0x18,0x30,0x07,0xC0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"8",8*/
{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xC0,0x18,0x20,
 0x30,0x10,0x30,0x18,0x60,0x08,0x60,0x0C,0x60,0x0C,0x60,0x0C,0x60,0x0C,0x60,0x0C,
 0x70,0x1C,0x30,0x2C,0x18,0x6C,0x0F,0x8C,0x00,0x0C,0x00,0x18,0x00,0x18,0x00,0x10,
 0x30,0x30,0x30,0x60,0x30,0xC0,0x0F,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},/*"9",9*/
};



static u8 lastOver[4][8]={0};  //反白数据标记
static u8 nowOver[4][8] ={0};

//现在显示的数据
static u8 OLSMTable[4][16] = {0};
//缓存的数据
static u8 LSMTable[4][16] =
{
  {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
  {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},  
  {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},
  {0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20},  
};
#define ReadBusy    0XFC       //读忙指令
#define ReadData    0XFE       //读数据指令
#define WriteCmd    0XF8       //写命令指令
#define WriteData   0XFA       //写数据指令

//PC8   --RS(CS ) H:数据 L:指令
//PC9   --RW(SID)
//PB15  --E(SCLK)
//PB14
//PB12

#define RS  PCout(8)
#define RW  PCout(9)
#define E   PBout(15)

#define LCD_CS   RS   //串行片选
#define LCD_SID  RW   //串行数据
#define LCD_CLK  E    //串行时钟  

/* 字符显示RAM地址    4行8列 (内部包含字库可寻找地址)*/
const u8 LCD_addr[4][8]={
	{0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87},  		//第一行
	{0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97},		//第二行
	{0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F},		//第三行
	{0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F}		//第四行
	};


/* 串行连接时序

[CS]******_____/¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯\____ 使能端线

                 1   2         6   7   8   9  10  11  12  ... 16  17  18  19  20  ... 24 
[SCLK]****______|¯|_|¯|_ ... _|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|_|¯|______ 时钟线
                              RW  RS   0   D7  D6  D5  D4   0   0  D3  D2  D1  D0   0   0    

[SID]*****------(1, 1,  1,1,1,RW, RS,  0)-(高4位数据            )-(低4位数据            )-------数据线

[RW] 传输方向 (H:读数据     L:写数据)
[RS] 传输性质 (H:命令寄存器 L:数据寄存器)
*/

/*
CGDRAM 地址分配(画图写入数据地址)
------------------------------------------------------------------------------------------------------------------------------
          0x80             0x81          0x82           0x83          0x84           0x85          0x86           0x87       
0x80     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x81     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0 
0x82     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
0x8E     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x8F     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
-----------------------------------------------------------------------------------------------------------------------------
0x90     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x91     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0 
0x92     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
0x9E     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x9F     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
------------------------------------------------------------------------------------------------------------------------------
          0x88             0x89          0x8A           0x8B          0x8C           0x8D          0x8E           0x8F       
0x80     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x81     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0 
0x82     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
0x8E     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x8F     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
-----------------------------------------------------------------------------------------------------------------------------
0x90     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x91     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0 
0x92     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
  .        .     .          .     .       .     .        .     .       .     .        .     .       .     .        .     .
0x9E     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
0x9F     D15-D8,D7-D0     D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0  D15-D8,D7-D0   D15-D8,D7-D0
------------------------------------------------------------------------------------------------------------------------------
*/

/**
  * @Name    LCD_GPIO_Init
  * @brief   管脚初始化
  * @param   None
  * @retval  None
  * @Data    None
 **/
static void LCD12864_GPIO_Init(void)
{
    TM_GPIO_Init(GPIOC,GPIO_Pin_8 ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //PC8
    TM_GPIO_Init(GPIOC,GPIO_Pin_9 ,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //PC9
    TM_GPIO_Init(GPIOB,GPIO_Pin_15,GPIO_Mode_Out_PP,GPIO_Speed_50MHz); //PB15
}
//***LCD显示****************************************************
/**
  * @Name    Write_StartCode
  * @brief   前导码，确定操作方向
  * @param   com (0xFC<读忙信号> ,0xFE<读数据指令>,0xF8<写命令>,0xFC<写数据>)
  * @retval  None
  * @Data    None
 **/
static void LCD_StartCode( u8 com )
{
    u8 i = 0;
    LCD_CS  = 1 ;  //指令 类型
    LCD_CLK = 0 ;  //
    for(i=0;i<8;i++)
    {
        if(com&0x80){
            LCD_SID = 1;
        }else{
            LCD_SID = 0;
        }
        LCD_CLK = 1;
        com <<=1;
        LCD_CLK = 0;
    }
    LCD_CS  = 0 ;  
}

/**
  * @Name    {FunctionName}
  * @brief   简要注释. 指该函数的简要介绍
  * @param   {Parameters}参数描述
  * @retval  |返回值描述
  * @Data    {Date}
 **/
static void LCD_WriteByte( u8 Dat )
{
    u8  i;
    u16 temp = (Dat&0xF0)<<4; //高4位数据
    temp    |= (Dat&0x0F);    //低4位数据
    temp   <<= 4;             //偏移   得到 [D7  D6  D5  D4  0 0 0 0]  [D3  D2  D1  D0  0 0 0 0]
    LCD_CS = 1;
    LCD_CLK = 0;
    for(i=0; i<16; i++)       //移位数据
    {
        if(temp & 0x8000)
        {
            LCD_SID = 1;
        }
        else
        {
            LCD_SID = 0;
        }
        LCD_CLK = 1;
        temp <<= 1;
        LCD_CLK = 0;
    }
    LCD_CS = 0;
}
/**
  * @Name    Write_com
  * @brief   寄存器操作,写入数据
  * @param   cmd 寄存器数据
  * @retval  None
  * @Data    None
 **/
static void Write_com(u8 cmd)
{
    delay_us(15);
    LCD_StartCode(WriteCmd); //前导码(写寄存器)
    LCD_WriteByte(cmd)     ; //数据
}
/**
  * @Name    Write_com
  * @brief   写入一个数据
  * @param   dat 将写入的数据
  * @retval  None
  * @Data    None
 **/
static void Write_Dat(u8 dat)
{
    delay_us(15);
    LCD_StartCode(WriteData); //前导码(写数据)
    LCD_WriteByte(dat )     ; //数据
}

void clear_lcd(u8 dat)
{
    u8  i,y;
    Write_com(0x34); //切换到扩充指令
    Write_com(0x34); //关闭图形显示		
	for(i = 0; i < 32; i++)   //上下屏写入
	{
		Write_com(0x80 + i); //行地址
		Write_com(0x80    ); //列地址 (0x80---0x8F)
		for(y=0;y<32;y++)    //垂直Y写32次
		{  
			Write_Dat(dat);		        //写高位字节数据 D15－D8 
		}      
	}
    Write_com(0x36); //打开图形显示	
    Write_com(0x30); //切换回基本指令
}


void  LCD_Init( void )
{
    LCD_CS = 0;   
    LCD12864_GPIO_Init();
    Write_com( 0x30 ); //功能设定:选择基本指令集 /*显示模式设置  00111000  设置16*2显示  5*7点阵  8位数据接口*/
    delay_ms(1);       //延时>100us
    delay_ms(1);       //延时>100us                             
    Write_com( 0x0C ); //显示设置
                                   /*显示开关及光标设置   00001DCB
                                   D=1，开显示     D=0， 关显示     1
                                   C=1，显示光标   C=0，不显示光标  0
                                   B=1，光标闪烁   B=0，光标不闪烁  0 */
    delay_ms(1);        //延>100us
    Write_com( 0x01 );  //清屏
    delay_ms(20);       //延时>10ms
    Write_com( 0x06 );  //显示模式  /*地址指针自动+1且光标+1，写字符屏幕不会移动 */
    delay_ms(20);       //延时>100us  
    clear_lcd(0x00);    //清除画图显示
}
/**
  * @Name    DisAddr
  * @brief   设置写入位置
  * @param   y :横坐标  x:竖坐标
  * @retval  None
  * @Data    None
 **/
static void DisAddr( u8 y, u8 x )
{
    if( x >= 7 )
    {
        x = 7;
    }
    if( y >= 3 )
    {
        y = 3;
    }    
    Write_com(LCD_addr[y][x] ); //显示模式
}

//显示数组
void LCD_display( u8 y, u8 x, u8 *Dat )
{
    DisAddr( y, x );
    while( *Dat )
    {
        Write_Dat( *Dat++ );
    }
}

//显示单数据(填充16x16单元)
void LCD_display1( u8 y, u8 x, u8 Dat1, u8 Dat2 )
{
    DisAddr( y, x );
    Write_Dat( Dat1 );
    Write_Dat( Dat2 );	
}

//清除屏幕
void LCD_Clear( void )
{
    u16 i = sizeof(LSMTable);
    u8 m,n;
    u8 *dat1 = (u8*)LSMTable ;
    u8 *dat2 = (u8*)OLSMTable;
    Write_com( 0x01 ); //清除LCD显示
    while(i--)
    {
      *dat1 = 0x20; dat1++;
      *dat2 = 0x20; dat2++;       
    }
    for(m=0;m<4;m++)
    for(n=0;n<8;n++)
    {
      nowOver[m][n ]=0;
      lastOver[m][n]=1;
    }
    delay_ms(1);    
}
//某行某列显示X个数据
void LCD_xLineOver(u8 line)
{
    Write_com(0x36); //切换到扩充指令
    Write_com(0x04|(line&0x03));
    delay_us(100);
    Write_com(0x30); //切换回基本指令
    
}
//某行某列显示X个数据
void LCD_XLine( u8 y, u8 x, u8 *Dat, u8 len )
{
    DisAddr( y, x );
    if( ( x + len ) > 20 ) //防止越界
    {
        len = 20 - x;
    }
    while( len-- )
    {
        Write_Dat( *Dat++ );
    }
}

//显示全屏数据
void LCD_AllLine( u8 *Line1, u8 *Line2, u8 *Line3, u8 *Line4 )
{
    LCD_XLine( 1, 0, Line1, 20 );
    LCD_XLine( 2, 0, Line2, 20 );
    LCD_XLine( 3, 0, Line3, 20 );
    LCD_XLine( 4, 0, Line4, 20 );
}




//图片显示
void Lcd12864_ClearScreen(void)
{
	u8 i, j;

	for(i=0; i<8; i++)
	{
		//--表格第3个命令，设置Y的坐标--//
		//--Y轴有64个，一个坐标8位，也就是有8个坐标--//
		//所以一般我们使用的也就是从0xB0到0x07,就够了--//	
		Write_com(0xB0+i); 

		//--表格第4个命令，设置X坐标--//
		//--当你的段初始化为0xA1时，X坐标从0x10,0x04到0x18,0x04,一共128位--//
		//--当你的段初始化为0xA0时，X坐标从0x10,0x00到0x18,0x00,一共128位--//
		//--在写入数据之后X坐标的坐标是会自动加1的，我们初始化使用0xA0所以--//
		//--我们的X坐标从0x10,0x00开始---//
		Write_com(0x10); 
		Write_com(0x04);							   
		
		//--X轴有128位，就一共刷128次，X坐标会自动加1，所以我们不用再设置坐标--//
		for(j=0; j<128; j++)
		{
			Write_Dat(0xFF);  //如果设置背景为白色时，清屏选择0XFF
            
		}
	}
}

#include "string.h"     //包含头文件
#include "stdarg.h"
#include "stdio.h"
void LCD_Printf( u8 y, u8 x, char *fmt, ... )
{
    u8  LCD_PRINTF_Buffer[18];//Usart1_printf发送缓冲区
    u8 i,m,n;
    va_list ap;  //建立变参列表
    va_start( ap, fmt ); //参数格式化成ASCII
    vsnprintf( ( char * )LCD_PRINTF_Buffer,18, fmt, ap );
    va_end( ap );
    n = strlen( ( char * )LCD_PRINTF_Buffer ); //计算数据长度
    if(y<=3 && x<=7)
    //写入缓冲区  
    for(i=x*2,m=0;i<16 && m<n ;i++,m++)
    {
      LSMTable[y][i] = LCD_PRINTF_Buffer[m];
    }
//    //某行某列显示X个数据
//    LCD_XLine( y, x, LCD_PRINTF_Buffer, i );
}

//============================================================================
//                      应用模版             
//============================================================================
/**
  * @Name    LcdUpDataDisplay
  * @brief   LCD屏幕更新显示字库数据
  * @param   None
  * @retval  None
  * @Data    None
 **/
void LcdUpDataDisplay(void)
{
   u8 m,n;
   for(m=0;m<4;m++)
   {
     for(n=0;n<8;n++)
     {
       if(LSMTable[m][n*2] != OLSMTable[m][n*2] || LSMTable[m][n*2+1] != OLSMTable[m][n*2+1]) 
       {
           LCD_display1( m, n,LSMTable[m][n*2], LSMTable[m][n*2+1] );
           OLSMTable[m][n*2+0]=LSMTable[m][n*2+0] ;  //缓冲的数据切换到正在显示的数据
           OLSMTable[m][n*2+1]=LSMTable[m][n*2+1] ;
       }
     }    
   }
}



/**
  * @Name    LCD_Display16x32Num
  * @brief   显示16x32 的一位数字
  * @param   y(行)x(列)num(数字) Over(是否反白) AddDat(分界与操作)
  * @retval  None
  * @Data    None
 **/
static void LCD_Display16x32Num(u8 y,u8 x,u8 num,u8 Over,u8 AddDat)
{
    u8 i;
    Write_com(0x36); //切换到扩充指令
    Write_com(0x36); //关闭图形显示	
   
    if(y<=3 && (x+0)<8)nowOver[y][x]         = Over;
    if(y<=3 && (x+1)<8)nowOver[y][x+1]       = Over;
    if((y+1)<=3 && (x+0)<8)nowOver[y+1][x]   = Over;
    if((y+1)<=3 && (x+1)<8)nowOver[y+1][x+1] = Over;
	
	for(i = 0; i < 32; i++)   //上下屏写入
	{
		Write_com(0x80 + i); //行地址
		Write_com(0x80 + ((y/2)?8:0) + x); //列地址 (0x80---0x8F)
        if(Over)
        {
            Write_Dat((~Num16x32Table[num%10][i*2  ]))        ;//写高位字节数据 D15－D8      
            Write_Dat((~Num16x32Table[num%10][i*2+1])& AddDat);//写高位字节数据 D7 －D0
        }else{
            Write_Dat(Num16x32Table[num%10][i*2  ]);//写高位字节数据 D15－D8      
            Write_Dat(Num16x32Table[num%10][i*2+1]);//写高位字节数据 D7 －D0        
        } 
	}
    Write_com(0x36); //打开图形显示	
    Write_com(0x30); //切换回基本指令
}

/**
  * @Name    LCD_Display16x32Number2Bit
  * @brief   显示两位16x32数字
  * @param   y(行)x(列)num(数字) Over(是否反白)
  * @retval  None
  * @Data    None
 **/
void LCD_Display16x32Number2Bit(u8 y,u8 x,u8 num,u8 Over)
{
    num %=100;
    LCD_Display16x32Num(y,x  ,num/10,Over,0xFF); 
    LCD_Display16x32Num(y,x+1,num%10,Over,0xFE);
}

//16x16单元反白控制填充
static void LCD_Display16x16Over(u8 y,u8 x,u8 Over)
{
    u8 i;
    if(y>=4||x>=8)return;
    Write_com(0x36); //切换到扩充指令
    Write_com(0x36); //关闭图形显示	
	for(i = 0; i < 16; i++)   //上下屏写入
	{
		Write_com(0x80 + ((y==1||y==3)?16:0) +i); //行地址
		Write_com(0x80 + ((y/2)?8:0)        + x); //列地址 (0x80---0x8F)
        if(Over)
        {
            Write_Dat(0xFF);//写高位字节数据 D15－D8      
            Write_Dat(0xFF);//写高位字节数据 D7 －D0
        }else{
            Write_Dat(0x00);//写高位字节数据 D15－D8      
            Write_Dat(0x00);//写高位字节数据 D7 －D0        
        } 
	}
    Write_com(0x36); //打开图形显示	
    Write_com(0x30); //切换回基本指令
}
//16x16单元画图填充
void LCD_Display16x16Picture(u8 y,u8 x,u8 Over ,u8 *dat)
{
    u8 i;
    if(y>=4||x>=8)return;
    Write_com(0x36); //切换到扩充指令
    Write_com(0x36); //关闭图形显示	
	for(i = 0; i < 16; i++)   //上下屏写入
	{
		Write_com(0x80 + ((y==1||y==3)?16:0) +i); //行地址
		Write_com(0x80 + ((y/2)?8:0)        + x); //列地址 (0x80---0x8F)
        if(Over)
        {
            Write_Dat(*dat++);//写高位字节数据 D15－D8      
            Write_Dat(*dat++);//写高位字节数据 D7 －D0
        }else{
            Write_Dat(0x00);//写高位字节数据 D15－D8      
            Write_Dat(0x00);//写高位字节数据 D7 －D0        
        } 
	}
    Write_com(0x36); //打开图形显示	
    Write_com(0x30); //切换回基本指令
}


//* @Name    LCD_SetPictureOver
//* @brief   设置反白的位置
//* @param   y(行)x(列)num(数字) Over(是否反白)len(反白的长度)
//* @retval  |返回值描述
//* @Data    {Date}
void LCD_SetPictureOver(u8 y,u8 x,u8 Over,u8 len)
{
    for(u8 i=0;y<4&&i<len &&(x+i)<8;i++)
    {
        if(Over==2)
        {
          nowOver[y][x+i] = 0;     
          lastOver[y][x+i]= 1;  
            
        }else if(Over==3)
        {
          nowOver[y][x+i] = 0;     
          lastOver[y][x+i]= 1;  
        }
        
        else{
          nowOver[y][x+i]= Over?1:0;        
        }

    } 
}
//* @Name   LCD_OverUpdate
//* @brief  反白数据刷新
//* @param  None
//* @retval None 
void LCD_ClearAllOver(void)
{
    u8 m,n;
    for(m=0;m<4;m++)
    {
        for(n=0;n<8;n++)
        {
            nowOver[m][n] = 0;
        }
    }
}
//* @Name   LCD_OverUpdate
//* @brief  反白数据刷新
//* @param  None
//* @retval None 
void LCD_OverUpdate(void)
{
    u8 m,n;
    for(m=0;m<4;m++)
    {
        for(n=0;n<8;n++)
        {
            if(nowOver[m][n]!=lastOver[m][n])
            {
                lastOver[m][n] = nowOver[m][n];
                LCD_Display16x16Over(m,n,nowOver[m][n]);              
            }
        }
    }
}



/********************************End of File************************************/

