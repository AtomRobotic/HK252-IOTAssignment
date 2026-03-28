#include "sensor_mock.h"

void mockSensorTask(void *pvParameters) {
    QueueHandle_t queue = (QueueHandle_t)pvParameters;
    SensorData data;

    while (1) {
        // Giả lập data
        data.temperature = random(200, 350) / 10.0;    
        data.humidity = random(400, 900) / 10.0;       
        data.soilMoisture = random(10, 80);            

        xQueueOverwrite(queue, &data);
        
        Serial.printf("[Sensor] Đã cập nhật lên Queue: Nhiệt độ=%.1f, Độ ẩm=%.1f\n", 
                      data.temperature, data.humidity);

        // Đọc cảm biến mỗi 5 giây
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void initMockSensorTask(QueueHandle_t dataQueue) {
    xTaskCreatePinnedToCore(mockSensorTask, "MockSensor", 4096, (void*)dataQueue, 1, NULL, 1);
}