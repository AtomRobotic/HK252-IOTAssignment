#include "ota_manager.h"
#include <Arduino.h>

void initOTA(AsyncWebServer *server) {
    ElegantOTA.begin(server);    // Start ElegantOTA
}
