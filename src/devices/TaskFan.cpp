#include "devices/TaskFan.h"

void TaskFanControl(void *pvParameters){
  while(1){
    // Code to control the fan based on sensor data
    pinMode(FAN_PIN, OUTPUT); // Initialize fan pin
    AppContext *app = (AppContext *)pvParameters;
    SensorData rcvSensorData;

    if(xSemaphoreTake(app->xSemaphoreFan, portMAX_DELAY) == pdTRUE){
      if (currentMode == MANUAL) {
        Serial.println("Fan Control in MANUAL mode");
        xSemaphoreGive(app->xSemaphoreFan); // Release the semaphore for other tasks
        continue; // Skip fan control in MANUAL mode
      }
      if(xQueuePeek(app->xQueueSensor, &rcvSensorData, 0) == pdTRUE){
        // Normal
        float temperature = rcvSensorData.temperature;
        float humidity = rcvSensorData.humidity;
        if (temperature > 20 && temperature < 30){
          digitalWrite(FAN_PIN, LOW); // Turn OFF fan
          Serial.println("Fan OFF");
        } // Warning
        else if((humidity >= 80 || humidity <= 90) && 
                (temperature >= 30 || temperature <= 35)){
          digitalWrite(FAN_PIN, HIGH); // Turn ON fan at low speed
          Serial.println("Fan ON at low speed");
        } // Critical
        else if (humidity > 90 || temperature > 35){
          digitalWrite(FAN_PIN, HIGH); // Turn ON fan at high speed
          Serial.println("Fan ON at high speed");
        }
      }
    }
    vTaskDelay(2000);
  }
}