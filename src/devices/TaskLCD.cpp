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

    while(1){
        displayTemperatureHumidity(lcd);
        vTaskDelay(2000);
        displayLux(lcd);
        vTaskDelay(2000);
    }

}