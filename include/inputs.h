#ifndef inputs_h
#define inputs_h

#include <OneButton.h>
#include "MathFun.h"
#include "data.h"



class inputs {
public:
    inputs(uint8_t button_pin, uint8_t x_axis_pin, uint8_t y_axis_pin): RButton(button_pin, true, true){
        pinMode(x_axis_pin, INPUT);
        pinMode(y_axis_pin, INPUT);

        RButton.attachClick(sys_Counter_click);
        RButton.attachDoubleClick(sys_Counter_doubleclick);
        RButton.attachLongPressStart(sys_Counter_longclick);
        RButton.setDebounceTicks(25);
        RButton.setClickTicks(30);
        RButton.setPressTicks(300);

        _x_axis_pin = x_axis_pin;
        _y_axis_pin = y_axis_pin;
    };

    void sys_Counter_click(void){
        Write_RButton_FIFO(1);
    }

    void sys_Counter_longclick(void){
        Write_RButton_FIFO(2);
    }

    void sys_Counter_doubleclick(void){
        Write_RButton_FIFO(3);
    }

    void KeyUpdate(KeyData &data) { // take configs as well, used for external joysticks and 
        // Digital
        RButton.tick();
        data.key = Read_RButton_FIFO();

        // Analogue
        //data.lxAxisValue    = MapAdjJoystick(analogRead(32), false);
        //data.lyAxisValue    = MapAdjJoystick(analogRead(33), false);
        data.rxAxisValue    = MapAdjJoystick(GetADC(_x_axis_pin), false);
        data.ryAxisValue    = MapAdjJoystick(GetADC(_y_axis_pin), false);

        // Vitural key
        data.key1 = VituralKey(data.rxAxisValue);
    }

private:
    bool _use_KFP;

    bool CounterChanged = false;
    uint8_t _x_axis_pin, _y_axis_pin;

    KFP KFP_Temp = {0.02,0,0,0,0.01,4.5};
    OneButton RButton;



    uint16_t GetADC(uint8_t ADC_PIN) {
        uint16_t ADC_RAW = analogRead(ADC_PIN);
        uint16_t ADC;

        //卡尔曼滤波器
        if (_use_KFP) ADC = kalmanFilter(&KFP_Temp, (float)ADC_RAW);
        else ADC = ADC_RAW;

    return ADC;
    }

    /*** 
     * @description: Convert ADC signal into keys
     * @param uint16_t ADC
     * @return 0 middle key, 1 upper key, 2 lower key
     */
    uint8_t VituralKey(uint16_t ADC) {
        if(ADC > 3072 && !CounterChanged){
            CounterChanged = true;
            return 1;
        } else if (ADC < 1024 && !CounterChanged) {
            CounterChanged = true;
            return 2;
        } else if (ADC > 1024 && ADC < 3072  ) {
            CounterChanged = false;
        };
        return 0;
    }

    int MapAdjJoystick(int value, bool reverse) {
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

};

#endif
