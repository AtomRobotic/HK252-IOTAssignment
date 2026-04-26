#include "common.h"

void initAppContext(AppContext *appContext){
    // Semaphores
    appContext->xQueueSensor = xQueueCreate(1, sizeof(SensorData));

    appContext->xSemaphoreLed = xSemaphoreCreateBinary();
    appContext->xSemaphoreNeoLed = xSemaphoreCreateBinary();
    appContext->xSemaphoreLCD = xSemaphoreCreateBinary();
    appContext->xSemaphoreFan = xSemaphoreCreateBinary();
    appContext->xSemaphorePump = xSemaphoreCreateBinary();
    appContext->xBinarySemaphoreInternet = xSemaphoreCreateBinary();

    // Sensor data
    appContext->sensorData.temperature = 0.0f;
    appContext->sensorData.humidity = 0.0f;

    // Mode
    appContext->currentMode = AUTO;

    appContext->ml_predicted_state = 0;

}



