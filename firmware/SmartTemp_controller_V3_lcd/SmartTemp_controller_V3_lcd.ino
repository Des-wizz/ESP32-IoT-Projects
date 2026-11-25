#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// OPERATING MODES
enum OperatingMode {
HEATING,
COOLING
};

enum ControlMode {
AUTO_MODE,
MANUAL_ON,
MANUAL_OFF
};

// BUTTON STRUCTURE
struct Button {
int pin;
bool stableState;
bool lastReading;
bool pressed;
bool shortPress;
bool longPress;
bool longPressHandled;
unsigned long lastDebounceTime;
unsigned long pressStartTime;
};

// ---------------------------------------------------------------
// SHARED VARIABLE DECLARATIONS
// --------------------------------------------------------------- 

// Hardware pins (defined in config.ino)
extern const int BUZZER_PIN;
extern const int BUTTON_UP;
extern const int BUTTON_ENTER;
extern const int BUTTON_ESC;

// Temperature / sensor (defined in config.ino)
extern float filteredTemperature;
extern bool sensorValid;

// Menu (defined in config.ino)
extern bool menuActive;

// Alarm settings (defined in config.ino)
extern float highTemperatureAlarm;
extern float lowTemperatureAlarm;
extern bool buzzerEnabled;
extern bool alarmMuted;
extern bool alarmActive;

// LCD Display Object (defined in lcd_display.ino)
extern LiquidCrystal_I2C lcd;

// MODULE FUNCTION DECLARATIONS
void initializeNTC();
void readTemperature();
void initializeLCD();
void updateLCD();
void initializeButtons();
void updateButtons();
void updateMenu();
void updateTemperatureControl();
void initializeRelay();
void updateRelay();
void initializeTimerManager();
bool timerAllowsRelay(bool demand);
void initializeAlarmSystem();
void updateAlarmSystem();
void initializeStorage();
bool saveSettings();
void restoreDefaultSettings();
void updateSystemMonitor();

// Serial Logger Declarations
void initializeSerialLogger();
void logSystemMetrics();
void logSystemError(const char* errorType, const char* description);

// SETUP
void setup() {
initializeSerialLogger();
delay(500);

Serial.println(F("[SYSTEM] SMARTTEMP CONTROLLER V1.0"));
Serial.println(F("[SYSTEM] STARTING CONFIGURATION"));

initializeStorage();
initializeRelay();
initializeNTC();
initializeLCD();
initializeButtons();
initializeTimerManager();
initializeAlarmSystem();

Serial.println(F("[SYSTEM] ALL MODULES INITIALIZED - RUNNING"));

}

// MAIN LOOP
void loop() {
readTemperature();

updateButtons();
updateMenu();

updateTemperatureControl();
updateRelay();

updateAlarmSystem();
updateSystemMonitor();

if (!menuActive) {
updateLCD();
}

logSystemMetrics();

}