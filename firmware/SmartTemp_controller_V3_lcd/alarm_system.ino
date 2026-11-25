const unsigned long BUZZER_INTERVAL = 500;
const unsigned long ALARM_SAMPLE_INTERVAL = 200;

unsigned long previousBuzzerTime = 0;
unsigned long previousAlarmCheck = 0;

bool buzzerState = false;
bool highTemperatureWarning = false;
bool lowTemperatureWarning = false;
bool sensorFaultWarning = false;

void initializeAlarmSystem() {
pinMode(BUZZER_PIN, OUTPUT);
digitalWrite(BUZZER_PIN, LOW);
Serial.println("ALARM SYSTEM INITIALIZED");
}

void checkTemperatureAlarms() {
unsigned long now = millis();
if (now - previousAlarmCheck < ALARM_SAMPLE_INTERVAL) {
return;
}
previousAlarmCheck = now;

sensorFaultWarning = !sensorValid;

if (sensorValid) {
highTemperatureWarning = filteredTemperature >= highTemperatureAlarm;
lowTemperatureWarning = filteredTemperature <= lowTemperatureAlarm;
} else {
highTemperatureWarning = false;
lowTemperatureWarning = false;
}

alarmActive = sensorFaultWarning || highTemperatureWarning || lowTemperatureWarning;

// Mute only lasts for the current alarm; the buzzer re-arms once the alarm clears.
if (!alarmActive) {
alarmMuted = false;
}

}

void updateBuzzer() {
if (!buzzerEnabled || alarmMuted || !alarmActive) {
digitalWrite(BUZZER_PIN, LOW);
buzzerState = false;
return;
}

unsigned long now = millis();
if (now - previousBuzzerTime >= BUZZER_INTERVAL) {
previousBuzzerTime = now;
buzzerState = !buzzerState;
digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
}
}

void setAlarmMute(bool mute) {
alarmMuted = mute;
if (mute) {
digitalWrite(BUZZER_PIN, LOW);
buzzerState = false;
}
}

void updateAlarmSystem() {
checkTemperatureAlarms();
updateBuzzer();
}