#include "coreiot_mqtt.h"
#include <Preferences.h>


void initMQTTTask(void *pvParameters) {
    AppContext *ctx = (AppContext *)pvParameters;
    
    Preferences prefs;
    prefs.begin("iot_config", false);
    String mqtt_server = prefs.getString("server", "app.coreiot.io"); // Mặc định local
    int mqtt_port = prefs.getInt("port", 1883);
    String mqtt_token = prefs.getString("token", "4rhHtCs98RoOhzFz6DlD"); // Mặc định local
    prefs.end();

    WiFiClient espClient;
    PubSubClient client(espClient);

    if(mqtt_server.length() == 0 || mqtt_token.length() == 0) {
        vTaskDelete(NULL); 
    }

    client.setServer(mqtt_server.c_str(), mqtt_port);
    SensorData data;
    const TickType_t publishInterval = 5000 / portTICK_PERIOD_MS;
    TickType_t lastPublishTime = xTaskGetTickCount();

    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            if (!client.connected()) {
                if (client.connect("ESP32S3_Client", mqtt_token.c_str(), "")) {
                    Serial.println("[MQTT] Thành công!");
                } else {
                    vTaskDelay(5000 / portTICK_PERIOD_MS);
                    continue; 
                }
            }
            client.loop(); 

            if (xTaskGetTickCount() - lastPublishTime >= publishInterval) {
                if (xQueuePeek(ctx->xQueueSensor, &data, 0) == pdPASS) {
                    StaticJsonDocument<200> doc;
                    doc["temperature"] = data.temperature;
                    doc["humidity"] = data.humidity;
                    char jsonBuffer[256];
                    serializeJson(doc, jsonBuffer);
                    client.publish("v1/devices/me/telemetry", jsonBuffer);
                }
                lastPublishTime = xTaskGetTickCount();
            }
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}