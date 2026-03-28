#include <Arduino.h>
#include "app_data_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Khai báo Queue Handle
QueueHandle_t xSensorQueue;

// Khai báo các Prototype Task
void sensor_task(void *pvParameters);
void tiny_ml_task(void *pvParameters); // Task bạn đã gửi trước đó

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("--- Smart Home RTOS Initializing ---");

  // 1. Khởi tạo Queue (Sức chứa 5 gói tin, loại bỏ biến toàn cục)
  xSensorQueue = xQueueCreate(5, sizeof(SensorData_t));

  if (xSensorQueue != NULL)
  {
    // 2. Tạo Task đọc/giả lập cảm biến (Độ ưu tiên 1)
    xTaskCreate(sensor_task, "Sensor_Task", 3072, NULL, 1, NULL);

    xTaskCreate(tiny_ml_task, "TinyML_Task", 8192, NULL, 2, NULL);

    Serial.println("FreeRTOS Tasks Started.");
  }
  else
  {
    Serial.println("Failed to create Queue!");
  }
}

void loop()
{
  // Để trống vì FreeRTOS đã quản lý luồng chạy [cite: 6]
}