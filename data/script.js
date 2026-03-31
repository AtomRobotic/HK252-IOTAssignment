function openTab(evt, tabId) {
    let tabContents = document.getElementsByClassName("tab-content");
    for (let i = 0; i < tabContents.length; i++) { tabContents[i].classList.remove("active"); }
    let tabBtns = document.getElementsByClassName("tab-btn");
    for (let i = 0; i < tabBtns.length; i++) { tabBtns[i].classList.remove("active"); }
    document.getElementById(tabId).classList.add("active");
    evt.currentTarget.classList.add("active");
}

let syncTimer;
function syncAutoConfigToServer() {
    clearTimeout(syncTimer); 
    syncTimer = setTimeout(() => {
        let isAuto = document.getElementById('btn-auto').classList.contains('active') ? 1 : 0;
        let r1 = document.getElementById('toggle-rule1').checked ? 1 : 0;
        let sL = document.getElementById('val-rule1').value;
        let r2 = document.getElementById('toggle-rule2').checked ? 1 : 0;
        let sH = document.getElementById('val-rule2').value;
        let r3 = document.getElementById('toggle-rule3').checked ? 1 : 0;
        let tH = document.getElementById('val-rule3').value;
        let r4 = document.getElementById('toggle-rule4').checked ? 1 : 0;
        let tL = document.getElementById('val-rule4').value;

        let url = `/save_auto?auto=${isAuto}&r1=${r1}&sL=${sL}&r2=${r2}&sH=${sH}&r3=${r3}&tH=${tH}&r4=${r4}&tL=${tL}`;
        fetch(url).then(res => console.log("Đã đồng bộ Rule xuống ESP32!"));
    }, 500); 
}

function setMode(mode) {
    document.getElementById('btn-manual').classList.remove('active');
    document.getElementById('btn-auto').classList.remove('active');
    let manualControls = document.getElementById('manual-controls');

    if (mode === 'manual') {
        document.getElementById('btn-manual').classList.add('active');
        manualControls.classList.remove('disabled'); 
    } else {
        document.getElementById('btn-auto').classList.add('active');
        manualControls.classList.add('disabled'); 
    }
    syncAutoConfigToServer();
}

function toggleRule(ruleId) {
    let isChecked = document.getElementById('toggle-' + ruleId).checked;
    let card = document.getElementById('card-' + ruleId);
    if(isChecked) card.classList.add('active'); else card.classList.remove('active');
    syncAutoConfigToServer(); 
}

function updateSummary(ruleId) {
    let sensorVal = document.getElementById('sensor-' + ruleId).value;
    let condVal = document.getElementById('cond-' + ruleId).value;
    let numVal = document.getElementById('val-' + ruleId).value;
    let actionVal = document.getElementById('action-' + ruleId).value;

    let sensorNames = { 'soil': 'Độ ẩm đất', 'temp': 'Nhiệt độ', 'hum': 'Độ ẩm không khí' };
    let sensorUnits = { 'soil': '%', 'temp': '°C', 'hum': '%' };
    let sensorIcons = { 'soil': '🌱', 'temp': '🌡️', 'hum': '💧' };

    document.getElementById('icon-' + ruleId).innerText = sensorIcons[sensorVal];
    document.getElementById('s-' + ruleId).innerText = sensorNames[sensorVal];
    document.getElementById('c-' + ruleId).innerText = condVal + " " + numVal;
    document.getElementById('u-' + ruleId).innerText = sensorUnits[sensorVal];
    document.getElementById('a-' + ruleId).innerText = actionVal;
    
    document.getElementById('title-' + ruleId).innerText = "Tự động " + actionVal.toLowerCase() + " theo " + sensorNames[sensorVal].toLowerCase();
    syncAutoConfigToServer(); 
}

function sendCmd(deviceId, state) {
    fetch(`/relay?id=${deviceId}&state=${state}`).then(response => { if(response.ok) console.log(`Gửi lệnh OK`); });
}

setInterval(function() {
    fetch('/data').then(res => res.json()).then(data => {
        let t = parseFloat(data.t); let h = parseFloat(data.h); let s = parseFloat(data.s);
        
        document.getElementById('temp').innerHTML = t; document.getElementById('hum').innerHTML = h; document.getElementById('soil').innerHTML = s;
        document.getElementById('temp-progress').style.width = Math.min((t / 50 * 100), 100) + "%";
        document.getElementById('hum-progress').style.width = Math.min(h, 100) + "%";
        document.getElementById('soil-progress').style.width = Math.min(s, 100) + "%";

        let tempBadge = document.getElementById('temp-status');
        if (t < 18 || t > 29) { tempBadge.innerHTML = '⚠️ Cảnh báo'; tempBadge.className = 'status-badge warning'; } else { tempBadge.innerHTML = '✓ Tốt'; tempBadge.className = 'status-badge'; }
        let humBadge = document.getElementById('hum-status');
        if (h < 60 || h > 80) { humBadge.innerHTML = '⚠️ Cảnh báo'; humBadge.className = 'status-badge warning'; } else { humBadge.innerHTML = '✓ Tốt'; humBadge.className = 'status-badge'; }
        let soilBadge = document.getElementById('soil-status');
        if (s < 40 || s > 60) { soilBadge.innerHTML = '⚠️ Cảnh báo'; soilBadge.className = 'status-badge warning'; } else { soilBadge.innerHTML = '✓ Tốt'; soilBadge.className = 'status-badge'; }

        let aiScore = 100; let aiStatus = "✨ Rất tốt"; let aiStatusClass = "";
        let aiRec = "Môi trường hiện tại đang cực kỳ lý tưởng. Cây phát triển khỏe mạnh, hãy tiếp tục duy trì!";
        if (s < 40) { aiScore -= 30; aiStatus = "💧 Thiếu nước"; aiStatusClass = "danger"; aiRec = "Đất quá khô (" + s + "%). Hệ thống bơm sẽ xử lý nếu đang bật Tự động."; } 
        else if (s > 60) { aiScore -= 15; aiStatus = "💦 Đất quá ẩm"; aiStatusClass = "warning"; aiRec = "Dư thừa nước (" + s + "%). Nguy cơ thối rễ."; }
        if (t > 29) { aiScore -= 25; if(aiStatusClass !== "danger") aiStatusClass = "danger"; aiStatus = "🔥 Quá nóng"; aiRec = "Nhiệt độ " + t + "°C. Hệ thống quạt sẽ xử lý nếu đang bật Tự động."; } 
        else if (t < 18) { aiScore -= 10; if(aiStatusClass === "") aiStatusClass = "warning"; aiStatus = "❄️ Hơi lạnh"; aiRec = "Nhiệt độ hơi thấp (" + t + "°C). Cần theo dõi thêm."; }
        if (aiScore < 0) aiScore = 0;

        document.getElementById('ai-score').innerHTML = aiScore + "%";
        let aiStatusEl = document.getElementById('ai-status');
        aiStatusEl.innerHTML = aiStatus; aiStatusEl.className = "ai-status " + aiStatusClass;
        document.getElementById('ai-text').innerHTML = aiRec;
    }).catch(e => console.log(e));
}, 5000);

function saveNetworkConfig() {
    const ssid = document.getElementById('ssid').value;
    const pass = document.getElementById('pass').value;
    const server = document.getElementById('mqtt_server').value;
    const port = document.getElementById('mqtt_port').value;
    const token = document.getElementById('mqtt_token').value;

    if(!ssid && !pass && !server && !token) {
        if(!confirm("Bạn đang để trống thông tin. Mạch sẽ khởi động lại với cấu hình cũ. Tiếp tục?")) return;
    }
    const url = `/config?ssid=${encodeURIComponent(ssid)}&pass=${encodeURIComponent(pass)}&server=${encodeURIComponent(server)}&port=${port}&token=${encodeURIComponent(token)}`;
    fetch(url).then(response => {
        if(response.ok) {
            alert("Đã lưu thành công! ESP32 đang khởi động lại...");
            setTimeout(() => { window.location.reload(); }, 5000);
        } else alert("Lỗi khi lưu cấu hình!");
    }).catch(error => alert("Lỗi kết nối tới mạch ESP32!"));
}

window.onload = function() {
    updateSummary('rule1'); updateSummary('rule2'); updateSummary('rule3'); updateSummary('rule4');
    
    fetch('/get_config').then(res => res.json()).then(data => {
        if(data.ssid) document.getElementById('ssid').placeholder = "Đang dùng: " + data.ssid;
        if(data.server) document.getElementById('mqtt_server').value = data.server;
        if(data.port) document.getElementById('mqtt_port').value = data.port;
        if(data.token) document.getElementById('mqtt_token').value = data.token;
    }).catch(e => console.log("Chưa tải được cấu hình mạng"));
};