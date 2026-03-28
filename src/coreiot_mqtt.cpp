#include "coreiot_mqtt.h"

// Thông tin Broker của CoreIOT
const char* mqtt_server = "app.coreiot.io";
const int mqtt_port = 1883;
const char* mqtt_token = "4rhHtCs98RoOhzFz6DlD";

void mqttTask(void *pvParameters) {
    QueueHandle_t sensorQueue = (QueueHandle_t)pvParameters;
    
    // Khai báo đối tượng MQTT cục bộ
    WiFiClient espClient;
    PubSubClient client(espClient);
    client.setServer(mqtt_server, mqtt_port);

    SensorData data;
    
    // Cài đặt gửi 5 giây một lần
    const TickType_t publishInterval = 5000 / portTICK_PERIOD_MS;
    TickType_t lastPublishTime = xTaskGetTickCount();

    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            
            // 1. Kết nối với CoreIOT
            if (!client.connected()) {
                Serial.print("[MQTT] Đang kết nối tới CoreIOT...");
                if (client.connect("ESP32S3_Client", mqtt_token, "")) {
                    Serial.println(" Thành công!");
                } else {
                    Serial.print(" Thất bại, mã lỗi = ");
                    Serial.println(client.state());
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                    continue;
                }
            }
            client.loop();

            // 2. Định kỳ gửi dữ liệu
            if (xTaskGetTickCount() - lastPublishTime >= publishInterval) {

                if (xQueuePeek(sensorQueue, &data, 0) == pdPASS) {
                    
                    // Đóng gói JSON
                    StaticJsonDocument<200> doc;
                    doc["temperature"] = data.temperature;
                    doc["humidity"] = data.humidity;
                    doc["soilMoisture"] = data.soilMoisture;

                    char jsonBuffer[256];
                    serializeJson(doc, jsonBuffer);

                    // Gửi lên CoreIOT
                    if(client.publish("v1/devices/me/telemetry", jsonBuffer)) {
                        Serial.printf("[MQTT] Đã đẩy lên CoreIOT: %s\n", jsonBuffer);
                    } else {
                        Serial.println("[MQTT] Lỗi đẩy dữ liệu!");
                    }
                }
                lastPublishTime = xTaskGetTickCount();
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

void initMQTTTask(QueueHandle_t sensorQueue) {
    xTaskCreatePinnedToCore(mqttTask, "MQTTTask", 4096, (void*)sensorQueue, 1, NULL, 0);
}