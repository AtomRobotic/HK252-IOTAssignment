#include "devices/TaskFan.h"

#define FAN_CHANNEL 0
#define FAN_FREQ 25000
#define FAN_RESOLUTION 8

#define FAN_OFF 0
#define FAN_HIGH 255

void TaskFanControl(void *pvParameters){
  ledcSetup(FAN_CHANNEL, FAN_FREQ, FAN_RESOLUTION);
  ledcAttachPin(FAN_PIN, FAN_CHANNEL);
  AppContext *app = (AppContext *)pvParameters;

  while(1){
    // Chờ tín hiệu (Semaphore) kích hoạt từ cảm biến
    if(xSemaphoreTake(app->xSemaphoreFan, portMAX_DELAY) == pdTRUE){
      
      // Tính năng an toàn dự phòng: Nếu đang ở MANUAL thì Task này không làm gì cả
      if (app->currentMode == MANUAL) {
        // Nhường quyền cho Web Server điều khiển
      } 
      else {
        // CHẾ ĐỘ AUTO: Dùng kết quả phân tích từ TinyML
        // 1: Bật quạt, 3: Bật cả 2
        if (app->ml_predicted_state == 1 || app->ml_predicted_state == 3) {
          ledcWrite(FAN_CHANNEL, FAN_HIGH); 
          Serial.println("[TinyML Auto] Quạt: BẬT");
        } else {
          ledcWrite(FAN_CHANNEL, FAN_OFF); 
          Serial.println("[TinyML Auto] Quạt: TẮT");
        }
      }
    }
  }
}