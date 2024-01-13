#include "inputs.h"
#include <OneButton.h>
#include <Arduino.h>
#include "MathFun.h"



OneButton RButton(BUTTON_PIN, true);
extern uint8_t BUTTON_PIN;
KFP KFP_Temp = {0.02,0,0,0,0.01,4.5};
extern uint8_t Use_KFP;
extern uint8_t x_axis;
extern uint8_t y_axis;

extern uint8_t POWER_ADC_PIN;
extern uint8_t POWER_ADC_VCC_R1;
extern uint8_t POWER_ADC_R2_GND;
extern float SYS_Voltage;

extern uint8_t SYSKey;
extern struct PacketData {
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

void InputsInit() {
    pinMode(x_axis, INPUT); // joystick
    pinMode(y_axis, INPUT); // joystick

    pinMode(POWER_ADC_PIN, INPUT);

    RButton.attachClick(sys_Counter_click);
    RButton.attachDoubleClick(sys_Counter_doubleclick);
    RButton.attachLongPressStart(sys_Counter_longclick);
    RButton.setDebounceTicks(25);
    RButton.setClickTicks(30);
    RButton.setPressTicks(300);
}

uint16_t GetADC(uint8_t ADC_PIN) {
    uint16_t ADC_RAW = analogRead(ADC_PIN);
    uint16_t ADC;

    //卡尔曼滤波器
    if (Use_KFP) ADC = kalmanFilter(&KFP_Temp, (float)ADC_RAW);
    else ADC = ADC_RAW;

    return ADC;
}

bool VituralKey(uint16_t ADC) {
    static bool CounterChanged = false;
    if(ADC > 3500 && !CounterChanged){
        //RotaryDown();
        CounterChanged = true;
    } else if (ADC < 1000 && !CounterChanged) {
        //RotaryUp();
        CounterChanged = true;
    } else if (ADC < 3500 && ADC > 1000) {
        CounterChanged = false;
    };
}

int mapAndAdjustJoystickDeadBandValues(int value, bool reverse) {
  if (value >= 2200) {
    value = map(value, 2200, 4095, 127, 254);
  } else if (value <= 1800) {
    value = (value == 0 ? 0 : map(value, 1800, 0, 127, 0));  
  } else {
    value = 127;
  }

  if (reverse) {
    value = 254 - value;
  }
  Serial.println(value);  
  return value;
}

void KeyTick(void) {
  // Digital
  RButton.tick();
  SYSKey = Read_RButton_FIFO();

  // Analogue
//   data.lxAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(32), false);
//   data.lyAxisValue    = mapAndAdjustJoystickDeadBandValues(analogRead(33), false);
  data.rxAxisValue    = mapAndAdjustJoystickDeadBandValues(GetADC(x_axis), false);
  data.ryAxisValue    = mapAndAdjustJoystickDeadBandValues(GetADC(y_axis), false);
}

//按键FIFO循环大小
#define RButton_FIFO_Size 10
//按键FIFO读写指针
static uint8_t RButton_FIFO_pwrite = 0;
static uint8_t RButton_FIFO_pread = 0;
//按键FIFO缓存区
static uint8_t RButton_FIFO[RButton_FIFO_Size];
//按键FIFO缓存区有效数据大小
static uint8_t RButton_FIFO_BufferSize = 0;

void Clear_RButton_FIFO(void){
    RButton_FIFO_pread = RButton_FIFO_pwrite;
    memset(RButton_FIFO, 0, RButton_FIFO_Size);
}

/*** 
 * @description: 写按键FIFO
 * @param uint8_t State 要写入FIFO的按键状态值
 * @return 无
 */
static void Write_RButton_FIFO(uint8_t State) {
    //重置事件计时器
    TimerUpdateEvent();
    
    RButton_FIFO[RButton_FIFO_pwrite] = State;
    printf("FIFO写入[%d]=%d\n", RButton_FIFO_pwrite, State);
    //写指针移位
    RButton_FIFO_pwrite++;
    //按键缓冲区数据大小+1
    if (RButton_FIFO_BufferSize < RButton_FIFO_Size) RButton_FIFO_BufferSize++;
    //循环写
    if (RButton_FIFO_pwrite >= RButton_FIFO_Size) RButton_FIFO_pwrite = 0;
    printf("FIFO缓存区大小:%d\n\n", RButton_FIFO_BufferSize);
}

/*** 
 * @description: 读按键FIFO
 * @param {*}
 * @return 按键State
 */
//读按键FIFO
static uint8_t Read_RButton_FIFO(void) {
    //判断当前按键缓冲区是否有数据
    if (RButton_FIFO_BufferSize == 0) return 0;

    //从按键FIFO缓存读取数据
    uint8_t res = RButton_FIFO[RButton_FIFO_pread];
    printf("FIFO读取[%d]=%d\n", RButton_FIFO_pread,res);
    //读指针移位
    RButton_FIFO_pread++;
    RButton_FIFO_BufferSize--;
    //循环写
    if (RButton_FIFO_pread >= RButton_FIFO_Size) RButton_FIFO_pread = 0;
    printf("FIFO缓存区大小:%d\n\n", RButton_FIFO_BufferSize);
    return res;
}

void sys_Counter_click(void){
    printf("触发单击事件\n");
    Write_RButton_FIFO(1);
}

void sys_Counter_longclick(void){
    printf("触发长按事件\n");
    Write_RButton_FIFO(2);
}

void sys_Counter_doubleclick(void){
    printf("触发双击事件\n");
    Write_RButton_FIFO(3);
}

void Get_MainPowerVoltage(void) {
    static uint32_t CoolTimer = 0;
    if (millis() - CoolTimer > 100) {
        //uint16_t POWER_ADC = analogRead(POWER_ADC_PIN);
        double TipADC_V_R2 = analogReadMilliVolts(POWER_ADC_PIN) / 1000.0;
        //double   TipADC_V_R2 = ESP32_ADC2Vol(POWER_ADC);
        double   TipADC_V_R1 = (TipADC_V_R2 * POWER_ADC_VCC_R1) / POWER_ADC_R2_GND;
        SYS_Voltage = TipADC_V_R1 + TipADC_V_R2;
        //重置冷却计时器
        CoolTimer = millis();
    }
    // printf("电压:%lf V\n", SYS_Voltage);
}
