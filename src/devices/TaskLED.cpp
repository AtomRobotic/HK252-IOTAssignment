#include "devices/TaskLED.h"

void TaskLEDControl(void *pvParameters) {
  pinMode(LED_PIN, OUTPUT); // Initialize LED pin
  int ledState = 0;
  while(1) {
    
    if (ledState == 0) {
      digitalWrite(LED_PIN, HIGH); // Turn ON LED
    } else {
      digitalWrite(LED_PIN, LOW); // Turn OFF LED
    }
    ledState = 1 - ledState;
    //Serial.println("LED State: " + String(ledState));
    vTaskDelay(2000);
  }
}