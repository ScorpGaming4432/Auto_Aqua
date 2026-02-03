# auto_aqua

**Automatic Aquarium Controller (Arduino)**

A complete, small-footprint firmware for automating aquarium water management, dosing, time display and multilingual UI on Arduino Mega-compatible boards.

---

## ✅ What this document specifies
This README now explicitly documents:
- Hardware and wiring (including pin assignments used by the firmware)
- Key mappings and UI behavior
- Pump roles and configuration (dosing vs. water inlet/outlet)
- Water level sensing behavior and thresholds
- EEPROM layout and "unset" sentinel values
- Build, upload & troubleshooting steps using `arduino-cli`
- How to configure, test and reset the system

---

## 📦 Features
- Multilanguage UI (10 languages) with persistent setting
- 16x2 I2C LCD display (LiquidCrystal_I2C)
- 4x4 matrix keypad for all user inputs
- Up to 5 pump channels: 3 dosing pumps + inlet + outlet
- I2C dual-level water sensors (high & low)
- Per-pump amount and duration settings for dosing pumps
- Automatic water level control (inlet/outlet) with thresholds
- EEPROM-backed persistent configuration and factory reset

---

## 🔧 Hardware & Wiring (explicit)
- Board: **Arduino Mega (or Mega2560)**
- LCD: I2C LiquidCrystal_I2C (commonly address `0x27`)
- Keypad: 4x4 matrix keypad (standard layout)
- Water level sensors (I2C):
  - High-level sensor address: `0x78` (defined as `ADDR_HIGH`)
  - Low-level sensor address:  `0x77` (defined as `ADDR_LOW`)
- Pumps:
  - **Dosing pumps (1..3)**: configurable amount & duration (wiring to chosen digital pins and/or relays)
  - **Inlet pump** (water fill): firmware example pin **D6** (`INLET_PUMP_PIN = 6`)
  - **Outlet pump** (water drain): firmware example pin **D7** (`OUTLET_PUMP_PIN = 7`)
  - Generic pump runner helper: `pump_work(pump_pin, duration_ms)` (in `water.cpp` / `pumps.h`)

Note: Dosing pump pins are not hard-coded in firmware; connect them to your relay/driver and call `pump_work()` or add pin variables as needed.

---

## ⌨️ Key mapping (current firmware)
Layout reference (4x4 keypad):
```
1 2 3 A
4 5 6 B
7 8 9 C
* 0 # D
```
Primary mappings implemented:
- **1–3**: Edit dosing pump amounts (ml)
- **A–C**: Edit dosing pump durations (ms)
- **4**: Toggle inlet pump mode (AUTO/MANUAL)
- **5**: Toggle outlet pump mode (AUTO/MANUAL)
- **D** (quick press): Edit tank volume (tap)
- **D** (hold 1s): Configure water thresholds (hold to enter threshold screen)
- **C**: Show measured water level
- **0**: Show current time
- **B**: Change language (brings up language selection screen)
- *** (star)**: Factory reset (confirm with `#`)

Behavioral notes:
- Dosing pumps (1–3) use persistent amount/duration stored in EEPROM
- Inlet/outlet pumps are modeled for automatic control; use **4/5** to change modes
- Holding D for 1 second opens the water threshold editor

---

## 💾 EEPROM layout and sentinel values
Addresses (in bytes):
- `EEPROM_ADDR_LANGUAGE_INDEX` = **0** (uint8_t). Value `0xFF` → treated as unset (language picker shows)
- `EEPROM_ADDR_TANK_VOLUME` = **1** (uint32_t). Value `0xFFFFFFFF` → unset (prompts tank config)
- `EEPROM_ADDR_TIME_OFFSET` = **5** (uint64_t). Value `0xFFFFFFFFFFFFFFFF` → unset
- `EEPROM_ADDR_PUMP_AMOUNTS` = **13** (each pump 2 bytes: uint16_t; `0xFFFF` -> unset)
- `EEPROM_ADDR_PUMP_DURATIONS` = **23** (each pump 8 bytes: uint64_t; `0xFFFFFFFFFFFFFFFF` -> unset)

Factory reset behavior (`factoryReset()` in `storage.cpp`):
- Sets language index to `0xFF`
- Sets tank volume to `0xFFFFFFFF`
- Sets time offset to `0xFFFFFFFFFFFFFFFF`
- Sets all pump amounts to `0xFFFF`
- Sets all pump durations to `0xFFFFFFFFFFFFFFFF`

Implementation notes:
- Unset values trigger configuration screens where applicable (e.g., tank volume, pump amounts/durations)

---

## 🌊 Water management details
- **Sensor addresses**: `ADDR_HIGH = 0x78`, `ADDR_LOW = 0x77`
- **Default thresholds** (scale is implementation-specific, default example):
  - `lowThreshold` = **100** (below this: inlet activates)
  - `highThreshold` = **900** (above this: outlet activates)
- **Automatic control**: `checkWaterLevel()` runs periodically (default every **5000 ms**) and toggles inlet/outlet pumps when in **AUTO** mode
- **Manual/Auto**: Use key `4` and `5` to toggle **inlet**/**outlet** between AUTO and MANUAL modes
- **Configuration**: Hold `D` for 1 second to open the threshold editor UI (enter digits, `#` toggles editing field, `A` saves, `*` cancels, `D` is backspace)
- **Pins**: Example pins used are **D6** for inlet and **D7** for outlet (modify as needed in `water.cpp`)

Safety tips:
- Add hardware float switches or mechanical stops for redundancy
- Add safety timers (e.g., max runtime) to avoid overfilling/emptying
- Ensure power and ground wiring are robust for pumps and relays

---

## 🧰 Build & upload (arduino-cli)
Examples used during development:
```bash
# Compile for Arduino Mega
arduino-cli compile --fqbn arduino:avr:mega .

# Upload to a board on COM7
arduino-cli upload --fqbn arduino:avr:mega --port COM7 .
```
Troubleshooting:
- If avrdude reports "can't open device" / "access denied":
  - Close Arduino IDE and any Serial Monitor that might hold the COM port
  - Unplug/replug USB cable or try another USB port
  - Use `arduino-cli board list` to confirm the device port
- Use `arduino-cli compile --verbose` to show compiler/linker output

---

## 🧪 Testing & calibration checklist
- Power up and confirm serial messages appear at `Serial.begin(9600)`
- Confirm I2C devices present (LCD, sensors) with `Wire.begin()` and see debug prints
- Test keypad inputs (watch Serial prints for each key press)
- Test dosing pump configuration and run a short pump test using `pump_work()`
- Validate water sensor readings (`C` key) and adjust thresholds with `D` hold
- Test factory reset (`*` then `#`) and verify EEPROM values are set to their unset sentinels

---

## ♻️ Extending and persisting thresholds (recommended next steps)
- Persist `lowThreshold` and `highThreshold` into EEPROM, similar to pump amounts and durations
- Add safety timers and flow sensors for accurate volume control
- Add web UI or serial command interface for remote monitoring

---

## 📁 Where to look in the code
- `auto_aqua.ino` — main application flow and key handling
- `water.cpp/.h` — water management, thresholds, inlet/outlet control
- `storage.cpp/.h` — EEPROM read/write and factory reset
- `screens.cpp/.h` — all UI screens (pump amount/duration, tank volume, threshold editor)
- `pumps.h` — pump struct and helpers
- `sensor.h` — I2C sensor addresses and helpers

---

## 📚 Related documentation (links)
- [QUICK_REFERENCE.md](QUICK_REFERENCE.md) — Fast access guide and hardware/pin constants (recommended start)
- [CODE_DOCUMENTATION.md](CODE_DOCUMENTATION.md) — Comprehensive code-level documentation and APIs
- [ARCHITECTURE_DIAGRAMS.md](ARCHITECTURE_DIAGRAMS.md) — Visual system diagrams and flowcharts
- [DOCUMENTATION_SUMMARY.md](DOCUMENTATION_SUMMARY.md) — Overview of what is documented and where to find it
- [DOCUMENTATION_COMPLETE.md](DOCUMENTATION_COMPLETE.md) — Consolidated documentation index and reading order
- [INDEX.md](INDEX.md) — Top-level index to all project documentation

---