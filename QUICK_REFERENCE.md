# Auto Aqua - Quick Reference Guide

## System Overview

**Auto Aqua** is a multi-language aquarium automation system with LCD display control and pump management.

## Quick File Reference

| File | Purpose |
|------|---------|
| `auto_aqua.ino` | Main entry point, setup(), loop() |
| `screens.h/.cpp` | All LCD screens and input handling |
| `display.h/.cpp` | LCD initialization and configuration |
| `language.h` | Multi-language support (10 languages) |
| `chars.h` | Custom LCD characters and glyphs |
| `sensor.h` | Water level sensor interface |
| `pumps.h` / `water.cpp` | Pump control and I2C communication |
| `storage.h/.cpp` | Data persistence (stub) |

## Hardware Pins

### Keypad
- **Rows**: 30, 32, 34, 36 (inputs)
- **Cols**: 31, 33, 35, 37 (outputs)

### I2C (LCD Display + Sensors)
- **SDA (Data)**: Pin 20 (Arduino Mega)
- **SCL (Clock)**: Pin 21 (Arduino Mega)

### I2C Addresses
- LCD: `0x27` (16x2 display)
- High-level sensor: `0x78`
- Low-level sensor: `0x77`

### Serial
- Baud rate: `9600`

## Main Constants

```cpp
PUMP_COUNT = 5              // Number of pumps
LANG_COUNT = 10             // Supported languages
SCREEN_WIDTH = 16           // LCD columns
SCREEN_HEIGHT = 2           // LCD rows
SCREEN_LOCATION = 0x27      // I2C address
PRZEPLYW = 2000             // Flow rate (ml/ms)
```

## Keypad Layout

```
1  2  3  A
4  5  6  B
7  8  9  C
*  0  #  D
```

## Main Screen Commands

| Key(s) | Function |
|--------|----------|
| 1-5 | Edit pump 1-5 amount |
| 0 | Display current time |
| D | Edit tank volume |
| C | Read water level |
| P | Edit pump duration |
| * | Factory reset (confirm with #) |
| # | Confirm/Enter edit mode |

## Setup Flow

1. Initialize serial & I2C
2. Display splash screen
3. Select language
4. Configure tank volume
5. Set pump amounts (1-5)
6. Synchronize time
7. Configure pump durations
8. Enter main loop

## Main Loop

```
while(1) {
  Display main screen
  Wait for keypad input
  Route to handler (pump edit, tank volume, time, etc.)
  Return to main screen
}
```

## Configuration Screens

### Numeric Input (Tank, Amount, Duration)
- **View mode**: See current value, press # to edit
- **Edit mode**: Enter digits (0-9), press # to confirm, * to cancel
- **Format**: Right-aligned with underscores
- **Example**: `<-* _______l #->` for tank volume

### Language Selection
- **Keys A/B**: Navigate between languages
- **Keys 0-9**: Jump to language directly
- **Key #**: Confirm selection
- **Key ***: Cancel

### Time Setup
- **Format**: HH:MM:SS
- **Keys A/B**: Navigate between fields
- **Keys 0-9**: Enter digit
- **Key #**: Confirm
- **Key ***: Cancel

## Data Structures

### Pump Configuration
```cpp
struct Pump {
  uint16_t amount;      // ml (0-9999)
  uint64_t duration;    // ms
};
```

### Global State
```cpp
namespace AppState {
  uint8_t languageIndex;        // 0-9
  Pump pumps[PUMP_COUNT];       // 5 pumps
  uint32_t tankVolume;          // liters
  int64_t timeOffset;           // for RTC sync
};
```

## Serial Debug Output

Messages follow a tag format for easy filtering:

- `[SETUP]` - Initialization
- `[LOOP]` - Main loop events
- `[AM]` - Amount editing
- `[TANK]` - Tank volume
- `[DUR]` - Duration editing
- `[TIME]` - Time synchronization

Example: `[SETUP] Serial started`

## Supported Languages

| # | Language | Native Name |
|---|----------|-------------|
| 0 | Polish | Polski |
| 1 | English | English |
| 2 | Russian | Русский |
| 3 | German | Deutsch |
| 4 | French | Français |
| 5 | Spanish | Español |
| 6 | Italian | Italiano |
| 7 | Portuguese | Português |
| 8 | Turkish | Türkçe |
| 9 | Czech | Čeština |

## Common Operations

### How to Select Language
1. On splash screen, you're in language selection
2. Press A/B to scroll or 0-9 to jump
3. Press # to confirm

### How to Configure Tank Volume
1. From main screen, press D
2. Current value shows in view mode
3. Press # to edit
4. Enter digits (0-9), press # to confirm

### How to Set Pump Amount
1. From main screen, press 1-5 (for pump 1-5)
2. View current amount
3. Press # to edit
4. Enter amount in ml
5. Press # to confirm

### How to Reset to Factory Defaults
1. From main screen, press *
2. Confirm: press # for yes, * for no
3. System reboots to setup()

## Troubleshooting

| Issue | Check |
|-------|-------|
| LCD not visible | I2C address (0x27), power, contrast |
| Keypad not working | Pin connections 30-37, debounce time |
| Language text garbled | Glyph set loaded, PROGMEM reading |
| Water sensor not reading | I2C addresses (0x77, 0x78), wire connections |
| Serial not working | Baud rate 9600, USB cable |

## Memory Usage

### PROGMEM (Flash)
- 10 Language structures (~1200 bytes)
- Custom glyphs (Polish, Russian, French, Spanish, Portuguese)
- Screen format strings

### RAM
- AppState + Pump array (~50 bytes)
- Display buffers (~100 bytes)
- Stack/heap (remaining)

## Performance Notes

- **Cursor blink**: 500ms period
- **Splash animation**: 80ms per frame
- **Key debounce**: Handled by Keypad library
- **Serial output**: 9600 baud (19.2ms per char)
- **I2C**: Standard 100kHz (Arduino default)

## Future Enhancements

- [ ] EEPROM persistence
- [ ] Scheduled pump cycles
- [ ] Wireless monitoring (Bluetooth/WiFi)
- [ ] Alarm system (high/low water)
- [ ] Pump diagnostics
- [ ] Settings menu (no restart)
- [ ] Data logging

## Resources

- **Keypad Library**: Docs in header
- **LiquidCrystal_I2C**: Standard Arduino library
- **Wire Library**: I2C communication
- **Code Documentation**: See CODE_DOCUMENTATION.md

---

**Version**: 1.0  
**Last Updated**: January 2026  
**Target**: Arduino Mega  
**Language**: C++ (Arduino)
