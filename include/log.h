#ifndef log_h
#define log_h

#include <Arduino.h>

enum MESSAGETYPE {
    LOG_INFO = 0,
    LOG_OK,
    LOG_FAILED,
    LOG_WARNING,
    LOG_ERROR,
};

void Log(MESSAGETYPE type, const char* s) {
    switch (type) {
        case LOG_INFO: printf("[INFO]"); break;
        case LOG_OK: printf("[OK]"); break;
        case LOG_FAILED: printf("[FAILED]"); break;
        case LOG_WARNING: printf("[WARNING]"); break;
        case LOG_ERROR: printf("[ERROR]"); break;
    }
    printf("%s\n",s);
}

#endif
