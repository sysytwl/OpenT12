#include "FilesSystem.h"
#include "main.h"



//存档路径
char SYS_SVAE_PATH[50] = "/MotionTX.sav";

void SYS_Save(void) {
    noInterrupts();
    //Pop_Windows("保存中 请勿切断电源");
    DrawMsgBox("保存中"); //软盘图标
    Draw_Slow_Bitmap_Resize(128 - 28 - 4, 64 - 28 - 4, Save + 1, Save[0], Save[0], 28, 28);
    Display();

    File file = SPIFFS.open(SYS_SVAE_PATH,FILE_WRITE);
    if (!file) {
        Log(LOG_ERROR, "存档写入失败!");
        Pop_Windows("写入失败");
        return;
    }

    //注意这个是判断版本的依据，载入不同版本的存档可能会导致致命错误
    file.write((uint8_t*)&ChipMAC, sizeof(ChipMAC));
    file.write((uint8_t*)&CompileTime, sizeof(CompileTime));
    file.write((uint8_t*)&ScreenProtectorTime, sizeof(ScreenProtectorTime));
    file.write((uint8_t*)&Use_KFP, sizeof(Use_KFP));
    file.write((uint8_t*)&PanelSettings              , sizeof(PanelSettings              ));
    file.write((uint8_t*)&ScreenFlip                 , sizeof(ScreenFlip                 ));
    file.write((uint8_t*)&SmoothAnimation_Flag       , sizeof(SmoothAnimation_Flag       ));
    file.write((uint8_t*)&ScreenBrightness           , sizeof(ScreenBrightness           ));
    //file.write((uint8_t*)&OptionStripFixedLength_Flag, sizeof(OptionStripFixedLength_Flag));
    file.write((uint8_t*)&KFP_Temp, sizeof(KFP_Temp));
    file.write((uint8_t*)&Volume, sizeof(Volume));
    file.write((uint8_t*)&RotaryDirection, sizeof(RotaryDirection));
    //file.write((uint8_t*)&HandleTrigger, sizeof(HandleTrigger));
    file.write((uint8_t*)&UndervoltageAlert, sizeof(UndervoltageAlert));
    file.write((uint8_t*)&Language, sizeof(Language));
    file.close();
    Log(LOG_OK, "存档保存成功!");
    interrupts(); //启用中断
}

void SYS_Load(void) {
    noInterrupts(); //关闭中断
    DrawMsgBox("加载中"); //软盘图标
    Draw_Slow_Bitmap(128 - 28 - 4, 64 - 28 - 4, IMG_Load, 28, 28);
    Display();

    File file = SPIFFS.open(SYS_SVAE_PATH);
    if (!file.available()) {
        Log(LOG_ERROR,"存档不存在！");
        Pop_Windows("存档不存在！");
        file.close();
        return;
    }

    //写入系统信息，载入不同版本的存档可能会导致致命错误
    uint64_t FSChipMAC;
    char     FSCompileTime[20];

    file.read((uint8_t*)&FSChipMAC, sizeof(FSChipMAC));
    file.read((uint8_t*)&FSCompileTime, sizeof(FSCompileTime));

    //判断存档版本，载入不同版本的存档可能会导致致命错误
    if (memcmp(CompileTime, FSCompileTime, sizeof(FSCompileTime))) {
        Log(LOG_ERROR, "存档版本不一致，拒绝加载存档！");
        Pop_Windows("存档版本不一致");
        return;
    }

    file.read((uint8_t*)&ScreenProtectorTime, sizeof(ScreenProtectorTime));
    file.read((uint8_t*)&Use_KFP, sizeof(Use_KFP));
    file.read((uint8_t*)&PanelSettings, sizeof(PanelSettings));
    file.read((uint8_t*)&ScreenFlip, sizeof(ScreenFlip));
    file.read((uint8_t*)&SmoothAnimation_Flag, sizeof(SmoothAnimation_Flag));
    file.read((uint8_t*)&ScreenBrightness, sizeof(ScreenBrightness));
    //file.read((uint8_t*)&OptionStripFixedLength_Flag, sizeof(OptionStripFixedLength_Flag));

    file.read((uint8_t*)&KFP_Temp, sizeof(KFP_Temp));

    file.read((uint8_t*)&Volume, sizeof(Volume));
    file.read((uint8_t*)&RotaryDirection, sizeof(RotaryDirection));
    //file.read((uint8_t*)&HandleTrigger, sizeof(HandleTrigger));
    file.read((uint8_t*)&UndervoltageAlert, sizeof(UndervoltageAlert));
    file.read((uint8_t*)&Language, sizeof(Language));
    file.close();
    Update_OLED_Flip();
    Update_OLED_Light_Level();
    Log(LOG_OK, "存档读取成功!");
    interrupts(); //启用中断
}

void FilesSystemInit(void) {
    noInterrupts(); //关闭中断

    if (!SPIFFS.begin(true)) {
        Log(LOG_OK,"文件系统初始化完成！");
    }

    interrupts(); //启用中断
    printf("[OK]文件系统\n");
    SYS_Load();
}