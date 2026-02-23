# Auto Aqua - Code Documentation

## Project Overview

**Auto Aqua** is an Arduino-based automatic aquarium water management system. It provides multi-language support for 10 languages with an intuitive LCD interface and keypad control. The system manages multiple pumps for precise liquid dispensing and monitors water levels via I2C sensors.

### Key Features

- **Multi-Language Support**: 10 languages (Polish, English, Russian, German, French, Spanish, Italian, Portuguese, Turkish, Czech)
- **LCD Display**: 16x2 I2C LCD with custom character support
- **Keypad Interface**: 4x4 matrix keypad for user input
- **Pump Management**: Control up to 5 pumps with configurable amounts and durations
- **Water Sensing**: I2C-based water level detection
- **Time Sync**: Built-in time management
- **Configurable Settings**: Tank volume, pump amounts, pump durations

---

## File Structure

### Main Files

#### `auto_aqua.ino` - Main Sketch

The entry point for the Arduino application. Contains:

- `Pump` struct: Encapsulates pump configuration (amount, duration)
- `AppState` namespace: Manages global application state
  - `languageIndex`: Currently selected language (0-9)
  - `pumps[]`: Array of 5 pump configurations
  - `tankVolume`: Tank capacity
  - `timeOffset`: Time synchronization offset

**Key Functions:**

- `setup()`: Initialization, hardware setup, and initial configuration
  - Displays splash screen
  - Language selection
  - Tank volume configuration
  - Pump amount setup
  - Time synchronization
  - Pump duration configuration

- `loop()`: Main application loop
  - Displays main screen
  - Handles keypad input
  - Dispatches to appropriate handlers

**Key Event Handlers:**

- `handleEditAmount(uint8_t idx)`: Edit pump amount
- `handleEditTankVolume()`: Edit tank capacity
- `handleEditPumpDuration(uint8_t idx)`: Configure pump runtime

### Display Management

#### `display.h` / `display.cpp`

Configures the 16x2 I2C LCD display.

- **Constants:**
  - `SCREEN_LOCATION`: I2C address (0x27)
  - `SCREEN_WIDTH`: 16 characters
  - `SCREEN_HEIGHT`: 2 lines
- **Objects:**
  - `lcd`: Global LiquidCrystal_I2C object
  - `dimTimer`: For display timeout/dimming

### User Interface

#### `screens.h` / `screens.cpp`

Implements all LCD screens and user input handling.

**Key Functions:**

- `splashScreen()`: Animated startup screen with water drop effect
- `langConfigScreen()`: Language selection interface
  - Keys A/B: Navigate languages
  - 0-9: Direct selection
  - #: Confirm
  - *: Cancel

- `editNumberScreen()`: Generic numeric input for tank, amounts, and duration
  - Supports up to 7 digits
  - Real-time cursor blinking
  - Multi-digit entry with backspace
  - Optional unit labels

- `tankVolumeScreen()`: Tank capacity configuration (liters)
- `pumpAmountScreen()`: Pump liquid amount setup (ml)
- `pumpDurationScreen()`: Pump runtime configuration (milliseconds)
- `timeSetupScreen()`: Time synchronization
  - Format: HH:MM:SS
  - Keys A/B: Navigate fields
  - 0-9: Enter digits
  - #: Confirm
  - *: Cancel

- `showTime()`: Display current time on LCD
- `seconds()`: Get elapsed seconds with millis() overflow handling
- `lcdPrintWithGlyphs()`: Print with custom character support

**Keypad Layout:**

```
1  2  3  A
4  5  6  B
7  8  9  C
*  0  #  D
```

**Key Mappings (Main Screen):**

- `1-5`: Edit pump 1-5 amounts
- `0`: Display current time
- `D`: Edit tank volume
- `C`: Read water level sensor
- `P`: Edit pump duration
- `*`: Factory reset (with confirmation)

### Language Support

#### `language.h`

Defines multi-language support infrastructure.

**Constants:**

- `LANG_COUNT`: 10 supported languages
- `PUMP_COUNT`: 5 pumps

**Language Structure:**

```cpp
struct Language {
  char langName[16];              // Language display name
  char langPrompt[12];            // Input prompt
  char tankVolumeTitle[24];       // Tank volume label
  char amountTitle[24];           // Pump amount label (contains #)
  char mainScreen[24];            // Main screen heading
  char noTask[24];                // "No task" message
  char pumpWorking[16];           // "Pump active" message
};
```

**Languages (0-9):**
0. Polski (Polish)

1. English
2. Русский (Russian)
3. Deutsch (German)
4. Français (French)
5. Español (Spanish)
6. Italiano (Italian)
7. Português (Portuguese)
8. Türkçe (Turkish)
9. Čeština (Czech)

**Functions:**

- `readLanguage()`: Load entire language structure
- `readLanguageField()`: Load single field (memory efficient)

### Custom Characters

#### `chars.h`

Manages custom LCD characters for language-specific glyphs.

**Features:**

- Drop animation frames for splash screen
- Language-specific glyphs for Polish, Russian, French, Spanish, Portuguese
- Glyph set mapping for all 10 languages

**Functions:**

- `makeRevealFrame()`: Create animation frame for reveal effect
- `animateIcon()`: Generate animation frames
- `loadCharSet()`: Load glyphs to LCD CGRAM
- `loadGlyphSet()`: Load language-specific characters

### Sensors & Pumps

#### `sensor.h` / `water.cpp`

Handles water level sensing and pump control.

**I2C Addresses:**

- `ADDR_HIGH`: 0x78 (high-level sensor)
- `ADDR_LOW`: 0x77 (low-level sensor)

**Functions:**

- `read_water_sensor()`: Get water level difference
- `readI2CData()`: Low-level I2C communication

#### `pumps.h` / `water.cpp`

Pump control interface.

**Constants:**

- `PRZEPLYW`: Flow rate (2000 ml/ms)

**Functions:**

- `pump_work()`: Activate pump for specified duration
  - `pump_pin`: Control pin (HIGH=off, LOW=on)
  - `duration_ms`: Runtime in milliseconds

### Storage

#### `storage.h` / `storage.cpp`

Data persistence (currently stub).

Planned features:

- EEPROM storage of configuration
- Tank volume persistence
- Pump settings
- Time offset

---

## Architecture & Design Patterns

### State Management

- `AppState` namespace encapsulates global application state
- Pump array for managing multiple pump configurations
- Language index for dynamic localization

### Screen Model

- **View Mode**: Display current value, # to enter edit
- **Edit Mode**: Modify values with real-time feedback
- **Numeric Input**: Generic editor for tank, amounts, durations

### Memory Optimization

- PROGMEM for all strings to save RAM
- Language field reading for targeted access vs. full structure loading
- Custom character caching to minimize I2C overhead

### User Interface

- Menu-driven configuration
- Real-time feedback with cursor blinking
- Multilingual error messages and prompts
- Contextual help (# and * key labels displayed)

---

## Typical Operation Flow

### Startup (setup())

1. Initialize serial and I2C
2. Display splash screen with animation
3. Language selection
4. Tank volume configuration
5. Pump amount setup for each pump (1-5)
6. Time synchronization
7. Pump duration configuration

### Runtime (loop())

1. Display main screen ("Main screen" / "No task")
2. Wait for keypad input
3. Route to appropriate handler:
   - Keys 1-5: Edit pump amounts
   - Key 0: Show time
   - Key D: Edit tank volume
   - Key C: Read water level
   - Key P: Edit pump duration
   - Key *: Factory reset confirmation
4. Return to main screen

### Input Editing

1. Show current value in view mode
2. User presses # to enter edit mode
3. Accept numeric input (0-9)
4. - to clear/backspace or cancel

5. `#` to confirm

6. Display returns to main screen

---

## Key Algorithms

### Numeric Input Processing

```
1. Initialize with current value
2. Display with cursor at last digit position
3. Wait for keypad input
4. On digit: append to number (left shift + digit)
5. On #: confirm if digits entered
6. On *: clear or cancel
7. Blink cursor every 500ms
```

### Language Switching

```
1. Load language glyphs to LCD
2. Read language name from PROGMEM
3. Display language
4. Wait for user selection (A/B to navigate, 0-9 direct, # to confirm)
5. Update global language index
```

### Water Level Sensing

```
1. Read high-level sensor (12 bytes)
2. Read low-level sensor (8 bytes)
3. Calculate difference
4. Return as water level indicator
```

---

## Keypad Architecture

### Hardware

- **Rows (input)**: Arduino pins 30, 32, 34, 36
- **Cols (output)**: Arduino pins 31, 33, 35, 37
- **Library**: Keypad (matrix scanning)

### Features

- Debouncing built into library
- Non-blocking input checking
- Custom keymap with symbols and letters

---

## Memory Considerations

### RAM Usage

- Language structures stored in PROGMEM (Flash)
- Minimal RAM buffer usage for display strings
- Dynamic string loading on-demand

### Flash Usage

- ~10 Language structures × 120 bytes each
- Custom glyphs for 6 languages
- Screen formatting strings

---

## Future Enhancement Opportunities

1. **Data Persistence**: Implement EEPROM storage
2. **Scheduling**: Add timed pump cycles
3. **Status Display**: Show pump status/progress
4. **Error Handling**: Handle sensor failures gracefully
5. **Calibration**: Water sensor calibration interface
6. **Pump Diagnostics**: Test pump functionality
7. **Settings Menu**: Access settings without restart
8. **Wireless**: Add Bluetooth/WiFi monitoring

---

## Debugging Tips

### Serial Output Format

- `[SETUP]`: Initialization phase
- `[LOOP]`: Main loop events
- `[AM]`: Pump amount editing
- `[TANK]`: Tank volume editing
- `[DUR]`: Pump duration editing
- `[TIME]`: Time synchronization
- `[LOOP]`: Main loop dispatch

### Common Issues

1. **LCD not displaying**: Check I2C address (0x27)
2. **Keypad not responding**: Verify pin configuration
3. **Language text garbled**: Glyph set not loaded for language
4. **Water sensor not reading**: Verify I2C addresses (0x77, 0x78)

---

## Code Style Notes

- Comprehensive documentation with Doxygen comments
- Namespaces for state and screen handlers
- Lambda functions for complex UI logic
- PROGMEM usage for string constants
- Inline overflow handling for millis()

---

Generated: January 2026
