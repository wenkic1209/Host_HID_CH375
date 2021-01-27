/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2021,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : SPI_communicate.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2021-01-05
* Description  :
* Function List:
******************************************************************************
*/
/********************************End of Head************************************/
#include "SPI_communicate.h"
#include "queue.h"
#include "debug.h"
#include "MyHID_Driver.h"
extern u16 delay_msA;
#if 0
    #define SPI_UsrLog(...)
    #define SPI_ErrLog(...)

#else
    #define SPI_UsrLog(...)        sprint(__VA_ARGS__);sprint("\r\n")
    #define SPI_ErrLog(...)        sprint(__VA_ARGS__);sprint("\r\n")
#endif


#define SPI_CmdStart  0xAA
#define SPI_CmdEnd    0xBB

u8 SPI_TxBuf[64] = {0};

//发送数据包合成
void SPI_SendCmdPack(uint8_t cmd, uint8_t *dat, uint16_t len)
{
    uint8_t TCRC = cmd;
    que_Write(&MasterSPI_Tque, SPI_CmdStart);  //包头
    que_Write(&MasterSPI_Tque, len+2);           //长度
    que_Write(&MasterSPI_Tque, cmd);           //命令
    for(u16 i = 0; i < len; i++) {
        que_Write(&MasterSPI_Tque, dat[i]);    //数据包
        TCRC += dat[i];
    }
    que_Write(&MasterSPI_Tque, TCRC);          //校验
    que_Write(&MasterSPI_Tque, SPI_CmdEnd);    //校验
}


typedef struct _SPICmd {
    u8 Buff[100]; //缓存数据
    u8 len      ; //长度
    u8 GetFlag  ; //获取标记
} SPICMD_Type;

//数据包
SPICMD_Type SPI_CmdPack = {0};
u16     SPI_Get_OutTime = 0;


//数据包提取
static void SPI_GetDataCmdPack(void)
{
    static u8 Port = 0; //状态
    static u8 len  = 0;
    static u8 Alen = 0;
    static u8 flag = 0;
    u8   clyce = 100;    //提取次数
    

    if(SPI_CmdPack.GetFlag) {
        return;    //数据未处理
    }
    if(que_size(MasterSPI_Rque) == 0) {
        return;    //无数据处理
    }
    while(clyce--) {
        //清除数据
        if(SPI_Get_OutTime == 0 && Port) {
            Port = 0;
            len  = 0;
            if(flag){
               flag = 0;
               SPI_UsrLog("Pack Err");            
            }
        }
        if(que_size(MasterSPI_Rque)) {     //检测剩余数据量
            if(len < sizeof(SPI_CmdPack.Buff)) {
                SPI_CmdPack.Buff[len] = que_Read(&MasterSPI_Rque);   //获得数据
            } else {
                SPI_ErrLog("数据溢出 %s %d", (uint8_t *)__FILE__, __LINE__);
                SPI_Get_OutTime = 0;
                Port = 5;
            }
            //命令解析状态机
            switch(Port) {
                //包头
                case 0: {
                    if(SPI_CmdPack.Buff[len] == SPI_CmdStart) {
                        SPI_Get_OutTime = 20;
                        len  = 1;
                        Port = 1;
                        flag = 1;
                      //  SPI_UsrLog("Pack Start");
                    } else {
                        len  = 0;
                    }
                }
                break;
                //长度
                case 1: {
                    Alen = SPI_CmdPack.Buff[len] + 3;
                    len  = 2;
                    Port = 2;
                }
                break;
                //数据包
                case 2: {
                    len++;
                    if(len == Alen - 1) {
                        Port++;
                    }
                }
                break;
                //包尾
                case 3: {
                    if(SPI_CmdPack.Buff[len] == SPI_CmdEnd) {
                        SPI_CmdPack.len = len + 1;
                        SPI_CmdPack.GetFlag = 1;
                        SPI_Get_OutTime = 0;
                        flag = 0;
                       // SPI_UsrLog("Pack Success");   
                        return ;
                    } else {
                        SPI_Get_OutTime = 0;
                    }
                }
                break;
            }
        }
    }
}

//命令处理
static void SPI_CmdDeal(u8 *dat,u8 len)
{
    switch(dat[2])
    {
        //获取VID PID
        case 0x01:
        {
           SPI_UsrLog("USB VID %04Xh",(u16)(dat[3]|dat[4]<<8));
           SPI_UsrLog("USB PID %04Xh",(u16)(dat[5]|dat[6]<<8));
        }break;
        //获取转发的数据包
        case 0x02:
        {
           for(u8 i=0;i<(dat[1]-2);i++)
            {
              que_Write(&Tx1_que,dat[i+3]);                
            }
            que_Write(&Tx1_que,0x0D);                         
            que_Write(&Tx1_que,0x0A);             
//            if(bDeviceState == CONFIGURED )//&& delay_msA==0)
//            HID_Send_Report(&dat[3],dat[1]-2);
        }break;    
    
    }
}

//SPI提取数据的处理
static void SPI_CmdAnalyze(void)
{
   if(SPI_CmdPack.GetFlag)
   {
       SPI_CmdDeal(SPI_CmdPack.Buff,SPI_CmdPack.len);
       SPI_CmdPack.GetFlag = 0;
   }
}

//SPI通讯协议处理
void SPI_Communicate(void)
{
   SPI_GetDataCmdPack(); //命令提取
   SPI_CmdAnalyze()    ; //命令处理
}


/********************************End of File************************************/

