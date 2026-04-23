#include "devices/TaskPump.h"

void TaskPumpControl(void *pvParameters){
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Ensure pump is off at startup
  while(1){
    // Code to control water pump based on soil moisture level
    AppContext *app = (AppContext *)pvParameters;

    if(xSemaphoreTake(app->xSemaphorePump, portMAX_DELAY) == pdTRUE){
      if (currentMode == MANUAL) {
        Serial.println("Pump Control in MANUAL mode");
        xSemaphoreGive(app->xSemaphorePump); // Release the semaphore for other tasks
        continue; // Skip pump control in MANUAL mode
      }
      SensorData rcvSensorData;
      if(xQueuePeek(app->xQueueSensor, &rcvSensorData, 0) == pdTRUE){
        float soilMoisture = rcvSensorData.soilMoisture;
        float humidity = rcvSensorData.humidity;
        if (soilMoisture < 30 || humidity < 40){ // Threshold for dry soil
          digitalWrite(PUMP_PIN, HIGH); // Turn ON water pump
          Serial.println("Water Pump ON");
        } else {
          digitalWrite(PUMP_PIN, LOW); // Turn OFF water pump
          Serial.println("Water Pump OFF");
        }
      }
    }
    vTaskDelay(2000);
  }
}