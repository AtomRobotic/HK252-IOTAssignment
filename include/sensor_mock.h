#ifndef SENSOR_MOCK_H
#define SENSOR_MOCK_H

#include "common.h"

// Hàm dùng để khởi tạo Task đọc cảm biến (giả lập)
void initMockSensorTask(QueueHandle_t dataQueue);

#endif