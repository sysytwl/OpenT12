#include "main.h"
#include <Arduino.h>



void setup() {
    Serial.begin(115200);
    //noInterrupts();

    Beeper.SetSound(BootSound);
}

void loop() {
    Inputs.KeyUpdate(key_data);

    Beeper.SoundUpdate();
    Events.StateUpdate(sys_status, sys_config);
    //ShellLoop();
    System_UI(); //刷新UI
}

//ESP.restart();
