#ifndef __COMMON_H_
#define __COMMON_H_

/* 1. Core definitions */
#include <Arduino.h>
// #if !defined(Serial)
//   #include <HardwareSerial.h>
// #endif

/* 2. Project Globals (Crucial for Task definitions) */
#include "global.h"

/* 3. System Libraries */
#include <Wire.h>
#include <SPI.h>
#include <LittleFS.h>
#include <DHT20.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>

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

#include <TensorFlowLite_ESP32.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/system_setup.h"

/* 4. Logic & TinyML */
#include "tinyml.h"

/* 5. Device Tasks */
#include "devices/TaskTempHumid.h"
#include "devices/TaskLED.h"
#include "devices/TaskLux.h"
#include "devices/TaskLCD.h"
#include "devices/TaskNEO.h"
#include "devices/TaskFan.h"
#include "devices/TaskSoilMoisture.h"
#include "devices/TaskPump.h"

/* 6. Hardware Pins */
#define SDA_PIN 11
#define SCL_PIN 12
#define LED_PIN 48
#define LUX_PIN 1
#define FAN_PIN 6
#define SOIL_MOISTURE_PIN 3
#define PUMP_PIN 8

#define NUM_PIXELS 1
#define PIN_NEO_PIXEL 45

#endif /* __COMMON_H_ */