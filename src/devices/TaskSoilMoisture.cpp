#include "devices/TaskSoilMoisture.h"

void TaskSoilMoisture(void *pvParameters){
  while(1){
    // Code to read soil moisture sensor and control water pump
    AppContext *app = (AppContext *)pvParameters;

    float sensorValue = analogRead(SOIL_MOISTURE_PIN);
    sensorValue = map(sensorValue, 0, 4095, 0, 100); // Convert to percentage
    Serial.print("Soil Moisture Value: "); Serial.print(sensorValue); Serial.println("%");

    
    app->sensorData.soilMoisture = sensorValue;

    SensorData packet;
    packet.soilMoisture = sensorValue;
    packet.humidity = app->sensorData.humidity;
    packet.temperature = app->sensorData.temperature;
    xQueueOverwrite(app->xQueueSensor, &packet);

    vTaskDelay(2000);
  }
}