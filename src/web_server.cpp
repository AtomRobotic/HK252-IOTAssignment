#include "web_server.h"
#include <LittleFS.h>
#include <Preferences.h>
#include "ota_manager.h"

void initWebServer(void *pvParameters) {
  AppContext *ctx = (AppContext *)pvParameters;

  AsyncWebServer *server = new AsyncWebServer(80);

  initOTA(server);

  if (!LittleFS.begin(true)) {
    Serial.println("[Web Server] Lỗi: Không thể mount LittleFS. Đang thử định dạng lại...");
    LittleFS.format();
    if (!LittleFS.begin()) {
        Serial.println("[Web Server] Lỗi nghiêm trọng: Không thể định dạng LittleFS");
        return;
    }
    Serial.println("[Web Server] Định dạng LittleFS thành công.");
  }

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/index.html", "text/html");
  });

  server->on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(LittleFS, "/script.js", "text/javascript");
  });

  // API LẤY DỮ LIỆU CẢM BIẾN
  server->on("/data", HTTP_GET, [ctx](AsyncWebServerRequest *request) {
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

  // =========================================================
  // API CHUYỂN CHẾ ĐỘ HOẠT ĐỘNG (AUTO / MANUAL)
  // =========================================================
  server->on("/set_mode", HTTP_GET, [ctx](AsyncWebServerRequest *request) {
    if (request->hasParam("mode")) {
      int modeVal = request->getParam("mode")->value().toInt();

      // 0 = AUTO, 1 = MANUAL
      ctx->currentMode = (modeVal == 0) ? AUTO : MANUAL;

      // Tính năng an toàn: Khi chuyển chế độ, tạm TẮT mọi thiết bị
      digitalWrite(PUMP_PIN, LOW);
      ledcWrite(0, 0); // Kênh 0 là Quạt

      Serial.printf("[Web] Chuyển chế độ: %s\n",
                    (ctx->currentMode == AUTO) ? "AUTO (TinyML)" : "MANUAL");
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // =========================================================
  // API ĐIỀU KHIỂN THỦ CÔNG TỪ WEB (CHỈ CHẠY KHI Ở MANUAL)
  // =========================================================
  server->on("/relay", HTTP_GET, [ctx](AsyncWebServerRequest *request) {
    if (request->hasParam("id") && request->hasParam("state")) {
      int deviceId = request->getParam("id")->value().toInt();
      int state = request->getParam("state")->value().toInt();

      if (ctx->currentMode == MANUAL) {
        if (deviceId == 1) { // 1 = Máy bơm
          digitalWrite(PUMP_PIN, state ? HIGH : LOW);
          Serial.printf("[Web MANUAL] Máy Bơm -> %s\n", state ? "BẬT" : "TẮT");
        } else if (deviceId == 2) {      // 2 = Quạt
          ledcWrite(0, state ? 255 : 0); // Quạt dùng PWM
          Serial.printf("[Web MANUAL] Quạt -> %s\n", state ? "BẬT" : "TẮT");
        }
        request->send(200, "text/plain", "OK");
      } else {
        request->send(403, "text/plain", "Loi: He thong dang o che do AUTO");
      }
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // CẤU HÌNH WI-FI & MQTT
  server->on("/get_config", HTTP_GET, [](AsyncWebServerRequest *request) {
    Preferences prefs;
    prefs.begin("iot_config", true);
    String ssid = prefs.getString("ssid", "");
    String server = prefs.getString("server", "");
    int port = prefs.getInt("port", 1883);
    String token = prefs.getString("token", "");
    prefs.end();

    char jsonBuffer[256];
    snprintf(
        jsonBuffer, sizeof(jsonBuffer),
        "{\"ssid\":\"%s\", \"server\":\"%s\", \"port\":%d, \"token\":\"%s\"}",
        ssid.c_str(), server.c_str(), port, token.c_str());
    request->send(200, "application/json", jsonBuffer);
  });

  server->on("/config", HTTP_GET, [](AsyncWebServerRequest *request) {
    Preferences prefs;
    prefs.begin("iot_config", false);
    if (request->hasParam("ssid") &&
        request->getParam("ssid")->value().length() > 0)
      prefs.putString("ssid", request->getParam("ssid")->value());
    if (request->hasParam("pass") &&
        request->getParam("pass")->value().length() > 0)
      prefs.putString("pass", request->getParam("pass")->value());
    if (request->hasParam("server") &&
        request->getParam("server")->value().length() > 0)
      prefs.putString("server", request->getParam("server")->value());
    if (request->hasParam("port") &&
        request->getParam("port")->value().length() > 0)
      prefs.putInt("port", request->getParam("port")->value().toInt());
    if (request->hasParam("token") &&
        request->getParam("token")->value().length() > 0)
      prefs.putString("token", request->getParam("token")->value());
    prefs.end();

    request->send(200, "text/plain", "OK");
    xTaskCreate(
        [](void *param) {
          vTaskDelay(1000 / portTICK_PERIOD_MS);
          ESP.restart();
        },
        "RebootTask", 2048, NULL, 1, NULL);
  });

  server->begin();
}