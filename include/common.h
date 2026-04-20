#ifndef __COMMON_H_
#define __COMMON_H_

/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LittleFS.h>
#include <DHT20.h>
#include <LiquidCrystal_I2C.h>
#include "global.h"
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

// #include <TensorFlowLite_ESP32.h>
// #include "tensorflow/lite/micro/all_ops_resolver.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
// #include "tensorflow/lite/schema/schema_generated.h"

/* Includes Devices ----------------------------------------------------------*/
#include "devices/TaskTempHumid.h"
#include "devices/TaskLED.h"
#include "devices/TaskLux.h"
#include "devices/TaskLCD.h"

/* Define --------------------------------------------------------------------*/

#define SDA_PIN 11
#define SCL_PIN 12
#define LED_PIN 48
#define LUX_PIN 1

#define NUM_PIXELS 1
#define PIN_NEO_PIXEL 45



/* Struct --------------------------------------------------------------------*/






#endif /* __COMMON_H_ */