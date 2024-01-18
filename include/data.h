#ifndef data_h
#define data_h

#include <Arduino.h>

struct KeyData {
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
} key_data;

struct SystemStatus{
  bool Output_Lock = true; 
  bool SYS_Ready = false;
  bool UnderVoltageEvent = false;
  bool ERROREvent = false;
  bool SleepEvent = false;
  float SYS_Voltage = 0;
  uint8_t SYSKey = 0;
  char* Status_Mes[]={"错误","正常",};
} sys_status;

struct SystemInfo {
  uint64_t ChipMAC;
  char ChipMAC_S[18] = {0};
  char CompileTime[20];
} sys_info;

struct PinConfig {

} pin_config;

struct SystemConfig {
  char* TxName[13] = "SPIFFS ERROR";
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
} sys_config;

#endif