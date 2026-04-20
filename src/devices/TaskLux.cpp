#include "devices/TaskLux.h"

void TaskLuxSensor(void *pvParameters){
  while(1){
    float luxValue = analogRead(LUX_PIN); // Read LUX sensor value
    Serial.print("LUX Value: "); Serial.println(luxValue);

    AppContext *app = (AppContext *)pvParameters;
    app->sensorData.lux = luxValue;
    SensorData packet;
    packet.lux = luxValue;
    packet.humidity = app->sensorData.humidity;
    packet.temperature = app->sensorData.temperature;
    packet.soilMoisture = app->sensorData.soilMoisture;
    xQueueOverwrite(app->xQueueSensor, &packet);
    
    vTaskDelay(5000);
  }
}   

float getLuxValue(){
  float luxValue = analogRead(LUX_PIN); // Read LUX sensor value
  return luxValue;
}