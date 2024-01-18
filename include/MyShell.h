#ifndef myshell_h
#define myshell_h

#include <Shell.h>



void shellInit(void) {
    shell_init(shell_reader, shell_writer, 0);
    shell_register((shell_program_t)command_test, PSTR("test"));

    // shell_register((shell_program_t)RotaryUp, PSTR("RotaryUp"));
    // shell_register((shell_program_t)RotaryDown, PSTR("RotaryDown"));
    // shell_register((shell_program_t)sys_Counter_click, PSTR("sys_Counter_click"));
    // shell_register((shell_program_t)sys_Counter_doubleclick, PSTR("sys_Counter_doubleclick"));
    // shell_register((shell_program_t)sys_Counter_longclick, PSTR("sys_Counter_longclick"));
}

void ICACHE_RAM_ATTR ShellLoop(void) {
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

int command_test(int argc, char** argv){
    shell_println("-----------------------------------------------");
    shell_println("SHELL DEBUG / TEST UTILITY");
    shell_println("-----------------------------------------------");
    shell_println("");
    shell_printf("Received %d arguments for test command\r\n", argc);

    // ChipMAC = ESP.getEfuseMac();
    // sprintf(CompileTime, "%s %s", __DATE__, __TIME__);
    // for (uint8_t i = 0;i < 6;i++) {
    //     sprintf(ChipMAC_S + i * 3, "%02X%s", ((uint8_t*)&ChipMAC)[i], (i != 5) ? ":" : "");
    // }

    // Print each argument with string lenghts
    for (int i = 0; i < argc; i++){
        // Print formatted text to terminal
        shell_printf("%d - \"%s\" [len:%d]\r\n", i, argv[i], strlen(argv[i]));
    }

    return SHELL_RET_SUCCESS;
}

#endif
