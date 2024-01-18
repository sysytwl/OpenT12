#ifndef event_h
#define event_h

#include <stdint.h>
#include "MathFun.h"

class events {
public:
    events(uint8_t POWER_ADC_PIN, uint8_t POWER_ADC_VCC_R1, uint8_t POWER_ADC_R2_GND){
        pinMode(POWER_ADC_PIN, INPUT);

        _POWER_ADC_PIN = POWER_ADC_PIN;
        _POWER_ADC_VCC_R1 = POWER_ADC_VCC_R1;
        _POWER_ADC_R2_GND = POWER_ADC_R2_GND;
    }

    void TimerUpdateEvent(void) {
        EventTimerUpdate = millis();
    }

    void StateUpdate(SystemStatus &SysStatus, SystemConfig &SysConfig) {
        EventTimer = millis() - EventTimerUpdate; //事件事件距离计时器
        
        //休眠
        if (SysConfig.SleepTime != 0 && EventTimer > Minute2Millis(SysConfig.SleepTime)) SysStatus.SleepEvent = true;

        SysStatus.voltage = GetMainPowerVoltage();

        //欠压警告
        if (SysConfig.UndervoltageAlert!=0 && SysStatus.voltage < SysConfig.UndervoltageAlert) SysStatus.UnderVoltageEvent = true;
        else SysStatus.UnderVoltageEvent = false;
    }

private:
    uint8_t _POWER_ADC_PIN, _POWER_ADC_VCC_R1, _POWER_ADC_R2_GND;
    uint32_t EventTimer, EventTimerUpdate = 0; //事件计时器

    float GetMainPowerVoltage(void) {
        //uint16_t POWER_ADC = analogRead(POWER_ADC_PIN);
        float TipADC_V_R2 = analogReadMilliVolts(_POWER_ADC_PIN) / 1000.0f;
        float TipADC_V_R1 = (TipADC_V_R2 * _POWER_ADC_VCC_R1) / _POWER_ADC_R2_GND;
        return TipADC_V_R1 + TipADC_V_R2;
    }

};

#endif
