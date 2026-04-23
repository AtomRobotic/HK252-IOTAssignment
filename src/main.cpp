#include "common.h"
void setup()
{
  Serial.begin(115200);
  Serial.println("Hệ thống đang khởi động..."); // put your setup code here, to run once:

  xBinarySemaphoreInternet = xSemaphoreCreateBinary();

  static AppContext app;

  app.xQueueSensor = xQueueCreate(1, sizeof(SensorData));
  app.xSemaphoreLed = xSemaphoreCreateBinary();
  app.xSemaphoreNeoLed = xSemaphoreCreateBinary();
  app.xSemaphoreLCD = xSemaphoreCreateBinary();
  app.xSemaphoreFan = xSemaphoreCreateBinary();
  app.xSemaphorePump = xSemaphoreCreateBinary();

  // if(xQueueSensor == NULL){
  //   Serial.println("Failed to create sensor data queue!");
  // }
  // else{
  //  Serial.println("Sensor data queue created successfully.");
  // }

  xTaskCreatePinnedToCore(TaskLEDControl, "LED Control", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskTemperature_Humidity, "Temp Humidity", 4096, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLuxSensor, "Lux Sensor", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCD Display", 4096, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskNeoLED, "Neo LED Control", 2048, &app, 2, NULL, 1);
  //xTaskCreatePinnedToCore(TaskFanControl, "Fan Control", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskSoilMoisture, "Soil Moisture", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskPumpControl, "Pump Control", 2048, &app, 2, NULL, 1);
  //xTaskCreatePinnedToCore(tiny_ml_task, "TinyML Task", 8192, &app, 2, NULL, 1);
  

}

void loop()
{
  // Để trống vì FreeRTOS đã quản lý luồng chạy [cite: 6]
}