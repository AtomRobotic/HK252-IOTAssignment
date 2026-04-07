#ifndef INCLUDE_TASKLCD_H_
#define INCLUDE_TASKLCD_H_

#include "common.h"

// Task
extern void TaskLCD(void *pvParameters);

// Optional Functions
extern void displayTemperatureHumidity(LiquidCrystal_I2C &lcd);
extern void displayLux(LiquidCrystal_I2C &lcd);

#endif /* INCLUDE_TASKLCD_H_ */