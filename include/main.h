#include <math.h>

#include <Ticker.h>
#include "MyShell.h"
#include "inputs.h"
#include "ExternDraw.h"
#include "Event.h"
#include "Menu.h"
#include "Calibration.h"
#include "Bezier.h"
#include "FilesSystem.h"
#include "qrcode.h"
#include "Bitmap.h"
#include "Beep.h"
#include "espnow.h"
#include "data.h"



#define setbit(x,y)       x|=(1<<y)
#define clrbit(x,y)       x&=~(1<<y)
#define reversebit(x,y)   x^=(1<<y)
#define getbit(x,y)       ((x) >> (y)&1)

//注意Pin36与Pin39连接了内部霍尔传感器,不建议复用高敏感用途
//建议给ADC输入串联一个0.1uF的电容，有利于减少噪声
#define BEEP_PIN 25
#define BEEP_Channel 2

// Joysticks
#define x_axis_pin 34
#define y_axis_pin 35
#define BUTTON_PIN 0

// Main Power Voltage
#define POWER_ADC_VCC_R1 10
#define POWER_ADC_R2_GND 1
#define POWER_ADC_PIN 36

#define CNSize 12
#define SCREEN_COLUMN 128
#define SCREEN_ROW    64
#define SCREEN_PAGE_NUM 8
#define SCREEN_FONT_ROW 4



void Log(MESSAGETYPE type, const char* s);
void About(void);

beeper Beeper(BEEP_PIN, BEEP_Channel); //蜂鸣器
inputs Inputs(BUTTON_PIN, x_axis_pin, y_axis_pin); // keys and joysticks
externdraw ExternDraw(Beeper);
filesystem FileSystem(ExternDraw);

void Log(MESSAGETYPE type, const char* s) {
    switch (type) {
        case LOG_INFO: printf("[INFO]"); break;
        case LOG_OK: printf("[OK]"); break;
        case LOG_FAILED: printf("[FAILED]"); break;
        case LOG_WARNING: printf("[WARNING]"); break;
        case LOG_ERROR: printf("[ERROR]"); break;
    }
    printf("%s\n",s);
    //Pop_Windows(s);
}