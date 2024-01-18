#ifndef filesystem_h
#define filesystem_h

#include "FS.h"
#include "SPIFFS.h"
#include "data.h"
#include "ExternDraw.h"
#include "log.h"

class filesystem {
public:
    filesystem(externdraw &ExternDraw): _ExternDraw(ExternDraw) {
        noInterrupts();
        if (!SPIFFS.begin(true)) {
            Log(LOG_OK,"文件系统初始化完成！");
            //printf("[OK]文件系统\n");
        }
        interrupts();

        //_ExternDraw = ExternDraw;
    }

    void save(void) {
        noInterrupts();
        //_ExternDraw.Pop_Windows("保存中 请勿切断电源");
        _ExternDraw.DrawMsgBox("保存中 请勿切断电源");
        _ExternDraw.Draw_Slow_Bitmap_Resize(128 - 28 - 4, 64 - 28 - 4, Save + 1, Save[0], Save[0], 28, 28); //软盘图标
        _ExternDraw.Display();

        file = SPIFFS.open(SYS_SAVE_PATH, FILE_WRITE);
        if (!file) {
            Log(LOG_ERROR, "写入失败!");
            _ExternDraw.Pop_Windows("写入失败");
            return;
        }

        file.write((uint8_t*) &sys_info, sizeof(sys_info));
        file.write((uint8_t*) &pin_config, sizeof(pin_config));
        file.write((uint8_t*) &sys_config, sizeof(sys_config));
        file.close();
        Log(LOG_OK, "保存成功!");
        interrupts();
    }

    void load(void) {
        noInterrupts();
        _ExternDraw.DrawMsgBox("加载中");
        _ExternDraw.Draw_Slow_Bitmap(128 - 28 - 4, 64 - 28 - 4, IMG_Load, 28, 28); //软盘图标
        _ExternDraw.Display();

        File file = SPIFFS.open(SYS_SAVE_PATH);
        if (!file.available()) {
            Log(LOG_ERROR,"存档不存在！");
            _ExternDraw.Pop_Windows("存档不存在！");
            file.close();
            return;
        }

        //写入系统信息，载入不同版本的存档可能会导致致命错误
        SystemInfo FS_sys_info;
        file.read((uint8_t*) &FS_sys_info, sizeof(FS_sys_info));
        if (memcmp(&sys_info, &FS_sys_info, sizeof(FS_sys_info))) {
            Log(LOG_ERROR, "存档版本不一致");
            _ExternDraw.Pop_Windows("存档版本不一致");
            return;
        }

        file.read((uint8_t*) &pin_config, sizeof(pin_config));
        file.read((uint8_t*) &sys_config, sizeof(sys_config));
        file.close();
        Log(LOG_OK, "存档读取成功!");

        _ExternDraw.UpdateOLEDLightLevel();

        interrupts();
    }

private:
    char SYS_SAVE_PATH[15] = "/MotionTX.sav"; //存档路径
    File file;
    externdraw &_ExternDraw;
};

#endif
