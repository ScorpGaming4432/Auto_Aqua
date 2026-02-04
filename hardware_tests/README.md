Hardware tests for the auto_aqua project

This folder contains small, self-contained Arduino sketches useful for quickly
verifying hardware components on your Arduino Mega.

Available tests (each is a separate Arduino sketch folder):

- `i2c_scanner` - Scans the I2C bus and prints found addresses (useful to detect the touch sensors at 0x77/0x78).
- `pump_toggle` - Toggles two digital pins (38 and 39) intended for inlet/outlet pump relays. Use LEDs or a relay module to observe switching.
- `lcd_demo` - Initializes a 16x2 I2C LCD (0x27) and prints a message.
- `keypad_test` - Reads a 4x4 keypad using the same row/col pins from the project and prints pressed keys over Serial.
- `eeprom_test` - Writes and reads a short value from EEPROM for sanity checking.

Compile and upload (example using arduino-cli):

```bash
# Compile a test (from repo root)
arduino-cli compile --fqbn arduino:avr:mega hardware_tests/i2c_scanner

# Upload to Arduino on COM3 (replace COM3 with your port)
arduino-cli upload --fqbn arduino:avr:mega -p COM3 hardware_tests/i2c_scanner
```

Notes:
- Each test is a minimal sketch and does not depend on the rest of the project files.
- Adjust pins and I2C address in the tests if your wiring differs.
- Open the Serial Monitor at 9600 baud to view output.
