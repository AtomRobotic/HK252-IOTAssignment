#include "sensor_mock.h"

void initMockSensorTask(void *pvParameters) {
    AppContext *ctx = (AppContext *)pvParameters;
    SensorData data;

    while (1) {
        ctx->sensorData.temperature = random(200, 350) / 10.0;    
        ctx->sensorData.humidity = random(400, 900) / 10.0;       
        
        SensorData packet;
        packet.temperature = ctx->sensorData.temperature;
        packet.humidity = ctx->sensorData.humidity;

        // Ghi vào Queue bên trong Context
        xQueueOverwrite(ctx->xQueueSensor, &packet);

        Serial.printf("[Sensor] Nhiệt độ=%.1f, Độ ẩm KK=%.1f\n", 
                      packet.temperature, packet.humidity);

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    //xTaskCreatePinnedToCore(mockSensorTask, "MockSensor", 4096, (void*)ctx, 1, NULL, 1);
}