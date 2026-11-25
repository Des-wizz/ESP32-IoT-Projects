# SmartTemp Controller V3 (16x2 LCD)

Standalone ESP32-S3 temperature controller. An NTC thermistor measures temperature, a 16x2 I2C LCD shows it, and one relay switches a heater, cooler or fridge. Settings are changed with three buttons and saved in the ESP32's flash memory (Preferences), so no computer is needed to adjust them.

## Features

- **Heating or Cooling** operating mode with Min and Max temperature limits and hysteresis.
  - Cooling: relay ON at Max, OFF at Min.
  - Heating: relay ON at Min, OFF at Max.
- **Timers** (each can be switched on or off in the menu): Max ON time, start delay, minimum ON/OFF interval, periodic ON/OFF.
- **Alarms**: high temperature, low temperature and sensor fault, with a buzzer and a mute button.
- **Sensor protection**: a disconnected, shorted or out-of-range sensor forces the relay OFF.
- **Settings menu** on the LCD, with cancel-safe editing (values are applied only when ENTER is pressed).
- **Serial Monitor** status output, all handled in `serial_logger.ino`.

## Target hardware

- ESP32-S3 development board (confirm your exact board's pinout)
- 10k-class NTC thermistor in a voltage divider with a fixed resistor
- 16x2 HD44780 LCD with I2C backpack, address `0x27`
- Relay module, active buzzer, three push buttons

## GPIO assignments (from `config.ino`)

| Function | GPIO |
|---|---|
| NTC divider (ADC) | 2 |
| LCD SDA | 8 |
| LCD SCL | 9 |
| Relay | 7 |
| Buzzer | 1 |
| UP button | 4 |
| ENTER button | 5 |
| ESC button | 6 |

Buttons connect between the GPIO and GND (`INPUT_PULLUP`).

## Libraries

- `LiquidCrystal_I2C` (Arduino Library Manager)
- ESP32 Arduino core (`Preferences`, `Wire` are included)

## Build and upload

1. Open `SmartTemp_controller_V3_lcd.ino` in the Arduino IDE. All `.ino` files in this folder open as tabs.
2. Select your ESP32-S3 board and port.
3. Install the libraries above, then upload.

## Using the buttons

- **ENTER** (home screen): open the menu.
- **UP**: next item, or increase the value while editing (hold for fast).
- **ENTER** (editing): save the value. **ESC**: cancel.
- **Hold ESC**: back to the home screen. **Hold UP** (home screen): mute an active alarm.

## Sensor calibration

`ntc_sensor.ino` uses a fixed resistor of 9630 ohms and an NTC reference of 6980 ohms at 25 C with Beta 3950. Adjust these to match your parts.

## Known limitations

- Not yet tested on a real load. Test with the relay disconnected or a low-voltage load first.
- The relay polarity in `relay_control.ino` does not match `RELAY_ACTIVE_LOW` in `config.ino`. Confirm your relay module's active level and make the two agree before connecting a load.
- The high alarm default is 45 C in `config.ino` but 40 C in `storage.ino`; the stored default is used at start-up.
- Do not rely on this firmware as the only safety device for a heater or compressor. Use independent, correctly rated hardware protection.
