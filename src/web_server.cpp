#include "web_server.h"
#include <LittleFS.h> // <--- 1. Đổi sang thư viện LittleFS
#include <Preferences.h>

AsyncWebServer server(80);

void initWebServer(QueueHandle_t sensorQueue, QueueHandle_t relayQueue) {
    
    // 2. Khởi tạo hệ thống File LittleFS (true: tự động format nếu bị lỗi)
    if(!LittleFS.begin(true)){
        Serial.println("[Web Server] Lỗi: Không thể mount LittleFS");
        return;
    }
    Serial.println("[Web Server] Đã khởi tạo LittleFS thành công!");

    // 3. Cấu hình Route đọc file từ LittleFS
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html"); // <--- Đổi thành LittleFS
    });

    server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/script.js", "text/javascript"); // <--- Đổi thành LittleFS
    });

    // --- Giữ nguyên Route /data ---
    server.on("/data", HTTP_GET, [sensorQueue](AsyncWebServerRequest *request){
        SensorData data;
        if (xQueuePeek(sensorQueue, &data, 0) == pdPASS) {
            char jsonBuffer[100];
            snprintf(jsonBuffer, sizeof(jsonBuffer), "{\"t\":%.1f, \"h\":%.1f, \"s\":%.1f}", 
                     data.temperature, data.humidity, data.soilMoisture);
            request->send(200, "application/json", jsonBuffer);
        } else {
            request->send(503, "application/json", "{\"error\":\"No data\"}");
        }
    });

    // --- Giữ nguyên Route /relay ---
    server.on("/relay", HTTP_GET, [relayQueue](AsyncWebServerRequest *request){
        if (request->hasParam("id") && request->hasParam("state")) {
            RelayCommand cmd;
            cmd.deviceId = request->getParam("id")->value().toInt();
            cmd.state = request->getParam("state")->value().toInt();
            
            xQueueSend(relayQueue, &cmd, 0);
            
            Serial.printf("[Web] Đã nhận lệnh: Thiết bị %d -> Trạng thái %d\n", cmd.deviceId, cmd.state);
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "Bad Request");
        }
    });

    server.on("/get_config", HTTP_GET, [](AsyncWebServerRequest *request){
        Preferences prefs;
        prefs.begin("iot_config", true); // true = Read-only

        String ssid = prefs.getString("ssid", "");
        String server = prefs.getString("server", "");
        int port = prefs.getInt("port", 1883);
        String token = prefs.getString("token", "");
        prefs.end();

        // Đóng gói thành chuỗi JSON
        char jsonBuffer[256];
        snprintf(jsonBuffer, sizeof(jsonBuffer), 
                 "{\"ssid\":\"%s\", \"server\":\"%s\", \"port\":%d, \"token\":\"%s\"}", 
                 ssid.c_str(), server.c_str(), port, token.c_str());
        
        request->send(200, "application/json", jsonBuffer);
    });

    server.on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
        Preferences prefs;
        prefs.begin("iot_config", false); // false = Read/Write

        // Kiểm tra: Chỉ ghi vào bộ nhớ nếu người dùng CÓ NHẬP chữ (length > 0)
        if(request->hasParam("ssid") && request->getParam("ssid")->value().length() > 0) {
            prefs.putString("ssid", request->getParam("ssid")->value());
        }
        if(request->hasParam("pass") && request->getParam("pass")->value().length() > 0) {
            prefs.putString("pass", request->getParam("pass")->value());
        }
        if(request->hasParam("server") && request->getParam("server")->value().length() > 0) {
            prefs.putString("server", request->getParam("server")->value());
        }
        if(request->hasParam("port") && request->getParam("port")->value().length() > 0) {
            prefs.putInt("port", request->getParam("port")->value().toInt());
        }
        if(request->hasParam("token") && request->getParam("token")->value().length() > 0) {
            prefs.putString("token", request->getParam("token")->value());
        }

        prefs.end();
        
        request->send(200, "text/plain", "OK");
        Serial.println("[Web] Đã cập nhật cấu hình mới! Hệ thống sẽ Reset sau 1 giây...");

        xTaskCreate([](void *param){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP.restart();
        }, "RebootTask", 2048, NULL, 1, NULL);
    });

    server.begin();
    Serial.println("[Web Server] Đang chạy ngầm...");
}