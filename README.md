# auto_aqua

**Automatic Aquarium Controller (Arduino)**

A small-footprint firmware for automating aquarium water management, dosing, time display, and multilingual UI on Arduino Mega-compatible boards.

---

## ✅ What this document covers
- Hardware and wiring (including pin assignments used by the firmware)
- Key mappings and UI behavior implemented in code
- Pump roles and configuration (dosing vs. inlet/outlet)
- Water level sensing behavior and thresholds
- EEPROM layout and “unset” sentinel values
- Build, upload, troubleshooting, and first-run validation

---

## 📦 Features
- Multilanguage UI (10 languages) with persistent setting
- 16x2 I2C LCD display (`LiquidCrystal_I2C`)
- 4x4 matrix keypad for user input
- Up to 5 pump channels: 3 dosing pumps + inlet + outlet
- I2C dual-level water sensors (high & low)
- Per-pump amount + dosing interval settings for dosing pumps
- Automatic water-level control with inlet/outlet pumps
- EEPROM-backed persistent configuration and factory reset

---

## 🔧 Hardware & wiring
- Board: **Arduino Mega / Mega2560**
- LCD: I2C `LiquidCrystal_I2C` (commonly address `0x27`)
- Keypad: 4x4 matrix keypad
- Water sensors (I2C):
  - High-level sensor address: `0x78` (`ADDR_HIGH`)
  - Low-level sensor address: `0x77` (`ADDR_LOW`)
- Pumps:
  - **Dosing pumps (1..3)**: configurable amount + interval
  - **Inlet pump** (fill): example pin **D6** (`INLET_PUMP_PIN = 6`)
  - **Outlet pump** (drain): example pin **D7** (`OUTLET_PUMP_PIN = 7`)
  - Generic helper: `pump_work(pump_pin, duration_ms)`

> Note: Dosing pump output pins are project-configurable depending on your relay board.

---

## 🚀 Quick start (first boot)
1. Wire LCD, keypad, sensors, and pump relay drivers.
2. Compile + upload firmware.
3. Open serial monitor at **9600 baud**.
4. Complete initial setup on LCD:
   - choose language,
   - set tank volume,
   - set dosing amounts + intervals for pumps 1–3,
   - set time,
   - set water thresholds.
5. Verify main screen and test keys (`1`, `D`, `C`, `0`, `B`, `*`).

---

## ⌨️ Key mapping (current firmware)
Layout reference (4x4 keypad):
```
1 2 3 A
4 5 6 B
7 8 9 C
* 0 # D
```

Main-loop mappings currently active:
- **1–3**: Edit dosing pump amounts (ml)
- **D**: Configure low/high water thresholds
- **C**: Show measured water level + status
- **0**: Show current time
- **B**: Change language
- **\***: Factory reset (confirm with `#`)

Behavioral notes:
- Dosing pumps (1–3) use persistent amount/interval values in EEPROM.
- Inlet/outlet control is handled by periodic automatic checks.
- Some older comments/docs may mention planned keys that are not currently active.

---

## 💾 EEPROM layout and sentinel values
Addresses (bytes):
- `EEPROM_ADDR_LANGUAGE_INDEX` = **0** (`uint8_t`), unset: `0xFF`
- `EEPROM_ADDR_TANK_VOLUME` = **1** (`uint32_t`), unset: `0xFFFFFFFF`
- `EEPROM_ADDR_TIME_OFFSET` = **5** (`uint64_t`), unset: `0xFFFFFFFFFFFFFFFF`
- `EEPROM_ADDR_PUMP_AMOUNTS` = **13** (`uint16_t` each), unset: `0xFFFF`
- `EEPROM_ADDR_PUMP_DURATIONS` = **23** (`uint64_t` each), unset: `0xFFFFFFFFFFFFFFFF`

Factory reset (`factoryReset()`):
- language index → `0xFF`
- tank volume → `0xFFFFFFFF`
- time offset → `0xFFFFFFFFFFFFFFFF`
- all pump amounts → `0xFFFF`
- all pump intervals/durations → `0xFFFFFFFFFFFFFFFF`

---

## 🌊 Water management details
- Sensor addresses: `ADDR_HIGH = 0x78`, `ADDR_LOW = 0x77`
- Default threshold examples:
  - `lowThreshold = 100` (below this, fill/inlet behavior)
  - `highThreshold = 900` (above this, drain/outlet behavior)
- Automatic logic: `checkWaterLevel()` is called in the main loop to evaluate state and drive pumps/status.
- Threshold configuration UI: press `D`.
  - Digits: enter value
  - `#`: switch edited field
  - `A`: save
  - `*`: cancel
  - `D`: backspace

Safety recommendations:
- Add independent hardware float switches.
- Add max-runtime cutoff for inlet/outlet.
- Use proper relay driver + flyback protection + stable PSU.

---

## 🧰 Build & upload (`arduino-cli`)
```bash
# Compile for Arduino Mega
arduino-cli compile --fqbn arduino:avr:mega .

# Upload (example port)
arduino-cli upload --fqbn arduino:avr:mega --port COM7 .
```

Troubleshooting:
- If upload port is busy: close IDE/serial monitor and retry.
- Re-check board/port mapping via `arduino-cli board list`.
- Use `arduino-cli compile --verbose` for full compiler logs.

---

## 🧪 Verification checklist
- Serial starts at `9600` and setup logs appear.
- LCD + keypad input work.
- Language switch via `B` works.
- Dosing amount edit (`1`–`3`) persists after reboot.
- Water level view (`C`) shows sensible values.
- Threshold screen (`D`) can save/cancel correctly.
- Factory reset (`*` then `#`) restores sentinel values.

---

## ♻️ Recommended next improvements
- Persist threshold values explicitly in EEPROM (if not already included in your configuration object).
- Add explicit max-runtime safety cutoffs for inlet/outlet pumps.
- Add calibration workflow for dosing (ml/minute per pump).
- Add serial command mode for remote diagnostics.

---

## 📁 Code map
- `auto_aqua.ino` — application flow + keypad handlers
- `water.cpp/.h` — water management, thresholds, inlet/outlet control
- `storage.cpp/.h` — EEPROM read/write + factory reset
- `screens.cpp/.h` — UI screens (amount, interval, volume, thresholds)
- `pumps.h` — pump model + helpers
- `sensor.h` — I2C sensor constants/helpers

---

## 📚 Related documentation
- [QUICK_REFERENCE.md](QUICK_REFERENCE.md) — concise developer quick guide
- [CODE_DOCUMENTATION.md](CODE_DOCUMENTATION.md) — code-level technical docs
- [ARCHITECTURE_DIAGRAMS.md](ARCHITECTURE_DIAGRAMS.md) — architecture diagrams and flow
- [DOCUMENTATION_SUMMARY.md](DOCUMENTATION_SUMMARY.md) — what is documented and why
- [DOCUMENTATION_COMPLETE.md](DOCUMENTATION_COMPLETE.md) — consolidated documentation overview
- [INDEX.md](INDEX.md) — master index of docs

---

## 🤝 Documentation maintenance tips
- Keep this README aligned with actual key handling in `auto_aqua.ino`.
- Update `README.md` and `QUICK_REFERENCE.md` together when controls change.
- When EEPROM schema changes, update constants + sentinel table in the same commit.
