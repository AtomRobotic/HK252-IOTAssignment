#include "devices/TaskLCD.h"

void displayTemperatureHumidity(LiquidCrystal_I2C &lcd){
  float temperature = getTemperature();
  float humidity = getHumidity();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C");
  
  lcd.setCursor(0,1);
  lcd.print("Humid: ");
  lcd.print(humidity);
  lcd.print("%");
}

void displayLux(LiquidCrystal_I2C &lcd){
  float luxValue = getLuxValue();

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Lux: ");
  lcd.print(luxValue);
}

void TaskLCD(void *pvParameters){
    LiquidCrystal_I2C lcd(33, 16, 2);

    lcd.init();
    lcd.backlight();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Initializing...");

    SensorData rcvSensorData;
    AppContext *app = (AppContext *)pvParameters;

    const char *state = "";

    while(1){
        if(xSemaphoreTake(app->xSemaphoreLCD, portMAX_DELAY) == pdTRUE){
          if(xQueuePeek(app->xQueueSensor, &rcvSensorData, 0) == pdTRUE){
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

            lcd.setCursor(0, 1);
            snprintf(buffer, sizeof(buffer), "SM:%.1f%% L:%.1f", rcvSensorData.soilMoisture, rcvSensorData.lux);
            lcd.print(buffer);

            //lcd.print("temp: "); lcd.print(temperature); lcd.print("C ");
            //lcd.print("humid: "); lcd.print(humidity); lcd.print("%");
          }
        }
        vTaskDelay(2000);
    }

}