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
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }

  // Cấp phát thiết bị động
  Adafruit_NeoPixel *myLed = new Adafruit_NeoPixel(1, 48, NEO_GRB + NEO_KHZ800);
  myLed->begin();
  myLed->setBrightness(50);
  myLed->clear();
  myLed->show();

  setupWiFi();

  // Tạo Context động (Nằm trên Heap, không phải biến toàn cục)
  SystemContext *ctx = new SystemContext();
  ctx->sensorQueue = xQueueCreate(1, sizeof(SensorData));
  ctx->relayQueue = xQueueCreate(5, sizeof(RelayCommand));
  ctx->autoMutex = xSemaphoreCreateMutex(); 
  ctx->led = myLed;
  ctx->autoCfg = {false, true, 40.0, true, 60.0, true, 32.0, true, 28.0};

  // Truyền duy nhất `ctx` vào tất cả các Task
  initMockSensorTask(ctx);
  initMQTTTask(ctx);
  initWebServer(ctx);

  xTaskCreatePinnedToCore(autoControlTask, "AutoTask", 4096, (void *)ctx, 1, NULL, 1);
  xTaskCreatePinnedToCore(relayControlTask, "RelayTask", 4096, (void *)ctx, 2, NULL, 1);
}

void loop() {
  vTaskDelete(NULL);
}