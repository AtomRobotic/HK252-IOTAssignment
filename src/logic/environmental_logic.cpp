#include "environmental_logic.h"

void handle_environmental_actions(SystemState_t state, SensorData_t data) {
    // Reset defaults
    bool pump = false;
    int fan_speed = 0;
    bool lamp = false;
    bool buzzer = false;

    if (state == STATE_NORMAL) {
        // Normal: Pump OFF, Fan OFF
        Serial.println("[LOGIC] State: NORMAL. System operating normally.");
    } 
    else if (state == STATE_WARNING) {
        Serial.println("[LOGIC] State: WARNING. Adjusting conditions...");
        
        // Temperature high -> fan (Threshold > 30)
        if (data.temperature > 30.0) fan_speed = 128; 
        
        // Air Humidity high -> fan (Threshold > 80)
        if (data.humidity > 80.0) fan_speed = 120;

        // Soil humidity low -> pump (Threshold < 40)
        if (data.soil_humidity < 40.0) pump = true;

        // Light low -> lamp (Threshold < 300)
        if (data.light < 300.0) lamp = true;
    } 
    else if (state == STATE_CRITICAL) {
        Serial.println("[LOGIC] State: CRITICAL! Taking immediate action!");
        buzzer = true;

        // Temperature > 35 -> fan max
        if (data.temperature > 35.0) fan_speed = 255;
        
        // Soil < 30 -> pump continuous
        if (data.soil_humidity < 30.0) {
            pump = true;
            Serial.println("[LOGIC] CRITICAL: Soil too dry!");
        }

        // Soil > 80 -> fan to evaporate
        if (data.soil_humidity > 80.0) fan_speed = 200;

        // Air Hum > 90 -> fan max (anti-mold)
        if (data.humidity > 90.0) fan_speed = 255;

        // Air Hum < 40 -> pump (missing mist)
        if (data.humidity < 40.0) pump = true;

        // Light too low -> lamp
        if (data.light < 100.0) lamp = true;
    }

    // Apply actions
    setPump(pump);
    setFan(fan_speed);
    setLamp(lamp);
    setBuzzer(buzzer);
}
