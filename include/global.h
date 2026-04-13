#ifndef __GLOBAL_H
#define __GLOBAL_H

// Includes
#include "common.h"

// Global variables

typedef struct {
    float temperature;
    float humidity;
    //float lux;
} SensorData;
extern SensorData sensorData;

extern SemaphoreHandle_t xBinarySemaphoreInternet;

extern QueueHandle_t xQueueSensor;
extern SemaphoreHandle_t xSemaphoreLed;
extern SemaphoreHandle_t xSemaphoreNeoLed;
extern SemaphoreHandle_t xSemaphoreLCD;
extern SemaphoreHandle_t xSensorMutex;

typedef enum {
    AUTO = 0,
    MANUAL = 1
} Mode;

extern Mode currentMode;

typedef struct {
    QueueHandle_t xQueueSensor;
    SemaphoreHandle_t xSemaphoreLed;
    SemaphoreHandle_t xSemaphoreNeoLed;
    SemaphoreHandle_t xSemaphoreLCD;
    SemaphoreHandle_t xBinarySemaphoreInternet;

    SensorData sensorData;

} AppContext;

#endif /* GLOBAL_H */