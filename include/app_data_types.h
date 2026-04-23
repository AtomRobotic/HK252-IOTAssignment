#ifndef APP_DATA_TYPES_H
#define APP_DATA_TYPES_H
#include <stdint.h>

typedef struct
{
    float temperature;
    float humidity;
    uint32_t timestamp;
} SensorData_t;

typedef struct
{
    int device_id;
    bool state;
} ControlMsg_t;

#endif