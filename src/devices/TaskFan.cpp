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

  while(1){
    // Code to control the fan based on sensor data
    //pinMode(FAN_PIN, OUTPUT); // Initialize fan pin
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
          ledcWrite(FAN_CHANNEL, FAN_OFF); // Turn OFF fan
          Serial.println("Fan OFF");
        } // Warning
        else if((humidity >= 80 && humidity <= 90) || 
                (temperature >= 30 && temperature <= 35)){
          ledcWrite(FAN_CHANNEL, FAN_LOW); // Turn ON fan at low speed
          //digitalWrite(FAN_PIN, HIGH); // Turn ON fan
          Serial.println("Fan ON at low speed");
        } // Critical
        else if (humidity > 90 || temperature > 35){
          ledcWrite(FAN_CHANNEL, FAN_HIGH); // Turn ON fan at high speed
          Serial.println("Fan ON at high speed");
        }
      }
    }
    vTaskDelay(2000);
  }
}