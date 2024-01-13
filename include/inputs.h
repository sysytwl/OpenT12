#ifndef inputs_h
#define inputs_h

void InputsInit();
int mapAndAdjustJoystickDeadBandValues(int value, bool reverse);
void KeyTick();

void sys_Counter_click(void);
void sys_Counter_doubleclick(void);
void sys_Counter_longclick(void);

void Get_MainPowerVoltage(void);

#endif
