#include "devices/TaskLux.h"

void TaskLuxSensor(void *pvParameters){
  while(1){
    float luxValue = analogRead(LUX_PIN); // Read LUX sensor value
    Serial.print("LUX Value: "); Serial.println(luxValue);
    vTaskDelay(5000);
  }
}   

float getLuxValue(){
  float luxValue = analogRead(LUX_PIN); // Read LUX sensor value
  return luxValue;
}