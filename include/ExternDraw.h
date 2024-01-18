#ifndef ExternDraw_h
#define ExternDraw_h

#include <stdint.h>
#include "Beep.h"
#include <U8g2lib.h>

class externdraw {
public:
    externdraw(beeper &Bepper): _disp(U8G2_R0, /* reset=*/ U8X8_PIN_NONE), _Bepper(Bepper) {
        //初始化OLED
        _disp.begin();
        // Disp.setBusClock(921600);
        _disp.enableUTF8Print();
        _disp.setFontDirection(0);
        _disp.setFontPosTop();
        _disp.setFont(u8g2_font_wqy12_t_gb2312);
        _disp.setDrawColor(1);
        _disp.setFontMode(1);

        //_Bepper = Beeper; // value <- *address?
    }

    void EnterLogo(void);

    void Clear(void);
    void Display(void);

    void Draw_Utf(int x, int y, const char* s);
    void Blur(int sx, int sy, int ex, int ey, int f, int delay);
    void Draw_Scale(int x, int y, int w, int h, int s, int v);
    void Draw_Num_Bar(float i, float a, float b, int x, int y, int w, int h, int c);
    void Draw_Pixel_Resize(int x, int y, int ox, int oy, int w, int h);
    void Draw_Slow_Bitmap(int x, int y, const unsigned char* bitmap, unsigned char w, unsigned char h);
    void Draw_Slow_Bitmap_Resize(int x, int y, uint8_t* bitmap, int w1, int h1, int w2, int h2);
    void DrawStatusBar(bool color);

    void DrawIntensiveComputingLine(void);
    void DrawIntensiveComputing(void);

    void DrawMsgBox(const char* s);
    void DrawHighLightText(int x, int y, const char* s);

    void TextEditor(const char* title, char* text);

    void Pop_Windows(const char* s);
    
private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C _disp; // (U8G2_R0, /* reset=*/ U8X8_PIN_NONE)
    // U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI Disp(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
    // U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI Disp(U8G2_R0, /* cs=*/ 5, /* dc=*/ 13, /* reset=*/ 15);
    // U8G2_ST7920_128X64_F_HW_SPI Disp(U8G2_R0, 5, 15);
    // U8G2_ST7920_128X64_F_SW_SPI Disp(U8G2_R0, /*clock*/18, /*data*/23, /*cs*/5, /*reset*/15);
    beeper &_Bepper;

    uint8_t DisplayFlashTick = 0;
};

#endif