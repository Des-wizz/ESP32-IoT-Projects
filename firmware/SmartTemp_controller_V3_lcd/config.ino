// HARDWARE PINS
const int NTC_PIN = 2;
const int OLED_SDA = 8;
const int OLED_SCL = 9;
const int RELAY_PIN = 7;
const int BUZZER_PIN = 1;
const int BUTTON_UP = 4;
const int BUTTON_ENTER = 5;
const int BUTTON_ESC = 6;

const unsigned long SAMPLE_INTERVAL = 100;
const unsigned long DISPLAY_INTERVAL = 200;

// TEMPERATURE SETTINGS
float minimumTemperature = 25.0;
float maximumTemperature = 35.0;
float filteredTemperature = 0.0;
bool sensorValid = false;

// OPERATING SETTINGS
OperatingMode operatingMode = COOLING;
ControlMode controlMode = AUTO_MODE;

// RELAY SETTINGS
const bool RELAY_ACTIVE_LOW = true;
bool relayState = false;
bool relayDemand = false;

// MENU SETTINGS
int selectedMenu = 0;
bool menuActive = false;
bool settingEditMode = false;

// TIMER SETTINGS
bool maxOnTimerEnabled = true;
bool delayTimerEnabled = false;
bool minimumIntervalEnabled = true;
bool periodicTimerEnabled = false;

unsigned long maximumOnDuration = 1800000UL;
unsigned long activationDelay = 5000UL;
unsigned long minimumOnDuration = 60000UL;
unsigned long minimumOffDuration = 30000UL;
unsigned long periodicOnDuration = 600000UL;
unsigned long periodicOffDuration = 300000UL;

// ALARM SETTINGS
float highTemperatureAlarm = 45.0;
float lowTemperatureAlarm = 10.0;
bool buzzerEnabled = true;
bool alarmMuted = false;
bool alarmActive = false;
