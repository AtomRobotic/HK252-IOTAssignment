#include "devices/TaskNEO.h"

#define RED 0
#define ORANGE 1
#define YELLOW 2
#define GREEN 3
#define BLUE 4
#define INDIGO 5
#define VIOLET 6
#define WHITE 7
#define BLACK 8

Adafruit_NeoPixel NeoPixel(NUM_PIXELS, PIN_NEO_PIXEL, NEO_GRB + NEO_KHZ800);

// Bảng màu với các giá trị RGB
const uint32_t colorTable[] = {
  NeoPixel.Color(255, 0, 0),    // Đỏ
  NeoPixel.Color(255, 165, 0),  // Cam
  NeoPixel.Color(255, 255, 0),  // Vàng
  NeoPixel.Color(0, 255, 0),    // Lục
  NeoPixel.Color(0, 0, 255),  // Lam
  NeoPixel.Color(75, 0, 130),    // Chàm
  NeoPixel.Color(128, 0, 128),  // Tím
  NeoPixel.Color(255, 255, 255),// Trắng
  NeoPixel.Color(0, 0, 0)       // Đen
};


void TaskNeoLED(void *pvParameters) {
  NeoPixel.begin();
  NeoPixel.clear();
  NeoPixel.show();

  AppContext *app = (AppContext *)pvParameters;

  SensorData rcvSensorData;
  while(1) {
    if(xSemaphoreTake(app->xSemaphoreNeoLed, portMAX_DELAY) == pdTRUE){
      if(currentMode == MANUAL){
        Serial.println("Neo LED Control in MANUAL mode");
        xSemaphoreGive(app->xSemaphoreNeoLed); // Release the semaphore for other tasks
        continue; // Skip Neo LED control in MANUAL mode
      }
      if(xQueuePeek(app->xQueueSensor, &rcvSensorData, 0) == pdTRUE){
        float humidity = rcvSensorData.humidity;

        int colorIndex;
        if (humidity > 50 && humidity < 80) {
          colorIndex = GREEN; // Normal
        } else if ((humidity >= 40 && humidity <= 50) || (humidity >= 80 && humidity <= 90)) {
          colorIndex = ORANGE; // Warning
        } else {
          colorIndex = RED; // Critical
        }

        NeoPixel.setPixelColor(0, colorTable[colorIndex]);
        NeoPixel.show();
      }
    }
    vTaskDelay(2000); // Update every second
  }
}

// void setRGBColor() {
  
// }