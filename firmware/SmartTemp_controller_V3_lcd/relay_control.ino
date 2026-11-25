void setRelay(bool state) {
relayState = state;
if (RELAY_ACTIVE_LOW) {
  digitalWrite(RELAY_PIN, state ? HIGH : LOW);

} else {
digitalWrite(RELAY_PIN, state ? LOW : HIGH);
}
}

void initializeRelay() {
digitalWrite(RELAY_PIN, RELAY_ACTIVE_LOW ? HIGH : LOW);
pinMode(RELAY_PIN, OUTPUT);
setRelay(false);
Serial.println("RELAY INITIALIZED");
}

void updateRelay() {
if (!sensorValid) {
setRelay(false);
return;
}
bool permitted = timerAllowsRelay(relayDemand);
setRelay(permitted);
}

bool getRelayState() {
return relayState;
}