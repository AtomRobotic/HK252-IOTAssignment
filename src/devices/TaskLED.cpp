#include "devices/TaskLED.h"

void TaskLEDControl(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT); // Initialize LED pin
  AppContext *app = (AppContext *)pvParameters;

  SensorData rcvSensorData;

  int delayTime = 1000; // Default delay time

  while(1){
    if(xSemaphoreTake(app->xSemaphoreLed, portMAX_DELAY) == pdTRUE){
      if (app->currentMode == MANUAL) {
        Serial.println("LED Control in MANUAL mode");
        xSemaphoreGive(app->xSemaphoreLed); // Release the semaphore for other tasks
        continue; // Skip LED control in MANUAL mode
      }
      if(xQueuePeek(app->xQueueSensor, &rcvSensorData, 0) == pdTRUE){
        // Normal
        if (rcvSensorData.temperature > 20 && rcvSensorData.temperature < 30){
          delayTime = 1000;
        } // Warning
        else if((rcvSensorData.temperature >= 15 && rcvSensorData.temperature <= 20) || 
                (rcvSensorData.temperature >= 30 && rcvSensorData.temperature <= 35)){
          delayTime = 500;
        } // Critical
        else if (rcvSensorData.temperature < 15 && rcvSensorData.temperature > 35){
          delayTime = 200;
        }
      }

      xSemaphoreGive(app->xSemaphoreLed); // Release the semaphore for other tasks

      digitalWrite(LED_PIN, HIGH); // Turn ON LED
      vTaskDelay(delayTime); 
      digitalWrite(LED_PIN, LOW); // Turn OFF LED
      vTaskDelay(delayTime);
    }
  }
}