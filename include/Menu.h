#include <stdint.h>



class menu {
public:
	uint8_t Menu_System_State = false; //菜单系统状态 0:自动退出 1:运行菜单系统
	uint32_t pages_Tip_Display_timer;
	uint8_t Menu_JumpAndExit;
	uint8_t Menu_JumpAndExit_Level;

	void System_UI(void);

	int Get_Real_Menu_Level_Id(uint8_t id);
	int Get_Menu_Id(uint8_t lid, uint8_t id);

	void Menu_Control(void);
	void Draw_APP(int x, int y, char* bitmap);

	void Run_Menu_Id(uint8_t lid, uint8_t id);
	void Next_Menu(void);
	void Exit_Menu_System(void);
	void Save_Exit_Menu_System(void);
	void JumpWithTitle(void);

	void Smooth_Animation_System(void);
	void About(void);
private:
	uint8_t _GlobalLayerNum = 0;

};




/*
	@名称 Slide_Bar
	@变量
		x    值
		min  最小值
		max  最大值
		step 步进
*/
struct Slide_Bar {
	float* x;	 //值
	float min;
	float max;
	float step;
};

extern struct Slide_Bar Slide_space[];

struct Menu_Level_System {
	uint8_t id;   //当前层id
	int x;   //当前层所指选项
	uint8_t min;  //最小页
	uint8_t max;  //最大页
	uint8_t a;    //参数A 0:无动作 1:开启图标化菜单
};

struct Menu_System {
	uint8_t lid;     //层id
	uint8_t id;      //选项id
	uint8_t x;       //执行操作 0-跳转到菜单 1-执行函数 2-菜单名 3-开关控件 4-滑动条控件
	const char* name;  //选项名称
	uint8_t* icon;
	uint8_t a; //附加参数_1 (0-jump_Level_id) (3-开关id) (4-滑动条id)
	uint8_t b; //附加参数_2 (0-jump_id) (4-滑动条：true?执行函数:无操作)
	void (*function)();
};






// //系统UI
// void menu::System_UI(void) {
//     if (Menu) {
//         _disp.clearBuffer();
//         Smooth_Animation_System(); //计算过渡动画

//         //分别获取 菜单层、菜单项 索引值
//         real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId); // MenuLevel global id -> MenuLevel[id]
//         Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + (int)*Slide_space[Slide_space_Scroll].x); // Menu global id -> menu[id]

//         //若当前菜单层级没有开题图标化则使用普通文本菜单的模式进行渲染显示 若屏幕分辨率低于128*32 则强制启用文本菜单模式
//         if (!MenuLevel[real_Level_Id].a || SCREEN_ROW <= 32) { // 文本菜单模式

//             //显示菜单项目名, 这里有两行文字是在屏幕外 用于动过渡动画
//             for (int i = -1;i < SCREEN_PAGE_NUM / 2 + 1;i++) { //屏幕可以显示多少行
//                 if (MenuLevel[real_Level_Id].x + i >= 0 && MenuLevel[real_Level_Id].x + i <= MenuLevel[real_Level_Id].max) { // 

//                     //绘制目录树
//                     if (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2) { // x != 2 不是菜单名
//                         // _disp.drawUTF8(x， y， char)
//                         _disp.drawUTF8(0, (1 - Menu_Smooth_Animation[3].x * (i != -1)) * ((i + Menu_Smooth_Animation[0].x) * 16 + 1), Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x == 0 ? "+" : "-"); // 0-跳转到菜单
//                     }

//                     //绘制目录名 _disp.drawUTF8(x,y+1,s);
//                     // 不是菜单名 * 7
//                     Draw_Utf(7 * (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x != 2), (1 - Menu_Smooth_Animation[3].x * (i != -1)) * ((i + Menu_Smooth_Animation[0].x) * 16 + 1), Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].name); // 选项名称

//                     //对特殊菜单控件的分类渲染
//                     switch (Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].x) {

//                         //开关控件
//                     case 3:
//                         Draw_Utf(SCREEN_COLUMN - 32 - 1, (i + Menu_Smooth_Animation[0].x) * 16 + 1, *Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] ? (char*)"开启" : (char*)"关闭");
//                         break;

//                         //滑动条
//                     case 4:
//                         char buffer[20];
//                         sprintf(buffer,"%.2f", *Slide_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a].x);
//                         Draw_Utf(SCREEN_COLUMN - 9 - _disp.getUTF8Width(buffer), \
//                             (int)((i + Menu_Smooth_Animation[0].x) * 16), \
//                             buffer);
//                         break;

//                         //单选框
//                     case 5:
//                         if ((*Switch_space[Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].a] == Menu[Get_Menu_Id(real_Level_Id, MenuLevel[real_Level_Id].x + i)].b)) {
//                             Draw_Slow_Bitmap(SCREEN_COLUMN - 32 - 1 + 15, \
//                                 (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
//                                 CheckBoxSelection, \
//                                 10, 10);
//                         } else {
//                             _disp.drawFrame(SCREEN_COLUMN - 32 - 1 + 15, \
//                                 (i + Menu_Smooth_Animation[0].x) * 16 + 2, \
//                                 10, 10);
//                         }
//                         //当前项高亮
//                         if ((int)*Slide_space[Slide_space_Scroll].x == i) {
//                             _disp.setDrawColor(2);
//                             _disp.drawBox(SCREEN_COLUMN - 32 - 2 + 15, \
//                                 (i + Menu_Smooth_Animation[0].x) * 16 + 1, \
//                                 12, 12);
//                             _disp.setDrawColor(1);
//                         }
                    
//                         break;
//                     default:
//                         break;
//                     }

//                 }
//             }

//             //绘制滚动条
//             Draw_Scale(SCREEN_COLUMN - RollingStripWidth, 0, RollingStripWidth, SCREEN_ROW - 1, MenuLevel[real_Level_Id].max + 1, map(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + *Slide_space[Slide_space_Scroll].x, 0, MenuLevel[real_Level_Id].max + 1, -Menu_Smooth_Animation[1].x * (SCREEN_ROW / (MenuLevel[real_Level_Id].max + 1)), SCREEN_ROW - 1));

//             //显示页码角标
//             Page_Footnotes(MenuLevel[real_Level_Id].x + 1 + (int)*Slide_space[Slide_space_Scroll].x, MenuLevel[real_Level_Id].max + 1);

//             //反色高亮被选项
//             _disp.setDrawColor(2);
//             _disp.drawRBox(0, \
//                                 ((int)*Slide_space[Slide_space_Scroll].x - Menu_Smooth_Animation[1].x) * 16, \
//                                 *Switch_space[SwitchSpace_OptionStripFixedLength]?123:(Get_UTF8_Ascii_Pix_Len(1,Menu[Pos_Id].name) - Menu_Smooth_Animation[2].x + 12 * (Menu[Pos_Id].x != 2) + 1), \
//                                 CNSize + 2 , \
//                                 0);
//             _disp.setDrawColor(1);

//             //项目滚动处理
//             *Slide_space[Slide_space_Scroll].x = sys_Counter_Get() - 1;
//             if ((int)*Slide_space[Slide_space_Scroll].x >= Slide_space[Slide_space_Scroll].max) {
//                 Log(LOG_INFO, "尝试往下滚动");
//                 MenuLevel[real_Level_Id].x++;
//                 sys_Counter_SetVal(Slide_space[Slide_space_Scroll].max);
//             }else if ((int)*Slide_space[Slide_space_Scroll].x <= -1) {
//                 Log(LOG_INFO,"尝试往上滚动");
//                 MenuLevel[real_Level_Id].x--;
//                 sys_Counter_SetVal(1);
//             }
//             //编码器控制页内选择框滚动选择
//             //CountMax = constrain(MenuLevel[real_Level_Id].max - MenuLevel[real_Level_Id].x + 1, 0, 7);
//             *Slide_space[Slide_space_Scroll].x = constrain((int)*Slide_space[Slide_space_Scroll].x, 0, Slide_space[Slide_space_Scroll].max - 1);

//             // *Slide_space[Slide_space_Scroll].x = constrain(*Slide_space[Slide_space_Scroll].x, 0, min((int)Slide_space[Slide_space_Scroll].max - 2, (int)MenuLevel[real_Level_Id].max));
//             MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x = constrain(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x, \
//                                                                         MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].min, \
//                 (MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max > Slide_space[Slide_space_Scroll].max - 1)?(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].max - (Slide_space[Slide_space_Scroll].max - 1)):0);

//             //更新过渡动画
//             real_Level_Id = Get_Real_Menu_Level_Id(MenuLevelId);
//             Pos_Id = Get_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id, MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].x + (int)*Slide_space[Slide_space_Scroll].x);
//             Menu_Smooth_Animation[0].val = MenuLevel[real_Level_Id].x;
//             Menu_Smooth_Animation[1].val = MenuLevel[real_Level_Id].x + (int)*Slide_space[Slide_space_Scroll].x;
//             Menu_Smooth_Animation[2].val = Get_UTF8_Ascii_Pix_Len(1,Menu[Pos_Id].name);

//         } else { //图标化
//             /*  当前菜单使用了图标化的渲染方式 该模式仅支持128*64的屏幕 若宏定义中选择了128*32的屏幕将自动切换为普通文本模式显示菜单
//                 接受两种尺寸的图标 14x14(推荐) 和 48*48 （不推荐）
//                 如果为14x14在128*64屏幕中会自动放大到48*48
//             */
//             int id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x);
//             int Pos_Id;

//             //居中显示项目名
//             Draw_Utf(UTF8_HMiddle(0,128,1,Menu[id].name), 50, Menu[id].name);
            
            
//             for (uint8_t i = 0; i < 5; i++) {
//                 Pos_Id = Get_Menu_Id(MenuLevel[real_Level_Id].id, MenuLevel[real_Level_Id].x + i - 2);

//                 if (MenuLevel[real_Level_Id].x - 2 + i >= 0 && MenuLevel[real_Level_Id].x - 2 + i <= MenuLevel[real_Level_Id].max) {
//                     //绘制菜单项目图标
//                     if (Menu[id].x != 2) {
//                         if (Menu[Pos_Id].x != 2) {
//                             Draw_APP((1 - Menu_Smooth_Animation[3].x * (i != -1))*(-69 + i * 56 + Menu_Smooth_Animation[0].x * 56), 3, Menu[Pos_Id].icon);
//                         }
//                     }
//                 }
//             }

//             MenuLevel[real_Level_Id].x = sys_Counter_Get();
//             Menu_Smooth_Animation[0].val=MenuLevel[real_Level_Id].x;
//         }
        
//         //编码器按下事件
//         //菜单被选项激活 触发菜单被选项预设事件
//         switch(SYSKey) {
//             case 1:
//             case 3:
//                 //单击和双击则执行当前项目
//                 Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,MenuLevel[real_Level_Id].x +  *Slide_space[Slide_space_Scroll].x);
//             break;
//             case 2:
//                 //长按执行 标题跳转
//                 Run_Menu_Id(MenuLevel[Get_Real_Menu_Level_Id(MenuLevelId)].id,0);
//             break;
            
//             default:break;
//         }
        
//     }else{
//         if (sys_status.SleepEvent) RunSleepLoop(); //睡眠模式屏保入口
//         else {
//             if (PanelSettings) DetailInfo(void);
//             else void BriefInfo(void);
//         }
//     }
//     Display();
// }
