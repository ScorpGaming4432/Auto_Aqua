/**
 * ============================================================================
 * HARDWARE.H - Hardware Abstraction Layer
 * ============================================================================
 * 
 * Centralized hardware definitions for the aquarium system.
 * All pin assignments, hardware addresses, and hardware-specific constants
 * are defined here to eliminate hardware leakage across modules.
 */

#ifndef HARDWARE_H
#define HARDWARE_H

#include <stdint.h>

namespace Hardware {

// ============================================================================
// Pin Definitions
// ============================================================================

// Pump control pins
constexpr uint8_t INLET_PUMP_PIN = 7;
constexpr uint8_t OUTLET_PUMP_PIN = 6;
constexpr uint8_t ELECTROVALVE_PIN = 5;

// Dosing pump pins (pumps 0-2)
constexpr uint8_t DOSING_PUMP_PINS[] = {2, 3, 4};

// Light control pin
constexpr uint8_t LIGHT_PIN = 8;

// Keypad pins
constexpr uint8_t KEYPAD_ROW_PINS[] = {30, 32, 34, 36};
constexpr uint8_t KEYPAD_COL_PINS[] = {31, 33, 35, 37};

// ============================================================================
// Hardware Addresses
// ============================================================================

// LCD I2C address
constexpr uint8_t LCD_I2C_ADDRESS = 0x27;

// Water sensor I2C addresses
constexpr uint8_t WATER_SENSOR_HIGH_ADDR = 0x78;
constexpr uint8_t WATER_SENSOR_LOW_ADDR = 0x77;

// ============================================================================
// Hardware Constants
// ============================================================================

// LCD dimensions
constexpr uint8_t LCD_WIDTH = 16;
constexpr uint8_t LCD_HEIGHT = 2;

// Pump flow rate (ml/second)
constexpr uint8_t PUMP_FLOW_RATE_ML_PER_SEC = 2;

// Safety limits
constexpr uint16_t MAX_PUMP_RUN_TIME_MS = 30000;   // 30 seconds maximum pump runtime
constexpr uint16_t SENSOR_READ_TIMEOUT_MS = 1000;   // 1 second timeout for sensor reads

// Water level sensing constants
constexpr uint8_t NO_TOUCH_VALUE = 0xFE;
constexpr uint8_t TOUCH_THRESHOLD = 100;

// Hysteresis margin (percentage)
constexpr uint8_t HYSTERESIS_MARGIN_PERCENT = 5;

// ============================================================================
// Hardware Configuration
// ============================================================================

constexpr uint8_t PUMP_COUNT = 5;
constexpr uint8_t DOSING_PUMP_COUNT = 3;
constexpr uint8_t KEYPAD_ROWS = 4;
constexpr uint8_t KEYPAD_COLS = 4;

} // namespace Hardware

#endif // HARDWARE_H