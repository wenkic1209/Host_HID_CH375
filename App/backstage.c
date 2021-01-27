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

//广告时间
typedef struct {
    u8 on_off;  //广告开关
    u8 time;    //时间
} Back_advertising;
//币数局数设置
typedef struct {
    u8 currency;  //币数
    u8 inning;    //局数
} Back_BIJU;
//智能模式
typedef struct {
    u16 voltage;  //检测到的电压
    u8 strong;    //强电+
    u8 weak;      //弱电-
    u8 on_off;    //智能模式开关
} Back_Intel;

typedef struct {
    u8 lang;               //语言
    Back_advertising adv;  //广告     -->(开关)(时间)
    Back_BIJU BiJu;        //几币几局 -->币数、局数
    u8 retain_Bi;          //币数保留
    u8 gametime;           //游戏时间
    u8 gamemode;           //游戏模式
    u8 winnumber;          //出奖概率
    u8 location_claw;      //爪子起始位置
    u8 location_gift;      //礼品出口位置
    u8 grab;               //空中抓物
    u8 back_A11;           //自启爪中置
    u8 sensor;             //光眼电平
    u8 give;               //赠送票币
    u8 reset;              //摇晃清分
    Back_Intel Intel;      //智能模式 -->电压,强电+,强电-,智能模式开关
} BackMenu_A;

const char menuMain[][16] = {
    {"    功能设定    "},  // 0
    {"A-> 基础设置    "},  // 1  -
    {"B-> 抓力电压设置"},  // 2
    {"C-> 马达速度设定"},  // 3

    {"D-> 测试        "},  // 4  -
    {"E-> 工厂测试    "},  // 5
    {"F-> 资料清除    "},  // 6
    {"G-> 免费模式    "},  // 7  -

    {"H-> 恢复工厂设置"},  // 8
    {"I-> 查账        "},  // 9
    {"J-> 摇杆前后倒置"},  // 10 -
    {"K-> 外接通讯设置"},  // 11

    {"L-> 手机设置    "},  // 12
    {"      返回      "},  // 13 -
};
const u8 menuB[][16] = {
    {"  抓力电压设置  "}, 
    {"B1->强抓力电压  "}, 
    {"B2->弱抓力电压  "},
    {"B3->弱抓后电压  "},
    {"B4->中奖电压    "}, 
    {"B5->强力维持时间"},
    {"B6->弱力维持时间"},
    {"B7->强变弱方式  "}, 
    {"B8->放线长度调整"},
    {"B9->连续强力报错"},
    {"      返回      "},
};
const u8 menuC[][16] = {
    {"  马达速度设定  "}, 
    {"C1->前后速度    "}, 
    {"C2->左右速度    "},
    {"C3->上下速度    "}, 
    {"      返回      "},
};

const u8 DIS_ERROR[][16] = {
    {"    铁电不存在  "}, {"  天车回位检测  "}, {"  上限位错误    "},
    {"  后限位错误    "}, {"  左限位错误    "}, {"  前限位错误    "},
    {"    光眼故障    "},
};

void menuMainDis(void);
void BackGraundSet(void)
{
    //设置键
    if(getKeyShortFlag(KeySet)) {
        clearKeyShortFlag(KeySet);
        LCD_ClearAllOver();
        menuMainDis();  //主菜单设置
        clearKeyShortFlag(KeySet);
    }
}

//单行反白
void LCDline_over(u8 line)
{
    LCD_SetPictureOver(0, 0, ((line % 4) == 0) ? 1 : 0, 16);
    LCD_SetPictureOver(1, 0, ((line % 4) == 1) ? 1 : 0, 16);
    LCD_SetPictureOver(2, 0, ((line % 4) == 2) ? 1 : 0, 16);
    LCD_SetPictureOver(3, 0, ((line % 4) == 3) ? 1 : 0, 16);
    LCD_OverUpdate();
}


//基础设置
void MenuA(void);  //选择1-15 [16]
void MenuB(void);  //选择1-9  [10]
void MenuC(void);  //选择1-3  [10]
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
//函数调用 (主页菜单函数)
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
//* @brief  主菜单显示
//* @param  None
//* @retval None
void menuMainDis(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    LCD_SetPictureOver(0, 0, 2, 16);   //清除画画内容
    LCD_SetPictureOver(1, 0, 2, 16);   //清除画画内容
    LCD_OverUpdate();
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 13;
                }
                if(menu == 13 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 14) {
                    menu = 1;
                }
                if(menu == 1 || (menu%4) == 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    功能设定    ");   // 0
                    LCD_Printf(1, 0, "A-> 基础设置    ");   // 1
                    LCD_Printf(2, 0, "B-> 抓力电压设置");   // 2
                    LCD_Printf(3, 0, "C-> 马达速度设定");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "D-> 测试        ");   // 4
                    LCD_Printf(1, 0, "E-> 工厂测试    ");   // 5
                    LCD_Printf(2, 0, "F-> 资料清除    ");   // 6
                    LCD_Printf(3, 0, "G-> 免费模式    ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "H-> 恢复工厂设置");   // 8
                    LCD_Printf(1, 0, "I-> 查账        ");   // 9
                    LCD_Printf(2, 0, "J-> 摇杆前后倒置");   // 10
                    LCD_Printf(3, 0, "K-> 外接通讯设置");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "L-> 手机设置    ");   // 12
                    LCD_Printf(1, 0, "      返回      ");   // 13
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if(Menu_1_IN(menu)==1){}
            else if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 13)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新           
        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}

//2级选择菜单     (最大选择,显示数据的指针,数据大小,操作函数,操作函数大小,)     
void Menu_2levelTemplate(u8 MaxSelect,u8*LCD_dat,size_t LCD_size,void (*funtion[])(void),size_t funtion_size)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    void (*fun)();
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MaxSelect;
                }
                if(menu == MaxSelect || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MaxSelect+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if((16*(menu/4)*4+3*16)< LCD_size) //限制数组访问大小
                {
                    LCD_Printf(0, 0, "%s",LCD_dat+16*(menu/4)*4+0*16);   // 
                    LCD_Printf(1, 0, "%s",LCD_dat+16*(menu/4)*4+1*16);   // 
                    LCD_Printf(2, 0, "%s",LCD_dat+16*(menu/4)*4+2*16);   // 
                    LCD_Printf(3, 0, "%s",LCD_dat+16*(menu/4)*4+3*16);   //                 
                }
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if(menu == MaxSelect)break;            
            if((menu-1) < funtion_size / sizeof(void (*)(void))&& funtion[menu-1]!=NULL) {
                funtion[menu-1]();  //下一级菜单运行
            }
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}








const u8 menuA[][16] = {
    {"    基础设置    "},
    {"A1->语言Language"},
    {"A2->广告音乐    "},
    {"A3->几币几局    "},
    
    {"A4->币数保留    "},
    {"A5->游戏时间    "},
    {"A6->模式选择    "},
    {"A7->出奖概率    "},
    
    {"A8->爪子起始位置"},
    {"A9->礼品出口位置"},
    {"A10-> 空中抓物  "},
    {"A11-> 自启爪中置"},
    
    {"A12-> 光眼电平  "},
    {"A13-> 赠送票币  "},
    {"A14-> 摇晃清分  "},
    {"A15-> 智能模式  "},
    
    {"      返回      "},
    {"                "},    
    {"                "},
    {"                "},    
};

typedef struct _menuFuntion
{
    u8  ID          ; //编号
    u8 MaxSelect   ; //选择
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

//A1->语言Language
void Menu_A1(void)
{
   LCD_Printf(0, 0, "广告音乐 A1   ");   //                
   LcdUpDataDisplay();                 //刷新所有数据
   delay_ms(2000);
   LCD_Clear();                        //清屏
   LcdUpDataDisplay();                 //刷新所有数据
   LCD_OverUpdate();                   //反白数据单元刷新

}
//A2->广告音乐   
void Menu_A2(void)
{
   LCD_Printf(0, 0, "广告音乐 A1   ");   //                
   LcdUpDataDisplay();                 //刷新所有数据
   delay_ms(2000);
   LCD_Clear();                        //清屏
   LcdUpDataDisplay();                 //刷新所有数据
   LCD_OverUpdate();                   //反白数据单元刷新

}
//A3->几币几局    
//A4->币数保留    
//A5->游戏时间    
//A6->模式选择    
//A7->出奖概率    
//A8->爪子起始位置
//A9->礼品出口位置
//A10-> 空中抓物  
//A11-> 自启爪中置
//A12-> 光眼电平  
//A13-> 赠送票币  
//A14-> 摇晃清分  
//A15-> 智能模式  


//A-> 基础设置
void MenuA(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    基础设置    ");   // 0
                    LCD_Printf(1, 0, "A1->语言Language");   // 1
                    LCD_Printf(2, 0, "A2->广告音乐    ");   // 2
                    LCD_Printf(3, 0, "A3->几币几局    ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "A4->币数保留    ");   // 4
                    LCD_Printf(1, 0, "A5->游戏时间    ");   // 5
                    LCD_Printf(2, 0, "A6->模式选择    ");   // 6
                    LCD_Printf(3, 0, "A7->出奖概率    ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "A8->爪子起始位置");   // 8
                    LCD_Printf(1, 0, "A9->礼品出口位置");   // 9
                    LCD_Printf(2, 0, "A10-> 空中抓物  ");   // 10
                    LCD_Printf(3, 0, "A11-> 自启爪中置");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> 光眼电平  ");   // 12
                    LCD_Printf(1, 0, "A13-> 赠送票币  ");   // 13
                    LCD_Printf(2, 0, "A14-> 摇晃清分  ");   // 14
                    LCD_Printf(3, 0, "A15-> 智能模式  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      返回      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 16)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}
//B-> 抓力电压设置
#define MenuB_Max  10
void MenuB(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuB_Max;
                }
                if(menu == MenuB_Max || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuB_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  抓力电压设置  ");   // 0
                    LCD_Printf(1, 0, "B1->强抓力电压  ");   // 1
                    LCD_Printf(2, 0, "B2->弱抓力电压  ");   // 2
                    LCD_Printf(3, 0, "B3->弱抓后电压  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->中奖电压    ");   // 4
                    LCD_Printf(1, 0, "B5->强力维持时间");   // 5
                    LCD_Printf(2, 0, "B6->弱力维持时间");   // 6
                    LCD_Printf(3, 0, "B7->强变弱方式  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->放线长度调整");   // 8
                    LCD_Printf(1, 0, "B9->连续强力报错");   // 9
                    LCD_Printf(2, 0, "      返回      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } 
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == MenuB_Max)
                break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}

//C-> 马达速度设定
#define MenuC_Max  4
void MenuC(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuC_Max;
                }
                if(menu == MenuC_Max || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuC_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  马达速度设定  ");   // 0
                    LCD_Printf(1, 0, "C1->前后速度    ");   // 1
                    LCD_Printf(2, 0, "C2->左右速度    ");   // 2
                    LCD_Printf(3, 0, "C3->上下速度    ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "      返回      ");   // 4
                    LCD_Printf(1, 0, "                ");   // 5
                    LCD_Printf(2, 0, "                ");   // 6
                    LCD_Printf(3, 0, "                ");   // 7
                }
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == MenuC_Max){break;}
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//D-> 测试
#define MenuD_Max  4
void MenuD(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuD_Max;
                }
                if(menu == MenuD_Max || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuD_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "光眼测试        ");   // 0
                    LCD_Printf(1, 0, "后微动  前微动  ");   // 1
                    LCD_Printf(2, 0, "左微动  右微动  ");   // 2
                    LCD_Printf(3, 0, "上微动  下微动  ");   // 3
                }
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == MenuD_Max)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//E-> 工厂测试 
void MenuE(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                PageUpload = 1;    //页刷新

            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                PageUpload = 1;    //页刷新

            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    工厂测试    ");   // 0
                    LCD_Printf(1, 0, "                ");   // 1
                    LCD_Printf(2, 0, "   -> 调整      ");   // 2
                    LCD_Printf(3, 0, "   -> 保存返回  ");   // 3
                  
//                    LCD_Printf(1, 0, "                ");   // 0
//                    LCD_Printf(1, 0, "爪子移向中央....");   // 1
//                    LCD_Printf(1, 0, "爪子下移........");   // 2
//                    LCD_Printf(1, 0, "爪子上移........");   // 3 
//                    LCD_Printf(1, 0, "爪子移向出口位置");   // 3 
//                    LCD_Printf(1, 0, "爪子复位        ");   // 3                     
                }
                LcdUpDataDisplay();                    //刷新所有数据
                LCD_Display16x16Picture(2,0,1,(u8*)LCD_icon[0]);
                LCD_Display16x16Picture(3,0,1,(u8*)LCD_icon[1]);                  
            }
        }
        //下一级
        else if(port == 2) {
            break;            

        }
    }
    LCD_Display16x16Picture(2,0,0,(u8*)LCD_icon[0]);
    LCD_Display16x16Picture(3,0,0,(u8*)LCD_icon[1]);  
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//F-> 资料清除
#define MenuF_Max  4
void MenuF(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = MenuF_Max;
                }
                if(menu == MenuF_Max || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == (MenuF_Max+1)) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    资料清除    ");   // 0
                    LCD_Printf(1, 0, "F1->账目清零    ");   // 1
                    LCD_Printf(2, 0, "F2->账目清零    ");   // 2
                    LCD_Printf(3, 0, "F3->概率结算清零");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "      返回      ");   // 4
                    LCD_Printf(1, 0, "                ");   // 5
                    LCD_Printf(2, 0, "                ");   // 6
                    LCD_Printf(3, 0, "                ");   // 7
                } 
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == MenuF_Max)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//G-> 免费模式  

void MenuG(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "    免费模式    ");   // 0
                    LCD_Printf(1, 0, "    关          ");   // 1
                    LCD_Printf(2, 0, " + -> 调整      ");   // 2
                    LCD_Printf(3, 0, " ● -> 保存返回  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->中奖电压    ");   // 4
                    LCD_Printf(1, 0, "B5->强力维持时间");   // 5
                    LCD_Printf(2, 0, "B6->弱力维持时间");   // 6
                    LCD_Printf(3, 0, "B7->强变弱方式  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->放线长度调整");   // 8
                    LCD_Printf(1, 0, "B9->连续强力报错");   // 9
                    LCD_Printf(2, 0, "      返回      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> 光眼电平  ");   // 12
                    LCD_Printf(1, 0, "A13-> 赠送票币  ");   // 13
                    LCD_Printf(2, 0, "A14-> 摇晃清分  ");   // 14
                    LCD_Printf(3, 0, "A15-> 智能模式  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      返回      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 16)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//H-> 恢复工厂设置
void MenuH(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  抓力电压设置  ");   // 0
                    LCD_Printf(1, 0, "B1->强抓力电压  ");   // 1
                    LCD_Printf(2, 0, "B2->弱抓力电压  ");   // 2
                    LCD_Printf(3, 0, "B3->弱抓后电压  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->中奖电压    ");   // 4
                    LCD_Printf(1, 0, "B5->强力维持时间");   // 5
                    LCD_Printf(2, 0, "B6->弱力维持时间");   // 6
                    LCD_Printf(3, 0, "B7->强变弱方式  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->放线长度调整");   // 8
                    LCD_Printf(1, 0, "B9->连续强力报错");   // 9
                    LCD_Printf(2, 0, "      返回      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> 光眼电平  ");   // 12
                    LCD_Printf(1, 0, "A13-> 赠送票币  ");   // 13
                    LCD_Printf(2, 0, "A14-> 摇晃清分  ");   // 14
                    LCD_Printf(3, 0, "A15-> 智能模式  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      返回      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 16)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//I-> 查账 
void MenuI(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  抓力电压设置  ");   // 0
                    LCD_Printf(1, 0, "B1->强抓力电压  ");   // 1
                    LCD_Printf(2, 0, "B2->弱抓力电压  ");   // 2
                    LCD_Printf(3, 0, "B3->弱抓后电压  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->中奖电压    ");   // 4
                    LCD_Printf(1, 0, "B5->强力维持时间");   // 5
                    LCD_Printf(2, 0, "B6->弱力维持时间");   // 6
                    LCD_Printf(3, 0, "B7->强变弱方式  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->放线长度调整");   // 8
                    LCD_Printf(1, 0, "B9->连续强力报错");   // 9
                    LCD_Printf(2, 0, "      返回      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> 光眼电平  ");   // 12
                    LCD_Printf(1, 0, "A13-> 赠送票币  ");   // 13
                    LCD_Printf(2, 0, "A14-> 摇晃清分  ");   // 14
                    LCD_Printf(3, 0, "A15-> 智能模式  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      返回      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 16)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//J-> 摇杆前后倒置
void MenuJ(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  抓力电压设置  ");   // 0
                    LCD_Printf(1, 0, "B1->强抓力电压  ");   // 1
                    LCD_Printf(2, 0, "B2->弱抓力电压  ");   // 2
                    LCD_Printf(3, 0, "B3->弱抓后电压  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->中奖电压    ");   // 4
                    LCD_Printf(1, 0, "B5->强力维持时间");   // 5
                    LCD_Printf(2, 0, "B6->弱力维持时间");   // 6
                    LCD_Printf(3, 0, "B7->强变弱方式  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->放线长度调整");   // 8
                    LCD_Printf(1, 0, "B9->连续强力报错");   // 9
                    LCD_Printf(2, 0, "      返回      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> 光眼电平  ");   // 12
                    LCD_Printf(1, 0, "A13-> 赠送票币  ");   // 13
                    LCD_Printf(2, 0, "A14-> 摇晃清分  ");   // 14
                    LCD_Printf(3, 0, "A15-> 智能模式  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      返回      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 16)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//K-> 外接通讯设置
void MenuK(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  抓力电压设置  ");   // 0
                    LCD_Printf(1, 0, "B1->强抓力电压  ");   // 1
                    LCD_Printf(2, 0, "B2->弱抓力电压  ");   // 2
                    LCD_Printf(3, 0, "B3->弱抓后电压  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->中奖电压    ");   // 4
                    LCD_Printf(1, 0, "B5->强力维持时间");   // 5
                    LCD_Printf(2, 0, "B6->弱力维持时间");   // 6
                    LCD_Printf(3, 0, "B7->强变弱方式  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->放线长度调整");   // 8
                    LCD_Printf(1, 0, "B9->连续强力报错");   // 9
                    LCD_Printf(2, 0, "      返回      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> 光眼电平  ");   // 12
                    LCD_Printf(1, 0, "A13-> 赠送票币  ");   // 13
                    LCD_Printf(2, 0, "A14-> 摇晃清分  ");   // 14
                    LCD_Printf(3, 0, "A15-> 智能模式  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      返回      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 16)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}


//L-> 手机设置    
 void MenuL(void)
{
    u8 port = 0;
    u8 menu = 1;
    u8 OverUpdate = 1;
    u8 PageUpload = 1;
    while(1) {
        if(port == 0) {        //初始化
            LcdUpDataDisplay();  //刷新所有数据
            LCDline_over(1);     //单行反白
            clearKeyShortFlag(KeyHook);
            clearKeyShortFlag(KeyBack);
            clearKeyShortFlag(KeyFront);
            port = 1;
        } else if(port == 1) {   //操作
            //上  =========================
            if(getKeyShortFlag(KeyFront)) {
                clearKeyShortFlag(KeyFront);
                OverUpdate = 1;
                menu--;
                if(menu == 0) {
                    menu = 16;
                }
                if(menu == 16 || (menu%4) == 3) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下  =========================
            if(getKeyShortFlag(KeyBack)) {
                clearKeyShortFlag(KeyBack);
                OverUpdate = 1;
                menu++;
                if(menu == 17) {
                    menu = 1;
                }
                if(menu == 1 || (menu %4)== 0) {
                    PageUpload = 1;    //页刷新
                }
            }
            //下勾 =========================
            if(getKeyShortFlag(KeyHook)) {
                clearKeyShortFlag(KeyHook);
                port = 2;
            }
            //页刷新 ***********************
            if(PageUpload) {
                PageUpload = 0;
                if(menu / 4 == 0) {
                    LCD_Printf(0, 0, "  抓力电压设置  ");   // 0
                    LCD_Printf(1, 0, "B1->强抓力电压  ");   // 1
                    LCD_Printf(2, 0, "B2->弱抓力电压  ");   // 2
                    LCD_Printf(3, 0, "B3->弱抓后电压  ");   // 3
                } else if(menu / 4 == 1) {
                    LCD_Printf(0, 0, "B4->中奖电压    ");   // 4
                    LCD_Printf(1, 0, "B5->强力维持时间");   // 5
                    LCD_Printf(2, 0, "B6->弱力维持时间");   // 6
                    LCD_Printf(3, 0, "B7->强变弱方式  ");   // 7
                } else if(menu / 4 == 2) {
                    LCD_Printf(0, 0, "B8->放线长度调整");   // 8
                    LCD_Printf(1, 0, "B9->连续强力报错");   // 9
                    LCD_Printf(2, 0, "      返回      ");   // 10
                    LCD_Printf(3, 0, "                ");   // 11
                } else if(menu / 4 == 3) {
                    LCD_Printf(0, 0, "A12-> 光眼电平  ");   // 12
                    LCD_Printf(1, 0, "A13-> 赠送票币  ");   // 13
                    LCD_Printf(2, 0, "A14-> 摇晃清分  ");   // 14
                    LCD_Printf(3, 0, "A15-> 智能模式  ");   // 15
                } else if(menu / 4 == 4) {
                    LCD_Printf(0, 0, "      返回      ");   // 16
                    LCD_Printf(1, 0, "                ");   // 
                    LCD_Printf(2, 0, "                ");   //
                    LCD_Printf(3, 0, "                ");   //
                }
                
                LcdUpDataDisplay();                    //刷新所有数据
            }
            //刷新反白位置 *****************
            if(OverUpdate) {
                OverUpdate = 0;
                LCDline_over(menu);
            }
        }
        //下一级
        else if(port == 2) {
            if((menu-1) < sizeof(menu_2) / sizeof(void (*)(void))&& menu_2[menu-1]!=NULL) {
//                menu_2[menu-1]();  //下一级菜单运行
            }
            if(menu == 16)break;
            port = 1;       //可操作
            PageUpload = 1; //页刷新
            OverUpdate = 1; //位置刷新  

        }
    }
    LCD_Clear();         //清屏
    LcdUpDataDisplay();  //刷新所有数据
    LCD_OverUpdate();    //反白数据单元刷新
}

 











/********************************End of
 * File************************************/
