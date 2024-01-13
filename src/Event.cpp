#include "Event.h"
#include "MathFun.h"
#include <Arduino.h>



uint32_t EventTimerUpdate = 0; //事件计时器
//static uint32_t ScreenProtectorTimer = 0; //屏保事件计时器
extern bool SleepEvent;
extern uint8_t SleepTime;

void TimerUpdateEvent(void) {
    EventTimerUpdate = millis();
}

/*** 
 * @description: 计时器事件Loop
 * @param {*}
 * @return {*}
 */
void TimerEventLoop(void) {
    uint32_t TimerEventTimer = millis() - EventTimerUpdate; //事件事件距离计时器
    
    //更新休眠和停机时间    停机优先级 > 休眠优先级
    if (SleepTime != 0 && TimerEventTimer > Minute2Millis(SleepTime)) {
        SleepEvent = true;
    }else{
        if (SleepEvent) {
            SleepEvent = false;
        }
        //ScreenProtectorTimer = millis();
    }
}

/*** 
 * @description: 更新系统状态码
 * @param {*}
 * @return {*}
 */
void SYS_StateCode_Update(void) {
    SleepEvent
    ERROREvent
    
    Get_MainPowerVoltage(); //读取系统输入电压

    //欠压警告
    if (UndervoltageAlert!=0 && SYS_Voltage < UndervoltageAlert) UnderVoltageEvent = true;
    else UnderVoltageEvent = false;

}
