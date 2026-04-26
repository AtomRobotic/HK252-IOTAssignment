#include "web_server.h"
#include <LittleFS.h>
#include <Preferences.h>

void initWebServer(void *pvParameters) {
    AppContext *ctx = (AppContext *)pvParameters;

    // Xóa biến Global AsyncWebServer, khởi tạo bằng con trỏ động
    AsyncWebServer *server = new AsyncWebServer(80);

    if(!LittleFS.begin(true)){
        Serial.println("[Web Server] Lỗi: Không thể mount LittleFS");
        return;
    }

    // (No AutoConfig in AppContext) — keep web server simple and stateless here

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html"); 
    });

    server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/script.js", "text/javascript"); 
    });

    // API NHẬN LUẬT TỰ ĐỘNG TỪ GIAO DIỆN WEB — lưu từng khóa vào Preferences (không dùng AutoConfig)
    server->on("/save_auto", HTTP_GET, [](AsyncWebServerRequest *request){
        Preferences prefs;
        prefs.begin("iot_auto", false);
        if(request->hasParam("auto")) prefs.putInt("isAuto", request->getParam("auto")->value().toInt());
        if(request->hasParam("r1")) prefs.putInt("r1", request->getParam("r1")->value().toInt());
        if(request->hasParam("sL")) prefs.putString("sL", request->getParam("sL")->value());
        if(request->hasParam("r2")) prefs.putInt("r2", request->getParam("r2")->value().toInt());
        if(request->hasParam("sH")) prefs.putString("sH", request->getParam("sH")->value());
        if(request->hasParam("r3")) prefs.putInt("r3", request->getParam("r3")->value().toInt());
        if(request->hasParam("tH")) prefs.putString("tH", request->getParam("tH")->value());
        if(request->hasParam("r4")) prefs.putInt("r4", request->getParam("r4")->value().toInt());
        if(request->hasParam("tL")) prefs.putString("tL", request->getParam("tL")->value());
        prefs.end();
        request->send(200, "text/plain", "OK");
    });

    server->on("/data", HTTP_GET, [ctx](AsyncWebServerRequest *request){
        SensorData data;
        if (xQueuePeek(ctx->xQueueSensor, &data, 0) == pdPASS) {
            char jsonBuffer[100];
            snprintf(jsonBuffer, sizeof(jsonBuffer), "{\"t\":%.1f, \"h\":%.1f}", 
                     data.temperature, data.humidity);
            request->send(200, "application/json", jsonBuffer);
        } else {
            request->send(503, "application/json", "{\"error\":\"No data\"}");
        }
    });

    // Relay control removed — device uses internal semaphores/flags for control

    // Cấu hình Wi-Fi
    server->on("/get_config", HTTP_GET, [](AsyncWebServerRequest *request){
        Preferences prefs;
        prefs.begin("iot_config", true); 
        String ssid = prefs.getString("ssid", "");
        String server = prefs.getString("server", "");
        int port = prefs.getInt("port", 1883);
        String token = prefs.getString("token", "");
        prefs.end();

        char jsonBuffer[256];
        snprintf(jsonBuffer, sizeof(jsonBuffer), 
                 "{\"ssid\":\"%s\", \"server\":\"%s\", \"port\":%d, \"token\":\"%s\"}", 
                 ssid.c_str(), server.c_str(), port, token.c_str());
        request->send(200, "application/json", jsonBuffer);
    });

    server->on("/config", HTTP_GET, [](AsyncWebServerRequest *request){
        Preferences prefs;
        prefs.begin("iot_config", false); 
        if(request->hasParam("ssid") && request->getParam("ssid")->value().length() > 0) prefs.putString("ssid", request->getParam("ssid")->value());
        if(request->hasParam("pass") && request->getParam("pass")->value().length() > 0) prefs.putString("pass", request->getParam("pass")->value());
        if(request->hasParam("server") && request->getParam("server")->value().length() > 0) prefs.putString("server", request->getParam("server")->value());
        if(request->hasParam("port") && request->getParam("port")->value().length() > 0) prefs.putInt("port", request->getParam("port")->value().toInt());
        if(request->hasParam("token") && request->getParam("token")->value().length() > 0) prefs.putString("token", request->getParam("token")->value());
        prefs.end();
        
        request->send(200, "text/plain", "OK");
        // Restart device shortly after saving network config
        xTaskCreate([](void *param){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP.restart();
        }, "RebootTask", 2048, NULL, 1, NULL);
    });

    server->begin(); // Dùng biến con trỏ ->

    // Tạo task để xuất dữ liệu hiện tại từ AppContext vào LittleFS để UI có thể đọc
    xTaskCreatePinnedToCore([](void *pvParameters){
        AppContext *ctx = (AppContext *)pvParameters;
        while (1) {
            char buf[128];
            // Dùng trực tiếp dữ liệu trong AppContext (cập nhật bởi TaskTempHumid)
            snprintf(buf, sizeof(buf), "{\"t\":%.1f,\"h\":%.1f}", ctx->sensorData.temperature, ctx->sensorData.humidity);
            File f = LittleFS.open("/latest.json", "w");
            if (f) { f.print(buf); f.close(); }
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }
    }, "WebUITask", 4096, ctx, 1, NULL, 1);
}