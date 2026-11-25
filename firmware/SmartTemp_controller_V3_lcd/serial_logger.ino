void initializeSerialLogger() {
Serial.begin(115200);
while (!Serial) {
; // Safeguard loop for native USB communication links
}
Serial.println(F("=========================================="));
Serial.println(F("    SERIAL DIAGNOSTIC LOGGER ACTIVE      "));
Serial.println(F("=========================================="));
}

void logSystemMetrics() {
static unsigned long lastLogTime = 0;
unsigned long now = millis();

// Print dashboard updates strictly once per second
if (now - lastLogTime >= 1000) {
lastLogTime = now;
Serial.print(F("[DATA] Uptime: "));
Serial.print(now / 1000);
Serial.print(F("s | Temp: "));
if (sensorValid) {
    Serial.print(filteredTemperature, 2);
    Serial.print(F(" C"));
} else {
    Serial.print(F("FAULTY_SENSOR"));
}

Serial.print(F(" | Range: ["));
Serial.print(minimumTemperature, 1);
Serial.print(F("C - "));
Serial.print(maximumTemperature, 1);
Serial.print(F("C]"));

Serial.print(F(" | Mode: "));
Serial.print(operatingMode == HEATING ? F("HEATING") : F("COOLING"));

Serial.print(F(" ("));
switch (controlMode) {
    case AUTO_MODE:  Serial.print(F("AUTO")); break;
    case MANUAL_ON:  Serial.print(F("MANUAL_ON")); break;
    case MANUAL_OFF: Serial.print(F("MANUAL_OFF")); break;
}
Serial.print(F(")"));

Serial.print(F(" | Relay: "));
Serial.print(relayState ? F("ON") : F("OFF"));

Serial.print(F(" | Alarm: "));
Serial.println(alarmActive ? F("TRIGGERED") : F("OK"));

}

}

void logSystemError(const char* errorType, const char* description) {
Serial.print(F("[SYSTEM ERROR] ["));
Serial.print(errorType);
Serial.print(F("] -> "));
Serial.println(description);
}