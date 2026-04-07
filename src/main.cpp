#include "common.h"



void setup()
{
  Serial.begin(115200);
  Serial.println("Hệ thống đang khởi động..."); // put your setup code here, to run once:

  xTaskCreatePinnedToCore(TaskLEDControl, "LED Control", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskTemperature_Humidity, "Temp Humidity", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLuxSensor, "Lux Sensor", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskLCD, "LCD Display", 4096, NULL, 2, NULL, 1);
}

void loop()
{
  // put your main code here, to run repeatedly:
}