function openTab(evt, tabId) {
    let tabContents = document.getElementsByClassName("tab-content");
    for (let i = 0; i < tabContents.length; i++) { tabContents[i].classList.remove("active"); }
    let tabBtns = document.getElementsByClassName("tab-btn");
    for (let i = 0; i < tabBtns.length; i++) { tabBtns[i].classList.remove("active"); }
    document.getElementById(tabId).classList.add("active");
    evt.currentTarget.classList.add("active");
}

// Logic lấy dữ liệu từ ESP32 mỗi 5 giây
setInterval(function() {
    fetch('/data')
        .then(res => res.json())
        .then(data => {
            let t = parseFloat(data.t); 
            let h = parseFloat(data.h); 
            
            // Cập nhật số liệu hiển thị
            document.getElementById('temp').innerHTML = t.toFixed(1); 
            document.getElementById('hum').innerHTML = h.toFixed(1); 

            // Cập nhật thanh Progress
            document.getElementById('temp-progress').style.width = Math.min((t / 50 * 100), 100) + "%";
            document.getElementById('hum-progress').style.width = Math.min(h, 100) + "%";

            // Logic cảnh báo nhiệt độ
            let tempBadge = document.getElementById('temp-status');
            if (t >= 20 && t <= 30) {
                tempBadge.innerHTML = '✓ Normal'; 
                tempBadge.className = 'status-badge';
            } else if ((t >= 15 && t < 20) || (t > 30 && t <= 35)) {
                tempBadge.innerHTML = '⚠️ Warning'; 
                tempBadge.className = 'status-badge warning';
            } else {
                tempBadge.innerHTML = '🚨 Critical'; 
                tempBadge.className = 'status-badge warning'; 
                tempBadge.style.color = '#f5222d'; 
                tempBadge.style.borderColor = '#ffa39e';
                tempBadge.style.backgroundColor = '#fff1f0';
            }

            // Logic cảnh báo độ ẩm
            let humBadge = document.getElementById('hum-status');
            if (h >= 50 && h <= 80) {
                humBadge.innerHTML = '✓ Normal'; 
                humBadge.className = 'status-badge';
            } else if ((h >= 40 && h < 50) || (h > 80 && h <= 90)) {
                humBadge.innerHTML = '⚠️ Warning'; 
                humBadge.className = 'status-badge warning';
            } else {
                humBadge.innerHTML = '🚨 Critical'; 
                humBadge.className = 'status-badge warning';
                humBadge.style.color = '#f5222d'; 
                humBadge.style.borderColor = '#ffa39e';
                humBadge.style.backgroundColor = '#fff1f0';
            }

        })
        .catch(e => console.log("Lỗi lấy dữ liệu:", e));
}, 5000);

// Nút gạt chế độ (UI tạm)
function setMode(mode) {
    document.getElementById('btn-manual').classList.toggle('active', mode === 'manual');
    document.getElementById('btn-auto').classList.toggle('active', mode === 'auto');
    console.log("Đã chuyển chế độ:", mode);
}

function sendCmd(deviceId, state) {
    fetch(`/relay?id=${deviceId}&state=${state}`).then(response => { if(response.ok) console.log(`Gửi lệnh OK`); });
}

// --- HÀM LƯU CẤU HÌNH XUỐNG ESP32 ---
function saveNetworkConfig() {
    const ssid = document.getElementById('ssid').value;
    const pass = document.getElementById('pass').value;
    const server = document.getElementById('mqtt_server').value;
    const port = document.getElementById('mqtt_port').value;
    const token = document.getElementById('mqtt_token').value;

    if(!ssid && !pass && !server && !token) {
        if(!confirm("Bạn đang để trống thông tin. Mạch sẽ khởi động lại với cấu hình cũ. Tiếp tục?")) return;
    }
    
    // Gửi URL chứa Query Params xuống ESP32
    const url = `/config?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}&server=${encodeURIComponent(server)}&port=${port}&token=${encodeURIComponent(token)}`;
    fetch(url).then(response => {
        if(response.ok) {
            alert("Đã lưu thành công! ESP32 đang khởi động lại...");
            setTimeout(() => { window.location.reload(); }, 5000);
        } else {
            alert("Lỗi khi lưu cấu hình!");
        }
    }).catch(error => alert("Lỗi kết nối tới mạch ESP32!"));
}

// Hàm chuyển đổi chế độ Auto / Manual
function setMode(mode) {
    // 1. Đổi màu nút bấm UI
    document.getElementById('btn-manual').classList.toggle('active', mode === 'manual');
    document.getElementById('btn-auto').classList.toggle('active', mode === 'auto');
    
    // 2. Làm mờ hoặc Mở khóa khu vực nút bấm thủ công
    let manualControls = document.getElementById('manual-controls');
    if (mode === 'manual') {
        manualControls.classList.remove('disabled');
    } else {
        manualControls.classList.add('disabled');
    }
    
    // 3. Gửi tín hiệu xuống ESP32 (0 = AUTO, 1 = MANUAL theo enum Mode trong C++)
    let modeValue = (mode === 'auto') ? 0 : 1;
    fetch(`/set_mode?mode=${modeValue}`)
        .then(response => {
            if(response.ok) console.log("Đã chuyển sang chế độ:", mode.toUpperCase());
        })
        .catch(e => console.error("Lỗi chuyển chế độ", e));
}

// Hàm gửi lệnh Bật/Tắt thiết bị
function sendCmd(deviceId, state) {
    fetch(`/relay?id=${deviceId}&state=${state}`)
        .then(response => { 
            if(response.ok) {
                console.log(`Đã gửi lệnh điều khiển: Thiết bị ${deviceId} -> ${state ? 'BẬT' : 'TẮT'}`); 
            }
        })
        .catch(e => console.error("Lỗi điều khiển", e));
}

// --- TỰ ĐỘNG TẢI CẤU HÌNH HIỆN TẠI KHI MỞ TRANG ---
window.onload = function() {
    fetch('/get_config')
        .then(response => response.json())
        .then(data => {
            if(data.ssid) {
                document.getElementById('ssid').placeholder = "Đang dùng: " + data.ssid;
            }
            if(data.server) document.getElementById('mqtt_server').value = data.server;
            if(data.port) document.getElementById('mqtt_port').value = data.port;
            if(data.token) document.getElementById('mqtt_token').value = data.token;
        })
        .catch(e => console.log("Chưa tải được cấu hình mạng"));
};