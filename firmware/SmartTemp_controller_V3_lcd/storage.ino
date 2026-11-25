#include <Preferences.h>

Preferences preferences;

bool saveSettings() {
if (!preferences.begin("smarttemp", false)) {
Serial.println("STORAGE OPEN FAILED");
return false;
}

preferences.putFloat("minTemp", minimumTemperature);
preferences.putFloat("maxTemp", maximumTemperature);
preferences.putUChar("opMode", (uint8_t)operatingMode);
preferences.putUChar("ctrlMode", (uint8_t)controlMode);
preferences.putBool("maxOnEn", maxOnTimerEnabled);
preferences.putBool("delayEn", delayTimerEnabled);
preferences.putBool("minIntEn", minimumIntervalEnabled);
preferences.putBool("periodicEn", periodicTimerEnabled);
preferences.putULong("maxOnTime", maximumOnDuration);
preferences.putULong("delayTime", activationDelay);
preferences.putULong("minOnTime", minimumOnDuration);
preferences.putULong("minOffTime", minimumOffDuration);
preferences.putULong("periodOn", periodicOnDuration);
preferences.putULong("periodOff", periodicOffDuration);
preferences.putFloat("highAlarm", highTemperatureAlarm);
preferences.putFloat("lowAlarm", lowTemperatureAlarm);
preferences.putBool("buzzerEn", buzzerEnabled);

preferences.end();
Serial.println("SETTINGS SAVED");
return true;

}

void loadSettings() {
if (!preferences.begin("smarttemp", true)) {
Serial.println("STORAGE OPEN FAILED");
return;
}

minimumTemperature = preferences.getFloat("minTemp", 25.0);
maximumTemperature = preferences.getFloat("maxTemp", 35.0);

uint8_t savedOperatingMode = preferences.getUChar("opMode", COOLING);
uint8_t savedControlMode = preferences.getUChar("ctrlMode", AUTO_MODE);

if (savedOperatingMode <= COOLING) {
operatingMode = (OperatingMode)savedOperatingMode;
} else {
operatingMode = COOLING;
}

if (savedControlMode <= MANUAL_OFF) {
controlMode = (ControlMode)savedControlMode;
} else {
controlMode = AUTO_MODE;
}

maxOnTimerEnabled = preferences.getBool("maxOnEn", true);
delayTimerEnabled = preferences.getBool("delayEn", false);
minimumIntervalEnabled = preferences.getBool("minIntEn", true);
periodicTimerEnabled = preferences.getBool("periodicEn", false);
maximumOnDuration = preferences.getULong("maxOnTime", 1800000UL);
activationDelay = preferences.getULong("delayTime", 5000UL);
minimumOnDuration = preferences.getULong("minOnTime", 60000UL);
minimumOffDuration = preferences.getULong("minOffTime", 30000UL);
periodicOnDuration = preferences.getULong("periodOn", 600000UL);
periodicOffDuration = preferences.getULong("periodOff", 300000UL);
highTemperatureAlarm = preferences.getFloat("highAlarm", 40.0);
lowTemperatureAlarm = preferences.getFloat("lowAlarm", 10.0);
buzzerEnabled = preferences.getBool("buzzerEn", true);

preferences.end();

if (!isfinite(minimumTemperature) || !isfinite(maximumTemperature) || minimumTemperature >= maximumTemperature) {
minimumTemperature = 25.0;
maximumTemperature = 35.0;
}

if (!isfinite(highTemperatureAlarm) || !isfinite(lowTemperatureAlarm) || lowTemperatureAlarm >= highTemperatureAlarm) {
lowTemperatureAlarm = 10.0;
highTemperatureAlarm = 40.0;
}

Serial.println("SETTINGS LOADED");

}

void initializeStorage() {
loadSettings();
}

void restoreDefaultSettings() {
minimumTemperature = 25.0;
maximumTemperature = 35.0;
operatingMode = COOLING;
controlMode = AUTO_MODE;
maxOnTimerEnabled = true;
delayTimerEnabled = false;
minimumIntervalEnabled = true;
periodicTimerEnabled = false;
maximumOnDuration = 1800000UL;
activationDelay = 5000UL;
minimumOnDuration = 60000UL;
minimumOffDuration = 30000UL;
periodicOnDuration = 600000UL;
periodicOffDuration = 300000UL;
highTemperatureAlarm = 40.0;
lowTemperatureAlarm = 10.0;
buzzerEnabled = true;
saveSettings();
Serial.println("DEFAULT SETTINGS RESTORED");
}