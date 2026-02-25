/**
 * ============================================================================
 * WATER.H - Water Management Interface
 * ============================================================================
 *
 * Defines water level monitoring and pump control for the aquarium system.
 * Handles automatic water inlet/outlet pump management based on thresholds.
 *
 */

#ifndef WATER_H
#define WATER_H

#include <stdint.h>
#include "screens.h"

#include "hardware.h"

// Forward declaration so functions can reference the result type
struct WaterLevelResult;

// Error codes for water management
enum WaterError {
  WATER_ERROR_NONE = 0,
  WATER_ERROR_SENSOR_TIMEOUT = 1,
  WATER_ERROR_SENSOR_COMMUNICATION = 2,
  WATER_ERROR_SENSOR_INVALID_DATA = 3,
  WATER_ERROR_PUMP_TIMEOUT = 4
};

// Consolidated pump state structure (reduces global variables from 13 to 1)
struct WaterPumpState {
  // Pump runtime tracking
  uint32_t inletPumpTotalRuntime = 0;
  uint32_t outletPumpTotalRuntime = 0;
  unsigned long pumpStartTime = 0;
  unsigned long pumpDuration = 0;  // Added for non-blocking timing
  bool pumpActive = false;
  uint8_t activePumpPin = 0;

  // Error tracking
  WaterError currentError = WATER_ERROR_NONE;

  // Hysteresis state
  bool inletPumpWasActive = false;
  bool outletPumpWasActive = false;

  // Actual pump running state (for display purposes)
  bool inletPumpRunning = false;
  bool outletPumpRunning = false;

  // Electrovalve state
  // bool electrovalveActive = false;
  // uint32_t electrovalveTotalRuntime = 0;

  // Added for non-blocking automatic control
  bool autoControlActive = false;
};

// Encapsulates low/high touch sensor data and reading logic
class WaterSensor {
public:
  WaterSensor();
  WaterError readSensorData();
  uint8_t getTouchedSections();
  uint8_t calculateWaterLevel();
  uint32_t readWaterLevelRaw();
  void getCurrentWaterLevel(uint8_t *highBuf, uint8_t *lowBuf);
  WaterError getLastError() const;
  bool calibrateSensor(uint8_t sensorType, uint8_t *referenceData);
  bool isSensorConnected() const;

private:
  static bool initialized;
  uint8_t high_data[12];
  uint8_t low_data[8];
  WaterError lastError : 3;  // Use bitfield to save memory
  unsigned long lastSuccessfulRead;
  bool sensorConnected : 1;  // Use bitfield to save memory
};

/**
 * Initialize water management system
 * Sets up sensors and pump control pins
 */
void initWaterManagement();

/**
 * Update water management system (non-blocking)
 * Should be called frequently in the main loop
 */
void updateWaterManagement();

/**
 * Check water level and auto-control pumps
 * Called periodically from main loop to maintain water levels
 * Only active if pumps are in AUTO mode
 */
WaterLevelResult checkWaterLevel();

// Pump mode controls moved to pumps module

/**
 * Get current low threshold value
 * @return Low threshold in percentage (0-100)
 */
int16_t getLowThreshold();

/**
 * Get current high threshold value
 * @return High threshold in percentage (0-100)
 */
int16_t getHighThreshold();

/**
 * Set low threshold value
 * @param threshold Low threshold in percentage (0-100)
 */
void setLowThreshold(uint16_t threshold);

/**
 * Set high threshold value
 * @param threshold High threshold in percentage (0-100)
 */
void setHighThreshold(uint16_t threshold);

/**
 * Get current water level sensor readings (both low and high)
 * Populates provided buffers with raw sensor data
 * @param highBuf Buffer to store high sensor data (12 bytes)
 * @param lowBuf Buffer to store low sensor data (8 bytes)
 */
void getCurrentWaterLevel(uint8_t *highBuf, uint8_t *lowBuf);

/**
 * Read water sensor data (internal helper)
 * Performs I2C communication with water level sensors
 * @param highBuf Buffer for high sensor data (can be NULL)
 * @param lowBuf Buffer for low sensor data (can be NULL)
 */
void read_water_sensor(uint8_t *highBuf, uint8_t *lowBuf);

/**
 * Set both low and high water thresholds together
 * @param low Low threshold (0-100)
 * @param high High threshold (0-100)
 */
void setWaterThresholds(int16_t low, int16_t high);

/**
 * Get the last error code from water management system
 * @return WaterError code indicating the last error
 */
WaterError getWaterError();

/**
 * Clear water management error state
 */
void clearWaterError();

/**
 * Check if water sensors are responding properly
 * @return true if sensors are connected and responding
 */
bool checkSensorHealth();

/**
 * Emergency stop for let pumps
 * Immediately stops outlet and inlet pump activity
 */
void emergencyStopLetPumps();

/**
 * Calculate pump duration based on water level deviation from threshold
 * @param currentLevel Current water level percentage (0-100)
 * @param threshold Target threshold percentage (low or high)
 * @return Duration in milliseconds to run the pump
 */
uint16_t calculatePumpDuration(uint8_t currentLevel, uint8_t target);

/**
 * Get pump runtime statistics
 * @param inletRuntime Returns total inlet pump runtime in ms
 * @param outletRuntime Returns total outlet pump runtime in ms
 */
void getPumpStatistics(uint32_t *inletRuntime, uint32_t *outletRuntime);

/**
 * Reset pump runtime statistics
 */
void resetPumpStatistics();

/**
 * Calculate current water level percentage
 * @return Water level percentage (0-100)
 */
uint8_t calculateWaterLevel();

/**
 * Display water level status on LCD screen
 * @param result Water level check result containing level, error, and pump status
 */
void displayWaterLevelStatus(const WaterLevelResult &result);

/**
 * Control electrovalve (open/close)
 * @param open true to open valve, false to close
 */
// void controlElectrovalve(bool open);

/**
 * Get electrovalve status
 * @return true if electrovalve is open, false if closed
 */
// bool isElectrovalveOpen();

// /**
//  * Check if any pump is currently active
//  * @return true if pump is running, false otherwise
//  */
// extern bool pumpActive;

// Structure to hold water level check results
struct WaterLevelResult {
  WaterError error;
  uint8_t level;
  bool inletPumpActive;
  bool outletPumpActive;
};

#endif
