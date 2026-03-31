#include "sensor_mock.h"

void mockSensorTask(void *pvParameters) {
    SystemContext *ctx = (SystemContext *)pvParameters;
    SensorData data;

    while (1) {
        data.temperature = random(200, 350) / 10.0;    
        data.humidity = random(400, 900) / 10.0;       
        data.soilMoisture = random(10, 80);            

        // Ghi vào Queue bên trong Context
        xQueueOverwrite(ctx->sensorQueue, &data);
        
        Serial.printf("[Sensor] Nhiệt độ=%.1f, Độ ẩm KK=%.1f, Độ ẩm đất=%.1f\n", 
                      data.temperature, data.humidity, data.soilMoisture);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void initMockSensorTask(SystemContext *ctx) {
    xTaskCreatePinnedToCore(mockSensorTask, "MockSensor", 4096, (void*)ctx, 1, NULL, 1);
}