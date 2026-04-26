#include "devices/TaskFan.h"

#define FAN_CHANNEL 0
#define FAN_FREQ 25000
#define FAN_RESOLUTION 8

#define FAN_OFF 0
#define FAN_LOW 128
#define FAN_HIGH 255

void TaskFanControl(void *pvParameters){
  ledcSetup(FAN_CHANNEL, FAN_FREQ, FAN_RESOLUTION);
  ledcAttachPin(FAN_PIN, FAN_CHANNEL);
  AppContext *app = (AppContext *)pvParameters;

  while(1){
    // Code to control the fan based on sensor data

    if(xSemaphoreTake(app->xSemaphoreFan, portMAX_DELAY) == pdTRUE){
      if (app->currentMode == MANUAL) {
        Serial.println("Fan Control in MANUAL mode");
        xSemaphoreGive(app->xSemaphoreFan); // Release the semaphore for other tasks
        continue; // Skip fan control in MANUAL mode
      }
      // Use TinyML prediction for Fan Control
      if (app->ml_predicted_state == 1 || app->ml_predicted_state == 3) {
        ledcWrite(FAN_CHANNEL, FAN_HIGH); // Turn ON fan
        Serial.println("Fan ON (TinyML)");
      } else {
        ledcWrite(FAN_CHANNEL, FAN_OFF); // Turn OFF fan
        Serial.println("Fan OFF (TinyML)");
      }
      xSemaphoreGive(app->xSemaphoreFan); // Release the semaphore for other tasks
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}