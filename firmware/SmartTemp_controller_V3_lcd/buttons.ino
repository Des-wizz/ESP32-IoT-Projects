const unsigned long DEBOUNCE_TIME = 40;
const unsigned long LONG_PRESS_TIME = 800;

Button upButton = { BUTTON_UP, HIGH, HIGH, false, false, false, false, 0, 0 };
Button enterButton = { BUTTON_ENTER, HIGH, HIGH, false, false, false, false, 0, 0 };
Button escButton = { BUTTON_ESC, HIGH, HIGH, false, false, false, false, 0, 0 };

void initializeButtons() {
pinMode(BUTTON_UP, INPUT_PULLUP);
pinMode(BUTTON_ENTER, INPUT_PULLUP);
pinMode(BUTTON_ESC, INPUT_PULLUP);
Serial.println("BUTTONS INITIALIZED");
}

void updateButton(Button &button) {
unsigned long currentTime = millis();
bool reading = digitalRead(button.pin);

button.shortPress = false;
button.longPress = false;

if (reading != button.lastReading) {
button.lastDebounceTime = currentTime;
button.lastReading = reading;
}

if (currentTime - button.lastDebounceTime >= DEBOUNCE_TIME) {
if (reading != button.stableState) {
button.stableState = reading;
    if (button.stableState == LOW) {
        button.pressed = true;
        button.longPressHandled = false;
        button.pressStartTime = currentTime;
    } else {
        button.pressed = false;
        if (!button.longPressHandled) {
            button.shortPress = true;
        }
    }
}

}

if (button.pressed && !button.longPressHandled &&
currentTime - button.pressStartTime >= LONG_PRESS_TIME) {
button.longPress = true;
button.longPressHandled = true;
}

}

void updateButtons() {
updateButton(upButton);
updateButton(enterButton);
updateButton(escButton);
}