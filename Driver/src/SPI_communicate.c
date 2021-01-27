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

//�������ݰ��ϳ�
void SPI_SendCmdPack(uint8_t cmd, uint8_t *dat, uint16_t len)
{
    uint8_t TCRC = cmd;
    que_Write(&MasterSPI_Tque, SPI_CmdStart);  //��ͷ
    que_Write(&MasterSPI_Tque, len+2);           //����
    que_Write(&MasterSPI_Tque, cmd);           //����
    for(u16 i = 0; i < len; i++) {
        que_Write(&MasterSPI_Tque, dat[i]);    //���ݰ�
        TCRC += dat[i];
    }
    que_Write(&MasterSPI_Tque, TCRC);          //У��
    que_Write(&MasterSPI_Tque, SPI_CmdEnd);    //У��
}


typedef struct _SPICmd {
    u8 Buff[100]; //��������
    u8 len      ; //����
    u8 GetFlag  ; //��ȡ���
} SPICMD_Type;

//���ݰ�
SPICMD_Type SPI_CmdPack = {0};
u16     SPI_Get_OutTime = 0;


//���ݰ���ȡ
static void SPI_GetDataCmdPack(void)
{
    static u8 Port = 0; //״̬
    static u8 len  = 0;
    static u8 Alen = 0;
    static u8 flag = 0;
    u8   clyce = 100;    //��ȡ����
    

    if(SPI_CmdPack.GetFlag) {
        return;    //����δ����
    }
    if(que_size(MasterSPI_Rque) == 0) {
        return;    //�����ݴ���
    }
    while(clyce--) {
        //�������
        if(SPI_Get_OutTime == 0 && Port) {
            Port = 0;
            len  = 0;
            if(flag){
               flag = 0;
               SPI_UsrLog("Pack Err");            
            }
        }
        if(que_size(MasterSPI_Rque)) {     //���ʣ��������
            if(len < sizeof(SPI_CmdPack.Buff)) {
                SPI_CmdPack.Buff[len] = que_Read(&MasterSPI_Rque);   //�������
            } else {
                SPI_ErrLog("������� %s %d", (uint8_t *)__FILE__, __LINE__);
                SPI_Get_OutTime = 0;
                Port = 5;
            }
            //�������״̬��
            switch(Port) {
                //��ͷ
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
                //����
                case 1: {
                    Alen = SPI_CmdPack.Buff[len] + 3;
                    len  = 2;
                    Port = 2;
                }
                break;
                //���ݰ�
                case 2: {
                    len++;
                    if(len == Alen - 1) {
                        Port++;
                    }
                }
                break;
                //��β
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

//�����
static void SPI_CmdDeal(u8 *dat,u8 len)
{
    switch(dat[2])
    {
        //��ȡVID PID
        case 0x01:
        {
           SPI_UsrLog("USB VID %04Xh",(u16)(dat[3]|dat[4]<<8));
           SPI_UsrLog("USB PID %04Xh",(u16)(dat[5]|dat[6]<<8));
        }break;
        //��ȡת�������ݰ�
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

//SPI��ȡ���ݵĴ���
static void SPI_CmdAnalyze(void)
{
   if(SPI_CmdPack.GetFlag)
   {
       SPI_CmdDeal(SPI_CmdPack.Buff,SPI_CmdPack.len);
       SPI_CmdPack.GetFlag = 0;
   }
}

//SPIͨѶЭ�鴦��
void SPI_Communicate(void)
{
   SPI_GetDataCmdPack(); //������ȡ
   SPI_CmdAnalyze()    ; //�����
}


/********************************End of File************************************/

