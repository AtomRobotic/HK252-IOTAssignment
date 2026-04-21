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
        data.temperature = 20.0 + (rand() % 200) / 10.0;     // 20 - 40 *C
        data.humidity = 40.0 + (rand() % 600) / 10.0;        // 40 - 100 %
        data.soil_humidity = 30.0 + (rand() % 600) / 10.0;   // 30 - 90 %
        data.light = 200.0 + (rand() % 10000) / 10.0;        // 200 - 1200 Lux
        Serial.print("[SYSTEM] Using Mock Data: ");
#else
        // --- KHU VỰC SENSOR THẬT ---
        data.temperature = dht.readTemperature();
        data.humidity = dht.readHumidity();
        data.soil_humidity = 50.0; // Mocked for real sensor mode until hardware is added
        data.light = 500.0;       // Mocked for real sensor mode until hardware is added

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
        Serial.printf("T: %.1f, H: %.1f, Soil: %.1f, Light: %.1f\n", 
                      data.temperature, data.humidity, data.soil_humidity, data.light);

        // Đẩy vào Queue - Các tầng trên (TinyML, Cloud) không cần biết
        // dữ liệu này là thật hay giả, chúng chỉ lấy từ Queue[cite: 26].
        xQueueSend(xSensorQueue, &data, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}