#include "common.h"
#include "sensor_mock.h"
#include "web_server.h"
#include "wifi_manager.h"
#include "coreiot_mqtt.h"
#include <Adafruit_NeoPixel.h>
#include <nvs_flash.h>

extern void setupWiFi();

// TẠO STRUCT ĐỂ ĐÓNG GÓI LED VÀ QUEUE
struct RelayTaskConfig
{
  QueueHandle_t commandQueue;
  Adafruit_NeoPixel *led;
};


void relayControlTask(void *pvParameters)
{
  RelayTaskConfig *config = (RelayTaskConfig *)pvParameters;
  QueueHandle_t relayQueue = config->commandQueue;
  Adafruit_NeoPixel *pixels = config->led;

  RelayCommand cmd;

  while (1)
  {
    if (xQueueReceive(relayQueue, &cmd, portMAX_DELAY) == pdPASS)
    {
      Serial.printf("[Relay Task] Web yêu cầu: Thiết bị %d -> %s\n",
                    cmd.deviceId, cmd.state ? "BẬT" : "TẮT");

      if (cmd.deviceId == 2)
      {
        if (cmd.state == 1)
        {
          pixels->setPixelColor(0, pixels->Color(0, 0, 255)); 
          Serial.println("[Relay Task] Quạt BẬT -> LED Xanh Dương");
        }
        else
        {
          pixels->setPixelColor(0, pixels->Color(255, 0, 0));
          Serial.println("[Relay Task] Quạt TẮT -> LED Đỏ");
        }

        // --- BÍ QUYẾT CHỐNG NHIỄU TÍN HIỆU WI-FI ---
        // Ép mạch đẩy tín hiệu 5 lần liên tiếp.
        // Nếu xung Wi-Fi làm hỏng lần 1, các lần sau sẽ lọt qua được và đè màu lại chuẩn xác.
        for (int i = 0; i < 5; i++)
        {
          pixels->show();
          vTaskDelay(2 / portTICK_PERIOD_MS); // Nghỉ 2ms để nhường CPU
        }
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Hệ thống đang khởi động...");

  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS bị lỗi hoặc khác phiên bản -> Xóa đi và định dạng lại
    nvs_flash_erase();
    err = nvs_flash_init();
  }
  Serial.println("Đã khởi tạo phân vùng NVS thành công!");

  // 4. KHỞI TẠO LED
  Adafruit_NeoPixel *myLed = new Adafruit_NeoPixel(1, 48, NEO_GRB + NEO_KHZ800);
  myLed->begin();
  myLed->setBrightness(50);
  myLed->clear();
  myLed->show();

  setupWiFi();

  // Khởi tạo Queue
  QueueHandle_t sensorDataQueue = xQueueCreate(1, sizeof(SensorData));
  QueueHandle_t relayCommandQueue = xQueueCreate(5, sizeof(RelayCommand));

  if (sensorDataQueue == NULL || relayCommandQueue == NULL)
  {
    Serial.println("Lỗi RAM!");
    return;
  }

  // ĐÓNG GÓI VÀO STRUCT ĐỂ TRUYỀN CHO TASK
  RelayTaskConfig *relayConfig = new RelayTaskConfig();
  relayConfig->commandQueue = relayCommandQueue;
  relayConfig->led = myLed;

  initMockSensorTask(sensorDataQueue);
  initWebServer(sensorDataQueue, relayCommandQueue);
  initMQTTTask(sensorDataQueue);

  xTaskCreatePinnedToCore(
      relayControlTask,
      "RelayTask",
      4096,
      (void *)relayConfig,
      2,
      NULL,
      1);

  Serial.println("Hoàn tất Setup! RTOS đang điều phối các luồng...");
}

void loop()
{
  vTaskDelete(NULL);
}