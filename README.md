# Auto Aqua

Auto Aqua is Arduino Mega firmware for aquarium automation.

From the current source code, the project includes:

- water level sensing over I2C,
- automatic inlet/outlet control,
- configurable dosing pumps,
- LCD + keypad setup/edit screens,
- EEPROM-backed configuration storage,
- multi-language UI strings.

## Detailed specification

The full, code-derived specification is in:

- [`PROJECT_SPECIFICATION.md`](PROJECT_SPECIFICATION.md)

## What is in this repository

- `auto_aqua.ino` — top-level `setup()` / `loop()` flow.
- `appstate.*` — global runtime state container.
- `hardware.h` — pin map, I2C addresses, timing/safety constants.
- `storage.*` — EEPROM persistence and factory reset behavior.
- `water*.*` — sensor reads, water-level calculation, control/status helpers.
- `pumps.*` — pump model and periodic dosing scheduler.
- `screens*.cpp` + `display.*` + `language.h` — LCD/keypad UI and localization.
- `hardware_tests/` — standalone hardware sketches (I2C, LCD, keypad, EEPROM, pump toggle).

## Build / upload

If `arduino-cli` is installed locally:

```bash
arduino-cli compile --fqbn arduino:avr:mega .
arduino-cli upload --fqbn arduino:avr:mega -p <PORT> .
```

## Coding rules

Project coding rules are documented in:

- [`CODE_GUIDELINES.md`](CODE_GUIDELINES.md)


## Code quality tooling

- [`docs/quality.md`](docs/quality.md)
