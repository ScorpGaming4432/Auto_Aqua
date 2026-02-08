/**
 * ============================================================================
 * STORAGE.H - Data Storage Interface (EEPROM/Flash)
 * ============================================================================
 * 
 * Defines storage and retrieval functions for persistent configuration data.
 * Stores settings like tank volume, pump amounts, and other user configurations.
 */

#ifndef STORAGE_H
#define STORAGE_H

#include "appstate.h"
#include <stdint.h>
#include "water.h"

// Configuration structure that mirrors AppState
struct Configuration {
  uint8_t languageIndex;
  uint32_t tankVolume;
  int64_t timeOffset;
  uint16_t pumpAmounts[PUMP_COUNT-2];
  uint64_t pumpDurations[PUMP_COUNT-2];
  uint32_t pumpDosingIntervals[PUMP_COUNT-2]; // Dosing intervals in hours
  uint16_t lowThreshold;
  uint16_t highThreshold;
};

// Default configuration values
const Configuration DEFAULT_CONFIG = {
  .languageIndex = 0,
  .tankVolume = 0,
  .timeOffset = 0,
  .pumpAmounts = { 0, 0, 0},
  .pumpDurations = { 0, 0, 0},
  .pumpDosingIntervals = { 0, 0, 0},
  .lowThreshold = 0,
  .highThreshold = 0
};

// Magic values to indicate unset configuration
const uint8_t UNSET_U8 = 0xFF;
const uint16_t UNSET_U16 = 0xFFFF;
const uint32_t UNSET_U32 = 0xFFFFFFFF;
const uint64_t UNSET_U64 = 0xFFFFFFFFFFFFFFFF;
const int64_t UNSET_I64 = 0xFFFFFFFFFFFFFFFF;

/**
 * Save configuration to EEPROM
 * @param config Configuration struct to save
 */
void saveConfiguration(const Configuration& config);

/**
 * Load configuration from EEPROM
 * @return Configuration struct (uses magic values for unset fields)
 */
Configuration loadConfiguration();

/**
 * Check if configuration is valid (not using magic values)
 * @param config Configuration to check
 * @return True if configuration is valid
 */
bool isConfigurationValid(const Configuration& config);

/**
 * Load configuration and apply to AppState
 * Used during setup to restore previous settings
 */
void loadConfigurationToAppState();

/**
 * Save AppState to configuration
 * Used after any change to persist settings
 */
void saveAppStateToConfiguration();

/**
 * Factory reset - sets all EEPROM values to unset state
 */
void factoryReset();

#endif