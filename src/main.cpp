#include "common.h"

// Task Fan - Done
// Task Bom nuoc
// Task Do am dat - Done


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

  

  xTaskCreatePinnedToCore(TaskLEDControl, "LED Control", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskTemperature_Humidity, "Temp Humidity", 4096, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLuxSensor, "Lux Sensor", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCD Display", 4096, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskNeoLED, "Neo LED Control", 2048, &app, 2, NULL, 1);
  //xTaskCreatePinnedToCore(TaskFanControl, "Fan Control", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskSoilMoisture, "Soil Moisture", 2048, &app, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskPumpControl, "Pump Control", 2048, &app, 2, NULL, 1);

}

void loop()
{
  // put your main code here, to run repeatedly:
}