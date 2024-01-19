#include "Menu.h"
#include "Bitmap.h"
#include "data.h"



//跳转即退出菜单，该标志位适用于快速打开菜单设置，当遇到跳转操作时将保存设置并退出菜单
uint8_t Menu_JumpAndExit  = false;
uint8_t Menu_JumpAndExit_Level = 255;   //当跳转完成后 的 菜单层级 等于“跳转即退出层级”时，“跳转即退出”立马生效

//菜单层id 菜单项目id
uint8_t MenuLevelId = 0, LastMenuLevelId =0, MenuId = 0; //标注值
int32_t real_Level_Id,Pos_Id,Back_Id=-1; //索引值


//全局变量 控制右下角角标显示时间
uint32_t pages_Tip_Display_timer = 0;
//默认无动作后的1500ms关闭悬浮角标显示
#define pages_Tip_Display_Timeout 1500

/*/////////////////////////////////////////////////////////////////////

    @需要手动设置的项目

*//////////////////////////////////////////////////////////////////////
int     MenuScroll = 0;


//由于sizeof的原理是在编译阶段由编译器替换数组大小，因此我们无法计算指针的sizeof！需要在位图的第一个字节填写 n 阶矩阵图像
/*
    @变量组 *Switch_space[]
    @brief 开关控件 用于存储开关控件的值
    @param -
    
    @注册
        -0->平滑动画
        -1->单选框测试
        -2->选项条固定长度
*/
enum Switch_space_Obj{
    SwitchSpace_SmoothAnimation = 0,
    SwitchSpace_OptionStripFixedLength,

    SwitchSpace_PIDMode,
    SwitchSpace_KFP,
    SwitchSpace_PanelSettings,
    SwitchSpace_ScreenFlip,
    SwitchSpace_Volume,
    SwitchSpace_RotaryDirection,
    SwitchSpace_HandleTrigger,
    SwitchSpace_Language,
    SwitchSpace_TipID,

    SwitchSpace_BLE_State,
    SwitchSpace_MenuListMode,
};

uint8_t *Switch_space[] = {
    &SmoothAnimation_Flag,
    &OptionStripFixedLength_Flag,

    &PIDMode,
    &Use_KFP,
    &PanelSettings,
    &ScreenFlip,
    &Volume,
    &RotaryDirection,
    &HandleTrigger,
    &Language,

    &MenuListMode,
};

/*
    @结构体 Slide_Bar Slide_space[]
    @brief 滑动条控件 用于存储滑动条控件的值
    @param -
    
    @注册
        -0->屏幕亮度设定值
        -1->自适应菜单滚动范围
    @变量
        int   x    值
        int   min  最小值
        int   max  最大值
        int   step 步进
*/
enum Slide_space_Obj{
    Slide_space_ScreenBrightness = 0,
    Slide_space_Scroll,

    Slide_space_BootTemp,
    Slide_space_SleepTemp,
    Slide_space_BoostTemp,

    Slide_space_ShutdownTime,
    Slide_space_SleepTime,
    Slide_space_ScreenProtectorTime,

    Slide_space_UndervoltageAlert,

    Slide_space_PID_AP,
    Slide_space_PID_AI,
    Slide_space_PID_AD,
    Slide_space_PID_CP,
    Slide_space_PID_CI,
    Slide_space_PID_CD,

    Slide_space_KFP_Q,
    Slide_space_KFP_R,

    Slide_space_SamplingRatioWork,
    Slide_space_ADC_PID_Cycle_List_0,
    Slide_space_ADC_PID_Cycle_List_1,
    Slide_space_ADC_PID_Cycle_List_2,

};

struct Slide_Bar Slide_space[] = {
    {(float*)&ScreenBrightness,0,255,16}, //亮度调整
    {(float*)&MenuScroll,0,SCREEN_ROW / 16,1}, //自适应菜单滚动范围

    {(float*)&BootTemp,150,400,5},
    {(float*)&SleepTemp,150,400,5},
    {(float*)&BoostTemp,0,150,1},

    {(float*)&ShutdownTime,0,60,1},
    {(float*)&SleepTime,0,60,1},
    {(float*)&ScreenProtectorTime,0,600,1},

    {(float*)&UndervoltageAlert,0,36,0.25},


    {(float*)&KFP_Temp.Q,0,5,0.01},
    {(float*)&KFP_Temp.R,0,25,0.1},


};

Slide_space[Slide_space_ScreenBrightness].x;


/*
	@名称 Smooth_Animation
	@变量
		x    过渡值
		last 上次的值
		val 目标值
		w    平滑权重
*/
struct Smooth_Animation {
	float x;	 //过渡值
	int last;	 //上次的值
	int val;	 //目标值
	float w; 	 //平滑权重
	uint8_t a;   //是否允许累加
	uint8_t b;   //选择过渡动画计算函数
};

/*
    @结构体 Smooth_Animation Menu_Smooth_Animation[]
    @brief 存放过渡动画设定集
    @param -
    
    @变量
        float   x   过渡值
        int32_t     last 上次的值
        int32_t     val 当前值
        float   w   平滑权重
        uint8_t a   是否允许累加
        uint8_t b   选择过渡动画计算函数
*/
#define Smooth_Animation_Num sizeof(Menu_Smooth_Animation)/sizeof(Menu_Smooth_Animation[0])
struct Smooth_Animation Menu_Smooth_Animation[] = {
    {0,0,0,0.4,1,0}, //菜单项目滚动动画
    {0,0,0,0.15,1,0}, //滚动条平滑动画
    {0,0,0,0.15,1,0}, //菜单选项条长度平滑动画
    {0,0,0,0.2,0,0}, //项目归位动画
};


/*
    @结构体 Menu_Level_System MenuLevel[]
    @brief 菜单层级 设定
    @param -
    
    @变量
        uint8_t id    当前层id
        int x    当前层所指选项
        uint8_t min   最小页
        uint8_t max   最大页
        uint8_t a     参数A 0:无动作 1:开启图标化菜单
*/

#define Menu_NULL_IMG 0
#define Menu_HAVE_IMG 1

struct Menu_Level_System MenuLevel[] = {
    {0,0,0,3,Menu_NULL_IMG},
    {1,0,0,5,Menu_HAVE_IMG},
    {2,0,0,8,Menu_HAVE_IMG},
    {3,0,0,4,Menu_HAVE_IMG},
    {4,0,0,5,Menu_HAVE_IMG},
    {5,0,0,7,Menu_HAVE_IMG},
    {6,0,0,5,Menu_HAVE_IMG},
    {7,0,0,7,Menu_HAVE_IMG},
    {8,0,0,2,Menu_HAVE_IMG},
    {9,0,0,9,Menu_NULL_IMG},
    {10,0,0,2,Menu_HAVE_IMG},
    {11,0,0,2,Menu_HAVE_IMG},
    {12,0,0,2,Menu_HAVE_IMG},
    {13,0,0,1,Menu_HAVE_IMG},
    {14,0,0,2,Menu_HAVE_IMG},
    {16,0,0,3,Menu_NULL_IMG},
    {17,0,0,4,Menu_NULL_IMG},
    {18,0,0,4,Menu_NULL_IMG},
    {19,0,0,5,Menu_NULL_IMG},
    {20,0,0,4,Menu_NULL_IMG},
    {21,0,0,4,Menu_NULL_IMG},
    {22,0,0,3,Menu_NULL_IMG},
};

/*
    @结构体 Menu_System Menu[]
    @brief 菜单层级项目 设定
    @param -
    
    @变量
        uint8_t lid             层id
        uint8_t id              选项id
        uint8_t x               执行操作 0-跳转到菜单 1-执行函数 2-菜单名 3-开关控件 4-滑动条控件
        char name[21]      选项名称 支持半角
        char *icon         图标(此项选填 若当前层级菜单开启了图标化) 详细可以参考关于图标化的说明文档
        uint8_t a               附加参数_1 (0-jump_Level_id) (3-开关id) (4-滑动条id)
        uint8_t b               附加参数_2 (0-jump_id) (4-滑动条：true?执行函数:无操作)
        void (*function)(void);
*/
#define Menu_NULL_F                 0
#define Jump_Menu_Op                0
#define F_Menu_Op                   1
#define Title_Menu_Op               2
#define Switch_Menu_Op              3
#define Progress_Bar_Menu_Op        4
#define SingleBox_Menu_Op           5
#define Menu_NULL_OP                6

struct Menu_System Menu[] = {
    {0,0, Title_Menu_Op, "欢迎使用[朱雀]控制器", Menu_NULL_IMG, 0, 0, *Save_Exit_Menu_System},
    {5,2, Jump_Menu_Op, "配对", IMG_BLE, 22, 0, Menu_NULL_F},
    {2,5, F_Menu_Op, "手动添加", IMG_Files, 0, 0, Menu_NULL_F},
    {5,2, Jump_Menu_Op, "蓝牙", IMG_BLE, 22, 0, Menu_NULL_F},
    {2,3, F_Menu_Op, "校准", Set9, 0, 0, Menu_NULL_F},
    {19,4, Jump_Menu_Op, "卡尔曼滤波器", Menu_NULL_IMG, 20, 0, Menu_NULL_F },
    {5,3, Progress_Bar_Menu_Op, "欠压提醒", Set6, Slide_space_UndervoltageAlert, 0, Menu_NULL_F},
    {5,1, Jump_Menu_Op, "个性化", IMG_Pen, 6, 0, Menu_NULL_F},
    {5,5, Jump_Menu_Op, "语言设置", Set_LANG, 13, 0, Menu_NULL_F},
    {5,6, F_Menu_Op, "关于", QRC, 5, 5, *About},
    {0,3, F_Menu_Op, "返回", Menu_NULL_IMG, 0, 0, *Save_Exit_Menu_System},

    {22, 0, Title_Menu_Op, "蓝牙", Menu_NULL_IMG, 5, 2, Menu_NULL_F},
    {22, 1, Switch_Menu_Op, "状态", Menu_NULL_IMG, SwitchSpace_BLE_State, 0, Menu_NULL_F}, // <---
    {22, 2, F_Menu_Op, "设备名称", Menu_NULL_IMG, 22, 2, Menu_NULL_F}, // <---
    {22, 3, Jump_Menu_Op, "返回", Menu_NULL_IMG, 5, 2, Menu_NULL_F},

    {20,0, Title_Menu_Op, "卡尔曼滤波器", Menu_NULL_IMG, 19, 4, Menu_NULL_F },
    {20,1, Switch_Menu_Op, "启用状态", Menu_NULL_IMG, SwitchSpace_KFP, 0, Menu_NULL_F },
    {20,2, Progress_Bar_Menu_Op, "过程噪声协方差", Menu_NULL_IMG, Slide_space_KFP_Q, 0, Menu_NULL_F },
    {20,3, Progress_Bar_Menu_Op, "观察噪声协方差", Menu_NULL_IMG, Slide_space_KFP_R, 0, Menu_NULL_F },
    {20,4, Jump_Menu_Op, "返回", Menu_NULL_IMG, 19, 4, Menu_NULL_F },

    { 6,0, Title_Menu_Op, "个性化", Menu_NULL_IMG, 5, 1, Menu_NULL_F},
    { 7,4, Progress_Bar_Menu_Op, "屏幕亮度", IMG_Sun, Slide_space_ScreenBrightness, 1, *Update_OLED_Light_Level},
    { 4,3, Progress_Bar_Menu_Op, "休眠触发(分)", Set11, Slide_space_SleepTime, 0, Menu_NULL_F},
    { 8,0, Jump_Menu_Op, "面板设置", Menu_NULL_IMG, 7, 1, Menu_NULL_F},
    { 6,2, Jump_Menu_Op, "声音设置", Set5, 10, 0, Menu_NULL_F},
    { 6,5, Jump_Menu_Op, "返回", Set7, 5, 1, Menu_NULL_F},

    { 13,0, Title_Menu_Op, "语言设置", Menu_NULL_IMG, 5, 5, Menu_NULL_F},
    { 13,1, SingleBox_Menu_Op, "English", Lang_EN, SwitchSpace_Language, LANG_Chinese, *JumpWithTitle},
    { 13,1, SingleBox_Menu_Op, "简体中文", Lang_CN, SwitchSpace_Language, LANG_Chinese, *JumpWithTitle},
 
    { 8,0, Title_Menu_Op, "面板设置", Menu_NULL_IMG, 7, 1, Menu_NULL_F},
    { 8,1, SingleBox_Menu_Op, "简约", Set17, SwitchSpace_PanelSettings, 0, *JumpWithTitle},
    { 8,2, SingleBox_Menu_Op, "详细", Set18, SwitchSpace_PanelSettings, 1, *JumpWithTitle},
 
    { 10,0, Title_Menu_Op, "声音设置", Menu_NULL_IMG, 6, 2, Menu_NULL_F},
    { 10,1, SingleBox_Menu_Op, "开启", Set5, SwitchSpace_Volume, true, *JumpWithTitle},
    { 10,2, SingleBox_Menu_Op, "关闭", Set5_1, SwitchSpace_Volume, false, *JumpWithTitle},

};

//系统UI
void System_UI(void) {
    if (Menu) {
    if (!Menu_System_State) return;
    _disp.clearBuffer();

    //计算过渡动画
    if (*Switch_space[SwitchSpace_SmoothAnimation]) Smooth_Animation_System();

    //分别获取 菜单层、菜单项 索引值
    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + (int)*Slide_space[Slide_space_Scroll].x);

    //若当前菜单层级没有开题图标化则使用普通文本菜单的模式进行渲染显示 若屏幕分辨率低于128*32 则强制启用文本菜单模式
    if (!MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].a || MenuListMode || SCREEN_ROW <= 32) {

        //分别获取 菜单层、菜单项 索引值
        //int id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x);
        //int Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x);
        //显示菜单项目名::这里有两行文字是在屏幕外 用于动过渡动画
        for (int i = -1;i < SCREEN_PAGE_NUM / 2 + 1;i++) {
            if (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + i >= 0 && MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + i <= MenuLevel[real_Level_Id].max) {

                //绘制目录树
                if (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2) {
                    //Set_Font_Size(2);
                    _disp.drawUTF8(0, (1 - Menu_Smooth_Animation[3].x * (i != -1)) * ((i + Menu_Smooth_Animation[0].x) * 16 + 1), Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x == 0 ? "+" : "-");
                }
                //绘制目录名
                Draw_Utf(7 * (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2), (1 - Menu_Smooth_Animation[3].x * (i != -1)) * ((i + Menu_Smooth_Animation[0].x) * 16 + 1), Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].name);

                //对特殊菜单控件的分类渲染
                switch (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x) {

                    //开关控件
                case 3:
                    Draw_Utf(SCREEN_COLUMN - 32 - 1, (i + Menu_Smooth_Animation[0].x) * 16 + 1, *Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] ? (char*)"开启" : (char*)"关闭");
                    break;

                    //滑动条
                case 4:
                    char buffer[20];
                    sprintf(buffer,"%.2f", *Slide_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a].x);
                    Draw_Utf(SCREEN_COLUMN - 9 - _disp.getUTF8Width(buffer), \
                        (int)((i + Menu_Smooth_Animation[0].x) * 16), \
                        buffer);
                    break;

                    //单选框
                case 5:
                    if ((*Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] == Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].b)) {
                        Draw_Slow_Bitmap(SCREEN_COLUMN - 32 - 1 + 15, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                            CheckBoxSelection, \
                            10, 10);
                    } else {
                        _disp.drawFrame(SCREEN_COLUMN - 32 - 1 + 15, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
                            10, 10);
                    }
                    //当前项高亮
                    if ((int)*Slide_space[Slide_space_Scroll].x == i) {
                        _disp.setDrawColor(2);
                        _disp.drawBox(SCREEN_COLUMN - 32 - 2 + 15, \
                            (i + Menu_Smooth_Animation[0].x) * 16 + 1, \
                            12, 12);
                        _disp.setDrawColor(1);
                    }
                
                    break;
                default:
                    break;
                }

            }
        }

        //绘制滚动条
        Draw_Scale(SCREEN_COLUMN - RollingStripWidth, 0, RollingStripWidth, SCREEN_ROW - 1, MenuLevel[real_Level_Id].max + 1, map(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x, 0, MenuLevel[real_Level_Id].max + 1, -Menu_Smooth_Animation[1].x * (SCREEN_ROW / (MenuLevel[real_Level_Id].max + 1)), SCREEN_ROW - 1));

        //显示页码角标
        Page_Footnotes(MenuLevel[real_Level_Id].x + 1 + (int)*Slide_space[Slide_space_Scroll].x, MenuLevel[real_Level_Id].max + 1);

        //反色高亮被选项
        _disp.setDrawColor(2);
        _disp.drawRBox(0, \
                            ((int)*Slide_space[Slide_space_Scroll].x - Menu_Smooth_Animation[1].x) * 16, \
                            *Switch_space[SwitchSpace_OptionStripFixedLength]?123:(Get_UTF8_Ascii_Pix_Len(1,Menu[Pos_Id].name) - Menu_Smooth_Animation[2].x + 12 * (Menu[Pos_Id].x != 2) + 1), \
                            CNSize + 2 , \
                            0);
        _disp.setDrawColor(1);

        //项目滚动处理
        *Slide_space[Slide_space_Scroll].x = sys_Counter_Get() - 1;
        if ((int)*Slide_space[Slide_space_Scroll].x >= Slide_space[Slide_space_Scroll].max) {
            Log(LOG_INFO, "尝试往下滚动");
            MenuLevel[real_Level_Id].x++;
            sys_Counter_SetVal(Slide_space[Slide_space_Scroll].max);
        }else if ((int)*Slide_space[Slide_space_Scroll].x <= -1) {
            Log(LOG_INFO,"尝试往上滚动");
            MenuLevel[real_Level_Id].x--;
            sys_Counter_SetVal(1);
        }
        //编码器控制页内选择框滚动选择
        //CountMax = constrain(MenuLevel[real_Level_Id].max - MenuLevel[real_Level_Id].x + 1, 0, 7);
        *Slide_space[Slide_space_Scroll].x = constrain((int)*Slide_space[Slide_space_Scroll].x, 0, Slide_space[Slide_space_Scroll].max - 1);

        // *Slide_space[Slide_space_Scroll].x = constrain(*Slide_space[Slide_space_Scroll].x, 0, min((int)Slide_space[Slide_space_Scroll].max - 2, (int)MenuLevel[real_Level_Id].max));
        MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x = constrain(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x, \
                                                                    MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].min, \
            (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max > Slide_space[Slide_space_Scroll].max - 1)?(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max - (Slide_space[Slide_space_Scroll].max - 1)):0);

        //更新过渡动画
        real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
        Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + (int)*Slide_space[Slide_space_Scroll].x);
        Menu_Smooth_Animation[0].val = MenuLevel[real_Level_Id].x;
        Menu_Smooth_Animation[1].val = MenuLevel[real_Level_Id].x + (int)*Slide_space[Slide_space_Scroll].x;
        Menu_Smooth_Animation[2].val = Get_UTF8_Ascii_Pix_Len(1,Menu[Pos_Id].name);

    }
    else {
        /*  当前菜单使用了图标化的渲染方式 该模式仅支持128*64的屏幕 若宏定义中选择了128*32的屏幕将自动切换为普通文本模式显示菜单
            接受两种尺寸的图标 14x14(推荐) 和 48*48 （不推荐）
            如果为14x14在128*64屏幕中会自动放大到48*48
        */
        int id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x);
        int Pos_Id;

        //居中显示项目名
        Draw_Utf(UTF8_HMiddle(0,128,1,Menu[id].name), 50, Menu[id].name);
        
        
        for (uint8_t i = 0; i < 5; i++) {
            Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + i - 2);

            if (MenuLevel[real_Level_Id].x - 2 + i >= 0 && MenuLevel[real_Level_Id].x - 2 + i <= MenuLevel[real_Level_Id].max) {
                //绘制菜单项目图标
                if (Menu[id].x != 2) {
                    if (Menu[Pos_Id].x != 2) {
                        Draw_APP((1 - Menu_Smooth_Animation[3].x * (i != -1))*(-69 + i * 56 + Menu_Smooth_Animation[0].x * 56), 3, Menu[Pos_Id].icon);
                    }
                }
            }
        }

        // Serial.print("x1:");
        // Serial.print(MenuLevel[real_Level_Id].x);

        MenuLevel[real_Level_Id].x = sys_Counter_Get();
        Menu_Smooth_Animation[0].val=MenuLevel[real_Level_Id].x;

        // Serial.print(" x2:");
        // Serial.print(MenuLevel[real_Level_Id].x);

        // Serial.print(" 编码器:");
        // Serial.println(sys_Counter_Get());
        
    }
    
    //编码器按下事件
    //菜单被选项激活 触发菜单被选项预设事件
    switch(SYSKey) {
        case 1:
        case 3:
            //单击和双击则执行当前项目
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,MenuLevel[real_Level_Id].x +  *Slide_space[Slide_space_Scroll].x);
        break;
        case 2:
            //长按执行 标题跳转
            Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,0);
        break;
        
        default:break;
    }
    
    }else{
        if (sys_status.SleepEvent) RunSleepLoop(); //睡眠模式屏保入口
        else {
            if (PanelSettings) DetailInfo(void);
            else void BriefInfo(void);
        }
    }
    Display();
}


/*/////////////////////////////////////////////////////////////////////

    @相关工具函数

*//////////////////////////////////////////////////////////////////////
#define RollingStripWidth 3

/*复选框选中 10*10*/
uint8_t CheckBoxSelection[] = { 0xff,0xc0,0x80,0x40,0x80,0xc0,0x81,0xc0,0x81,0xc0,0x83,0x40,0x9b,0x40,0x8e,0x40,0x86,0x40,0xff,0xc0 };

void Save_Exit_Menu_System(void) {
    //过渡离开
    _disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 66 * *Switch_space[SwitchSpace_SmoothAnimation]);
    _disp.setDrawColor(1);

    //保存配置
    SYS_Save();

    Exit_Menu_System();
}

void Exit_Menu_System(void) {
    printf("退出菜单系统\n");
    Menu_System_State = 0;
    Menu_JumpAndExit = false;
    Menu_JumpAndExit_Level = 255;
    
    //退出菜单后重新初始化主界面
    System_UI_Init();
}

//按照标题进行跳转 标题跳转 跳转标题
void JumpWithTitle(void) {
    Run_Menu_Id(MenuLevelId, 0);
}

/*
    @函数 Smooth_Animation_System
    @brief 过渡动画运算
*/
void Smooth_Animation_System() {
    for (uint8_t i = 0;i < Smooth_Animation_Num;i++) {
        //优化计算：变形过滤器
        if (Menu_Smooth_Animation[i].x && abs(Menu_Smooth_Animation[i].x * 100) < 1.5) Menu_Smooth_Animation[i].x = 0;
        //动画控制变量是否需要更新
        if (Menu_Smooth_Animation[i].last != Menu_Smooth_Animation[i].val) {
            //是否允许累加
            if (Menu_Smooth_Animation[i].a)  Menu_Smooth_Animation[i].x += Menu_Smooth_Animation[i].val - Menu_Smooth_Animation[i].last;
            else  Menu_Smooth_Animation[i].x = Menu_Smooth_Animation[i].val - Menu_Smooth_Animation[i].last;
            //重置标志
            Menu_Smooth_Animation[i].last = Menu_Smooth_Animation[i].val;
        }
        //使用被选的动画计算函数计算动画
        switch(Menu_Smooth_Animation[i].b) {
            case 0:
                Menu_Smooth_Animation[i].x -= Menu_Smooth_Animation[i].x * Menu_Smooth_Animation[i].w;
            break;
        }
    }
}

/*
    @函数 Get_Real_Menu_Level_Id
    @brief 获取 菜单层对象 的索引值
    @param uint8_t id 菜单层对象id
    @return 菜单层对象 的索引值
*/
int Get_Real_Menu_Level_Id(uint8_t id) {
    uint8_t real_Level_Id;
    for (uint8_t i = 0;i < sizeof(MenuLevel) / sizeof(MenuLevel[0]);i++)
        if (MenuLevel[i].id == id) real_Level_Id = i;
    return real_Level_Id;
}

/*
    @函数 Get_Menu_Id
    @brief 获取 菜单对象 的索引值
    @param 菜单层对象id 菜单对象id
    @return 菜单对象 的索引值
*/
int Get_Menu_Id(uint8_t lid, uint8_t id) {
    uint8_t real_Level_Id;
    for (uint8_t i = 0;i < sizeof(Menu) / sizeof(Menu[0]);i++)
        if (Menu[i].lid == lid && Menu[i].id == id) real_Level_Id = i;
    return real_Level_Id;
}

/*** 
 * @description: 菜单系统设置编码器
 * @param {*}
 * @return {*}
 */
void MenuSYS_SetCounter() {
    if (!Menu_System_State) return;
    printf("菜单系统设置编码器\n");
    if (!MenuLevel[real_Level_Id].a || MenuListMode || SCREEN_ROW <= 32) {
        //设置编码器滚动范围
        MenuLevel[real_Level_Id].min = 0;   //重置选项最小值：从图标模式切换到列表模式会改变该值
        uint8_t MinimumScrolling = min((int)Slide_space[Slide_space_Scroll].max, (int)MenuLevel[real_Level_Id].max);
        sys_Counter_Set((int)Slide_space[Slide_space_Scroll].min, MinimumScrolling + 1, 1, (int)*Slide_space[Slide_space_Scroll].x + (1)); //+(1) 是因为实际上计算会-1 ,这里要补回来
        
    } else {
        printf("Next_Menu:图标模式\n");
        if (Menu[Get_Menu_Id(real_Level_Id, 0)].x) MenuLevel[real_Level_Id].min = 1; //当前处在图标模式 如果目标层菜单的第一项为标题，则给予屏蔽
        sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
        *Slide_space[Slide_space_Scroll].x = 0;
    }
}

/*
    @函数 Next_Menu
    @brief 多级菜单跳转初始化参数
*/
void Next_Menu() {
    //清除按键缓存
    SYSKey = 255;
    //设置菜单标志位
    Menu_System_State = 1;
    real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    uint8_t Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, 0);
    //设置编码器
    MenuSYS_SetCounter();
    if (*Switch_space[SwitchSpace_SmoothAnimation]) {
        if (LastMenuLevelId != MenuLevelId) {
            _disp.setDrawColor(0);
            Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 4, 20 * *Switch_space[SwitchSpace_SmoothAnimation]);
            _disp.setDrawColor(1);
        }
        //项目归位动画
        Menu_Smooth_Animation[3].last = 0;
        Menu_Smooth_Animation[3].val = 1;
    }
}

/*
    @函数 Run_Menu_Id
    @brief 按照菜单项预设的参数执行命令
    @param uint8_t lid 菜单层对象id 
    @param uint8_t id菜单对象id
*/
void Run_Menu_Id(uint8_t lid, uint8_t id) {
    uint8_t Id;
    uint8_t real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
    Id = Get_Menu_Id(lid, id);
    switch (Menu[Id].x) {
    case 0:
    case 2:
        LastMenuLevelId = MenuLevelId; //决定是否播放转场动画
        MenuLevelId = Menu[Id].a;
        if (!MenuLevel[Menu[Id].a].a || MenuListMode) { //如果当前菜单层没有开启了图表化显示则对子菜单选项定向跳转执行配置

            uint8_t ExcellentLimit = (MenuLevel[MenuLevelId].max + 1) - SCREEN_FONT_ROW; //(MenuLevel[MenuLevelId].max + 1)是为了从1开始计算
            uint8_t ExcellentMedian = (SCREEN_FONT_ROW/2); //注意：这里从1开始计数
            //计算最优显示区域
            if (Menu[Id].b==0) {
                //头只有最差显示区域
                MenuLevel[Menu[Id].a].x = 0;
                *Slide_space[Slide_space_Scroll].x = 0;
            }else if (Menu[Id].b > 0 && Menu[Id].b <= MenuLevel[MenuLevelId].max - ExcellentMedian) {
                //中部拥有绝佳的显示区域
                MenuLevel[Menu[Id].a].x = Menu[Id].b - 1;
                *Slide_space[Slide_space_Scroll].x = 1;
            }else{
                //靠后位置 以及 最差的尾部
                MenuLevel[Menu[Id].a].x = ExcellentLimit;
                *Slide_space[Slide_space_Scroll].x = Menu[Id].b - ExcellentLimit;
            }
        }else{
            //当前是图标模式
            MenuLevel[Menu[Id].a].x = Menu[Id].b;
        }
        //按需求跳转完成后执行函数
        if (Menu[Id].function) Menu[Id].function();
        //检查“跳转即退出”标志
        if (Menu_JumpAndExit && MenuLevelId == Menu_JumpAndExit_Level) Save_Exit_Menu_System();
        //再次确认菜单状态
        if (Menu_System_State) Next_Menu();         //由于执行函数可能会导致菜单状态被更改，所以这里需要确定菜单状态
        break;
    case 1:
        //Pop_Windows("正在处理");
        if (Menu[Id].function) Menu[Id].function();
        MenuSYS_SetCounter();
        break;
    case 3:
        *Switch_space[Menu[Id].a] = !*Switch_space[Menu[Id].a];
        if (Menu[Id].function) Menu[Id].function();
        break;
    case 4:
        //*Switch_space[Menu[Id].a]=!*Switch_space[Menu[Id].a];
        sys_Counter_Set(Slide_space[Menu[Id].a].min, Slide_space[Menu[Id].a].max, Slide_space[Menu[Id].a].step, *Slide_space[Menu[Id].a].x);
        _disp.setDrawColor(0);
        Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 11 * *Switch_space[SwitchSpace_SmoothAnimation]);
        _disp.setDrawColor(1);

        while (!sys_KeyProcess()) {
            _disp.setDrawColor(0);
            _disp.drawBox(SCREEN_COLUMN / 8 - 2, (SCREEN_ROW - 24) / 2 - 3, 3 * SCREEN_COLUMN / 4 + 4, 24 + 4);
            _disp.setDrawColor(1);

            _disp.drawRFrame(SCREEN_COLUMN / 8 - 3, (SCREEN_ROW - 24) / 2 - 4, 3 * SCREEN_COLUMN / 4 + 4, 24 + 6 ,2);

            *Slide_space[Menu[Id].a].x = sys_Counter_Get();
            Draw_Utf(SCREEN_COLUMN / 8, (SCREEN_ROW - 24) / 2, Menu[Id].name);
            Draw_Num_Bar(*Slide_space[Menu[Id].a].x, Slide_space[Menu[Id].a].min, Slide_space[Menu[Id].a].max, SCREEN_COLUMN / 8, (SCREEN_ROW - 24) / 2 + CNSize + 3, 3 * SCREEN_COLUMN / 4, 7, 1);
           
            Display();
            //当前滑动条为屏幕亮度调节 需要特殊设置对屏幕亮度进行实时预览
            if (Menu[Id].function) Menu[Id].function();
        }
        delay(50);

        //sys_Counter_Set(MenuLevel[real_Level_Id].min, MenuLevel[real_Level_Id].max, 1, MenuLevel[real_Level_Id].x);
        if (Menu[Id].function) Menu[Id].function();
        MenuSYS_SetCounter();
        break;
    
    case 5://单选模式
        *Switch_space[Menu[Id].a] = Menu[Id].b;
        if (Menu[Id].function) Menu[Id].function();
        break;
    }
}

void About(void) {
    //播放Logo动画
    //EnterLogo();
    //生成项目QRCode
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];
    qrcode_initText(&qrcode, qrcodeData, 3, 0, "https://github.com/createskyblue/OpenT12");
    Clear();

    uint8_t x_offset = (SCREEN_COLUMN - qrcode.size*2)/2;
    uint8_t y_offset = (SCREEN_ROW - qrcode.size*2)/2;
    
    for (uint8_t y=0;y<qrcode.size;y++)
        for (uint8_t x=0;x<qrcode.size;x++)
            if (qrcode_getModule(&qrcode, x, y)) Draw_Pixel_Resize(x + x_offset,y + y_offset,x_offset,y_offset,2,2);

    _disp.setDrawColor(2);
    _disp.drawBox(x_offset - 2 ,y_offset - 2 ,qrcode.size * 2 + 4,qrcode.size * 2 + 4);
    _disp.setDrawColor(1);

    while(!sys_KeyProcess()) {
        Display();
    }
    //显示系统信息
    Clear();
    char buffer[50];
    for (uint8_t i = 0;i < 5;i++) {
        _disp.setCursor(0, 12 * i + 1);

        switch (i) {
            case 0: sprintf(buffer, "%s Ver-%d",ESP.getChipModel(),(int)ESP.getChipRevision()); break;
            case 1: sprintf(buffer, "%d@%uMHz %uMb U%.1f%%", (int)ESP.getChipCores(),ESP.getCpuFreqMHz(),ESP.getFlashChipSize()/1024/1024,(float)ESP.getSketchSize()/ESP.getFlashChipSize() * 100.0); break;
            case 2: sprintf(buffer, "%s %s", __DATE__,__TIME__); break;
            case 3: sprintf(buffer, "%s", ESP.getSdkVersion()); break;
            case 4: sprintf(buffer, "MAC %s", ChipMAC_S); break;
        }
        _disp.print(buffer);
    }
    while(!sys_KeyProcess()) {
        Display();
    }
}
