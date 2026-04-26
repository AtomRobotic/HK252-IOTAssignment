#include "devices/TaskTempHumid.h"

DHT20 dht20;

void TaskTemperature_Humidity(void *pvParameters){
  AppContext *app = (AppContext *)pvParameters;

  Wire.begin(SDA_PIN, SCL_PIN); // Initialize I2C communication
  dht20.begin();
  while(1){
    dht20.read();
    float temperature = dht20.getTemperature();
    float humidity = dht20.getHumidity();

    if(isnan(temperature) || isnan(humidity)){
      Serial.println("Failed to read from DHT20 sensor!");
      temperature = humidity = -1;
    }

    app->sensorData.temperature = temperature;
    app->sensorData.humidity = humidity;

    SensorData packet;
    packet.temperature = temperature;
    packet.humidity = humidity;
    
    xQueueOverwrite(app->xQueueSensor, &packet);

    xSemaphoreGive(app->xSemaphoreLCD);
    if(app->currentMode == AUTO){
      xSemaphoreGive(app->xSemaphoreLed);
      xSemaphoreGive(app->xSemaphoreNeoLed);
      xSemaphoreGive(app->xSemaphoreFan);
      xSemaphoreGive(app->xSemaphorePump);
    }

    Serial.print("Temperature: "); Serial.print(temperature); Serial.print("°C ");
    Serial.print("Humidity: "); Serial.print(humidity); Serial.print("%");
    Serial.println();
    vTaskDelay(2000);
  }
}

