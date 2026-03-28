#include "wifi_manager.h"

void setupWiFi() {
    const char* ap_ssid = "ESP32_Config_AP";
    const char* ap_pass = "12345678";  
    
    const char* sta_ssid = "Luqi.";     
    const char* sta_pass = "0611151004"; 

    // Bật chế độ Vừa phát (AP) vừa nhận (STA)
    WiFi.mode(WIFI_AP_STA);

    // 1. Khởi tạo mạng Access Point (AP) để chạy Web Server nội bộ
    Serial.println("\n--- Khởi tạo Access Point ---");
    WiFi.softAP(ap_ssid, ap_pass);
    Serial.print("Đã phát Wi-Fi! IP để vào Web (AP): ");
    Serial.println(WiFi.softAPIP());

    // 2. Kết nối vào mạng Station (STA) để chuẩn bị cho MQTT CoreIOT
    Serial.println("\n--- Bắt đầu kết nối Router (STA) ---");
    WiFi.begin(sta_ssid, sta_pass);

    // Dùng cơ chế Timeout 10 giây để không bị treo hệ thống nếu sai pass Wi-Fi
    unsigned long startAttemptTime = millis();
    const unsigned long timeout = 10000; 

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < timeout) {
        Serial.print(".");
        // Dùng delay của RTOS thay vì delay() thường để nhường CPU cho task khác
        vTaskDelay(500 / portTICK_PERIOD_MS); 
    }
    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("Kết nối Internet thành công! IP (STA): ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Kết nối Internet thất bại! ESP32 vẫn giữ AP mode để bạn xài Web.");
    }
}