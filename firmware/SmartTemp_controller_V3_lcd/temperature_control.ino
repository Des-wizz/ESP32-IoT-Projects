bool previousTemperatureDemand = false;

bool calculateTemperatureDemand() {
if (!sensorValid) {
return false;
}

if (operatingMode == COOLING) {
if (filteredTemperature >= maximumTemperature) {
return true;
}
if (filteredTemperature <= minimumTemperature) {
return false;
}
}
else if (operatingMode == HEATING) {
if (filteredTemperature <= minimumTemperature) {
return true;
}
if (filteredTemperature >= maximumTemperature) {
return false;
}
}
return previousTemperatureDemand;
}

void updateTemperatureControl() {
if (!sensorValid) {
relayDemand = false;
previousTemperatureDemand = false;
return;
}

switch (controlMode) {
case AUTO_MODE:
relayDemand = calculateTemperatureDemand();
break;
case MANUAL_ON:
relayDemand = true;
break;
case MANUAL_OFF:
relayDemand = false;
break;
}

previousTemperatureDemand = (controlMode == AUTO_MODE) ? relayDemand : previousTemperatureDemand;

}