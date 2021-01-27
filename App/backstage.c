/*
************************************* Copyright ******************************
*
*                 (C) Copyright 2020,Wenkic,China, GCU.
*                            All Rights Reserved
*
*                     By(Wenkic)
*                      https://blog.csdn.net/Wekic
*
* FileName     : backstage.c
* Version      : v1.0
* Author       : Wenkic
* Date         : 2020-12-10
* Description  :
* Function List:
******************************************************************************
*/
/********************************End of
 * Head************************************/
#include "backstage.h"
#include "driver.h"
#include "public.h"

//���ʱ��
typedef struct {
    u8 on_off;  //��濪��
    u8 time;    //ʱ��
} Back_advertising;
//������������
typedef struct {
    u8 currency;  //����
    u8 inning;    //����
} Back_BIJU;
//����ģʽ
typedef struct {
    u16 voltage;  //��⵽�ĵ�ѹ
    u8 strong;    //ǿ��+
    u8 weak;      //����-
    u8 on_off;    //����ģʽ����
} Back_Intel;

typedef struct {
    u8 lang;               //����
    Back_advertising adv;  //���     -->(����)(ʱ��)
    Back_BIJU BiJu;        //���Ҽ��� -->����������
    u8 retain_Bi;          //��������
    u8 gametime;           //��Ϸʱ��
    u8 gamemode;           //��Ϸģʽ
    u8 winnumber;          //��������
    u8 location_claw;      //צ����ʼλ��
    u8 location_gift;      //��Ʒ����λ��
    u8 grab;               //����ץ��
    u8 back_A11;           //����צ����
    u8 sensor;             //���۵�ƽ
    u8 give;               //����Ʊ��
    u8 reset;              //ҡ�����
    Back_Intel Intel;      //����ģʽ -->��ѹ,ǿ��+,ǿ��-,����ģʽ����
} BackMenu_A;

const char menuMain[][16] = {
    {"    �����趨    "},  // 0
    {"A-> ��������    "},  // 1  -
    {"B-> ץ����ѹ����"},  // 2
    {"C-> ����ٶ��趨"},  // 3

    {"D-> ����        "},  // 4  -
    {"E-> ��������    "},  // 5
    {"F-> �������    "},  // 6
    {"G-> ���ģʽ    "},  // 7  -

    {"H-> �ָ���������"},  // 8
    {"I-> ����        "},  // 9
    {"J-> ҡ��ǰ����"},  // 10 -
    {"K-> ���ͨѶ����"},  // 11

    {"L-> �ֻ�����    "},  // 12
    {"      ����      "},  // 13 -
};
const u8 menuB[][16] = {
    {"  ץ����ѹ����  "}, 
    {"B1->ǿץ����ѹ  "}, 
    {"B2->��ץ����ѹ  "},
    {"B3->��ץ���ѹ  "},
    {"B4->�н���ѹ    "}, 
    {"B5->ǿ��ά��ʱ��"},
    {"B6->����ά��ʱ��"},
    {"B7->ǿ������ʽ  "}, 
    {"B8->���߳��ȵ���"},
    {"B9->����ǿ������"},
    {"      ����      "},
};
const u8 menuC[][16] = {
    {"  ����ٶ��趨  "}, 
    {"C1->ǰ���ٶ�    "}, 
    {"C2->�����ٶ�    "},
    {"C3->�����ٶ�    "}, 
    {"      ����      "},
};

const u8 DIS_ERROR[][16] = {
    {"    ���粻����  "}, {"  �쳵��λ���  "}, {"  ����λ����    "},
    {"  ����λ����    "}, {"  ����λ����    "}, {"  ǰ��λ����    "},
    {"    ���۹���    "},
};

void menuMainDis(void);
void BackGraundSet(void)
{
    //���ü�
    if(getKeyShortFlag(KeySet)) {
        clearKeyShortFlag(KeySet);
        LCD_ClearAllOver();
        menuMainDis();  //���˵�����
        clearKeyShortFlag(KeySet);
    }
}

//���з���
void LCDline_over(u8 line)
{
    LCD_SetPictureOver(0, 0, ((line % 4) == 0) ? 1 : 0, 16);
    LCD_SetPictureOver(1, 0, ((line % 4) == 1) ? 1 : 0, 16);
    LCD_SetPictureOver(2, 0, ((line % 4) == 2) ? 1 : 0, 16);
    LCD_SetPictureOver(3, 0, ((line % 4) == 3) ? 1 : 0, 16);
    LCD_OverUpdate();
}


//��������
void MenuA(void);  //ѡ��1-15 [16]
void MenuB(void);  //ѡ��1-9  [10]
void MenuC(void);  //ѡ��1-3  [10]
void MenuD(void);  //
void MenuE(void);  //
void MenuF(void);  //
void MenuG(void);  //
void MenuH(void);  //
void MenuI(void);  //
void MenuJ(void);  //
void MenuK(void);  //
void MenuL(void);  //
u8 Menu_1_IN(u8 ID);
//�������� (��ҳ�˵�����)
void (*menu_2[12])(void) = {
&MenuA,
&MenuB,
&MenuC,
&MenuD,
&MenuE,
&MenuF,
&MenuG,
&MenuH,
&MenuI,
&MenuJ,
&MenuK,
&MenuL,   
};

//* @Name   menuMainDis
//* @brief  ���˵���ʾ
//* @param  None
//* @retval None
void menuMainDis(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    LCD_SetPictureOver(0, 0, 2, 16);   //�����������
    LCD_SetPictureOver(1, 0, 2, 16);   //�����������
    LCD_OverUpdate();
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 13;
                }
                if(menu == 13 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 14) {
                    menu = 1;
                }
                if(menu == 1 || (menu%4) == 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    �����趨    ");   // 0
                    LCD_Printf(1, 0, "A-> ��������    ");   // 1
                    LCD_Printf(2, 0, "B-> ץ����ѹ����");   // 2
                    LCD_Printf(3, 0, "C-> ����ٶ��趨");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "D-> ����        ");   // 4
                    LCD_Printf(1, 0, "E-> ��������    ");   // 5
                    LCD_Printf(2, 0, "F-> �������    ");   // 6
                    LCD_Printf(3, 0, "G-> ���ģʽ    ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "H-> �ָ���������");   // 8
                    LCD_Printf(1, 0, "I-> ����        ");   // 9
                    LCD_Printf(2, 0, "J-> ҡ��ǰ����");   // 10
                    LCD_Printf(3, 0, "K-> ���ͨѶ����");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "L-> �ֻ�����    ");   // 12
                    LCD_Printf(1, 0, "      ����      ");   // 13
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if(Menu_1_IN(menu)==1){}
            else if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 13)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��           
        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}

//2��ѡ��˵�     (���ѡ��,��ʾ���ݵ�ָ��,���ݴ�С,��������,����������С,)     
void Menu_2levelTemplate(u8 MaxSelect,u8*LCD_dat,size_t LCD_size,void (*funtion[])(void),size_t funtion_size)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    void (*fun)();
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MaxSelect;
                }
                if(menu == MaxSelect || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MaxSelect+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if((16*(menu/4)*4+3*16)< LCD_size) //����������ʴ�С
                {
                    LCD_Printf(0, 0, "%s",LCD_dat+16*(menu/4)*4+0*16);   // 
                    LCD_Printf(1, 0, "%s",LCD_dat+16*(menu/4)*4+1*16);   // 
                    LCD_Printf(2, 0, "%s",LCD_dat+16*(menu/4)*4+2*16);   // 
                    LCD_Printf(3, 0, "%s",LCD_dat+16*(menu/4)*4+3*16);   //                 
                }
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if(menu == MaxSelect)break;            
            if((menu-1) < funtion_size / sizeof(void (*)(void))&& funtion[menu-1]!=NULL) {
                funtion[menu-1]();  //��һ���˵�����
            }
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}








const u8 menuA[][16] = {
    {"    ��������    "},
    {"A1->����Language"},
    {"A2->�������    "},
    {"A3->���Ҽ���    "},
    
    {"A4->��������    "},
    {"A5->��Ϸʱ��    "},
    {"A6->ģʽѡ��    "},
    {"A7->��������    "},
    
    {"A8->צ����ʼλ��"},
    {"A9->��Ʒ����λ��"},
    {"A10-> ����ץ��  "},
    {"A11-> ����צ����"},
    
    {"A12-> ���۵�ƽ  "},
    {"A13-> ����Ʊ��  "},
    {"A14-> ҡ�����  "},
    {"A15-> ����ģʽ  "},
    
    {"      ����      "},
    {"                "},    
    {"                "},
    {"                "},    
};

typedef struct _menuFuntion
{
    u8  ID          ; //���
    u8 MaxSelect   ; //ѡ��
    u8 *LCD_dat    ;
    size_t LCD_size;
    void *funtion  ;
    size_t funtion_size;
}menuFuntionType;



void Menu_A1(void);
void Menu_A2(void);
void (*MenuAFuntion[15])(void)={
 &Menu_A1
};

menuFuntionType Menu_1_main[] = {  
 {0,16,(u8*)menuA[0],sizeof(menuA),MenuAFuntion,sizeof(MenuAFuntion)},
 {1,16,(u8*)menuA[0],sizeof(menuA),MenuAFuntion,sizeof(MenuAFuntion)},
};

u8 Menu_1_IN(u8 ID)
{
   u16 i;
   for(i=0;i<sizeof(Menu_1_main)/sizeof(menuFuntionType);i++)
    {
       if(ID == Menu_1_main[i].ID)
       {
          Menu_2levelTemplate(Menu_1_main[i].MaxSelect,
                              Menu_1_main[i].LCD_dat,Menu_1_main[i].LCD_size,
                              Menu_1_main[i].funtion,Menu_1_main[i].funtion_size);
          return 1;     
       }
    }
   return 0;
}

//A1->����Language
void Menu_A1(void)
{
   LCD_Printf(0, 0, "������� A1   ");   //                
   LcdUpDataDisplay();                 //ˢ����������
   delay_ms(2000);
   LCD_Clear();                        //����
   LcdUpDataDisplay();                 //ˢ����������
   LCD_OverUpdate();                   //�������ݵ�Ԫˢ��

}
//A2->�������   
void Menu_A2(void)
{
   LCD_Printf(0, 0, "������� A1   ");   //                
   LcdUpDataDisplay();                 //ˢ����������
   delay_ms(2000);
   LCD_Clear();                        //����
   LcdUpDataDisplay();                 //ˢ����������
   LCD_OverUpdate();                   //�������ݵ�Ԫˢ��

}
//A3->���Ҽ���    
//A4->��������    
//A5->��Ϸʱ��    
//A6->ģʽѡ��    
//A7->��������    
//A8->צ����ʼλ��
//A9->��Ʒ����λ��
//A10-> ����ץ��  
//A11-> ����צ����
//A12-> ���۵�ƽ  
//A13-> ����Ʊ��  
//A14-> ҡ�����  
//A15-> ����ģʽ  


//A-> ��������
void MenuA(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    ��������    ");   // 0
                    LCD_Printf(1, 0, "A1->����Language");   // 1
                    LCD_Printf(2, 0, "A2->�������    ");   // 2
                    LCD_Printf(3, 0, "A3->���Ҽ���    ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "A4->��������    ");   // 4
                    LCD_Printf(1, 0, "A5->��Ϸʱ��    ");   // 5
                    LCD_Printf(2, 0, "A6->ģʽѡ��    ");   // 6
                    LCD_Printf(3, 0, "A7->��������    ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "A8->צ����ʼλ��");   // 8
                    LCD_Printf(1, 0, "A9->��Ʒ����λ��");   // 9
                    LCD_Printf(2, 0, "A10-> ����ץ��  ");   // 10
                    LCD_Printf(3, 0, "A11-> ����צ����");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> ���۵�ƽ  ");   // 12
                    LCD_Printf(1, 0, "A13-> ����Ʊ��  ");   // 13
                    LCD_Printf(2, 0, "A14-> ҡ�����  ");   // 14
                    LCD_Printf(3, 0, "A15-> ����ģʽ  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      ����      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 16)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}
//B-> ץ����ѹ����
#define MenuB_Max  10
void MenuB(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuB_Max;
                }
                if(menu == MenuB_Max || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuB_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  ץ����ѹ����  ");   // 0
                    LCD_Printf(1, 0, "B1->ǿץ����ѹ  ");   // 1
                    LCD_Printf(2, 0, "B2->��ץ����ѹ  ");   // 2
                    LCD_Printf(3, 0, "B3->��ץ���ѹ  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->�н���ѹ    ");   // 4
                    LCD_Printf(1, 0, "B5->ǿ��ά��ʱ��");   // 5
                    LCD_Printf(2, 0, "B6->����ά��ʱ��");   // 6
                    LCD_Printf(3, 0, "B7->ǿ������ʽ  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->���߳��ȵ���");   // 8
                    LCD_Printf(1, 0, "B9->����ǿ������");   // 9
                    LCD_Printf(2, 0, "      ����      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } 
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == MenuB_Max)
                break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}

//C-> ����ٶ��趨
#define MenuC_Max  4
void MenuC(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuC_Max;
                }
                if(menu == MenuC_Max || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuC_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  ����ٶ��趨  ");   // 0
                    LCD_Printf(1, 0, "C1->ǰ���ٶ�    ");   // 1
                    LCD_Printf(2, 0, "C2->�����ٶ�    ");   // 2
                    LCD_Printf(3, 0, "C3->�����ٶ�    ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "      ����      ");   // 4
                    LCD_Printf(1, 0, "                ");   // 5
                    LCD_Printf(2, 0, "                ");   // 6
                    LCD_Printf(3, 0, "                ");   // 7
                }
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == MenuC_Max){break;}
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//D-> ����
#define MenuD_Max  4
void MenuD(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuD_Max;
                }
                if(menu == MenuD_Max || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuD_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "���۲���        ");   // 0
                    LCD_Printf(1, 0, "��΢��  ǰ΢��  ");   // 1
                    LCD_Printf(2, 0, "��΢��  ��΢��  ");   // 2
                    LCD_Printf(3, 0, "��΢��  ��΢��  ");   // 3
                }
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == MenuD_Max)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//E-> �������� 
void MenuE(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                PageUpload = 1;    //ҳˢ��

            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                PageUpload = 1;    //ҳˢ��

            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    ��������    ");   // 0
                    LCD_Printf(1, 0, "                ");   // 1
                    LCD_Printf(2, 0, "   -> ����      ");   // 2
                    LCD_Printf(3, 0, "   -> ���淵��  ");   // 3
                  
//                    LCD_Printf(1, 0, "                ");   // 0
//                    LCD_Printf(1, 0, "צ����������....");   // 1
//                    LCD_Printf(1, 0, "צ������........");   // 2
//                    LCD_Printf(1, 0, "צ������........");   // 3 
//                    LCD_Printf(1, 0, "צ���������λ��");   // 3 
//                    LCD_Printf(1, 0, "צ�Ӹ�λ        ");   // 3                     
                }
                LcdUpDataDisplay();                    //ˢ����������
                LCD_Display16x16Picture(2,0,1,(u8*)LCD_icon[0]);
                LCD_Display16x16Picture(3,0,1,(u8*)LCD_icon[1]);                  
            }
        }
        //��һ��
        else if(port == 2) {
            break;            

        }
    }
    LCD_Display16x16Picture(2,0,0,(u8*)LCD_icon[0]);
    LCD_Display16x16Picture(3,0,0,(u8*)LCD_icon[1]);  
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//F-> �������
#define MenuF_Max  4
void MenuF(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuF_Max;
                }
                if(menu == MenuF_Max || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuF_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    �������    ");   // 0
                    LCD_Printf(1, 0, "F1->��Ŀ����    ");   // 1
                    LCD_Printf(2, 0, "F2->��Ŀ����    ");   // 2
                    LCD_Printf(3, 0, "F3->���ʽ�������");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "      ����      ");   // 4
                    LCD_Printf(1, 0, "                ");   // 5
                    LCD_Printf(2, 0, "                ");   // 6
                    LCD_Printf(3, 0, "                ");   // 7
                } 
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == MenuF_Max)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//G-> ���ģʽ  

void MenuG(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    ���ģʽ    ");   // 0
                    LCD_Printf(1, 0, "    ��          ");   // 1
                    LCD_Printf(2, 0, " + -> ����      ");   // 2
                    LCD_Printf(3, 0, " �� -> ���淵��  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->�н���ѹ    ");   // 4
                    LCD_Printf(1, 0, "B5->ǿ��ά��ʱ��");   // 5
                    LCD_Printf(2, 0, "B6->����ά��ʱ��");   // 6
                    LCD_Printf(3, 0, "B7->ǿ������ʽ  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->���߳��ȵ���");   // 8
                    LCD_Printf(1, 0, "B9->����ǿ������");   // 9
                    LCD_Printf(2, 0, "      ����      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> ���۵�ƽ  ");   // 12
                    LCD_Printf(1, 0, "A13-> ����Ʊ��  ");   // 13
                    LCD_Printf(2, 0, "A14-> ҡ�����  ");   // 14
                    LCD_Printf(3, 0, "A15-> ����ģʽ  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      ����      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 16)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//H-> �ָ���������
void MenuH(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  ץ����ѹ����  ");   // 0
                    LCD_Printf(1, 0, "B1->ǿץ����ѹ  ");   // 1
                    LCD_Printf(2, 0, "B2->��ץ����ѹ  ");   // 2
                    LCD_Printf(3, 0, "B3->��ץ���ѹ  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->�н���ѹ    ");   // 4
                    LCD_Printf(1, 0, "B5->ǿ��ά��ʱ��");   // 5
                    LCD_Printf(2, 0, "B6->����ά��ʱ��");   // 6
                    LCD_Printf(3, 0, "B7->ǿ������ʽ  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->���߳��ȵ���");   // 8
                    LCD_Printf(1, 0, "B9->����ǿ������");   // 9
                    LCD_Printf(2, 0, "      ����      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> ���۵�ƽ  ");   // 12
                    LCD_Printf(1, 0, "A13-> ����Ʊ��  ");   // 13
                    LCD_Printf(2, 0, "A14-> ҡ�����  ");   // 14
                    LCD_Printf(3, 0, "A15-> ����ģʽ  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      ����      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 16)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//I-> ���� 
void MenuI(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  ץ����ѹ����  ");   // 0
                    LCD_Printf(1, 0, "B1->ǿץ����ѹ  ");   // 1
                    LCD_Printf(2, 0, "B2->��ץ����ѹ  ");   // 2
                    LCD_Printf(3, 0, "B3->��ץ���ѹ  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->�н���ѹ    ");   // 4
                    LCD_Printf(1, 0, "B5->ǿ��ά��ʱ��");   // 5
                    LCD_Printf(2, 0, "B6->����ά��ʱ��");   // 6
                    LCD_Printf(3, 0, "B7->ǿ������ʽ  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->���߳��ȵ���");   // 8
                    LCD_Printf(1, 0, "B9->����ǿ������");   // 9
                    LCD_Printf(2, 0, "      ����      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> ���۵�ƽ  ");   // 12
                    LCD_Printf(1, 0, "A13-> ����Ʊ��  ");   // 13
                    LCD_Printf(2, 0, "A14-> ҡ�����  ");   // 14
                    LCD_Printf(3, 0, "A15-> ����ģʽ  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      ����      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 16)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//J-> ҡ��ǰ����
void MenuJ(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  ץ����ѹ����  ");   // 0
                    LCD_Printf(1, 0, "B1->ǿץ����ѹ  ");   // 1
                    LCD_Printf(2, 0, "B2->��ץ����ѹ  ");   // 2
                    LCD_Printf(3, 0, "B3->��ץ���ѹ  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->�н���ѹ    ");   // 4
                    LCD_Printf(1, 0, "B5->ǿ��ά��ʱ��");   // 5
                    LCD_Printf(2, 0, "B6->����ά��ʱ��");   // 6
                    LCD_Printf(3, 0, "B7->ǿ������ʽ  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->���߳��ȵ���");   // 8
                    LCD_Printf(1, 0, "B9->����ǿ������");   // 9
                    LCD_Printf(2, 0, "      ����      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> ���۵�ƽ  ");   // 12
                    LCD_Printf(1, 0, "A13-> ����Ʊ��  ");   // 13
                    LCD_Printf(2, 0, "A14-> ҡ�����  ");   // 14
                    LCD_Printf(3, 0, "A15-> ����ģʽ  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      ����      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 16)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//K-> ���ͨѶ����
void MenuK(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  ץ����ѹ����  ");   // 0
                    LCD_Printf(1, 0, "B1->ǿץ����ѹ  ");   // 1
                    LCD_Printf(2, 0, "B2->��ץ����ѹ  ");   // 2
                    LCD_Printf(3, 0, "B3->��ץ���ѹ  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->�н���ѹ    ");   // 4
                    LCD_Printf(1, 0, "B5->ǿ��ά��ʱ��");   // 5
                    LCD_Printf(2, 0, "B6->����ά��ʱ��");   // 6
                    LCD_Printf(3, 0, "B7->ǿ������ʽ  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->���߳��ȵ���");   // 8
                    LCD_Printf(1, 0, "B9->����ǿ������");   // 9
                    LCD_Printf(2, 0, "      ����      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> ���۵�ƽ  ");   // 12
                    LCD_Printf(1, 0, "A13-> ����Ʊ��  ");   // 13
                    LCD_Printf(2, 0, "A14-> ҡ�����  ");   // 14
                    LCD_Printf(3, 0, "A15-> ����ģʽ  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      ����      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 16)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}


//L-> �ֻ�����    
 void MenuL(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //��ʼ��
            LcdUpDataDisplay();  //ˢ����������
            LCDline_over(1);     //���з���
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //����
            //��  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //��  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //ҳˢ��
                }
            }
            //�¹� =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //ҳˢ�� ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  ץ����ѹ����  ");   // 0
                    LCD_Printf(1, 0, "B1->ǿץ����ѹ  ");   // 1
                    LCD_Printf(2, 0, "B2->��ץ����ѹ  ");   // 2
                    LCD_Printf(3, 0, "B3->��ץ���ѹ  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->�н���ѹ    ");   // 4
                    LCD_Printf(1, 0, "B5->ǿ��ά��ʱ��");   // 5
                    LCD_Printf(2, 0, "B6->����ά��ʱ��");   // 6
                    LCD_Printf(3, 0, "B7->ǿ������ʽ  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->���߳��ȵ���");   // 8
                    LCD_Printf(1, 0, "B9->����ǿ������");   // 9
                    LCD_Printf(2, 0, "      ����      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> ���۵�ƽ  ");   // 12
                    LCD_Printf(1, 0, "A13-> ����Ʊ��  ");   // 13
                    LCD_Printf(2, 0, "A14-> ҡ�����  ");   // 14
                    LCD_Printf(3, 0, "A15-> ����ģʽ  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      ����      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //ˢ����������
            }
            //ˢ�·���λ�� *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //��һ��
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //��һ���˵�����
            }
            if(menu == 16)break;
            port = 1;       //�ɲ���
            PageUpload = 1; //ҳˢ��
            OverUpdate = 1; //λ��ˢ��  

        }
    }
    LCD_Clear();         //����
    LcdUpDataDisplay();  //ˢ����������
    LCD_OverUpdate();    //�������ݵ�Ԫˢ��
}

 











/********************************End of
 * File************************************/
