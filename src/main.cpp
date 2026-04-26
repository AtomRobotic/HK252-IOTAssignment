#include "common.h"

void setup() {
  Serial.begin(115200);

  delay(1000);
  Serial.println("Hệ thống đang khởi động...");

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    nvs_flash_erase();
    nvs_flash_init();
  }

  /* Thiết lập Wi-Fi (Cả AP để cấu hình và STA để kết nối Internet) */
  setupWiFi();

  /* Khởi tạo AppContext và các Semaphore, Queue */
  AppContext *appContext = new AppContext();
  initAppContext(appContext);

  // Khởi tạo Web Server (phục vụ UI) sử dụng AppContext
  initWebServer(appContext);


  /* Tạo Task */
  xTaskCreatePinnedToCore(TaskLEDControl, "LED Control", 2048, appContext, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskTemperature_Humidity, "Temp Humidity", 4096, appContext, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCD Display", 4096, appContext, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskNeoLED, "Neo LED Control", 2048, appContext, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskFanControl, "Fan Control", 2048, appContext, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskPumpControl, "Pump Control", 2048, appContext, 2, NULL, 1);
  xTaskCreatePinnedToCore(tiny_ml_task, "TinyML Task", 8192, appContext, 2, NULL, 1);
  // xTaskCreatePinnedToCore(TaskLuxSensor, "Lux Sensor", 2048, appContext, 2, NULL, 1);
  // xTaskCreatePinnedToCore(TaskSoilMoisture, "Soil Moisture", 2048, appContext, 2, NULL, 1);

  /* Khởi tạo MQTT Task */
  xTaskCreatePinnedToCore(initMQTTTask, "MQTT Task", 4096, appContext, 1, NULL, 0);

  Serial.println("Khởi tạo hệ thống thành công!");
}

void loop()
{
  // FreeRTOS quản lý, loop để trống
}