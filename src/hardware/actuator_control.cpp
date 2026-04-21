#include "hardware/actuator_control.h"

void setPump(bool state) {
    if (state) {
        Serial.println("[ACTUATOR] Pump: ON");
    } else {
        Serial.println("[ACTUATOR] Pump: OFF");
    }
}

void setFan(int speed) {
    if (speed > 0) {
        Serial.printf("[ACTUATOR] Fan Speed: %d\n", speed);
    } else {
        Serial.println("[ACTUATOR] Fan: OFF");
    }
}

void setLamp(bool state) {
    if (state) {
        Serial.println("[ACTUATOR] Lamp: ON");
    } else {
        Serial.println("[ACTUATOR] Lamp: OFF");
    }
}

void setBuzzer(bool state) {
    if (state) {
        Serial.println("[ACTUATOR] Buzzer: ALARM ON");
    } else {
        Serial.println("[ACTUATOR] Buzzer: OFF");
    }
}
