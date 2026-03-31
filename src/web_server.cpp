#include "web_server.h"
#include <LittleFS.h>
#include <Preferences.h>

void initWebServer(SystemContext *ctx) {
    
    // Xóa biến Global AsyncWebServer, khởi tạo bằng con trỏ động
    AsyncWebServer *server = new AsyncWebServer(80);

    if(!LittleFS.begin(true)){
        Serial.println("[Web Server] Lỗi: Không thể mount LittleFS");
        return;
    }

    // Đọc Rule Tự động từ Flash và Cập nhật Context (Có dùng Mutex)
    Preferences pAuto;
    pAuto.begin("iot_auto", true);
    if (pAuto.getBytesLength("auto_cfg") == sizeof(AutoConfig)) {
        if (xSemaphoreTake(ctx->autoMutex, portMAX_DELAY) == pdTRUE) {
            pAuto.getBytes("auto_cfg", &ctx->autoCfg, sizeof(AutoConfig));
            xSemaphoreGive(ctx->autoMutex);
        }
    }
    pAuto.end();

    server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/index.html", "text/html"); 
    });

    server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(LittleFS, "/script.js", "text/javascript"); 
    });

    // API NHẬN LUẬT TỪ GIAO DIỆN WEB GỬI XUỐNG
    server->on("/save_auto", HTTP_GET, [ctx](AsyncWebServerRequest *request){
        // Mở Mutex Khóa biến AutoConfig để ghi dữ liệu mới vào
        if (xSemaphoreTake(ctx->autoMutex, portMAX_DELAY) == pdTRUE) {
            if(request->hasParam("auto")) ctx->autoCfg.isAuto = request->getParam("auto")->value().toInt();
            if(request->hasParam("r1")) ctx->autoCfg.r1_en = request->getParam("r1")->value().toInt();
            if(request->hasParam("sL")) ctx->autoCfg.soil_low = request->getParam("sL")->value().toFloat();
            if(request->hasParam("r2")) ctx->autoCfg.r2_en = request->getParam("r2")->value().toInt();
            if(request->hasParam("sH")) ctx->autoCfg.soil_high = request->getParam("sH")->value().toFloat();
            if(request->hasParam("r3")) ctx->autoCfg.r3_en = request->getParam("r3")->value().toInt();
            if(request->hasParam("tH")) ctx->autoCfg.temp_high = request->getParam("tH")->value().toFloat();
            if(request->hasParam("r4")) ctx->autoCfg.r4_en = request->getParam("r4")->value().toInt();
            if(request->hasParam("tL")) ctx->autoCfg.temp_low = request->getParam("tL")->value().toFloat();

            // Lưu Flash để mất điện không mất quy tắc
            Preferences prefs;
            prefs.begin("iot_auto", false);
            prefs.putBytes("auto_cfg", &ctx->autoCfg, sizeof(AutoConfig));
            prefs.end();

            // Nhả Mutex
            xSemaphoreGive(ctx->autoMutex);
        }
        request->send(200, "text/plain", "OK");
    });

    server->on("/data", HTTP_GET, [ctx](AsyncWebServerRequest *request){
        SensorData data;
        if (xQueuePeek(ctx->sensorQueue, &data, 0) == pdPASS) {
            char jsonBuffer[100];
            snprintf(jsonBuffer, sizeof(jsonBuffer), "{\"t\":%.1f, \"h\":%.1f, \"s\":%.1f}", 
                     data.temperature, data.humidity, data.soilMoisture);
            request->send(200, "application/json", jsonBuffer);
        } else {
            request->send(503, "application/json", "{\"error\":\"No data\"}");
        }
    });

    server->on("/relay", HTTP_GET, [ctx](AsyncWebServerRequest *request){
        if (request->hasParam("id") && request->hasParam("state")) {
            RelayCommand cmd;
            cmd.deviceId = request->getParam("id")->value().toInt();
            cmd.state = request->getParam("state")->value().toInt();
            xQueueSend(ctx->relayQueue, &cmd, 0);
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "Bad Request");
        }
    });

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
        xTaskCreate([](void *param){
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            ESP.restart();
        }, "RebootTask", 2048, NULL, 1, NULL);
    });

    server->begin(); // Dùng biến con trỏ ->
}