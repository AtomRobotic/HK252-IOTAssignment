#include "common.h"

void TaskLEDControl(void *pvParameters) {
  pinMode(GPIO_NUM_48, OUTPUT); // Initialize LED pin
  int ledState = 0;
  while(1) {
    
    if (ledState == 0) {
      digitalWrite(GPIO_NUM_48, HIGH); // Turn ON LED
    } else {
      digitalWrite(GPIO_NUM_48, LOW); // Turn OFF LED
    }
    ledState = 1 - ledState;
    //Serial.println("LED State: " + String(ledState));
    vTaskDelay(2000);
  }
}

void TaskTemperature_Humidity(void *pvParameters){
  DHT20 dht20;
  Wire.begin(GPIO_NUM_11, GPIO_NUM_12); // Initialize I2C communication
  dht20.begin();
  while(1){
    dht20.read();
    float temperature = dht20.getTemperature();
    float humidity = dht20.getHumidity();

    Serial.print("Temperature: "); Serial.print(temperature); Serial.print("°C ");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.print("%");
    Serial.println();
    vTaskDelay(5000);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Hệ thống đang khởi động..."); // put your setup code here, to run once:

  xTaskCreatePinnedToCore(TaskLEDControl, "LED Control", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskTemperature_Humidity, "Temp Humidity", 2048, NULL, 2, NULL, 1);
}

void loop()
{
  // put your main code here, to run repeatedly:
}