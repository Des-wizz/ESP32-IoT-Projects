// ---------------------------------------------------------------
// SETTINGS MENU  (16x2 LCD, three buttons)
//
// HOME SCREEN : ENTER      = open the menu
//               hold UP    = mute / unmute an active alarm
// BROWSING    : UP         = next item (hold UP = keep scrolling)
//               ENTER      = edit the item / run the action
//               ESC        = back to the home screen
// EDITING     : UP         = increase value (hold UP = fast, wraps at the top)
//               ENTER      = save the new value to storage
//               ESC        = cancel (original value is kept)
// ANY STATE   : hold ESC   = straight back to the home screen
//               (an unsaved edit is discarded)
//               No button for 60 s = menu closes by itself
//
// Editing is transactional: the live setting is only changed
// when ENTER is pressed, never while the value is being adjusted.
// ---------------------------------------------------------------

const int ITEM_MIN_TEMP     = 0;
const int ITEM_MAX_TEMP     = 1;
const int ITEM_OP_MODE      = 2;
const int ITEM_CTRL_MODE    = 3;
const int ITEM_HIGH_ALARM   = 4;
const int ITEM_LOW_ALARM    = 5;
const int ITEM_BUZZER       = 6;
const int ITEM_MAXON_EN     = 7;
const int ITEM_MAXON_TIME   = 8;
const int ITEM_DELAY_EN     = 9;
const int ITEM_DELAY_TIME   = 10;
const int ITEM_MININT_EN    = 11;
const int ITEM_MIN_ON       = 12;
const int ITEM_MIN_OFF      = 13;
const int ITEM_PERIODIC_EN  = 14;
const int ITEM_PERIOD_ON    = 15;
const int ITEM_PERIOD_OFF   = 16;
const int ITEM_DEFAULTS     = 17;
const int ITEM_EXIT         = 18;

const int MENU_COUNT = 19;

// Names are kept to 10 characters so they fit after the "MENU> " prefix.
const char* menuItems[MENU_COUNT] = {
    "Min Temp",
    "Max Temp",
    "Op Mode",
    "Ctrl Mode",
    "High Alarm",
    "Low Alarm",
    "Buzzer",
    "MaxON Tmr",
    "MaxON Time",
    "Delay Tmr",
    "Delay Time",
    "MinInt Tmr",
    "Min ON",
    "Min OFF",
    "Period Tmr",
    "Period ON",
    "Period OFF",
    "Defaults",
    "Exit Menu"
};

const float TEMP_STEP = 0.5;
const float TEMP_FAST_STEP = 5.0;
const float TEMP_LIMIT_MIN = -20.0;
const float TEMP_LIMIT_MAX = 100.0;

const unsigned long REPEAT_INTERVAL = 150;     // hold-UP repeat rate
const unsigned long MENU_TIMEOUT = 60000UL;    // auto-close after 60 s idle

// Temporary values used while editing (live settings stay untouched until ENTER)
float editTemp = 0.0;
int editChoice = 0;
unsigned long editDuration = 0;

unsigned long lastRepeatTime = 0;
unsigned long menuLastActivity = 0;
const char* menuNotice = "";

// ---------------------------------------------------------------
// ITEM TYPE HELPERS
// ---------------------------------------------------------------
bool isTempItem(int item) {
    return item == ITEM_MIN_TEMP || item == ITEM_MAX_TEMP ||
           item == ITEM_HIGH_ALARM || item == ITEM_LOW_ALARM;
}

bool isChoiceItem(int item) {
    return item == ITEM_OP_MODE || item == ITEM_CTRL_MODE || item == ITEM_BUZZER ||
           item == ITEM_MAXON_EN || item == ITEM_DELAY_EN ||
           item == ITEM_MININT_EN || item == ITEM_PERIODIC_EN;
}

bool isDurationItem(int item) {
    return item == ITEM_MAXON_TIME || item == ITEM_DELAY_TIME ||
           item == ITEM_MIN_ON || item == ITEM_MIN_OFF ||
           item == ITEM_PERIOD_ON || item == ITEM_PERIOD_OFF;
}

// ---------------------------------------------------------------
// TEMPERATURE ITEMS
// ---------------------------------------------------------------
float getTempValue(int item) {
    switch (item) {
        case ITEM_MIN_TEMP:   return minimumTemperature;
        case ITEM_MAX_TEMP:   return maximumTemperature;
        case ITEM_HIGH_ALARM: return highTemperatureAlarm;
        default:              return lowTemperatureAlarm;
    }
}

void setTempValue(int item, float value) {
    switch (item) {
        case ITEM_MIN_TEMP:   minimumTemperature = value;   break;
        case ITEM_MAX_TEMP:   maximumTemperature = value;   break;
        case ITEM_HIGH_ALARM: highTemperatureAlarm = value; break;
        default:              lowTemperatureAlarm = value;  break;
    }
}

// Limits keep the pairs valid: min < max and low alarm < high alarm.
float tempLowerBound(int item) {
    switch (item) {
        case ITEM_MAX_TEMP:   return minimumTemperature + TEMP_STEP;
        case ITEM_HIGH_ALARM: return lowTemperatureAlarm + TEMP_STEP;
        default:              return TEMP_LIMIT_MIN;
    }
}

float tempUpperBound(int item) {
    switch (item) {
        case ITEM_MIN_TEMP:   return maximumTemperature - TEMP_STEP;
        case ITEM_LOW_ALARM:  return highTemperatureAlarm - TEMP_STEP;
        default:              return TEMP_LIMIT_MAX;
    }
}

// ---------------------------------------------------------------
// CHOICE ITEMS (mode selectors and ON/OFF switches)
// ---------------------------------------------------------------
int choiceCount(int item) {
    return (item == ITEM_CTRL_MODE) ? 3 : 2;
}

int getChoice(int item) {
    switch (item) {
        case ITEM_OP_MODE:     return (int)operatingMode;
        case ITEM_CTRL_MODE:   return (int)controlMode;
        case ITEM_BUZZER:      return buzzerEnabled ? 1 : 0;
        case ITEM_MAXON_EN:    return maxOnTimerEnabled ? 1 : 0;
        case ITEM_DELAY_EN:    return delayTimerEnabled ? 1 : 0;
        case ITEM_MININT_EN:   return minimumIntervalEnabled ? 1 : 0;
        default:               return periodicTimerEnabled ? 1 : 0;
    }
}

void applyChoice(int item, int value) {
    switch (item) {
        case ITEM_OP_MODE:     operatingMode = (OperatingMode)value;  break;
        case ITEM_CTRL_MODE:   controlMode = (ControlMode)value;     break;
        case ITEM_BUZZER:      buzzerEnabled = (value != 0);          break;
        case ITEM_MAXON_EN:    maxOnTimerEnabled = (value != 0);      break;
        case ITEM_DELAY_EN:    delayTimerEnabled = (value != 0);      break;
        case ITEM_MININT_EN:   minimumIntervalEnabled = (value != 0); break;
        default:               periodicTimerEnabled = (value != 0);   break;
    }
}

const char* choiceLabel(int item, int value) {
    switch (item) {
        case ITEM_OP_MODE:
            return (value == HEATING) ? "HEATING" : "COOLING";

        case ITEM_CTRL_MODE:
            if (value == MANUAL_ON)  return "M-ON";
            if (value == MANUAL_OFF) return "M-OFF";
            return "AUTO";

        default:
            return value ? "ON" : "OFF";
    }
}

// ---------------------------------------------------------------
// DURATION ITEMS (stored in milliseconds, edited in seconds or minutes)
// ---------------------------------------------------------------
unsigned long getDuration(int item) {
    switch (item) {
        case ITEM_MAXON_TIME: return maximumOnDuration;
        case ITEM_DELAY_TIME: return activationDelay;
        case ITEM_MIN_ON:     return minimumOnDuration;
        case ITEM_MIN_OFF:    return minimumOffDuration;
        case ITEM_PERIOD_ON:  return periodicOnDuration;
        default:              return periodicOffDuration;
    }
}

void setDuration(int item, unsigned long value) {
    switch (item) {
        case ITEM_MAXON_TIME: maximumOnDuration = value;   break;
        case ITEM_DELAY_TIME: activationDelay = value;     break;
        case ITEM_MIN_ON:     minimumOnDuration = value;   break;
        case ITEM_MIN_OFF:    minimumOffDuration = value;  break;
        case ITEM_PERIOD_ON:  periodicOnDuration = value;  break;
        default:              periodicOffDuration = value; break;
    }
}

// Minutes for the long timers, seconds for the short ones.
unsigned long durationUnit(int item) {
    if (item == ITEM_MAXON_TIME || item == ITEM_PERIOD_ON || item == ITEM_PERIOD_OFF) {
        return 60000UL;
    }
    return 1000UL;
}

unsigned long durationStep(int item) {
    return (durationUnit(item) == 60000UL) ? 60000UL : 5000UL;      // 1 min or 5 s
}

unsigned long durationMin(int item) {
    return (durationUnit(item) == 60000UL) ? 60000UL : 5000UL;      // 1 min or 5 s
}

unsigned long durationMax(int item) {
    return (durationUnit(item) == 60000UL) ? 14400000UL : 600000UL; // 240 min or 600 s
}

// ---------------------------------------------------------------
// LCD DRAWING
// ---------------------------------------------------------------
// Writes one full 16-character row, padded with spaces, so the screen
// never needs clearing (no flicker, no leftover characters).
void menuPrintLine(int row, const char* text) {
    char padded[17];
    snprintf(padded, sizeof(padded), "%-16s", text);
    lcd.setCursor(0, row);
    lcd.print(padded);
}

void drawMenuScreen() {
    char line[32];

    // Row 0: mode + item name
    snprintf(line, sizeof(line), "%s%s",
             settingEditMode ? "EDIT> " : "MENU> ",
             menuItems[selectedMenu]);
    menuPrintLine(0, line);

    // Row 1: one-shot notice (Saved / No change ...) or the item value
    if (menuNotice[0] != '\0') {
        menuPrintLine(1, menuNotice);
        return;
    }

    if (isTempItem(selectedMenu)) {
        float value = settingEditMode ? editTemp : getTempValue(selectedMenu);
        snprintf(line, sizeof(line), "Value: %.1f%cC", value, (char)223);

    } else if (isChoiceItem(selectedMenu)) {
        int value = settingEditMode ? editChoice : getChoice(selectedMenu);
        snprintf(line, sizeof(line), "Value: %s", choiceLabel(selectedMenu, value));

    } else if (isDurationItem(selectedMenu)) {
        unsigned long value = settingEditMode ? editDuration : getDuration(selectedMenu);
        unsigned long unit = durationUnit(selectedMenu);
        snprintf(line, sizeof(line), "Value: %lu %s",
                 (unsigned long)(value / unit),
                 (unit == 60000UL) ? "min" : "s");

    } else if (selectedMenu == ITEM_DEFAULTS && settingEditMode) {
        snprintf(line, sizeof(line), "%s", "ENTER=Y  ESC=N");

    } else {
        snprintf(line, sizeof(line), "%s", "[Press Enter]");
    }

    menuPrintLine(1, line);
}

// ---------------------------------------------------------------
// OPEN / CLOSE
// ---------------------------------------------------------------
void openMenu() {
    menuActive = true;
    selectedMenu = 0;
    settingEditMode = false;
    menuNotice = "";
    menuLastActivity = millis();
    lastRepeatTime = millis();
    lcd.clear();
    drawMenuScreen();
}

void closeMenu() {
    menuActive = false;
    settingEditMode = false;
    menuNotice = "";
    lcd.clear();
}

// ---------------------------------------------------------------
// EDITING
// ---------------------------------------------------------------
// Copies the live value into the temporary edit value.
void beginEdit() {
    if (isTempItem(selectedMenu)) {
        editTemp = getTempValue(selectedMenu);

    } else if (isChoiceItem(selectedMenu)) {
        editChoice = getChoice(selectedMenu);

    } else if (isDurationItem(selectedMenu)) {
        editDuration = getDuration(selectedMenu);

        if (editDuration < durationMin(selectedMenu)) editDuration = durationMin(selectedMenu);
        if (editDuration > durationMax(selectedMenu)) editDuration = durationMax(selectedMenu);
    }
}

// Raises the temporary value by one step; wraps to the lowest value after the highest.
void incrementEditValue(bool fast) {
    if (isTempItem(selectedMenu)) {
        float lowest = tempLowerBound(selectedMenu);
        float highest = tempUpperBound(selectedMenu);
        float step = fast ? TEMP_FAST_STEP : TEMP_STEP;

        if (editTemp >= highest) {
            editTemp = lowest;
        } else {
            editTemp += step;
            if (editTemp > highest) editTemp = highest;
        }

    } else if (isChoiceItem(selectedMenu)) {
        editChoice++;
        if (editChoice >= choiceCount(selectedMenu)) editChoice = 0;

    } else if (isDurationItem(selectedMenu)) {
        unsigned long lowest = durationMin(selectedMenu);
        unsigned long highest = durationMax(selectedMenu);
        unsigned long step = durationStep(selectedMenu) * (fast ? 10UL : 1UL);

        if (editDuration >= highest) {
            editDuration = lowest;
        } else {
            editDuration += step;
            if (editDuration > highest) editDuration = highest;
        }
    }
}

// ENTER while editing: apply the temporary value and save it.
void commitEdit() {
    bool changed = false;

    if (selectedMenu == ITEM_DEFAULTS) {
        restoreDefaultSettings();
        menuNotice = "Defaults reset";
        return;
    }

    if (isTempItem(selectedMenu)) {
        if (editTemp != getTempValue(selectedMenu)) {
            setTempValue(selectedMenu, editTemp);
            changed = true;
        }

    } else if (isChoiceItem(selectedMenu)) {
        if (editChoice != getChoice(selectedMenu)) {
            applyChoice(selectedMenu, editChoice);
            changed = true;
        }

    } else if (isDurationItem(selectedMenu)) {
        if (editDuration != getDuration(selectedMenu)) {
            setDuration(selectedMenu, editDuration);
            changed = true;
        }
    }

    if (!changed) {
        menuNotice = "No change";
    } else if (saveSettings()) {
        menuNotice = "Saved";
    } else {
        menuNotice = "Save failed";
    }
}

// ---------------------------------------------------------------
// BUTTON HANDLING
// ---------------------------------------------------------------
void updateMenu() {
    unsigned long now = millis();

    // ---------- HOME SCREEN ----------
    if (!menuActive) {
        if (enterButton.shortPress) {
            openMenu();
        } else if (upButton.longPress && (alarmActive || alarmMuted)) {
            setAlarmMute(!alarmMuted);
        }
        return;
    }

    // ---------- MENU ACTIVE ----------
    bool redraw = false;

    // Hold UP = repeat. The first repeat comes right after the long-press time.
    bool upStep = upButton.shortPress;
    bool upFast = false;

    if (upButton.pressed && upButton.longPressHandled &&
        now - lastRepeatTime >= REPEAT_INTERVAL) {
        lastRepeatTime = now;
        upStep = true;
        upFast = true;
    }

    bool anyActivity = upStep || upButton.longPress ||
                       enterButton.shortPress || enterButton.longPress ||
                       escButton.shortPress || escButton.longPress;

    if (anyActivity) {
        menuLastActivity = now;

        if (menuNotice[0] != '\0') {
            menuNotice = "";
            redraw = true;
        }
    }

    // Idle timeout: leave the menu so the home screen (and alarm text) returns.
    if (now - menuLastActivity >= MENU_TIMEOUT) {
        closeMenu();
        return;
    }

    // Hold ESC: abandon everything and go home.
    if (escButton.longPress) {
        closeMenu();
        return;
    }

    if (settingEditMode) {
        // ----- EDITING -----
        if (upStep && selectedMenu != ITEM_DEFAULTS) {
            incrementEditValue(upFast);
            redraw = true;
        }

        if (enterButton.shortPress) {
            commitEdit();
            settingEditMode = false;
            redraw = true;
        } else if (escButton.shortPress) {
            settingEditMode = false;      // cancel: live value was never changed
            redraw = true;
        }

    } else {
        // ----- BROWSING -----
        if (upStep) {
            selectedMenu++;
            if (selectedMenu >= MENU_COUNT) {
                selectedMenu = 0;
            }
            redraw = true;
        }

        if (enterButton.shortPress) {
            if (selectedMenu == ITEM_EXIT) {
                closeMenu();
                return;
            }

            beginEdit();
            settingEditMode = true;
            redraw = true;

        } else if (escButton.shortPress) {
            closeMenu();
            return;
        }
    }

    if (redraw) {
        drawMenuScreen();
    }
}
