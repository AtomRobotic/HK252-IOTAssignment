#include "common.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Hệ thống đang khởi động..."); // put your setup code here, to run once:
}

void loop()
{
  // put your main code here, to run repeatedly:
  Serial.println("Hệ thống đang chạy...");
  vTaskDelay(1000);
}