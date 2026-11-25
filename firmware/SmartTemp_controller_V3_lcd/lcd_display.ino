#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

unsigned long previousDisplayTime = 0;

void initializeLCD() {
    Wire.begin(8, 9);
    
    lcd.init();
    lcd.backlight();
    lcd.clear();
    
    lcd.setCursor(0, 0);
    lcd.print("  SMARTTEMP V1  ");
    lcd.setCursor(0, 1);
    lcd.print(" INITIALIZING.. ");
    delay(1500);
    lcd.clear();
}

void updateLCD() {
    unsigned long currentTime = millis();
    if (currentTime - previousDisplayTime < DISPLAY_INTERVAL) {
        return;
    }
    previousDisplayTime = currentTime;

    lcd.clear();

    lcd.setCursor(0, 0);
    if (sensorValid) {
        lcd.print("T:");
        lcd.print(filteredTemperature, 1);
        lcd.print((char)223);
        lcd.print("C");
    } else {
        lcd.print("T:ERR  ");
    }

    lcd.setCursor(9, 0);
    lcd.print("S:");
    if (operatingMode == COOLING) {
        lcd.print(maximumTemperature, 0);
    } else {
        lcd.print(minimumTemperature, 0);
    }
    lcd.print((char)223);
    lcd.print(operatingMode == HEATING ? "H" : "C");

    lcd.setCursor(0, 1);
    if (alarmActive) {
        lcd.print(alarmMuted ? "ALARM: MUTED" : "!!ALARM ACTIVE!!");
    } else {
        lcd.print("OUT:");
        lcd.print(relayState ? "ON " : "OFF");
        
        lcd.setCursor(9, 1);
        lcd.print("M:");
        switch (controlMode) {
            case AUTO_MODE:  lcd.print("AUTO"); break;
            case MANUAL_ON:  lcd.print("M-ON"); break;
            case MANUAL_OFF: lcd.print("M-OFF"); break;
        }
    }
}
