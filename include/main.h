#include <math.h>
#include <U8g2lib.h>
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



#define setbit(x,y)       x|=(1<<y)
#define clrbit(x,y)       x&=~(1<<y)
#define reversebit(x,y)   x^=(1<<y)
#define getbit(x,y)       ((x) >> (y)&1)

//注意Pin36与Pin39连接了内部霍尔传感器,不建议复用高敏感用途
//建议给ADC输入串联一个0.1uF的电容，有利于减少噪声
#define LED_Pin 2
uint8_t BEEP_PIN = 25;
uint8_t BEEP_Channel = 2;

uint8_t x_axis = 34;
uint8_t y_axis = 35;
uint8_t BUTTON_PIN = 0;

//计算主电源电压
uint8_t POWER_ADC_VCC_R1 = 10;
uint8_t POWER_ADC_R2_GND = 1;
uint8_t POWER_ADC_PIN = 36;

#define CNSize 12
#define SCREEN_COLUMN 128
#define SCREEN_ROW    64
#define SCREEN_PAGE_NUM 8
#define SCREEN_FONT_ROW 4



enum MESSAGETYPE {
    LOG_INFO = 0,
    LOG_OK,
    LOG_FAILED,
    LOG_WARNING,
    LOG_ERROR,
};
void Log(MESSAGETYPE type, const char* s);
void SYS_Reboot(void);
void About(void);

struct PacketData {
  int16_t lxAxisValue;
  int16_t lyAxisValue;
  int16_t rxAxisValue;
  int16_t ryAxisValue;
 
  int16_t channel1;
  int16_t channel2;
  int16_t channel3;
  int16_t channel4;  
  int16_t channel5;
  int16_t channel6;
  int16_t channel7;
  int16_t channel8;  
} data;

uint64_t ChipMAC;
char ChipMAC_S[18] = {0};

char CompileTime[20];
U8G2_SSD1306_128X64_NONAME_F_HW_I2C Disp(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
// U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI Disp(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
// U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI Disp(U8G2_R0, /* cs=*/ 5, /* dc=*/ 13, /* reset=*/ 15);
// U8G2_ST7920_128X64_F_HW_SPI Disp(U8G2_R0, 5, 15);
// U8G2_ST7920_128X64_F_SW_SPI Disp(U8G2_R0, /*clock*/18, /*data*/23, /*cs*/5, /*reset*/15);

bool Output_Lock = true; 
bool SYS_Ready = false;
bool UnderVoltageEvent = false;
bool ERROREvent = false;
bool SleepEvent = false;
float SYS_Voltage = 0;
uint8_t SYSKey = 0;

uint8_t* C_table[] = { c1, c2, c3, Lightning, c5, c6, c7 };
char* Status_Mes[]={"错误","正常",};


char* TxName = "SPIFFS ERROR";
char RxMAC[18] = {0};
float ScreenProtectorTime = 60;
uint8_t Use_KFP = true;
uint8_t PanelSettings = PANELSET_Simple;
uint8_t ScreenFlip = false;
uint8_t SmoothAnimation_Flag = true;
float ScreenBrightness = 128;
uint8_t Volume = true;
uint8_t RotaryDirection = false;
uint8_t SleepTime = 1;
float UndervoltageAlert = 7.0;
uint8_t Language = LANG_Chinese;
enum SYSLANG {
    LANG_English = 0,
    LANG_Chinese,
    LANG_Russian,
};

enum PANELSET {
    PANELSET_Simple = 0,
    PANELSET_Detailed,
};
