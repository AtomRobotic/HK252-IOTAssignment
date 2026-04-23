#include <Arduino.h>
#include "app_config.h"
#include "app_data_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

// Nếu dùng sensor thật, ta sẽ include thư viện ở đây
#if !USE_MOCK_DATA
#include "DHT.h"
DHT dht(DHT_PIN, DHT_TYPE);
#endif

extern QueueHandle_t xSensorQueue;

void sensor_task(void *pvParameters)
{
    SensorData_t data;

#if !USE_MOCK_DATA
    dht.begin(); // Khởi tạo cảm biến thật
#endif

    while (1)
    {
#if USE_MOCK_DATA
        // --- KHU VỰC MOCK DATA ---
        data.temperature = 25.0 + (rand() % 100) / 10.0;
        data.humidity = 60.0 + (rand() % 200) / 10.0;
        Serial.print("[SYSTEM] Using Mock Data: ");
#else
        // --- KHU VỰC SENSOR THẬT ---
        data.temperature = dht.readTemperature();
        data.humidity = dht.readHumidity();

        // Kiểm tra lỗi đọc sensor
        if (isnan(data.temperature) || isnan(data.humidity))
        {
            Serial.println("[ERROR] Failed to read from DHT sensor!");
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }
        Serial.print("[SYSTEM] Using Real Sensor: ");
#endif

        data.timestamp = millis();
        Serial.printf("T: %.1f, H: %.1f\n", data.temperature, data.humidity);

        // Đẩy vào Queue - Các tầng trên (TinyML, Cloud) không cần biết
        // dữ liệu này là thật hay giả, chúng chỉ lấy từ Queue[cite: 26].
        xQueueSend(xSensorQueue, &data, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}