#include "global.h"

SemaphoreHandle_t xBinarySemaphoreInternet;

SemaphoreHandle_t xSensorMutex;

QueueHandle_t xQueueSensor;
SemaphoreHandle_t xSemaphoreLed;
SemaphoreHandle_t xSemaphoreNeoLed;
SemaphoreHandle_t xSemaphoreLCD;
SemaphoreHandle_t xSemaphoreFan;
SemaphoreHandle_t xSemaphorePump;

Mode currentMode = AUTO;
int ml_predicted_state = 0;
SensorData sensorData = {0.0, 0.0, 0.0, 0.0};