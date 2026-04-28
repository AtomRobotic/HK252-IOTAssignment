#include "devices/TaskPump.h"

void TaskPumpControl(void *pvParameters){
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW); // Đảm bảo bơm tắt lúc vừa cấp điện
  
  AppContext *app = (AppContext *)pvParameters;

  while(1){
    // Chờ tín hiệu kích hoạt
    if(xSemaphoreTake(app->xSemaphorePump, portMAX_DELAY) == pdTRUE){
      
      // Tính năng an toàn dự phòng
      if (app->currentMode == MANUAL) {
         // Nhường quyền cho Web Server
      } 
      else {
        // CHẾ ĐỘ AUTO: Dùng kết quả phân tích từ TinyML
        // 2: Bật bơm, 3: Bật cả 2
        if (app->ml_predicted_state == 2 || app->ml_predicted_state == 3) {
          digitalWrite(PUMP_PIN, HIGH); 
          Serial.println("[TinyML Auto] Máy Bơm: BẬT");
        } else {
          digitalWrite(PUMP_PIN, LOW); 
          Serial.println("[TinyML Auto] Máy Bơm: TẮT");
        }
      }
    }
  }
}