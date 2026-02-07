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

#define PUMP_COUNT 5

// Error codes for water management
enum WaterError {
  WATER_ERROR_NONE = 0,
  WATER_ERROR_SENSOR_TIMEOUT = 1,
  WATER_ERROR_SENSOR_COMMUNICATION = 2,
  WATER_ERROR_SENSOR_INVALID_DATA = 3,
  WATER_ERROR_PUMP_TIMEOUT = 4
};

// Safety limits
#define MAX_PUMP_RUN_TIME_MS 30000  // 30 seconds maximum pump runtime
#define SENSOR_READ_TIMEOUT_MS 1000  // 1 second timeout for sensor reads

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
 * Check water level and auto-control pumps
 * Called periodically from main loop to maintain water levels
 * Only active if pumps are in AUTO mode
 */
void checkWaterLevel();

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
void setLowThreshold(int16_t threshold);

/**
 * Set high threshold value
 * @param threshold High threshold in percentage (0-100)
 */
void setHighThreshold(int16_t threshold);

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
uint16_t calculatePumpDuration(uint8_t currentLevel, uint8_t threshold);

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


#endif
