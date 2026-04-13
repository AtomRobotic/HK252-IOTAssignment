#include "common.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Hệ thống đang khởi động..."); // put your setup code here, to run once:

  xBinarySemaphoreInternet = xSemaphoreCreateBinary();

  static AppContext app;

  app.xQueueSensor = xQueueCreate(1, sizeof(SensorData));
  app.xSemaphoreLed = xSemaphoreCreateMutex();
  app.xSemaphoreNeoLed = xSemaphoreCreateMutex();
  app.xSemaphoreLCD = xSemaphoreCreateMutex();

  xTaskCreatePinnedToCore(TaskLEDControl, "LED Control", 8192, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskTemperature_Humidity, "Temp Humidity", 8192, &app, 2, NULL, 1);
  //xTaskCreatePinnedToCore(TaskLuxSensor, "Lux Sensor", 8192, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCD Display", 8192, &app, 2, NULL, 1);
}

void loop()
{
  // put your main code here, to run repeatedly:
}