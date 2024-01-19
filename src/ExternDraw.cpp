#include "ExternDraw.h"
#define CNSize 12
#define SCREEN_COLUMN 128
#define SCREEN_ROW    64
#define SCREEN_PAGE_NUM 8
#define SCREEN_FONT_ROW 4



void externdraw::Clear(void) {
    _disp.clearBuffer();
}

void externdraw::Display(void) {
    _disp.sendBuffer();
    DisplayFlashTick++;
}

void externdraw::UpdateOLEDLightLevel(void) {
    _disp.sendF("c",0x81);  //向SSD1306发送指令：设置内部电阻微调
    _disp.sendF("c",(uint8_t)); //微调范围（0-255）
}

// void Update_OLED_Flip(void) {
//     _disp.setFlipMode(ScreenFlip);
//     if (Menu_System_State) PopMsg_ScreenFlip();
// }

// void PopMsg_RotaryDirection(void) {
//     char buffer[20];
//     sprintf(buffer, "编码器:%s", (RotaryDirection == true) ? "顺时针" : "逆时针");
//     Pop_Windows(buffer);
//     delay(500);
// }

// void PopMsg_ScreenFlip(void) {
//     char buffer[20];
//     sprintf(buffer, "%s", (ScreenFlip == true) ? "翻转显示" : "正常显示");
//     Pop_Windows(buffer);
//     delay(500);
// }

// void PopMsg_ListMode(void) {
//     char buffer[20];
//     sprintf(buffer, "%s", (MenuListMode == true) ? "列表模式" : "图标模式");
//     Pop_Windows(buffer);
//     delay(500);
//     Next_Menu();
// }

void externdraw::Draw_Utf(int x, int y, const char* s) {
    // _disp.setCursor(x,y + 1);
    // _disp.print(s);
    _disp.drawUTF8(x,y+1,s);
}

/*
@ 作用：抖动1
*/
void externdraw::Blur(int sx, int sy, int ex, int ey, int f,int delayMs) {
	for (int i = 0;i < f;i++) {
		for (int y = 0; y < ey; y++) {
			for (int x = 0; x < ex; x++) {
                if (x % 2 == y % 2 && x % 2 == 0 && x >= sx && x <= ex && y >= sy && y <= ey) _disp.drawPixel(x + (i > 0 && i < 3), y + (i > 1));
                //else _disp.drawPixel(x + (i > 0 && i < 3), y + (i > 1), 0);
			}
		}
		if (delayMs) {
            _disp.sendBuffer();
			delay(delayMs);
		}
	}
		
}

/*
@ 作用：画滚动条
*/
void externdraw::Draw_Scale(int x, int y, int w, int h, int s, int v) {
  //((h / s >= 4)?(h / (float)s):4)
	_disp.setDrawColor(1);
	if (w < h) {
        _disp.drawVLine(x + w / 2.0, y, h);
		if (s < h && h / s >= 4) for (int i = 0;i < s + 1;i++) {
            if (i % 2) _disp.drawHLine(x + w / (float)s, y + (h / (float)s) * i, w / 2.0 + 1);
            else _disp.drawHLine(x, y + (h / (float)s) * i, w);
		}
		if(s>h) s=h;
        _disp.drawBox(x, v, w, h / (float)s);
	}
	else {
        _disp.drawHLine(x, y + h / 2.0, w);
		if (s < h && h / s >= 4) for (int i = 0;i < s + 1;i++) {
            if (i % 2) _disp.drawVLine(x + (w / (float)s) * i, y + (h / (float)s), h / 2.0 + 1);
            else _disp.drawVLine(x + (w / (float)s) * i, y, h);
		}
		if(s>w) s=w;
		_disp.drawBox(v, y, w / (float)s, w);
	}
}

/*
	@作用：绘制数值条
	@输入：i=值 a=值的最小值 b=值的最大值 x=左上顶点x轴坐标 y=左上顶点y轴坐标 w=宽度 h=高度 c=颜色
*/
void externdraw::Draw_Num_Bar(float i, float a, float b, int x, int y, int w, int h, int c) {
    char buffer[20];
    sprintf(buffer, "%.2f", i);
    uint8_t textWidth = _disp.getUTF8Width(buffer) + 3;

    _disp.setDrawColor(c);
    _disp.drawFrame(x, y, w - textWidth -2, h);
    _disp.drawBox(x + 2, y + 2, map(i, a, b, 0, w - textWidth - 6 ), h - 4);

    

    _disp.drawStr(x + w - textWidth, y - 1, buffer);
	//进行去棱角操作:增强文字视觉焦点
    _disp.setDrawColor(0);
    _disp.setDrawColor(1);
}

void externdraw::Draw_Pixel_Resize(int x, int y, int ox,int oy,int w, int h) {
    int xi = x - ox;
    int yi = y - oy;

    _disp.drawBox(ox + xi * w, oy + yi * h, w, h);
}

void externdraw::Draw_Slow_Bitmap(int x, int y, const unsigned char* bitmap, unsigned char w, unsigned char h) {
    uint8_t color = _disp.getDrawColor();
    int xi, yi, intWidth = (w + 7) / 8;
    for (yi = 0; yi < h; yi++) {
        for (xi = 0; xi < w; xi++) {
            if (pgm_read_byte(bitmap + yi * intWidth + xi / 8) & (128 >> (xi & 7))) {
                _disp.drawPixel(x + xi, y + yi);
            }else if (color != 2) {
                _disp.setDrawColor(0);
                _disp.drawPixel(x + xi, y + yi);
                _disp.setDrawColor(color);
            }
        }
    }
}

//位图缩放 代码片段改自arduboy2
void externdraw::Draw_Slow_Bitmap_Resize(int x, int y, uint8_t *bitmap, int w1,int h1,int w2,int h2) {
    uint8_t color = _disp.getDrawColor();
    // Serial.print("颜色");
    // Serial.println(color);
	float mw=(float)w2/w1;
	float mh=(float)h2/h1;
	uint8_t cmw=ceil(mw);
	uint8_t cmh=ceil(mh);
	int xi, yi, byteWidth = (w1 + 7) / 8;
	for (yi = 0; yi < h1; yi++) {
		for (xi = 0; xi < w1; xi++) {
			if (pgm_read_byte(bitmap + yi * byteWidth + xi / 8) & (1 << (7 - (xi & 7)))) {
				_disp.drawBox(x + xi*mw, y + yi*mh, cmw, cmh);
            }else if (color != 2) {
                _disp.setDrawColor(0);
                _disp.drawBox(x + xi*mw, y + yi*mh, cmw, cmh);
                _disp.setDrawColor(color);
            }
		}
	}
}

//绘制屏保-密集运算线条
void externdraw::DrawIntensiveComputingLine(void) {
    static uint8_t Line[4];
    for (uint8_t a = 0; a < 4; a++) {
        Line[a] += rand() % 2 - 1;
        if (Line[a] > 128) Line[a] -= 128;
        for (uint8_t b = 0; b < rand() % 3 + 3; b++) {
            _disp.drawHLine(0, Line[a] + rand() % 20 - 10, 128); //水平线
            _disp.drawVLine(Line[a] + rand() % 20 - 10, 0, 64); //垂直线
        }
    }
}

//FP 密集运算屏保
void externdraw::DrawIntensiveComputing(void) {
    float calculate;

    //随机线条
    DrawIntensiveComputingLine();

    calculate = sin(millis() / 4000.0);
    //模拟噪点
    for (int i = 0; i < calculate * 256 + 256; i++)  _disp.drawPixel(rand() % 128, rand() % 64);
    //波浪警告声
    //SetTone(64 + calculate * 64 + rand() % 16 - 8);
    SetTone(1500 + calculate * 500 + rand() % 64 - 32 - (((millis() / 1000) % 2 == 1) ? 440 : 0));
}

/*** 
 * @description: 在屏幕中心绘制文本
 * @param {*}
 * @return {*}
 */
void externdraw::DrawMsgBox(const char* s) {
    int w = Get_UTF8_Ascii_Pix_Len(1, s) + 2;
    int h = 12;
    int x = (SCREEN_COLUMN - w) / 2;
    int y = (SCREEN_ROW - h) / 2;

    _disp.setDrawColor(0);

    _disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 0);
    _disp.drawFrame(x - 1 , y - 3, w + 1, h + 3);
    _disp.setDrawColor(1);
    _disp.drawRBox(x , y - 2, w, h + 2, 2);
    _disp.setDrawColor(0);
    Draw_Utf(x + 1 , y - 1, s);
    _disp.setDrawColor(1);
}

/*** 
 * @description: 绘制高亮文本
 * @param {int} x
 * @param {int} y
 * @param {char} *s
 * @return {*}
 */
void externdraw::DrawHighLightText(int x, int y, const char* s) {
    int TextWidth = _disp.getUTF8Width(s);
    int TextHigh  = _disp.getMaxCharHeight();
    uint8_t color = _disp.getDrawColor();

    if (color==2) {
        _disp.drawUTF8(x + 1, y + 2, s);
        _disp.drawRBox(x, y, TextWidth + 2, TextHigh,3);
    }else{
        _disp.drawRBox(x, y, TextWidth + 2, TextHigh, 3);
        _disp.setDrawColor(!color);
        _disp.drawUTF8(x + 1, y + 2, s);
        _disp.setDrawColor(color);
    }
}

/*** 
 * @description: 绘制温度状态条
 * @param bool color 颜色
 * @return {*}
 */
void externdraw::DrawStatusBar(bool color) {
    _disp.setDrawColor(color);
    //温度条
    //框
    _disp.drawFrame(0, 53, 103, 11);
    //条
    _disp.drawBox(0, 53, map(99, 0, 500, 2, 103), 11); //<--- segnal stength

    //功率条
    _disp.drawFrame(104, 53, 23, 11);
    _disp.drawBox(104, 53, map(40, 0, 255, 0, 23), 11); // <--- TX voltage

    _disp.drawHLine(117, 51, 11);
    _disp.drawPixel(103, 52);
    _disp.drawPixel(127, 52);

    //////////////进入反色////////////////////////////////
    _disp.setDrawColor(2);

    //画指示针
    //Draw_Slow_Bitmap(map(PID_Setpoint, 0, 500, 2, 103) - 4, 54, PositioningCursor, 8, 8);

    _disp.setCursor(2,53);
    //_disp.printf("%.0f",PID_Setpoint);

    _disp.setCursor(105, 53);
    //显示输出功率 百分比
    // _disp.printf("%d%%", map(POWER, 0, 255, 0, 100));
    //显示真实功率
    _disp.printf("%.0fW", SYS_Voltage * SYS_Current);

    

    // arduboy.setCursor(105, 55); arduboy.print(map(PID_Output, 255, 0, 0, 100)); arduboy.print(F("%")); //功率百分比
    _disp.setDrawColor(color);
}

/*** 
 * @description: 短文本编辑器
 * @param {*}
 * @return {*}
 */
void externdraw::TextEditor(const char* title, char* text) {
    // Pop_Windows("双击保存 长按退出");
    // delay(1000);
    char newText[20] = { 0 };
    strcpy(newText, text);

    uint8_t charCounter = 0;      //光标指针
    char editChar = 'A';

    bool exitRenameGUI = false;
    bool editFlag = 0, lastEditFlag = 1;       //编辑器状态：0:选择要编辑的字符    1:选择ASCII

    uint8_t key = 0;              //存储按键状态

    while (!exitRenameGUI) {

        //设置编码器
        if (editFlag != lastEditFlag) {
            if (editFlag == 0) sys_Counter_Set(0, 19, 1, charCounter);
            else sys_Counter_Set(0, 255, 1, newText[charCounter]);

            lastEditFlag = editFlag;
        }

        //获取编码器输入
        switch (editFlag) {
        case 0: charCounter = sys_Counter_Get(); break;
        case 1:
            editChar = sys_Counter_Get();
            newText[charCounter] = editChar;
            break;
        }

        //////////////////////////////////////////////////////////
        Clear();

        _disp.setDrawColor(1);
        //第一行显示标题
        _disp.drawUTF8(0, 1, title);
        //第二行显示编辑文本
        _disp.drawUTF8(0, 12 + 1, newText);



        //显示当前选中的ASCII
        _disp.setDrawColor(1);
        _disp.setFont(u8g2_font_logisoso26_tf);

        _disp.setCursor(0, 34);
        _disp.printf("%c", newText[charCounter]);

        _disp.setCursor(32, 34);
        _disp.printf("0X%02X", newText[charCounter]);

        _disp.setFont(u8g2_font_wqy12_t_gb2312);

        //反色显示光标
        _disp.setDrawColor(2);
        if (editFlag) {
            //选择字符时光标闪烁
            if ((millis() / 250) % 2) _disp.drawBox(charCounter * 6, 12, 6, 12);
        }else _disp.drawBox(charCounter * 6, 12, 6, 12);
        

        //字符选择区反色高亮
        _disp.drawBox(0, 32, 32, 32);

        Display();

        //////////////////////////////////////////////////////////

        //处理按键事件
        key = sys_KeyProcess();
        switch (key) {
            //单击切换编辑器状态
        case 1: editFlag = !editFlag; break;

        case 2:
        case 3:
            //保存并退出
            strcpy(text, newText);
            //Pop_Windows("已保存");
            exitRenameGUI = true;
            break;
        }
    }
}

/*
    @函数 Pop_Windows
    @brief 自适应文本大小信息弹窗
    @param (char* s):字符串首地址
    
*/
void externdraw::Pop_Windows(const char* s) {
    // _disp.setCursor(0, 0);
    // _disp.print(s);
    // Display();
    //Set_Font_Size(2);
    int w = Get_UTF8_Ascii_Pix_Len(1,s) + 2;
    int h = 12;
    // for (int i = 5;i > 0;i--) {
    //     //Set_Font_Size(i);
    //     w = CNSize * Get_Max_Line_Len(s) * Get_Font_Size() / 2;
    //     //h = CNSize * Get_Font_Size() * Get_Str_Next_Line_O(s);
    //     if (w < SCREEN_COLUMN && h < SCREEN_ROW) break;
    // }
    int x = (SCREEN_COLUMN - w) / 2;
    int y = (SCREEN_ROW - h) / 2;

    _disp.setDrawColor(0);
    Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 66 * *Switch_space[SwitchSpace_SmoothAnimation]); //<=15FPS以便人眼察觉细节变化

    int ix = 0;
    for (int i = 1;i <= 10;i++) {
        //震荡动画
        if (*Switch_space[SwitchSpace_SmoothAnimation]) ix = (10 * cos((i * 3.14) / 2.0)) / i;

        _disp.setDrawColor(0);
        Blur(0, 0, SCREEN_COLUMN, SCREEN_ROW, 3, 0);
        _disp.drawFrame(x - 1 + ix, y - 3, w + 1, h + 3);
        _disp.setDrawColor(1);
        _disp.drawRBox(x + ix, y - 2, w, h + 2 ,2);
        _disp.setDrawColor(0);
        Draw_Utf(x + 1 + ix, y - 1, s);
        _disp.setDrawColor(1);
        Display();
        delay(20 * *Switch_space[SwitchSpace_SmoothAnimation]);
    }
    //Set_Font_Size(1);
}

/*
    @函数 Page_Footnotes
    @brief 自适应屏幕右下角角标绘制
    @param int a 分子
    @param int b 分母
*/
void externdraw::Page_Footnotes(int a, int b) {
    char buffer[20];
    uint8_t w = (Get_Dec_Deep(a) + Get_Dec_Deep(b) + 3) * 6;
    uint8_t x = SCREEN_COLUMN - 8 - w;
    uint8_t y = SCREEN_ROW - 12;

    if (millis() < pages_Tip_Display_timer + pages_Tip_Display_Timeout) {
        //绘制白色底色块
        _disp.setDrawColor(1);
        _disp.drawRBox(x + 1, y - 1, w, 13 ,1);
        //绘制下标文字
        _disp.setDrawColor(0);
        _disp.setCursor(x,y + 1);
        sprintf(buffer,"[%d/%d]", a, b);
        _disp.print(buffer);
    }
    //恢复颜色设置
    _disp.setDrawColor(1);
}

void externdraw::Draw_APP(int x, int y, uint8_t* bitmap) {
    _disp.setDrawColor(1);
    _disp.drawRBox(x - 3, y - 3, 42 + 6, 42 + 6, 4);

    Draw_Slow_Bitmap_Resize(x, y, bitmap + 1, bitmap[0], bitmap[0], 42, 42);
}

void externdraw::DetailInfo(void) {
    char buffer[50];
    for (uint8_t i = 0;i < 5;i++) {
        _disp.setCursor(0, 12 * i + 1);
        switch (i) {
            case 0: sprintf(buffer, "状态%d:%s 控温:%s", TempCTRL_Status, TempCTRL_Status_Mes[TempCTRL_Status], (PIDMode == 1) ? "PID" : "模糊"); break;
            case 1: sprintf(buffer, "设定%.0lf°C 当前%.0lf°C", PID_Setpoint, TipTemperature); break;
            case 2: sprintf(buffer, "ADC:%d PID:%.0lf", LastADC, PID_Output); break;
            case 3: sprintf(buffer, "%.2lfV %.2lfA %.2lfW", SYS_Voltage, SYS_Current,SYS_Voltage * SYS_Current); break;
            case 4: sprintf(buffer, "%.3lf %.3lf %.3lf", aggKp, aggKi, aggKd); break;
            default: break;
        }
        _disp.print(buffer);
    }
}


void externdraw::BriefInfo(void) {
    _disp.drawUTF8(0, 1, TipName); //显示烙铁头名称
    Draw_Slow_Bitmap(74, 37, C_table[TempCTRL_Status], 14, 14); //温度控制状态图标
    _disp.drawUTF8(91, 40, TempCTRL_Status_Mes[TempCTRL_Status]); //显示中文状态信息
    if (UnderVoltageEvent) { //欠压警报
        if ((millis() / 1000) % 2) {
            Draw_Slow_Bitmap(74, 21, Battery_NoPower, 14, 14); //欠压告警图标
        }else{ //主电源电压
            _disp.setCursor(74, 24);
            _disp.printf("%.1fV", SYS_Voltage);
        }
    }else{ //显示蓝牙图标
        Draw_Slow_Bitmap(92, 25, IMG_BLE_S, 9, 11);
    }

    //显示当前温度
    _disp.setFont(u8g2_font_logisoso38_tr);
    _disp.setCursor(0,12);
    if (TempCTRL_Status == TEMP_STATUS_ERROR || ERROREvent) {
        if ((millis() / 250) % 2) _disp.print("---");
    }else {
        _disp.printf("%.0lf", TipTemperature); //显示真实温度
    }

    _disp.setFont(u8g2_font_wqy12_t_gb2312);

    //右上角运行指示角标
    if (POWER > 0 && PWM_WORKY) {
        uint8_t TriangleSize = map(POWER,0,255,16,0);
        //_disp.drawTriangle(100 + TriangleSize, 0, 127, 0, 127, 27 - TriangleSize);
        _disp.drawTriangle((119 - 12) + TriangleSize, 12, 125, 12, 125, (18 +12) - TriangleSize);
        // Draw_Slow_Bitmap(114, 15, PositioningCursor, 8, 8);
        //_disp.drawTriangle(103, 0, 127, 0, 127, 24);
    }

    /////////////////////////////////////绘制遮罩层//////////////////////////////////////////////
    _disp.setDrawColor(2);
    //几何图形切割
    _disp.drawBox(0, 12, 96, 40);
    _disp.drawTriangle(96,12,96,52,125,42);
    _disp.drawTriangle(125,42,96,52,118,52);
    _disp.setDrawColor(1);

    //绘制底部状态条
    DrawStatusBar(1);
}