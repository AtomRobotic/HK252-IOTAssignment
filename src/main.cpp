#include "common.h"
#include "sensor_mock.h"
#include "web_server.h"
#include "wifi_manager.h"
#include "coreiot_mqtt.h"
#include <nvs_flash.h>

extern void setupWiFi();

// ==========================================================
// KHÔNG CÓ BẤT KỲ BIẾN GLOBAL NÀO Ở ĐÂY CẢ! ĐÚNG CHUẨN RTOS!
// ==========================================================

void autoControlTask(void *pvParameters) {
  SystemContext *ctx = (SystemContext *)pvParameters;
  
  // Biến cục bộ để nhớ trạng thái trước đó, tránh gửi lệnh spam
  int currentPumpState = -1; 
  int currentFanState = -1;

  while (1) {
    // 1. Take Semaphore (Bảo vệ dữ liệu cấu hình đang đọc)
    if (xSemaphoreTake(ctx->autoMutex, portMAX_DELAY) == pdTRUE) {
      
      if (ctx->autoCfg.isAuto) {
        SensorData data;
        if (xQueuePeek(ctx->sensorQueue, &data, 0) == pdPASS) {
          RelayCommand cmd;

          // Rule Bơm (Relay 1)
          if (ctx->autoCfg.r1_en && data.soilMoisture < ctx->autoCfg.soil_low && currentPumpState != 1) {
            cmd = {1, 1}; xQueueSend(ctx->relayQueue, &cmd, 0); currentPumpState = 1;
            Serial.println("[AUTO] Đất khô -> BẬT Bơm");
          } else if (ctx->autoCfg.r2_en && data.soilMoisture > ctx->autoCfg.soil_high && currentPumpState != 0) {
            cmd = {1, 0}; xQueueSend(ctx->relayQueue, &cmd, 0); currentPumpState = 0;
            Serial.println("[AUTO] Đất đủ ẩm -> TẮT Bơm");
          }

          // Rule Quạt (Relay 2)
          if (ctx->autoCfg.r3_en && data.temperature > ctx->autoCfg.temp_high && currentFanState != 1) {
            cmd = {2, 1}; xQueueSend(ctx->relayQueue, &cmd, 0); currentFanState = 1;
            Serial.println("[AUTO] Nóng -> BẬT Quạt");
          } else if (ctx->autoCfg.r4_en && data.temperature < ctx->autoCfg.temp_low && currentFanState != 0) {
            cmd = {2, 0}; xQueueSend(ctx->relayQueue, &cmd, 0); currentFanState = 0;
            Serial.println("[AUTO] Mát -> TẮT Quạt");
          }
        }
      } else {
        // Reset nếu chuyển qua Manual
        currentPumpState = -1; 
        currentFanState = -1;
      }
      
      // 2. Trả lại Semaphore
      xSemaphoreGive(ctx->autoMutex);
    }
    vTaskDelay(5000 / portTICK_PERIOD_MS);
  }
}

void relayControlTask(void *pvParameters) {
  SystemContext *ctx = (SystemContext *)pvParameters;
  RelayCommand cmd;
  
  while(1) {
    if (xQueueReceive(ctx->relayQueue, &cmd, portMAX_DELAY) == pdPASS) {
      if(cmd.deviceId == 2) {
        if(cmd.state == 1) ctx->led->setPixelColor(0, ctx->led->Color(0, 0, 255));
        else ctx->led->setPixelColor(0, ctx->led->Color(255, 0, 0));
        ctx->led->show();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Hệ thống đang khởi động...");

  /* Khởi tạo Semaphores & Queues */
  xBinarySemaphoreInternet = xSemaphoreCreateBinary();
  xQueueSensor = xQueueCreate(1, sizeof(SensorData));
  xSemaphoreLed = xSemaphoreCreateBinary();
  xSemaphoreNeoLed = xSemaphoreCreateBinary();
  xSemaphoreLCD = xSemaphoreCreateBinary();
  xSemaphoreFan = xSemaphoreCreateBinary();
  xSemaphorePump = xSemaphoreCreateBinary();
  
  // Giải phóng Semaphore ban đầu (nếu cần)
  xSemaphoreGive(xSemaphoreFan);
  xSemaphoreGive(xSemaphorePump);

  /* Gán vào AppContext cho các task */
  static AppContext app;
  app.xQueueSensor = xQueueSensor;
  app.xSemaphoreLed = xSemaphoreLed;
  app.xSemaphoreNeoLed = xSemaphoreNeoLed;
  app.xSemaphoreLCD = xSemaphoreLCD;
  app.xSemaphoreFan = xSemaphoreFan;
  app.xSemaphorePump = xSemaphorePump;
  app.xBinarySemaphoreInternet = xBinarySemaphoreInternet;

  /* Tạo Task */
  xTaskCreatePinnedToCore(TaskLEDControl, "LED Control", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskTemperature_Humidity, "Temp Humidity", 4096, &app, 2, NULL, 1);
  // xTaskCreatePinnedToCore(TaskLuxSensor, "Lux Sensor", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCD Display", 4096, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskNeoLED, "Neo LED Control", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskFanControl, "Fan Control", 2048, &app, 2, NULL, 1);
  // xTaskCreatePinnedToCore(TaskSoilMoisture, "Soil Moisture", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskPumpControl, "Pump Control", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(tiny_ml_task, "TinyML Task", 8192, &app, 2, NULL, 1);

  Serial.println("Khởi tạo hệ thống thành công!");
}

void loop()
{
  // FreeRTOS quản lý, loop để trống
}