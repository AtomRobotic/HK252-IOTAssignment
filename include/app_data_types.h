#ifndef APP_DATA_TYPES_H
#define APP_DATA_TYPES_H
#include <stdint.h>

typedef enum
{
    STATE_NORMAL = 0,
    STATE_WARNING = 1,
    STATE_CRITICAL = 2
} SystemState_t;

typedef struct
{
    float temperature;
    float humidity;
    float soil_humidity;
    float light;
    uint32_t timestamp;
} SensorData_t;

typedef struct
{
    int device_id;
    bool state;
} ControlMsg_t;

#endif