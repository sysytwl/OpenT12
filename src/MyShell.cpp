#include "MyShell.h"
#include <Shell.h>



uint8_t EasyCursor[10][2]={
    {74,37}, //温度状态图标
    {91,40}, //状态秒速文字
};

void SetEasyCursor(int argc, char** argv) {
    uint8_t id = atof(argv[1]);
    if (id >=10) return;
    EasyCursor[id][0] = atof(argv[2]);
    EasyCursor[id][1] = atof(argv[3]);
}

//截图工具
uint8_t OLED_ScreenshotFlag = 0;
void OLED_ScreenshotInit(void) {
    OLED_ScreenshotFlag = 1;
}

void OLED_ScreenshotPrint(void) {
    if (!OLED_ScreenshotFlag) return;
    Serial.print("OLED_ScreenshotInit\r\n"); //固定请求头
    //SerialBT.print("OLED_ScreenshotInit\r\n"); //固定请求头
    uint8_t* p = (uint8_t*)Disp.getBufferPtr();
    for (uint32_t i = 0;i < SCREEN_PAGE_NUM * SCREEN_COLUMN;i++) {
        Serial.write(*p++);
        //SerialBT.write(*p++);
    }
    OLED_ScreenshotFlag = 0;
}

void shellInit(void) {
    shell_init(shell_reader, shell_writer, 0);
    shell_register((shell_program_t)command_test, PSTR("test"));
    shell_register((shell_program_t)SetEasyCursor, PSTR("SetEasyCursor"));
    shell_register((shell_program_t)OLED_ScreenshotInit, PSTR("OLED_ScreenshotInit"));

    //模拟控制输入
    shell_register((shell_program_t)RotaryUp, PSTR("RotaryUp"));
    shell_register((shell_program_t)RotaryDown, PSTR("RotaryDown"));
    shell_register((shell_program_t)sys_Counter_click, PSTR("sys_Counter_click"));
    shell_register((shell_program_t)sys_Counter_doubleclick, PSTR("sys_Counter_doubleclick"));
    shell_register((shell_program_t)sys_Counter_longclick, PSTR("sys_Counter_longclick"));
}

void ICACHE_RAM_ATTR ShellLoop(void) {
    //命令解析器
    while (Serial.available()) shell_task();
}

/**
 * Function to read data from serial port
 * Functions to read from physical media should use this prototype:
 * int my_reader_function(char * data)
 */
int shell_reader(char* data){
    if (Serial.available()) {
        *data = Serial.read();
        return 1;
    }
    return 0;
}

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype:
 * void my_writer_function(char data)
 */
void shell_writer(char data) {
    Serial.write(data);
}

void shell_SendDatas(uint8_t buf[],uint32_t size) {
    /*
        包头：0xA5
        原数据：buf
        校验和：原数据所有字节和的低八位
        包尾：0x5A
    */
   uint8_t CHECK = 0;
   //计算校验和
   for (uint32_t i=0;i<size;i++)
    CHECK += buf[i];

   Serial.write(0xA5);
   Serial.write(buf, size);
   Serial.write(CHECK);
   Serial.write(0x5A);
}

int command_test(int argc, char** argv){
    shell_println("-----------------------------------------------");
    shell_println("SHELL DEBUG / TEST UTILITY");
    shell_println("-----------------------------------------------");
    shell_println("");
    shell_printf("Received %d arguments for test command\r\n", argc);

    // Print each argument with string lenghts
    for (int i = 0; i < argc; i++){
        // Print formatted text to terminal
        shell_printf("%d - \"%s\" [len:%d]\r\n", i, argv[i], strlen(argv[i]));
    }

    return SHELL_RET_SUCCESS;
}

