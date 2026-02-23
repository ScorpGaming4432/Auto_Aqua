# Auto Aqua - Source-Derived Specification

This specification is written from the current project source files and `CODE_GUIDELINES.md`.
It intentionally avoids deriving requirements from historical PR text or other narrative docs.

## 1) Scope and platform

- Firmware target is Arduino Mega (`arduino:avr:mega`).
- Runtime model is Arduino `setup()` + `loop()`.
- UI path uses a 16x2 I2C LCD and a 4x4 keypad.
- Persistence is EEPROM-backed through a packed configuration struct.

## 2) Hardware mapping currently defined in code

From `hardware.h`:

- Inlet pump: pin `7`
- Outlet pump: pin `6`
- Electrovalve: pin `5`
- Dosing pumps: pins `2`, `3`, `4`
- Light control: pin `8`
- Keypad rows: `30`, `32`, `34`, `36`
- Keypad columns: `31`, `33`, `35`, `37`
- LCD I2C address: `0x27`
- Water sensor I2C addresses: low `0x77`, high `0x78`

Related constants:

- Serial baud `9600`
- LCD geometry `16x2`
- UI delays: `100 ms`, `1000 ms`, `2000 ms`
- Pump flow reference: `2 ml/s`
- Max pump runtime: `30000 ms`
- Sensor read timeout: `1000 ms`
- Touch threshold: `100`
- Hysteresis margin: `5%`

## 3) Runtime state model

`AppState` namespace defines global mutable state:

- `languageIndex`
- `pumps[Hardware::PUMP_COUNT]`
- `tankVolume`
- `timeOffset`
- `lowThreshold`
- `highThreshold`

Pump model (`Pump` + `DosingConfig`) contains:

- amount,
- duration,
- interval,
- last execution time,
- role (`DOSING`, `INLET`, `OUTLET`).

## 4) Startup sequence (from `auto_aqua.ino`)

1. Start serial and I2C.
2. Show splash screen.
3. Load persisted configuration.
4. Validate loaded config.
5. Load config into AppState.
6. If invalid, run first-time configuration flow:
   - language,
   - tank volume,
   - dosing amount per dosing pump,
   - dosing interval per dosing pump,
   - time setup,
   - thresholds,
   - light schedule,
   - save to EEPROM.
7. If valid, load language buffer from selected index.
8. Initialize water-management subsystem.

## 5) Main loop behavior (from `auto_aqua.ino`)

Each cycle:

1. Draw main status screen text.
2. Read keypad key.
3. Handle key actions:
   - `1..3`: edit dosing pump amount,
   - `A`: edit tank volume,
   - `C`: perform water-level measurement and display status,
   - `D`: edit water thresholds,
   - `0`: show current time,
   - `B`: language selection,
   - `*`: enter factory reset confirmation prompt.
4. Run water monitoring routine.
5. Run dosing schedule checks.
6. Apply short delay.

## 6) UI and input contracts

### Keypad layout

Declared layout:

```
1 2 3 A
4 5 6 B
7 8 9 C
* 0 # D
```

### Numeric editing behavior (`editNumberScreen`)

- `#` enters edit mode from view mode and confirms value in edit mode.
- `*` cancels when empty or clears current entry when digits exist.
- digits append until max length.
- overflow of configured max digits returns unset sentinel.
- blinking cursor is timer-based (`500 ms`).

### Time setup behavior (`timeSetupScreen`)

- six editable digits (`HHMMSS`).
- `A`/`B` move cursor.
- `#` confirms (with modulo correction for invalid HH/MM/SS ranges).
- `*` cancels to unset value.

## 7) Water sensing and control interfaces

### Sensor path

- Reads 8 bytes from low sensor and 12 bytes from high sensor over I2C.
- Fails on read timeout.
- flags invalid data when all sensor bytes are zero.
- water level is computed as `touched_sections * 5`.

### Control/status API surface

Code exposes:

- initialization (`initWaterManagement`),
- periodic check (`checkWaterLevel`),
- threshold getters/setters,
- threshold screen-driven update helper,
- status rendering on LCD,
- sensor health check helpers.

## 8) Persistence model

Configuration struct fields:

- language index,
- tank volume,
- time offset,
- pump amounts,
- pump durations,
- pump dosing intervals,
- low/high thresholds.

Behavior:

- full struct is serialized byte-wise to EEPROM starting at address `0`.
- validity checks reject unset magic values and invalid thresholds.
- on invalid config, defaults are loaded into AppState.
- factory reset writes unset magic values back to all fields.

## 9) Localization model

- `LANG_COUNT = 10` in `language.h`.
- language strings are stored in PROGMEM.
- runtime language buffer (`LANG_BUFFER`) is loaded via `readLanguage(index)`.

## 10) Coding constraints from `CODE_GUIDELINES.md`

The project’s declared rules include:

- function size and nesting limits,
- parameter-count limits,
- header hygiene and include discipline,
- modern C++ constraints (casts, raw memory guidance),
- Arduino-specific boundaries (especially `.ino` role and hardware leakage),
- complexity and formatting rules.

## 11) Hardware smoke tests included

`hardware_tests/` includes independent sketches for:

- I2C scanner,
- LCD demo,
- keypad test,
- EEPROM test,
- pump toggle behavior.

## 12) Notes for maintainers

This document intentionally describes what is present in source today.
Where comments and behavior diverge, implementation behavior in source files is treated as authoritative.
