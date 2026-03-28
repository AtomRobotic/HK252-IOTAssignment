#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include "common.h"

// Hàm khởi tạo Web Server, nhận vào 2 con trỏ Queue
void initWebServer(QueueHandle_t sensorQueue, QueueHandle_t relayQueue);

#endif