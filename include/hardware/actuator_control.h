#ifndef ACTUATOR_CONTROL_H
#define ACTUATOR_CONTROL_H

#include <Arduino.h>

void setPump(bool state);
void setFan(int speed); // 0-255
void setLamp(bool state);
void setBuzzer(bool state);

#endif
