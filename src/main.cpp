#include "main.h"
#include <Arduino.h>

uint8_t POWER_ADC_PIN, uint8_t POWER_ADC_VCC_R1, uint8_t POWER_ADC_R2_GND

void setup() {
    Serial.begin(115200);
    //noInterrupts();

    ////////////////////////////初始化硬件/////////////////////////////
    ChipMAC = ESP.getEfuseMac();
    sprintf(CompileTime, "%s %s", __DATE__, __TIME__);
    for (uint8_t i = 0;i < 6;i++) {
        sprintf(ChipMAC_S + i * 3, "%02X%s", ((uint8_t*)&ChipMAC)[i], (i != 5) ? ":" : "");
    }

    ////////////////////////////初始化软件/////////////////////////////
    FilesSystemInit(); //启动文件系统，并读取存档
    shellInit(); //初始化命令解析器
    //BLE_Init(); //初始化蓝牙（可选）
    SetSound(BootSound); //播放音效
    System_UI_Init(); //初始化UI
}

void loop() {
    Inputs.KeyTick(data);
    TimerEventLoop(); //更新系统事件
    PlaySoundLoop();
    SYS_StateCode_Update(); //更新状态码
    ShellLoop(void);
    System_UI(); //刷新UI
}

//ESP.restart();
void About(void) {
    //播放Logo动画
    //EnterLogo();
    //生成项目QRCode
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(3)];

    switch (Language) {
        case LANG_Chinese:
            qrcode_initText(&qrcode, qrcodeData, 3, 0, "https://github.com/createskyblue/OpenT12");
        break;

        default:
            qrcode_initText(&qrcode, qrcodeData, 3, 0, "https://github.com/createskyblue/OpenT12");
        break;
    }

    Clear();

    uint8_t x_offset = (SCREEN_COLUMN - qrcode.size*2)/2;
    uint8_t y_offset = (SCREEN_ROW - qrcode.size*2)/2;
    
    for (uint8_t y=0;y<qrcode.size;y++)
        for (uint8_t x=0;x<qrcode.size;x++)
            if (qrcode_getModule(&qrcode, x, y)) Draw_Pixel_Resize(x + x_offset,y + y_offset,x_offset,y_offset,2,2);

    Disp.setDrawColor(2);
    Disp.drawBox(x_offset - 2 ,y_offset - 2 ,qrcode.size * 2 + 4,qrcode.size * 2 + 4);
    Disp.setDrawColor(1);

    while(!sys_KeyProcess()) {
        Display();
    }
    //显示系统信息
    Clear();
    char buffer[50];
    for (uint8_t i = 0;i < 5;i++) {
        Disp.setCursor(0, 12 * i + 1);

        switch (i) {
            case 0: sprintf(buffer, "%s Ver-%d",ESP.getChipModel(),(int)ESP.getChipRevision()); break;
            case 1: sprintf(buffer, "%d@%uMHz %uMb U%.1f%%", (int)ESP.getChipCores(),ESP.getCpuFreqMHz(),ESP.getFlashChipSize()/1024/1024,(float)ESP.getSketchSize()/ESP.getFlashChipSize() * 100.0); break;
            case 2: sprintf(buffer, "%s %s", __DATE__,__TIME__); break;
            case 3: sprintf(buffer, "%s", ESP.getSdkVersion()); break;
            case 4: sprintf(buffer, "MAC %s", ChipMAC_S); break;
        }
        Disp.print(buffer);
    }
    while(!sys_KeyProcess()) {
        Display();
    }

}