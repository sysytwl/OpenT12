#include <math.h>

#include <Ticker.h>
//#include "MyShell.h"
#include "inputs.h"
#include "ExternDraw.h"
#include "Event.h"
#include "Menu.h"
#include "Calibration.h"
#include "FilesSystem.h"
#include "qrcode.h"
#include "Bitmap.h"
#include "Beep.h"
#include "espnow.h"
//#include "data.h"



#define setbit(x,y)       x|=(1<<y)
#define clrbit(x,y)       x&=~(1<<y)
#define reversebit(x,y)   x^=(1<<y)
#define getbit(x,y)       ((x) >> (y)&1)

//注意Pin36与Pin39连接了内部霍尔传感器,不建议复用高敏感用途
//建议给ADC输入串联一个0.1uF的电容，有利于减少噪声
#define beep_pin 25
#define beep_chennel 2

// Joysticks
#define x_axis_pin 34
#define y_axis_pin 35
#define button_pin 0
// External Joysticks
#define left_x_axis_pin -1
#define left_y_axis_pin -1
#define left_button_pin -1
#define right_x_axis_pin -1
#define right_y_axis_pin -1
#define right_button_pin -1

// Main Power Voltage
#define POWER_ADC_VCC_R1 10
#define POWER_ADC_R2_GND 1
#define POWER_ADC_PIN 36



void About(void);

beeper Beeper(beep_pin, beep_chennel); //蜂鸣器
inputs Inputs(button_pin, x_axis_pin, y_axis_pin); // keys and joysticks
externdraw ExternDraw(Beeper);
filesystem FileSystem(ExternDraw);
events Events(POWER_ADC_PIN, POWER_ADC_VCC_R1, POWER_ADC_R2_GND);
