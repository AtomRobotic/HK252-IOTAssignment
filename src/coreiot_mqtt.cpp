#include "coreiot_mqtt.h"
#include <Preferences.h>

// Thông tin Broker của CoreIOT
const char* mqtt_server = "app.coreiot.io";
const int mqtt_port = 1883;
const char* mqtt_token = "4rhHtCs98RoOhzFz6DlD";

void mqttTask(void *pvParameters) {
    QueueHandle_t sensorQueue = (QueueHandle_t)pvParameters;
    
    // ĐỌC THÔNG TIN MQTT TỪ BỘ NHỚ
    Preferences prefs;
    prefs.begin("iot_config", false);
    String mqtt_server = prefs.getString("server", "");
    int mqtt_port = prefs.getInt("port", 1883);
    String mqtt_token = prefs.getString("token", "");
    prefs.end();

    WiFiClient espClient;
    PubSubClient client(espClient);

    // Nếu chưa có cấu hình Server, tự sát (xóa) Task MQTT để tiết kiệm RAM chờ Reset
    if(mqtt_server.length() == 0 || mqtt_token.length() == 0) {
        Serial.println("[MQTT] Thiếu cấu hình Server/Token. Tạm ngưng MQTT.");
        vTaskDelete(NULL); 
    }

    client.setServer(mqtt_server.c_str(), mqtt_port);

    SensorData data;
    const TickType_t publishInterval = 5000 / portTICK_PERIOD_MS;
    TickType_t lastPublishTime = xTaskGetTickCount();

    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            if (!client.connected()) {
                Serial.print("[MQTT] Đang kết nối Server...");
                // Dùng Token làm username
                if (client.connect("ESP32S3_Client", mqtt_token.c_str(), "")) {
                    Serial.println(" Thành công!");
                } else {
                    Serial.print(" Lỗi=");
                    Serial.println(client.state());
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                    continue; 
                }
            }
            client.loop(); 

            if (xTaskGetTickCount() - lastPublishTime >= publishInterval) {
                if (xQueuePeek(sensorQueue, &data, 0) == pdPASS) {
                    
                    StaticJsonDocument<200> doc;
                    doc["temperature"] = data.temperature;
                    doc["humidity"] = data.humidity;
                    doc["soilMoisture"] = data.soilMoisture;

                    char jsonBuffer[256];
                    serializeJson(doc, jsonBuffer);

                    if(client.publish("v1/devices/me/telemetry", jsonBuffer)) {
                        Serial.printf("[MQTT] Đã gửi: %s\n", jsonBuffer);
                    } else {
                        Serial.println("[MQTT] Lỗi gửi data!");
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