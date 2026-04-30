#include "devices/TaskLCD.h"

void TaskLCD(void *pvParameters){
    LiquidCrystal_I2C lcd(33, 16, 2);

    lcd.init();
    lcd.backlight();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Initializing...");

    vTaskDelay(2000 / portTICK_PERIOD_MS);

    SensorData rcvSensorData;
    AppContext *app = (AppContext *)pvParameters;

    const char *state = "";

    while(1){
        if(xSemaphoreTake(app->xSemaphoreLCD, portMAX_DELAY) == pdTRUE){
          if(app->currentMode == MANUAL){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("MANUAL MODE");

            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("");

            vTaskDelay(1000 / portTICK_PERIOD_MS);

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("MANUAL MODE");


            xSemaphoreGive(app->xSemaphoreLCD); // Release the semaphore for other tasks
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            continue; // Skip LCD update in MANUAL mode
          }
          else if(xQueuePeek(app->xQueueSensor, &rcvSensorData, 0) == pdTRUE){
            rcvSensorData = app->sensorData;
            float temperature = rcvSensorData.temperature;
            float humidity = rcvSensorData.humidity;

            if(temperature > 20 && temperature < 30){
              state = "Normal";
            } else if((temperature >= 15 && temperature <= 20) || (temperature >= 30 && temperature <= 35)){
              state = "Warning";
            } else {
              state = "Critical";
            }

            if(humidity > 50 && humidity < 80){
              state = "Normal";
            } else if((humidity >= 40 && humidity <= 50) || (humidity >= 80 && humidity <= 90)){
              state = "Warning";
            } else {
              state = "Critical";
            }

            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("State: ");
            lcd.print(state);


            vTaskDelay(2000);
            lcd.clear();
            lcd.setCursor(0, 0);
            char buffer[32];
            snprintf(buffer, sizeof(buffer), "T:%.1fC H:%.1f%%", temperature, humidity);
            lcd.print(buffer);

          }
          xSemaphoreGive(app->xSemaphoreLCD); // Release the semaphore for other tasks
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

}