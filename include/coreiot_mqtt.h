#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include "common.h"

// Hàm khởi tạo Task MQTT, nhận vào con trỏ Queue chứa dữ liệu cảm biến
void initMQTTTask(QueueHandle_t sensorQueue);

#endif