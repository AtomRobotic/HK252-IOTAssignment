#include "devices/TaskPump.h"

void TaskPumpControl(void *pvParameters){
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Ensure pump is off at startup
  while(1){
    // Code to control water pump based on soil moisture level
    AppContext *app = (AppContext *)pvParameters;

    if(xSemaphoreTake(app->xSemaphorePump, portMAX_DELAY) == pdTRUE){
      if (app->currentMode == MANUAL) {
        Serial.println("Pump Control in MANUAL mode");
        xSemaphoreGive(app->xSemaphorePump); // Release the semaphore for other tasks
        continue; // Skip pump control in MANUAL mode
      }
      // Use TinyML prediction for Pump control
      if (app->ml_predicted_state == 2 || app->ml_predicted_state == 3) {
        digitalWrite(PUMP_PIN, HIGH); // Turn ON water pump
        Serial.println("Pump ON (TinyML)");
      } else {
        digitalWrite(PUMP_PIN, LOW); // Turn OFF water pump
        Serial.println("Pump OFF (TinyML)");
      }
      xSemaphoreGive(app->xSemaphorePump); // Release the semaphore for other tasks
    }
    vTaskDelay(2000);
  }
}