#ifndef beep_h
#define beep_h

#include <Arduino.h>



enum CALCULATORMUSICTONE {
    CMT_NULL = 255,
    CMT_1 = 0,
    CMT_2 = 2,
    CMT_3 = 3,
    CMT_4 = 5,
    CMT_5 = 7,
    CMT_6 = 9,
    CMT_7 = 11,
    CMT_8 = 12,
    CMT_9 = 14,
    CMT_P = 15,
    CMT_S = 17,
    CMT_M = 18,
    CMT_D = 21,
    CMT_E = 22,
};

struct TONE{
    note_t note;
    uint8_t rp;
    uint16_t delay;
};

TONE testSound[] {
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_8,250},
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_NULL,250},

    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_4,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_NULL,250},
    
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_5,250},
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_M,250},
    {NOTE_D,CMT_NULL,250},

    {NOTE_MAX,255,0},
};

TONE BootSound[]{
    {NOTE_D,CMT_5,230},
    {NOTE_D,CMT_7,230},
    {NOTE_D,CMT_9,215},
    {NOTE_D,CMT_M,215},
    {NOTE_MAX,0,0},
};

TONE RxConnected[]{
    {NOTE_D,CMT_7,250},
    {NOTE_D,CMT_M,250},
    {NOTE_MAX,0,0},
};

TONE RxLostConnection[]{
    {NOTE_D,CMT_9,250},
    {NOTE_D,CMT_5,250},
    {NOTE_MAX,0,0},
};

TONE Beep1[]{
    {NOTE_D,CMT_8,50},
    {NOTE_MAX,0,0},
};

TONE Beep2[]{
    {NOTE_D,CMT_M,50},
    {NOTE_D,CMT_NULL,50},
    {NOTE_D,CMT_M,50},
    {NOTE_MAX,0,0},
};

TONE Beep3[]{
    {NOTE_D,CMT_7,50},
    {NOTE_D,CMT_9,50},
    {NOTE_D,CMT_M,50},
    {NOTE_MAX,0,0},
};



void BeepInit();
void SetSound(TONE sound[]);
void PlaySoundLoop(void);

#endif
