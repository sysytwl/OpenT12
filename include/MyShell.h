void shellInit(void);
void ShellLoop(void);
int shell_reader(char* data);
void shell_writer(char data);
void shell_SendDatas(uint8_t buf[], uint32_t size); //发送协议包数据
int command_test(int argc, char** argv);

void OLED_ScreenshotInit(void);
void OLED_ScreenshotPrint(void);
