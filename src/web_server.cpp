#include "web_server.h"

// --- Giao diện HTML/CSS/JS ---
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="vi">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Bảng Điều Khiển IoT</title>
  <style>
    body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; background-color: #f4f7f6; color: #333; text-align: center; margin: 0; padding: 20px; }
    h1 { color: #2c3e50; }
    .card-container { display: flex; flex-wrap: wrap; justify-content: center; gap: 20px; margin-bottom: 30px; }
    .card { background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); width: 150px; }
    .card h3 { margin-top: 0; color: #7f8c8d; font-size: 1.2rem; }
    .card .value { font-size: 2rem; font-weight: bold; color: #3498db; }
    .btn { padding: 15px 30px; font-size: 1.2rem; border: none; border-radius: 5px; cursor: pointer; color: white; transition: 0.3s; margin: 10px; font-weight: bold;}
    .btn-on { background-color: #2ecc71; }
    .btn-on:hover { background-color: #27ae60; }
    .btn-off { background-color: #e74c3c; }
    .btn-off:hover { background-color: #c0392b; }
    .control-group { margin-top: 20px; background: white; padding: 20px; border-radius: 10px; display: inline-block;}
  </style>
</head>
<body>
  <h1>TEST</h1>
  
  <div class="card-container">
    <div class="card">
      <h3>Nhiệt độ</h3>
      <div class="value" id="temp">-- &deg;C</div>
    </div>
    <div class="card">
      <h3>Độ ẩm không khí</h3>
      <div class="value" id="hum">-- %</div>
    </div>
    <div class="card">
      <h3>Độ ẩm đất</h3>
      <div class="value" id="soil">-- %</div>
    </div>
  </div>

  <div class="control-group">
    <h2>Điều Khiển Thiết Bị</h2>
    <div>
      <span style="font-size: 1.2rem; font-weight: bold; margin-right: 15px;">Máy Bơm Nước:</span>
      <button class="btn btn-on" onclick="sendCmd(1, 1)">BẬT</button>
      <button class="btn btn-off" onclick="sendCmd(1, 0)">TẮT</button>
    </div>
    <div style="margin-top: 15px;">
      <span style="font-size: 1.2rem; font-weight: bold; margin-right: 15px;">Quạt Thông Gió:</span>
      <button class="btn btn-on" onclick="sendCmd(2, 1)">BẬT</button>
      <button class="btn btn-off" onclick="sendCmd(2, 0)">TẮT</button>
    </div>
  </div>

  <script>
    // Tự động lấy dữ liệu cảm biến mỗi 2 giây
    setInterval(function() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp').innerHTML = data.t + ' &deg;C';
          document.getElementById('hum').innerHTML = data.h + ' %';
          document.getElementById('soil').innerHTML = data.s + ' %';
        });
    }, 5000);

    // Hàm gửi lệnh điều khiển Relay
    function sendCmd(device, state) {
      fetch(`/control?dev=${device}&state=${state}`)
        .then(response => {
          if(!response.ok) alert("Lỗi khi gửi lệnh!");
        });
    }
  </script>
</body>
</html>
)rawliteral";

// --- Logic khởi tạo Web Server ---
void initWebServer(QueueHandle_t sensorQueue, QueueHandle_t relayQueue) {
  // Cấp phát động AsyncWebServer (Không dùng biến toàn cục)
  AsyncWebServer* server = new AsyncWebServer(80);

  // 1. Route trang chủ: Trả về file HTML
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  // 2. Route lấy dữ liệu cảm biến (Web gọi mỗi 5 giây)
  // Dùng [sensorQueue] để "bắt" con trỏ Queue vào trong hàm nặc danh (Lambda)
  server->on("/data", HTTP_GET, [sensorQueue](AsyncWebServerRequest *request){
    SensorData currentData = {0, 0, 0};
    
    // Dùng xQueuePeek để XEM dữ liệu mới nhất mà KHÔNG LẤY MẤT của MQTT
    if (sensorQueue != NULL) {
      xQueuePeek(sensorQueue, &currentData, 0); 
    }

    // Đóng gói thành JSON
    String json = "{\"t\":" + String(currentData.temperature) + 
                  ",\"h\":" + String(currentData.humidity) + 
                  ",\"s\":" + String(currentData.soilMoisture) + "}";
    
    request->send(200, "application/json", json);
  });

  // 3. Route nhận lệnh điều khiển Relay từ Web
  server->on("/control", HTTP_GET, [relayQueue](AsyncWebServerRequest *request){
    if (request->hasParam("dev") && request->hasParam("state")) {
      RelayCommand cmd;
      cmd.deviceId = request->getParam("dev")->value().toInt();
      cmd.state = request->getParam("state")->value().toInt();

      // Đẩy lệnh vào Queue để Task Relay (sẽ viết sau) xử lý
      if (relayQueue != NULL) {
        xQueueSend(relayQueue, &cmd, 0);
        Serial.printf("[Web] Đã nhận lệnh: Thiết bị %d -> Trạng thái %d\n", cmd.deviceId, cmd.state);
      }
      request->send(200, "text/plain", "OK");
    } else {
      request->send(400, "text/plain", "Bad Request");
    }
  });

  // Khởi động server
  server->begin();
  Serial.println("Web Server đã khởi động trên cổng 80!");
}