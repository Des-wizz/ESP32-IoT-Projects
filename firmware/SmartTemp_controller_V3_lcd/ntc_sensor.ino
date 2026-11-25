#include <math.h>

const float SUPPLY_VOLTAGE = 3.300;
const float FIXED_RESISTOR = 9630.0;
const float NTC_REFERENCE_RESISTANCE = 6980.0;
const float REFERENCE_TEMPERATURE = 298.15;
const float BETA_COEFFICIENT = 3950.0;

const float FILTER_ALPHA = 0.15;
const float SENSOR_MIN_TEMP = -20.0;
const float SENSOR_MAX_TEMP = 100.0;

float rawTemperature = 0.0;
float ntcResistance = 0.0;
float adcVoltage = 0.0;

bool filterInitialized = false;
unsigned long previousSampleTime = 0;

void initializeNTC() {
analogReadResolution(12);
analogSetPinAttenuation(NTC_PIN, ADC_11db);
pinMode(NTC_PIN, INPUT);
Serial.println(F("[NTC] SENSOR SETUP COMPLETED"));
}

float readADCVoltage() {
uint32_t millivolts = analogReadMilliVolts(NTC_PIN);
return millivolts / 1000.0;
}

float calculateNTCResistance(float voltage) {
if (voltage <= 0.0 || voltage >= SUPPLY_VOLTAGE) {
return -1.0;
}
return FIXED_RESISTOR * (voltage / (SUPPLY_VOLTAGE - voltage));
}

float calculateTemperature(float resistance) {
if (resistance <= 0.0) return NAN;

float inverseTemperature =
(1.0 / REFERENCE_TEMPERATURE) +
(1.0 / BETA_COEFFICIENT) *
log(resistance / NTC_REFERENCE_RESISTANCE);

float temperatureKelvin = 1.0 / inverseTemperature;
return temperatureKelvin - 273.15;

}

bool validateSensor(float voltage, float resistance, float temperature) {
if (voltage >= SUPPLY_VOLTAGE - 0.02) {
logSystemError("NTC_DISCONNECTED", "Check sensor connection wires.");
return false;
}

if (voltage <= 0.02) {
logSystemError("NTC_SHORT_CIRCUIT", "Grounded input detected.");
return false;
}

if (resistance <= 0.0 || !isfinite(temperature)) {
logSystemError("INVALID_CALCULATION", "Math limits reached.");
return false;
}

if (temperature < SENSOR_MIN_TEMP || temperature > SENSOR_MAX_TEMP) {
logSystemError("TEMP_OUT_OF_RANGE", "Thermal boundary exceeded.");
return false;
}

return true;

}

float filterTemperature(float newTemperature) {
if (!filterInitialized) {
filteredTemperature = newTemperature;
filterInitialized = true;
} else {
filteredTemperature =
(FILTER_ALPHA * newTemperature) +
((1.0 - FILTER_ALPHA) * filteredTemperature);
}
return filteredTemperature;
}

void readTemperature() {
unsigned long currentTime = millis();
if (currentTime - previousSampleTime < SAMPLE_INTERVAL) {
return;
}
previousSampleTime = currentTime;

adcVoltage = readADCVoltage();
ntcResistance = calculateNTCResistance(adcVoltage);
rawTemperature = calculateTemperature(ntcResistance);

sensorValid = validateSensor(adcVoltage, ntcResistance, rawTemperature);

if (!sensorValid) {
filterInitialized = false;
return;
}

filteredTemperature = filterTemperature(rawTemperature);

}