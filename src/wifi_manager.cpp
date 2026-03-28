#include "wifi_manager.h"
#include <Preferences.h> // <--- Thêm thư viện này

void setupWiFi() {
    // Thông tin mạng phát ra để cấu hình (Cố định)
    const char* ap_ssid = "ESP32_Config_AP";
    const char* ap_pass = "12345678";  
    
    // ĐỌC THÔNG TIN WI-FI TỪ BỘ NHỚ FLASH
    Preferences prefs;
    prefs.begin("iot_config", false); // true = Chế độ Read-only
    String sta_ssid = prefs.getString("ssid", ""); 
    String sta_pass = prefs.getString("pass", "");
    prefs.end();

    WiFi.mode(WIFI_AP_STA);

    // 1. Luôn phát Wi-Fi AP để đề phòng trường hợp mất mạng còn có thể vào cấu hình lại
    Serial.println("\n--- Khởi tạo Access Point ---");
    WiFi.softAP(ap_ssid, ap_pass);
    Serial.print("IP Web Config: ");
    Serial.println(WiFi.softAPIP());

    // 2. Chỉ kết nối STA nếu người dùng đã từng lưu cấu hình Wi-Fi
    if (sta_ssid.length() > 0) {
        Serial.printf("\n[Wi-Fi] Đang kết nối tới: %s\n", sta_ssid.c_str());
        WiFi.begin(sta_ssid.c_str(), sta_pass.c_str());

        unsigned long startAttemptTime = millis();
        // Chờ tối đa 10s
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
            Serial.print(".");
            vTaskDelay(500 / portTICK_PERIOD_MS); 
        }
        Serial.println();

        if (WiFi.status() == WL_CONNECTED) {
            Serial.print("[Wi-Fi] Đã có Internet! IP: ");
            Serial.println(WiFi.localIP());
        } else {
            Serial.println("[Wi-Fi] Sai mật khẩu hoặc mất mạng! (Vào IP 192.168.4.1 để cấu hình lại)");
        }
    } else {
        Serial.println("[Wi-Fi] Chưa có cấu hình mạng! Đang chờ bạn nhập trên Web...");
    }
}