#include "devices/TaskTempHumid.h"

DHT20 dht20;

void TaskTemperature_Humidity(void *pvParameters){
  Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C communication
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

void SerialPrintTemperature(){
  dht20.read();
  float temperature = dht20.getTemperature();
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println("°C");
}

void SerialPrintHumidity(){
  dht20.read();
  float humidity = dht20.getHumidity();
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
}

float getTemperature(){
  dht20.read();
  float temperature = dht20.getTemperature();
  return temperature;
}

float getHumidity(){
  dht20.read();
  float humidity = dht20.getHumidity();
  return humidity;
}
