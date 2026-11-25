bool demandActive = false;
bool maxDurationLockout = false;
bool lastObservedRelayState = false;

unsigned long demandStartTime = 0;
unsigned long relayOnStartTime = 0;
unsigned long lastRelayChangeTime = 0;
unsigned long periodicStartTime = 0;

void initializeTimerManager() {
unsigned long now = millis();
demandActive = false;
maxDurationLockout = false;
lastObservedRelayState = false;
demandStartTime = now;
relayOnStartTime = now;
lastRelayChangeTime = now;
periodicStartTime = now;
Serial.println("TIMER MANAGER INITIALIZED");
}

void monitorRelayTiming() {
unsigned long now = millis();
if (relayState != lastObservedRelayState) {
lastRelayChangeTime = now;
if (relayState) {
relayOnStartTime = now;
}
lastObservedRelayState = relayState;
}
}

bool periodicWindowActive() {
if (!periodicTimerEnabled) return true;
unsigned long cycleDuration = periodicOnDuration + periodicOffDuration;
if (cycleDuration == 0) return false;
unsigned long elapsed = millis() - periodicStartTime;
unsigned long cyclePosition = elapsed % cycleDuration;
return cyclePosition < periodicOnDuration;
}

bool maximumOnTimeExpired() {
if (!maxOnTimerEnabled || !relayState) return false;
return millis() - relayOnStartTime >= maximumOnDuration;
}

bool timerAllowsRelay(bool demand) {
unsigned long now = millis();
monitorRelayTiming();

if (!demand) {
demandActive = false;
maxDurationLockout = false;
}

if (demand && !demandActive) {
demandActive = true;
demandStartTime = now;
}

if (maximumOnTimeExpired()) {
maxDurationLockout = true;
Serial.println("MAXIMUM RELAY ON TIME EXCEEDED");
}

if (maxDurationLockout) return false;
if (!periodicWindowActive()) return false;

if (!demand) {
if (!relayState) return false;
if (minimumIntervalEnabled && (now - relayOnStartTime < minimumOnDuration)) {
return true;
}
return false;
}

if (relayState) return true;

if (delayTimerEnabled && (now - demandStartTime < activationDelay)) {
return false;
}

if (minimumIntervalEnabled && (now - lastRelayChangeTime < minimumOffDuration)) {
return false;
}

return true;

}