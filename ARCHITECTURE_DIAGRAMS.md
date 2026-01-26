# Auto Aqua - Architecture & System Diagrams

## System Architecture

```
┌─────────────────────────────────────────────────────────┐
│               AUTO AQUA SYSTEM                          │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────┐  ┌──────────────┐  ┌──────────────┐   │
│  │  LCD Display│  │   Keypad     │  │  Sensors     │   │
│  │   (I2C)     │  │  (4x4 Matrix)│  │  (I2C)       │   │
│  │  16x2 0x27  │  │  Pins 30-37  │  │  0x77/0x78   │   │
│  └──────┬──────┘  └──────┬───────┘  └──────┬───────┘   │
│         │                 │                 │           │
│         └─────────────────┼─────────────────┘           │
│                           │                            │
│                    ┌──────▼──────┐                     │
│                    │ Arduino Mega │                     │
│                    │              │                     │
│                    │ - Serial     │                     │
│                    │ - I2C (Wire) │                     │
│                    │ - GPIO       │                     │
│                    └──────┬───────┘                     │
│                           │                            │
│         ┌─────────────────┼──────────────┐             │
│         │                 │              │             │
│    ┌────▼────┐      ┌─────▼──┐    ┌─────▼───┐        │
│    │ Pump 1  │      │ Pump 2 │    │ Pump 3-5│        │
│    │ (GPIO)  │      │ (GPIO) │    │ (GPIO)  │        │
│    └─────────┘      └────────┘    └─────────┘        │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

## Software Architecture

```
┌────────────────────────────────────────────────────────┐
│                  auto_aqua.ino                         │
│  (Main entry point, setup(), loop())                  │
├────────────────────────────────────────────────────────┤
│                                                        │
│  ┌─────────────────────────────────────────────────┐  │
│  │           AppState Namespace                    │  │
│  │  ├─ uint8_t languageIndex (0-9)               │  │
│  │  ├─ Pump pumps[5]                             │  │
│  │  ├─ uint32_t tankVolume                       │  │
│  │  └─ int64_t timeOffset                        │  │
│  └─────────────────────────────────────────────────┘  │
│                                                        │
│  ┌──────────────────────────────────────────────────┐ │
│  │          Screen Handlers (namespace Screen)      │ │
│  │  ├─ handleEditAmount(idx)                       │ │
│  │  ├─ handleEditTankVolume()                      │ │
│  │  └─ handleEditPumpDuration(idx)                 │ │
│  └──────────────────────────────────────────────────┘ │
│                                                        │
└────────────────────────────────────────────────────────┘
         │
         │ includes
         ▼
┌────────────────────────────────────────────────────────┐
│                   screens.cpp                          │
│  ├─ langConfigScreen()                               │
│  ├─ tankVolumeScreen()                               │
│  ├─ pumpAmountScreen()                               │
│  ├─ pumpDurationScreen()                             │
│  ├─ timeSetupScreen()                                │
│  ├─ editNumberScreen() (generic numeric input)       │
│  └─ showTime()                                        │
├────────────────────────────────────────────────────────┤
│ Libraries:                                             │
│ ├─ Keypad (keypad input)                             │
│ └─ LiquidCrystal_I2C (LCD display)                   │
└────────────────────────────────────────────────────────┘
         │
         │ uses
         ├─────────────────┬──────────────┬───────────┐
         ▼                 ▼              ▼           ▼
    ┌─────────┐      ┌─────────┐   ┌─────────┐  ┌────────┐
    │ display │      │language │   │  chars  │  │ water  │
    │ (LCD)   │      │(i18n)   │   │(glyphs) │  │(pumps) │
    └─────────┘      └─────────┘   └─────────┘  └────────┘
         │                │             │           │
    I2C  │                PROGMEM      CGRAM    I2C/GPIO
    0x27 │                storage      loading   0x77,0x78
         │                                           │
    ┌────▼───────────────────────────────────────────▼──┐
    │         Arduino I2C & GPIO Interface              │
    │  ├─ Wire library (I2C)                            │
    │  ├─ digitalWrite() (pump control)                 │
    │  └─ Serial (debugging)                            │
    └────────────────────────────────────────────────────┘
```

## State Machine - Main Application

```
                    ┌─────────────┐
                    │   STARTUP   │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │ Serial & I2C│
                    │  Init       │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │   Splash    │
                    │   Screen    │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │  Language   │
                    │ Selection   │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Tank Volume  │
                    │Config       │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Pump Amounts │
                    │Config (1-5) │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Time Setup   │
                    │             │
                    └──────┬──────┘
                           │
                           ▼
                    ┌─────────────┐
                    │Pump Durations
                    │Config (1-5) │
                    └──────┬──────┘
                           │
                           ▼
                    ╔═════════════╗
                    ║   MAIN      ║
                    ║   LOOP      ║
                    ║ ┌─────────┐ ║
                    ║ │Display  │ ║
                    ║ │Main UI  │ ║
                    ║ └────┬────┘ ║
                    ║      │      ║
                    ║  ┌───▼────┐ ║
                    ║  │ Listen  │ ║
                    ║  │ Keypad  │ ║
                    ║  └───┬────┘ ║
                    ║      │      ║
                    ║  ┌───▼────┐ ║
                    ║  │Dispatch│ ║
                    ║  │Handler │ ║
                    ║  └────────┘ ║
                    ║      ▲      ║
                    ║      │      ║
                    ║      └──────┘ (repeat)
                    ╚═════════════╝
```

## Numeric Input Screen Flow

```
┌─────────────────────────────────────────┐
│    Numeric Input Screen                 │
│  (Tank, Amount, Duration)               │
└──────────────┬──────────────────────────┘
               │
               ▼
         ┌──────────────┐
         │ Display Label│
         │ & Format     │
         └──────┬───────┘
                │
                ▼
         ┌──────────────┐
         │  View Mode   │
         │ (Read-Only)  │
         └──────┬───────┘
                │
         ┌──────┴────────┐
         │               │
    (Press #)       (Press *)
         │               │
         ▼               ▼
    ┌─────────┐   ┌──────────┐
    │Edit Mode│   │  Cancel  │
    │(Input)  │   │ Return -1│
    └────┬────┘   └──────────┘
         │
         ▼
    ┌──────────────────┐
    │ Blink Cursor     │
    │ 500ms period     │
    └────┬─────────────┘
         │
    ┌────┴──────────────────┬────────────┬─────────────┐
    │                       │            │             │
(Digit 0-9)         (Key #)        (Key *)         (Other)
    │                       │            │             │
    ▼                       ▼            ▼             │
 Append      ┌──────────────────┐  ┌────────┐        │
 Digit       │  Validate &      │  │Clear   │        │
 Update      │  Confirm Entry   │  │Number  │        │
 Display     │  Return Value    │  │Try Agn.│        │
    │        └────────────────┬─┘  └────────┘        │
    │                         │         │             │
    │                         ▼         ▼             │
    └────────────────────►(Exit)  (Edit Mode)        │
                                      │              │
                                      └──────────────┘
```

## Pump Data Structure

```
struct Pump {
    uint16_t amount           // 0-9999 ml
    uint64_t duration         // 0-999999 ms
    
    Methods:
    ├─ edit()                 // Enter edit mode
    ├─ viewEdit()             // View then optionally edit
    ├─ setAmount()
    ├─ getAmount()
    ├─ setDuration()
    └─ getDuration()
}

Array: Pump pumps[5]
       [0] Pump 1
       [1] Pump 2
       [2] Pump 3
       [3] Pump 4
       [4] Pump 5
```

## Language Support Structure

```
Language {
    char langName[16]           // "Polish   "
    char langPrompt[12]         // "Język  "
    char tankVolumeTitle[24]    // "Poj. zbiornika"
    char amountTitle[24]        // "Ilość płynu w #"
    char mainScreen[24]         // "Ekran główny"
    char noTask[24]             // "Brak zadania"
    char pumpWorking[16]        // "Pompa #"
}

Storage: const Language LANGUAGES[10] in PROGMEM
Index:
  0 - Polish
  1 - English
  2 - Russian
  3 - German
  4 - French
  5 - Spanish
  6 - Italian
  7 - Portuguese
  8 - Turkish
  9 - Czech
```

## LCD Display Layout

```
┌─────────────────────┐
│ 1234567890123456    │  Row 0 (20 chars)
│ 1234567890123456    │  Row 1 (20 chars)
└─────────────────────┘

Example - Tank Volume Screen:
┌─────────────────────┐
│ Poj. zbiornika      │
│ <-* _______l #->    │
└─────────────────────┘
         ▲       ▲
      Input     Back

Example - Main Screen:
┌─────────────────────┐
│ Ekran główny        │
│ Brak zadania        │
└─────────────────────┘
```

## Keypad Key Mapping

```
Physical Layout:        Function Mapping:
┌─────────────┐        ┌──────────────┐
│ 1 2 3 A     │        │ P1 P2 P3 PrV │
│ 4 5 6 B     │        │ P4 P5 P6 NtV │
│ 7 8 9 C     │        │ P7 P8 P9 H2O │
│ * 0 # D     │        │ Rst Tim TnkV │
└─────────────┘        └──────────────┘

P1-P5 = Pump 1-5 amount editing
H2O = Water level reading
Rst = Reset (factory defaults)
Tim = Time display
TnkV = Tank volume editing
NtV = Navigation (if supported)
PrV = Previous (if supported)
```

## Data Flow - Setup Phase

```
              ┌─────────────┐
              │ Serial.begin│
              │ Wire.begin()│
              └──────┬──────┘
                     │
    ┌─────────────────┼─────────────────┐
    │                 │                 │
    ▼                 ▼                 ▼
┌────────┐      ┌─────────┐      ┌──────────┐
│Splash  │      │Language │      │Tank Vol  │
│Screen  │      │ Select  │      │ Config   │
└────┬───┘      └────┬────┘      └────┬─────┘
     │               │               │
     └───────┬───────┴───────┬───────┘
             │               │
             ▼               ▼
         ┌────────────────────────┐
         │ Pump Amounts (1-5)     │
         │ for i in 0..PUMP_COUNT │
         └────────┬───────────────┘
                  │
                  ▼
         ┌──────────────────┐
         │ Time Setup       │
         │ HH:MM:SS         │
         └────────┬─────────┘
                  │
                  ▼
         ┌────────────────────────┐
         │ Pump Durations (1-5)   │
         │ for i in 0..PUMP_COUNT │
         └────────┬───────────────┘
                  │
                  ▼
         ┌──────────────────┐
         │ Enter Main Loop  │
         └──────────────────┘
```

## I2C Communication Map

```
Arduino Mega SDA/SCL (Pins 20/21)
        │
        │ 100kHz
        │
    ┌───┴────────┬──────────┐
    │            │          │
  0x27         0x77        0x78
  ┌────┐      ┌─────┐    ┌─────┐
  │LCD │      │Sensor    │Sensor
  │    │      │ Low │    │ High │
  └────┘      └─────┘    └─────┘
   • Init
   • Write text
   • Set cursor
   • Create chars
   
   12 bytes       8 bytes
   Read           Read
```

---

**Version 1.0 - January 2026**
