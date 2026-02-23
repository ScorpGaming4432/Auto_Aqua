# Auto Aqua - Quick Reference Guide

## System Overview
**Auto Aqua** is an Arduino Mega aquarium automation firmware with multilingual UI, pump configuration, and automatic water-level handling.

## Quick File Reference

| File | Purpose |
|------|---------|
| `auto_aqua.ino` | Entry point, setup flow, main key handlers |
| `screens.h/.cpp` | LCD screens and numeric input workflows |
| `display.h/.cpp` | LCD initialization/configuration |
| `language.h` | 10-language definitions in PROGMEM |
| `chars.h` | Custom LCD glyphs |
| `sensor.h` | Water sensor addressing/helpers |
| `water.h/.cpp` | Water-level checks and inlet/outlet control |
| `pumps.h` | Pump model + scheduling helpers |
| `storage.h/.cpp` | EEPROM persistence + factory reset |

## Hardware Snapshot

### Keypad pins (Arduino Mega)
- Rows: `30, 32, 34, 36`
- Cols: `31, 33, 35, 37`

### I2C bus
- SDA: pin `20`
- SCL: pin `21`

### I2C addresses
- LCD: `0x27`
- High-level sensor: `0x78`
- Low-level sensor: `0x77`

### Serial
- `9600` baud

## Main Constants

```cpp
PUMP_COUNT = 5            // 3 dosing + inlet + outlet
LANG_COUNT = 10           // supported languages
SCREEN_WIDTH = 16         // LCD columns
SCREEN_HEIGHT = 2         // LCD rows
SCREEN_LOCATION = 0x27    // LCD I2C address
```

## Keypad Layout

```
1  2  3  A
4  5  6  B
7  8  9  C
*  0  #  D
```

## Active Main-Screen Commands

| Key(s) | Function |
|--------|----------|
| 1-3 | Edit dosing pump amount |
| D | Edit water thresholds |
| C | Read/display water level |
| 0 | Display current time |
| B | Change language |
| * | Factory reset (confirm with `#`) |

> Note: Some historical docs/comments mention keys planned for future behavior.

## First-Run Setup Flow
1. Initialize serial + I2C
2. Show splash screen
3. Select language
4. Configure tank volume
5. Configure dosing amount + interval for pumps 1-3
6. Set time offset
7. Configure water thresholds
8. Save configuration and enter loop

## Main Loop Summary
```
Draw main screen
Read keypad key
Run action (amount/threshold/level/time/language/reset)
Run automatic water-level check
Run dosing schedule check
```

## Numeric Input Behavior
- **View mode**: show current value
- **Edit mode**: digits `0-9`
- `#`: confirm / move field (screen-dependent)
- `*`: cancel
- `D`: backspace (threshold editor)
- `A`: save (threshold editor)

## Core Data Structures

```cpp
struct Pump {
  uint16_t amount;      // ml
  uint64_t duration;    // ms / interval context
};
```

```cpp
namespace AppState {
  uint8_t languageIndex;
  Pump pumps[PUMP_COUNT];
  uint32_t tankVolume;
  int64_t timeOffset;
}
```

## EEPROM Sentinel Values
- Language unset: `0xFF`
- Tank volume unset: `0xFFFFFFFF`
- Time offset unset: `0xFFFFFFFFFFFFFFFF`
- Pump amount unset: `0xFFFF`
- Pump duration/interval unset: `0xFFFFFFFFFFFFFFFF`

## Supported Languages
Polish, English, Russian, German, French, Spanish, Italian, Portuguese, Turkish, Czech.

## Common Operations

### Change language
`B` → choose language → confirm.

### Edit pump amount
`1`, `2`, or `3` → edit value → confirm.

### Configure thresholds
`D` → edit low/high fields → save with `A`.

### Factory reset
`*` → `#` to confirm.

## Troubleshooting Quick Table

| Issue | Check |
|-------|-------|
| LCD blank | I2C address/power/contrast |
| Keypad no response | Pins 30-37 wiring |
| Sensor read errors | I2C addresses `0x77`/`0x78`, cabling |
| Upload fails | Correct board/port, free serial port |
| Serial logs missing | 9600 baud and cable quality |

## Suggested Documentation Practice
When key mappings or setup flow change, update both `README.md` and this file in the same commit.
