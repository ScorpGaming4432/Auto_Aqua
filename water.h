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

// Water level threshold modes
enum WaterMode {
  WATER_MODE_MANUAL = 0,
  WATER_MODE_AUTO = 1
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

/**
 * Toggle inlet pump between AUTO and MANUAL mode
 */
void toggleInletPumpMode();

/**
 * Toggle outlet pump between AUTO and MANUAL mode
 */
void toggleOutletPumpMode();

/**
 * Get current inlet pump mode
 * @return true if AUTO, false if MANUAL
 */
bool getInletPumpMode();

/**
 * Get current outlet pump mode
 * @return true if AUTO, false if MANUAL
 */
bool getOutletPumpMode();

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

#endif
