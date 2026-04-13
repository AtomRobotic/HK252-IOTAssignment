#include "devices/TaskLED.h"

// void TaskLEDControl(void *pvParameters) {
//   pinMode(LED_PIN, OUTPUT); // Initialize LED pin
//   int ledState = 0;
//   while(1) {
    
//     if (ledState == 0) {
//       digitalWrite(LED_PIN, HIGH); // Turn ON LED
//     } else {
//       digitalWrite(LED_PIN, LOW); // Turn OFF LED
//     }
//     ledState = 1 - ledState;
//     //Serial.println("LED State: " + String(ledState));
//     vTaskDelay(2000);
//   }
// }

void TaskLEDControl(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT); // Initialize LED pin
  AppContext *app = (AppContext *)pvParameters;

  SensorData rcvSensorData;

  int delayTime = 1000; // Default delay time

  while(1){
    if(xSemaphoreTake(app->xSemaphoreLed, portMAX_DELAY) == pdTRUE){
      if (currentMode == MANUAL) {
        Serial.println("LED Control in MANUAL mode");
        xSemaphoreGive(app->xSemaphoreLed); // Release the semaphore for other tasks
        continue; // Skip LED control in MANUAL mode
      }
      if(xQueuePeek(app->xQueueSensor, &rcvSensorData, 0) == pdTRUE){
        // Normal
        if (rcvSensorData.temperature > 20 && rcvSensorData.temperature < 30){
          delayTime = 1000;
        } // Warning
        else if((rcvSensorData.temperature >= 15 || rcvSensorData.temperature <= 20) && 
                (rcvSensorData.temperature >= 30 || rcvSensorData.temperature <= 35)){
          delayTime = 500;
        } // Critical
        else if (rcvSensorData.temperature < 15 || rcvSensorData.temperature > 35){
          delayTime = 200;
        }
      }

      digitalWrite(LED_PIN, HIGH); // Turn ON LED
      vTaskDelay(delayTime); 
      digitalWrite(LED_PIN, LOW); // Turn OFF LED
      vTaskDelay(delayTime);
    }
  }
}