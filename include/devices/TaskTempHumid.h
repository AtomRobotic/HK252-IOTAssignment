#ifndef INCLUDE_TASKTEMPHUMID_H_
#define INCLUDE_TASKTEMPHUMID_H_

#include "global.h"

//Task
extern void TaskTemperature_Humidity(void *pvParameters);

//Optional Functions 
extern void SerialPrintTemperature();
extern void SerialPrintgetHumidity();
extern float getTemperature();
extern float getHumidity();

#endif /* INCLUDE_TASKTEMPHUMID_H_ */