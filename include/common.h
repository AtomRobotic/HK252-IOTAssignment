#ifndef __COMMON_H_
#define __COMMON_H_

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LittleFS.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>
#include <WiFi.h>              
#include <AsyncTCP.h>           
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ThingsBoard.h>
#include <Adafruit_NeoPixel.h>

// #include <TensorFlowLite_ESP32.h>
// #include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
// #include "tensorflow/lite/schema/schema_generated.h"

/* Define --------------------------------------------------------------------*/






/* Struct --------------------------------------------------------------------*/
typedef struct {
    float temperature;
    float humidity;
    float soilMoisture;
} SensorData;

typedef struct {
    uint8_t deviceId; // 1: Máy bơm, 2: Quạt
    uint8_t state;    // 0: Tắt, 1: Bật
} RelayCommand;

// --- CẤU TRÚC LƯU QUY TẮC TỰ ĐỘNG ---
typedef struct {
    bool isAuto;          
    bool r1_en; float soil_low;   
    bool r2_en; float soil_high;  
    bool r3_en; float temp_high;  
    bool r4_en; float temp_low;   
} AutoConfig;

// --- "CÁI TÚI" CHỨA TẤT CẢ DỮ LIỆU ĐỂ TRUYỀN QUA TASK ---
typedef struct {
    QueueHandle_t sensorQueue;
    QueueHandle_t relayQueue;
    SemaphoreHandle_t autoMutex; 
    AutoConfig autoCfg;          
    Adafruit_NeoPixel *led;
} SystemContext;


#endif /* __COMMON_H_ */