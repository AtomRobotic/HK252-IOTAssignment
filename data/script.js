// Tự động lấy dữ liệu cảm biến mỗi 5 giây
setInterval(function() {
    fetch('/data')
        .then(response => response.json())
        .then(data => {
            document.getElementById('temp').innerHTML = data.t;
            document.getElementById('hum').innerHTML = data.h;
            document.getElementById('soil').innerHTML = data.s;
        });
}, 5000);

// Hàm gửi lệnh Bật/Tắt thiết bị (Relay/Quạt)
function sendCmd(deviceId, state) {
    fetch(`/relay?id=${deviceId}&state=${state}`)
        .then(response => {
            if(response.ok) {
                console.log(`Đã gửi lệnh: Thiết bị ${deviceId} -> Trạng thái ${state}`);
            }
        });
}

// --- TỰ ĐỘNG TẢI CẤU HÌNH HIỆN TẠI KHI MỞ TRANG ---
window.onload = function() {
    fetch('/get_config')
        .then(response => response.json())
        .then(data => {
            // Thay đổi Placeholder (chữ mờ) để hiển thị Wi-Fi đang kết nối
            if(data.ssid) {
                document.getElementById('ssid').placeholder = "Đang dùng: " + data.ssid;
            }
            
            // Tự động điền các thông số MQTT cũ vào ô nhập liệu cho tiện
            if(data.server) document.getElementById('mqtt_server').value = data.server;
            if(data.port) document.getElementById('mqtt_port').value = data.port;
            if(data.token) document.getElementById('mqtt_token').value = data.token;
        });
};

// --- HÀM LƯU CẤU HÌNH (Đã bỏ yêu cầu bắt buộc điền) ---
function saveConfig() {
    const ssid = document.getElementById('ssid').value;
    const pass = document.getElementById('pass').value;
    const server = document.getElementById('mqtt_server').value;
    const port = document.getElementById('mqtt_port').value;
    const token = document.getElementById('mqtt_token').value;

    // Chỉ cảnh báo nhẹ, vẫn cho phép gửi đi (các trường trống ESP32 sẽ tự bỏ qua)
    if(!ssid && !pass && !server && !token) {
        if(!confirm("Bạn đang để trống tất cả thông tin. Mạch sẽ khởi động lại với cấu hình cũ. Tiếp tục?")) return;
    }

    // Gửi Request chứa thông tin cấu hình lên ESP32
    fetch(`/config?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}&server=${encodeURIComponent(server)}&port=${port}&token=${encodeURIComponent(token)}`)
        .then(response => {
            if(response.ok) {
                alert("Đã lưu! ESP32 đang khởi động lại...");
                setTimeout(() => { window.location.reload(); }, 5000);
            } else {
                alert("Lỗi khi lưu cấu hình!");
            }
        });
}