#include "common.h"



void setup()
{
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