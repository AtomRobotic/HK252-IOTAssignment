#include "global.h"

SemaphoreHandle_t xBinarySemaphoreInternet;

SemaphoreHandle_t xSensorMutex;

QueueHandle_t xQueueSensor;
SemaphoreHandle_t xSemaphoreLed;
SemaphoreHandle_t xSemaphoreNeoLed;
SemaphoreHandle_t xSemaphoreLCD;

Mode currentMode = AUTO;
SensorData sensorData = {0.0, 0.0, 0.0};