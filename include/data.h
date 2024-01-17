#ifndef data_h
#define data_h

#include <Arduino.h>

enum MESSAGETYPE {
    LOG_INFO = 0,
    LOG_OK,
    LOG_FAILED,
    LOG_WARNING,
    LOG_ERROR,
};

struct PacketData {
  uint16_t lxAxisValue;
  uint16_t lyAxisValue;
  uint16_t rxAxisValue;
  uint16_t ryAxisValue;
 
  uint16_t channel1;
  uint16_t channel2;
  uint16_t channel3;
  uint16_t channel4;
  uint16_t channel5;
  uint16_t channel6;
  uint16_t channel7;
  uint16_t channel8;

  uint8_t key;
  uint8_t key1;
} data;

struct SystemStatus{
  bool Output_Lock = true; 
  bool SYS_Ready = false;
  bool UnderVoltageEvent = false;
  bool ERROREvent = false;
  bool SleepEvent = false;
  float SYS_Voltage = 0;
  uint8_t SYSKey = 0;
  uint8_t* C_table[] = { c1, c2, c3, Lightning, c5, c6, c7 };
  char* Status_Mes[]={"错误","正常",};
} sys_status;

uint64_t ChipMAC;
char ChipMAC_S[18] = {0};
char CompileTime[20];



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

#endif